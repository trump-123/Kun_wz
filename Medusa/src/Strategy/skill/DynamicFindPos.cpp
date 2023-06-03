#include "DynamicFindPos.h"
#include "skill/Factory.h"
#include "parammanager.h"
#include "KickStatus.h"
#include "DribbleStatus.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "WorldModel.h"
namespace{
    const CGeoPoint THEIR_GOAL = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0);
    const CGeoPoint LEFT_GOAL_POST  = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2+100);
    const CGeoPoint RIGHT_GOAL_POST = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2-100);
//    bool judturn(const CVisionModule* pVision,double dir)
//    {
//        double neardist = 99999;
//        int nearnum;
//        const MobileVisionT& ball = pVision->ball();
//        for(int i=0;i<16;i++)
//        {
//            const PlayerVisionT& their = pVision->theirPlayer(i);
//            if((ball.Pos() - their.Pos()).mod()<neardist)
//            {
//                nearnum = i;
//                neardist= (ball.Pos() - their.Pos()).mod();
//            }
//        }
//        const PlayerVisionT& their1 = pVision->theirPlayer(nearnum);

//    }
    bool DEBUG_SWITCH;
}
CDynamicFindPos::CDynamicFindPos(){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH,"Debug/ChaseKick",false);
}
void CDynamicFindPos::plan(const CVisionModule* pVision){
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
    auto d = PARAM::Math::PI;
    CGeoPoint targetPos;
    double targetDir;
//    if(ballVelMod < 300){
//        targetDir = (target - ballPos).dir();
//        targetPos = ballPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,targetDir + PARAM::Math::PI);

//    }else{
//        targetDir = useInter ? Utils::Normalize(ballVelDir + PARAM::Math::PI) : (target - mousePos).dir();
//        targetPos = projectionPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,targetDir + PARAM::Math::PI);
//    }

//    // add avoid ball flag
//    if(toBallDist>200){
//        taskFlag |= PlayerStatus::DODGE_BALL;
//    }
//    if(toBallDist > 700)
//    {
//        targetDir = (ball.Pos() - me.Pos()).dir();
//        targetPos = ball.Pos() + Utils::Polar2Vector(1000,targetDir);
//        taskFlag  = PlayerStatus::NOT_AVOID_OUR_VEHICLE;
//    }
//    else
//    {
//        targetDir = (ball.Pos() - me.Pos()).dir();
//        targetPos = ball.Pos() + Utils::Polar2Vector(80,targetDir);
//        DribbleStatus::Instance()->setDribbleCommand(runner, 3);
//        taskFlag  = PlayerStatus::NOT_AVOID_OUR_VEHICLE + PlayerStatus::NOT_AVOID_THEIR_VEHICLE;
//    }

//    CWorldModel p;
//    int time = p.InfraredOnCount(runner);
//    if(time > 30 && toBallDist < 105)
//    {
//        if((LEFT_GOAL_POST - me.Pos()).dir() < me.Dir()&& (RIGHT_GOAL_POST - me.Pos()).dir() > me.Dir())
//        {
//            KickStatus::Instance()->setKick(runner,6500);
//            targetPos = ball.Pos() + Utils::Polar2Vector(80,(ball.Pos() - THEIR_GOAL).dir());
//            targetDir = (THEIR_GOAL - me.Pos()).dir();
//        }
//        else
//        {
//            DribbleStatus::Instance()->setDribbleCommand(runner,3);
//            if(time < 300||abs((THEIR_GOAL - me.Pos()).dir() - me.Dir()) < d/2)
//            {
//                targetPos = ball.Pos() + Utils::Polar2Vector(130,(ball.Pos() - THEIR_GOAL).dir());
//                targetDir = (THEIR_GOAL - me.Pos()).dir();
//            }
//            else
//            {
//                targetPos = ball.Pos() + Utils::Polar2Vector(130,(ball.Pos() - THEIR_GOAL).dir());
//                targetDir = (THEIR_GOAL - me.Pos()).dir();
//            }
//        }
//    }
    if(target.y()>300)
    {

    }
    else if(target.y()<-300)
    {

    }
    else
    {

    }



    TaskT newTask(task());
    newTask.player.pos   = targetPos;
    newTask.player.angle = targetDir;
    newTask.player.flag  = taskFlag;
    setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));

    if(DEBUG_SWITCH){
        auto endPos = ballPos + Utils::Polar2Vector(ballVelMod,ballVelDir);
        GDebugEngine::Instance()->gui_debug_line(ballPos,endPos,4);
    }

    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}

