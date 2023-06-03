#include "wdrag.h"
#include <skill/Factory.h>
#include "staticparams.h"
#include "WorldModel.h"
#include "TaskMediator.h"
#include "parammanager.h"

namespace  {
enum {
    GOTO = 1,
    MARKED,
    ANTIDRAG,
    REALDRAG,
    ESCAPE,
    BLOCK,
    FREE,
};
bool DEBUG = true;

const double MIN_SPEED_FOR_ENEMY  = 1500;

const double CHECK_MARK_TIME        = PARAM::Vision::FRAME_RATE;
const double CHECK_FREE_DIST        = 300;
const double CHECK_FREE_REDRAG_TIME = 20;
const double MAX_WAIT_TIME          = PARAM::Vision::FRAME_RATE * 5;

//////////////////////////////////////////
// [0] 敌方禁区上横线
// [1] 敌方禁区上竖直线
// [2] 敌方禁区下竖直线
// [3] 我方禁区上横线
// [4] 我方禁区上竖直线
// [5] 我方禁区下竖直线
const CGeoSegment PENALTY_BORDER[6] = {CGeoSegment(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH, PARAM::Field::PENALTY_AREA_WIDTH / 2), CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH, - PARAM::Field::PENALTY_AREA_WIDTH / 2)),
                                       CGeoSegment(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH, PARAM::Field::PENALTY_AREA_WIDTH / 2), CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::PENALTY_AREA_WIDTH / 2)),
                                       CGeoSegment(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH, - PARAM::Field::PENALTY_AREA_WIDTH / 2), CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, - PARAM::Field::PENALTY_AREA_WIDTH / 2)),
                                       CGeoSegment(CGeoPoint(-(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH), PARAM::Field::PENALTY_AREA_WIDTH / 2), CGeoPoint(-(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH), - PARAM::Field::PENALTY_AREA_WIDTH / 2)),
                                       CGeoSegment(CGeoPoint(-(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH), PARAM::Field::PENALTY_AREA_WIDTH / 2), CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::PENALTY_AREA_WIDTH / 2)),
                                       CGeoSegment(CGeoPoint(-(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH), - PARAM::Field::PENALTY_AREA_WIDTH / 2), CGeoPoint(-PARAM::Field::PITCH_LENGTH / 2, - PARAM::Field::PENALTY_AREA_WIDTH / 2))};
}

CWDrag::CWDrag() : _lastCycle(0), _lastState(GOTO), _lastConfirmCycle(0), _state(GOTO)
{
      ZSS::ZParamManager::instance()->loadParam(DEBUG, "Debug/WDrag", true);
}

void CWDrag::plan(const CVisionModule* pVision) {
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        setState(BEGINNING);
        enemyNum = -999;
        markDist = 999999;
        v1.setVector(0,0);
        v2 = v1;
        finalVelVec = v2;
        antiTarget.fill(0,0);
        realTarget.fill(0,0);
        freeBallDist = 3000;
        freeCount = 0;
        antiCnt = 0;
        antiVelCount = 0;
        realCnt = 0;
        escapeCnt = 0;

    }

    int vecNumber = task().executor;
    CGeoPoint target = task().player.pos;
    CGeoPoint purposeTarget   = CGeoPoint(task().player.kickpower,task().player.chipkickpower);
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);
    const double self2BallDir = Utils::Normalize((pVision->ball().RawPos() - self.RawPos()).dir());
    const CGeoPoint interPoint   = ZSkillUtils::instance()->predictedTheirInterPoint(self.RawPos(),pVision->ball().RawPos());
    const double self2InterPoint = self.RawPos().dist(interPoint);

    //需要拉扯的点赋值
    //如果给了那优先给的点  如果没给则离球200mm的点
    if(Utils::IsInField(purposeTarget) && purposeTarget.dist(target) > 1000){
        purposeMode = true;
    } else {
        purposeMode = false;
        purposeTarget = pVision->ball().RawPos() + CVector(-200,0).rotate(self2BallDir);//如果加了避球的flag后，点不能直接发在球上
    }


    if(_state == GOTO || _state == FREE){
        //如果到点了
        //now_state = "Normal GOTO";
        if(target.dist(self.RawPos()) < PARAM::Vehicle::V2::PLAYER_SIZE * 2) {
            int enemyMarkCar = -999;
            std::vector<int> enemyNumVec;
            //获取有几台敌方车会比我方车先到截球点
            int num = getEnemyAmountInArea(interPoint, self2InterPoint, enemyNumVec, 200,pVision);
            GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0, 2000), QString("num: %1").arg(QString::number(num)).toLatin1(), COLOR_PURPLE);

            //获取敌方最优防守车
            if(num > 0){
                for(int i = 0; i < num; i++)
                {
                    auto &enemy = pVision->theirPlayer(enemyNumVec[i]);
                    if(enemy.Valid()) {
                        double tmpDist = enemy.Pos().dist(self.RawPos());
                        if(tmpDist < markDist){
                            markDist = tmpDist;
                            enemyMarkCar = enemyNumVec[i];
                            //markDist -= 200;
                        }
                    }
                }
            }
            enemyNum = enemyMarkCar;
            //如果有敌方车在防守则进入跑位状态
            if(enemyNum > 0){
                _state = MARKED;

            }else{
                _state = FREE;
                enemyNum = -999;
                freeBallDist = 3000;
            }
        } else {
            //继续跑目标点
            _state = GOTO;
        }
    }

    CGeoPoint ball = pVision->ball().RawPos();
    //如果传了waitpos就换点
    if(purposeMode){
        ball = purposeTarget;
    }
    double judgeNum = -1;
    double self2BallAngle = Utils::Normalize((ball - self.RawPos()).dir());
    double self2BallDist  = self.RawPos().dist(ball);
    double enemy2BallDist = 99999;
    double enemy2selfDist = 99999;
    double enemy2InterPointDist = 99999;
    if(enemyNum > 0){
        judgeNum = ((ball.x() - self.X()) * (pVision->theirPlayer(enemyNum).Y() - self.Y())
                    -(pVision->theirPlayer(enemyNum).X() - self.X()) * (ball.y() - self.Y())) * (purposeMode ? -1 : 1);//判断公式，正值为顺时针，负为逆时针 // purposeMode下需要反拉
        enemy2BallDist = pVision->theirPlayer(enemyNum).Pos().dist(ball);
        enemy2selfDist = pVision->theirPlayer(enemyNum).Pos().dist(self.RawPos());
        enemy2InterPointDist = pVision->theirPlayer(enemyNum).Pos().dist(interPoint);
    }

    if(_state == MARKED){
        //now_state = "MARKED";
        if (self.Vel().mod() < 100){
            v1.setVector(3 * (enemy2selfDist > 500 ? enemy2selfDist : 500), 0);
            v1 = v1.rotate(self2BallAngle + (judgeNum > 0 ? 1 : -1) * 90 * PARAM::Math::PI / 180);
            antiTarget = self.RawPos() + v1;
            if (purposeMode && pVision->theirPlayer(enemyNum).Pos().dist(purposeTarget) - 200 > self.RawPos().dist(purposeTarget)){
                _state = ESCAPE;
            } else {
                //如果直线碰到禁区线则退出
                for(int i = 0; i < 6; i++){
                    if(CGeoSegment(self.RawPos(),antiTarget).IsSegmentsIntersect(PENALTY_BORDER[i])) {
                        _state = BLOCK;
                        break;
                    }
                    _state = ANTIDRAG;
                }
            }
        }
    }
    if(_state == ANTIDRAG){
        //now_state = "ANTIDRAG";
        antiCnt = pVision->theirPlayer(enemyNum).Vel().mod() < 300 ? antiCnt + 1 : 0;
        if(antiCnt > CHECK_MARK_TIME){
            antiCnt = 0;
            _state = ESCAPE;
        }
        if(pVision->theirPlayer(enemyNum).Vel().mod() > MIN_SPEED_FOR_ENEMY) {
            _state = REALDRAG;
            v2.setVector(3 * (enemy2selfDist > 1000 ? enemy2selfDist : 1000),0);
            v2 = v2.rotate(v1.dir() + PARAM::Math::PI);
            realTarget = self.RawPos() + v2;
        }
        antiVelCount = self.RawPos().dist(antiTarget) < 500 ? antiVelCount + 1 : 0;
        if(antiVelCount > CHECK_FREE_REDRAG_TIME){
            _state = MARKED;
            antiVelCount = 0;
        }
    }
    if(_state == REALDRAG){
        //now_state = "REALDRAG";
        if(purposeMode){
            CGeoLine purposeLine(self.RawPos(), purposeTarget);
            CGeoSegment purposeSegment(self.RawPos(),purposeTarget);
            CGeoPoint purposeProjection = purposeLine.projection(pVision->theirPlayer(enemyNum).Pos());

            if(purposeSegment.IsPointOnLineOnSegment(purposeProjection) && self.RawPos().dist(purposeProjection) < pVision->theirPlayer(enemyNum).Pos().dist(purposeProjection) && pVision->theirPlayer(enemyNum).Pos().dist(purposeProjection) > 70){
                _state = ESCAPE;
            }
        }
        else if(enemy2InterPointDist > (self2InterPoint > 500 ? self2InterPoint : (self2InterPoint + CHECK_FREE_DIST))) {
            _state = BLOCK;
        }
        realCnt = self.RawPos().dist(realTarget) < 1000 ? realCnt + 1 : 0;
        if(realCnt > CHECK_FREE_REDRAG_TIME) {
            _state = MARKED;
            realCnt = 0;
        }
    }
    if(_state == ESCAPE) {
        //now_state = "ESCAPE";
        if(purposeMode) {
            escapeCnt = (self.RawPos().dist(purposeTarget) < PARAM::Vehicle::V2::PLAYER_SIZE * 2) ? escapeCnt + 1 : 0;
            if(escapeCnt > MAX_WAIT_TIME) {
                _state = GOTO;
                markDist = 99999;
                escapeCnt = 0;
            }
        } else {
            //阶段性位移，位移一会就回去
            if(!confirmFreeBallDist) {
                freeBallDist = self2BallDist * 0.6;
                confirmFreeBallDist = true;
            }
            if (self2BallDist < (freeBallDist < 2000 ? 2000 : freeBallDist)) {
                _state = GOTO;
                markDist = 99999;
                confirmFreeBallDist = false;
            }
            if (enemy2BallDist - self2BallDist > 400 && !(enemy2BallDist > 88888)) {
                _state = FREE;
                confirmFreeBallDist = false;
            }
        }
    }
    if(_state == BLOCK){
        //now_state = "BLOCK";
        if(self2BallDist < 2000){
            _state = GOTO;
            markDist = 99999;
        }
        if(pVision->theirPlayer(enemyNum).Vel().mod() < 50) {
            _state = FREE;
        }
    }
    if(_state == FREE){
        //now_state = "FREE";
        if(enemy2BallDist - self2BallDist < 200 || self2BallDist < (freeBallDist < 2000 ? 2000 : freeBallDist)) {
            _state = GOTO;
            markDist = 99999;
        }
    }

    //强制换位置 escape的位置和goto对换 每五秒换一次
    freeCount = self.RawPos().dist(task().player.pos) < PARAM::Vehicle::V2::PLAYER_SIZE * 2 ? ++freeCount : 0;
    if(freeCount > MAX_WAIT_TIME) {
        _state = ESCAPE;
        freeCount = 0;
    }

    //防止车出场
    if(!Utils::IsInField(antiTarget,PARAM::Vehicle::V2::PLAYER_SIZE * 2))
        antiTarget = Utils::MakeInField(antiTarget,PARAM::Vehicle::V2::PLAYER_SIZE * 2);
    if(!Utils::IsInField(realTarget,PARAM::Vehicle::V2::PLAYER_SIZE * 2))
        realTarget = Utils::MakeInField(realTarget,PARAM::Vehicle::V2::PLAYER_SIZE * 2);
    //防止车进入禁区
    if (Utils::InTheirPenaltyArea(antiTarget, PARAM::Vehicle::V2::PLAYER_SIZE * 2))
        antiTarget = Utils::MakeOutOfTheirPenaltyArea(antiTarget,PARAM::Vehicle::V2::PLAYER_SIZE * 2);
    if (Utils::InOurPenaltyArea(antiTarget, PARAM::Vehicle::V2::PLAYER_SIZE * 2))
        antiTarget = Utils::MakeOutOfOurPenaltyArea(antiTarget,PARAM::Vehicle::V2::PLAYER_SIZE * 2);
    if (Utils::InTheirPenaltyArea(realTarget, PARAM::Vehicle::V2::PLAYER_SIZE * 2))
        realTarget = Utils::MakeOutOfTheirPenaltyArea(realTarget,PARAM::Vehicle::V2::PLAYER_SIZE * 2);
    if (Utils::InOurPenaltyArea(realTarget, PARAM::Vehicle::V2::PLAYER_SIZE * 2))
        realTarget = Utils::MakeOutOfOurPenaltyArea(realTarget,PARAM::Vehicle::V2::PLAYER_SIZE * 2);

    if((_state == MARKED || _state == ANTIDRAG || _state == REALDRAG) && DEBUG) {
        GDebugEngine::Instance()->gui_debug_msg(pVision->theirPlayer(enemyNum).Pos(), QString("W").toLatin1(), COLOR_PURPLE);
        GDebugEngine::Instance()->gui_debug_line(pVision->theirPlayer(enemyNum).Pos(), self.RawPos(), COLOR_PURPLE);
    }
    if (DEBUG) {
        GDebugEngine::Instance()->gui_debug_x(target, COLOR_PURPLE);
        GDebugEngine::Instance()->gui_debug_arc(interPoint, self2InterPoint + 20, 0.0, 360.0);
        //GDebugEngine::Instance()->gui_debug_msg(self.RawPos() + CVector(0, 200), QString("STATE: %1").arg(QString::number(_state)).toLatin1(), COLOR_PURPLE);
        //GDebugEngine::Instance()->gui_debug_msg(self.RawPos() + CVector(0, 200), QString(now_state).toLatin1(), COLOR_PURPLE);
        GDebugEngine::Instance()->gui_debug_x(interPoint, COLOR_ORANGE);
        GDebugEngine::Instance()->gui_debug_x(purposeTarget, COLOR_WHITE);
    }
//    std::cout << "ZDRAG STATE NOW IS " << _state << " " << pVision->Cycle()<< std::endl;

    TaskT dragTask(task());
    dragTask.player.flag |= PlayerStatus::ALLOW_DSS | PlayerStatus::DODGE_BALL;

    switch (_state) {
    case GOTO:
        GDebugEngine::Instance()->gui_debug_msg(self.RawPos() + CVector(0, 200), QString("GOTO").toLatin1(), COLOR_PURPLE);
        dragTask.player.pos = target;
        dragTask.player.angle = self2BallDir;
        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(dragTask));
        break;

    case MARKED:
        GDebugEngine::Instance()->gui_debug_msg(self.RawPos() + CVector(0, 200), QString("MARKED").toLatin1(), COLOR_PURPLE);
        setSubTask(TaskFactoryV2::Instance()->StopRobot(dragTask));
        break;

    case ANTIDRAG:
        GDebugEngine::Instance()->gui_debug_msg(self.RawPos() + CVector(0, 200), QString("ANTIDRAG").toLatin1(), COLOR_PURPLE);
        GDebugEngine::Instance()->gui_debug_msg(antiTarget, QString("ANTI").toLatin1(), COLOR_PURPLE);
        dragTask.player.pos = antiTarget;
        dragTask.player.angle = self2BallDir;
        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(dragTask));
        break;

    case REALDRAG:
        GDebugEngine::Instance()->gui_debug_msg(self.RawPos() + CVector(0, 200), QString("REALDRAG").toLatin1(), COLOR_PURPLE);
        dragTask.player.pos = realTarget;
        dragTask.player.angle = self2BallDir;
//        dragTask.player.vel = finalVelVec;
        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(dragTask));
        break;

    case ESCAPE:
        GDebugEngine::Instance()->gui_debug_msg(self.RawPos() + CVector(0, 200), QString("ESCAPE").toLatin1(), COLOR_PURPLE);
        dragTask.player.pos = purposeTarget;
        dragTask.player.angle = self2BallDir;
        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(dragTask));
        break;

    case BLOCK:
        GDebugEngine::Instance()->gui_debug_msg(self.RawPos() + CVector(0, 200), QString("BLOCK").toLatin1(), COLOR_PURPLE);
        dragTask.player.angle = self2BallDir;
        dragTask.player.pos = ZSkillUtils::instance()->getMarkingPoint(pVision->theirPlayer(enemyNum).Pos(), pVision->theirPlayer(enemyNum).Vel(), 4500, 4500, 450, 3000, pVision->ball().RawPos());
        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(dragTask));
        break;

    case FREE:
        GDebugEngine::Instance()->gui_debug_msg(self.RawPos() + CVector(0, 200), QString("FREE").toLatin1(), COLOR_PURPLE);
        markDist = 99999;
        dragTask.player.angle = self2BallDir;
//        dragTask.player.max_acceleration = MAX_ACC - 200;
        setSubTask(TaskFactoryV2::Instance()->StopRobot(dragTask));
        break;

    default:
        GDebugEngine::Instance()->gui_debug_msg(self.RawPos() + CVector(0, 200), QString("NULL").toLatin1(), COLOR_PURPLE);
        dragTask.player.pos = target;
        dragTask.player.angle = self2BallDir;
        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(dragTask));
        break;
    }

    _lastCycle = pVision->getCycle();
    _lastState = _state;
    return CStatedTask::plan(pVision);
}

CPlayerCommand* CWDrag::execute(const CVisionModule *pVision) {
    if (subTask()) {
        return subTask()->execute(pVision);
    }
    return nullptr;
}

//can move to WorldModel
int CWDrag::getEnemyAmountInArea(const CGeoPoint& center ,double radius, std::vector<int>& enemyNumVec, double buffer,const CVisionModule* _pVision) {
    enemyNumVec.clear();
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        if (_pVision->theirPlayer(i).Valid() && _pVision->theirPlayer(i).Pos().dist(center) <= radius + buffer) {
            enemyNumVec.push_back(i);
        }
    }
    return enemyNumVec.size();
}

