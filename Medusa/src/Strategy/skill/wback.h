#ifndef WBACK_H
#define WBACK_H
#include "skill/PlayerTask.h"

class CWback : public CStatedTask
{
public:
    CWback();
    virtual void plan(const CVisionModule* pVision);
    virtual CPlayerCommand* execute(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
private:
    int _lastCycle;
    int _mylastCycle;
    int _flytimediff;
};

#endif // WBACK_H
