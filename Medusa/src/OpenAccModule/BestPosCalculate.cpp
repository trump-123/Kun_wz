#include "BestPosCalculate.h"
#include <stdio.h>
#include "geometry.h"
#include "parammanager.h"
#include "CMmotion.h"
#include "SkillUtils.h"
#include "Semaphore.h"
#include "passposevaluate.h"
#include <thread>
#include <QThread>
#include "ThreadPool.h"
#include "TaskMediator.h"
//Semaphore Getbest(4);



#define PLAYER_SIZE (90.0)
#define STEP_TIME (0.2)
//---CUDA參數---
#define PUSH_CYCLE (15)
#define STEP_NUM (10)
#define PLAYER_NUM (16)
#define SAMPLE_NUM (17)
#define ANGLE_NUM (12)
#define DIST_NUM (8)
#define STEP_DIST (175)
#define FOOT_DIST (350)
#define SAFE_DIST (3.9*PLAYER_SIZE)
#define IGNORE_DIST (4*PLAYER_SIZE)
#define IGNORE_GOALIE (1)
#define IGNORE_GUARD (1)

#define OUR_PREDICT_BUFFER_TIME (0.0)

//摩擦參數
#define ROLLING_FRACTION_REAL (800)
#define ROLLING_FRACTION_SIM (1520)
#define SLIDING_FRACTION_REAL (ROLLING_FRACTION_REAL * 15.0)
#define SLIDING_FRACTION_SIM (ROLLING_FRACTION_SIM * 15.0)
#define FLAT_ROLL_RATIO (5.0/7.0)
#define FLAT_SLIDE_RATIO (2.0/7.0)

#define MAX_FLAT_VEL (5000.0)
#define MIN_FLAT_VEL (2000.0)
#define MIN_PASS_DIST (1500)

namespace  {
    //std::thread *getbest;
    double FRICTION;
    bool IS_SIMULATION;
    int buffer_time = 0;
    double PI = PARAM::Math::PI;
    double calculateFlatVel(double distance, double interTime, bool isSim){
           double ballSlidAcc = (isSim ? SLIDING_FRACTION_SIM : SLIDING_FRACTION_REAL) / 2;
           double ballRollAcc = (isSim ? ROLLING_FRACTION_SIM : ROLLING_FRACTION_REAL) / 2;
           const double minPassVel = sqrt(2 * ballRollAcc * distance);
           double passVel = max((distance + 1.0/2.0 * ballRollAcc * interTime * interTime) / interTime, minPassVel);
           passVel /= FLAT_ROLL_RATIO;
           return passVel;
    }

    bool IsOutOfLimit(const double value, const double upLimit, const double lowLimit, const double range){
        return (value > upLimit*(1-range) || value < lowLimit*(1+range));
    }

    double flatSlideTime(double startVel,double slideAcc){
        if(startVel < 0.0)
            return 0.0;
        return (startVel * FLAT_SLIDE_RATIO/slideAcc);
    }

    double flatRollTime(double startVel,double RollAcc){
        if(startVel < 0.0)
            return 0.0;
        return (startVel * FLAT_ROLL_RATIO/RollAcc);
    }

    double flatRollDist(double startVel, double rollAcc){
        if(startVel < 0.0)
            return 0.0;
        return (pow(startVel * FLAT_ROLL_RATIO,2.0) / (2.0 * rollAcc));
    }

    double flatSlideDist(double startVel, double slideAcc) {
        if(startVel < 0.0)
            return 0.0;
        return (pow(startVel,2.0) - pow(startVel * FLAT_ROLL_RATIO,2.0)) / (2.0 * slideAcc);
    }

    double flatStopTime(double startVel,double slideAcc,double rollAcc){
        return flatSlideTime(startVel,slideAcc) + flatRollTime(startVel,rollAcc);
    }

    double flatMoveDist(double startVel,double time, double slideacc ,double rollacc){
        double moveDist = 0;
        double slidingTime = flatSlideTime(startVel,slideacc);
        double totalTime = flatStopTime(startVel,slideacc,rollacc);
        if(time > totalTime)
            moveDist = flatRollDist(startVel,rollacc) + flatSlideDist(startVel,slideacc);
        else if(time < slidingTime)
            moveDist = startVel * time - 0.5 * slideacc * time *time;
        else{
            double slideDist = (pow(startVel,2.0) - pow(startVel*FLAT_ROLL_RATIO, 2.0))/ (2.0 * slideacc);
            double rollStartVel = startVel * FLAT_ROLL_RATIO;
            double rollTime = time - slidingTime;
            double rollDist = rollStartVel * rollTime - 0.5 * rollacc * pow(rollTime,2.0);
            moveDist = slideDist + rollDist;
        }

        return moveDist;
    }

    bool flatSecurityCheck(const CVisionModule *pVision,int robotNum, CGeoPoint target,CVector ballFlatVel,
                           int enemyRole, double responseTime,
                           double ignoreCloseEnemyDist, bool ignoreTheirGoalie, bool ignoreTheirGuard,
                           bool isSim){
        const PlayerVisionT& me = pVision->theirPlayer(robotNum);
        const MobileVisionT& ball = pVision->ball();
        CGeoPoint passLine;
        double ballSpeed = ballFlatVel.mod();
        double passLineDist = (ball.Pos() - target).mod();
        double passLineDir  = (ball.Pos() - target).dir();
        double ballRollAcc = (isSim ? ROLLING_FRACTION_SIM : ROLLING_FRACTION_REAL) / 2;
        double ballSlideAcc = (isSim ? SLIDING_FRACTION_SIM : SLIDING_FRACTION_REAL) / 2;
        if(me.Pos().dist(ball.Pos()) < ignoreCloseEnemyDist && !Utils::InTheirPenaltyArea(me.Pos(),0) && !Utils::InOurPenaltyArea(me.Pos(),0)){
            return true;
        }


        //判断敌方离截球点很近
        CGeoLine balltotarget(ball.Pos(),target);
        CGeoPoint pedal = balltotarget.projection(me.Pos());
        if(pedal.x() > fmin(ball.Pos().x(),target.x()) && pedal.x() < fmax(ball.Pos().x(),target.x())){
            if(me.Pos().dist(pedal) < SAFE_DIST){
                return false;
            }
        } else {
            //判断敌方离我或者目标点很近
            if(fmin(me.Pos().dist(ball.Pos()), me.Pos().dist(target)) < SAFE_DIST) return false;
        }


        if(enemyRole == 1 && ignoreTheirGoalie) return true;
        if(enemyRole == 2 && ignoreTheirGuard) return true;
        double ballMoveTime = 0;
        while(true){
            ballMoveTime += STEP_TIME;
            double ballMoveDist = flatMoveDist(ballSpeed,ballMoveTime,ballSlideAcc,ballRollAcc);
            CGeoPoint ballPos(ball.Pos() + Utils::Polar2Vector(ballMoveDist,passLineDir));
            if(!Utils::IsInField(ballPos,300) || ballMoveDist > passLineDist)
                return true;
            double adjustDir = (me.Pos() - ballPos).dir();
            CGeoPoint testPoint(ballPos + Utils::Polar2Vector(SAFE_DIST,adjustDir));
            double enemyTime = predictedTime(me,testPoint);
            if(enemyTime + responseTime - ballMoveTime < 0)
                return  false;
        }
        return true;
    }

    bool toposSecurity(const CVisionModule *pVision,CGeoPoint candidate,int robotNum){
        const PlayerVisionT &me = pVision->ourPlayer(robotNum);
        const MobileVisionT &ball = pVision->ball();
        if(!Utils::IsInFieldV2(candidate,300)||candidate.dist(ball.Pos()) < MIN_PASS_DIST) return false;
        for(int i = 0;i < PARAM::Field::MAX_PLAYER;i++){
            const PlayerVisionT &enemy = pVision->theirPlayer(i);
            if(!enemy.Valid()) continue;
            CGeoSegment me2candidate(candidate,me.Pos());
            if(me2candidate.IsPointOnLineOnSegment(me2candidate.projection(enemy.Pos()))){
                if(enemy.Pos().dist(me2candidate.projection(enemy.Pos())) < SAFE_DIST)
                    return  false;
            }else{
                if(enemy.Pos().dist(candidate) < SAFE_DIST)
                    return false;
            }
        }
        return true;
    }

}

//extern {
        //extern int test();
//}

BestPosCalculate::BestPosCalculate():pool(22)
{
    //_pVision = new CVisionModule;
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION,"Alert/IsSimmulation",false);
    if (IS_SIMULATION)
        ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Sim",800);
    else
        ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Real",1520);
    //ThreadPool1::Instance()->enqui(20);
     //pool = ThreadPool(22);
}

BestPosCalculate::~BestPosCalculate(void){
        //this->t1.join();
        //t2.join();
        //t3.join();
        //t4.join();
}

void BestPosCalculate::initialize(const CVisionModule *pVision){
    this->_pVision = pVision;
}



//void BestPosCalculate::calculateAllInterInfoV1(int start,int end,const CVisionModule *pVision,Player *players, CGeoPoint *ballPos, nType *bestPass, int leader, double theirFlatResponseTime,bool isSim,CGeoPoint &bestflatpos,int &flatpassnum,float &bestflatpassq,float &bestflatvel){
void BestPosCalculate::calculateAllInterInfoV1(int start,int end,int index,const CVisionModule *pVision,Player *players, CGeoPoint *ballPos, nType *bestPass, int leader, double theirFlatResponseTime,bool isSim){

    bool canInter;
    float interTime = 0.0;
    Point interPoint;

    int catcherIndex = 0;//PLAYER_NUM;//blockidx.x
    int angleIndex = 0;//ANGLE_NUM;//blockidx.y
    int totalIndex = 0;
    int distanceIndex = 0;//PLAYER_NUM * DIST_NUM / PLAYER_NUM; //threadidx /playernum
    //int blockerIndex  = 0;//PLAYER_NUM * DIST_NUM % PLAYER_NUM; //threadidx %playernum
    bool flatSecurity = true;

    CGeoPoint catchPoint;
    CGeoPoint testPoint;
    int TOTAL_ANGLE = ANGLE_NUM;

    for(catcherIndex = start; catcherIndex <= end ; catcherIndex ++) {
        if(!players[catcherIndex].isValid || catcherIndex == leader) continue;
        //GDebugEngine::Instance()->gui_debug_arc(players[catcherIndex].Pos,1800,0,360,COLOR_PURPLE);
        for(totalIndex = (index == 0 ? 0 : DIST_NUM/2); totalIndex <= (index == 0 ? DIST_NUM/2 : DIST_NUM); totalIndex++){
//            if(totalIndex<=1)
//                TOTAL_ANGLE = ANGLE_NUM;
//            else if(totalIndex > 1 && totalIndex < 4)
//                TOTAL_ANGLE = ANGLE_NUM * 2;
//            else if(totalIndex > 3 && totalIndex < 7)
//                TOTAL_ANGLE = ANGLE_NUM * 3;
//            else if(totalIndex >= 7)
//                TOTAL_ANGLE = ANGLE_NUM * 4;
            for(angleIndex = 0; angleIndex <= TOTAL_ANGLE; angleIndex++){
                std::lock_guard<std::mutex> mtx_locker(mtx);
                distanceIndex = totalIndex;
                //blockerIndex  = totalIndex%PLAYER_NUM;

                catchPoint = players[catcherIndex].Pos + Utils::Polar2Vector((distanceIndex + 2) * STEP_DIST,2.0 * PI * angleIndex / TOTAL_ANGLE);
                if(!Utils::IsInField(catchPoint,600))
                    catchPoint = Utils::MakeInField(catchPoint,600);
                if(Utils::InOurPenaltyArea(catchPoint,300))
                    catchPoint = Utils::MakeOutOfOurPenaltyArea(catchPoint,300);
                if(Utils::InTheirPenaltyArea(catchPoint,420))
                    catchPoint = Utils::MakeOutOfTheirPenaltyArea(catchPoint,420);

//                if(!Utils::IsInFieldV2(catchPoint,300)){
//                    flatSecurity = false;
//                    continue;
//                }

                /////////////针对自传//////////////////////////////////
//                if(catcherIndex == leader && distanceIndex != 2)
//                    flatSecurity = false;
//                if(catcherIndex == leader && blockerIndex != leader && (players[blockerIndex].Pos.dist(catchPoint)) < 900)
//                    flatSecurity = false;
                /////////////////////////////////////////////////////

                double catchTime = predictedTime(players[catcherIndex].me,catchPoint);
                double ballLineDist = catchPoint.dist(*ballPos);
                double ballLineAngle = (catchPoint - *ballPos).dir();
                CVector ballflatVel;

                double ballFlatSpeed = calculateFlatVel(ballLineDist,catchTime + OUR_PREDICT_BUFFER_TIME,isSim);
                //float ball2targetDir = atan2();
                if(players[catcherIndex].isValid && IsOutOfLimit(ballFlatSpeed,MAX_FLAT_VEL,MIN_FLAT_VEL,0.05)) {
                    if(ballFlatSpeed > MAX_FLAT_VEL) {
                        ballFlatSpeed = MAX_FLAT_VEL;
                        ballflatVel = CVector(ballFlatSpeed * cos(ballLineAngle),ballFlatSpeed * sin(ballLineAngle));
                        ZSkillUtils::instance()->predictedInterTime(pVision, catcherIndex, catchPoint ,catchTime);
                    }
                }
                ballflatVel = CVector(ballFlatSpeed * cos(ballLineAngle),ballFlatSpeed * sin(ballLineAngle));


                int enemyRole = 0;
              for(int i = 0;i<PARAM::Field::MAX_PLAYER;++i){
                const PlayerVisionT&enemy = pVision->theirPlayer(i);
                if(!enemy.Valid()) continue;
                if(Utils::InTheirPenaltyArea(players[i  + PLAYER_NUM].Pos,0))
                    enemyRole = 1;
                else
                    if(Utils::InTheirPenaltyArea(players[i + PLAYER_NUM].Pos,300))
                    enemyRole = 2;

                if(players[i + PLAYER_NUM].isValid && flatSecurity) {
                    flatSecurity =  flatSecurityCheck(pVision,i,catchPoint,ballflatVel,enemyRole,
                                                      theirFlatResponseTime,IGNORE_DIST,IGNORE_GOALIE,IGNORE_GUARD,isSim);
                }
              }
              //GDebugEngine::Instance()->gui_debug_arc(players[catcherIndex].Pos,1770,0,360,COLOR_PURPLE);

              int posIdx = catcherIndex * TOTAL_ANGLE * ANGLE_NUM + distanceIndex * TOTAL_ANGLE + angleIndex;
             // if(toposSecurity(pVision,catchPoint,catcherIndex)){
                  //GDebugEngine::Instance()->gui_debug_arc(catchPoint,40,0,360,COLOR_PURPLE);
                  if(catchTime < 10 && catchTime > 0 && !ZPassPosEvaluate::instance()->passTooClose(catchPoint,ZGetBestUtils::Instance()->getLeaderPos())){
                      //CGeoPoint candidate(result[posIdx].interPos.x(),result[posIdx].interPos.y()),p1(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH /2) , p2(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH/2)
                      double interTime = catchTime;
                      //if(passValid[i][0] && )
                      std::vector<float> passScores = ZPassPosEvaluate::instance()->evaluateFunc(catchPoint,pVision->ourPlayer(leader).Pos(),PASS);
                      std::vector<float> shootScores = ZPassPosEvaluate::instance()->evaluateFunc(catchPoint,pVision->ourPlayer(leader).Pos(),SHOOT);
                      std::vector<float> freeScores = ZPassPosEvaluate::instance()->evaluateFunc(catchPoint,pVision->ourPlayer(leader).Pos(),FREE_KICK);
                      float passScore  = passScores.front();
                      float shootScore = shootScores.front();
                      float freeScore  = freeScores.front();
                      //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-2000,0),QString::number(1).toLatin1(),COLOR_RED);
                      if(passScore > ZGetBestUtils::Instance()->getBestFlatPassQ()) {
                          ZGetBestUtils::Instance()->setBestFlatPass(catchPoint);
                          ZGetBestUtils::Instance()->setBestChipPass(catchPoint);
                          ZGetBestUtils::Instance()->setBestChipPassQ(catchTime);
                          ZGetBestUtils::Instance()->setBestFlatPassQ(catchTime);
                          ZGetBestUtils::Instance()->setBestFlatPassNum(catcherIndex);
                          ZGetBestUtils::Instance()->setBestChipPassNum(catcherIndex);
                      }

                      if(shootScore > ZGetBestUtils::Instance()->getBestFlatShootQ()) {
                          ZGetBestUtils::Instance()->setBestFlatShoot(catchPoint);
                          ZGetBestUtils::Instance()->setBestChipShoot(catchPoint);
                          ZGetBestUtils::Instance()->setBestChipShootQ(catchTime);
                          ZGetBestUtils::Instance()->setBestFlatShootQ(catchTime);
                          ZGetBestUtils::Instance()->setBestFlatShootNum(catcherIndex);
                          ZGetBestUtils::Instance()->setBestChipShootNum(catcherIndex);
                      }

                      if(freeScore > ZGetBestUtils::Instance()->getBestFreeKickQ()) {
                          ZGetBestUtils::Instance()->setBestFreeKickPos(catchPoint);
                          ZGetBestUtils::Instance()->setBestFreeKickQ(catchTime);
                          ZGetBestUtils::Instance()->setBestFreePassNum(catcherIndex);
                      }

                  }
              //}
//              bestPass[posIdx].interPos = catchPoint;
//              bestPass[posIdx].interTime = catchTime;
//              bestPass[posIdx].playerIndex = catcherIndex;
//              bestPass[posIdx].dir = ballLineAngle;
//              bestPass[posIdx].Vel = flatSecurity?ballFlatSpeed:0;
//              bestPass[posIdx].isValid = flatSecurity;
//              len++;

//                int offset = catcherIndex * ANGLE_NUM * DIST_NUM  +  distanceIndex * ANGLE_NUM  + angleIndex;

//                if(toposSecurity(pVision,catchPoint,catcherIndex)){
//                    bestPass[offset].interPos  = catchPoint;
//                    bestPass[offset].interTime = catchTime;
//                    bestPass[offset].playerIndex = catcherIndex;
//                    bestPass[offset].dir = ballLineAngle;
//                    bestPass[offset].Vel = flatSecurity?ballFlatSpeed:0;
//                    bestPass[offset].isValid = flatSecurity;
//                }else{
//                    bestPass[offset].interPos  = CGeoPoint(99999,99999);
//                    bestPass[offset].interTime = 99999;
//                    bestPass[offset].playerIndex = 99999;
//                    bestPass[offset].dir = 999999;
//                    bestPass[offset].Vel = flatSecurity?ballFlatSpeed:0;
//                    bestPass[offset].isValid = flatSecurity;
//                }

            //    offset += PLAYER_NUM * ANGLE_NUM * DIST_NUM *PLAYER_NUM;

            //    bestPass[offset].interPos = catchPoint;
            //    bestPass[offset].interTime = catchTime;
            //    bestPass[offset].playerIndex = catcherIndex;
            //    bestPass[offset].dir = ballLineAngle;
            //    bestPass[offset].Vel = flatSecurity? ballFlatSpeed: 0;
            //    bestPass[offset].isValid = chipSecurity;
            }
            //ANGLE_NUM = ANGLE_NUM * distanceIndex;
            //TOTAL_ANGLE = ANGLE_NUM * distanceIndex;
        }
    }

}

//void BestPosCalculate::getBestInfo(const CVisionModule *pVision,nType *bestPass,int leader,CGeoPoint &bestflatpos,int &flatpassnum,float &bestflatpassq,float &bestflatvel){
//        CGeoPoint catchPoint;
//        int catcherIndex = -1;
//        double catchTime = 99999;
//        if(buffer_time==0)
//        {
//            for(int i = 0;i< PLAYER_NUM * DIST_NUM * ANGLE_NUM /* PLAYER_NUM*/;++i){
//                catchTime = bestPass[i].interTime;
//                catchPoint= bestPass[i].interPos;
//                catcherIndex = bestPass[i].playerIndex;
//                if(toposSecurity(pVision,catchPoint,catcherIndex)){
//                  //GDebugEngine::Instance()->gui_debug_arc(catchPoint,40,0,360,COLOR_PURPLE);
//                  if(catchTime < 10 && catchTime > 0){
//                      //CGeoPoint candidate(result[posIdx].interPos.x(),result[posIdx].interPos.y()),p1(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH /2) , p2(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH/2)
//                      double interTime = catchTime;
//                      //if(passValid[i][0] && )
//                      std::vector<float> passScores = ZPassPosEvaluate::instance()->evaluateFunc(catchPoint,ZGetBestUtils::Instance()->getLeaderPos(),PASS);
//                      float passScore = passScores.front();
//                      //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-2000,0),QString::number(1).toLatin1(),COLOR_RED);
//                      if(passScore > bestflatpassq) {
//                          bestflatpassq = passScore;
//                          bestflatpos = catchPoint;
//                          flatpassnum = catcherIndex;
//                          //bestflatvel = flatSecurity?ballFlatSpeed:0;
//                      }
//                      // 更新最佳平射射门点
//                      std::vector<float> shootScores = ZPassPosEvaluate::instance()->evaluateFunc(catchPoint,ZGetBestUtils::Instance()->getLeaderPos(),SHOOT);
//                      float shootScore = shootScores.front();
//                      if(shootScore > bestflatpassq){
//                          bestflatpassq = shootScore;
//                          bestflatpos = catchPoint;
//                          flatpassnum = catcherIndex;
//                          //bestFlatShootVel = result[i * PLAYER_NUM].Vel;
//                      }
//                  }
//              }
//            }
//        } else {
//            buffer_time++;
//            if(buffer_time==30) buffer_time = 0;
//        }
//}


void BestPosCalculate::BestPass(const CVisionModule *pVision,Player *players, CGeoPoint *ball, nType *result, int leader, double theirFlatresponseTime,bool isSim){
    //nType *pointInfor = new nType [2*PLAYER_NUM*ANGLE_NUM*DIST_NUM*PLAYER_NUM];
    //pointInfor = (nType*)malloc(2*PLAYER_NUM*ANGLE_NUM*DIST_NUM*PLAYER_NUM*sizeof (nType));

    //int threadnum = 4;
//       len = 0;
    //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(1000,0),QString("%1").arg(TaskMediator::Instance()->isBack(1)).toLatin1());
         for(int i = 0;i < PARAM::Field::MAX_PLAYER; ++i){      
             if(_pVision->ourPlayer(i).Valid() && !TaskMediator::Instance()->isBack(i) && !TaskMediator::Instance()->isGoalie(i) && i!=leader && !Utils::InOurPenaltyArea(pVision->ourPlayer(i).Pos(),PLAYER_SIZE * 6)){
                 for(int j = 0;j < 2;j++){
                        results.emplace_back(
                                    pool.enqueue([this,i,j,pVision,players,ball,result,leader,theirFlatresponseTime,isSim]{
                                     calculateAllInterInfoV1(i,i,j,pVision,players,ball,result,leader,theirFlatresponseTime,isSim);
                                     //return 0;
                                    })
                                    );
                 }
             }
         }

//       results.emplace_back(
//                   pool.enqueue([this,pVision,players,ball,result,leader,theirFlatresponseTime,isSim]{
//                    calculateAllInterInfoV1(0,1,pVision,players,ball,result,leader,theirFlatresponseTime,isSim);
//                    return 0;
//                   })
//                   );
//       results.emplace_back(
//                   pool.enqueue([this,pVision,players,ball,result,leader,theirFlatresponseTime,isSim]{
//                    calculateAllInterInfoV1(2,3,pVision,players,ball,result,leader,theirFlatresponseTime,isSim);
//                    return 0;
//                   })
//                   );
//       results.emplace_back(
//                   pool.enqueue([this,pVision,players,ball,result,leader,theirFlatresponseTime,isSim]{
//                    calculateAllInterInfoV1(4,5,pVision,players,ball,result,leader,theirFlatresponseTime,isSim);
//                    return 0;
//                   })
//                   );
//       results.emplace_back(
//                   pool.enqueue([this,pVision,players,ball,result,leader,theirFlatresponseTime,isSim]{
//                    calculateAllInterInfoV1(6,7,pVision,players,ball,result,leader,theirFlatresponseTime,isSim);
//                    return 0;
//                   })
//                   );
//       results.emplace_back(
//                   pool.enqueue([this,pVision,players,ball,result,leader,theirFlatresponseTime,isSim]{
//                    calculateAllInterInfoV1(8,9,pVision,players,ball,result,leader,theirFlatresponseTime,isSim);
//                    return 0;
//                   })
//                   );
//       results.emplace_back(
//                   pool.enqueue([this,pVision,players,ball,result,leader,theirFlatresponseTime,isSim]{
//                    calculateAllInterInfoV1(10,11,pVision,players,ball,result,leader,theirFlatresponseTime,isSim);
//                    return 0;
//                   })
//                   );
//       results.emplace_back(
//                   pool.enqueue([this,pVision,players,ball,result,leader,theirFlatresponseTime,isSim]{
//                    calculateAllInterInfoV1(12,13,pVision,players,ball,result,leader,theirFlatresponseTime,isSim);
//                    return 0;
//                   })
//                   );
//       results.emplace_back(
//                   pool.enqueue([this,pVision,players,ball,result,leader,theirFlatresponseTime,isSim]{
//                    calculateAllInterInfoV1(14,15,pVision,players,ball,result,leader,theirFlatresponseTime,isSim);
//                    return 0;
//                   })
//                   );
//       for(auto &&result:results)
//           result.get();

//       results.emplace_back(
//                   pool.enqueue([this,pVision,result,leader,&bestflatpos,&flatpassnum,&bestflatpassq,&bestflatvel]{
                    //getBestInfo(pVision,result,leader,bestflatpos,flatpassnum,bestflatpassq,bestflatvel);
//                    return 0;
//                   })
//                   );


    //calculateAllInterInfoV1(0,15,pVision,players,ball,result,leader,theirFlatresponseTime,isSim,std::ref(bestflatpos),std::ref(flatpassnum),std::ref(bestflatpassq),std::ref(bestflatvel));
//       t1 = thread(&BestPosCalculate::calculateAllInterInfoV1,this,0,3,pVision,players,ball,result,leader,theirFlatresponseTime,isSim,std::ref(bestflatpos),std::ref(flatpassnum),std::ref(bestflatpassq),std::ref(bestflatvel));
//       t2 = thread(&BestPosCalculate::calculateAllInterInfoV1,this,4,7,pVision,players,ball,result,leader,theirFlatresponseTime,isSim,std::ref(bestflatpos),std::ref(flatpassnum),std::ref(bestflatpassq),std::ref(bestflatvel));
//       t3 = thread(&BestPosCalculate::calculateAllInterInfoV1,this,8,11,pVision,players,ball,result,leader,theirFlatresponseTime,isSim,std::ref(bestflatpos),std::ref(flatpassnum),std::ref(bestflatpassq),std::ref(bestflatvel));
//       t4 = thread(&BestPosCalculate::calculateAllInterInfoV1,this,12,15,pVision,players,ball,result,leader,theirFlatresponseTime,isSim,std::ref(bestflatpos),std::ref(flatpassnum),std::ref(bestflatpassq),std::ref(bestflatvel));
//    thread t3 = thread(&BestPosCalculate::calculateAllInterInfoV1,this,8,11,pVision,players,ball,result,leader,theirFlatresponseTime,isSim,std::ref(bestflatpos),std::ref(flatpassnum),std::ref(bestflatpassq),std::ref(bestflatvel));
//    thread t4 = thread(&BestPosCalculate::calculateAllInterInfoV1,this,12,15,pVision,players,ball,result,leader,theirFlatresponseTime,isSim,std::ref(bestflatpos),std::ref(flatpassnum),std::ref(bestflatpassq),std::ref(bestflatvel));
//       t1.join();
//       t2.join();
//       t3.join();
//       t4.join();

//       getbest = new thread([=]{
//           while(true){
//               Getbest.Wait();
//               calculateAllInterInfoV1(0,15,pVision,players,ball,result,leader,theirFlatresponseTime,isSim,std::ref(bestflatpos),std::ref(flatpassnum),std::ref(bestflatpassq),std::ref(bestflatvel));

//           }
//       });




    //CGeoPoint blocks(PLAYER_NUM, ANGLE_NUM);

//    for(int i = 0;i < 2*PLAYER_NUM * ANGLE_NUM * DIST_NUM * PLAYER_NUM; ++i)
//        result[i] = pointInfor[i];

//    if(pointInfor!=NULL){
//        free(pointInfor);
//        pointInfor = NULL;
//    }
}
