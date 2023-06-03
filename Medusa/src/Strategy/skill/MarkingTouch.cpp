#include "MarkingTouch.h"
#include "skill/Factory.h"
#include "parammanager.h"
#include "WorldDefine.h"
#include "staticparams.h"
#include "parammanager.h"
#include "DribbleStatus.h"
#include "WorldModel.h"
#include "VisionModule.h"
#include "staticparams.h"
namespace{
    const CGeoPoint OUR_GOAL(-PARAM::Field::PITCH_LENGTH/2,0);
    const CGeoPoint THEIR_GOAL = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0);
    const CGeoPoint LEFT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2 - 2);
    const CGeoPoint RIGHT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2 + 2);
    const CGeoPoint LEFT_THEIR__GOAL_POST(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2 - 2);
    const CGeoPoint RIGHT_THEIR_GOAL_POST(PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2 + 2);
    const CGeoPoint LEFT_TOP(0,-PARAM::Field::PITCH_WIDTH/2);
    const CGeoPoint RIGHT_TOP(0,PARAM::Field::PITCH_WIDTH/2);
    bool DEBUG_SWITCH;
    bool theirtogoal(const CGeoPoint&pos,const CGeoPoint&ball)
    {
        double togoaldir = (ball - pos).dir();
        double leftdir   = (LEFT_GOAL_POST - ball).dir();
        double rightdir  = (RIGHT_GOAL_POST - ball).dir();
        if(ball.y()>LEFT_GOAL_POST.y()&&ball.y()<RIGHT_GOAL_POST.y())
        {
            //cout<<2<<endl;
            if(togoaldir > rightdir || togoaldir < leftdir)
                return  true;
            else
                return false;
        }
        else
        {
            if(togoaldir > rightdir && togoaldir < leftdir)
                return  true;
            else
                return false;
        }
    }
}
CMarkingTouch::CMarkingTouch(){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH,"Debug/MarkingTouch",false);
}
void CMarkingTouch::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
    }

    const CGeoPoint target = task().player.pos;
    const bool useInter = task().player.is_specify_ctrl_method;
    const int runner = task().executor;
    auto taskFlag = task().player.flag;
    const PlayerVisionT& me = pVision->ourPlayer(runner);
    const auto mousePos = me.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir());
    const MobileVisionT& ball = pVision->ball();
    auto pi = PARAM::Math::PI;
    const double ballVelDir = ball.Vel().dir();
    const CGeoPoint& ballPos = ball.RawPos();
    const CGeoLine ballVelLine(ballPos, ballVelDir);
    const double ballVelMod = ball.Vel().mod();
    const CGeoPoint projectionPos = ballVelLine.projection(mousePos);
    const double toBallDist = (mousePos - ballPos).mod();
    const CGeoLine standline(CGeoPoint(PARAM::Field::PITCH_LENGTH/8,0),CGeoPoint(PARAM::Field::PITCH_LENGTH/8,100));
    const CGeoLine targettoourgoal(target,OUR_GOAL);
    const CGeoLineLineIntersection intersection(standline,targettoourgoal);
    const CGeoPoint markingpos = intersection.IntersectPoint();
    CGeoPoint targetPos;
    double targetDir;


    if(ball.Vel().mod()>1500)
    {
        if(abs((ballPos - me.Pos()).dir() - (THEIR_GOAL - me.Pos()).dir()) < 3.14/9*2)
        {
            targetDir = (THEIR_GOAL - me.Pos()).dir();
            targetPos = projectionPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir()+pi);
        }
        else
        {
            targetDir = (ball.Pos() - me.Pos()).dir();
            targetPos = projectionPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir()+pi);
        }
    }
    else
    {
//        if(toBallDist > 100)
//        {
//            targetDir = (ball.Pos() - me.Pos()).dir();
//            targetPos = ball.Pos() + Utils::Polar2Vector(150,(ball.Pos() - me.Pos()).dir());
//        }
//        else
//        {
            targetDir = (ball.Pos() - me.Pos()).dir();
            targetPos = ball.Pos() + Utils::Polar2Vector(90,(me.Pos() - ball.Pos()).dir());
       // }
            if(toBallDist>200)
            {
                taskFlag = PlayerStatus::DODGE_BALL;
            }
    }

    double leftdir = (LEFT_THEIR__GOAL_POST - me.Pos()).dir();
    double rightdir = (RIGHT_THEIR_GOAL_POST - me.Pos()).dir();
    CWorldModel p;
    int time = p.InfraredOnCount(runner);
    if(time>1)
    {
        if(me.Dir()>leftdir&&me.Dir()<rightdir)
        {
            KickStatus::Instance()->setKick(runner,6400);
            DribbleStatus::Instance()->setDribbleOff(runner);
            targetPos = ball.Pos() + Utils::Polar2Vector(80,(ball.Pos()-THEIR_GOAL).dir());
            targetDir = (THEIR_GOAL - me.Pos()).dir();
        }
        else
        {
            DribbleStatus::Instance()->setDribbleCommand(runner,3);
            targetPos = ball.Pos() + Utils::Polar2Vector(80,(ball.Pos()-THEIR_GOAL).dir());
            targetDir = (THEIR_GOAL - me.Pos()).dir();
        }
    }

    taskFlag  |= PlayerStatus::ALLOW_DSS;

    TaskT newTask(task());
    newTask.player.pos = targetPos;
    newTask.player.angle = targetDir;
    newTask.player.flag = taskFlag;
    setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));

    if(DEBUG_SWITCH){
        auto endPos = ballPos + Utils::Polar2Vector(ballVelMod,ballVelDir);
        GDebugEngine::Instance()->gui_debug_line(ballPos,endPos,4);
    }

    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
