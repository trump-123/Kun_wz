#ifndef NAQIU_H
#define NAQIU_H
#include <skill/PlayerTask.h>
#include "WorldDefine.h"

class Cnaqiu : public CPlayerTask{
public:
    Cnaqiu();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "naqiu"; }
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
#endif // NAQIU_H
