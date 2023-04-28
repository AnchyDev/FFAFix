#include "FFAFix.h"

#include "Config.h"
#include "Player.h"

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

bool FFAFixPlayerScript::HasAreaFlag(uint32 area, uint32 flag)
{
    if (auto areaTable = sAreaTableStore.LookupEntry(area))
    {
        return (areaTable->flags & flag) == flag;
    }

    return false;
}

void FFAFixPlayerScript::OnUpdate(Player* player, uint32 p_time)
{
    if (!sConfigMgr->GetOption<bool>("FFAFix.Enable", false))
    {
        return;
    }

    if (!player)
    {
        return;
    }

    auto newArea = player->GetAreaId();

    if (HasAreaFlag(newArea, AREA_FLAG_CITY) ||
        HasAreaFlag(newArea, AREA_FLAG_CAPITAL) ||
        HasAreaFlag(newArea, AREA_FLAG_SANCTUARY))
    {
        UpdateFFAFlag(player, false);
        StopAttackers(player);
    }
    else
    {
        UpdateFFAFlag(player, true);
    }
}

void SC_AddFFAFixScripts()
{
    new FFAFixPlayerScript();
}
