#ifndef DEFEND_05_H
#define DEFEND_05_H
#include <skill/PlayerTask.h>

class  CDefend_05: public CPlayerTask{
public:
    CDefend_05();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Defend_05"; }
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
