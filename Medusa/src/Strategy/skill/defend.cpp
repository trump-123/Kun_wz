#include "defend.h"
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

Cdefend::Cdefend():_state(Cdefend::NOTHING){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH,"Debug/naqiu",false);
}
void Cdefend::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        _state = Cdefend::NOTHING;
    }
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);
    auto ball = pVision->ball().Pos();


    // SAVE
    auto ball_vel = pVision->ball().Vel().mod();
    auto ball_vel_dir = pVision->ball().Vel().dir();
    double d = PARAM::Math::PI;

    TaskT newTask(task());




        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));



    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}

