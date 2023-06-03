#ifndef STATICGETBALL_H
#define STATICGETBALL_H

#include "skill/PlayerTask.h"

class CStaticGetBall:public CStatedTask
{
public:
    CStaticGetBall();
    virtual void plan(const CVisionModule* pVision);
    virtual CPlayerCommand* execute(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
private:
    int _lastCycle;
    int _mylastCycle;
    int _flytimediff;
    bool _goBackball;
    bool _backjud;
    bool _canKick;
};

#endif // STATICGETBALL_H
