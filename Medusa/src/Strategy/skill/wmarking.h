#ifndef CWMARKING_H
#define CWMARKING_H
#include "skill/PlayerTask.h"

class CWmarking : public CStatedTask{
public:
    CWmarking();
    virtual void plan(const CVisionModule* pVision);
    virtual CPlayerCommand* execute(const CVisionModule * pVision);
    virtual bool isEmpty() const {return false;}
protected:
    virtual void toStream(std::ostream &os) const{ os << "Marking Defense";}
private:
    int _lastCycle;
};

#endif // CWMARKING_H
