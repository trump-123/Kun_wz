#ifndef ASSISTER_H
#define ASSISTER_H
#include <skill/PlayerTask.h>

class  CAssister: public CPlayerTask{
public:
    CAssister();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Assister"; }
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
