#ifndef PENALTYKICK_H
#define PENALTYKICK_H
#include <skill/PlayerTask.h>
#include "WorldDefine.h"

class CPenaltyKick:public CPlayerTask
{
public:
    CPenaltyKick();
    virtual void plan(const CVisionModule *pVision);
    virtual bool isEmpty()const {return false;}
    virtual void toStream(std::ostream &os)const {os << "PenaltyKick";}
private:
    int _lastCycle;
    bool goBackBall;
    bool turnPos;
    bool canKick;
    bool isKick;
    bool notDribble;
    enum STATE{
        NOTHING = 0,
        GETBALL,
        DECEPTIVE,
        SHOOT
    };
};



#endif // PENALTYKICK_H
