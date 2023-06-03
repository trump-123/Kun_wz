#ifndef LEADER_H
#define LEADER_H
#include <skill/PlayerTask.h>

class  CLeader: public CPlayerTask{
public:
    CLeader();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Leader"; }
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
