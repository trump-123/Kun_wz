#ifndef SHOOTMODULE_H
#define SHOOTMODULE_H

#include "VisionModule.h"

class CShootModule
{
public:
    CShootModule();
    bool generateBestTarget(const CVisionModule *pVision, CGeoPoint &bestTarget, const CGeoPoint& pos);
    double generateBestTimeDiff(const CVisionModule *pVision, CGeoPoint &bestTarget, const CGeoPoint &pos = CGeoPoint(0, 0));
    bool canShoot(const CVisionModule *pVision, CGeoPoint shootPos);
private:
//避免一帧内重复计算
    int _lastCycle;
    bool isCalculated;
    int keepedCanShoot;
    CGeoPoint keepedBestTarget;
//算法所需参数
    double tolerance;
    double stepSize; //度数步长，弧度制
    double responseTime;
    double agressiveresponseTime;
};

typedef NormalSingleton <CShootModule> ShootModule;

#endif // SHOOTMODULE_H
