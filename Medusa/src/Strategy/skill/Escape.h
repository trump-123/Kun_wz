#ifndef ESCAPE_H
#define ESCAPE_H
#include <skill/PlayerTask.h>

class CEscape:public CPlayerTask{
public:
    CEscape();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Escape"; }
private:
    int _lastCycle;
};

#endif // ESCAPE_H
