#include "goAndTurnKick.h"
#include <bits/stdc++.h>
#include "skill/Factory.h"
#include "parammanager.h"
#include "KickStatus.h"
#include "SkillUtils.h"
#include "RobotSensor.h"
#include "DribbleStatus.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "WorldModel.h"
#include "GetBestUtils.h"
#include "QWindow"
#include "messidecition.h"
#include "ShootModule.h"
//#include "test1.cpp"
//extern int test();
namespace{

/********* Ball State Flag ***********/
const int WAIT_TOUCH = 0;
const int STATIC = 1;
const int INTER_TOUCH = 2;
const int INTER = 3;
const int CHASE = 4;
const int RUSH = 5;
//const int CHIP = 6;

QString state="";

bool USE_CHIPSOLVED = false;


    const CGeoPoint THEIR_GOAL = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0);
    const CGeoPoint LEFT_GOAL_POST  = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2+200);
    const CGeoPoint RIGHT_GOAL_POST = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2-200);
    const static int MIN_X = -PARAM::Field::PITCH_LENGTH/2;
    const static int MAX_X = -PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH;
    const static int MIN_Y = -PARAM::Field::PENALTY_AREA_WIDTH/2;
    const static int MAX_Y = PARAM::Field::PENALTY_AREA_WIDTH/2;
    const static int tMAX_X = PARAM::Field::PITCH_LENGTH/2;
    const static int tMIN_X = PARAM::Field::PITCH_LENGTH/2 - PARAM::Field::PENALTY_AREA_DEPTH;

    double FRICTION;
    bool IS_SIMULATION;
    double IGNORE_ENEMY_DIST = 650;
    double MAX_DRIBBLE_TIME = 210;

    enum ballState{
        NEED_GET = 1,
        HOLDING = 2,
        CHIP  = 3
    }mode;

    bool goWBreak(const CVisionModule* pVision,const CGeoPoint &dribblepoint,const int num)
    {
        static const double minBreakDist = 1000;
        std::vector<int> enemyNumVec;
        if(WorldModel::Instance()->getEnemyAmountInArea(dribblepoint,minBreakDist,enemyNumVec)){
            for(auto test_enemy:enemyNumVec){
                if(Utils::InTheirPenaltyArea(pVision->theirPlayer(test_enemy).Pos(),0)) continue;
                bool enemyInFront = fabs((pVision->theirPlayer(test_enemy).Pos() - pVision->ourPlayer(num).Pos()).dir() - pVision->ourPlayer(num).Dir()) < PARAM::Math::PI/3;
                bool enemyTooClose = (pVision->theirPlayer(test_enemy).Pos() - pVision->ourPlayer(num).Pos()).mod() < IGNORE_ENEMY_DIST;
                if(enemyInFront && enemyTooClose) {
                    return true;
                    break;
                }
            }
            return false;
        } else
            return false;
    }

    bool judnearestenemy(const CVisionModule* pVision,int num){
        double tonearstenemy = 99999;
        double tonearstour = 99999;
        const PlayerVisionT &me = pVision->ourPlayer(num);
        for(int i =0;i<PARAM::Field::MAX_PLAYER;i++){
            const PlayerVisionT&enemy = pVision->theirPlayer(i);
            if(!enemy.Valid()) continue;
            if((me.Pos() - enemy.Pos()).mod()<tonearstenemy) tonearstenemy = (me.Pos() - enemy.Pos()).mod();
        }
        for(int i =0;i<PARAM::Field::MAX_PLAYER;i++){
            const PlayerVisionT&our = pVision->ourPlayer(i);
            if(!our.Valid()||i==num) continue;
            if((me.Pos() - our.Pos()).mod()<tonearstour) tonearstour = (me.Pos() - our.Pos()).mod();
        }
        if(tonearstenemy<40*10/*||tonearstour<200*/) return true;
        else return false;
    }

    CGeoPoint judbestshootpos(CGeoPoint target,const CVisionModule* pVision,CGeoPoint pos)
    {
//        double dist = 99999;
//        int theirgoalie = -1;
        if(!Utils::InTheirPenaltyArea(target,0))
            return target;
        else
        {
            ShootModule::Instance()->generateBestTarget(pVision,target,pos);
            return target;
//            for(int i =0;i<PARAM::Field::MAX_PLAYER-1;i++)
//            {
//                const PlayerVisionT &enemy = pVision->theirPlayer(i);
//                if(enemy.Pos().dist(THEIR_GOAL) < dist)
//                {
//                    theirgoalie = i;
//                    dist = enemy.Pos().dist(THEIR_GOAL);
//                }
//                else
//                    continue;
//            }
//            const PlayerVisionT &enemy = pVision->theirPlayer(theirgoalie);
//            if(enemy.Y()>300||enemy.Y()<-300)
//                return THEIR_GOAL;
//            else
//            {
//                if(pVision->ball().Y()>0)
//                    return LEFT_GOAL_POST;
//                else
//                    return RIGHT_GOAL_POST;
//            }
        }
    }
    bool IF_DEBUG = true;
    CGeoPoint ballLast(0,0);
}
CgoAndTurnKick::CgoAndTurnKick()
    :getBallMode(NEED_GET)
    ,lastGetBallMode(NEED_GET)
    ,fraredOn(0)
    ,fraredOff(0)
    ,breakon(0)
    ,breakoff(0)
    ,gobreakdiff(false)
    ,predictBall(true)
    ,cnt(0){
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION, "Alert/IsSimulation", false);
    ZSS::ZParamManager::instance()->loadParam(IF_DEBUG,"Debug/goAndTurnKick",false);
    ZSS::ZParamManager::instance()->loadParam(USE_CHIPSOLVED, "chipsolver/USE_CHIPSOLVER_IN_MEDUSA", false);
    if (IS_SIMULATION)
        ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Sim",800);
    else
        ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Real",1520);
}
void CgoAndTurnKick::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        _state = CgoAndTurnKick::NOTHING;
        fraredOn = 0;
        fraredOff= 0;
        breakon = 0;
        breakoff = 0;
        lastGetBallMode = NEED_GET;
        getBallMode = NEED_GET;
        gobreakdiff = false;
        predictBall = true;
        cnt = 0;
    }
    //ZGetBestUtils::instance()->calculateBestPass();
    //GDebugEngine::Instance()->gui_debug_x(ZGetBestUtils::Instance()->getBestFlatPass(),COLOR_GREEN);
    const CGeoPoint target = task().player.pos;
    double power = task().player.angle;
    //const bool useInter = task().player.is_specify_ctrl_method;
    const int runner = task().executor;
    auto taskFlag = task().player.flag;
    const PlayerVisionT& me = pVision->ourPlayer(runner);
    const auto mousePos = me.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir());
    const int oppNum = ZSkillUtils::instance()->getTheirBestPlayer();
    const PlayerVisionT &Bestenemy = pVision->theirPlayer(oppNum);
    const MobileVisionT& ball = pVision->ball();
    const double ballVelDir = ball.Vel().dir();
    CGeoPoint ballPos = ball.Valid() ? ball.Pos() : ballLast;
    const CGeoLine ballVelLine(ballPos, ballVelDir);
    const double ballVelMod = ball.Vel().mod();
    const CGeoPoint projectionPos = ballVelLine.projection(mousePos);
    const double toBallDist = (mousePos - ballPos).mod();
    const CVector me2ball = ballPos - me.RawPos();
    const CVector enemy2me = Bestenemy.RawPos() - me.RawPos();
    auto d = PARAM::Math::PI;
    CGeoPoint targetPos;
    double targetDir;
    interPoint = ZSkillUtils::instance()->getOurInterPoint(runner);
    interTime  = ZSkillUtils::instance()->getOurInterTime(runner);
    dribblePoint = RobotSensor::Instance()->dribblePoint(runner);
    //interTime  = ZSkillUtils::instance()->predictedInterTimeV2(ball.Vel(),ball.Pos(),me,interPoint,interTime);

    GDebugEngine::Instance()->gui_debug_x(target);

    bool frared = RobotSensor::Instance()->IsInfraredOn(runner);

    if(frared && !ball.Valid()){
        ballPos = me.RawPos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir());
        ballLast = ballPos;
    }

    if(frared){
        fraredOn =  fraredOn> MAX_DRIBBLE_TIME ? MAX_DRIBBLE_TIME : fraredOn + 1;
        if(fraredOn > 5) fraredOff = 0;
        predictBall = true;
    } else {
        fraredOff = fraredOff > MAX_DRIBBLE_TIME ? MAX_DRIBBLE_TIME : fraredOff + 1;
        if(fraredOff > 5) fraredOn = 0;
        if(!ball.Valid()){
//            ballPos = ballLast;
//            ballLast = ballPos;
//        }
            if(predictBall){
                float ourdist = 99999;
                float ournum = -1;
                for(int i = 0;i<PARAM::Field::MAX_PLAYER;i++){
                    if(!pVision->ourPlayer(i).Valid()) continue;
                    if(pVision->ourPlayer(i).Pos().dist(ballLast) < ourdist){
                        ourdist = pVision->ourPlayer(i).Pos().dist(ballLast);
                        ournum = i;
                    }
                }

                float theirdist = 99999;
                float theirnum = -1;
                for(int i = 0;i<PARAM::Field::MAX_PLAYER;i++){
                    if(!pVision->theirPlayer(i).Valid()) continue;
                    if(pVision->theirPlayer(i).Pos().dist(ballLast) < theirdist){
                        theirdist = pVision->theirPlayer(i).Pos().dist(ballLast);
                        theirnum = i;
                    }
                }
                if(theirdist < ourdist && fabs((ballLast - pVision->theirPlayer(theirnum).Pos()).dir() - pVision->theirPlayer(theirnum).Dir()) < PARAM::Math::PI / 8){
                    ballPos = pVision->theirPlayer(theirnum).Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,pVision->theirPlayer(theirnum).Dir());
                    ballLast = ballPos;
                }else{
                    ballPos = ballLast ;//+ (ballLast - pVision->ourPlayer(ournum).Pos()).unit()*20;
                }
                predictBall = false;
            }
        }else{
            predictBall = true;
        }
        //ballPos = ballLast;
    }

    //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,-2000),QString("%1").arg(ballPos.x()).toLatin1());
    if(fraredOn > 20){
        getBallMode = HOLDING;
        state = "HOLDING";
        //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PITCH_WIDTH/2-200),QString("%1").arg("HOLDING").toLatin1());

    } else if(fraredOff > 7){
        if(MessiDecision::Instance()->isFlytime()){
            getBallMode = CHIP;
            state = "CHIP";
            //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PITCH_WIDTH/2-200),QString("%1").arg("CHIP").toLatin1());
        }
        else{
            getBallMode = NEED_GET;
            state = "NEED_GET";
        }
    }
    if(frared && lastGetBallMode == HOLDING) getBallMode = HOLDING;

    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PITCH_WIDTH/2-200),QString(state).toLatin1());
    //红外误判处理，预防红外出问题或者吸到别人的屁股
    bool ballInFront = (me2ball.mod() < 20*10 && fabs(me2ball.dir() - me.Dir()) < PARAM::Math::PI/4);
    if(!ballInFront && frared && enemy2me.mod() < 24*10){
        frared = false;
        fraredOn = 0;
        fraredOff = 0;
        getBallMode = NEED_GET;
    }
    lastGetBallMode = getBallMode;

    if(goWBreak(pVision,ballPos,runner)){
        breakon = breakon > MAX_DRIBBLE_TIME ? MAX_DRIBBLE_TIME : breakon + 1;
        if(breakon>5) {
            breakoff = 0;
            gobreakdiff = true;
        }
    } else{
        breakoff = breakoff > MAX_DRIBBLE_TIME ? MAX_DRIBBLE_TIME : breakoff + 1;
        if(breakoff>7){
            breakon = 0;
            gobreakdiff = false;
        }
    }


    CGeoLine ballline(ballPos,ball.Vel().dir());//&&abs(ball.Vel().dir() - (me.Pos() - ball.Pos()).dir())<3.14/3

    if(((Utils::InTheirPenaltyArea(ballPos,1*10) || Utils::InOurPenaltyArea(ballPos,9*10)) && ball.Vel().mod() < 1000) && !(taskFlag&PlayerStatus::NOT_AVOID_PENALTY)){
        if(Utils::InOurPenaltyArea(ballPos,9*10))
        {
            targetPos = CGeoPoint(0-500,90);
        }
        if(Utils::InTheirPenaltyArea(ballPos,1*10))
        {
            targetPos = CGeoPoint(0+500,90);
        }
        targetDir = me2ball.dir();
        setSubTask(PlayerRole::makeItGoto(runner,targetPos,targetDir,taskFlag));
    }
    else if(getBallMode == NEED_GET){
        if(ballVelMod > 700 &&abs(Utils::Normalize(ball.Vel().dir() - (me.Pos() -  ballPos).dir()))<3.14/3)
        {
            _state = TOUCH;

            taskFlag |= PlayerStatus::NOT_AVOID_OUR_VEHICLE | PlayerStatus::NOT_AVOID_THEIR_VEHICLE;
            double inVel;
            if(ball.Vel().mod2() > FRICTION * me2ball.mod())
                inVel = sqrt(ball.Vel().mod2() - FRICTION * me2ball.mod());
            else
                inVel = ball.Vel().mod();

            CGeoPoint targetPoint = target;
            if(me.RawPos().x() > PARAM::Field::PITCH_LENGTH/18*5)
                 targetPoint = ball.RawPos().y() > 0 ? LEFT_GOAL_POST : RIGHT_GOAL_POST;

    //        double beta = Utils::Normalize(ball.Vel().dir() + PARAM::Math::PI - (targetPoint - me.RawPos()).dir());
    //        CVector semi_line = (-ball.Vel()/inVel*1.3 + (targetPoint_ - me.RawPos())/(targetPoint_ - me.RawPos()).mod()*3);
    //        double semi_angle = (-ball.Vel()/inVel*1.3 + (targetPoint_ - me.RawPos())/(targetPoint_ - me.RawPos()).mod()*3*(me.RawPos().x()/PARAM::Field::PITCH_LENGTH/2+1.4)).dir();
    //        double final_angle = (semi_line + ((-ball.Vel()/inVel + (targetPoint - me.RawPos())/(targetPoint - me.RawPos()).mod())) ).mod();
    //        if(me.RawPos().x()>2500 ){
    //            if(ball.RawPos().y()>0)
    //                semi_angle = (-ball.Vel()/inVel*1.4 + (targetPoint_ - me.RawPos())/(targetPoint_ - me.RawPos()).mod()*3*(me.RawPos().x()/PARAM::Field::PITCH_LENGTH/2+1.4)+ (down_post - me.RawPos())/(down_post-me.RawPos()).mod()*0.5).dir();
    //            else
    //                semi_angle = (-ball.Vel()/inVel*1.4 + (targetPoint_ - me.RawPos())/(targetPoint_ - me.RawPos()).mod()*3*(me.RawPos().x()/PARAM::Field::PITCH_LENGTH/2+1.4)+ (up_post - me.RawPos())/(up_post-me.RawPos()).mod()*0.5).dir();
    //        }
    //        double alpha = atan2(power * sin(beta) / 0.4 + 1e-5, inVel + power * cos(beta) / 0.4 + 1e-5);


        double beta = Utils::Normalize(ball.Vel().dir() + PARAM::Math::PI - (target - me.RawPos()).dir());
        double alpha = atan2(power * sin(beta) / 0.42 + 1e-5, inVel + power * cos(beta) / 0.42 + 1e-5);
        bool cantouch = ZSkillUtils::instance()->isSafeWaitTouch(pVision,runner,me.Pos());
            if(abs(Utils::Normalize((target - me.RawPos()).dir() + beta - alpha - (ballPos - me.Pos()).dir())) > 3.14/3)
            {
                if(ballline.projection(me.Pos()).dist(me.Pos())<1000||cantouch)
                    targetPos = interPoint;//ballline.projection(me.Pos());
                else
                    targetPos = interPoint;//ballline.projection(me.Pos());
                targetDir = (ballPos - me.RawPos()).dir();
            }
            else
            {
                if(ballline.projection(me.RawPos()).dist(me.RawPos())<1000||cantouch)
                    targetPos = ballline.projection(me.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir())) + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir()+3.14);//interPoint;
                else
                    //targetPos = ballline.projection(me.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir())) + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir()+3.14);
                    targetPos = interPoint + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir()+3.14);
                targetDir = (target - me.RawPos()).dir() + beta - alpha;
                DribbleStatus::Instance()->setDribbleOff(runner);
                if(MessiDecision::Instance()->needChip()){
                    KickStatus::Instance()->setChipKick(runner,power);//6000
                } else {
                    KickStatus::Instance()->setKick(runner,power);//6000
                }
            }
            if(me2ball.mod() < 35*10)
               DribbleStatus::Instance()->setDribbleCommand(runner,3);
        }
        else {

            _state = CHASE;
            if(judnearestenemy(pVision,runner)) taskFlag |=  PlayerStatus::ALLOW_DSS | PlayerStatus::NOT_AVOID_OUR_VEHICLE;
            else taskFlag |= PlayerStatus::NOT_AVOID_OUR_VEHICLE | PlayerStatus::NOT_AVOID_THEIR_VEHICLE;//+ PlayerStatus::NOT_AVOID_PENALTY

            //CGeoPoint interPoint = ZSkillUtils::instance()->getOurInterPoint(runner);
            bool catchball = ZSkillUtils::instance()->isSafeWaitTouch(pVision,runner,interPoint);
//            GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(1000,0),QString("iscatch:%1").arg(QString::number(catchball)).toLatin1());
            if(ball.Vel().mod() > 1000 && catchball){//
                targetDir = Utils::Normalize((interPoint - ballPos).dir() + PARAM::Math::PI);
                targetPos = interPoint + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,Utils::Normalize((interPoint - ballPos).dir()));
            }
            else{
                if(toBallDist > 70*10)
                {
                    targetDir = (ballPos - me.Pos()).dir();
                    float dist = 200 + 0.2 * toBallDist;
                    targetPos = ballPos + Utils::Polar2Vector(dist,targetDir);
                }
                else
                {
                    targetDir = me2ball.dir();
                    if(fraredOn > 15 && me2ball.mod() < 15*10){
                        targetPos = ballPos + Utils::Polar2Vector(-90,targetDir);
                    }
                    else{
                        if(ballVelMod > 30*10)
                        {
                             float dist = 15*10 + 0.15 * toBallDist;
                             targetPos = ballPos + Utils::Polar2Vector(dist,targetDir);
                        }
                        else
                        {
                             float dist = 10*10 + 0.2 * toBallDist;
                             targetPos = ballPos + Utils::Polar2Vector(dist,targetDir);
                        }
                    }
                    DribbleStatus::Instance()->setDribbleCommand(runner, 3);
                }
            }
        }
//        if(fraredOn > 20){
//            targetPos = me.Pos();
//        }
        setSubTask(PlayerRole::makeItGoto(runner,targetPos,targetDir,taskFlag));
    }
    else if(getBallMode == HOLDING)
    {
        if(gobreakdiff && me2ball.mod()<15*10 && fabs(me.Dir() - me2ball.dir()) < d/3){
            _state = DEFEND;
            setSubTask(PlayerRole::makeItWBreak(runner, target, power, -1, 3,false));
        }
        else{
            _state = SHOOT;
            if(abs((judbestshootpos(target,pVision,me.RawPos()) - me.RawPos()).dir() - me.Dir()) < d/10)
            {
                if(pVision->getCycle() - _mylastCycle > 15)
                {
                    if(Utils::InTheirPenaltyArea(target,0)){
                        if(abs((judbestshootpos(target,pVision,me.RawPos()) - me.RawPos()).dir() - me.Dir()) < 5 * d / 180){
                            KickStatus::Instance()->setKick(runner,power);
                        }
                    }else{
                        if(MessiDecision::Instance()->needChip()){
                            KickStatus::Instance()->setChipKick(runner,power);
                        } else{
                            KickStatus::Instance()->setKick(runner,power);//*1.26000
                        }
                    }
                    DribbleStatus::Instance()->setDribbleOff(runner);
                }

                    DribbleStatus::Instance()->setDribbleCommand(runner,3);
                    targetPos = ballPos + Utils::Polar2Vector(100,(ballPos - judbestshootpos(target,pVision,me.RawPos())).dir());
                    targetDir = (judbestshootpos(target,pVision,me.RawPos()) - me.Pos()).dir();
            }
            else
            {
                DribbleStatus::Instance()->setDribbleCommand(runner,3);
                if(abs((judbestshootpos(target,pVision,me.RawPos()) - me.Pos()).dir() - me.Dir()) < d/2)
                {
                    targetPos = ballPos + Utils::Polar2Vector(100,(ballPos - judbestshootpos(target,pVision,me.RawPos())).dir());
                    targetDir = (judbestshootpos(target,pVision,me.RawPos()) - me.Pos()).dir();
                }
                else
                {
                    targetPos = ballPos + Utils::Polar2Vector(90,(ballPos - judbestshootpos(target,pVision,me.RawPos())).dir());
                    targetDir = (judbestshootpos(target,pVision,me.RawPos()) - me.Pos()).dir();
                }
                _mylastCycle = pVision->getCycle();
            }
            setSubTask(PlayerRole::makeItGoto(runner,targetPos,targetDir,taskFlag));
        }
    }
    else if(getBallMode==CHIP){
        if(ball.RawPos().dist(me.RawPos()) < 4500){
            targetPos = interPoint;
        } else {
            CGeoSegment ballisFlyLine(MessiDecision::Instance()->firstChipPos(),MessiDecision::Instance()->firstChipPos() + Utils::Polar2Vector(99999,MessiDecision::Instance()->firstChipDir()));
            CGeoPoint projection = ballisFlyLine.projection(me.Pos());
            if(ballisFlyLine.IsPointOnLineOnSegment(projection)){
                targetPos = projection;
            } else {
                targetPos = me.Pos();
            }
        }
        //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,1000),QString("%1").arg(222).toLatin1(),COLOR_RED);
        targetDir = (ballPos - me.RawPos()).dir();
        setSubTask(PlayerRole::makeItGoto(runner,targetPos,targetDir,taskFlag));
    }

    if(ball.Valid() || frared) ballLast = ballPos;

    MessiDecision::Instance()->setBallPos(ballPos);


    if(IF_DEBUG){
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
            //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(2000,0),QString("ipower:%1").arg(power).toLatin1(),3);
            break;
        case TOUCH:
            debug_state = "TOUCH BALL";
            GDebugEngine::Instance()->gui_debug_line(me.Pos(),target,COLOR_GREEN);
            break;
        default:
            debug_state = "ERROR STATE";
        }
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PITCH_WIDTH/2-400),debug_state.c_str());
    }
     //GDebugEngine::Instance()->gui_debug_x(ball.RawPos(),COLOR_RED);
    GDebugEngine::Instance()->gui_debug_line(me.Pos(),target,COLOR_GREEN);
    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}

bool CgoAndTurnKick::judgeShootMode(const CVisionModule * pVision) {
    const MobileVisionT& ball = pVision->ball();
    const int robotNum = task().executor;
    const PlayerVisionT& me = pVision->ourPlayer(robotNum);
    const CVector me2Target = targetPoint - me.RawPos();
    double finalDir = me2Target.dir();
    double ballVel2FinalDiff = Utils::Normalize(ball.Vel().dir() - finalDir);

    bool shootMode = fabs(ballVel2FinalDiff) < 0.5;
    return shootMode;
}

void CgoAndTurnKick::judgeMode(const CVisionModule* pVision){
    const MobileVisionT& ball = pVision->ball();
    const int robotNum = task().executor;
    const PlayerVisionT& me = pVision->ourPlayer(robotNum);
    bool frared = RobotSensor::Instance()->IsInfraredOn(robotNum);
    const CVector me2Ball = ball.RawPos() - me.RawPos();

    if(isTouch)
    {
        getBallMode = WAIT_TOUCH;
        return;
    }

    interPoint = ZSkillUtils::instance()->getOurInterPoint(robotNum);
    interTime  = ZSkillUtils::instance()->getOurInterTime(robotNum);

//    CGeoLine ballLine(ball.Pos(), ball.Vel().dir());
//    CGeoPoint car_pos = me.RawPos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER, me.RawDir());
//    // ballLineProjection = ballLine.projection(me.RawPos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER, me.RawDir()));
//    CVector me2Projection = ballLineProjection - car_pos;
//    CVector interPoint2target;

//    if (me2Projection.mod()<PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER){
//        CVector interPoint2target = (me.RawPos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER, me.RawDir()))- interPoint;
//        if(IF_DEBUG) GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-5000,1700),QString("car position").toLatin1(),COLOR_GREEN);
//    }
//    else{
//        CVector interPoint2target = targetPoint - interPoint;
//        if(IF_DEBUG) GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-5000,1700),QString("target position").toLatin1(),COLOR_GREEN);
//    }

//    double ballBias = fabs(Utils::Normalize(ball.Vel().dir() - interPoint2target.dir()));
//    double ballAngleDiff = fabs(Utils::Normalize(ball.Vel().dir() + PARAM::Math::PI - interPoint2target.dir()));

}
