#include "messidecition.h"
#include "SkillUtils.h"
#include "RobotSensor.h"
#include "passposevaluate.h"
#include "GetBestUtils.h"
#include "TaskMediator.h"
#include "ballmodel.h"
#include "ShootModule.h"
#include "runpos.h"
#include "TaskMediator.h"


namespace  {
double FRICTION;

double minRadaius = PARAM::Vehicle::V2::PLAYER_SIZE * 2.0;

const CGeoPoint STATIC_POS[2] = {CGeoPoint(PARAM::Field::PITCH_LENGTH / 4, PARAM::Field::PITCH_WIDTH / 4),
                                 CGeoPoint(PARAM::Field::PITCH_LENGTH / 4, -PARAM::Field::PITCH_WIDTH / 4)};
//free_kick更新周期
int freetime = 0;

//存放freeKickpos
CGeoPoint freeposLast(0,0);

//挑球期间diff
int flydiff = 0;

//获取位置的diff
int diff = 0;

//避免射门后马上改变Leader
const int MIN_CHANGE_LEADER_INTERVEL = 10;

//改变接球车的最小间隔
const int MIN_CHANGE_RECEIVR_INTERVEL = 60;

//判断leader拿着球的距离
const int HOLDBALL_DIST = 200;

// 传球失败时的截球时间差
const int WRONG_LEADER_INTERTIME = 2;

bool ONLY_PASSPOS = false;
bool ONLY_SHOOTPOS = false;
bool ONLY_FLATPOS = false;
bool ONLY_CHIPPOS = false;
bool SIGNAL_TO_CUDA = false;
bool INTER_DEBUG = false;
bool USE_11vs11SOLVER = false;

namespace RECALCULATE {
//对方的反应时间
double THEIR_RESPONSE_TIME = 0.5;
double THEIR_CHIP_RESPONSE_TIME = 0.3;
//传球角度误差，角度值
double PASS_ANGLE_ERROR = 2;
//重算的条件
QString recomputeCondition = "";
//是否忽略近处的车
double IGNORE_CLOSE_ENEMY_DIST = 1000;
bool IGNORE_THEIR_GUARD = true;
//无效传球点保持的最大帧数
const int MAX_INVALID_CNT = 30;
}

namespace PASS {
bool passMode = true;
bool selfPass = false;
double maxPassScore = -9999;
double maxShootScore = -9999;
double flatPassQ = -9999;
double chipPassQ = -9999;
int CHOOSE_FLAT_DIFF = 25;
int CHOOSE_SHOOT_THRESHOLD = 60;
int NO_KICK_CNT = 0;
int MAX_NO_KICK = 10;
double bufferTime = 0.4;
}

}


CMessiDecision::CMessiDecision()
    : _leader(1)
    , _receiver(2)
    , _lastLeader(3)
    , _cycle(0)
    , _diff(0)
    , _stateChangeCycle(0)
    , _lastRecomputeCycle(0)
    , _lastChangeReceiverCycle(0)
    , _lastChangeLeaderCycle(-99999)
    , _lastUpdateRunPosCycle(-99999)
    , inValidPassCnt(0)
    , inValidShootCnt(0)
    , _passVel(0)
    ,_freepassVel(0)
    , _firstChipDir(0)
    , _isFlat(true)
    , _isfreeFlat(true)
    , _isbackFlat(true)
    , _isFlyTime(false)
    , _flydiff(false)
    , _canKick(false)
    , _canKickReceiver(false)
    , _isChange(false)
    , _isPassC(false)
    , _state("GetBall")
    , _laststate("GetBall")
    , _leaderState("PASS")
    , _passPos(CGeoPoint(PARAM::Field::PITCH_LENGTH/2, 0))
    , _receiverPos(STATIC_POS[0])
    , _flatPassPos(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH - 500 , 0))
    , _firstChipPos(CGeoPoint(0,0))
    , _pVision(nullptr){

    bool isSimulation;
    ZSS::ZParamManager::instance()->loadParam(isSimulation, "Alert/IsSimulation", false);
    if(isSimulation)
        ZSS::ZParamManager::instance()->loadParam(FRICTION, "AlertParam/Friction4Sim", 1520.0);
    else
        ZSS::ZParamManager::instance()->loadParam(FRICTION, "AlertParam/Friction4Real", 800.0);
    ZSS::ZParamManager::instance()->loadParam(USE_11vs11SOLVER, "Messi/USE_11vs11SOLVER", false);
    ZSS::ZParamManager::instance()->loadParam(ONLY_PASSPOS, "Messi/ONLY_PASSPOS", false);
    ZSS::ZParamManager::instance()->loadParam(ONLY_SHOOTPOS, "Messi/ONLY_SHOOTPOS", false);
    ZSS::ZParamManager::instance()->loadParam(ONLY_FLATPOS, "Messi/ONLY_FLATPOS", false);
    ZSS::ZParamManager::instance()->loadParam(ONLY_CHIPPOS, "Messi/ONLY_CHIPPOS", false);
    ZSS::ZParamManager::instance()->loadParam(SIGNAL_TO_CUDA, "Messi/SIGNAL_TO_CUDA", false);
    ZSS::ZParamManager::instance()->loadParam(INTER_DEBUG, "Messi/INTER_DEBUG", false);

    _otherPos[0] = CGeoPoint(PARAM::Field::PITCH_LENGTH / 8,  PARAM::Field::PITCH_WIDTH / 4);
    _otherPos[1] = CGeoPoint(PARAM::Field::PITCH_LENGTH / 8, -PARAM::Field::PITCH_WIDTH / 4);
    _otherPos[2] = CGeoPoint(PARAM::Field::PITCH_LENGTH * 3 /8,  PARAM::Field::PITCH_WIDTH / 4);
    _otherPos[3] = CGeoPoint(PARAM::Field::PITCH_LENGTH * 3 /8, -PARAM::Field::PITCH_WIDTH / 4);
    _otherPos[4] = CGeoPoint(-PARAM::Field::PITCH_LENGTH / 8,  PARAM::Field::PITCH_WIDTH / 4);
    _otherPos[5] = CGeoPoint(-PARAM::Field::PITCH_LENGTH / 8, -PARAM::Field::PITCH_WIDTH / 4);
    _otherPos[6] = CGeoPoint(-PARAM::Field::PITCH_LENGTH * 3 /8,  PARAM::Field::PITCH_WIDTH / 4);
    _otherPos[7] = CGeoPoint(-PARAM::Field::PITCH_LENGTH * 3 /8, -PARAM::Field::PITCH_WIDTH / 4);

}

void CMessiDecision::generateAttackDecision(const CVisionModule* pVision) {
    //更新图像信息
    _pVision = pVision;
    _cycle = pVision->getCycle();
    //判断状态
    judgeState();
    //确定Leader
    confirmLeader();
    //计算Leader的截球点
    generateLeaderPos();
    //计算Receiver的跑位点
    generateReceiverAndPos();
    //判断Leader的状态
    judgeLeaderState();
    //获取其他跑位点
    generateOtherPos();
    //获取freekick点
    freeKickPos();

    //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,300),QString("%1").arg(_otherPos[0].x()).toLatin1());
    //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,600),QString("%1").arg(_otherPos[0].y()).toLatin1());
    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-200,PARAM::Field::PITCH_WIDTH/2+200),QString(_isFlyTime?"Fly":"NoFly").toLatin1(),COLOR_BLUE);
    GDebugEngine::Instance()->gui_debug_line(_firstChipPos,_firstChipPos + Utils::Polar2Vector(2000,_firstChipDir),COLOR_YELLOW);
    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-200,PARAM::Field::PITCH_WIDTH/2), QString(MessiDecision::Instance()->needChip()?"Chip":"Flat").toLatin1(), COLOR_PURPLE);

    //获得laststate方便计算点的判断
    _laststate = _state;
}

void CMessiDecision::judgeState(){
    //判断球的状态
    string ballStatus = ZSkillUtils::instance()->getBallStatus();
    bool _ourBall = ballStatus == "Our" || ballStatus == "OurHolding";
    bool _theirBall = ballStatus == "Their" || ballStatus == "TheirHolding";

//    static int fixBuf = 0;
    //解决传球失败时错误的匹配
    if(_state == "Pass" && ZSkillUtils::instance()->getOurInterTime(_leader) - ZSkillUtils::instance()->getOurBestInterTime() > WRONG_LEADER_INTERTIME
        && !Utils::InOurPenaltyArea(_pVision->ourPlayer(ZSkillUtils::instance()->getOurBestPlayer()).Pos(),500)) {
        _state = "GetBall";
        return;
    }
//    if(_state != "fix") fixBuf = 0;
//    //保持fix状态
//    if(fixBuf > 0){
//        _state = "fix";

//    }
    static int count = 0;
    static string lastTemp = "GetBall";
    string temp = "GetBall";
    //所有状态机的判断
    const PlayerVisionT& enemy = _pVision->theirPlayer(ZSkillUtils::instance()->getTheirBestPlayer());
    bool enemyHoldBall = (enemy.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_SIZE,enemy.Dir())).dist(_pVision->ball().Pos()) < HOLDBALL_DIST;
    const PlayerVisionT& leader = _pVision->ourPlayer(_leader);
    bool leaderHoldBall = (leader.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_SIZE,leader.Dir())).dist(_pVision->ball().Pos()) < HOLDBALL_DIST;
    bool leaderInfrared = RobotSensor::Instance()->IsInfraredOn(_leader);
    if(leaderInfrared || (_ourBall || (leaderHoldBall && !enemyHoldBall))){
        temp = "Pass";
    } else if(_theirBall || enemyHoldBall){
        temp = "GetBall";
    } else {
        temp = _state;
    }
    //状态机持续时间大于5帧，跳转
//    if(lastTemp != temp){
//        count = 0;
//    }else {
//        count++;
//    }

    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,1000),QString("%1").arg(count).toLatin1());

//    if(temp == "Pass" && count >=25){
//        _state = "Pass";
//        count = 0;
//    }else if(temp == "GetBall"&&count >= 5){
//        _state = "GetBall";
//        count = 0;
//    }
//    else{
//        _state = lastTemp;
//    }

    if(_state=="Pass") _isChange = true;
    if(_state == "GetBall") _isPassC = true;
    if(count > 25){
        if (_isChange && temp == "GetBall"){
            _state = "GetBall";
            _isChange = false;
        }
        else if(_isPassC && temp == "Pass"){
            _state = "Pass";
            _isPassC = false;
        }
        count = 0;
    }
    if(_isChange||_isPassC) count++;



    lastTemp = temp;
}

void CMessiDecision::generateLeaderPos() {
    // fix状态使用的截球点
    static int ballStopCnt = 0;
    ballStopCnt = (_pVision->ball().Vel().mod() < 15 && _pVision->ourPlayer(_leader).Pos().dist(_pVision->ball().Pos()) > 1500) ? ballStopCnt + 1 : 0;
    if(ballStopCnt >= 5)
        _leaderWaitPos = _pVision->ball().Pos();
    else if(_cycle == _lastChangeLeaderCycle /*&& _laststate != "fix"*/)
        _leaderWaitPos = _receiverPos;

    if(_pVision->ourPlayer(_leader).Pos().dist(_pVision->ball().Pos()) < 500)
        _leaderPos = _pVision->ball().Pos();
    else if(_cycle == _lastChangeLeaderCycle /*&& _laststate != "fix"*/)
        _leaderPos = _receiverPos;
    else
        _leaderPos = ZSkillUtils::instance()->getOurInterPoint(_leader);
}

//确定Leader
void CMessiDecision::confirmLeader(){
    bool changeLeader = false;
    if(_cycle - _lastChangeLeaderCycle > MIN_CHANGE_LEADER_INTERVEL) {
        //选择最快的拦截车作为Leader，而不是单纯使用距离
        int bestInterPlayer = _leader;
        double bestInterTime = ZSkillUtils::instance()->getOurInterTime(bestInterPlayer) - 0.1;
        double bestInterPointDist = _pVision->ourPlayer(bestInterPlayer).Pos().dist(ZSkillUtils::instance()->getOurInterPoint(bestInterPlayer)) - 150;
        //为了防止硬件层面有车空踢导致踢球判断失误
        if(RobotSensor::Instance()->IsKickerOn(_leader) > 0 && !Utils::InTheirPenaltyArea(_passPos,0)){
            if(!_isFlat){
                if(_flydiff) {
                    _isFlyTime = true;
                    _flydiff = false;
                    _firstChipDir = _pVision->ourPlayer(_leader).Dir();
                    _firstChipPos = _pVision->ball().Pos();
                }
            }
            _leader = _receiver;
            changeLeader = true;
        }
        //状态跳转，Pass时，选择最快截球车
        else if((_state != _laststate/*RobotSensor::Instance()->IsKickerOn(_leader) > 0*/||  (_state == "Pass" && _pVision->ball().Vel().mod() < 650)) || _pVision->ball().Vel().mod() >= 1000 /*&& _state!="fix"*/) {
            //情况允许就选后卫
            for(int i = 0;i < PARAM::Field::MAX_PLAYER; i++){
                if(judcanuseBack(i)){
                   double interTime = ZSkillUtils::instance()->getOurInterTime(i);
                   double newInterPointDist = _pVision->ourPlayer(i).Pos().dist(ZSkillUtils::instance()->getOurInterPoint(i));
                    if(interTime < bestInterTime && newInterPointDist < bestInterPointDist){
                        bestInterTime = interTime;
                        bestInterPlayer = i;
                        bestInterPointDist = newInterPointDist;
                    }
                }
            }
            if(_leader != bestInterPlayer) {
                _leader = bestInterPlayer;
                changeLeader = true;
            }
        }

        if(!RobotSensor::Instance()->IsKickerOn(_leader)) _flydiff = true;

        //激进打法
//        else if(_state == "GetBall"){
//            for(int i = 0;i < PARAM::Field::MAX_PLAYER; i++){
//                if(_pVision->ourPlayer(i).Valid() && !Utils::InOurPenaltyArea(_pVision->ourPlayer(i).Pos(),0) && !(Utils::InOurPenaltyArea(_pVision->ball().Pos(),1300) && _pVision->theirPlayer(ZSkillUtils::instance()->getTheirBestPlayer()).Pos().dist(_pVision->ball().Pos()) < 150)){
//                    double interTime = ZSkillUtils::instance()->getOurInterTime(i);
//                    double newInterPointDist = _pVision->ourPlayer(i).Pos().dist(ZSkillUtils::instance()->getOurInterPoint(i));
//                    if(interTime < bestInterTime && newInterPointDist < bestInterPointDist) {
//                        bestInterTime = interTime;
//                        bestInterPlayer = i;
//                        bestInterPointDist = newInterPointDist;
//                    }
//                }
//            }
//            if(_leader != bestInterPlayer){
//                _leader = bestInterPlayer;
//                changeLeader = true;
//            }
//        }
        if(changeLeader) _lastChangeLeaderCycle = _cycle;
    }
    //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,-1000平),QString("%1").arg(diff).toLatin1(),COLOR_RED);
}

//判断是否要重算
bool CMessiDecision::needReceivePos(){
    CGeoPoint leaderPosition = _pVision->ourPlayer(_leader).Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_SIZE,_pVision->ourPlayer(_leader).Dir());
    CGeoPoint ballPosition = _pVision->ball().Pos();
    if(_leader == _receiver) {
        RECALCULATE::recomputeCondition = "LEADER = RECEIVER";
        return true;
    }
    //leader拿到球后，判断此时传球点是否有效
    else if(inValidPassCnt == RECALCULATE::MAX_INVALID_CNT) {
        RECALCULATE::recomputeCondition = "INVALID PASS";
        return true;
    }
    else if(inValidShootCnt == RECALCULATE::MAX_INVALID_CNT){
        RECALCULATE::recomputeCondition = "INVALID SHOOT";
    }
    //当传球点太近时,重新计算
    else if(!Utils::InTheirPenaltyArea(_passPos,0) && leaderPosition.dist(ballPosition) < HOLDBALL_DIST && ZPassPosEvaluate::instance()->passTooClose(_passPos,_leaderPos)){
        RECALCULATE::recomputeCondition = "TOO CLOSE";
        return true;
    }
    //极端条件：receiver被拿下去，receiver 在禁区
    else if((!Utils::InTheirPenaltyArea(_passPos,0) && !_pVision->ourPlayer(_receiver).Valid())||!Utils::InOurPenaltyArea(_pVision->ourPlayer(_receiver).Pos(),0)) {
        RECALCULATE::recomputeCondition = "INVALID RECEIVER";
        return true;
    }
    return false;
}

// passPos在前场就传passPos，不在前场就传前场干扰对面
CGeoPoint CMessiDecision::goaliePassPos(){
    if(_passPos.x() > 0){
        return _passPos;
    }else{
        return CGeoPoint(500,0);
    }
}

void CMessiDecision::getPassPos(){
    PASS::maxPassScore = max(ZGetBestUtils::Instance()->getBestFlatPassQ(), ZGetBestUtils::Instance()->getBestChipPassQ());
    PASS::maxShootScore = max(ZGetBestUtils::Instance()->getBestFlatShootQ(), ZGetBestUtils::Instance()->getBestChipShootQ());

    if(ONLY_PASSPOS){
        PASS::passMode = true;
    }
    else if (ONLY_SHOOTPOS || Utils::InTheirPenaltyArea(_pVision->ourPlayer(_leader).Pos(), 1000)){
        PASS::passMode = false;
    }
    else{
        PASS::passMode = PASS::maxShootScore > PASS::CHOOSE_SHOOT_THRESHOLD ? false : true;//DEBUG::maxShootScore - DEBUG::maxPassScore > CHOOSE_SHOOT_DIFF ? false : true;
    }

    // 维护最佳平射点和挑射点
    _flatPassPos  = Utils::IsInField(ZGetBestUtils::Instance()->getBestFlatPass()) ? ZGetBestUtils::Instance()->getBestFlatPass() : _flatPassPos;
    _flatShootPos = Utils::IsInField(ZGetBestUtils::Instance()->getBestFlatShoot()) ? ZGetBestUtils::Instance()->getBestFlatShoot() : _flatShootPos;
    _chipPassPos  = Utils::IsInField(ZGetBestUtils::Instance()->getBestChipPass()) ? ZGetBestUtils::Instance()->getBestChipPass() : _chipPassPos;
    _chipShootPos = Utils::IsInField(ZGetBestUtils::Instance()->getBestChipShoot()) ? ZGetBestUtils::Instance()->getBestChipShoot() : _chipShootPos;
//    _flatPassQ    = ZGetBestUtils::Instance()->getBestFlatPassQ()!=-99999 ? ZGetBestUtils::Instance()->getBestFlatPass() : _flatPassPos;
//    _flatShootQ   = Utils::IsInField(ZGetBestUtils::Instance()->getBestFlatShoot()) ? ZGetBestUtils::Instance()->getBestFlatShoot() : _flatShootPos;
//    _chipPassQ    = Utils::IsInField(ZGetBestUtils::Instance()->getBestChipPass()) ? ZGetBestUtils::Instance()->getBestChipPass() : _chipPassPos;
//    _chipShootQ   = Utils::IsInField(ZGetBestUtils::Instance()->getBestChipShoot()) ? ZGetBestUtils::Instance()->getBestChipShoot() : _chipShootPos;

    // 获得平射点和挑球点
    CGeoPoint flatPassPoint = PASS::passMode ? ZGetBestUtils::Instance()->getBestFlatPass() : ZGetBestUtils::Instance()->getBestFlatShoot();
    CGeoPoint chipPassPoint = PASS::passMode ? ZGetBestUtils::Instance()->getBestChipPass() : ZGetBestUtils::Instance()->getBestChipShoot();
    PASS::flatPassQ = PASS::passMode ? ZGetBestUtils::Instance()->getBestFlatPassQ() : ZGetBestUtils::Instance()->getBestFlatShootQ();
    PASS::chipPassQ = PASS::passMode ? ZGetBestUtils::Instance()->getBestChipPassQ() : ZGetBestUtils::Instance()->getBestChipShootQ();

    // 选择平射点或者挑射点 //need to modify
    if(Utils::IsInField(flatPassPoint) && Utils::IsInField(chipPassPoint)){
        //_isFlat = PASS::chipPassQ - PASS::flatPassQ > PASS::CHOOSE_FLAT_DIFF ? false : true;
        _isFlat = judgePassModule(_leader,flatPassPoint);
        //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,2000),QString("%1").arg(1).toLatin1());
    }
    else if(Utils::IsInField(chipPassPoint)){
        _isFlat = judgePassModule(_leader,flatPassPoint);
    }
    else if(Utils::IsInField(flatPassPoint)){
        _isFlat = judgePassModule(_leader,flatPassPoint);
    }
    if(ONLY_FLATPOS)
        _isFlat = true;
    if(ONLY_CHIPPOS)
        _isFlat = false;

    //特殊情况，算不出最佳跑位点
    if(ZGetBestUtils::Instance()->getBestFlatPassQ() == -99999 && ZGetBestUtils::Instance()->getBestChipPassQ() == -99999){
        _lastUpdateRunPosCycle = _cycle;
        if (RECALCULATE::recomputeCondition == "PASS TO BACK")
        {
            _passPos = _receiverPos = _pVision->ourPlayer(_leader).Pos().y() < 0 ? STATIC_POS[0] : STATIC_POS[1];
        }
    }

    // 计算receiver、passvel和receivePos
    int lastReceiver = _receiver;

    if(_isFlat && Utils::IsInField(flatPassPoint)){
        if(_diff > 20 || _leader != _lastLeader){
            _diff = 0;
            _lastLeader = _leader;
            _passPos = _receiverPos = flatPassPoint;
            _receiver = PASS::passMode ? ZGetBestUtils::Instance()->getBestFlatPassNum() : ZGetBestUtils::Instance()->getBestFlatShootNum();
        } else {
            _diff++;
            //_passPos = _receiverPos = _passPos;
            //if(_diff > 20) _diff = 0;
        }
        inValidPassCnt = 0;
        inValidShootCnt = 0;
    }
    else if(!_isFlat && Utils::IsInField(chipPassPoint)){
        //_passPos = _receiverPos = chipPassPoint;
        if(_diff > 20 || _leader != _lastLeader){
            _diff = 0;
            _lastLeader = _leader;
            _passPos = _receiverPos = chipPassPoint;
            _receiver = PASS::passMode ? ZGetBestUtils::Instance()->getBestChipPassNum() : ZGetBestUtils::Instance()->getBestChipShootNum();
        } else {
            _diff++;
            //_passPos = _receiverPos = _passPos;
            //if(_diff > 20) _diff = 0;
        }
        inValidPassCnt = 0;
        inValidShootCnt = 0;
    }
    else{
        _passPos = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0);
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(1000,0),QString("%1").arg(1).toLatin1(),COLOR_RED);
    }
    //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(1000,0),QString("%1").arg(ZSkillUtils::instance()->getOurInterTime(_leader)).toLatin1(),COLOR_RED);


    //_receiver = ZGetBestUtils::Instance()->getBestFlatPassNum();

    //判断当前是否改变receiver
    if(_receiver != lastReceiver) _lastChangeReceiverCycle = _cycle;
}

//计算Receiver跑位点
void CMessiDecision::generateReceiverAndPos(){
    if(needReceivePos()){
        ZGetBestUtils::Instance()->setLeader(_leader);
        ZGetBestUtils::Instance()->setLeaderPos(_leaderPos);
        if(_cycle - _lastChangeReceiverCycle > MIN_CHANGE_RECEIVR_INTERVEL || _leader == _receiver ||_pVision->ourPlayer(_receiver).Valid()){
            ZGetBestUtils::Instance()->setReceiver(-1);
        }
        else
            ZGetBestUtils::Instance()->setReceiver(_receiver);
        //if(_isNormalPlay){
            ZGetBestUtils::Instance()->run();
        //}
        getPassPos();
    }

    // 意外情况处理:拿不到点时leader还在上一个receiverPos
    static int inValidPosCnt = 0;
    static const double CLOSE_DIST = 1000;
    inValidPosCnt = _receiverPos.dist(_pVision->ourPlayer(_leader).Pos()) < CLOSE_DIST ? inValidPosCnt + 1 : 0;
    if(inValidPosCnt > 3*PARAM::Vision::FRAME_RATE){
        double dist1 = STATIC_POS[0].dist(_pVision->ourPlayer(_leader).Pos());
        double dist2 = STATIC_POS[1].dist(_pVision->ourPlayer(_leader).Pos());
        if(dist1 > CLOSE_DIST && dist2 > CLOSE_DIST)
            _passPos = _receiverPos = dist1 < dist2 ? STATIC_POS[0] : STATIC_POS[1];
        else
            _passPos = _receiverPos = dist1 > CLOSE_DIST ? STATIC_POS[0] : STATIC_POS[1];
    }
    // 判断能不能射门
    if (ShootModule::Instance()->canShoot(_pVision, _leaderPos) /*|| Utils::InTheirPenaltyArea(_leaderPos,1500)*/) {\
        _passPos = CGeoPoint(PARAM::Field::PITCH_LENGTH / 2.0, 0);
        _passVel = 6000;
        _isFlat = true;
        //receiver应该跑的点，避免挡住射门路线
//        _receiverPos = _pVision->ourPlayer(_leader).Pos().y() < 0 ? STATIC_POS[0] : STATIC_POS[1];
    }
}

bool CMessiDecision::judcanuseBack(int robotNum){
   if(TaskMediator::Instance()->isGoalie(robotNum) /*|| TaskMediator::Instance()->isBack(robotNum)*/) return false;
//    if(_pVision->ourPlayer(robotNum).Valid()&&Utils::InOurPenaltyArea(_pVision->ourPlayer(robotNum).Pos(),6 * PARAM::Vehicle::V2::PLAYER_SIZE)&&!Utils::InOurPenaltyArea(_pVision->ourPlayer(robotNum).Pos(),0)){
//        CGeoPoint interPos = ZSkillUtils::instance()->getOurInterPoint(robotNum);
//        double interTime   = ZSkillUtils::instance()->getOurInterTime(robotNum);
//        double tinterTime = 99999;
//        double ballAcc = FRICTION/2;
        //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-2500,0),QString::number(TaskMediator::Instance()->isGoalie(1)).toLatin1());
//        if(!Utils::InOurPenaltyArea(interPos,PARAM::Vehicle::V2::PLAYER_SIZE * 1.5)){
//            for(int i = 0;i<PARAM::Field::MAX_PLAYER;++i){
//                if(!_pVision->theirPlayer(i).Valid()||Utils::InTheirPenaltyArea(_pVision->theirPlayer(i).Pos(),PARAM::Vehicle::V2::PLAYER_SIZE*6)) continue;
//                if(tinterTime > ZSkillUtils::instance()->getTheirInterTime(i))
//                    tinterTime = ZSkillUtils::instance()->getTheirInterTime(i);
//            }
//            double balltoInterPosTime = 2 * _pVision->ball().Pos().dist(interPos) / (_pVision->ball().Vel().mod() + sqrt(pow(_pVision->ball().Vel().mod(),2) - 2 * ballAcc * _pVision->ball().Pos().dist(interPos)));
//            if(tinterTime > interTime + 0.2 && balltoInterPosTime > interTime)
//                return true;
//            else
//            return false;
//        }
//        return false;
//    }
    double theirTime = ZSkillUtils::instance()->getTheirBestInterTime();
    double ourTime   = ZSkillUtils::instance()->getOurInterTime(robotNum);
    CGeoPoint ourpoint = ZSkillUtils::instance()->getOurInterPoint(robotNum);
    GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(2000,0),QString("%1").arg(TaskMediator::Instance()->isBack(2)).toLatin1());
    bool candidate = !Utils::InOurPenaltyArea(ourpoint,PARAM::Vehicle::V2::PLAYER_SIZE*1.5) ;//&& Utils::InOurPenaltyArea(ourpoint,PARAM::Vehicle::V2::PLAYER_SIZE*11);
    if(TaskMediator::Instance()->isBack(robotNum) && candidate && theirTime > ourTime + 0.2)
        return true;
    else if(_pVision->ourPlayer(robotNum).Valid() && !Utils::InOurPenaltyArea(_pVision->ourPlayer(robotNum).Pos(),6 * PARAM::Vehicle::V2::PLAYER_SIZE) && !TaskMediator::Instance()->isGoalie(robotNum))
        return true;
    else
        return false;
}

// 判断leader状态
void CMessiDecision::judgeLeaderState() {
    //计算传球力度
    if(PASS::selfPass)
    {
        SIGNAL_TO_CUDA = true;
        _leaderState = "SELFPASS";
    }
    else
    {
        SIGNAL_TO_CUDA = false;
        _leaderState = "PASS";
    }
    if(Utils::InTheirPenaltyArea(_passPos, 0)){
        _passVel = 6000;
        _isFlat = true;
    }
    else if(_isFlat){
//        if(_leaderState == "SELFPASS")
//            _passVel = 1000.0;
//        else
            _passVel = BallModel::instance()->flatPassVel(_pVision, _passPos, _receiver, PASS::bufferTime, RECALCULATE::PASS_ANGLE_ERROR);
//            if(PASS::selfPass){
//                if (_passVel < 1500.0) _passVel = 1500.0;
//                if (_passVel > 1900.0) _passVel = 1900.0;
//            }
    }
    else
        _passVel = BallModel::instance()->chipPassVel(_pVision, _passPos);
    //判断leader应该调什么skill
    /*if(_leader == _receiver)
        _leaderState = "COMPUTE";
    else */

    //判断leader能否传球
    CGeoPoint leaderPosition = _pVision->ourPlayer(_leader ).Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_SIZE, _pVision->ourPlayer(_leader).Dir());
    CGeoPoint ballPosition = _pVision->ball().Pos();
    CVector passLine = _passPos - ballPosition;
//    CGeoPoint abnormalPos1 = ballPosition + Utils::Polar2Vector(ballPosition.dist(_passPos), passLine.dir() + RECALCULATE::PASS_ANGLE_ERROR*PARAM::Math::PI/180);
//    CGeoPoint abnormalPos2 = ballPosition + Utils::Polar2Vector(ballPosition.dist(_passPos), passLine.dir() - RECALCULATE::PASS_ANGLE_ERROR*PARAM::Math::PI/180);
    double interTime;
    _canKick = false;
    _canKickReceiver = false;



    const static double riskPosX = PARAM::Field::PITCH_LENGTH/2 - PARAM::Field::PENALTY_AREA_DEPTH - 4*PARAM::Vehicle::V2::PLAYER_SIZE;
    const static double riskPassX = PARAM::Field::PITCH_LENGTH/2 - PARAM::Field::PENALTY_AREA_DEPTH - 10*PARAM::Vehicle::V2::PLAYER_SIZE;

    //特殊情况放宽条件:禁区附近
    if(!_isFlat && Utils::InTheirPenaltyArea(_receiverPos, 1500) && _receiverPos.x() > riskPosX && leaderPosition.x() > riskPassX){
//        _canKick = true;
        _canKickReceiver = true;
    }
    //非自传的平射点
    else if(_isFlat && !PASS::selfPass && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, _receiverPos, interTime, RECALCULATE::THEIR_RESPONSE_TIME, RECALCULATE::IGNORE_CLOSE_ENEMY_DIST, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG)/* && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, abnormalPos1, interTime, RECALCULATE::THEIR_RESPONSE_TIME, RECALCULATE::IGNORE_CLOSE_ENEMY_DIST, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG) && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, abnormalPos2, interTime, RECALCULATE::THEIR_RESPONSE_TIME, RECALCULATE::IGNORE_CLOSE_ENEMY_DIST, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG)*/){
//        _canKick = true;
        _canKickReceiver = true;
    }
    //自传的平射点
    else if(_isFlat && PASS::selfPass){// && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, _receiverPos, interTime, RECALCULATE::THEIR_RESPONSE_TIME, -9999, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG)/* && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, abnormalPos1, interTime, 0.6, -9999, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG) && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, abnormalPos2, interTime, 0.6, -9999, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG)*/){
//        _canKick = true;
        _canKickReceiver = true;
    }
    //挑射点
    else if (!_isFlat && ZSkillUtils::instance()->validChipPos(_pVision, leaderPosition, _passVel, _receiverPos, RECALCULATE::THEIR_CHIP_RESPONSE_TIME, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG) /*&& ZSkillUtils::instance()->validChipPos(_pVision, leaderPosition, _passVel, abnormalPos1, RECALCULATE::THEIR_CHIP_RESPONSE_TIME, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG) && ZSkillUtils::instance()->validChipPos(_pVision, leaderPosition, _passVel, abnormalPos2, RECALCULATE::THEIR_CHIP_RESPONSE_TIME, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG)*/) {
//        _canKick = true;
        _canKickReceiver = true;
    }

    //射门
    if(Utils::InTheirPenaltyArea(_passPos, 0)){
        if(ShootModule::Instance()->canShoot(_pVision, _leaderPos))
            _canKick = true;
    }
    else _canKick = _canKickReceiver;
//    //传球
//    else {
//        const static double riskPosX = PARAM::Field::PITCH_LENGTH/2 - PARAM::Field::PENALTY_AREA_DEPTH - 4*PARAM::Vehicle::V2::PLAYER_SIZE;
//        const static double riskPassX = PARAM::Field::PITCH_LENGTH/2 - PARAM::Field::PENALTY_AREA_DEPTH - 10*PARAM::Vehicle::V2::PLAYER_SIZE;
//        //特殊情况放宽条件:禁区附近
//        if(!_isFlat && Utils::InTheirPenaltyArea(_passPos, 1000) && _passPos.x() > riskPosX && leaderPosition.x() > riskPassX){
//            _canKick = true;
//        }
//        //非自传的平射点
//        else if(_isFlat && !PASS::selfPass && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, _passPos, interTime, RECALCULATE::THEIR_RESPONSE_TIME, RECALCULATE::IGNORE_CLOSE_ENEMY_DIST, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG)/* && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, abnormalPos1, interTime, RECALCULATE::THEIR_RESPONSE_TIME, RECALCULATE::IGNORE_CLOSE_ENEMY_DIST, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG) && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, abnormalPos2, interTime, RECALCULATE::THEIR_RESPONSE_TIME, RECALCULATE::IGNORE_CLOSE_ENEMY_DIST, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG)*/){
//            _canKick = true;
//        }
//        //自传的平射点
//        else if(_isFlat && PASS::selfPass && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, _passPos, interTime, 0.6, -9999, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG)/* && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, abnormalPos1, interTime, 0.6, -9999, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG) && ZSkillUtils::instance()->validShootPos(_pVision, leaderPosition, _passVel, abnormalPos2, interTime, 0.6, -9999, true, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG)*/){
//            _canKick = true;
//        }
//        //挑射点
//        else if (!_isFlat && ZSkillUtils::instance()->validChipPos(_pVision, leaderPosition, _passVel, _passPos, RECALCULATE::THEIR_CHIP_RESPONSE_TIME, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG) /*&& ZSkillUtils::instance()->validChipPos(_pVision, leaderPosition, _passVel, abnormalPos1, RECALCULATE::THEIR_CHIP_RESPONSE_TIME, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG) && ZSkillUtils::instance()->validChipPos(_pVision, leaderPosition, _passVel, abnormalPos2, RECALCULATE::THEIR_CHIP_RESPONSE_TIME, RECALCULATE::IGNORE_THEIR_GUARD, INTER_DEBUG)*/) {
//            _canKick = true;
//        }
//    }
    //判断传球点是否非法
    if((leaderPosition.dist(ballPosition) < HOLDBALL_DIST || RobotSensor::Instance()->IsInfraredOn(_leader)) && !_canKickReceiver){
        inValidPassCnt = inValidPassCnt >= RECALCULATE::MAX_INVALID_CNT ? RECALCULATE::MAX_INVALID_CNT : inValidPassCnt + 1;
    }
    else {
        inValidPassCnt = 0;
    }

    if((leaderPosition.dist(ballPosition) < HOLDBALL_DIST || RobotSensor::Instance()->IsInfraredOn(_leader)) && !_canKick){
        inValidShootCnt = inValidShootCnt >= RECALCULATE::MAX_INVALID_CNT ? RECALCULATE::MAX_INVALID_CNT : inValidShootCnt + 1;
    }
    else {
        inValidShootCnt = 0;
    }


    // 当receiver离挑球点比较近的时候踢出
    if((!_isFlat && !PASS::selfPass && _pVision->ourPlayer(_receiver).Pos().dist(_passPos) > 1000) || (_isFlat && !PASS::selfPass && !Utils::InTheirPenaltyArea(_passPos, 0) && predictedTime(_pVision->ourPlayer(_receiver), _passPos) > 1))
        _canKick = false;
    // 后场没点时强行挑射
//    if(!_isFlat && _pVision->ourPlayer(_leader).X() < /*-PARAM::Field::PITCH_LENGTH/8*/0 && Utils::InTheirPenaltyArea(_passPos, 50))
//        _canKick = true;

    // NO KICK计数
//    if ((leaderPosition.dist(ballPosition) < HOLDBALL_DIST || RobotSensor::Instance()->IsInfraredOn(_leader)) && !_canKick) PASS::NO_KICK_CNT = PASS::NO_KICK_CNT < static_cast<int>(PARAM::Vision::FRAME_RATE * PASS::MAX_NO_KICK) ? PASS::NO_KICK_CNT + 1 : static_cast<int>(PARAM::Vision::FRAME_RATE * PASS::MAX_NO_KICK);
//    else PASS::NO_KICK_CNT = 0;
//    if (PASS::NO_KICK_CNT >= static_cast<int>(PARAM::Vision::FRAME_RATE * PASS::MAX_NO_KICK) || leaderPosition.x() <= -PARAM::Field::PITCH_LENGTH/3){
//        _passPos = _receiverPos = CGeoPoint(PARAM::Field::PITCH_LENGTH/2 - PARAM::Field::PENALTY_AREA_DEPTH - 350, _pVision->ball().Y()>0 ? 800 : -800);
//        _passVel = BallModel::instance()->chipPassVel(_pVision, _passPos);
//        _isFlat = false;
//        _canKick = true;
//    }

    //判断我方挑球时接球状态
    if(_isFlyTime){
        flydiff++;
        if(flydiff > 60) _isFlyTime = false;
    } else {
        flydiff = 0;
    }
}

bool CMessiDecision::judgePassModule(const int playernum,CGeoPoint passpos){
    const PlayerVisionT & me = _pVision->ourPlayer(playernum);
    for(int i= 0; i < PARAM::Field::MAX_PLAYER;++i){
        if(!_pVision->theirPlayer(i).Valid() || Utils::InTheirPenaltyArea(_pVision->theirPlayer(i).RawPos(),minRadaius * 3.0)) continue;
           //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,1000),QString("go").toLatin1());
           const PlayerVisionT & enemy = _pVision->theirPlayer(i);
           CVector me2enemy = enemy.RawPos() - me.RawPos();
           //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,0),QString("%1").arg(enemy.RawPos().x()).toLatin1());
           CGeoSegment passLine(me.RawPos(),passpos);
           CGeoPoint projection = passLine.projection(enemy.Pos());
           CVector enemy2projection = enemy.Pos() - projection;
           CVector enemy2me = enemy.RawPos() - me.RawPos();
           CVector me2projection = me.RawPos() - projection;
           //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,2000),QString("%1").arg(passLine.IsPointOnLineOnSegment(projection)).toLatin1());
           if(passLine.IsPointOnLineOnSegment(projection)){
               if(me2enemy.mod() < minRadaius) {
                   return false;
               } else {
                   if(enemy2projection.mod() < enemy2me.mod() * 2 / 5 && me2projection.mod() > enemy2projection.mod()){
                        return false;
                   }
               }
           }
       }
    return true;
}

CGeoPoint CMessiDecision::freeKickPos() {
    CGeoPoint freekickpos;
    if(freetime < 1){
        freeposLast = ZGetBestUtils::Instance()->getBestFreeKickPos();
        freetime++;
    } else {
        freetime++;
        if(freetime > 50) freetime = 0;
    }
    freekickpos = freeposLast;
    _isfreeFlat = judgePassModule(_leader,freekickpos);

    double bufferdist = 99999;
    int receiver = -1;
    for(int i = 0;i<PARAM::Field::MAX_PLAYER;++i){
        if(i==_leader) continue;
        if(vision->ourPlayer(i).Valid() && vision->ourPlayer(i).Pos().dist(freekickpos) < bufferdist){
            receiver = i;
            bufferdist = vision->ourPlayer(i).Pos().dist(freekickpos);
        }
    }
    //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,0),QString("%1").arg(BallModel::instance()->flatPassVel(vision,freekickpos,receiver, PASS::bufferTime, RECALCULATE::PASS_ANGLE_ERROR)).toLatin1());
    if(_isfreeFlat) _freepassVel = BallModel::instance()->flatPassVel(vision,freekickpos,receiver, PASS::bufferTime, RECALCULATE::PASS_ANGLE_ERROR);
    else _freepassVel = BallModel::instance()->chipPassVel(vision,freekickpos);
    //}
    return freekickpos;

}

bool CMessiDecision::needRunPos(){
    bool needRunPos = false;
//    //状态改变时算一次
//    if(_laststate!=_state){
//        needRunPos = true;
//    }
    //一般情况，隔一段时间更新一次
    if((_cycle - _lastUpdateRunPosCycle)>50){
        needRunPos = true;
    }

    //能射门时不算点
    if(ShootModule::Instance()->canShoot(_pVision,_leaderPos)){
        needRunPos = false;
    }
    return needRunPos;
}

void CMessiDecision::generateOtherPos(){
    if(needRunPos()){
        RunPosModule::Instance()->generateRunPos(_pVision,_leaderPos,_receiverPos);
        _otherPos[0] = RunPosModule::Instance()->runPos(0);
        _otherPos[1] = RunPosModule::Instance()->runPos(1);
        _otherPos[2] = RunPosModule::Instance()->runPos(2);
        _otherPos[3] = RunPosModule::Instance()->runPos(3);
        _otherPos[4] = RunPosModule::Instance()->runPos(4);
        _otherPos[5] = RunPosModule::Instance()->runPos(5);
        if(USE_11vs11SOLVER){
            _otherPos[6] = RunPosModule::Instance()->runPos(6);
            _otherPos[7] = RunPosModule::Instance()->runPos(7);
        }
        _lastUpdateRunPosCycle = _cycle;
    }
}
