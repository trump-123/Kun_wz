#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "VisionModule.h"
#include "DribbleStatus.h"
#include "KickStatus.h"
#include "GDebugEngine.h"
#include "singleton.h"
#include "SkillUtils.h"
#include "WorldModel.h"
#include "GetBestUtils.h"
#include "messidecition.h"
#include "defence/DefenceInFo.h"
#include "defence/defencesequence.h"
#include "PointCalculation/guardpos.h"
#include "SkillUtils.h"

extern CVisionModule*  vision;
extern CKickStatus*    kickStatus;
extern CDribbleStatus* dribbleStatus;
extern CGDebugEngine*  debugEngine;
extern CWorldModel* world;
extern GetBestUtils * best;
extern CMessiDecision *messi;
extern CDefenceInfo *defenceInfo;
extern CDefenceSquence *defenceSquence;
extern CGuardPos *guardpos;
extern SkillUtils *skillutils;
void initializeSingleton();
#endif
