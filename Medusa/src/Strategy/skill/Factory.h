#ifndef _TASK_FACTORY_V2_H_
#define _TASK_FACTORY_V2_H_

#include <singleton.h>
#include <misc_types.h>
#include "staticparams.h"
/************************************************************************/
/*                         CTaskFactoryV2                               */
/************************************************************************/

class CPlayerTask;
class CTaskFactoryV2{
public:
    CPlayerTask* GotoPosition(const TaskT& task);
    CPlayerTask* SmartGotoPosition(const TaskT& task);
	CPlayerTask* StopRobot(const TaskT& task);
	CPlayerTask* Speed(const TaskT& task); // by zc
    CPlayerTask* OpenSpeed(const TaskT& task);
    CPlayerTask* Goalie(const TaskT& task);
    CPlayerTask* defend(const TaskT &task);
    CPlayerTask* Touch(const TaskT& task);

/************************************************************************/
/*                              my skill                                */
/************************************************************************/
    /////////////////////////////////////////////////////////////////////////
    ///
    ///
    ///
    //////////////////////////////////////////////////////////////////////////

    CPlayerTask* sao(const TaskT &task);//by 范佳明
    CPlayerTask* guan(const TaskT &task);//by 范佳明
    CPlayerTask* Tier(const TaskT &task);//by 范佳明
    CPlayerTask* Tier1(const TaskT &task);//by 范佳明
    CPlayerTask* naqiu(const TaskT &task);//by 范佳明
    CPlayerTask* leader(const TaskT& task);//by 贾瑞帆
    CPlayerTask* assister(const TaskT& task);//by 贾瑞帆
    CPlayerTask* special(const TaskT& task);//by 贾瑞帆
    CPlayerTask* defend_05(const TaskT& task);//by 贾瑞帆
    CPlayerTask* defend_07_0(const TaskT& task);//by 贾瑞帆
    CPlayerTask* defend_07_1(const TaskT& task);//by 贾瑞帆
    CPlayerTask* leader_getball(const TaskT& task);//by 贾瑞帆
    CPlayerTask* PenaltyKick(const TaskT &task);//by 范佳明
    CPlayerTask* PenaltyGoalie(const TaskT &task);//by 范佳明
    CPlayerTask* FetchBall(const TaskT &task);//by 范佳明
    CPlayerTask* Escape(const TaskT &task);//by 范佳明
    CPlayerTask* Marking(const TaskT &task);//by 范佳明
    CPlayerTask* MarkingFront(const TaskT &task);//by 范佳明
    CPlayerTask* MarkingTouch(const TaskT &task);//by 范佳明
    CPlayerTask* LeftHalf(const TaskT &task);//by 范佳明
    CPlayerTask* RightHalf(const TaskT &task);//by 范佳明
    CPlayerTask* MiddleHalf(const TaskT &task);//by 范佳明
    CPlayerTask* SingleTier(const TaskT &task);//by 范佳明
    CPlayerTask* ChaseKick(const TaskT& task);
    CPlayerTask* goAndTurnKick(const TaskT& task);
    CPlayerTask* StaticGetBall(const TaskT& task);
    CPlayerTask* Dnamicfindpos(const TaskT& task);
    CPlayerTask* Uniqueskillm(const TaskT& task);
    CPlayerTask* Uniqueskilll(const TaskT& task);
    CPlayerTask* Uniqueskillr(const TaskT& task);
    CPlayerTask* WDrag(const TaskT &task);
    CPlayerTask* WMarking(const TaskT &task);
    CPlayerTask* WBack(const TaskT &task);
    CPlayerTask* WBreak(const TaskT &task);
    CPlayerTask* BIGpenaltyKick(const TaskT &task);
//    CPlayerTask* GetBallV1(const TaskT& task);
//    CPlayerTask* Defend(const TaskT &task);//by 范佳明

protected:
	template < class CTaskType >
	CPlayerTask* MakeTask( const TaskT& task );
};
typedef NormalSingleton<CTaskFactoryV2> TaskFactoryV2;
/************************************************************************/
/*                 一些包裹(wrap)函数, 方便调用tasks                    */
/************************************************************************/

//! 调用skill的接口
namespace PlayerRole{
	extern CPlayerTask* makeItStop(const int num, const int flags = 0);
    extern CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const int flags = 0, const int sender = 0);
    extern CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags = 0, const int sender = 0);
    extern CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed = 0, const double maxRotSpeed = 0, const int flags = 0, const int sender = 0);
    extern CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const int flags = 0);
    extern CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags=0);
    extern CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed = 0, const double maxRotSpeed = 0, const int flags = 0);
   // extern CPlayerTask* makeItGoAndTurnKick(const int num, const CGeoPoint& target, const double dir,const int flags);

    extern CPlayerTask* makeItWBreak(const int num, const CGeoPoint&target, const double power,const int kick_flag, const double precision, const bool is_penalty);
    extern CPlayerTask* makeItStatic(const int num, const CGeoPoint&target, const double kpower,const double cpower);
    extern CPlayerTask* makeItWBack(const int num, const int guardNum, const int index, const int defendNUm, const int flag);
    extern CPlayerTask* makeItWDrag(const int num,  const CGeoPoint& target, const CGeoPoint& waitPos);
    extern CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const int flags = 0);
    extern CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags=0);
    extern CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed = 0, const double maxRotSpeed = 0, const int flags = 0);
    extern CPlayerTask* makeItWMarkEnemy(const int num, const int oppNum, const int priority, const int flag);
    //    extern CPlayerTask* makeItGetBallV1(const int num, const int flag, const CGeoPoint& pos, const CGeoPoint& waitPos, const double power, const double precision=-1);

    extern CPlayerTask* makeItRun(const int num, const double speedX, const double speedY, const double rotSpeed, int flags = 0);
    extern CPlayerTask* makeItOpenRun(const int num, const double speedX, const double speedY, const double rotSpeed, int flags = 0);
}

#endif // _TASK_FACTORY_V2_H_
