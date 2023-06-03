#include "runpos.h"
#include "ShootRangeList.h"
#include "WorldModel.h"
#include "parammanager.h"
#include "ThreadPool.h"


namespace  {
bool USE_11vs11SOLVER;
int ATTACK_AREA_NUM = 8;
CGeoRectangle MIDDLE_AREA_LEFT(0,-PARAM::Field::PITCH_WIDTH/2,PARAM::Field::PITCH_LENGTH/4,0);
CGeoRectangle MIDDLE_AREA_MID(0,-PARAM::Field::PITCH_WIDTH/6,PARAM::Field::PITCH_LENGTH/4,PARAM::Field::PITCH_WIDTH/6);
CGeoRectangle MIDDLE_AREA_RIGHT(0,0,PARAM::Field::PITCH_LENGTH/4,PARAM::Field::PITCH_WIDTH/2);
CGeoRectangle FRONT_AREA_LEFT(PARAM::Field::PITCH_LENGTH/4,-PARAM::Field::PITCH_WIDTH/2,PARAM::Field::PITCH_LENGTH/2,0);
CGeoRectangle FRONT_AREA_MID(PARAM::Field::PITCH_LENGTH/4,-PARAM::Field::PITCH_WIDTH/6,PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PITCH_WIDTH/6);
CGeoRectangle FRONT_AREA_RIGHT(PARAM::Field::PITCH_LENGTH/4,0,PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PITCH_WIDTH/2);
CGeoRectangle BACK_AREA_LEFT(-PARAM::Field::PITCH_LENGTH/4,-PARAM::Field::PITCH_WIDTH/2,0,0);
CGeoRectangle BACK_AREA_RIGHT(-PARAM::Field::PITCH_LENGTH/4,0,0,PARAM::Field::PITCH_WIDTH/2);
CGeoRectangle ATTACK_AREA[8] = {MIDDLE_AREA_LEFT,MIDDLE_AREA_MID,MIDDLE_AREA_RIGHT,FRONT_AREA_LEFT,FRONT_AREA_MID,FRONT_AREA_RIGHT,BACK_AREA_LEFT,BACK_AREA_RIGHT};
runPosProperties areaRawPoint[8];
int runPosCycle = 0;
}

runpos::runpos():_pVision(nullptr),pool1(8){
    ZSS::ZParamManager::instance()->loadParam(USE_11vs11SOLVER, "Messi/USE_11vs11SOLVER", false);
    if(!USE_11vs11SOLVER) ATTACK_AREA_NUM = 6;
    for(int i=0;i<ATTACK_AREA_NUM;++i){
        _runPos[i].pos = CGeoPoint((ATTACK_AREA[i]._point[0].x() + ATTACK_AREA[i]._point[2].x())/2,(ATTACK_AREA[i]._point[0].y() + ATTACK_AREA[i]._point[2].y())/2);
        _runPos[i].areanum = i;
    }
}

bool runpos::isInRangeOfPos(CGeoPoint &pos1, CGeoPoint &pos2, float threshold){
    return pos1.dist(pos2) < threshold;
}

bool runpos::isInStripZone(CGeoPoint &testpos, CGeoPoint &pos1, CGeoPoint &pos2, float threshold){
    CGeoLine testline(pos1,pos2);
    if(testline.b()==0){
        return abs(testpos.x() - pos1.x()) < threshold;
    }else{
        float y = -testline.a()/testline.b() * testpos.x() + testline.c()/testline.b();
        return abs(y - testpos.y()) < threshold;
    }
}

void runpos::generateRunPos(const CVisionModule *pVision, const CGeoPoint &avoidPos1, const CGeoPoint &avoidPos2){
    _pVision = pVision;
    _avoidPos1 = avoidPos1;
    _avoidPos2 = avoidPos2;
    _ballPos = pVision->ball().Pos();

    generateBroad();

    for(int i = 0;i<ATTACK_AREA_NUM;++i){

//        results1.emplace_back(
//                    pool1.enqueue([this,i]{
                     selectAreaBestPoint(i);
                     //return 0;
//                    })
//                    );

//        if(Utils::IsInField(_avoidPos1) || Utils::IsInField(_avoidPos2)){
//            if(ATTACK_AREA[i].HasPoint(_avoidPos1) || ATTACK_AREA[i].HasPoint(_avoidPos2)){
//                areaRawPoint[i].isValid = false;
//            }
//        }
    }

    judgeRunPosValid();
}

void runpos::generateBroad(){
   const MobileVisionT &ball = _pVision->ball();
   float bx = ball.X();
   float by = ball.Y();
   if(USE_11vs11SOLVER){
       if(bx > 0){
            MIDDLE_AREA_LEFT  = CGeoRectangle(0, -PARAM::Field::PITCH_WIDTH/2, PARAM::Field::PITCH_LENGTH/4, -PARAM::Field::PITCH_WIDTH/6);
            MIDDLE_AREA_MID   = CGeoRectangle(0, -PARAM::Field::PITCH_WIDTH/6, PARAM::Field::PITCH_LENGTH/4,  PARAM::Field::PITCH_WIDTH/6);
            MIDDLE_AREA_RIGHT = CGeoRectangle(0,  PARAM::Field::PITCH_WIDTH/6, PARAM::Field::PITCH_LENGTH/4,  PARAM::Field::PITCH_WIDTH/2);
            FRONT_AREA_LEFT   = CGeoRectangle(PARAM::Field::PITCH_LENGTH/4, -PARAM::Field::PITCH_WIDTH/2, PARAM::Field::PITCH_LENGTH/2, -PARAM::Field::PITCH_WIDTH/6);
            FRONT_AREA_MID    = CGeoRectangle(PARAM::Field::PITCH_LENGTH/4, -PARAM::Field::PITCH_WIDTH/6, PARAM::Field::PITCH_LENGTH/2,  PARAM::Field::PITCH_WIDTH/6);
            FRONT_AREA_RIGHT  = CGeoRectangle(PARAM::Field::PITCH_LENGTH/4,  PARAM::Field::PITCH_WIDTH/6, PARAM::Field::PITCH_LENGTH/2,  PARAM::Field::PITCH_WIDTH/2);
            BACK_AREA_LEFT    = CGeoRectangle(-PARAM::Field::PITCH_LENGTH/4,-PARAM::Field::PITCH_WIDTH/2,0,0);
            BACK_AREA_RIGHT   = CGeoRectangle(-PARAM::Field::PITCH_LENGTH/4,0,0,PARAM::Field::PITCH_WIDTH/2);
       }else{
           MIDDLE_AREA_LEFT  = CGeoRectangle(0, -PARAM::Field::PITCH_WIDTH/2, PARAM::Field::PITCH_LENGTH/4, -PARAM::Field::PITCH_WIDTH/6);
           MIDDLE_AREA_MID   = CGeoRectangle(0, -PARAM::Field::PITCH_WIDTH/6, PARAM::Field::PITCH_LENGTH/4,  PARAM::Field::PITCH_WIDTH/6);
           MIDDLE_AREA_RIGHT = CGeoRectangle(0,  PARAM::Field::PITCH_WIDTH/6, PARAM::Field::PITCH_LENGTH/4,  PARAM::Field::PITCH_WIDTH/2);
           FRONT_AREA_LEFT   = CGeoRectangle(PARAM::Field::PITCH_LENGTH/4, -PARAM::Field::PITCH_WIDTH/2, PARAM::Field::PITCH_LENGTH/2, -PARAM::Field::PITCH_WIDTH/6);
           FRONT_AREA_MID    = CGeoRectangle(PARAM::Field::PITCH_LENGTH/4, -PARAM::Field::PITCH_WIDTH/6, PARAM::Field::PITCH_LENGTH/2,  PARAM::Field::PITCH_WIDTH/6);
           FRONT_AREA_RIGHT  = CGeoRectangle(PARAM::Field::PITCH_LENGTH/4,  PARAM::Field::PITCH_WIDTH/6, PARAM::Field::PITCH_LENGTH/2,  PARAM::Field::PITCH_WIDTH/2);
           BACK_AREA_LEFT    = CGeoRectangle(-PARAM::Field::PITCH_LENGTH/4,-PARAM::Field::PITCH_WIDTH/2,0,0);
           BACK_AREA_RIGHT   = CGeoRectangle(-PARAM::Field::PITCH_LENGTH/4,0,0,PARAM::Field::PITCH_WIDTH/2);
       }
       ATTACK_AREA[0] = MIDDLE_AREA_LEFT;
       ATTACK_AREA[1] = MIDDLE_AREA_MID;
       ATTACK_AREA[2] = MIDDLE_AREA_RIGHT;
       ATTACK_AREA[3] = FRONT_AREA_LEFT;
       ATTACK_AREA[4] = FRONT_AREA_MID;
       ATTACK_AREA[5] = FRONT_AREA_RIGHT;
       ATTACK_AREA[6] = BACK_AREA_LEFT;
       ATTACK_AREA[7] = BACK_AREA_RIGHT;
   }else{
       if(bx > 0){
            MIDDLE_AREA_LEFT  = CGeoRectangle(0,-PARAM::Field::PITCH_WIDTH/2,PARAM::Field::PITCH_LENGTH/4,0);
            MIDDLE_AREA_RIGHT = CGeoRectangle(0,0,PARAM::Field::PITCH_LENGTH/4,PARAM::Field::PITCH_WIDTH/2);
            FRONT_AREA_LEFT   = CGeoRectangle(PARAM::Field::PITCH_LENGTH/4,-PARAM::Field::PITCH_WIDTH/2,PARAM::Field::PITCH_LENGTH/2,0);
            FRONT_AREA_RIGHT  = CGeoRectangle(PARAM::Field::PITCH_LENGTH/4,0,PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PITCH_WIDTH/2);
            BACK_AREA_LEFT    = CGeoRectangle(-PARAM::Field::PITCH_LENGTH/4,-PARAM::Field::PITCH_WIDTH/2,0,0);
            BACK_AREA_RIGHT   = CGeoRectangle(-PARAM::Field::PITCH_LENGTH/4,0,0,PARAM::Field::PITCH_WIDTH/2);
       }else{
           MIDDLE_AREA_LEFT  = CGeoRectangle(0,-PARAM::Field::PITCH_WIDTH/2,PARAM::Field::PITCH_LENGTH/4,0);
           MIDDLE_AREA_RIGHT = CGeoRectangle(0,0,PARAM::Field::PITCH_LENGTH/4,PARAM::Field::PITCH_WIDTH/2);
           FRONT_AREA_LEFT   = CGeoRectangle(PARAM::Field::PITCH_LENGTH/4,-PARAM::Field::PITCH_WIDTH/2,PARAM::Field::PITCH_LENGTH/2,0);
           FRONT_AREA_RIGHT  = CGeoRectangle(PARAM::Field::PITCH_LENGTH/4,0,PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PITCH_WIDTH/2);
           BACK_AREA_LEFT    = CGeoRectangle(-PARAM::Field::PITCH_LENGTH/4,-PARAM::Field::PITCH_WIDTH/2,0,0);
           BACK_AREA_RIGHT   = CGeoRectangle(-PARAM::Field::PITCH_LENGTH/4,0,0,PARAM::Field::PITCH_WIDTH/2);
       }
       ATTACK_AREA[0] = MIDDLE_AREA_LEFT;
       ATTACK_AREA[1] = MIDDLE_AREA_RIGHT;
       ATTACK_AREA[2] = FRONT_AREA_LEFT;
       ATTACK_AREA[3] = FRONT_AREA_RIGHT;
       ATTACK_AREA[4] = BACK_AREA_LEFT;
       ATTACK_AREA[5] = BACK_AREA_RIGHT;
   }
}

void runpos::selectAreaBestPoint(int areaNum){
    CGeoPoint bestCandidate(99999,99999);
    float bestScore = -9999;
    float avoidBuffer = 1500;
    float stripBuffer = 1200;
    bool isStripZone = false;
    for(float bx = ATTACK_AREA[areaNum]._point[0].x();bx<ATTACK_AREA[areaNum]._point[2].x();bx+=400){
        for(float by = ATTACK_AREA[areaNum]._point[0].y();by<ATTACK_AREA[areaNum]._point[1].y();by+=400){
            CGeoPoint candidate(bx,by);

            //排除距离场外过近的点
            if(!Utils::IsInField(candidate,280)) continue;

            //排除距离敌方禁区过近的点
            if(Utils::InTheirPenaltyArea(candidate,290)) continue;

            //排除离lederpos 和 receiverpos 过近的点
            if(isInRangeOfPos(candidate,_avoidPos1,avoidBuffer) || isInRangeOfPos(candidate,_avoidPos2,avoidBuffer)) continue;

            std::vector<CGeoPoint> posList;
            posList.push_back(_avoidPos1);
            posList.push_back(_avoidPos2);
            posList.push_back(_ballPos);

            for(int i = 0;i<posList.size();++i){
                if(posList[i].dist(posList[(i+1)%posList.size()]) < 300) continue;
                //不进leaderpos 和 receiverpos 连线胶囊区的点
                if(isInStripZone(candidate,posList[i],posList[i+1],stripBuffer)){
                    isStripZone = true;
                    break;
                }
            }

            if(isStripZone){
                isStripZone = false;
                continue;
            }

            std::vector<float> passScores = evaluateFunc(_pVision,candidate);
            if(passScores.front() > bestScore){
                bestScore = passScores.front();
                bestCandidate = candidate;
            }
        }
    }

    //if(bestCandidate.dist(CGeoPoint(99999,99999)) > 1e8){
        areaRawPoint[areaNum].pos = bestCandidate;
        areaRawPoint[areaNum].areanum = areaNum;
        areaRawPoint[areaNum].scores = bestScore;
        areaRawPoint[areaNum].isValid = true;
    //}
}

std::vector<float> runpos::evaluateFunc(const CVisionModule *pVision, const CGeoPoint &candidate){
    //1.与对方球门的距离
    CGeoPoint goal(PARAM::Field::PITCH_LENGTH/2,0);
    float dist2Goal = goal.dist(candidate);

    //2.射门有效角度
    float shootAngle = 0;
    CShootRangeList shootRangList(pVision,0,candidate);
    const CValueRangeList &shootRange = shootRangList.getShootRange();
    if (shootRange.size() > 0) {
        auto bestRange = shootRange.getMaxRangeWidth();
        if (bestRange && bestRange->getWidth() > PARAM::Field::BALL_SIZE + 50) {	// 要求射门空档足够大
            shootAngle = bestRange->getWidth() > RUNPOS_PARAM::maxShootAngle ? RUNPOS_PARAM::maxShootAngle : bestRange->getWidth();
        }
    }

    //3.计算与球的距离
    const MobileVisionT &ball = _pVision->ball();
    float dist2Ball = ball.Pos().dist(candidate) > RUNPOS_PARAM::maxDistToBall ? RUNPOS_PARAM::maxDistToBall : candidate.dist(ball.Pos());

    //4.best angle 2 goal
    CVector v2 = goal - candidate;
    float angle2Goal = 0;
    if(fabs(fabs(Utils::Normalize(v2.dir())) - PARAM::Math::PI/4) < PARAM::Math::PI/12){
        angle2Goal = PARAM::Math::PI/12;
    } else {
        angle2Goal = fabs(fabs(Utils::Normalize(v2.dir())) - PARAM::Math::PI/4);
    }

    //5.dist 2 enemy
    float dist2enemy = RUNPOS_PARAM::maxDist2Enemy;
    std::vector<int> enemyNumVec;
    int num = WorldModel::Instance()->getEnemyAmountInArea(candidate,RUNPOS_PARAM::maxDist2Enemy,enemyNumVec);
    for(int i = 0;i < num; ++i){
        if(dist2enemy > candidate.dist(pVision->theirPlayer(enemyNumVec.at(i)).Pos())){
            dist2Ball = candidate.dist(pVision->theirPlayer(enemyNumVec.at(i)).Pos());
        }
    }

    //6.predict for their guard
    float guardMinTime = 9999;
    CGeoPoint p1(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::GOAL_WIDTH / 2), p2(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH / 2);
    CGeoSegment shootLine1(candidate, p1), shootLine2(candidate, p2);
    CGeoPoint p = WorldModel::Instance()->penaltyIntersection(shootLine1),
              q = WorldModel::Instance()->penaltyIntersection(shootLine2);
    WorldModel::Instance()->normalizeCoordinate(p);
    WorldModel::Instance()->normalizeCoordinate(q);
    for(int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        const PlayerVisionT& enemy = pVision->theirPlayer(i);
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

    //7.sector behind enemy
    float sectorScore;
    float candiDir = RUNPOS_PARAM::maxSectorDir;
    float candiDist = RUNPOS_PARAM::maxSectorDist;
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        if(pVision->theirPlayer(i).Valid()) {
            const PlayerVisionT& enemy = pVision->theirPlayer(i);
            float baseDir = (enemy.Pos() - ball.Pos()).dir();
            float baseDist = enemy.Pos().dist(ball.Pos());
            if (fabs(Utils::Normalize((candidate - ball.Pos()).dir() - baseDir)) < RUNPOS_PARAM::maxSectorDir && candidate.dist(ball.Pos()) > baseDist) {
                candiDir = std::min(fabs(Utils::Normalize((candidate - ball.Pos()).dir() - baseDir)), double(candiDir));
                candiDist = std::min(candidate.dist(ball.Pos()), double(candiDist));
            }
        }
    }

    // 归一化处理
    dist2Goal = 1 - dist2Goal/RUNPOS_PARAM::maxDistToGoal;
    shootAngle = shootAngle/RUNPOS_PARAM::maxShootAngle;
    dist2Ball = dist2Ball/RUNPOS_PARAM::maxDistToBall;
    angle2Goal = 1 - angle2Goal / RUNPOS_PARAM::maxAngle2Goal;
    dist2enemy = dist2enemy > RUNPOS_PARAM::maxDist2Enemy ? 1 : dist2enemy / RUNPOS_PARAM::maxDist2Enemy;
    guardMinTime = guardMinTime > 999 ? 0 : guardMinTime > RUNPOS_PARAM::maxGuardTime ? 1.0 : guardMinTime / RUNPOS_PARAM::maxGuardTime;
    sectorScore = candiDir / RUNPOS_PARAM::maxSectorDir * 0.3f + candiDist / RUNPOS_PARAM::maxSectorDist * 0.7f;


    //计算评分
    std::vector<float> scores;
    scores.push_back(RUNPOS_PARAM::weight1 * dist2Goal  +
                     RUNPOS_PARAM::weight2 * shootAngle +
                     RUNPOS_PARAM::weight3 * dist2Ball  +
                     RUNPOS_PARAM::weight4 * angle2Goal +
                     RUNPOS_PARAM::weight5 * dist2enemy +
                     RUNPOS_PARAM::weight6 * guardMinTime+
                     RUNPOS_PARAM::weight7 * sectorScore);
    scores.push_back(RUNPOS_PARAM::weight1 * dist2Goal);
    scores.push_back(RUNPOS_PARAM::weight2 * shootAngle);
    scores.push_back(RUNPOS_PARAM::weight3 * dist2Ball);
    scores.push_back(RUNPOS_PARAM::weight4 * angle2Goal);
    scores.push_back(RUNPOS_PARAM::weight5 * dist2enemy);
    scores.push_back(RUNPOS_PARAM::weight6 * guardMinTime);
    scores.push_back(RUNPOS_PARAM::weight7 * sectorScore);
    return scores;
}

void runpos::judgeRunPosValid(){
    //每隔2s重新算优先级
    if(_pVision->getCycle() - runPosCycle > 120){
        for(int i = 0;i < ATTACK_AREA_NUM -1; i++){
            for(int j = 0;j < ATTACK_AREA_NUM -1; j++){
                runPosProperties temp;
                if(areaRawPoint[j].scores < areaRawPoint[j+1].scores){
//                        temp = areaRawPoint[j];
//                        areaRawPoint[j] = areaRawPoint[j+1];
//                        areaRawPoint[j+1] = temp;
                    memcpy(&temp,&areaRawPoint[j],sizeof (runPosProperties));
                    memcpy(&areaRawPoint[j],&areaRawPoint[j+1],sizeof (runPosProperties));
                    memcpy(&areaRawPoint[j+1],&temp,sizeof (runPosProperties));
                }
            }
        }
        runPosCycle = _pVision->getCycle();
    }


    for(int i = 0;i < ATTACK_AREA_NUM; ++i){
        _runPos[i] = areaRawPoint[i];
    }
}
