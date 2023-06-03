#ifndef WDRAG_H
#define WDRAG_H
#include <skill/PlayerTask.h>

class CWDrag : public CStatedTask
{
public:
    CWDrag();
    virtual void plan(const CVisionModule* pVision);
    virtual CPlayerCommand *execute(const CVisionModule *pVision);
    int getEnemyAmountInArea(const CGeoPoint& center ,double radius, std::vector<int>& enemyNumVec, double buffer,const CVisionModule* _pVision);
protected:
    virtual void toStream(std::ostream& os) const {os << "Skill: WDrag" << std::endl;}

private:
    int _lastCycle;
    int _lastState;
    int _lastConfirmCycle;
    int _state;
    //QString now_state;
    int enemyNum = -999;
    int freeCount = 0;
    int antiCnt = 0;
    int antiVelCount = 0;
    int realCnt = 0;
    int escapeCnt = 0;
    //const CVisionModule* _pVision;
    bool confirmFreeBallDist = false;
    double markDist = 9999;
    CVector v1, v2;
    CVector finalVelVec;
    CGeoPoint antiTarget, realTarget;
    double freeBallDist = 9999;
    bool purposeMode = false;
};

#endif // WDRAG_H
