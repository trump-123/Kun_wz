#include "passposevaluate.h"
#include "staticparams.h"
#include "Global.h"
#include "ShootRangeList.h"
#include "runpos.h"

namespace  {
    namespace PASS_PARAM {
    const float wDist = 0.15f;
    const float wPassLineDist = 0.1;
    const float wTurnAngle = 0.1;
    const float wClosestEnemyDist = 0.10f;
    const float wUnderPass = 0;
    const float wCandirect = 0.15;
    const float minPassDist = 1500;
    }
    namespace SHOOT_PARAM {
    const float wShootAngle = 0.15;
    const float wDist = 0.2;
    const float wRefracAngle = 0.15;
    const float wAngle2Goal = 0.1;
    const float wPassLineDist = 0.1;
    const float wGuardTime = 0.073f;
    }

    namespace FREE_KICK_PARAM_EVOLVE {
    float wShootAngle = 0.3f;
    float wClosestEnemyDist = 0.1f;
    float wDist = 0.2;
    float wRefracAngle = 0.0f;
    float wAngle2Goal = 0.0;
    float wGuardTime = 0.1f;
    float wSector = 0.0f;
    float wPassLineDist = 0.4f;
    float min2PenaltyDist = 50;
    float wshoot_dir = 0.3;
    float wShootLineDist = 0.3f;
    }

}

CPassPosEvaluate::CPassPosEvaluate()
{

}

std::vector <float> CPassPosEvaluate::evaluateFunc(CGeoPoint candidate, CGeoPoint leaderPos, EvaluateMode mode){
    std::vector<float> scores;
    CGeoPoint p0(candidate), p1(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::GOAL_WIDTH / 2), p2(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH / 2), p3(PARAM::Field::PITCH_LENGTH/2,0);

    //计算有效射门角度
    float shootAngle = 0;
    CShootRangeList shootRangeList(vision, 1, p0);
    const CValueRangeList& shootRange = shootRangeList.getShootRange();
    if (shootRange.size() > 0) {
        auto bestRange = shootRange.getMaxRangeWidth();
        if(bestRange) {
            shootAngle = bestRange->getWidth();
        }
    }

    //计算射门折射角，角度越小越适合touch touch角最大为60度
    CVector v1 = leaderPos - p0;
    CVector v2 = p1.midPoint(p2) - p0;
    float refracAngle = fabs(v1.dir() - v2.dir());
    refracAngle = refracAngle > PARAM::Math::PI ? 2*PARAM::Math::PI - refracAngle : refracAngle;

    //计算距离对方球门的距离
    float defDist = fabs(p0.dist(CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0)));
    float Angle2Goal = fabs(fabs(Utils::Normalize(v2.dir())) - PARAM::Math::PI / 4);
    float Angle2Goal4FreeKick = (fabs(Utils::Normalize(v2.dir())) < PARAM::Math::PI / 4.0f && fabs(Utils::Normalize(v2.dir())) > PARAM::Math::PI / 9.0f) ? 1 : 0;

    //下底传中
    float underPass = (p0.x() < PARAM::Field::PITCH_LENGTH/2 && p0.x() > PARAM::Field::PITCH_LENGTH/2 - PARAM::Field::PENALTY_AREA_DEPTH) ? 1 : 0;

    //预测后卫
    float guardMinTime = 9999;
    CGeoSegment shootLine1(p0, p1), shootLine2(p0, p2);
    CGeoPoint p = WorldModel::Instance()->penaltyIntersection(shootLine1),
              q = WorldModel::Instance()->penaltyIntersection(shootLine2);
    WorldModel::Instance()->normalizeCoordinate(p);
    WorldModel::Instance()->normalizeCoordinate(q);
    for(int i = 1; i < PARAM::Field::MAX_PLAYER; i++) {
        const PlayerVisionT& enemy = vision->theirPlayer(i);
        if (!Utils::InTheirPenaltyArea(enemy.Pos(), 0) && Utils::InTheirPenaltyArea(enemy.Pos(), PARAM::Vehicle::V2::PLAYER_SIZE*6)) {
            float pTime = WorldModel::Instance()->preditTheirGuard(enemy, p);
            float qTime = WorldModel::Instance()->preditTheirGuard(enemy, q);
            if (pTime < guardMinTime) {
                guardMinTime = pTime;
            }
            if (qTime < guardMinTime) {
                guardMinTime = qTime;
            }
        }
    }

    float sectorScore;
    float candiDir = RUNPOS_PARAM::maxSectorDir;
    float candiDist = RUNPOS_PARAM::maxSectorDist;
//    float candiDir = (candidate - ball.Pos()).dir();
//    float candiDist = candidate.dist(ball.Pos());
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        if(vision->theirPlayer(i).Valid()) {
            const PlayerVisionT& enemy = vision->theirPlayer(i);
            float baseDir = (enemy.Pos() - vision->ball().Pos()).dir();
            float baseDist = enemy.Pos().dist(vision->ball().Pos());
//            if (fabs(Utils::Normalize(candiDir - baseDir)) < RUNPOS_PARAM::maxSectorDir && candiDist > baseDist) {
//                candiDir = fabs(Utils::Normalize(candiDir - baseDir));
//                candiDist = (candiDist - baseDist) > RUNPOS_PARAM::maxSectorDist ? RUNPOS_PARAM::maxSectorDist : candiDist - baseDist;
//            }
            if (fabs(Utils::Normalize((candidate - vision->ball().Pos()).dir() - baseDir)) < RUNPOS_PARAM::maxSectorDir && candidate.dist(vision->ball().Pos()) > baseDist) {
                candiDir = std::min(fabs(Utils::Normalize((candidate - vision->ball().Pos()).dir() - baseDir)), double(candiDir));
                candiDist = std::min(candidate.dist(vision->ball().Pos()), double(candiDist));
            }
        }
    }

    float passLineDist = 9999;
    float shootLineDist = 9999;
    float closestEnemyDist = 9999;
    float closestEnemyDist2me = 9999;
    for(int i = 0; i<PARAM::Field::MAX_PLAYER;i++){
        if(!vision->theirPlayer(i).Valid()) continue;
        if(Utils::InTheirPenaltyArea(vision->theirPlayer(i).Pos(),0)) continue;
        if(vision->theirPlayer(i).Pos().dist(p0)<closestEnemyDist)
            closestEnemyDist = vision->theirPlayer(i).Pos().dist(p0);
        if(vision->theirPlayer(i).Pos().dist(leaderPos) < closestEnemyDist2me)
            closestEnemyDist2me = vision->theirPlayer(i).Pos().dist(leaderPos);
    }
    CGeoSegment BallLine(vision->ball().Pos(),p0);
    for(int i = 0;i<PARAM::Field::MAX_PLAYER;++i){
        if(!vision->theirPlayer(i).Valid())  continue;
        CGeoPoint targetPos = vision->theirPlayer(i).Pos();
        float dist = std::min(BallLine.dist2Point(targetPos),1000.0);
        if(dist < passLineDist)
            passLineDist = dist;
    }

    CGeoSegment ShootLine(p0,p3);
    for(int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        if(!vision->theirPlayer(i ).Valid())
            continue;
        CGeoPoint targetPos = vision->theirPlayer(i ).Pos();
        float dist = std::min(ShootLine.dist2Point(targetPos), 300.0);
        if(dist < shootLineDist)
            shootLineDist = dist;
    }

    CGeoPoint ballPos = vision->ball().Pos();
    CVector leader2Ball =ballPos - leaderPos;
    CVector leader2PassPos = p0 - leaderPos;
    float turnAngle = fabs(leader2Ball.dir() - leader2PassPos.dir());
    turnAngle = turnAngle > PARAM::Math::PI ? 2 * PARAM::Math::PI - turnAngle : turnAngle;
    if(ballPos.x() < leaderPos.x()) turnAngle = PARAM::Math::PI;

    float candirec = 99999;
    for(int i = 0; i<PARAM::Field::MAX_PLAYER;i++){
        if(!vision->theirPlayer(i).Valid()) continue;
        if(Utils::InTheirPenaltyArea(vision->theirPlayer(i).Pos(),0)) continue;
        const PlayerVisionT &enemy = vision->theirPlayer(i);
        CGeoSegment passLine(ballPos,candidate);
        CGeoPoint projectionpos = passLine.projection(enemy.Pos());
        if(passLine.IsPointOnLineOnSegment(projectionpos)){
            if(candirec > projectionpos.dist(enemy.Pos()))
                candirec = projectionpos.dist(enemy.Pos());
        }
    }

    CVector shoot_dis = candidate - CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0);
    double shoot_length = PARAM::Field::PITCH_LENGTH/2 - candidate.x();
    double shoot_dir_sin = abs(shoot_length)/(shoot_dis.mod());

    //归一化
    //*********   射门点评分   ******************//
    shootAngle = shootAngle > 25 ? 1 : shootAngle/25;
    refracAngle = refracAngle < PARAM::Math::PI/6 ? PARAM::Math::PI/6 : refracAngle;
    refracAngle = defDist < PARAM::Field::PITCH_LENGTH/2 ? 1 - refracAngle/PARAM::Math::PI : 0;  // 折射角在一定范围内给固定奖励
    defDist = 1 - defDist/PARAM::Field::PITCH_LENGTH;
    Angle2Goal = 1 - Angle2Goal/PARAM::Math::PI;
    guardMinTime = guardMinTime > 999 ? 0 : guardMinTime > 2.0 ? 2.0 : guardMinTime / 2.0;
    sectorScore = candiDir / RUNPOS_PARAM::maxSectorDir * 0.3f + candiDist / RUNPOS_PARAM::maxSectorDist * 0.7f;
    passLineDist = passLineDist > 750 ? 1 : passLineDist /750;
    shootLineDist = shootLineDist > 400 ? 1 : shootLineDist / 400;
    //*********   传球点评分   ******************//
    turnAngle = turnAngle < PARAM::Math::PI/12 ?PARAM::Math::PI/12 :turnAngle;
    turnAngle = 1 - turnAngle/PARAM::Math::PI;
    closestEnemyDist = closestEnemyDist > 300 ? 1 : (closestEnemyDist < 150 ? 0 : (closestEnemyDist - 150)/150);
    candirec = candirec < PARAM::Vehicle::V2::PLAYER_SIZE * 4 ? candirec/PARAM::Vehicle::V2::PLAYER_SIZE * 4 : 1;

    float totalWeight = 0;
    float totalWeight_evolve = 0;

    switch (mode) {
    case PASS:
        totalWeight = PASS_PARAM::wDist + PASS_PARAM::wPassLineDist //+ PASS_PARAM::wCandirect
                + PASS_PARAM::wTurnAngle + PASS_PARAM::wClosestEnemyDist + PASS_PARAM::wUnderPass * underPass;
        scores.push_back(1/totalWeight*100 * (PASS_PARAM::wDist * defDist
                         +PASS_PARAM::wPassLineDist * passLineDist
                         +PASS_PARAM::wTurnAngle * turnAngle
                         +PASS_PARAM::wClosestEnemyDist * closestEnemyDist
                         +PASS_PARAM::wUnderPass * underPass));
        scores.push_back(1/totalWeight * 100 * PASS_PARAM::wDist * defDist);
        scores.push_back(1/totalWeight * 100 * PASS_PARAM::wPassLineDist * passLineDist);
        scores.push_back(1/totalWeight * 100 * PASS_PARAM::wTurnAngle * turnAngle);
        scores.push_back(1/totalWeight * 100 * PASS_PARAM::wClosestEnemyDist * closestEnemyDist);
        scores.push_back(1/totalWeight * 100 * PASS_PARAM::wUnderPass * underPass);
        break;
    case SHOOT:
        totalWeight = SHOOT_PARAM::wShootAngle + SHOOT_PARAM::wRefracAngle
                + SHOOT_PARAM::wAngle2Goal + SHOOT_PARAM::wDist + SHOOT_PARAM::wPassLineDist + SHOOT_PARAM::wGuardTime;
        scores.push_back(1/totalWeight * 100 * (SHOOT_PARAM::wShootAngle * shootAngle
                         + SHOOT_PARAM::wRefracAngle * refracAngle
                         + SHOOT_PARAM::wDist * defDist
                         + SHOOT_PARAM::wAngle2Goal * Angle2Goal
                         + SHOOT_PARAM::wPassLineDist * passLineDist
                         + SHOOT_PARAM::wGuardTime * guardMinTime));
        scores.push_back(1/totalWeight * 100 * SHOOT_PARAM::wShootAngle * shootAngle);
        scores.push_back(1/totalWeight * 100 * SHOOT_PARAM::wRefracAngle * refracAngle);
        scores.push_back(1/totalWeight * 100 * SHOOT_PARAM::wDist * defDist);
        scores.push_back(1/totalWeight * 100 * SHOOT_PARAM::wAngle2Goal * Angle2Goal);
        scores.push_back(1/totalWeight * 100 * SHOOT_PARAM::wPassLineDist * passLineDist);
        scores.push_back(1/totalWeight * 100 * SHOOT_PARAM::wGuardTime * guardMinTime);
        break;
    case FREE_KICK:
        totalWeight_evolve = FREE_KICK_PARAM_EVOLVE::wShootAngle + FREE_KICK_PARAM_EVOLVE::wClosestEnemyDist + FREE_KICK_PARAM_EVOLVE::wRefracAngle
                + FREE_KICK_PARAM_EVOLVE::wAngle2Goal + FREE_KICK_PARAM_EVOLVE::wDist + FREE_KICK_PARAM_EVOLVE::wGuardTime + FREE_KICK_PARAM_EVOLVE::wSector + FREE_KICK_PARAM_EVOLVE::wPassLineDist +FREE_KICK_PARAM_EVOLVE::wshoot_dir + FREE_KICK_PARAM_EVOLVE::wShootLineDist;
        scores.push_back(1/totalWeight_evolve * 100 * (FREE_KICK_PARAM_EVOLVE::wShootAngle * shootAngle
                         + FREE_KICK_PARAM_EVOLVE::wClosestEnemyDist * closestEnemyDist
                         + FREE_KICK_PARAM_EVOLVE::wRefracAngle * refracAngle
                         + FREE_KICK_PARAM_EVOLVE::wDist * defDist
                         + FREE_KICK_PARAM_EVOLVE::wAngle2Goal * Angle2Goal4FreeKick
                         + FREE_KICK_PARAM_EVOLVE::wGuardTime * guardMinTime
                         + FREE_KICK_PARAM_EVOLVE::wSector * sectorScore
                         + FREE_KICK_PARAM_EVOLVE::wPassLineDist * passLineDist
                         + FREE_KICK_PARAM_EVOLVE::wshoot_dir * shoot_dir_sin
                         + FREE_KICK_PARAM_EVOLVE::wShootLineDist * shootLineDist));
        scores.push_back(shootAngle);
        scores.push_back(closestEnemyDist);
        scores.push_back(refracAngle);
        scores.push_back(defDist);
        scores.push_back(Angle2Goal4FreeKick);
        scores.push_back(guardMinTime);
        scores.push_back(sectorScore);
        scores.push_back(passLineDist);
        scores.push_back(shoot_dir_sin);
        scores.push_back(shootLineDist);
        break;
    }

    return scores;
}

bool CPassPosEvaluate::passTooClose(CGeoPoint candidate,CGeoPoint leaderPos){
    return leaderPos.dist(candidate) < PASS_PARAM::minPassDist;
}
