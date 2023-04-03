#include "FFAFix.h"
#include "Config.h"

bool FFAFixPlayerScript::HasAreaFlag(uint32 area, uint32 flag)
{
    if (auto areaTable = sAreaTableStore.LookupEntry(area))
    {
        return (areaTable->flags & flag) == flag;
    }

    return false;
}

void FFAFixPlayerScript::OnUpdateArea(Player* player, uint32 /*oldArea*/, uint32 newArea)
{
    if (!sConfigMgr->GetOption<bool>("FFAFix.Enable", false))
    {
        // Cleanup any auras.
        if (player->HasAura(SPELL_AURA_IMMUNE))
        {
            player->RemoveAura(SPELL_AURA_IMMUNE);
        }

        return;
    }

    if (HasAreaFlag(newArea, AREA_FLAG_CITY) ||
        HasAreaFlag(newArea, AREA_FLAG_CAPITAL) ||
        HasAreaFlag(newArea, AREA_FLAG_SANCTUARY))
    {
        if (!player->HasAura(SPELL_AURA_IMMUNE))
        {
            player->AddAura(SPELL_AURA_IMMUNE, player);
        }
    }
    else
    {
        if (player->HasAura(SPELL_AURA_IMMUNE))
        {
            player->RemoveAura(SPELL_AURA_IMMUNE);
        }
    }
}

void SC_AddFFAFixScripts()
{
    new FFAFixPlayerScript();
}
