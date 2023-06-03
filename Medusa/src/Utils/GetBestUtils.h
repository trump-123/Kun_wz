/*********************
* Imitate for CUDA
* Author: Fan
* Created Date: 2022/8/12
*********************/
#ifndef GETBESTUTILS_H
#define GETBESTUTILS_H

#include "VisionModule.h"
#include <singleton.h>
#include "singleton.hpp"
#include "geometry.h"

//typedef struct{
//    float x,y;
//} Vector;

typedef struct{
    float x,y;
} Point;

typedef struct{
    PlayerVisionT me;
    CGeoPoint Pos;
    CVector Vel;
    bool isValid;
} Player;

typedef struct{
    CGeoPoint interPos;
    float interTime;
    float Vel;
    float dir;
    int playerIndex;
    float deltaTime;
    float Q;
} rType;

typedef struct{
    CGeoPoint interPos;
    bool isValid;
    float interTime;
    float Vel;
    float dir;
    int playerIndex;
    float deltaTime;
    float Q;
} nType;

class GetBestUtils
{
public:
    GetBestUtils();
    ~GetBestUtils();
    void initialize(const CVisionModule *);
    void run();
    void calculateFreeKickPos();
    CGeoPoint getBestFreeKickPos(){ return bestFreeKickPos; }
    CGeoPoint getBestFlatPass(){ return bestFlatPassPos; }
    CGeoPoint getBestFlatShoot(){ return bestFlatShootPos; }
    float getBestFlatPassQ(){ return bestFlatPassQ; }
    float getBestFlatShootQ(){ return bestFlatShootQ; }
    int getBestFlatPassNum(){ return bestFlatPassNum; }
    int getBestFlatShootNum(){ return bestFlatShootNum; }
    int getBestFreePassNum(){ return bestFreePassNum; }
    int getLeader();
    float getBestFlatPassVel(){ return bestFlatPassVel; }
    float getBestFlatShootVel(){ return bestFlatShootVel; }
    std::vector<float> getPassParameters(){ return passParameters; }
    float calRotateTime(CGeoPoint candi);
    void calculateBestPass();
    void setLeader(int _leader){ this->leader = _leader; }
    void setLeaderPos(CGeoPoint leaderPos){ this->leaderPos = leaderPos; }
    void setReceiver(int _receiver){ this->receiver = _receiver; }
    //void setNeymar(int _neymar){ neymar = _neymar; }
    void reset();
    bool IsTurnTooMuch(CGeoPoint candi);
    bool posAcrossPenalty(CGeoPoint candi);
    bool posTowardOutField(CGeoPoint candi);

private:
    CGeoPoint bestFlatPassPos;
    CGeoPoint bestFlatShootPos;
    CGeoPoint bestFreeKickPos;
    float bestFlatPassQ;
    float bestFlatShootQ;
    CGeoPoint bestScorePosition[16];
    int bestFlatPassNum;
    int bestFlatShootNum;
    int bestFreePassNum;
    float bestFlatShootVel;
    float bestFlatPassVel;
    int leader;
    CGeoPoint leaderPos;
    int receiver;
    //int neymar;
    Player *players;
    Player *enemy;
    CGeoPoint *ball;
    float *rollingFraction;
    float *slidingFraction;
    const CVisionModule* pVision;
    std::vector<float> passParameters;
    std::vector<nType> passPoints;
};
//typedef Singleton<GetBestUtils> ZGetBestUtils;
//typedef NormalSingleton<GetBestUtils> ZGetBestUtils;

#endif // GETBESTUTILS_H
