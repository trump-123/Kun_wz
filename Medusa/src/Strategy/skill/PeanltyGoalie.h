#ifndef PEANLTYGOALIE_H
#define PEANLTYGOALIE_H
#include "skill/PlayerTask.h"
#include "WorldDefine.h"

class CPeanltyGoalie:public CPlayerTask{
public:
    CPeanltyGoalie();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "PenaltyGoalie"; }
private:
    int _lastCycle;
    bool _judege;
    enum STATE{
        NOTHING = 0,
        STAND,
        CLEAR,
        SAVE
    };
    STATE _state;
    std::string debug_state;
};



#endif // PEANLTYGOALIE_H
