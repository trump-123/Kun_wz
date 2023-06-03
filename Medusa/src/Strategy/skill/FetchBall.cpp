#include "FetchBall.h"
#include "skill/Factory.h"
#include "parammanager.h"
#include "WorldDefine.h"
#include "RobotSensor.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "WorldModel.h"
#include "DribbleStatus.h"
namespace{
    auto PI = PARAM::Math::PI;
    const CGeoPoint THEIR_GOAL = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0);
    bool DEBUG_SWITCH;
    int BPFraredOn = 30;
    int fraredOn = 0;
    int fraredOff = 0;
    CGeoPoint ballLast(0,0);
}
CFetchBall::CFetchBall():goBackBall(true){
    ZSS::ZParamManager::instance()->loadParam(BPFraredOn,"GetBall/FraredBuffer",30);
}
void CFetchBall::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        setState(BEGINNING);
        fraredOn = 0;
        fraredOff = 0;

        goBackBall = true;
        cnt = 0;
        notDribble = false;
    }

    notDribble = false;
    const CGeoPoint target = task().player.pos;
    const bool useInter = task().player.is_specify_ctrl_method;
    const int runner = task().executor;
    auto taskFlag = task().player.flag;
    double power = task().player.kickpower;
    const PlayerVisionT& me = pVision->ourPlayer(runner);
    const auto mousePos = me.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir());
    const MobileVisionT& ball = pVision->ball();
    CGeoPoint ballPos = ball.Valid() ? ball.Pos() : ballLast;
    CVector me2ball = ballPos - me.RawPos();
    CVector me2target = target - me.RawPos();
    CVector ball2target = target - ballPos;

    static double dirstaic = me.Dir();

    static CGeoPoint carPos;

    bool frared = RobotSensor::Instance()->IsInfraredOn(runner);
    if(frared && !ball.Valid()){
        ballPos = me.RawPos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir());
    }

    if(frared){
        fraredOn = fraredOn >= 1024 ? 1024 :fraredOn + 1;
        fraredOff = 0;
        if(!ball.Valid()){
            me2ball = CVector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER*cos(me.Dir()),PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER*sin(me.Dir()));
            ballPos = me.Pos() + me2ball;
        }
    } else {
        if(!ball.Valid()){
            ballPos  = ballLast + (ballLast - me.Pos()).unit()*20;
            ballLast = ballPos;
        }
        cnt = 0;
        fraredOn = 0;
        fraredOff = fraredOff >= 1024 ? 1024 : fraredOff + 1;
    }

    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(1000,0),QString("%1").arg(goBackBall).toLatin1());

    /********************* set subTask*********************/
    if(!Utils::IsInField(ballPos,PARAM::Vehicle::V2::PLAYER_SIZE)){
        if(fraredOn <  1 * BPFraredOn){
            carPos = target + Utils::Polar2Vector(-0.8 * PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me2target.dir());
            if(goBackBall){//先跑到球点在场上的映射点
                CGeoPoint gotoPoint = Utils::MakeInField(ballPos,2.0 * PARAM::Vehicle::V2::PLAYER_SIZE);
                if((me.RawPos() - gotoPoint).mod() < 1 * 15){
                    dirstaic = me2ball.dir();
                    goBackBall = false;
                }
                setSubTask(PlayerRole::makeItGoto(runner,gotoPoint,me2ball.dir(),PlayerStatus::NOT_AVOID_PENALTY|PlayerStatus::ALLOW_DSS));
            } else{
                setSubTask(PlayerRole::makeItGoto(runner,ballPos,me2ball.dir(),PlayerStatus::NOT_AVOID_PENALTY|PlayerStatus::ALLOW_DSS));
            }
        } else {
            goBackBall = true;
            CGeoPoint gotoPoint = Utils::MakeInField(ballPos,3.0 * PARAM::Vehicle::V2::PLAYER_SIZE);
            setSubTask(PlayerRole::makeItGoto(runner,gotoPoint,dirstaic,CVector(0,0),0,3000,5,3000,5,PlayerStatus::NOT_AVOID_PENALTY|PlayerStatus::ALLOW_DSS));
        }
    } else {
        if(fraredOn < BPFraredOn && cnt == 0){
            carPos = target + Utils::Polar2Vector(-0.8 * PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me2target.dir());
            dirstaic = me2target.dir();
            if(goBackBall){
                CGeoPoint gotoPoint = ballPos + Utils::Polar2Vector(-2.0 * PARAM::Vehicle::V2::PLAYER_SIZE,ball2target.dir());
                if((me.RawPos() - gotoPoint).mod() < 1 * 15){
                    goBackBall = false;
                }
                notDribble = true;
                setSubTask(PlayerRole::makeItGoto(runner,gotoPoint,me2ball.dir(),PlayerStatus::NOT_AVOID_PENALTY|PlayerStatus::ALLOW_DSS|PlayerStatus::DODGE_BALL));
            } else {
                notDribble = false;
                setSubTask(PlayerRole::makeItGoto(runner,ballPos,me2ball.dir(),PlayerStatus::NOT_AVOID_PENALTY|PlayerStatus::ALLOW_DSS));
            }
        } else {
            goBackBall = true;
            if(fabs(me2target.dir() - me.Dir()) < PI / 10){
                if(ball2target.mod() > 2000){
                    if(useInter && abs(me.Dir() - me2target.dir()) < 5 * PI / 180 && pVision->getCycle() - _mylastCycle >20){
                        KickStatus::Instance()->setKick(runner,power);
                    }
                    CGeoPoint shootpos = ballPos + Utils::Polar2Vector(90,me2target.dir() + PI);
                    setSubTask(PlayerRole::makeItGoto(runner,ballPos,me2target.dir(),PlayerStatus::NOT_AVOID_PENALTY|PlayerStatus::ALLOW_DSS));
                }
                else if(ball2target.mod() < 300){
                    notDribble = false;
                    if (cnt > 20){
                        notDribble = true;
                    }
//                    setSubTask(PlayerRole::makeItStop(vecNumber, PlayerStatus::ALLOW_DSS|PlayerStatus::NOT_AVOID_PENALTY));
                    setSubTask(PlayerRole::makeItGoto(runner, carPos, dirstaic, CVector(0, 0), 0, 100, 8, 300, 5, PlayerStatus::ALLOW_DSS|PlayerStatus::NOT_AVOID_PENALTY));
                } else {
                    carPos = target + Utils::Polar2Vector(-1 * PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me2target.dir());
                    dirstaic = me2target.dir();
                    CGeoPoint slowrunPos = ballPos + Utils::Polar2Vector(150,me2target.dir());
                    setSubTask(PlayerRole::makeItGoto(runner,slowrunPos,dirstaic,PlayerStatus::NOT_AVOID_PENALTY|PlayerStatus::ALLOW_DSS));
                }
            } else {
                _mylastCycle = pVision->getCycle();
                carPos = target + Utils::Polar2Vector(-1 * PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me2target.dir());
                dirstaic = me2target.dir();
                CGeoPoint turnPos = ballPos + Utils::Polar2Vector(-90,me2target.dir());
                setSubTask(PlayerRole::makeItGoto(runner,turnPos,dirstaic,PlayerStatus::NOT_AVOID_PENALTY|PlayerStatus::ALLOW_DSS));
            }
        }
    }

    if(ball.Valid() || fraredOn) ballLast = ballPos;
    if(ball2target.mod() < 120) cnt++;
    else cnt = 0;
    if(cnt > 50) WorldModel::Instance()->setBPFinished(true);
    else  WorldModel::Instance()->setBPFinished(false);

    if((me2ball.mod() < 250 || frared) && cnt <=50 && !notDribble){
        DribbleStatus::Instance()->setDribbleCommand(runner,2);
    } else {
        DribbleStatus::Instance()->setDribbleOff(runner);
    }


    _lastCycle = pVision->getCycle();
    CStatedTask::plan(pVision);
}
