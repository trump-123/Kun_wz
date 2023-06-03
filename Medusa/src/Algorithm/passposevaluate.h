#ifndef PASSPOSEVALUATE_H
#define PASSPOSEVALUATE_H
#include <vector>
#include "geometry.h"
#include "singleton.hpp"

enum EvaluateMode{
    PASS = 1,
    SHOOT = 2,
    FREE_KICK = 3
};

class CPassPosEvaluate {
public:
    CPassPosEvaluate();
    std::vector<float> evaluateFunc(CGeoPoint candidate,CGeoPoint leaderPos,EvaluateMode mode);
    bool passTooClose(CGeoPoint candidate,CGeoPoint leaderPos);
};

typedef  Singleton<CPassPosEvaluate> ZPassPosEvaluate;

#endif // PASSPOSEVALUATE_H
