#ifndef CTOUCH_H
#define CTOUCH_H

#include <skill/PlayerTask.h>

class CTouch : public CPlayerTask{
public:
    CTouch();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Touch"; }
private:
    int _lastCycle;
    CGeoPoint interPoint;
    CGeoPoint chipInterPoint;
    double interTime = 9999;
};
#endif // CTOUCH_H
