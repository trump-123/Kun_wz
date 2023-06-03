#include "MarkingFront.h"
#include "skill/Factory.h"
#include "parammanager.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
namespace{
    const CGeoPoint OUR_GOAL(-PARAM::Field::PITCH_LENGTH/2,0);
    const CGeoPoint THEIR_GOAL = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0);
    const CGeoPoint LEFT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2 - 2);
    const CGeoPoint RIGHT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2 + 2);
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
CMarkingFront::CMarkingFront(){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH,"Debug/MarkingFront",false);
}
void CMarkingFront::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
    }

    const CGeoPoint target = task().player.pos;
    const bool useInter = task().player.is_specify_ctrl_method;
    const int runner = task().executor;
    auto taskFlag = task().player.flag;
    const PlayerVisionT& me = pVision->ourPlayer(runner);
    const auto mousePos = me.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir());
    const MobileVisionT& ball = pVision->ball();
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


    if(target.x() < PARAM::Field::PITCH_LENGTH/8+200)
    {
        targetPos = target + Utils::Polar2Vector(350,(ball.Pos() - target).dir());
        targetDir = (ball.Pos() - me.Pos()).dir();
    }
    else
    {
      targetPos = markingpos;
      targetDir = (target - me.Pos()).dir();
    }

    taskFlag  = PlayerStatus::ALLOW_DSS;
    if(ball.Vel().mod()<1000)
    {
        taskFlag |= PlayerStatus::DODGE_BALL;
    }
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
