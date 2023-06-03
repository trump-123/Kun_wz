#ifndef CMARKINGTOUCH_H
#define CMARKINGTOUCH_H
#include <skill/PlayerTask.h>

class CMarkingTouch : public CPlayerTask{
public:
    CMarkingTouch();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Marking"; }
private:
    int _lastCycle;
};

#endif // CMARKINGTOUCH_H
