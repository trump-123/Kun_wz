#ifndef GOALIE_H
#define GOALIE_H
#include <skill/PlayerTask.h>
#include "WorldDefine.h"

class CGoalie : public CPlayerTask{
public:
    CGoalie();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Goalie"; }
private:
    int _lastCycle;
    bool _flytimediff;
    CGeoPoint interPos;
    enum STATE{
        NOTHING = 0,
        STAND,
        CLEAR,
        SAVE
    };
    STATE _state;
    std::string debug_state;
};

class valid{
    protected:
    void valid1();
};
#endif // GOALIE_H
