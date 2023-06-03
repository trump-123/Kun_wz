#ifndef WBREAK_H
#define WBREAK_H

#include "skill/PlayerTask.h"

class CWBreak : public CStatedTask{
public:
    CWBreak();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual CPlayerCommand* execute(const CVisionModule* pVision);
private:
    virtual void toStream(std::ostream& os) const { os << "Skill: WBreak\n" << std::endl; }
private:
    int _lastCycle;
    bool isDribble = false;
    int grabMode;
    int last_mode;
    int fraredOn;
    int fraredOff;
    CGeoPoint move_Point;
    CGeoPoint dribblePoint;
};

#endif // WBREAK_H
