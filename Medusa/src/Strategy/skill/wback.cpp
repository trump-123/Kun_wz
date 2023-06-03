#include "wback.h"
#include "VisionModule.h"
#include "PointCalculation/guardpos.h"
#include "SkillUtils.h"
#include "skill/Factory.h"
#include "SkillUtils.h"
#include "messidecition.h"
#include "TaskMediator.h"
#include "DribbleStatus.h"
#include "RobotSensor.h"
#include "ballmodel.h"

namespace  {
double FRICTION;
bool IS_SIMULATION;
double PASS_ANGLE_ERROR = 2;
double bufferTime = 0.4;
int fraredOn = 0;
int fraredOff = 0;
int maxTime = 1024;
double minRadaius = PARAM::Vehicle::V2::PLAYER_SIZE * 2;
const double MIN_DIST_TO_PENALTY = PARAM::Vehicle::V2::PLAYER_SIZE;
const double DSS_DIST_FROM_PENALTY = PARAM::Vehicle::V2::PLAYER_SIZE*11;
const CGeoPoint OUR_GOAL(-PARAM::Field::PITCH_LENGTH/2,0);
}

CWback::CWback()
    :_lastCycle(0)
    ,_mylastCycle(0)
    ,_flytimediff(0)
{
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION, "Alert/IsSimulation", false);

    if (IS_SIMULATION)
        ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Sim",800);
    else
        ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Real",1520);

}
void CWback::plan(const CVisionModule *pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        setState(BEGINNING);
    }

    int player = task().executor;
    int flag = task().player.flag;
    int index = task().player.rotdir; // direction of rtation
    int guardNum = task().player.kick_flag;
    int defendNum = task().ball.Sender;

    int bestEnemyNum = ZSkillUtils::instance()->getTheirBestPlayer();
    const PlayerVisionT& bestEnemy = pVision->theirPlayer(bestEnemyNum);
    const PlayerVisionT& defendEnemy = pVision->theirPlayer(defendNum);
    const MobileVisionT& ball = pVision->ball();
    const PlayerVisionT& me = pVision->ourPlayer(player);

    bool frared = RobotSensor::Instance()->IsInfraredOn(player);
    if(frared){
        fraredOn = fraredOn > maxTime ? maxTime : fraredOn + 1;
        fraredOff = 0;
    } else {
        fraredOff = fraredOff > maxTime ? maxTime : fraredOff + 1;
        fraredOn = 0;
    }


    TaskT GuardTask(task());
    CGeoPoint guardPos = GuardPos::Instance()->backPos(guardNum, index, player, -1); // KE avoid nil value

    if(defendNum<0){
        guardPos = GuardPos::Instance()->backPos(guardNum, index, player, -1);
    }
    else{
        guardPos = GuardPos::Instance()->backPos(guardNum, index, player, defendNum);
        GuardTask.player.angle = (defendEnemy.Pos() - me.Pos()).dir();
    }

    if (Utils::InOurPenaltyArea(ball.Pos(), MIN_DIST_TO_PENALTY) || !ball.Valid()) {
        // Ball in penalty area // L
        GuardTask.player.angle = (bestEnemy.Pos() - OUR_GOAL).dir();
    }
    else {
        GuardTask.player.angle = (ball.Pos() - OUR_GOAL).dir();
    }
    CGeoPoint interPos = ZSkillUtils::instance()->getOurInterPoint(player);
    double interTime  = ZSkillUtils::instance()->getOurInterTime(player);
    double bestInterTime = 99999;
    double bestTheirInterTime = 99999;
    double bestInterNum = -1;
    for(int i = 0;i<PARAM::Field::MAX_PLAYER;++i){
        if(ZSkillUtils::instance()->getTheirInterTime(i) < bestTheirInterTime){
            bestTheirInterTime = ZSkillUtils::instance()->getTheirInterTime(i);
        }
        const PlayerVisionT&me = pVision->ourPlayer(i);
        if(!me.Valid()||TaskMediator::Instance()->isGoalie(i)) continue;
        if(ZSkillUtils::instance()->getOurInterTime(i) < bestInterTime){
            bestInterTime = ZSkillUtils::instance()->getOurInterTime(i);
            bestInterNum = i;
        }
    }

    bool isflat = true;

    double chippower = BallModel::instance()->chipPassVel(pVision, MessiDecision::Instance()->passPos());
    double flatpower = BallModel::instance()->flatPassVel(pVision, MessiDecision::Instance()->passPos(),MessiDecision::Instance()->receiverNum(),bufferTime,PASS_ANGLE_ERROR);

    //judge flat or chip
    isflat = MessiDecision::Instance()->judgePassModule(player,MessiDecision::Instance()->passPos());
    if(RobotSensor::Instance()->IsKickerOn(player) > 0){
        if(_flytimediff < 1 && !isflat){
            _flytimediff++;
            MessiDecision::Instance()->setisflyTime(true);
            MessiDecision::Instance()->setfirstChipDir(me.Dir());
            MessiDecision::Instance()->setfirstChipPos(pVision->ball().RawPos());
        }
    } else {
        _flytimediff = 0;
    }

    double inVel;
    CVector me2ball = ball.Pos() - me.RawPos();
    if(pVision->ball().Vel().mod2() > FRICTION * me2ball.mod())
        inVel = sqrt(pVision->ball().Vel().mod2() - FRICTION * me2ball.mod());
    else
        inVel = pVision->ball().Vel().mod();

    double beta = Utils::Normalize(pVision->ball().Vel().dir() + PARAM::Math::PI - (MessiDecision::Instance()->passPos() - me.RawPos()).dir());
    double alpha = atan2((MessiDecision::Instance()->passPos() - me.RawPos()).mod() * sin(beta) / 0.42 + 1e-5, inVel + (MessiDecision::Instance()->passPos() - me.RawPos()).mod() * cos(beta) / 0.42 + 1e-5);

    float bestdist = 99999;
    for(int i = 0; i < PARAM::Field::MAX_PLAYER;++i){
        if(pVision->theirPlayer(i).Pos().dist(ball.Pos()) < bestdist){
            bestdist = pVision->theirPlayer(i).Pos().dist(ball.Pos());
        }
    }

    CGeoPoint theirbestPos = pVision->theirPlayer(ZSkillUtils::instance()->getTheirBestPlayer()).Pos();
//    if(!Utils::InOurPenaltyArea(ball.Pos(),PARAM::Vehicle::V2::PLAYER_SIZE*1.5) && Utils::InOurPenaltyArea(ball.Pos(),PARAM::Vehicle::V2::PLAYER_SIZE*11)  && bestdist > 70 * 10 && !Utils::InOurPenaltyArea(pVision->ourPlayer(MessiDecision::Instance()->leaderNum()).Pos(),PARAM::Vehicle::V2::PLAYER_SIZE*11) && ball.Vel().mod() < 100*10 && index ==1){
//        flag |= PlayerStatus::NOT_AVOID_PENALTY;

//        bool passdirjud = (isflat ? (fabs((MessiDecision::Instance()->passPos() - me.Pos()).dir() - alpha + beta - (ball.Pos() - me.Pos()).dir()) < PARAM::Math::PI/3) : (fabs((ball.Pos() - me.Pos()).dir() - (MessiDecision::Instance()->passPos() - me.Pos()).dir())< PARAM::Math::PI/3))&&ball.Vel().mod()>650;
//        if(passdirjud){
//            if(!isflat){
//                GuardTask.player.angle =(MessiDecision::Instance()->passPos() - me.Pos()).dir() /*- alpha + beta*/;
//                KickStatus::Instance()->setChipKick(player,chippower);
//            } else {
//                GuardTask.player.angle =(MessiDecision::Instance()->passPos() - me.Pos()).dir() - alpha + beta;
//                KickStatus::Instance()->setKick(player,flatpower);
//            }
//            GuardTask.player.pos = interPos + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,GuardTask.player.angle + PARAM::Math::PI);
//        }
//        else{
//            GuardTask.player.angle =(ball.Pos() - me.RawPos()).dir();
//            if((ball.Pos() - me.Pos()).mod() < 50*10){
//                DribbleStatus::Instance()->setDribbleCommand(player,3);
//            }
////            GuardTask.player.pos = interPos;
////        }
//        if(fraredOn > 10){
//            DribbleStatus::Instance()->setDribbleCommand(player,3);
//            GuardTask.player.angle =(MessiDecision::Instance()->passPos() - me.Pos()).dir();
//            if(fabs(me.Dir() - GuardTask.player.angle) < PARAM::Math::PI/10){
//                //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(3000,0),QString("%1").arg(pVision->getCycle() - _mylastCycle).toLatin1());
//                if(pVision->getCycle() - _mylastCycle > 5){
//                    if(!isflat){
//                        //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(3000,0),QString("%1").arg(1).toLatin1());
//                        KickStatus::Instance()->setChipKick(player,chippower);
//                    } else {
//                        //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(3000,0),QString("%1").arg(2).toLatin1());
//                        KickStatus::Instance()->setKick(player,flatpower);
//                    }
//                }
//            } else _mylastCycle = pVision->getCycle();
//            GuardTask.player.pos = ball.Pos() + Utils::Polar2Vector(-90,GuardTask.player.angle);
//        } else {
//            GuardTask.player.angle = me2ball.dir();
//            GuardTask.player.pos = ball.Pos(); //+ Utils::Polar2Vector(-90,GuardTask.player.angle);
//        }
//    }
//    else
    GuardTask.player.pos = guardPos;
    GuardTask.player.flag = flag;
    if (!Utils::InOurPenaltyArea(me.Pos(), DSS_DIST_FROM_PENALTY)) GuardTask.player.flag = PlayerStatus::ALLOW_DSS;

    setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(GuardTask));
    _lastCycle = pVision->getCycle(); // last frame
    CStatedTask::plan(pVision);
}

CPlayerCommand* CWback::execute(const CVisionModule *pVision)
{
    if(subTask()) return subTask()->execute(pVision);
    return nullptr;
}
