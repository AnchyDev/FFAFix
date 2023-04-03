#ifndef MODULE_FFAFIX_H
#define MODULE_FFAFIX_H

#include "ScriptMgr.h"
#include "Player.h"

enum FFAFixConstants
{
    SPELL_AURA_IMMUNE = 29230
};

class FFAFixPlayerScript : PlayerScript
{
public:
    FFAFixPlayerScript() : PlayerScript("FFAFixPlayerScript") { }

private:
    void OnUpdateArea(Player* /*player*/, uint32 /*oldArea*/, uint32 /*newArea*/) override;
    bool HasAreaFlag(uint32 area, uint32 flag);
};

#endif // MODULE_FFAFIX_H
