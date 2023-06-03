#ifndef UNIQUEKILLSKILLM_H
#define UNIQUEKILLSKILLM_H

#include <skill/PlayerTask.h>

class CUniquekillskillm : public CPlayerTask{
public:
    CUniquekillskillm();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Uniquekillskillm"; }
private:
    int _lastCycle;
};

#endif // UNIQUEKILLSKILLM_H
