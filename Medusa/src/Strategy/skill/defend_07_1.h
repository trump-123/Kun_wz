#ifndef DEFEND_07_1_H
#define DEFEND_07_1_H
#include <skill/PlayerTask.h>

class  CDefend_07_1: public CPlayerTask{
public:
    CDefend_07_1();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Defend_07_0"; }
private:
    int _lastCycle;
    enum STATE{
        NOTHING = 0,
        STAND,
        CLEAR,
        SAVE
    };
    STATE _state;
    std::string debug_state;
};


#endif // LEADER_H
