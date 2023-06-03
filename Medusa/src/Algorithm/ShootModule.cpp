#include "ShootModule.h"
#include "SkillUtils.h"
#include "parammanager.h"

namespace {
    bool TEST_PASS;
    bool FORCE_SHOOT;
}

CShootModule::CShootModule():_lastCycle(0),isCalculated(0),keepedCanShoot(false),
    keepedBestTarget(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, 0))
{
    ZSS::ZParamManager::instance()->loadParam(TEST_PASS, "Messi/NoShoot", false);
    ZSS::ZParamManager::instance()->loadParam(FORCE_SHOOT, "Messi/FORCE_SHOOT", true);
    ZSS::ZParamManager::instance()->loadParam(tolerance,"Shoot/Tolerance",200);
    ZSS::ZParamManager::instance()->loadParam(stepSize,"Shoot/StepSize",1);
    ZSS::ZParamManager::instance()->loadParam(responseTime,"Shoot/ResponseTime",0);
    ZSS::ZParamManager::instance()->loadParam(agressiveresponseTime,"Shoot/AgressiveResponseTime",0);
    stepSize = stepSize / 180 * PARAM::Math::PI; //转化为弧度制
}

bool CShootModule::generateBestTarget(const CVisionModule *pVision, CGeoPoint &bestTarget, const CGeoPoint& pos)
{
    if (pVision->getCycle() - _lastCycle >= 1) {
        isCalculated = false;
        _lastCycle = pVision->getCycle();
        keepedCanShoot = false;
        keepedBestTarget = CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, 0);
    }
    if(!isCalculated)
    {
        CGeoPoint startPos;
        if (std::abs(pos.x()) > 16000) {
            startPos = pVision->ball().Pos();
        }else {
            startPos = pos;
        }
        bestTarget = CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, 0);
        const CGeoPoint leftPost(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::GOAL_WIDTH / 2 + tolerance); // 左门柱
        const CGeoPoint rightPost(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH / 2 - 0.5 * tolerance); // 右门柱 此处不使用tolerance是因为进行stepsize时会自动留有余量，而左门柱是起点
        const CGeoLine bottomLine = CGeoLine(leftPost, rightPost);
        const double leftPostAngle = (leftPost - startPos).dir();   //vector.dir()都是弧度制
        const double rightPostAngle = (rightPost - startPos).dir();
        const double AngleRange = abs(leftPostAngle - rightPostAngle);
        const double responsetime = Utils::InTheirPenaltyArea(startPos, 800) ? agressiveresponseTime : responseTime;
        if(AngleRange < 8.0 / 180 * PARAM::Math::PI)
        {
            keepedCanShoot = false;
            keepedBestTarget = bestTarget;
            return keepedCanShoot;
        }
        double bestInterTime = -99999;
        CGeoPoint lastTarget = leftPost;
        double lastInterTime, nowInterTime;
        ZSkillUtils::instance()->validShootPos(pVision, startPos, 6500, lastTarget, lastInterTime, responsetime);
        for(int i = 0;  i * stepSize <= AngleRange; i++) {
            double tempDir = leftPostAngle + (leftPostAngle > rightPostAngle ? - i * stepSize : i * stepSize);
            CGeoLine tempLine = CGeoLine(startPos, startPos + Utils::Polar2Vector(10000, tempDir));
            CGeoLineLineIntersection intersect = CGeoLineLineIntersection(tempLine,bottomLine);
            if(intersect.Intersectant()) {
                CGeoPoint tempTarget = intersect.IntersectPoint();
                double interTime;
                ZSkillUtils::instance()->validShootPos(pVision, startPos, 6500, tempTarget, nowInterTime, responsetime);
                //interTime = (lastInterTime + nowInterTime) / 2;
                interTime = nowInterTime;
//                if(isDebug && SHOWALLINFOR) {
//                    GDebugEngine::Instance()->gui_debug_x(tempTarget, COLOR_BLACK);
//                    GDebugEngine::Instance()->gui_debug_msg(tempTarget, QString("%1").arg(nowInterTime).toLatin1(), COLOR_RED);
//                }
                if(interTime > bestInterTime) {
                    bestInterTime = interTime;
//                    bestTarget = CGeoPoint((lastTarget.x() + tempTarget.x()) / 2, (lastTarget.y() + tempTarget.y()) / 2);
                    bestTarget = tempTarget;
                }
                lastTarget = tempTarget;
                lastInterTime = nowInterTime;
            }
        }
//        if(isDebug)
//        {
////            GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,0), QString("%1").arg(bestInterTime).toLatin1(), COLOR_RED);
//            GDebugEngine::Instance()->gui_debug_line(startPos, bestTarget, COLOR_RED);
//        }
        if(bestInterTime < 0)
        {
            keepedCanShoot = false;
            keepedBestTarget = bestTarget;
        }
        else
        {
            keepedCanShoot = true;
            keepedBestTarget = bestTarget;
        }
        return keepedCanShoot;
    }
    else
    {
        bestTarget = keepedBestTarget;
        return keepedCanShoot;
    }
}

double CShootModule::generateBestTimeDiff(const CVisionModule *pVision, CGeoPoint &bestTarget, const CGeoPoint &pos){
    CGeoPoint startPos;
    if(std::abs(pos.x()) > 16000){
        startPos = pVision->ball().Pos();
    }else{
        startPos = pos;
    }
    bestTarget = CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, 0);
    const CGeoPoint leftPost(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::GOAL_WIDTH / 2 + tolerance);
    const CGeoPoint rightPost(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::GOAL_WIDTH / 2 - tolerance);
    const CGeoLine bottomLine(leftPost, rightPost);
    const double leftPosAngle = (leftPost - startPos).dir();
    const double rightPosAngle = (rightPost - startPos).dir();
    const double AngleRange = abs(leftPosAngle - rightPosAngle);
    const double responsetime = Utils::InTheirPenaltyArea(startPos, 800) ? responseTime : agressiveresponseTime;
    if(AngleRange < 8.0 / 180 * PARAM::Math::PI){
        keepedCanShoot = false;
        keepedBestTarget = bestTarget;
        return -99999.0;
    }
    double bestInterTime = -99999;
    CGeoPoint lastTarget = leftPost;
    double lastInterTime, nowInterTime;
    ZSkillUtils::instance()->validShootPos(pVision, startPos, 6500, lastTarget, lastInterTime,responsetime);
    for(int i = 0; i*stepSize < AngleRange; i++){
        double tempDir = leftPosAngle + (leftPosAngle > rightPosAngle ? -i * stepSize : i*stepSize);
        CGeoLine tempLine(startPos, startPos + Utils::Polar2Vector(10000,tempDir));
        CGeoLineLineIntersection intersect(tempLine,bottomLine);
        if(intersect.Intersectant()){
            CGeoPoint tempTarget = intersect.IntersectPoint();
            double interTime;
            //如果点被敌人挡住则取-10
            ZSkillUtils::instance()->validShootPos(pVision, startPos, 6500, tempTarget, nowInterTime, responsetime);
            interTime = nowInterTime;
            if(interTime > bestInterTime){
                bestInterTime = interTime;
                bestTarget = tempTarget;
            }
            lastTarget = tempTarget;
            lastInterTime = nowInterTime;
        }
    }
    return bestInterTime;
}

bool CShootModule::canShoot(const CVisionModule *pVision, CGeoPoint shootPos)
{
    if(TEST_PASS)
        return false;
    //ZPass在禁区附近强制射门
//    if(Utils::InTheirPenaltyArea(shootPos, 3*PARAM::Vehicle::V2::PLAYER_SIZE) &&
//            shootPos.x() <= PARAM::Field::PITCH_LENGTH/2 - PARAM::Field::PENALTY_AREA_DEPTH)
//        return true;
    // 只剩不多于1辆进攻车时，强开射门.
    if (FORCE_SHOOT && pVision->getValidNum() <= 2)
        return true;
    // 在后场不射门
    if (shootPos.x() < 0)
        return false;

    //进行一般判断，出现可行小球路经即return true，否则return false
    CGeoPoint bestTarget = CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, 0);
    return generateBestTarget(pVision, bestTarget, shootPos);
    //return ShootModule::Instance()->GenerateShootDir(robotNum);
}
