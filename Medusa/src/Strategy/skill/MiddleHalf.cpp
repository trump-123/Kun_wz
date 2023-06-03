#include "MiddleHalf.h"
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

   bool theirtendourgoal(double dir,const CGeoPoint&pos)
   {
        double leftdir = (LEFT_GOAL_POST - pos).dir();
        double rightdir= (RIGHT_GOAL_POST - pos).dir();
        if(pos.y()>LEFT_GOAL_POST.y()&&pos.y()<RIGHT_GOAL_POST.y())
        {
            if(dir>rightdir||dir<leftdir)
                return true;
            else
                return false;
        }
        else
        {
            if(dir>rightdir&&dir<leftdir)
                return true;
            else
                return false;
        }
   }
}

CMiddleHalf::CMiddleHalf(){
     ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH, "DEBUG/MiddleHalf", false);
}
void CMiddleHalf::plan(const CVisionModule* pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
    }
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);
    const auto mouse = self.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,self.Dir());

    auto taskFlag = task().player.flag;
    auto ball = pVision->ball();
    CGeoRectangle stand_rectangle(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH + PARAM::Field::MAX_PLAYER_SIZE/2,-PARAM::Field::PENALTY_AREA_WIDTH/2 - PARAM::Field::MAX_PLAYER_SIZE/2,-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PENALTY_AREA_WIDTH/2 + PARAM::Field::MAX_PLAYER_SIZE/2);
    CGeoPoint stand_pos,stand_pos1;

    CGeoPoint TargetPos;
    double TargetDir;
    int uptotal = 0,upattack[6],assist = 0,midattack[6],downtotal = 0,downattack[6];
    double updist[6],middist[6],downdist[6];

    for(int num = 0;num<PARAM::Field::MAX_PLAYER-1;num++)
    {
        const PlayerVisionT &enemy = pVision->theirPlayer(num);
        if(enemy.Valid())
        {
            if(enemy.X()<0)
            {
                if(enemy.Y()<-PARAM::Field::PENALTY_AREA_WIDTH/2)
                {
                    downdist[downtotal] = (enemy.Pos() - OUR_GOAL).mod();
                    downattack[downtotal] = num;
                    downtotal++;
                }
                else if(enemy.Y()>=-PARAM::Field::PENALTY_AREA_WIDTH/2&&enemy.Y()<=PARAM::Field::PENALTY_AREA_WIDTH/2)
                {
                    midattack[assist] = num;
                    middist[assist] = (enemy.Pos() - OUR_GOAL).mod();
                    assist++;
                }
                else
                {
                    updist[uptotal] = (enemy.Pos() - OUR_GOAL).mod();
                    upattack[uptotal] = num;
                    uptotal++;
                }
            }
        }
    }
    int real = -1,j = downattack[0],i = midattack[0],k = upattack[0];
    double m = downdist[0],m1 = middist[0],m2 = updist[0];
    double ball_vel_dir = ball.Vel().dir();
    double d = PARAM::Math::PI;

    CWorldModel p;
    int time = p.InfraredOnCount(vecNumber);
    bool need_clear = time>3;//清球
    CGeoPoint clear_pos = ball.Pos() + Utils::Polar2Vector(90,0);//清球点

    CGeoSegment ball_line(ball.Pos(),ball.Pos() + Utils::Polar2Vector(99999,ball_vel_dir));
    CGeoLineLineIntersection danger_intersection1(ball_line, CGeoLine(self.Pos(),ball_vel_dir + d/2));//求机器人最近防守点
    CGeoLineLineIntersection danger_intersection2(ball_line, CGeoLine(self.Pos(),ball_vel_dir - d/2));//同上
    CGeoLineLineIntersection danger_intersection(ball_line,GOAL_LINE);
    bool danger_to_our_goal = danger_intersection.Intersectant() && ball_line.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) && GOAL_LINE.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) && (ball_vel_dir - (danger_intersection.IntersectPoint() - ball.Pos()).dir()) < PARAM::Math::PI/18;

    if(danger_to_our_goal&& ball.Vel().mod() > 1500){//紧急防球
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
        if(assist>0||uptotal>1||downtotal>1)
        {
            if(assist>0)
            {
                if(assist==1)
                {
                    real = midattack[0];
                }
                else
                {
                    for(int num = 1;num < assist;num++)
                    {
                        if(middist[num]<m1)
                        {
                            m1 = middist[num];
                            i = midattack[num];
                        }
                    }
                    real = i;
                }
            }
            else
            {
                if(uptotal>=downtotal)
                {
                    for(int num = 1;num < uptotal;num++)
                    {
                        if(updist[num]>m2)
                        {
                            m2 = updist[num];
                            k = upattack[num];
                        }
                    }
                    real = k;
                }
                else
                {
                    for(int num = 1;num < downtotal;num++)
                    {
                        if(downdist[num]>m)
                        {
                            m = downdist[num];
                            j = downattack[num];
                        }
                    }
                    real = j;
                }
            }
            //cout<<real<<endl;
            const PlayerVisionT&enemy = pVision->theirPlayer(real);
                if(theirtendourgoal(enemy.Dir(),enemy.Pos()))
                {
                    CGeoLineRectangleIntersection stand_intersection(CGeoLine(enemy.Pos(),enemy.Dir()),stand_rectangle);
                    if(stand_intersection.intersectant())
                    {
                      if(enemy.Pos().dist2(stand_intersection.point1())<enemy.Pos().dist2(stand_intersection.point2()))
                          stand_pos = stand_intersection.point1();
                      else
                          stand_pos = stand_intersection.point2();
                      if(stand_pos.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
                          stand_pos.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
                    }
                    else
                    {
                        stand_pos = CGeoPoint(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::MAX_PLAYER_SIZE/2 + PARAM::Field::PENALTY_AREA_DEPTH,0);
                    }
                    TargetDir = (enemy.Pos() - self.Pos()).dir();
                    TargetPos = stand_pos;
                }
                else
                {
                    CGeoLineRectangleIntersection stand_intersection(CGeoLine(enemy.Pos(),OUR_GOAL),stand_rectangle);
                    if(stand_intersection.intersectant())
                    {
                      if(enemy.Pos().dist2(stand_intersection.point1())<enemy.Pos().dist2(stand_intersection.point2()))
                          stand_pos = stand_intersection.point1();
                      else
                          stand_pos = stand_intersection.point2();
                      if(stand_pos.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
                          stand_pos.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
                    }
                    else
                    {
                        stand_pos = CGeoPoint(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::MAX_PLAYER_SIZE/2 + PARAM::Field::PENALTY_AREA_DEPTH,0);
                    }
                }
                TargetDir = (enemy.Pos() - self.Pos()).dir();
                TargetPos = stand_pos;
        }
        else
        {
                TargetDir = (self.Pos() - OUR_GOAL).dir();
                TargetPos = CGeoPoint(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::MAX_PLAYER_SIZE/2 + PARAM::Field::PENALTY_AREA_DEPTH,0);
        }
    }


    if((self.Pos() - TargetPos).mod() > PARAM::Vehicle::V2::PLAYER_SIZE*1)
    {
        taskFlag = PlayerStatus::ALLOW_DSS;
    }
    else
    {
        taskFlag = PlayerStatus::NOT_AVOID_OUR_VEHICLE;
    }

    if((self.Pos() - TargetPos).mod() > 700 || self.X() > -PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH + 700)
    {
       taskFlag = PlayerStatus::ALLOW_DSS + PlayerStatus::DODGE_BALL;
    }

    if((self.X()>-PARAM::Field::PITCH_LENGTH/2+PARAM::Field::PENALTY_AREA_DEPTH-200)&&abs(self.Y())>(PARAM::Field::PENALTY_AREA_WIDTH/2-200))
    {
        taskFlag |= PlayerStatus::NOT_AVOID_PENALTY;
    }

    TaskT newTask(task());
    newTask.player.flag = taskFlag;
    newTask.player.pos = TargetPos;
    newTask.player.angle = TargetDir;
//newTask.player.pos = CGeoPoint(100,0);

    setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
    //setSubTask(PlayerRole::makeItSmartGoto(vecNumber,CGeoPoint(100,100),0,0));
    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}

