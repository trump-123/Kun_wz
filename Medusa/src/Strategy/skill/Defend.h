#ifndef DEFEND_H
#define DEFEND_H

#include <skill/PlayerTask.h>

class CDefend : public CPlayerTask{
public:
    CDefend();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Defend"; }
private:
    int _lastCycle;
};

#endif // DEFEND_H
