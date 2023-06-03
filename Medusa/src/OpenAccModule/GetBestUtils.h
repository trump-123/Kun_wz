/*********************
* Imitate for CUDA
* Author: Fan
* Created Date: 2022/8/12
*********************/
#ifndef _GETBESTUTILS_H
#define _GETBESTUTILS_H

#include "VisionModule.h"
#include <singleton.h>
#include "singleton.hpp"
#include "geometry.h"
//#include "BestPosCalculate.h"


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
    CGeoPoint getBestChipPass(){ return bestChipPassPos; }
    CGeoPoint getBestChipShoot(){ return bestChipShootPos; }
    float getBestFlatPassQ(){ return bestFlatPassQ; }
    float getBestFlatShootQ(){ return bestFlatShootQ; }
    float getBestChipPassQ(){ return bestChipPassQ; }
    float getBestChipShootQ(){ return bestChipShootQ; }
    float getBestFreeKickQ() {return bestFreePassQ; }
    int getBestFlatPassNum(){ return bestFlatPassNum; }
    int getBestFlatShootNum(){ return bestFlatShootNum; }
    int getBestChipPassNum(){ return bestChipPassNum; }
    int getBestChipShootNum(){ return bestChipShootNum; }
    int getBestFreePassNum(){ return bestFreePassNum; }
    void setBestFreeKickPos(CGeoPoint &index){  bestFreeKickPos = index; }
    void setBestFlatPass(CGeoPoint &index){  bestFlatPassPos= index; }
    void setBestFlatShoot(CGeoPoint &index){  bestFlatShootPos= index; }
    void setBestChipPass(CGeoPoint &index){  bestChipPassPos= index; }
    void setBestChipShoot(CGeoPoint &index){  bestChipShootPos= index; }
    void setBestFlatPassQ(float index){  bestFlatPassQ= index; }
    void setBestFlatShootQ(float index){  bestFlatShootQ= index; }
    void setBestChipPassQ(float index){  bestChipPassQ= index; }
    void setBestChipShootQ(float index){  bestChipShootQ= index; }
    void setBestFreeKickQ(float index) { bestFreePassQ = index; }
    void setBestFlatPassNum(int index){  bestFlatPassNum= index; }
    void setBestFlatShootNum(int index){  bestFlatShootNum= index; }
    void setBestChipPassNum(int index){  bestChipPassNum= index; }
    void setBestChipShootNum(int index){  bestChipShootNum= index; }
    void setBestFreePassNum(int index){  bestFreePassNum= index; }
    int getLeader();
    float getBestFlatPassVel(){ return bestFlatPassVel; }
    float getBestFlatShootVel(){ return bestFlatShootVel; }
    std::vector<float> getPassParameters(){ return passParameters; }
    float calRotateTime(CGeoPoint candi);
    void calculateBestPass();
    void setLeader(int _leader){ this->leader = _leader; }
    void setLeaderPos(CGeoPoint leaderPos){ this->leaderPos = leaderPos; }
    CGeoPoint getLeaderPos(){ return leaderPos;}
    void setReceiver(int _receiver){ this->receiver = _receiver; }
    //void setNeymar(int _neymar){ neymar = _neymar; }
    void reset();
    bool IsTurnTooMuch(CGeoPoint candi);
    bool posAcrossPenalty(CGeoPoint candi);
    bool posTowardOutField(CGeoPoint candi);

private:
    CGeoPoint bestFlatPassPos;
    CGeoPoint bestChipPassPos;
    CGeoPoint bestFlatShootPos;
    CGeoPoint bestChipShootPos;
    CGeoPoint bestFreeKickPos;
    float bestFlatPassQ;
    float bestChipPassQ;
    float bestFlatShootQ;
    float bestChipShootQ;
    float bestFreePassQ;
    CGeoPoint bestScorePosition[16];
    int bestFlatPassNum;
    int bestChipPassNum;
    int bestFlatShootNum;
    int bestChipShootNum;
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
typedef NormalSingleton<GetBestUtils> ZGetBestUtils;

#endif // GETBESTUTILS_H
