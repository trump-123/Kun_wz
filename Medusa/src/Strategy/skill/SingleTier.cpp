#include "SingleTier.h"
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

   double get_defence_direction(const CGeoPoint & pos){
       double leftPostToBallDir = (pos - LEFT_GOAL_POST).dir();
       double rightPostToBallDir = (pos - RIGHT_GOAL_POST).dir();
       if(DEBUG_SWITCH){
           GDebugEngine::Instance()->gui_debug_line(pos,LEFT_GOAL_POST,6);
           GDebugEngine::Instance()->gui_debug_line(pos,RIGHT_GOAL_POST,6);
       }
       return Utils::Normalize((leftPostToBallDir + rightPostToBallDir) / 2 + PARAM::Math::PI);
   }
}

CSingleTier::CSingleTier(){
     ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH, "DEBUG/Tier", false);
}
void CSingleTier::plan(const CVisionModule* pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
    }
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);
    const auto mouse = self.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,self.Dir());

    auto ball = pVision->ball();
    double ball_vel_dir = ball.Vel().dir();
    double d = PARAM::Math::PI;
    double TargetDir;

    CWorldModel p;
    int time = p.InfraredOnCount(vecNumber);
    bool need_clear = time>3 && (self.Pos() - ball.Pos()).mod() < 105 && abs((ball.Pos() - self.Pos()).dir() - self.Dir())<d/6;//清球
    CGeoPoint clear_pos = ball.Pos() + Utils::Polar2Vector(90,0);//清球点
    CGeoRectangle stand_rectangle(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH + PARAM::Field::MAX_PLAYER_SIZE/2+90,-PARAM::Field::PENALTY_AREA_WIDTH/2 - PARAM::Field::MAX_PLAYER_SIZE/2 - 90,-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PENALTY_AREA_WIDTH/2 + PARAM::Field::MAX_PLAYER_SIZE/2 + 90);
    CGeoLineRectangleIntersection stand_intersection(CGeoLine(ball.Pos(),get_defence_direction(ball.Pos())),stand_rectangle);
    CGeoPoint stand_pos,TargetPos;

    CGeoSegment ball_line(ball.Pos(),ball.Pos() + Utils::Polar2Vector(99999,ball_vel_dir));
    CGeoLineLineIntersection danger_intersection1(ball_line, CGeoLine(self.Pos(),ball_vel_dir + d/2));//求机器人最近防守点
    CGeoLineLineIntersection danger_intersection2(ball_line, CGeoLine(self.Pos(),ball_vel_dir - d/2));//同上
    CGeoLineLineIntersection danger_intersection(ball_line,GOAL_LINE);
    bool danger_to_our_goal = danger_intersection.Intersectant() && ball_line.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) && GOAL_LINE.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) && (ball_vel_dir - (danger_intersection.IntersectPoint() - ball.Pos()).dir()) < PARAM::Math::PI/18;


    if(stand_intersection.intersectant()){
        if(ball.Pos().dist2(stand_intersection.point1())<ball.Pos().dist2(stand_intersection.point2()))
            stand_pos = stand_intersection.point1() + Utils::Polar2Vector(PARAM::Field::MAX_PLAYER_SIZE/2+10,Utils::Normalize(get_defence_direction(ball.Pos())+d/2));
        else
            stand_pos = stand_intersection.point2() + Utils::Polar2Vector(PARAM::Field::MAX_PLAYER_SIZE/2+10,Utils::Normalize(get_defence_direction(ball.Pos())+d/2));
        if(stand_pos.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
            stand_pos.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
    }else{
        stand_pos=CGeoPoint(-PARAM::Field::PITCH_LENGTH+PARAM::Field::PENALTY_AREA_DEPTH+PARAM::Vehicle::V2::PLAYER_SIZE,0);
    }

    if(danger_to_our_goal&&ball.Vel().mod()>1000){//紧急防球
        DribbleStatus::Instance()->setDribbleCommand(vecNumber, 3);
        if(ball_vel_dir > d/2)
        {

            if(ball_vel_dir>d/6*5)
            {
                TargetPos = ball_line.projection(mouse) + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,self.Dir()+d);
                TargetDir = 0;
            }
            else
            {
                TargetDir = ball_vel_dir + PARAM::Math::PI;
                TargetPos = danger_intersection2.IntersectPoint();
            }
        }
        else
        {
            if(ball_vel_dir<-d/6*5)
            {
                TargetDir = 0;
                TargetPos = ball_line.projection(mouse) + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,self.Dir()+d);
            }
            else
            {
                TargetDir = ball_vel_dir + PARAM::Math::PI;
                TargetPos = danger_intersection1.IntersectPoint();
            }
        }
    }
    else if(need_clear)
    {
        DribbleStatus::Instance()->setDribbleCommand(vecNumber,3);
        TargetDir = 0;
        TargetPos = clear_pos;
        KickStatus::Instance()->setKick(vecNumber,6400);
    }
    else
    {
            TargetDir = (ball.Pos() - self.Pos()).dir();
            TargetPos = stand_pos;
    }
    int taskFlag;
    if((self.Pos() - TargetPos).mod() > 700 || self.X() > -PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH + 700)
    {
        taskFlag = PlayerStatus::ALLOW_DSS | PlayerStatus::DODGE_BALL;
    }
    else
    {
        if((self.Pos() - TargetPos).mod() > 210)
        {
            taskFlag = PlayerStatus::NOT_AVOID_OUR_VEHICLE;
        }
        else
        {
            taskFlag = PlayerStatus::ALLOW_DSS;
        }
    }

//    if((abs(self.X())>-PARAM::Field::PITCH_LENGTH/2+PARAM::Field::PENALTY_AREA_DEPTH-200)&&abs(self.Y())>(PARAM::Field::PENALTY_AREA_WIDTH/2-200))
//    {
//        taskFlag |= PlayerStatus::NOT_AVOID_PENALTY;
//    }

    TaskT newTask(task());
    newTask.player.pos = TargetPos;
    newTask.player.angle = TargetDir;
    newTask.player.max_acceleration = 5000;
    newTask.player.flag = taskFlag;

    setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
    //setSubTask(PlayerRole::makeItSmartGoto(vecNumber,CGeoPoint(100,100),0,0));
    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
