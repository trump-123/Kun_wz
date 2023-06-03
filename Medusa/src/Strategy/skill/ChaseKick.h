#ifndef CHASEKICK_H
#define CHASEKICK_H

#include <skill/PlayerTask.h>

class CChaseKick : public CPlayerTask{
public:
    CChaseKick();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "ChaseKick"; }
private:
    int _lastCycle;
    enum STATE{
        NOTHING = 0,
        CHASE,
        SHOOT,
        DEFEND
    };
    STATE _state;
    std::string debug_state;
};

#endif // CHASEKICK_H
