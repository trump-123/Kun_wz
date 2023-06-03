#ifndef OPPOSEBACK_H
#define OPPOSEBACK_H
#include "WorldDefine.h"
#include "skill/PlayerTask.h"

class CLeftHalf:public CPlayerTask
{
public:
    CLeftHalf();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "CLeftHalf"; }
private:
    int _lastCycle;
};

#endif // OPPOSEBACK_H
