#ifndef GUAN_H
#define GUAN_H
#include <skill/PlayerTask.h>
#include "WorldDefine.h"


class Cguan : public CPlayerTask{
public:
    Cguan();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "guan"; }
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

#endif // GUAN_H
