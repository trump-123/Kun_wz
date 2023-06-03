#ifndef TIER_H
#define TIER_H
#include <skill/PlayerTask.h>
#include "WorldDefine.h"


class CTier : public CPlayerTask{
public:
    CTier();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Tier"; }
private:
    int _lastCycle;
};

#endif // TIER_H
