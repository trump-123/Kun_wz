#ifndef UNIQUEKILLSKILLR_H
#define UNIQUEKILLSKILLR_H

#include <skill/PlayerTask.h>

class CUniquekillskillr : public CPlayerTask{
public:
    CUniquekillskillr();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "CUniquekillskillr"; }
private:
    int _lastCycle;
};


#endif // UNIQUEKILLSKILLR_H
