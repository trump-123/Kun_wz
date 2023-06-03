#ifndef RIGHTHALF_H
#define RIGHTHALF_H
#include "WorldDefine.h"
#include "skill/PlayerTask.h"

class CRightHalf:public CPlayerTask
{
public:
    CRightHalf();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "RightHalf"; }
private:
    int _lastCycle;
};

#endif // RIGHTHALF_H
