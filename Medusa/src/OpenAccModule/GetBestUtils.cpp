#include "GetBestUtils.h"
#include "parammanager.h"
#include "BestPosCalculate.h"
#include "passposevaluate.h"
#include "GDebugEngine.h"



#define PLAYER_NUM (16)
#define SAMPLE_NUM (17)
#define ANGLE_NUM (12)
#define DIST_NUM (8)
#define STEP_NUM (10)
#define FOOT_DIST (350)
#define PUSH_CYCLE (15)

namespace {
    bool IS_SIMULATION = true;
    // 摩擦系数
    double ROLLING_FRICTION = 40;
    double SLIDING_FRICTION = ROLLING_FRICTION * 15;
    double THEIR_RESPONSE_TIME = 0.0;
}

GetBestUtils::GetBestUtils()
     : bestFlatPassPos(CGeoPoint(99999,99999))
     , bestFlatShootPos(CGeoPoint(99999,99999))
     , bestFlatPassQ(-99999)
     , bestFlatShootQ(-99999)
     , bestFlatPassNum(-99999)
     , bestFlatShootNum(-99999)
     , bestFreePassNum(-99999)
     , bestFlatShootVel(0)
     , bestFlatPassVel(0)
     , leader(1)
     , receiver(2)
     , passParameters({0})
     , pVision(nullptr) {
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION,"Alert/IsSimulation",false);
    players = new Player [2*PLAYER_NUM];
    ball = new CGeoPoint;
    rollingFraction = new float;
    slidingFraction = new float;

    if(IS_SIMULATION)
        ZSS::ZParamManager::instance()->loadParam(ROLLING_FRICTION,"AlertParam/Friction4Sim",400.0);
    else
        ZSS::ZParamManager::instance()->loadParam(ROLLING_FRICTION,"AlertParam/Friction4Real",400.0);
    SLIDING_FRICTION = ROLLING_FRICTION * 15;
    *rollingFraction = static_cast<float>(ROLLING_FRICTION);
    *slidingFraction = static_cast<float>(SLIDING_FRICTION);

}

GetBestUtils::~GetBestUtils() {
    if(players!=NULL){
        delete players;
        players = NULL;
    }
    if(ball!=NULL){
        delete ball;
        ball = NULL;
    }
}

void GetBestUtils::initialize(const CVisionModule *pVision) {
    //更新图像信息
    this->pVision = pVision;
}

void GetBestUtils::run() {
    //while(true){
        ZGetBestUtils::Instance()->calculateBestPass();
   // }
}

void GetBestUtils::reset() {
    bestFlatPassPos = CGeoPoint(99999,99999);
    bestFlatShootPos  = CGeoPoint(99999,99999);
    bestFlatPassQ  = -99999;
    bestFlatShootQ = -99999;
    passPoints.clear();
}

void GetBestUtils::calculateBestPass() {
    int i,j;
    static nType result[2 * PLAYER_NUM * ANGLE_NUM * DIST_NUM * PLAYER_NUM];
    reset();
    //我方球员基本信息
    for(i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        //players[i].me  = this->pVision->ourPlayer(i);
        players[i].Pos = this->pVision->ourPlayer(i).Pos();
        //players[i].Pos.y = this->pVision->ourPlayer(i).Pos().y();
        players[i].Vel = this->pVision->ourPlayer(i).Vel();
        //players[i].Vel.y = this->pVision->ourPlayer(i).Vel().y();
        players[i].isValid = this->pVision->ourPlayer(i).Valid();

        //判断守门员和后卫
        if(Utils::InOurPenaltyArea(this->pVision->ourPlayer(i).Pos(),6*PARAM::Vehicle::V2::PLAYER_SIZE))
            players[i].isValid = false;
    }
    //敌方球员基本信息
    for(i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        //players[i + PARAM::Field::MAX_PLAYER].me = this->pVision->theirPlayer(i);
        players[i + PARAM::Field::MAX_PLAYER].Pos = this->pVision->theirPlayer(i).Pos();
        //players[i + PARAM::Field::MAX_PLAYER].Pos.y = this->pVision->theirPlayer(i).Pos().y();
        players[i + PARAM::Field::MAX_PLAYER].Vel = this->pVision->theirPlayer(i).Vel();
        //players[i + PARAM::Field::MAX_PLAYER].Vel.y = this->pVision->theirPlayer(i).Vel().y();
        players[i + PARAM::Field::MAX_PLAYER].isValid = this->pVision->theirPlayer(i).Valid();

        //判断守门员和后卫
//        if(Utils::InOurPenaltyArea(this->pVision->ourPlayer(i).Pos(),6*PARAM::Vehicle::V2::PLAYER_SIZE))
//            players[i].isValid = false;
    }
    //leader = getLeader();
    players[leader].isValid = false;
    //*ball = leaderPos;
    *ball = leaderPos;

    //calculate Module
    ZBestPosCalculate::instance()->BestPass(pVision,players,ball,result,leader,THEIR_RESPONSE_TIME,IS_SIMULATION);
//    GDebugEngine::Instance()->gui_debug_x(getBestFlatPass(),COLOR_GREEN);

//    //bestFlatPassPos = CGeoPoint(0,0);

//    static bool passValid[PLAYER_NUM * ANGLE_NUM * DIST_NUM][2];

//    for(i = 0; i < PLAYER_NUM * ANGLE_NUM * DIST_NUM; ++i) {
//        passValid[i][0] = passValid[i][1] = true;
//        for(j = 0; j < PLAYER_NUM ; ++j) {
//            if(result[i * PLAYER_NUM + j ].isValid == false)
//                passValid[i][0] = 0;
//            if(result[PLAYER_NUM * ANGLE_NUM * DIST_NUM * PLAYER_NUM + i * PLAYER_NUM + j].isValid == false)
//                passValid[i][1] = 0;
//        }
//    }

//    int posIdx;
    //Flat
//    for(i = 0; i < PLAYER_NUM ; i++){
//        if(!players[i].isValid || Utils::InOurPenaltyArea(players[i].Pos,300)) continue;
//        for(int j = 0 ; j< DIST_NUM ;j++){
//            for( int x = 0; x < ANGLE_NUM; x++){
//                posIdx = i * DIST_NUM * ANGLE_NUM + j * ANGLE_NUM + x;
//                if(result[posIdx].interTime < 10 && result[posIdx].interTime > 0){
//                    CGeoPoint candidate(result[posIdx].interPos.x(),result[posIdx].interPos.y()),p1(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH /2) , p2(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH/2);
//                    GDebugEngine::Instance()->gui_debug_x(candidate,COLOR_GREEN);
//                    double interTime = result[posIdx].interTime;
//                    //if(passValid[i][0] && )
//                    std::vector<float> passScores = ZPassPosEvaluate::instance()->evaluateFunc(candidate,leaderPos,PASS);
//                    float passScore = passScores.front();
//                    //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-2000,0),QString::number(1).toLatin1(),COLOR_RED);
//                    if(passScore > bestFlatPassQ) {
//                        bestFlatPassQ = passScore;
//                        bestFlatPassPos = candidate;
//                        bestFlatPassNum = result[posIdx].playerIndex;
//                        bestFlatPassVel = result[posIdx].Vel;
//                    }
//                }
//            }
//        }
//    }

//    CGeoPoint catchPoint;
//    int catcherIndex = -1;
//    double catchTime = 99999;
//        for(int i = 0;i< PLAYER_NUM * DIST_NUM * ANGLE_NUM /* PLAYER_NUM*/;++i){
//            catchTime = result[i].interTime;
//            catchPoint= result[i].interPos;
//            catcherIndex = result[i].playerIndex;
//              //GDebugEngine::Instance()->gui_debug_arc(catchPoint,40,0,360,COLOR_PURPLE);
//              if(catchTime < 10 && catchTime > 0 && !ZPassPosEvaluate::instance()->passTooClose(catchPoint,leaderPos)){
//                  //CGeoPoint candidate(result[posIdx].interPos.x(),result[posIdx].interPos.y()),p1(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH /2) , p2(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH/2)
//                  double interTime = catchTime;
//                  //if(passValid[i][0] && )
//                  std::vector<float> passScores = ZPassPosEvaluate::instance()->evaluateFunc(catchPoint,leaderPos,PASS);
//                  float passScore = passScores.front();
//                  //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-2000,0),QString::number(1).toLatin1(),COLOR_RED);
//                  if(passScore > bestFlatPassQ) {
//                      bestFlatPassQ = passScore;
//                      bestChipPassQ = passScore;
//                      bestFlatPassPos = catchPoint;
//                      bestChipPassPos = catchPoint;
//                      bestFlatPassNum = catcherIndex;
//                      bestChipPassNum = catcherIndex;
//                      //bestflatvel = flatSecurity?ballFlatSpeed:0;
//                  }
//                  // 更新最佳平射射门点
//                  std::vector<float> shootScores = ZPassPosEvaluate::instance()->evaluateFunc(catchPoint,leaderPos,SHOOT);
//                  float shootScore = shootScores.front();
//                  if(shootScore > bestFlatShootQ){
//                      bestFlatShootQ = shootScore;
//                      bestChipShootQ = shootScore;
//                      bestFlatShootPos = catchPoint;
//                      bestChipShootPos = catchPoint;
//                      bestFlatShootNum = catcherIndex;
//                      bestChipShootNum = catcherIndex;
//                      //bestFlatShootVel = result[i * PLAYER_NUM].Vel;
//                  }
//                  // 更新最佳free_kickpos
//                  std::vector<float> freeScores = ZPassPosEvaluate::instance()->evaluateFunc(catchPoint,leaderPos,FREE_KICK);
//                  float freeScore = freeScores.front();
//                  if(freeScore > bestFreePassQ){
//                      bestFreePassQ = freeScore;
//                      bestFreeKickPos = catchPoint;
//                      bestFreePassNum = catcherIndex;
//                      //bestFlatShootVel = result[i * PLAYER_NUM].Vel;
//                  }
//              }
//          }
        //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,1000),QString("%1").arg(getBestFlatPass().x()).toLatin1(),COLOR_RED);

    //更新最佳平射传球点

}

int GetBestUtils::getLeader(){
    double tobaldist = 99999;
    int real = -1;
    const MobileVisionT&ball = pVision->ball();
    for(int i = 0;i<PARAM::Field::MAX_PLAYER;++i){
        const PlayerVisionT &me = pVision->ourPlayer(i);
        if(!me.Valid()) continue;
        if(me.Pos().dist(ball.Pos())<tobaldist) {
            tobaldist = me.Pos().dist(ball.Pos());
            real = i;
        }
    }
    return real;
}
