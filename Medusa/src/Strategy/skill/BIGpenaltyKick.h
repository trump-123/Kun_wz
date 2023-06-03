#ifndef BIGPENALTYKICK_H
#define BIGPENALTYKICK_H

#include <skill/PlayerTask.h>
#include "WorldDefine.h"

class CBIGpenaltyKick:public CPlayerTask
{
public:
    CBIGpenaltyKick();
    virtual void plan(const CVisionModule *pVision);
    virtual bool isEmpty()const {return false;}
    virtual void toStream(std::ostream &os)const {os << "BIGPenaltyKick";}
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



#endif // BIGPENALTYKICK_H
