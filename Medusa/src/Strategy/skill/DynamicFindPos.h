#ifndef DYNAMICFINDPOS_H
#define DYNAMICFINDPOS_H

#include <skill/PlayerTask.h>

class CDynamicFindPos : public CPlayerTask{
public:
    CDynamicFindPos();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "DynamicFindPos"; }
private:
    int _lastCycle;
};
#endif // DYNAMICFINDPOS_H
