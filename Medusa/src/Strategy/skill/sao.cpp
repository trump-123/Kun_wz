#include "sao.h"
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

Csao::Csao():_state(Csao::NOTHING){
     ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH, "DEBUG/sao", true);
}


void Csao::plan(const CVisionModule* pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1)
    {_state = Csao::NOTHING;}
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);

    auto taskFlag = task().player.flag;
    auto taskmaxspeed = task().player.max_rot_speed;
    auto ball = pVision->ball().Pos();
    auto last_ball = pVision->rawBall().Pos();
    CGeoEllipse stand_ellipse(OUR_GOAL,PARAM::Field::PENALTY_AREA_DEPTH + PARAM::Field::MAX_PLAYER_SIZE,PARAM::Field::PENALTY_AREA_WIDTH/2+PARAM::Field::MAX_PLAYER_SIZE/2);
    CGeoLineEllipseIntersection stand_intersection(CGeoLine(ball,get_defence_direction(ball)),stand_ellipse);
    CGeoPoint stand_pos,stand_pos1;
    double d = PARAM::Math::PI;
    double dir1 = (self.Pos() - ball).dir();
    double dir2 = (CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0) - ball).dir();
    double dir3 = Utils::Normalize(dir2+d);

    TaskT newTask(task());

    CWorldModel time;
    int time1;
    time1 = time.InfraredOnCount(vecNumber);
    CGeoPoint stand_pos2;
    int flag = 0;
    auto vertical = Utils::Normalize((ball - self.Pos()).dir()+ d/2);
    auto vertical1= Utils::Normalize((ball - self.Pos()).dir()- d/2);
    if(time1 > 50)
    {
        CGeoPoint pos = ball + Utils::Polar2Vector(100,Utils::Normalize(self.Dir() + d));
        if((ball - last_ball).mod() > 70)
        {
            CWorldModel time1;
            int time2;
            time2 = time1.InfraredOnCount(vecNumber);
            CGeoPoint stand_pos2;
            if(time2>70)
            {
                KickStatus::Instance()->setKick(vecNumber,8000);
                flag = 1;
            }
            else
            {
                if(flag)
                {
                    stand_pos2 = self.Pos() + Utils::Polar2Vector(70,vertical);
                }
                else
                {
                    stand_pos2 = self.Pos() + Utils::Polar2Vector(70,vertical1);
                }
            }

        }
        else
        {
            stand_pos2 = pos;
        }
    }
    else
    {
        newTask.player.angle = Utils::Normalize(dir1 + d);
        CGeoPoint pos1 = ball + Utils::Polar2Vector(90,dir3);
        if((self.Pos() - pos1).mod() < 100)
        {
            DribbleStatus::Instance()->setDribbleCommand(vecNumber,3);
            newTask.player.pos =  ball + Utils::Polar2Vector(50,dir1);
            newTask.player.flag = PlayerStatus::DRIBBLE|PlayerStatus::DRIBBLING;
        }
        else
        {
            newTask.player.pos = pos1;
            newTask.player.flag = PlayerStatus::DODGE_BALL;
        }
    }

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

    //setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
    //setSubTask(PlayerRole::makeItSmartGoto(vecNumber,CGeoPoint(100,100),0,0));
    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
