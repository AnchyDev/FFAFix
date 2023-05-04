#ifndef MODULE_FFAFIX_H
#define MODULE_FFAFIX_H

#include "ScriptMgr.h"
#include "Player.h"

#include <map>

std::unordered_set<uint32> safeAreas;
bool IsSafeArea(uint32 areaId);

class FFAFixPlayerScript : PlayerScript
{
public:
    FFAFixPlayerScript() : PlayerScript("FFAFixPlayerScript") { }

private:
    void OnUpdate(Player* player, uint32 p_time) override;
    void OnUpdateArea(Player* player, uint32 oldArea, uint32 newArea) override;
    bool HasAreaFlag(uint32 area, uint32 flag);
    void UpdateFFAFlag(Player* player, bool state);
    void StopAttackers(Player* player);
    void AlertFFA(Player* player, bool state);
};

class FFAFixWorldScript : WorldScript
{
public:
    FFAFixWorldScript() : WorldScript("FFAFixWorldScript") { }

private:
    void OnAfterConfigLoad(bool reload) override;
};

#endif // MODULE_FFAFIX_H
