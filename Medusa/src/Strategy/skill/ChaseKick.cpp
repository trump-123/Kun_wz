#include "ChaseKick.h"
#include <bits/stdc++.h>
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
    const CGeoPoint LEFT_GOAL_POST  = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2+200);
    const CGeoPoint RIGHT_GOAL_POST = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2-200);
    const static int MIN_X = -PARAM::Field::PITCH_LENGTH/2;
    const static int MAX_X = -PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH;
    const static int MIN_Y = -PARAM::Field::PENALTY_AREA_WIDTH/2;
    const static int MAX_Y = PARAM::Field::PENALTY_AREA_WIDTH/2;
    const static int tMAX_X = PARAM::Field::PITCH_LENGTH/2;
    const static int tMIN_X = PARAM::Field::PITCH_LENGTH/2 - PARAM::Field::PENALTY_AREA_DEPTH;
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
    bool in_our_penalty(const CGeoPoint& pos,double padding){
        if(pos.x() > MIN_X + padding && pos.x() < MAX_X - padding && pos.y() > MIN_Y-padding &&pos.y()< MAX_Y +padding)
            return true;
        else
            return false;
    }
    bool in_their_penalty(const CGeoPoint& pos,double padding){
        if(pos.x() > tMIN_X - padding && pos.x() < tMAX_X + padding && pos.y() > MIN_Y-padding &&pos.y()< MAX_Y +padding)
            return true;
        else
            return false;
    }
    bool ifcanwithdraw(const CVisionModule* pVision,const CGeoPoint &ballpos,double d,const int num)
    {
        double dist = 99999;
        int neastnum = -1;
        for(int i = 0;i<PARAM::Field::MAX_PLAYER-1;i++)
        {
            const PlayerVisionT &enemy = pVision->theirPlayer(i);
            if(enemy.Valid())
            {
                if((enemy.Pos() - ballpos).mod() < dist)
                {
                    dist = (enemy.Pos() - ballpos).mod();
                    neastnum = i;
                }
            }
        }
        const PlayerVisionT &enemy = pVision->theirPlayer(neastnum);
        const PlayerVisionT &me = pVision->ourPlayer(num);

        if(dist < d && abs((enemy.Pos() - me.Pos()).dir() - me.Dir()) < 3.1415/4)
            return false;
        else
            return true;
    }
    bool DEBUG_SWITCH;
}
CChaseKick::CChaseKick(){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH,"Debug/ChaseKick",true);
}
void CChaseKick::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        _state = CChaseKick::NOTHING;
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
    const double toBallDir  = abs((ball.Pos() - me.Pos()).dir()-me.Dir());
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
    CWorldModel p;
    int time = p.InfraredOnCount(runner);
    cout<<time<<endl;
    if(!ifcanwithdraw(pVision,ball.Pos(),250,runner)&&((time>30 && time<400)||(((time>30 && time<400)&&(ball.Pos() - me.Pos()).mod()<130)&&abs(me.Dir() - (ball.Pos() - me.Pos()).dir()) < d/8)))
    {
        _state = DEFEND;
        DribbleStatus::Instance()->setDribbleCommand(runner,3);
        targetDir = (ball.Pos() - me.Pos()).dir();
        targetPos = ball.Pos() + Utils::Polar2Vector(1000,targetDir+d);
    }
    else if(ifcanwithdraw(pVision,ball.Pos(),250,runner)&&time > 30 &&(toBallDist < 115 && toBallDir < d/8 && time < 500))
    {
        _state = SHOOT;
        if((LEFT_GOAL_POST - me.Pos()).dir() < me.Dir()&& (RIGHT_GOAL_POST - me.Pos()).dir() > me.Dir())
        {
            DribbleStatus::Instance()->setDribbleCommand(runner,3);
            KickStatus::Instance()->setKick(runner,6000);
            targetPos = ball.Pos() + Utils::Polar2Vector(80,(ball.Pos() - THEIR_GOAL).dir());
            targetDir = (THEIR_GOAL - me.Pos()).dir();
        }
        else
        {
            DribbleStatus::Instance()->setDribbleCommand(runner,3);
            if(time < 300||abs((THEIR_GOAL - me.Pos()).dir() - me.Dir()) < d/2)
            {
                targetPos = ball.Pos() + Utils::Polar2Vector(90,(ball.Pos() - THEIR_GOAL).dir());
                targetDir = (THEIR_GOAL - me.Pos()).dir();
            }
            else
            {
                targetPos = ball.Pos() + Utils::Polar2Vector(90,(ball.Pos() - THEIR_GOAL).dir());
                targetDir = (THEIR_GOAL - me.Pos()).dir();
            }
        }
    }
    else
    {
        _state = CHASE;
        if(toBallDist > 700)
        {
            targetDir = (ball.Pos() - me.Pos()).dir();
            targetPos = ball.Pos() + Utils::Polar2Vector(1000,targetDir);
        }
        else
        {
            targetDir = (ball.Pos() - me.Pos()).dir();
//            if(!ifcanwithdraw(pVision,ball.Pos(),600)||(ball.Vel().mod()<700||ball.Vel().mod()>3000))
//            {
                if(ball.Vel().mod() > 300)
                {
                     targetPos = ball.Pos() + Utils::Polar2Vector(200,targetDir);
                }
                else
                {
                     targetPos = ball.Pos() + Utils::Polar2Vector(80,targetDir);
                }
//            }
//            else
//            {
//                double roratedir = Utils::Normalize(targetDir+ d - d*2/(4*1.2));
//                if(abs((me.Pos() - ball.Pos()).dir() - ball.Vel().dir()) < d/3)
//                {
//                    ballVelLine.projection(me.Pos());
//                }
//                else
//                {
//                   targetPos = ball.Pos() + Utils::Polar2Vector(500,roratedir);
//                }
//            }

            DribbleStatus::Instance()->setDribbleCommand(runner, 3);
        }
    }

//    CWorldModel p;
//    int time = p.InfraredOnCount(runner);
//    if(time > 20 &&(toBallDist < 105 && toBallDir < d/8 && time < 400))
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
//                targetPos = ball.Pos() + Utils::Polar2Vector(90,(ball.Pos() - THEIR_GOAL).dir());
//                targetDir = (THEIR_GOAL - me.Pos()).dir();
//            }
//            else
//            {
//                targetPos = ball.Pos() + Utils::Polar2Vector(90,(ball.Pos() - THEIR_GOAL).dir());
//                targetDir = (THEIR_GOAL - me.Pos()).dir();
//            }
//        }
//    }

    //DribbleStatus::Instance()->setDribbleCommand(runner,3);
    taskFlag =  PlayerStatus::ALLOW_DSS;
    if(in_our_penalty(ballPos,0))
    {
        targetPos = CGeoPoint(-PARAM::Field::PITCH_LENGTH/4+500,90);
    }
    if(in_their_penalty(ballPos,0))
    {
        targetPos = CGeoPoint(PARAM::Field::PITCH_LENGTH/4-500,90);
    }

    TaskT newTask(task());
    newTask.player.pos   = targetPos;
    newTask.player.angle = targetDir;
    newTask.player.flag  = taskFlag;
    setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));

    if(DEBUG_SWITCH){
        switch(_state){
        case NOTHING:
            debug_state = "NOTHING";
            break;
        case CHASE:
            debug_state = "NORMAL CHASE";
            break;
        case DEFEND:
            debug_state = "DEFEND BALL";
            //GDebugEngine::Instance()->gui_debug_x(clear_pos,0);
            //GDebugEngine::Instance()->gui_debug_msg(clear_pos,"cpos",0);
            break;
        case SHOOT:
            debug_state = "SHOOT BALL";
            break;
        default:
            debug_state = "ERROR STATE";
        }
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-1000,0),debug_state.c_str());
    }

    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
