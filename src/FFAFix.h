#ifndef MODULE_FFAFIX_H
#define MODULE_FFAFIX_H

#include "ScriptMgr.h"
#include "Player.h"

class FFAFixPlayerScript : PlayerScript
{
public:
    FFAFixPlayerScript() : PlayerScript("FFAFixPlayerScript") { }

private:
    void OnUpdate(Player* player, uint32 p_time) override;
    bool HasAreaFlag(uint32 area, uint32 flag);
    void UpdateFFAFlag(Player* player, bool state);
    void StopAttackers(Player* player);
};

#endif // MODULE_FFAFIX_H
