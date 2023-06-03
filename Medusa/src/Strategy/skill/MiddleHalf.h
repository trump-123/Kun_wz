#ifndef MIDDLEHALF_H
#define MIDDLEHALF_H
#include "WorldDefine.h"
#include "skill/PlayerTask.h"

class CMiddleHalf:public CPlayerTask
{
public:
    CMiddleHalf();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "CMiddleHalf"; }
private:
    int _lastCycle;
};
#endif // MIDDLEHALF_H
