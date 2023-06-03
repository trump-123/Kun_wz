#ifndef CSINGLETIER_H
#define CSINGLETIER_H
#include <skill/PlayerTask.h>
#include "WorldDefine.h"


class CSingleTier : public CPlayerTask{
public:
    CSingleTier();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "SingleTier"; }
private:
    int _lastCycle;
};


#endif // CSINGLETIER_H
