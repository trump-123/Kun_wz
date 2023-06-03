#include "Tire.h"
#include "skill/Factory.h"
#include "TaskMediator.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "parammanager.h"
#include "KickStatus.h"

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
       if(pos.x() > MIN_X + padding && pos.x() < MAX_X - padding && pos.y() > MIN_Y + padding && pos.y() < MAX_Y - padding)
           return true;
       return false;
   }
}

CTire::CTire():_state(CTire::NOTHING){
     ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH, "DEBUG/Goalie1", true);
}
void CTire::plan(const CVisionModule* pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1)
    {_state = CTire::NOTHING;}
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);

    auto ball = pVision->ball().Pos();
    auto player1 = pVision->ourPlayer(vecNumber).Pos();
    CGeoPoint clear_pos = ball + Utils::Polar2Vector(80,(OUR_GOAL - ball).dir());
    CGeoEllipse stand_ellipse(OUR_GOAL,PARAM::Field::PENALTY_AREA_DEPTH + PARAM::Field::MAX_PLAYER_SIZE,PARAM::Field::PENALTY_AREA_WIDTH/2);
    CGeoLineEllipseIntersection stand_intersection(CGeoLine(ball,get_defence_direction(ball)),stand_ellipse);
    CGeoPoint stand_pos,stand_pos1;

    if(stand_intersection.intersectant()){
        if(ball.dist2(stand_intersection.point1())<ball.dist2(stand_intersection.point2()))
            stand_pos = stand_intersection.point1();
        else
            stand_pos = stand_intersection.point2();
        if(stand_pos.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
            stand_pos.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
    }else{
        CGeoPoint(-PARAM::Field::PITCH_LENGTH+PARAM::Field::PENALTY_AREA_DEPTH+PARAM::Vehicle::V2::PLAYER_SIZE,PARAM::Field::MAX_PLAYER/2.0);
    }
//cout<<"0"<<endl;

    double stand_dir = (ball - stand_pos).dir();

    static double po5[6];
     int i=0;
     static int num1[6];
    for(int num=0;num<12;num++)
    { //获取敌方有效车号
            const PlayerVisionT& pos= pVision->theirPlayer(num);
            if(pos.Valid())
            {
                num1[i] =num;
                po5[i] =  (ball - pos.Pos()).mod();
               // cout<<i<<"     "<<po5[i]<<endl;
                i++;
            }
    }
//cout<<"1"<<endl;
    static  int n = 0;
    double q=99999;
     for(int i=0;i<6;i++)
     {
         if(po5[i]< q)
         {
              q = po5[i];
              n = num1[i];
         }
     }

    auto pi = PARAM::Math::PI;
    const PlayerVisionT& pos1= pVision->theirPlayer(n);
    CGeoPoint p = pos1.Pos();
    auto dir = pos1.Dir();
    auto d = Utils::Normalize(dir+pi/2.0);
    auto ang= Utils::Normalize((ball-self.Pos()).dir());
    CGeoPoint pos2 = stand_pos + Utils::Polar2Vector(PARAM::Field::MAX_PLAYER_SIZE/2+1,ang);

    CGeoEllipse stand_ellipse1(OUR_GOAL,PARAM::Field::PENALTY_AREA_DEPTH + PARAM::Field::MAX_PLAYER_SIZE,PARAM::Field::PENALTY_AREA_WIDTH/2);
    CGeoLineEllipseIntersection stand_intersection1(CGeoLine(p,d),stand_ellipse1);

    if(stand_intersection1.intersectant()){
        if(p.dist2(stand_intersection1.point1())<p.dist2(stand_intersection1.point2()))
            stand_pos1 = stand_intersection1.point1();
        else
            stand_pos1 = stand_intersection1.point2();
        if(stand_pos1.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
            stand_pos1.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
    }else{
        stand_pos1 = CGeoPoint(-PARAM::Field::PITCH_LENGTH+PARAM::Field::PENALTY_AREA_DEPTH+PARAM::Vehicle::V2::PLAYER_SIZE,PARAM::Field::MAX_PLAYER/2.0);
    }

    _state = STAND;
    TaskT newTask(task());

    double dir1 = Utils::Normalize((LEFT_GOAL_POST - pos1.Pos()).dir());
    double dir2 = Utils::Normalize((RIGHT_GOAL_POST - pos1.Pos()).dir());
    double stand_dir1 = (pos1.Pos() - self.Pos()).dir();

    /*if(pos1.Y()>RIGHT_GOAL_POST.y()||pos1.Y()<LEFT_GOAL_POST.y())
    {
        if(dir>dir2 && dir<dir1)
        {
           // cout<<"dir="<<x<<endl;
            newTask.player.pos = stand_pos1;
            newTask.player.angle = stand_dir1;
        }
        else{
            newTask.player.pos = pos2;
            newTask.player.angle = stand_dir;
        }
    }
    else
    {
        if(dir>dir2 || dir<dir1)
        {
           // cout<<"dir="<<theirdir<<endl;
            newTask.player.pos = stand_pos1;
            newTask.player.angle = stand_dir1;
        }
        else{
            newTask.player.pos = pos2;
            newTask.player.angle = stand_dir;
        }

    }*/
newTask.player.pos = CGeoPoint(100,0);






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
            GDebugEngine::Instance()->gui_debug_x(clear_pos,0);
            GDebugEngine::Instance()->gui_debug_msg(clear_pos,"cpos",0);
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
