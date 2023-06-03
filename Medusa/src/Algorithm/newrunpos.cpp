//#include "newrunpos.h"
//#include "staticparams.h"
//#include "GDebugEngine.h"
//#include "ShootRangeList.h"
//#include "math.h"
//#include "WorldModel.h"
//#include "SkillUtils.h"
//#include "parammanager.h"

//namespace{
//bool OTHERPOS_DEBUG = false;
//const int ATTACK_AREA_NUM = 6;
//const int WRONG_RUNPOS_CHANGE_BUFFER = 20;
//CGeoRectangle BACK_LEFT_AREA(- PARAM::Field::PITCH_LENGTH / 4, 0, 0, PARAM::Field::PITCH_WIDTH / 2);
//CGeoRectangle BACK_RIGHT_AREA(- PARAM::Field::PITCH_LENGTH / 4, - PARAM::Field::PITCH_WIDTH / 2, 0, 0);
//CGeoRectangle MIDDLE_LEFT_AREA(0, 0, PARAM::Field::PITCH_LENGTH / 4, PARAM::Field::PITCH_WIDTH / 2);
//CGeoRectangle MIDDLE_RIGHT_AREA(0, - PARAM::Field::PITCH_WIDTH / 2, PARAM::Field::PITCH_LENGTH / 4, 0);
//CGeoRectangle FRONT_LEFT_AREA(PARAM::Field::PITCH_LENGTH / 4, 0, PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::PITCH_WIDTH / 2);
//CGeoRectangle FRONT_RIGHT_AREA(PARAM::Field::PITCH_LENGTH / 4, - PARAM::Field::PITCH_WIDTH / 2, PARAM::Field::PITCH_LENGTH / 2, 0);

//CGeoRectangle ATTACK_AREA[6] = {BACK_LEFT_AREA, BACK_RIGHT_AREA, MIDDLE_LEFT_AREA, MIDDLE_RIGHT_AREA, FRONT_LEFT_AREA, FRONT_RIGHT_AREA};
//newRunPosProperties areaRawPoint[6];
//int runPosCycle = 0;

//}


//NewRunPos::NewRunPos():_pVision(nullptr)
//{
//    ZSS::ZParamManager::instance()->loadParam(OTHERPOS_DEBUG, "Debug/OtherPos", false);
//    for(int i = 0; i < ATTACK_AREA_NUM; i++)
//    {
//        _runPos[i].pos = CGeoPoint((ATTACK_AREA[i]._point[0].x() + ATTACK_AREA[i]._point[2].x()) / 2, (ATTACK_AREA[i]._point[0].y() + ATTACK_AREA[i]._point[2].y()) / 2);
//        _runPos[i].areaNum = i;
//    }

//}

//void NewRunPos::generateRunPos(const CVisionModule* pVision, const CGeoPoint& avoidPos, const CGeoPoint& avoidPos2)
//{
//    _pVision = pVision;
//    _ballPos = pVision->ball().Pos();

//    _avoidPos = avoidPos;
//    _avoidPos2 = avoidPos2;

//    generateSection();

//    for(int i = 0; i < ATTACK_AREA_NUM; i++)
//    {
////        if(1)
//        if(OTHERPOS_DEBUG)
//        {
//            GDebugEngine::Instance()->gui_debug_line(ATTACK_AREA[i]._point[0], ATTACK_AREA[i]._point[1], COLOR_WHITE);
//            GDebugEngine::Instance()->gui_debug_line(ATTACK_AREA[i]._point[1], ATTACK_AREA[i]._point[2], COLOR_WHITE);
//            GDebugEngine::Instance()->gui_debug_line(ATTACK_AREA[i]._point[2], ATTACK_AREA[i]._point[3], COLOR_WHITE);
//            GDebugEngine::Instance()->gui_debug_line(ATTACK_AREA[i]._point[3], ATTACK_AREA[i]._point[0], COLOR_WHITE);
//        }

//        selectAreaBestPoint(i);
////        GDebugEngine::Instance()->gui_debug_msg(areaRawPoint[i].pos, QString("%1%1%1%1%1").arg(i).toLatin1(), COLOR_GREEN);
//    }

//    judgeRunPosValid();
//}


//void NewRunPos::generateSection()
//{
//    const MobileVisionT& ball = _pVision->ball();
//    float bx = ball.X();
//    float by = ball.Y();

//    //this part needs to be modified if it doesn't works well
//    if(bx > PARAM::Field::PITCH_LENGTH / 4 && bx < PARAM::Field::PITCH_LENGTH / 3)
//    {
//        FRONT_LEFT_AREA   = CGeoRectangle(bx / 3 + PARAM::Field::PITCH_LENGTH / 4, by / 2, PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::PITCH_WIDTH / 2);
//        FRONT_RIGHT_AREA  = CGeoRectangle(bx / 3 + PARAM::Field::PITCH_LENGTH / 4, - PARAM::Field::PITCH_WIDTH / 2, PARAM::Field::PITCH_LENGTH / 2, by / 2);
//        MIDDLE_LEFT_AREA  = CGeoRectangle(0, by / 2, bx / 3 + PARAM::Field::PITCH_LENGTH / 4, PARAM::Field::PITCH_WIDTH / 2);
//        MIDDLE_RIGHT_AREA = CGeoRectangle(0, -PARAM::Field::PITCH_WIDTH / 2, bx / 3 + PARAM::Field::PITCH_LENGTH / 4, by / 2);
//        BACK_LEFT_AREA    = CGeoRectangle(- PARAM::Field::PITCH_LENGTH / 4, by / 2, 0, PARAM::Field::PITCH_WIDTH / 2);
//        BACK_RIGHT_AREA   = CGeoRectangle(- PARAM::Field::PITCH_LENGTH / 4, - PARAM::Field::PITCH_WIDTH / 2, 0, by / 2);
//    }
//    else if(bx >= PARAM::Field::PITCH_LENGTH / 3)
//    {
//        FRONT_LEFT_AREA   = CGeoRectangle(PARAM::Field::PITCH_LENGTH / 4, by / 2, PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::PITCH_WIDTH / 2);
//        FRONT_RIGHT_AREA  = CGeoRectangle(PARAM::Field::PITCH_LENGTH / 4, - PARAM::Field::PITCH_WIDTH / 2, PARAM::Field::PITCH_LENGTH / 2, by / 2);
//        MIDDLE_LEFT_AREA  = CGeoRectangle(0, by / 2, PARAM::Field::PITCH_LENGTH / 4, PARAM::Field::PITCH_WIDTH / 2);
//        MIDDLE_RIGHT_AREA = CGeoRectangle(0, - PARAM::Field::PITCH_WIDTH / 2, PARAM::Field::PITCH_LENGTH / 4, by / 2);
//        BACK_LEFT_AREA    = CGeoRectangle(- PARAM::Field::PITCH_LENGTH / 4, by / 2, 0, PARAM::Field::PITCH_WIDTH / 2);
//        BACK_RIGHT_AREA   = CGeoRectangle(- PARAM::Field::PITCH_LENGTH / 4, - PARAM::Field::PITCH_WIDTH / 2, 0, by / 2);
//    }
//    else if(bx <= PARAM::Field::PITCH_LENGTH / 4 && bx >= - PARAM::Field::PITCH_LENGTH / 4)
//    {
//        FRONT_LEFT_AREA   = CGeoRectangle(bx + PARAM::Field::PITCH_LENGTH / 4, by / 2, PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::PITCH_WIDTH / 2);
//        FRONT_RIGHT_AREA  = CGeoRectangle(bx + PARAM::Field::PITCH_LENGTH / 4, - PARAM::Field::PITCH_WIDTH / 2, PARAM::Field::PITCH_LENGTH / 2, by / 2);
//        MIDDLE_LEFT_AREA  = CGeoRectangle(bx, by / 2, bx + PARAM::Field::PITCH_LENGTH / 4, PARAM::Field::PITCH_WIDTH / 2);
//        MIDDLE_RIGHT_AREA = CGeoRectangle(bx, - PARAM::Field::PITCH_WIDTH / 2, bx + PARAM::Field::PITCH_LENGTH / 4, by / 2);
//        BACK_LEFT_AREA    = CGeoRectangle(bx - PARAM::Field::PITCH_LENGTH / 4, by / 2, bx, PARAM::Field::PITCH_WIDTH / 2);
//        BACK_RIGHT_AREA   = CGeoRectangle(bx - PARAM::Field::PITCH_LENGTH / 4, - PARAM::Field::PITCH_WIDTH / 2, bx, by / 2);
//    }
//    else if(bx <= - PARAM::Field::PITCH_LENGTH / 4)
//    {
//        FRONT_LEFT_AREA   = CGeoRectangle(bx + 2 * PARAM::Field::PITCH_LENGTH / 4, by / 2, PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::PITCH_WIDTH / 2);
//        FRONT_RIGHT_AREA  = CGeoRectangle(bx + 2 * PARAM::Field::PITCH_LENGTH / 4, -PARAM::Field::PITCH_WIDTH / 2, PARAM::Field::PITCH_LENGTH / 2, by / 2);
//        MIDDLE_LEFT_AREA  = CGeoRectangle(bx + PARAM::Field::PITCH_LENGTH / 4, by / 2, bx + 2 * PARAM::Field::PITCH_LENGTH / 4, PARAM::Field::PITCH_WIDTH / 2);
//        MIDDLE_RIGHT_AREA = CGeoRectangle(bx + PARAM::Field::PITCH_LENGTH / 4, -PARAM::Field::PITCH_WIDTH / 2, bx + 2 * PARAM::Field::PITCH_LENGTH / 4, by / 2);
//        BACK_LEFT_AREA    = CGeoRectangle(bx, by / 2, bx + PARAM::Field::PITCH_LENGTH / 4, PARAM::Field::PITCH_WIDTH / 2);
//        BACK_RIGHT_AREA   = CGeoRectangle(bx, -PARAM::Field::PITCH_WIDTH / 2, bx + PARAM::Field::PITCH_LENGTH / 4, by / 2);
//    }
//    else{}

//    ATTACK_AREA[0] = BACK_LEFT_AREA;
//    ATTACK_AREA[1] = BACK_RIGHT_AREA;
//    ATTACK_AREA[2] = MIDDLE_LEFT_AREA;
//    ATTACK_AREA[3] = MIDDLE_RIGHT_AREA;
//    ATTACK_AREA[4] = FRONT_LEFT_AREA;
//    ATTACK_AREA[5] = FRONT_RIGHT_AREA;
//}


//void NewRunPos::selectAreaBestPoint(int areaNum)
//{
//    CGeoPoint bestCandidate(9999,9999);
//    float bestScore = -9999;
//    float avoidBuffer = 1500;
//    float stripBuffer = 1200;
//    bool inStripZone = false;
//    bool nearSecTooClose = false;
//    float step = 300;

//    for (float bx = ATTACK_AREA[areaNum]._point[0].x() + 1; bx < ATTACK_AREA[areaNum]._point[2].x(); bx += step) {
//        for (float by = ATTACK_AREA[areaNum]._point[0].y() + 1; by < ATTACK_AREA[areaNum]._point[2].y(); by += step) {
//            CGeoPoint candidate(bx, by);
//            // 排除禁區
//            if(Utils::InTheirPenaltyArea(candidate, 300))
//                continue;
//            //排除过于边界的点
//            if(!Utils::IsInField(candidate, 400))
//                continue;

//            for(int k = 0; k < areaNum; k++)
//            {
//                if(isInRangeOfPos(areaRawPoint[k].pos, candidate, 1000))
//                {
//                    nearSecTooClose = true;
//                    break;
//                }

//            }
//            if(nearSecTooClose)
//            {
//                nearSecTooClose = false;
//                continue;
//            }

//            if(isInRangeOfPos(candidate, _avoidPos, avoidBuffer) || isInRangeOfPos(candidate, _avoidPos2, avoidBuffer))
//                continue;

//            std::vector<CGeoPoint> pointList;
//            pointList.push_back(_avoidPos);
//            pointList.push_back(_avoidPos2);
//            pointList.push_back(_ballPos);

//            for(int i = 0; i < pointList.size(); i++ ){
//                if(pointList[i].dist(pointList[(i + 1) % pointList.size()]) < 300) continue;
//                if(isInStripZone(candidate, pointList[i], pointList[(i + 1) % pointList.size()], stripBuffer)){
//                    inStripZone = true;
//                    break;
//                }
//            }
//            if(inStripZone)
//            {
//                inStripZone = false;
//                continue;
//            }

//            std::vector<float> posScores = evaluateFunc(_pVision,candidate);
//            float score = posScores.front();
//            if(score > bestScore) {
//                bestScore = score;
//                bestCandidate = candidate;
//            }
//        }
//    }

//    areaRawPoint[areaNum].pos     = bestCandidate;
//    areaRawPoint[areaNum].areaNum = areaNum;
//    areaRawPoint[areaNum].score   = bestScore;
//    areaRawPoint[areaNum].isValid = true;


//}

//std::vector<float> NewRunPos::evaluateFunc(const CVisionModule *pVision, const CGeoPoint &candidate)
//{
//    // 1.与对方球门的距离
//    CGeoPoint goal = CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, 0);
//    float distToGoal = (candidate - goal).mod() > NEW_RUNPOS_PARAM::maxDistToGoal ? NEW_RUNPOS_PARAM::maxDistToGoal : (candidate - goal).mod();

//    // 2.射门有效角度
//    float shootAngle = 0;
//    CShootRangeList shootRangeList(pVision, 0, candidate);
//    const CValueRangeList& shootRange = shootRangeList.getShootRange();
//    if (shootRange.size() > 0) {
//        auto bestRange = shootRange.getMaxRangeWidth();
//        if (bestRange && bestRange->getWidth() > PARAM::Field::BALL_SIZE + 50) {	// 要求射门空档足够大
//            shootAngle = bestRange->getWidth() > NEW_RUNPOS_PARAM::maxShootAngle ? NEW_RUNPOS_PARAM::maxShootAngle : bestRange->getWidth();
//        }
//    }

//    // 3.与球的距离
//    const MobileVisionT& ball = pVision->ball();
//    float distToBall = (candidate - ball.Pos()).mod() > NEW_RUNPOS_PARAM::maxDistToBall ? NEW_RUNPOS_PARAM::maxDistToBall : (candidate - ball.Pos()).mod();

//    // 4.Angle to Goal
//    CVector v2 = goal - candidate;
////            float angle2Goal = fabs(fabs(Utils::Normalize(v2.dir())) - PARAM::Math::PI / 4) > NEW_RUNPOS_PARAM::maxAngle2Goal ? NEW_RUNPOS_PARAM::maxAngle2Goal : fabs(fabs(Utils::Normalize(v2.dir())) - PARAM::Math::PI  / 4);
//    float angle2Goal = 0;
//    if (fabs(fabs(Utils::Normalize(v2.dir())) - PARAM::Math::PI / 4) < PARAM::Math::PI / 12) {
//        angle2Goal = PARAM::Math::PI / 12;
//    } else {
//        angle2Goal = fabs(fabs(Utils::Normalize(v2.dir())) - PARAM::Math::PI / 4);
//    }

//    // 5.Dist to enemy
//    float dist2Enemy = NEW_RUNPOS_PARAM::maxDist2Enemy;
//    std::vector<int> enemyNumVec;
//    int num = WorldModel::Instance()->getEnemyAmountInArea(candidate, NEW_RUNPOS_PARAM::maxDist2Enemy, enemyNumVec);
//    for (int i = 0; i < num; i++) {
////        dist2Enemy += (NEW_RUNPOS_PARAM::maxDist2Enemy - candidate.dist(pVision->theirPlayer(enemyNumVec.at(i)).Pos()));
//        if (dist2Enemy > candidate.dist(pVision->theirPlayer(enemyNumVec.at(i)).Pos())) {
//            dist2Enemy = candidate.dist(pVision->theirPlayer(enemyNumVec.at(i)).Pos());
//        }
//    }
////    qDebug() << "FUCK DIST IS" << dist2Enemy;

//    // 6. predict their guard
//    float guardMinTime = 9999;
//    CGeoPoint p1(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::GOAL_WIDTH / 2), p2(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH / 2);
//    CGeoSegment shootLine1(candidate, p1), shootLine2(candidate, p2);
//    CGeoPoint p = WorldModel::Instance()->penaltyIntersection(shootLine1),
//              q = WorldModel::Instance()->penaltyIntersection(shootLine2);
//    WorldModel::Instance()->normalizeCoordinate(p);
//    WorldModel::Instance()->normalizeCoordinate(q);
//    for(int i = 1; i < PARAM::Field::MAX_PLAYER; i++) {
//        const PlayerVisionT& enemy = pVision->theirPlayer(i);
//        if (!Utils::InTheirPenaltyArea(enemy.Pos(), 0) && Utils::InTheirPenaltyArea(enemy.Pos(), 50)) {
//            float pTime = WorldModel::Instance()->preditTheirGuard(enemy, p);
//            float qTime = WorldModel::Instance()->preditTheirGuard(enemy, q);
//            if (pTime < guardMinTime) {
//                guardMinTime = pTime;
//            }
//            if (qTime < guardMinTime) {
//                guardMinTime = qTime;
//            }
//        }
//    }
//    //7. sector behind enemy
////    static const float maxDist = 300;
////    static const float maxDir = PARAM::Math::PI / 180 * 10;
//    float sectorScore;
//    float candiDir = NEW_RUNPOS_PARAM::maxSectorDir;
//    float candiDist = NEW_RUNPOS_PARAM::maxSectorDist;
////    float candiDir = (candidate - ball.Pos()).dir();
////    float candiDist = candidate.dist(ball.Pos());
//    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
//        if(pVision->theirPlayer(i).Valid()) {
//            const PlayerVisionT& enemy = pVision->theirPlayer(i);
//            float baseDir = (enemy.Pos() - ball.Pos()).dir();
//            float baseDist = enemy.Pos().dist(ball.Pos());
////            if (fabs(Utils::Normalize(candiDir - baseDir)) < NEW_RUNPOS_PARAM::maxSectorDir && candiDist > baseDist) {
////                candiDir = fabs(Utils::Normalize(candiDir - baseDir));
////                candiDist = (candiDist - baseDist) > NEW_RUNPOS_PARAM::maxSectorDist ? NEW_RUNPOS_PARAM::maxSectorDist : candiDist - baseDist;
////            }
//            if (fabs(Utils::Normalize((candidate - ball.Pos()).dir() - baseDir)) < NEW_RUNPOS_PARAM::maxSectorDir && candidate.dist(ball.Pos()) > baseDist) {
//                candiDir = std::min(fabs(Utils::Normalize((candidate - ball.Pos()).dir() - baseDir)), double(candiDir));
//                candiDist = std::min(candidate.dist(ball.Pos()), double(candiDist));
//            }
//        }
//    }

//    // 归一化处理
//    distToGoal = 1 - distToGoal/NEW_RUNPOS_PARAM::maxDistToGoal;
//    shootAngle = shootAngle/NEW_RUNPOS_PARAM::maxShootAngle;
//    distToBall = distToBall/NEW_RUNPOS_PARAM::maxDistToBall;
//    angle2Goal = 1 - angle2Goal / NEW_RUNPOS_PARAM::maxAngle2Goal;
//    dist2Enemy = dist2Enemy > NEW_RUNPOS_PARAM::maxDist2Enemy ? 1 : dist2Enemy / NEW_RUNPOS_PARAM::maxDist2Enemy;
//    guardMinTime = guardMinTime > 999 ? 0 : guardMinTime > NEW_RUNPOS_PARAM::maxGuardTime ? 1.0 : guardMinTime / NEW_RUNPOS_PARAM::maxGuardTime;
//    sectorScore = candiDir / NEW_RUNPOS_PARAM::maxSectorDir * 0.3f + candiDist / NEW_RUNPOS_PARAM::maxSectorDist * 0.7f;

//    // 计算得分
//    std::vector<float> scores;
//    scores.push_back(NEW_RUNPOS_PARAM::weight1 * distToGoal +
//                     NEW_RUNPOS_PARAM::weight2 * shootAngle +
//                     NEW_RUNPOS_PARAM::weight3 * distToBall +
//                     NEW_RUNPOS_PARAM::weight4 * angle2Goal +
//                     NEW_RUNPOS_PARAM::weight5 * dist2Enemy +
//                     NEW_RUNPOS_PARAM::weight6 * guardMinTime +
//                     NEW_RUNPOS_PARAM::weight7 * sectorScore);
//    scores.push_back(NEW_RUNPOS_PARAM::weight1 * distToGoal);
//    scores.push_back(NEW_RUNPOS_PARAM::weight2 * shootAngle);
//    scores.push_back(NEW_RUNPOS_PARAM::weight3 * distToBall);
//    scores.push_back(NEW_RUNPOS_PARAM::weight4 * angle2Goal);
//    scores.push_back(NEW_RUNPOS_PARAM::weight5 * dist2Enemy);
//    scores.push_back(NEW_RUNPOS_PARAM::weight6 * guardMinTime);
//    scores.push_back(NEW_RUNPOS_PARAM::weight7 * sectorScore);
//    return scores;
//}

//double NewRunPos::calculateArea(int areaNum)
//{
//    return (ATTACK_AREA[areaNum]._point[2].x() - ATTACK_AREA[areaNum]._point[1].x()) * (ATTACK_AREA[areaNum]._point[1].y() - ATTACK_AREA[areaNum]._point[0].y());
//}

//bool NewRunPos::isInRangeOfPos(CGeoPoint &_pos1, CGeoPoint &_pos2, float threshold)
//{
//    if(_pos1.dist(_pos2) < threshold) return true;
//    else return false;
//}

//bool NewRunPos::isInStripZone(CGeoPoint &testPos, CGeoPoint &_pos1, CGeoPoint &_pos2, float threshold)
//{
//    CGeoLine testline(_pos1, _pos2);
//    float stripWidth = threshold;

//    if(testline.b() == 0){
//        if(abs(testPos.x() - _pos1.x()) < stripWidth) return true;
//        else return false;
//    }
//    else{
//        float y = -testline.a() / testline.b() * testPos.x() - testline.c() / testline.b();
//        if(abs(y - testPos.y()) < stripWidth) return true;
//        else return false;
//    }

//}

//void NewRunPos::judgeRunPosValid()
//{
//    for(int i = 0; i < ATTACK_AREA_NUM; i++)
//    {
//        _runPos[i] = areaRawPoint[i];
//    }
//}


