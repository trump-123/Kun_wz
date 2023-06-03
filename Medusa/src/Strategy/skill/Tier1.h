#ifndef GOALIE1_H
#define GOALIE1_H
#include <skill/PlayerTask.h>

class CTier1 : public CPlayerTask
{
public:
    CTier1();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Tier1"; }
private:
    int _lastCycle;
    enum STATE{
        NOTHING = 0,
        STAND,
        CLEAR,
        SAVE
    };
    STATE _state;
    std::string debug_state;
};

/*#include <skill/PlayerTask.h>
class CStopRobotV2 : public CPlayerTask{
public:
    CStopRobotV2();
    virtual CPlayerCommand* execute(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
protected:
    virtual void toStream(std::ostream& os) const { os << "Stopping"; }
};*/
#endif // GOALIE1_H
