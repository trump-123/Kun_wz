#ifndef UNIQUEKILLSKILLL_H
#define UNIQUEKILLSKILLL_H

#include <skill/PlayerTask.h>

class CUniquekillskilll : public CPlayerTask{
public:
    CUniquekillskilll();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Uniquekillskilll"; }
private:
    int _lastCycle;
};

#endif // UNIQUEKILLSKILLL_H
