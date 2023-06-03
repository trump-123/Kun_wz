#ifndef DNAMICFINDPOS_H
#define DNAMICFINDPOS_H

#include <skill/PlayerTask.h>

class CDnamicfindpos : public CPlayerTask{
public:
    CDnamicfindpos();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Touch"; }
private:
    int _lastCycle;
};
#endif // DNAMICFINDPOS_H
