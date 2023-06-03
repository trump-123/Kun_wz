#ifndef DEFEND_H
#define DEFEND_H
#include <skill/PlayerTask.h>
#include "WorldDefine.h"


class Cdefend : public CPlayerTask{
public:
    Cdefend();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Defend"; }
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
#endif // DEFEND_H
