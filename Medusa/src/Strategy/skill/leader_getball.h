#ifndef LEADER_GETBALL_H
#define LEADER_GETBALL_H
#include <skill/PlayerTask.h>

class  CLeader_getball: public CPlayerTask{
public:
    CLeader_getball();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Leader_getball"; }
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
