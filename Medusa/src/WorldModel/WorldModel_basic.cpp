#include "WorldModel.h"
#include "VisionModule.h"
#include "RobotCapability.h"
#include "ControlModel.h"
#include "RobotSensor.h"
#include "KickStatus.h"
#include "DribbleStatus.h"
#include "GDebugEngine.h"
#include "PlayInterface.h"
#include "BufferCounter.h"
#include "defence/DefenceInFo.h"

// 默认参数初始化
const int CWorldModel::myDefaultNum = 0;
const int CWorldModel::enemyDefaultNum = 0;
namespace{
    const double beta = 1.0;
    bool deltaMarginKickingMetric(int current_cycle, double gt, double delta, double mydir, int myNum)
    {
        // 计算当前的margin并做记录
        double gl = Utils::Normalize(gt - delta);	// 目标方向向左偏置射门精度阈值
        double gr = Utils::Normalize(gt + delta);	// 目标方向向右偏置射门精度阈值
        double current_margin = max(Utils::Normalize(mydir - gl), Utils::Normalize(gr - mydir));
        return (current_margin > 0 && current_margin < beta*2*delta);
    }
    ///> 关于控球
    // 控球控制阈值
    const bool Allow_Start_Dribble = true;
    const double Allow_Start_Dribble_Dist = 6.0 * PARAM::Vehicle::V2::PLAYER_SIZE;
    const double Allow_Start_Dribble_Angle = 5.0 * PARAM::Vehicle::V2::KICK_ANGLE;
}
// 自己到球的矢量
const CVector CWorldModel::self2ball(int current_cycle,  int myNum) {
	static int last_cycle[PARAM::Field::MAX_PLAYER] = {-1,-1,-1,-1,-1,-1};
	static CVector _self2ball[PARAM::Field::MAX_PLAYER];

	if (last_cycle[myNum-1] < current_cycle) {
		_self2ball[myNum-1] = _pVision->ball().Pos() - _pVision->ourPlayer(myNum).Pos();
		last_cycle[myNum-1] = current_cycle;
	}

	return _self2ball[myNum-1];
}

// 自己到球的距离
double CWorldModel::self2ballDist(int current_cycle,  int myNum,  int enemyNum) {
    return self2ball(current_cycle, myNum).mod();	// 可以减少计算量
}

// 自己到球的角度
double CWorldModel::self2ballDir(int current_cycle,  int myNum,  int enemyNum) {
    return self2ball(current_cycle, myNum).dir();	// 可以减少计算量
}

const string CWorldModel::CurrentRefereeMsg()
{
	return vision()->getCurrentRefereeMsg();
}
void CWorldModel::SPlayFSMSwitchClearAll(bool clear_flag)
{
    if (! clear_flag) {
        return ;
    }
    // 暂时只有清理 球被提出的状态
    KickStatus::Instance()->resetKick2ForceClose(true,this->vision()->getCycle());
    BallStatus::Instance()->clearKickCmd();
    BufferCounter::Instance()->clear();
    // TODO
    return ;
}
bool CWorldModel::IsBallKicked(int num){
    return (RobotSensor::Instance()->IsKickerOn(num) != 0);
}
bool CWorldModel::IsInfraredOn(int num){
    return (RobotSensor::Instance()->IsInfraredOn(num));
}
int CWorldModel::InfraredOnCount(int num){
    return (RobotSensor::Instance()->fraredOn(num));
}
int CWorldModel::InfraredOffCount(int num){
    return (RobotSensor::Instance()->fraredOff(num));
}

double CWorldModel::timeToTarget(int player, const CGeoPoint target) {
//    return predictedTimeWithRawVel(_pVision->ourPlayer(player), target);
    return predictedTime(_pVision->ourPlayer(player), target);
}
//string CWorldModel::getBallStatus(){
//    for(int x = 0;x < 16;x++)
//    {
//        const PlayerVisionT& pos= pVision->theirPlayer(num);
//    }
//    return "s";
//}
bool CWorldModel::KickDirArrived(int current_cycle, double kickdir, double kickdirprecision, int myNum){
    const PlayerVisionT& me = this->_pVision->ourPlayer(myNum);
    return ::deltaMarginKickingMetric(current_cycle,kickdir,kickdirprecision,me.Dir(),myNum);
}

//每帧只调用一次
int CWorldModel::getEnemyKickOffNum()
{
    const string refMsg = vision()->getCurrentRefereeMsg();
    int kickOffNum = 0;
    vector<int> kickOffList;
    static int lastKickOffNum = 1;
    static int keepCnt = 0;
    static int lastCycle = 0;
    static int retVal = 0;
    if (vision()->getCycle() - lastCycle >6){
        lastKickOffNum = 1;
        keepCnt = 0;
    }
    if (lastCycle == vision()->getCycle()){
        return retVal;
    }
    kickOffList.clear();
    //暂且用区域内的车的数量判断敌方开球车数量
    if ("TheirIndirectKick" == refMsg || "TheirDirectKick" == refMsg){
        CGeoCirlce kickOffArea = CGeoCirlce(vision()->ball().Pos(),PARAM::AvoidDist::DEFENDKICK_MARKING_DIST);
        for (int i =0;i<PARAM::Field::MAX_PLAYER;i++){
            if (kickOffArea.HasPoint(vision()->theirPlayer(i).Pos())){
                kickOffNum++;
                //cout<<i<<" might be kicking"<<endl;
                kickOffList.push_back(i);
            }
        }
    }
    if (kickOffNum == lastKickOffNum){
        keepCnt++;
    }else if (kickOffNum!=lastKickOffNum && kickOffNum!=0){
        lastKickOffNum = kickOffNum;
        keepCnt = 0;
    }else{
        keepCnt = 0;
    }
    //当有大于两辆车开球时，剔除不合理的盯人车
    if (keepCnt >=3 && kickOffNum>=2){
        double minToBallDist = 1000;
        int minNum = 0;
        for (vector<int>::iterator ir = kickOffList.begin();ir!=kickOffList.end();ir++){
            double check =vision()->theirPlayer(*ir).Pos().dist(vision()->ball().Pos());
            //cout<<*ir<<" "<<check<<endl;
            if (check<minToBallDist){
                minNum = *ir;
                minToBallDist = check;
            }
        }
        //cout<<"minNum is "<<minNum<<endl;
        DefenceInfo::Instance()->resetMarkingInfo();
        for (vector<int>::iterator ir = kickOffList.begin();ir!=kickOffList.end();ir++){
            if (*ir!=minNum){
                DefenceInfo::Instance()->setNoMarkingNum(*ir);
            }
        }
    }else{
        DefenceInfo::Instance()->resetMarkingInfo();
    }

    lastCycle = vision()->getCycle();

    if (keepCnt>=3 && kickOffNum>=2){
        retVal = kickOffNum;
        return kickOffNum;
    }else{
        retVal = 1;
        return 1;
    }
}

int CWorldModel::getEnemyAmountInArea(double x1, double x2, double y1, double y2, std::vector<int>& enemyNumVec, double buffer) {
    enemyNumVec.clear();
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        if (_pVision->theirPlayer(i).Valid()
                && _pVision->theirPlayer(i).Pos().x() >= x1 - buffer
                && _pVision->theirPlayer(i).Pos().x() <= x2 + buffer
                && _pVision->theirPlayer(i).Pos().y() >= y1 - buffer
                && _pVision->theirPlayer(i).Pos().y() <= y2 + buffer) {
            enemyNumVec.push_back(i);
        }
    }
    return enemyNumVec.size();
}

int CWorldModel::getEnemyAmountInArea(const CGeoPoint& center ,double radius, std::vector<int>& enemyNumVec, double buffer) {
    enemyNumVec.clear();
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        if (_pVision->theirPlayer(i).Valid() && _pVision->theirPlayer(i).Pos().dist(center) <= radius + buffer) {
            enemyNumVec.push_back(i);
        }
    }
    return enemyNumVec.size();
}

CGeoPoint CWorldModel::penaltyIntersection(CGeoSegment s) {
    CGeoPoint result(999999,999999);
    CGeoSegment penaltyTop(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH, PARAM::Field::PENALTY_AREA_WIDTH / 2),
                           CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, PARAM::Field::PENALTY_AREA_WIDTH / 2)),
                penaltyLeft(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH, PARAM::Field::PENALTY_AREA_WIDTH / 2),
                            CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH, - PARAM::Field::PENALTY_AREA_WIDTH / 2)),
                penaltyDown(CGeoPoint(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH, - PARAM::Field::PENALTY_AREA_WIDTH / 2),
                            CGeoPoint(PARAM::Field::PITCH_LENGTH / 2, - PARAM::Field::PENALTY_AREA_WIDTH / 2));
    if (s.IsSegmentsIntersect(penaltyTop)) {
        result = s.segmentsIntersectPoint(penaltyTop);
        return result;
    } else if (s.IsSegmentsIntersect(penaltyLeft)){
        result = s.segmentsIntersectPoint(penaltyLeft);
        return result;
    } else if(s.IsSegmentsIntersect(penaltyDown)) {
        result = s.segmentsIntersectPoint(penaltyDown);
        return result;
    }
    return result;
}

void CWorldModel::normalizeCoordinate(CGeoPoint& p) {
    //将禁区界线化为一条长线段，线段上有两个转身节点
    if (p.y() - PARAM::Field::PENALTY_AREA_WIDTH / 2 < 1.0e-3 && p.y() - PARAM::Field::PENALTY_AREA_WIDTH / 2 > -1.0e-3) {
        p.setY(p.y() + p.x() - (PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH));
        p.setX(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH);
        return;
    }
    if (p.y() + PARAM::Field::PENALTY_AREA_WIDTH / 2 < 1.0e-3 && p.y() + PARAM::Field::PENALTY_AREA_WIDTH / 2 > -1.0e-3) {
        p.setY(p.y() - p.x() + (PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH));
        p.setX(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH);
        return;
    }
    return;
}

double CWorldModel::preditTheirGuard(const PlayerVisionT& enemy, CGeoPoint& p) {
    double time = 9999;
    double buffer = 0.2;
    //线段上的两个转身节点
    CGeoPoint b(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH, PARAM::Field::PENALTY_AREA_WIDTH / 2),
              c(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH, - PARAM::Field::PENALTY_AREA_WIDTH / 2);
    time = predictedTime(enemy, p);
    CGeoPoint enemyPos = enemy.Pos();
    //将敌方车投影到禁区边界并化为线段
    if (enemy.X() < PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH) {
        enemyPos.setX(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH);
    }
    if (abs(enemy.Y()) > PARAM::Field::PENALTY_AREA_WIDTH / 2) {
        enemyPos.setY(enemy.Y() > 0 ? PARAM::Field::PENALTY_AREA_WIDTH / 2 : -PARAM::Field::PENALTY_AREA_WIDTH / 2);
    }
    normalizeCoordinate(enemyPos);
    CGeoSegment mp(enemyPos, p);
    if (mp.IsPointOnLineOnSegment(b)) {
        time += buffer;
    }
    if (mp.IsPointOnLineOnSegment(c)) {
        time += buffer;
    }
    return time;
}


double CWorldModel::preditOurGuard(const PlayerVisionT& our, CGeoPoint& p) {
    double time = 9999;
    double buffer = 0.2;
    //线段上的两个转身节点
    CGeoPoint b(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH, PARAM::Field::PENALTY_AREA_WIDTH / 2),
              c(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH, - PARAM::Field::PENALTY_AREA_WIDTH / 2);
    time = predictedTime(our, p);
    CGeoPoint ourPos = our.Pos();
    //将敌方车投影到禁区边界并化为线段
    if (our.X() < -PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH) {
        ourPos.setX(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH);
    }
    if (abs(our.Y()) > PARAM::Field::PENALTY_AREA_WIDTH / 2) {
        ourPos.setY(our.Y() > 0 ? PARAM::Field::PENALTY_AREA_WIDTH / 2 : -PARAM::Field::PENALTY_AREA_WIDTH / 2);
    }
    normalizeCoordinate(ourPos);
    CGeoSegment mp(ourPos, p);
    if (mp.IsPointOnLineOnSegment(b)) {
        time += buffer;
    }
    if (mp.IsPointOnLineOnSegment(c)) {
        time += buffer;
    }
    return time;
}
