#ifndef FETCHBALL_H
#define FETCHBALL_H
#include <skill/PlayerTask.h>
#include "WorldDefine.h"

class CFetchBall:public CStatedTask{
public:
    CFetchBall();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Touch"; }
private:
    int _lastCycle;
    int _mylastCycle;
    bool goBackBall;
    int cnt = 0;
    bool notDribble;
};

#endif // FETCHBALL_H
