#include "StaticGetBall.h"
#include "VisionModule.h"
#include "RobotSensor.h"
#include "skill/Factory.h"
#include "messidecition.h"
#include "DribbleStatus.h"

namespace  {
double PI = PARAM::Math::PI;
int backTime = 0;
int fraredOn = 0;
int fraredOff = 0;
int maxTime = 1024;
CGeoPoint ballLast(0,0);
}


CStaticGetBall::CStaticGetBall():_flytimediff(0),_goBackball(true),_backjud(true),_canKick(false)
{

}

void CStaticGetBall::plan(const CVisionModule *pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        setState(BEGINNING);
        _goBackball = true;
        backTime = 0;
        fraredOn = 0;
        fraredOff = 0;
        _flytimediff = 0;
        _backjud = true;
        _canKick = false;
    }

    int runner = task().executor;
    CGeoPoint target = task().player.pos;
    int flag = task().player.flag;
    float kpower = task().player.kickpower;
    float cpower = task().player.chipkickpower;


    const PlayerVisionT &me = pVision->ourPlayer(runner);
    const MobileVisionT &ball = pVision->ball();
    CGeoPoint ballPos = ball.Valid() ? ball.Pos() : ballLast;
    CVector me2ball = ballPos - me.RawPos();
    CVector me2target = target - me.RawPos();
    CVector ball2target = target - ballPos;

    bool frared = RobotSensor::Instance()->IsInfraredOn(runner);
    if(frared && !ball.Valid()){
        ballPos = me.RawPos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir());
    }

    if(frared){
        fraredOn = fraredOn > maxTime ? maxTime : fraredOn + 1;
        fraredOff = 0;
    } else {
        fraredOff = fraredOff > maxTime ? maxTime : fraredOff + 1;
        fraredOn = 0;
        if(!ball.Valid()){
            ballPos = ballLast ;//+ (ballLast - me.RawPos()).unit() * 20;
            ballLast = ballPos;
        }
    }
    bool isflat = MessiDecision::Instance()->isfreeFlat();
    if(RobotSensor::Instance()->IsKickerOn(runner) > 0){
        if(_flytimediff < 1 && !isflat){
            MessiDecision::Instance()->setisflyTime(true);
            _flytimediff++;
            MessiDecision::Instance()->setfirstChipDir(me.Dir());
            MessiDecision::Instance()->setfirstChipPos(pVision->ball().RawPos());
        }
    } else {
        _flytimediff = 0;
    }

   /**************  set subtask **********************/
   if(_goBackball){
        CGeoPoint gotoPoint = ballPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_SIZE*3,ballPos.x() < 1000 && ballPos.x() > -1000 && ballPos.y() < 1000 && ballPos.y()>-1000 ? PI : (ballPos.x() > 0 ? PI : 0));
        backTime = backTime > maxTime ? maxTime : backTime + 1;
        if((me.Pos() - gotoPoint).mod() < 1.5 * 10 && backTime > PARAM::Vision::FRAME_RATE*2.5){
            _goBackball = false;
        }
        setSubTask(PlayerRole::makeItGoto(runner,gotoPoint,me2ball.dir(),PlayerStatus::ALLOW_DSS | PlayerStatus::DODGE_BALL));
   } else {
        CGeoPoint rollPoint;
        double rollDir;
        if(Utils::Normalize((ballPos - target).dir() - (me.Pos() - ballPos).dir()) < 0){
            rollDir = me2ball.dir() + PI - PI * 2/(4.5*1.4);
        } else {
            rollDir = me2ball.dir() + PI + PI * 2/(4.5*1.4);
        }
        CGeoSegment balltargetLine(ballPos,target);
        CGeoPoint projection = balltargetLine.projection(me.Pos());
        CGeoPoint finalPos = ballPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER+20,me2target.dir() + PI);
        if(_backjud){
            if(balltargetLine.IsPointOnLineOnSegment(projection)){
                rollPoint = ballPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_SIZE*3,rollDir);
            }else{
                if(projection.dist(ballPos) > PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER){
                    rollPoint = finalPos;
                    if(me.Pos().dist(rollPoint) < 1.5*10){
                        _backjud = false;
                    }
                }else{
                    rollPoint = ballPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_SIZE*2,rollDir);
                }
            }
            setSubTask(PlayerRole::makeItGoto(runner,rollPoint,ball2target.dir(),PlayerStatus::ALLOW_DSS));
        }else{
            double taskdir;
            CGeoPoint gotoPoint;
            if(fraredOn > 5){
                taskdir = me2target.dir();
                gotoPoint  = ballPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me2target.dir() + PI);
            }else{
                taskdir = me2ball.dir();
                gotoPoint  = ballPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me2ball.dir() + PI);
            }
            if(fabs(me.Dir() - me2target.dir()) < 5 * PI/180){
                _canKick = true;
            }else{
                _canKick = false;
            }
            setSubTask(PlayerRole::makeItGoto(runner,gotoPoint,taskdir,PlayerStatus::ALLOW_DSS));
        }
   }

   GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,-2000),QString("%1").arg(cpower).toLatin1());
   if(MessiDecision::Instance()->isfreeFlat()){
        if(_canKick) KickStatus::Instance()->setKick(runner,kpower);
   } else {
        if(_canKick && fraredOn > 10) KickStatus::Instance()->setChipKick(runner,cpower==0.0 ? kpower :cpower);
        if(ballPos.dist(me.Pos()) < 120 || frared) DribbleStatus::Instance()->setDribbleCommand(runner,3);
        else DribbleStatus::Instance()->setDribbleCommand(runner,0);
   }
   //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,0),QString("%1").arg(power).toLatin1());
    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,PARAM::Field::PITCH_WIDTH/2+200), QString("%1").arg(RobotSensor::Instance()->fraredOn(runner)).toLatin1(), COLOR_PURPLE);
    _lastCycle = pVision->getCycle(); // last frame
    CStatedTask::plan(pVision);
}

CPlayerCommand* CStaticGetBall::execute(const CVisionModule *pVision)
{
    if(subTask()) return subTask()->execute(pVision);
    return nullptr;
}

