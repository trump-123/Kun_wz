#include "guardpos.h"
#include "Global.h"
#include "staticparams.h"
#include "messidecition.h"

namespace  {
const CGeoPoint GOAL_MIDDLE(-PARAM::Field::PITCH_LENGTH/2.0, 0);
const CGeoPoint GOAL_LEFT(-PARAM::Field::PITCH_LENGTH/2.0, -PARAM::Field::PITCH_WIDTH/2);
const CGeoPoint GOAL_RIGHT(-PARAM::Field::PITCH_LENGTH/2.0, PARAM::Field::PITCH_WIDTH/2);
const double MIN_DIST_TO_PENALTY  = PARAM::Vehicle::V2::PLAYER_SIZE * 2;
const double MIN_DIST_TO_TEAMMATE = PARAM::Vehicle::V2::PLAYER_SIZE * 2 + PARAM::Field::BALL_SIZE * 2;
//后卫防守区间
CGeoPoint PENALTY_RIGHT_UP(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH + MIN_DIST_TO_PENALTY, -(PARAM::Field::PENALTY_AREA_WIDTH/2 + MIN_DIST_TO_PENALTY));
CGeoPoint PENALTY_RIGHT_DOWN(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH + MIN_DIST_TO_PENALTY, PARAM::Field::PENALTY_AREA_WIDTH/2 + MIN_DIST_TO_PENALTY);
CGeoPoint PENALTY_LEFT_UP(-PARAM::Field::PITCH_LENGTH/2,-(PARAM::Field::PENALTY_AREA_WIDTH/2 + MIN_DIST_TO_PENALTY));
CGeoPoint PENALTY_LEFT_DOWN(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PENALTY_AREA_WIDTH/2 + MIN_DIST_TO_PENALTY);
CGeoRectangle guardMoveRec(PENALTY_LEFT_UP,PENALTY_RIGHT_DOWN);
//防守目标关键点,如果超了就只防该点
CGeoPoint LEFTBACK_CRITICAL_POINT(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH + PARAM::Vehicle::V2::PLAYER_SIZE , -PARAM::Field::PITCH_WIDTH/2);
CGeoPoint RIGHTBACK_CRITICAL_POINT(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH + PARAM::Vehicle::V2::PLAYER_SIZE , PARAM::Field::PITCH_WIDTH/2);
}

CGuardPos::CGuardPos(){
    for(int i = 0; i < PARAM::Field::MAX_PLAYER; ++i){
        _backPos[i] = CGeoPoint(-9999,-9999);
        _multibackPos[i] = CGeoPoint(-9999,-9999);
    }
}

void CGuardPos::generatePos(int guardNum){
   guardNum = std::min(PARAM::Field::MAX_PLAYER,std::max(1,guardNum));
   const MobileVisionT& ball = vision->ball();
   //防守目标,球或控球敌方
   int bestEnemy = ZSkillUtils::instance()->getTheirBestPlayer();
   CGeoPoint defendTarget = ball.Valid() ? ball.RawPos() : MessiDecision::Instance()->getBallPos();

   if((defendTarget - GOAL_MIDDLE).dir() > (RIGHTBACK_CRITICAL_POINT - GOAL_MIDDLE).dir())
       defendTarget = RIGHTBACK_CRITICAL_POINT;
   else if((defendTarget - GOAL_MIDDLE).dir() < (LEFTBACK_CRITICAL_POINT - GOAL_MIDDLE).dir())
       defendTarget = LEFTBACK_CRITICAL_POINT;

   CGeoLine defendLine(defendTarget,GOAL_MIDDLE);

   //计算和长方形的交点
   CGeoLineRectangleIntersection intersecMiddle(defendLine,guardMoveRec);

   if(intersecMiddle.intersectant()){
     //单数后卫
       bool leftValid=true, rightValid=true;
       if(guardNum%2 == 1){
           _backPos[guardNum/2] = intersecMiddle.point2().dist(GOAL_MIDDLE) < 1e-8 ? intersecMiddle.point1() : intersecMiddle.point2();
           for(int i = 0; i< guardNum/2;++i){
                leftValid = leftNextPos(_backPos[guardNum/2-i],_backPos[guardNum/2-i-1]);
                rightValid= rightNextPos(_backPos[guardNum/2 + i],_backPos[guardNum/2 + i +1]);
           }
       } else{
           CGeoPoint intersecPos = intersecMiddle.point2().dist(GOAL_MIDDLE) < 1e-8 ? intersecMiddle.point1() : intersecMiddle.point2();
           leftValid = leftNextPos(intersecPos,_backPos[guardNum/2-1],MIN_DIST_TO_TEAMMATE/2);
           rightValid= rightNextPos(intersecPos,_backPos[guardNum/2],MIN_DIST_TO_TEAMMATE/2);
           for(int i =0; i< (guardNum-2)/2 ;i++){
               leftValid = leftNextPos(_backPos[guardNum/2-1-i],_backPos[guardNum/2-2-i]);
               rightValid= rightNextPos(_backPos[guardNum/2+i],_backPos[guardNum/2+1+i]);
           }
       }
       if(!leftValid){
           for(int i = 1;i<guardNum;i++){
               rightNextPos(_backPos[i-1],_backPos[i]);
           }
       }
       if(!rightValid){
           for(int i = 1; i<guardNum;i++){
               leftNextPos(_backPos[guardNum-i],_backPos[guardNum-i-1]);
           }
       }
   }
   else
       qDebug() << "GUARDPOS NOT INTERSECTION";

}

void CGuardPos::generatemultiPos(int defendNum,int index){
    const PlayerVisionT &enemy = vision->theirPlayer(defendNum);
    CGeoSegment our_goal_line(GOAL_LEFT,GOAL_RIGHT);
    CVector vec = Utils::Polar2Vector(99999,enemy.Dir());
    CGeoSegment enemy_line(enemy.Pos(),enemy.Pos() + vec);
    CGeoLine right_goal_line(enemy.Pos(),GOAL_RIGHT);
    CGeoLine left_goal_line(enemy.Pos(),GOAL_LEFT);
    CGeoRectangle stand_rectangle(PENALTY_LEFT_UP,PENALTY_RIGHT_DOWN);
    CGeoLineRectangleIntersection stand_intersection(enemy_line,stand_rectangle);
    CGeoLineRectangleIntersection stand_intersection1(right_goal_line,stand_rectangle);
    CGeoLineRectangleIntersection stand_intersection2(left_goal_line,stand_rectangle);

    CGeoPoint stand_pos,stand_pos1,stand_pos2;
    if(stand_intersection.intersectant()){
        if(enemy.Pos().dist2(stand_intersection.point1()) < enemy.Pos().dist2(stand_intersection.point2()))
            stand_pos = stand_intersection.point1();
        else
            stand_pos = stand_intersection.point2();
        if(stand_pos.x() < -PARAM::Field::PITCH_LENGTH/2 + PARAM::Vehicle::V2::PLAYER_SIZE)
            stand_pos.setX(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Vehicle::V2::PLAYER_SIZE);
    }else{
        stand_pos = CGeoPoint(-PARAM::Field::PITCH_LENGTH + PARAM::Field::PENALTY_AREA_DEPTH+PARAM::Vehicle::V2::PLAYER_SIZE,0);
    }

    if(stand_intersection1.intersectant()){
        if(enemy.Pos().dist2(stand_intersection1.point1()) < enemy.Pos().dist2(stand_intersection1.point2()))
            stand_pos1 = stand_intersection1.point1();
        else
            stand_pos1 = stand_intersection1.point2();
        if(stand_pos1.x() < -PARAM::Field::PITCH_LENGTH/2 + PARAM::Vehicle::V2::PLAYER_SIZE)
            stand_pos1.setX(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Vehicle::V2::PLAYER_SIZE);
    }else{
        stand_pos1 = PENALTY_RIGHT_DOWN + Utils::Polar2Vector(PARAM::Field::PENALTY_AREA_WIDTH/6,-PARAM::Math::PI/2);
    }

    if(stand_intersection2.intersectant()){
        if(enemy.Pos().dist2(stand_intersection2.point1()) < enemy.Pos().dist2(stand_intersection2.point2()))
            stand_pos2 = stand_intersection2.point1();
        else
            stand_pos2 = stand_intersection2.point2();
        if(stand_pos2.x() < -PARAM::Field::PITCH_LENGTH/2 + PARAM::Vehicle::V2::PLAYER_SIZE)
            stand_pos2.setX(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Vehicle::V2::PLAYER_SIZE);
    }else{
        stand_pos2 = PENALTY_RIGHT_UP + Utils::Polar2Vector(PARAM::Field::PENALTY_AREA_WIDTH/6,PARAM::Math::PI/2);
    }

    if(enemy.Valid()){
        if(our_goal_line.IsSegmentsIntersect(enemy_line)){
            _multibackPos[index] = stand_pos;
        } else {
            if(((GOAL_RIGHT - enemy.Pos()).dir() - enemy.Dir())>0){
                _multibackPos[index] = stand_pos1;
            } else if(((GOAL_LEFT - enemy.Pos()).dir() - enemy.Dir()) < 0) {
                _multibackPos[index] = stand_pos2;
            }
        }
    }
}

bool CGuardPos::usedoubleBack(int index){
    CGeoPoint inipos(-9999,-9999);
    for(int i = 0;i < PARAM::Field::MAX_PLAYER;i++){
        bool leftValid = true, rightValid = true;
        if(i==index) continue;
        if((_multibackPos[i] - inipos).mod() < 0.01*10 || i == index) continue;
        CGeoPoint basePos((_multibackPos[index].x() + _multibackPos[i].x())/2.0,(_multibackPos[index].y() + _multibackPos[i].y())/2.0);
        CGeoPoint leftpos,rightpos,speciabasePos;
        CGeoRectangle rec(PENALTY_LEFT_UP,PENALTY_RIGHT_DOWN);
        CGeoLine specialPos(basePos,(_multibackPos[i] - _multibackPos[index]).dir() + PARAM::Math::PI/2);
        CGeoLineRectangleIntersection stand_intersection(specialPos,rec);

        if(stand_intersection.intersectant()){
            if(basePos.dist2(stand_intersection.point1()) < basePos.dist2(stand_intersection.point2()))
                speciabasePos = stand_intersection.point1();
            else
                speciabasePos = stand_intersection.point2();
            if(speciabasePos.x() < -PARAM::Field::PITCH_LENGTH/2 + PARAM::Vehicle::V2::PLAYER_SIZE)
                speciabasePos.setX(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Vehicle::V2::PLAYER_SIZE);
        }else{
            speciabasePos = CGeoPoint(-PARAM::Field::PITCH_LENGTH + PARAM::Field::PENALTY_AREA_DEPTH+PARAM::Vehicle::V2::PLAYER_SIZE,0);
        }

        if(fabs(_multibackPos[i].x() - _multibackPos[index].x()) < 0.01*10 || fabs(_multibackPos[i].y() - _multibackPos[index].y()) < 0.01*10){
            if((_multibackPos[i] - _multibackPos[index]).mod() < PARAM::Vehicle::V2::PLAYER_SIZE * 2.5){
                leftValid = leftNextPos(basePos,leftpos,MIN_DIST_TO_TEAMMATE/2);
                rightValid= rightNextPos(basePos,rightpos,MIN_DIST_TO_TEAMMATE/2);
                if((leftpos - _multibackPos[index]).mod() < (rightpos - _multibackPos[index]).mod()){
                    _revisebackPos[index] = leftpos;
                } else {
                    _revisebackPos[index] = rightpos;
                }
                return true;
            }
        } else {
            if((_multibackPos[i] - PENALTY_RIGHT_DOWN).mod() + (_multibackPos[index] - PENALTY_RIGHT_DOWN).mod() < PARAM::Vehicle::V2::PLAYER_SIZE * 2.5 || (_multibackPos[i] - PENALTY_RIGHT_UP).mod() + (_multibackPos[index] - PENALTY_RIGHT_UP).mod() < PARAM::Vehicle::V2::PLAYER_SIZE * 2.5 ){
                leftValid = leftNextPos(speciabasePos,leftpos,MIN_DIST_TO_TEAMMATE/2);
                rightValid= rightNextPos(speciabasePos,rightpos,MIN_DIST_TO_TEAMMATE/2);
                if((leftpos - _multibackPos[index]).mod() < (rightpos - _multibackPos[index]).mod()){
                    _revisebackPos[index] = leftpos;
                } else {
                    _revisebackPos[index] = rightpos;
                }
                return  true;
            }
        }
    }
    return false;
}

CGeoPoint CGuardPos::backPos(int guardNum, int index, int realNum, int defendNum){
    //后卫数量限制
    guardNum = std::min(PARAM::Field::MAX_PLAYER,std::max(1,guardNum));
    _guardNum = guardNum;
    index = std::min(guardNum,std::max(1,index));
    generatePos(guardNum);
    return _backPos[index-1];
}

CGeoPoint CGuardPos::multibackPos(int defendNum){
    if(usedoubleBack(defendNum)){
        return _revisebackPos[defendNum];
    } else{
        return _multibackPos[defendNum];
    }
}

bool CGuardPos::leftNextPos(CGeoPoint basePos, CGeoPoint& nextPos, double dist)
{
    if (dist < 0) dist = MIN_DIST_TO_TEAMMATE;
    //中间那条线
    if (basePos.y() >= PENALTY_RIGHT_UP.y() + dist && fabs(basePos.x() - PENALTY_RIGHT_DOWN.x()) < 1e-8){
        nextPos = basePos + CVector(0, -dist);
    }
    else if (basePos.y() < PENALTY_RIGHT_UP.y() + dist) {
        if (fabs(basePos.x() - PENALTY_RIGHT_UP.x()) < 1e-8) {
            nextPos = PENALTY_RIGHT_UP + CVector(-sqrt(pow(dist, 2) - pow(basePos.y() - PENALTY_RIGHT_UP.y(), 2)), 0);
        }
        else if (fabs(basePos.x() - PENALTY_LEFT_UP.x()) >= dist) {
            nextPos = basePos + CVector(-dist, 0);
        }
        else {
            nextPos = PENALTY_LEFT_UP;
            return false;
        }
    }
    else {
        if (fabs(basePos.x() - PENALTY_RIGHT_DOWN.x()) <= dist) {
            nextPos = PENALTY_RIGHT_DOWN + CVector(0, -sqrt(pow(dist, 2) - pow(basePos.x() - PENALTY_RIGHT_DOWN.x(), 2)));
        }
        else {
            nextPos = basePos + CVector(dist, 0);
        }
    }
    return true;
}

bool CGuardPos::rightNextPos(CGeoPoint basePos, CGeoPoint &nextPos, double dist)
{
    if (dist < 0) dist = MIN_DIST_TO_TEAMMATE;
    if (basePos.y() <= PENALTY_RIGHT_DOWN.y() - dist && fabs(basePos.x() - PENALTY_RIGHT_UP.x()) < 1e-8){
        nextPos = basePos + CVector(0, dist);
    }
    else if (basePos.y() > PENALTY_RIGHT_DOWN.y() - dist) {
        if (fabs(basePos.x() - PENALTY_RIGHT_DOWN.x()) < 1e-8) {
            nextPos = PENALTY_RIGHT_DOWN + CVector(-sqrt(pow(dist, 2) - pow(basePos.y() - PENALTY_RIGHT_DOWN.y(), 2)), 0);
        }
//        else if(basePos.x() - dist < -PARAM::Field::PITCH_LENGTH/2 + PARAM::Vehicle::V2::PLAYER_SIZE){
//            nextPos =
//        }
        else if (fabs(basePos.x() - PENALTY_LEFT_DOWN.x()) >= dist) {
            nextPos = basePos + CVector(-dist, 0);
        }
        else {
            nextPos = PENALTY_LEFT_DOWN;
            return false;
        }
    }
    else {
        if (fabs(basePos.x() - PENALTY_RIGHT_UP.x()) <= dist) {
            nextPos = PENALTY_RIGHT_UP + CVector(0, sqrt(pow(dist, 2) - pow(basePos.x() - PENALTY_RIGHT_UP.x(), 2)));
        }
        else {
            nextPos = basePos + CVector(dist, 0);
        }
    }
    return true;
}
