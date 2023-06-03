#ifndef GUARDPOS_H
#define GUARDPOS_H
#include "VisionModule.h"
#include "singleton.h"
#include <vector>

class CGuardPos
{
public:
    CGuardPos();
    CGeoPoint backPos(int guardNum, int index, int realNum, int defendNum);
    CGeoPoint multibackPos(int defendNum);
    bool validBackPos(CGeoPoint backPos, int realNUm);
private:
    void generatePos(int guardNum);
    void generatemultiPos(int defendNum,int index);
    bool usedoubleBack(int index);
    bool validdefendNum(int index, int defendNum);
    bool leftNextPos(CGeoPoint basePos,CGeoPoint &nextPos,double dist = -9999);
    bool rightNextPos(CGeoPoint basePos,CGeoPoint &nextPos,double dist = -9999);
    CGeoPoint _backPos[PARAM::Field::MAX_PLAYER];
    CGeoPoint _multibackPos[PARAM::Field::MAX_PLAYER];
    CGeoPoint _revisebackPos[PARAM::Field::MAX_PLAYER];
    int _guardNum;
};
typedef  NormalSingleton<CGuardPos> GuardPos;

#endif // GUARDPOS_H
