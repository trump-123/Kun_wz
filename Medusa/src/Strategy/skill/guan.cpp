#include "guan.h"
#include "skill/Factory.h"
#include "TaskMediator.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "parammanager.h"
#include "KickStatus.h"
#include "game_state.h"
#include "RefereeBoxIf.h"

namespace  {
   bool DEBUG_SWITCH;
   const CGeoPoint OUR_GOAL(-PARAM::Field::PITCH_LENGTH/2,0);
   const CGeoPoint LEFT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2 - 2);
   const CGeoPoint RIGHT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2 + 2);
   const CGeoSegment GOAL_LINE(LEFT_GOAL_POST,RIGHT_GOAL_POST);
   const static int MIN_X = -PARAM::Field::PITCH_LENGTH/2;
   const static int MAX_X = -PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH;
   const static int MIN_Y = -PARAM::Field::PENALTY_AREA_WIDTH/2;
   const static int MAX_Y = PARAM::Field::PENALTY_AREA_WIDTH/2;

   double get_defence_direction(const CGeoPoint & pos){
       double leftPostToBallDir = (pos - LEFT_GOAL_POST).dir();
       double rightPostToBallDir = (pos - RIGHT_GOAL_POST).dir();
       if(DEBUG_SWITCH){
           GDebugEngine::Instance()->gui_debug_line(pos,LEFT_GOAL_POST,6);
           GDebugEngine::Instance()->gui_debug_line(pos,RIGHT_GOAL_POST,6);
       }
       return Utils::Normalize((leftPostToBallDir + rightPostToBallDir) / 2 + PARAM::Math::PI);
   }
   bool in_our_penalty(const CGeoPoint& pos,double padding){
       if(pos.x() > MIN_X + padding && pos.x() < MAX_X - padding + PARAM::Field::MAX_PLAYER_SIZE/2)
       {
          if(pos.y() > -175 && pos.y() < 175)
          {
                 return true;
          }
           else if(pow((pos.x() + PARAM::Field::PITCH_LENGTH/2),2)+pow((pos.y() - PARAM::Field::GOAL_WIDTH/2),2) < 900*900||pow((pos.x() + PARAM::Field::PITCH_LENGTH/2),2)+pow((pos.y() + PARAM::Field::GOAL_WIDTH/2),2) < 900*900)
           return true;
           else
           return false;
       }
       return false;
   }
}

Cguan::Cguan():_state(Cguan::NOTHING){
     ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH, "DEBUG/Tier", true);
}
void Cguan::plan(const CVisionModule* pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1)
    {_state = Cguan::NOTHING;}
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);
    TaskT newTask(task());

    newTask.player.needkick = false;
    newTask.player.pos = self.Pos();

    if(DEBUG_SWITCH){
        switch(_state){
        case NOTHING:
            debug_state = "NOTHING";
            break;
        case STAND:
            debug_state = "NORMAL STAND";
            break;
        case CLEAR:
            debug_state = "CLEAR BALL";
            break;
        case SAVE:
            debug_state = "SAVE";
            break;
        default:
            debug_state = "ERROR STATE";
        }
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2),debug_state.c_str());
    }

    setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
    //setSubTask(PlayerRole::makeItSmartGoto(vecNumber,CGeoPoint(100,100),0,0));
    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
