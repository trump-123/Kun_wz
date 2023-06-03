#include "Global.h"

CVisionModule*   vision;
CKickStatus*    kickStatus;
CDribbleStatus* dribbleStatus;
CGDebugEngine*  debugEngine;
CWorldModel* world;
GetBestUtils* best;
CMessiDecision* messi;
CDefenceInfo* defenceInfo;
CDefenceSquence* defenceSquence;
CGuardPos* guardpos;
SkillUtils* skillutils;
void initializeSingleton()
{
    vision         = VisionModule::Instance();
    kickStatus     = KickStatus::Instance();
    dribbleStatus  = DribbleStatus::Instance();
    debugEngine    = GDebugEngine::Instance();
    world          = WorldModel::Instance();
    best           = ZGetBestUtils::Instance();
    messi          = MessiDecision::Instance();
    defenceInfo    = DefenceInfo::Instance();
    defenceSquence = DefenceSquence::Instance();
    guardpos       = GuardPos::Instance();
    skillutils     = ZSkillUtils::instance();
}
