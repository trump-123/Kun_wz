#ifndef CMARKINGFRONT_H
#define CMARKINGFRONT_H

#include <skill/PlayerTask.h>

class CMarkingFront : public CPlayerTask{
public:
    CMarkingFront();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Marking"; }
private:
    int _lastCycle;
};
#endif // CMARKINGFRONT_H
