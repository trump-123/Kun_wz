#include "PenaltyKick.h"
#include "skill/Factory.h"
#include "TaskMediator.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "parammanager.h"
#include "KickStatus.h"
#include "DribbleStatus.h"
#include "WorldModel.h"
#include "RobotSensor.h"
#include "string.h"
#include "math.h"

namespace {
    bool DEBUG_SWITCH;
    bool IS_SIMULATION;
    double PI = PARAM::Math::PI;
    int fraredOn = 0;
    int fraredOff = 0;
    int getballTime = 30;
    int maxTime = 1024;
    double FRICTION;
    CGeoPoint MIDDLE_GOAL(PARAM::Field::PITCH_LENGTH/2,0);
    CGeoPoint RIGHT_GOAL(PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2 - 100);
    CGeoPoint LEFT_GOAL(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2 + 50);
    CGeoPoint ballLast(0,0);

    bool canshoot(const CVisionModule*pVision,CGeoPoint &pos){
        int num = -1;
        double diffdist = 99999;
        for(int i = 0;i < PARAM::Field::MAX_PLAYER;++i){
            if(pVision->theirPlayer(i).Valid()){
                if(pVision->theirPlayer(i).Pos().dist(MIDDLE_GOAL) < diffdist){
                    diffdist = pVision->theirPlayer(i).Pos().dist(MIDDLE_GOAL);
                    num = i;
                }
            }
        }
        if(!Utils::InTheirPenaltyArea(pVision->theirPlayer(num).Pos(),0)){
            return true;
        }else{
            if(pVision->theirPlayer(num).Pos().dist(pos) < 400){
                return  false;
            } else {
                return true;
            }
        }
    }
}

CPenaltyKick::CPenaltyKick()
    :goBackBall(true)
    ,turnPos(true)
    ,canKick(false)
    ,isKick(false)
    ,notDribble(true){
        //ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH, "DEBUG/Penaltykick", true);
        ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION, "Alert/IsSimulation", false);
        if (IS_SIMULATION)
            ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Sim",800);
        else
            ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Real",1520);
}

void CPenaltyKick::plan(const CVisionModule* pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        fraredOn = 0;
        fraredOff = 0;
        goBackBall= true;
        turnPos=true;
        canKick=false;
        isKick=false;
        notDribble = true;
    }
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);
    const MobileVisionT &ball = pVision->ball();
    CGeoPoint ballPos = ball.Valid() ? ball.Pos() : ballLast;
    CVector me2ball = ballPos - self.RawPos();
    CVector ball2right = RIGHT_GOAL - ballPos;
    CVector ball2left = LEFT_GOAL - ballPos;
    CVector me2right = RIGHT_GOAL - ballPos;
    CVector me2left = LEFT_GOAL - ballPos;

    bool frared = RobotSensor::Instance()->IsInfraredOn(vecNumber);

    if(frared && !ball.Valid()){
        ballPos = self.RawPos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,self.Dir());
    }

    if(frared){
        fraredOn = fraredOn >= maxTime ? maxTime : fraredOn + 1;
        fraredOff = 0;
        if(!ball.Valid()){
            me2ball = CVector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER*cos(self.Dir()),PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER*sin(self.Dir()));
            ballPos = self.Pos() + me2ball;
        }
    } else {
        if(!ball.Valid()){
            ballPos = ballLast + (ballLast - self.Pos()).unit()*20;
            ballLast = ballPos;
        }

        fraredOff = fraredOff > maxTime ? maxTime : fraredOff + 1;
        fraredOn = 0;
    }

    /**********        set subTask       ************/
    if(goBackBall){
        CGeoPoint gotoPoint = ballPos + Utils::Polar2Vector(-500,ball2right.dir());
        setSubTask(PlayerRole::makeItGoto(vecNumber,gotoPoint,me2ball.dir(),PlayerStatus::ALLOW_DSS | PlayerStatus::NOT_AVOID_PENALTY));
        if((gotoPoint - self.Pos()).mod() < 1 * 15){
            goBackBall = false;
        }
    } else {
        if(fraredOn > getballTime){
           if(canshoot(pVision,RIGHT_GOAL) && turnPos){
             canKick = true;
             turnPos = false;
             notDribble = true;
           }else{
               CGeoPoint getballPos = ballPos + Utils::Polar2Vector(-90,me2left.dir());
               if(abs(me2left.dir() - self.Dir()) < 5*PI/180){
                   canKick = true;
                   notDribble = true;
               }
               notDribble = false;
               setSubTask(PlayerRole::makeItGoto(vecNumber,getballPos,me2left.dir(),PlayerStatus::ALLOW_DSS | PlayerStatus::NOT_AVOID_PENALTY));
           }
        } else {
            notDribble = false;
            CGeoPoint getballPos = ballPos + Utils::Polar2Vector(-90,me2ball.dir());
            double taskDir = me2ball.dir();
            if(fraredOn > 10){
                getballPos = self.Pos();
                taskDir = me2right.dir();
            }
            if(isKick){
                setSubTask(PlayerRole::makeItGoto(vecNumber,getballPos,taskDir,CVector(0,0),8,3000,8,5000,5,PlayerStatus::ALLOW_DSS));
            }else{
                setSubTask(PlayerRole::makeItGoto(vecNumber,getballPos,taskDir,CVector(0,0),8,600,8,1000,5,PlayerStatus::ALLOW_DSS | PlayerStatus::NOT_AVOID_PENALTY));
            }
        }
    }

    if((me2ball.mod() < 250 || frared) && !notDribble){
        DribbleStatus::Instance()->setDribbleCommand(vecNumber,3);
    }else{
        DribbleStatus::Instance()->setDribbleCommand(vecNumber,0);
    }

    if(canKick){
        KickStatus::Instance()->setKick(vecNumber,6000);
    }
    if(RobotSensor::Instance()->IsKickerOn(vecNumber)>0){
        isKick = true;
        canKick = false;
    }

    if(ball.Valid() || fraredOn) ballLast = ballPos;



    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
