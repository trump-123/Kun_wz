#include "naqiu.h"
#include "skill/Factory.h"
#include "TaskMediator.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "parammanager.h"
#include "KickStatus.h"
#include "WorldModel.h"
#include "math.h"
using namespace std;
namespace{
    bool DEBUG_SWITCH;
    const CGeoPoint OUR_GOAL(-PARAM::Field::PITCH_LENGTH/2,0);
    const CGeoPoint THEIR_GOAL(PARAM::Field::PITCH_LENGTH/2,200);
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
        if(pos.x() > MIN_X + padding && pos.x() < MAX_X - padding )
        {
           if(pos.y() > -175 && pos.y() < 175)
           {
                  return true;
           }
              else if(pow((pos.x() + PARAM::Field::PITCH_LENGTH/2),2)+pow((pos.y() - PARAM::Field::GOAL_WIDTH/2),2) < 805*805||pow((pos.x() + PARAM::Field::PITCH_LENGTH/2),2)+pow((pos.y() + PARAM::Field::GOAL_WIDTH/2),2) < 805*805)
              return true;
           else
               return false;
        }
        return false;
    }
}
Cnaqiu::Cnaqiu():_state(Cnaqiu::NOTHING){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH,"Debug/naqiu",false);
}
void Cnaqiu::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        _state = Cnaqiu::NOTHING;
    }
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);
    auto ball = pVision->ball().Pos();
    CWorldModel time;
    int time1;
    time1 = time.InfraredOnCount(vecNumber);
    double d = PARAM::Math::PI;
    double dir1 = (self.Pos() - ball).dir();
    double dir2 = (CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0) - ball).dir();
    double dir3 = Utils::Normalize(dir2+d);

    // SAVE
    auto ball_vel = pVision->ball().Vel().mod();
    auto ball_vel_dir = pVision->ball().Vel().dir();


    TaskT newTask(task());
    newTask.player.needkick=false;
    if(time1>10)
    {
        DribbleStatus::Instance()->setDribbleCommand(vecNumber, 3);
        if(fabs(self.Dir() - dir2) < 0.15)
            newTask.player.pos = self.Pos();
        else
        {
            newTask.player.angle = dir2;
            newTask.player.pos = self.Pos();
        }
    }
    else
    {
        if(fabs(ball.x())>2800||fabs(ball.y())>1800)
        {
            DribbleStatus::Instance()->setDribbleCommand(vecNumber, 3);
            newTask.player.pos = ball + Utils::Polar2Vector(50,dir1);
            newTask.player.angle = Utils::Normalize(dir1 + d);
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
    }
        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));

    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
