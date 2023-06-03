#ifndef GOANDTURNKICK_H
#define GOANDTURNKICK_H

#include <skill/PlayerTask.h>

class CgoAndTurnKick : public CPlayerTask{
public:
    CgoAndTurnKick();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    void judgeMode(const CVisionModule* pVision);
    bool judgeShootMode(const CVisionModule* pVision);
    virtual void toStream(std::ostream& os) const { os << "ChaseKick"; }
private:
    int _lastCycle;
    int _mylastCycle;
    int getBallMode;
    int lastGetBallMode;
    bool gobreakdiff;
    int breakon;
    int breakoff;
    CGeoPoint dribblePoint;
    int cnt = 0;
    bool needdribble = false;
    bool needkick = false;
    bool chip = false;
    bool safeMode = false;
    bool rushMode = false;
    bool isTouch = false;
    bool ballplacement = false;
    bool predictBall = false;
    CGeoPoint targetPoint;
    CGeoPoint waitPoint;
    CGeoPoint ballLineProjection;
    //double meArriveTime = 9999;
    CGeoPoint interPoint;
    CGeoPoint chipInterPoint;
    double interTime = 9999;
    double last_speed = 0;
    bool needAvoidBall;
    bool canGetBall;
    bool canForwardShoot;
    int fraredOn;
    int fraredOff;
    int staticDir;
    enum STATE{
        NOTHING = 0,
        CHASE,
        SHOOT,
        DEFEND,
        TOUCH
    };
    STATE _state;
    std::string debug_state;
};

#endif // GOANDTURNKICK_H
