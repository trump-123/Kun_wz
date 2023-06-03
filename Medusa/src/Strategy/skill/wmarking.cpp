#include "wmarking.h"
#include "skill/Factory.h"
#include "VisionModule.h"
#include "utils.h"
#include "geometry.h"
#include "defence/DefenceInFo.h"
#include "SkillUtils.h"
#include "TaskMediator.h"

CWmarking::CWmarking(){
    _lastCycle = 0;
}

void CWmarking::plan(const CVisionModule *pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE * 0.1){
        setState(BEGINNING);
    }

    const MobileVisionT&ball = pVision->ball();
    int vecNumber = task().executor;
    int enemyNum = task().ball.Sender;
    int pri = task().ball.receiver;
    int flag = task().player.flag;
    bool stop_mode = flag & PlayerStatus::AVOID_STOP_BALL_CIRCLE;
    bool half_field_mode = flag & PlayerStatus::AVOID_HALF_FIELD;

    const PlayerVisionT& me = pVision->ourPlayer(vecNumber);
    const PlayerVisionT& enemy = pVision->theirPlayer(enemyNum);
    CGeoPoint enemyPos = enemy.Pos();
    CGeoPoint mePos = me.Pos();
    double me2balldir = CVector(ball.Pos() - mePos).dir();
    TaskT MarkingTask(task());
    if(stop_mode) MarkingTask.player.flag |= PlayerStatus::AVOID_STOP_BALL_CIRCLE;

    MarkingTask.player.angle = me2balldir;
    MarkingTask.player.pos = ZSkillUtils::instance()->getZMarkingPos(pVision,vecNumber,enemyNum,pri,flag);
    if(half_field_mode) {
        MarkingTask.player.specify_path_plan_area = true;
    }
    if(enemy.Valid()){
        DefenceInfo::Instance()->setMarkList(pVision,vecNumber,enemyNum);
        DefenceInfo::Instance()->setMarkMode(vecNumber,enemyNum,false);
    }
    MarkingTask.player.flag = (flag | PlayerStatus::ALLOW_DSS);

    CVector enemy2ball = ball.RawPos() - enemy.Pos();
    CVector me2ball    = ball.RawPos() - me.RawPos();

    //敌方车的移速在敌方车到球线的投影点
    double theta_1 = Utils::Normalize(enemy.Vel().dir() - enemy2ball.dir());
    double enemyVn = enemy.Vel().mod()*sin(theta_1);
    double enemyVt = enemy.Vel().mod()*cos(theta_1);

    double meVn = enemyVn * me2ball.mod() / enemy2ball.mod();
    double meVt = enemyVt * me2ball.mod() / enemy2ball.mod();

    double theta_2 = atan2(meVn,meVt);

    double meVelDir = Utils::Normalize(theta_2 + me2ball.dir());
    double meVelMod = 0;

    if(enemy.Vel().mod() < 50*10) meVelMod = 0;
    if(Utils::InOurPenaltyArea(MarkingTask.player.pos + Utils::Polar2Vector(meVelMod/3,meVelDir),PARAM::Vehicle::V2::PLAYER_SIZE)
            ||Utils::InTheirPenaltyArea(MarkingTask.player.pos + Utils::Polar2Vector(meVelMod/3,meVelDir),PARAM::Vehicle::V2::PLAYER_SIZE)){
        meVelMod = 0;
    }
    MarkingTask.player.vel = Utils::Polar2Vector(meVelMod, meVelDir);//0

    setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(MarkingTask));

    _lastCycle = pVision->getCycle();
    CStatedTask::plan(pVision);
}

CPlayerCommand* CWmarking::execute(const CVisionModule *pVision){
    if(subTask() ) return subTask() -> execute(pVision);
    return nullptr;
}
