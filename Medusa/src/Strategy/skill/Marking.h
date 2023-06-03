#ifndef MARKING_H
#define MARKING_H


#include <skill/PlayerTask.h>

class CMarking : public CPlayerTask{
public:
    CMarking();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Marking"; }
private:
    int _lastCycle;
};

#endif // MARKING_H
