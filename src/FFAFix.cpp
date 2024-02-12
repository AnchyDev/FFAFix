#include "FFAFix.h"

#include "Chat.h"
#include "Config.h"
#include "Player.h"

bool IsInSafeArea(Player* player)
{
    auto map = player->GetMap();
    if (!map)
    {
        return true;
    }

    if (map->IsDungeon() ||
        map->IsRaid())
    {
        if (sConfigMgr->GetOption<bool>("FFAFix.Protect.Dungeons", true))
        {
            return !player->IsCharmed() && !player->isPossessed();
        }

        if (sConfigMgr->GetOption<bool>("FFAFix.Protect.Raids", true))
        {
            return !player->IsCharmed() && !player->isPossessed();
        }
    }

    return IsSafeArea(player->GetAreaId());
}

bool IsSafeArea(uint32 areaId)
{
    return safeAreas.find(areaId) != safeAreas.end();
}

void FFAFixPlayerScript::UpdateFFAFlag(Player* player, bool state)
{
    if (!player->HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP) && state)
    {
        player->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);

        for (auto it = player->m_Controlled.begin(); it != player->m_Controlled.end(); ++it)
        {
            (*it)->SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);
        }
    }
    else if(player->HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP) && !state)
    {
        player->RemoveByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);

        for (auto it = player->m_Controlled.begin(); it != player->m_Controlled.end(); ++it)
        {
            (*it)->RemoveByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);
        }
    }
}

void FFAFixPlayerScript::StopAttackers(Player* player)
{
    std::unordered_set<Unit*> toRemove;
    auto attackers = player->getAttackers();
    for (auto it = attackers.begin(); it != attackers.end(); ++it)
    {
        if (!(*it)->IsValidAttackTarget(player))
        {
            toRemove.insert(*it);
        }
    }

    for (auto it = toRemove.begin(); it != toRemove.end(); ++it)
    {
        (*it)->AttackStop();
    }

    if (Unit* victim = player->GetVictim())
    {
        if (!player->IsValidAttackTarget(victim))
        {
            player->AttackStop();
        }
    }
}

void FFAFixPlayerScript::AlertFFA(Player* player, bool state)
{
    if (!player)
    {
        return;
    }

    if (!sConfigMgr->GetOption<bool>("FFAFix.Alert.Change", true))
    {
        return;
    }

    std::string message = sConfigMgr->GetOption<std::string>(state ? "FFAFix.Alert.Change.FFA" : "FFAFix.Alert.Change.Safe", "");

    if (message.size() < 1)
    {
        return;
    }

    WorldPacket data(SMSG_NOTIFICATION, (message.size() + 1));
    data << message;

    player->SendDirectMessage(&data);
}

bool FFAFixPlayerScript::HasAreaFlag(uint32 area, uint32 flag)
{
    if (auto areaTable = sAreaTableStore.LookupEntry(area))
    {
        return (areaTable->flags & flag) == flag;
    }

    return false;
}

void FFAFixPlayerScript::OnUpdate(Player* player, uint32 /*p_time*/)
{
    if (!sConfigMgr->GetOption<bool>("FFAFix.Enable", false))
    {
        return;
    }

    if (!player)
    {
        return;
    }

    if (IsInSafeArea(player))
    {
        UpdateFFAFlag(player, false);
        StopAttackers(player);
    }
    else
    {
        UpdateFFAFlag(player, true);
    }
}

void FFAFixPlayerScript::OnUpdateArea(Player* player, uint32 oldArea, uint32 newArea)
{
    if (!player)
    {
        return;
    }

    if (IsSafeArea(oldArea) && !IsSafeArea(newArea))
    {
        AlertFFA(player, true);
    }
    else if (!IsSafeArea(oldArea) && IsSafeArea(newArea))
    {
        AlertFFA(player, false);
    }
}

void FFAFixWorldScript::OnAfterConfigLoad(bool reload)
{
    if (reload)
    {
        safeAreas.clear();
    }

    QueryResult qResult = WorldDatabase.Query("SELECT areaid FROM `ffafix_safe_areas`");

    if (qResult)
    {
        uint32 count = 0;

        do
        {
            Field* fields = qResult->Fetch();

            uint32 id = fields[0].Get<int32>();
            safeAreas.insert(id);

            count++;
        } while (qResult->NextRow());

        LOG_INFO("module", "Loaded '{}' rows from 'ffafix_safe_areas' table.", count);
    }
    else
    {
        LOG_INFO("module", "Loaded '0' rows from 'ffafix_safe_areas' table.");
    }
}

void SC_AddFFAFixScripts()
{
    new FFAFixWorldScript();
    new FFAFixPlayerScript();
}
