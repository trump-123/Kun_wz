#include "Factory.h"
#include <TaskMediator.h>

#include <misc_types.h>
#include "CommandFactory.h"
#include "GDebugEngine.h"

/************************************************************************/
/*								concrete skill							*/
/************************************************************************/
//1.atom skills(TODO)
#include "GotoPosition.h"
#include "SmartGotoPosition.h"

//2.basic skills(TODO)
#include "StopRobot.h"
#include "Speed.h"
#include "OpenSpeed.h"

// test
#include "Goalie.h"
#include "Tier1.h"
#include "Tier.h"
#include "SingleTier.h"
#include "defend.h"
#include "Touch.h"
#include "ChaseKick.h"
#include "naqiu.h"
#include "sao.h"
#include "guan.h"

#include "leader.h"
#include "assister.h"
#include "special.h"
#include "defend_05.h"
#include "defend_07_0.h"
#include "defend_07_1.h"
#include "leader_getball.h"
#include "PenaltyKick.h"
#include "PeanltyGoalie.h"
#include "FetchBall.h"
#include "Escape.h"
#include "Marking.h"
#include "MarkingFront.h"
#include "MarkingTouch.h"
#include "LeftHalf.h"
#include "RightHalf.h"
#include "MiddleHalf.h"
#include "wdrag.h"
#include "wback.h"
#include "wbreak.h"
#include "wmarking.h"
//#include "GetBallV1.h"
#include "Uniquekillskillm.h"
#include "Uniquekillskillr.h"
#include "Uniquekillskilll.h"
#include "Dnamicfindpos.h"
#include "goAndTurnKick.h"
#include "StaticGetBall.h"
#include "BIGpenaltyKick.h"
//#include "Defend.h"

/************************************************************************/
/*                      TaskFactoryV2									*/
/* 请注意:																*/
/* 上层（CStatedScriptPlay) 通过调用 CBasicPlay 中的 setTask() 设定任务	*/
/* 并将每个策略执行小车对应的任务存放在 TaskMediator 里面				*/
/* 执行（ActionModule）通过任务的 execute() 接口生成任务小车对应的指令	*/
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// an template used as a task pool for storing tasks for executing player
template < class CTaskType >
CPlayerTask* CTaskFactoryV2::MakeTask(const TaskT& task )
{
	static CTaskType taskPool[PARAM::Field::MAX_PLAYER];
	taskPool[task.executor].reset(task);
	return &taskPool[task.executor];
}

//////////////////////////////////////////////////////////////////////////
// current now debugged skill for game
CPlayerTask* CTaskFactoryV2::GotoPosition(const TaskT& task) {
	return MakeTask< CGotoPositionV2 >(task);
}

CPlayerTask* CTaskFactoryV2::SmartGotoPosition(const TaskT& task) {
    return MakeTask< CSmartGotoPositionV2 >(task);
}

CPlayerTask* CTaskFactoryV2::Tier1(const TaskT &task) {
    return MakeTask< CTier1 >(task);
}

CPlayerTask* CTaskFactoryV2::Tier(const TaskT &task) {
    return MakeTask< CTier >(task);
}

CPlayerTask* CTaskFactoryV2::SingleTier(const TaskT &task) {
    return MakeTask< CSingleTier >(task);
}

CPlayerTask* CTaskFactoryV2::LeftHalf(const TaskT &task) {
    return MakeTask< CLeftHalf >(task);
}

CPlayerTask* CTaskFactoryV2::RightHalf(const TaskT &task) {
    return MakeTask< CRightHalf >(task);
}

CPlayerTask* CTaskFactoryV2::MiddleHalf(const TaskT &task) {
    return MakeTask< CMiddleHalf >(task);
}

CPlayerTask* CTaskFactoryV2::guan(const TaskT &task) {
    return MakeTask< Cguan >(task);
}

CPlayerTask* CTaskFactoryV2::sao(const TaskT &task) {
    return MakeTask< Csao >(task);
}

CPlayerTask* CTaskFactoryV2::defend(const TaskT &task) {
    return MakeTask< Cdefend >(task);
}

CPlayerTask* CTaskFactoryV2::Goalie(const TaskT& task) {
    return MakeTask< CGoalie >(task);
}

CPlayerTask* CTaskFactoryV2::naqiu(const TaskT& task) {
    return MakeTask< Cnaqiu >(task);
}

CPlayerTask* CTaskFactoryV2::Uniqueskillm(const TaskT& task) {
    return MakeTask< CUniquekillskillm >(task);
}

CPlayerTask* CTaskFactoryV2::Uniqueskilll(const TaskT& task) {
    return MakeTask< CUniquekillskilll >(task);
}

CPlayerTask* CTaskFactoryV2::Uniqueskillr(const TaskT& task) {
    return MakeTask< CUniquekillskillr >(task);
}

CPlayerTask* CTaskFactoryV2::Touch(const TaskT& task) {
    return MakeTask< CTouch >(task);
}

CPlayerTask* CTaskFactoryV2::ChaseKick(const TaskT &task){
    return MakeTask< CChaseKick >(task);
}

CPlayerTask* CTaskFactoryV2::goAndTurnKick(const TaskT &task){
    return MakeTask< CgoAndTurnKick >(task);
}

CPlayerTask* CTaskFactoryV2::StaticGetBall(const TaskT &task){
    return MakeTask< CStaticGetBall >(task);
}

CPlayerTask* CTaskFactoryV2::StopRobot(const TaskT& task) {
	return MakeTask< CStopRobotV2 >(task);
}

CPlayerTask	* CTaskFactoryV2::Speed(const TaskT& task) {
	return MakeTask< CSpeed >(task);
}

CPlayerTask	* CTaskFactoryV2::OpenSpeed(const TaskT& task) {
	return MakeTask< COpenSpeed >(task);
}
//======================================================================//
CPlayerTask* CTaskFactoryV2::leader(const TaskT& task) {
    return MakeTask< CLeader >(task);
}
CPlayerTask* CTaskFactoryV2::assister(const TaskT& task) {
    return MakeTask< CAssister >(task);
}
CPlayerTask* CTaskFactoryV2::special(const TaskT& task) {
    return MakeTask< CSpecial >(task);
}
CPlayerTask* CTaskFactoryV2::defend_05(const TaskT& task) {
    return MakeTask< CDefend_05 >(task);
}
CPlayerTask* CTaskFactoryV2::defend_07_0(const TaskT& task) {
    return MakeTask< CDefend_05 >(task);
}
CPlayerTask* CTaskFactoryV2::defend_07_1(const TaskT& task) {
    return MakeTask< CDefend_05 >(task);
}
CPlayerTask* CTaskFactoryV2::leader_getball(const TaskT &task) {
    return MakeTask< CLeader_getball >(task);
}
CPlayerTask* CTaskFactoryV2::PenaltyKick(const TaskT& task){
    return MakeTask< CPenaltyKick >(task);
}
CPlayerTask* CTaskFactoryV2::PenaltyGoalie(const TaskT& task){
    return MakeTask< CPeanltyGoalie >(task);
}
CPlayerTask* CTaskFactoryV2::FetchBall(const TaskT& task){
    return MakeTask< CFetchBall >(task);
}
CPlayerTask* CTaskFactoryV2::Escape(const TaskT& task){
    return MakeTask< CEscape >(task);
}
CPlayerTask* CTaskFactoryV2::Marking(const TaskT& task){
    return MakeTask< CMarking >(task);
}
CPlayerTask* CTaskFactoryV2::WMarking(const TaskT& task){
    return MakeTask< CWmarking >(task);
}

//CPlayerTask* CTaskFactoryV2::GetBallV1(const TaskT& task) {
//    return MakeTask< CGetBallV1 >(task);
//}

CPlayerTask* CTaskFactoryV2::WDrag(const TaskT &task){
    return MakeTask<CWDrag>(task);
}

CPlayerTask* CTaskFactoryV2::WBreak(const TaskT &task){
    return MakeTask<CWBreak>(task);
}

CPlayerTask* CTaskFactoryV2::Dnamicfindpos(const TaskT& task){
    return MakeTask< CDnamicfindpos >(task);
}

CPlayerTask* CTaskFactoryV2::MarkingFront(const TaskT &task){
    return MakeTask< CMarkingFront >(task);
}

CPlayerTask* CTaskFactoryV2::MarkingTouch(const TaskT &task){
    return MakeTask< CMarkingTouch >(task);
}
CPlayerTask* CTaskFactoryV2::WBack(const TaskT &task){
    return MakeTask< CWback >(task);
}

CPlayerTask* CTaskFactoryV2::BIGpenaltyKick(const TaskT &task){
    return MakeTask< CBIGpenaltyKick >(task);
}
//CPlayerTask* CTaskFactoryV2::Defend(const TaskT& task){
//    return MakeTask< CDefend >(task);
//}
//=====================================================================//
//////////////////////////////////////////////////////////////////////////
// define the namespace used to provide interface for task calling
namespace PlayerRole {
	CPlayerTask* makeItStop(const int num, const int flags)
	{
		static TaskT playerTask;
		playerTask.executor = num;
		playerTask.player.flag = flags;
		return TaskFactoryV2::Instance()->StopRobot(playerTask);
	}
	CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const int flags, const int sender)
	{
		static TaskT playerTask;
		playerTask.executor = num;
		playerTask.player.pos = target;
        playerTask.player.vel = CVector(0.0,0.0);
		playerTask.player.rotvel = 0;
		playerTask.player.angle = dir;
		playerTask.player.flag = flags;
		playerTask.ball.Sender = sender;
		return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
	}
	CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags, const int sender)
	{
		static TaskT playerTask;
		playerTask.executor = num;
		playerTask.player.pos = target;
		playerTask.player.angle = dir;
		playerTask.player.vel = vel;
		playerTask.player.rotvel = rotvel;
		playerTask.player.flag = flags;
		playerTask.ball.Sender = sender;
		return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
	}

    CPlayerTask* makeItGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed, const double maxRotSpeed, const int flags, const int sender) {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.angle = dir;
        playerTask.player.vel = vel;
        playerTask.player.rotvel = rotVel;
        playerTask.player.max_acceleration = maxAcc;
        playerTask.player.max_rot_acceleration = maxRotAcc;
        playerTask.player.max_speed = maxSpeed;
        playerTask.player.max_rot_speed = maxRotSpeed;
        playerTask.player.flag = flags;
        playerTask.ball.Sender = sender;
        return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
    }


	CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const int flags)
	{
		static TaskT playerTask;
		playerTask.executor = num;
		playerTask.player.pos = target;
		playerTask.player.vel = CVector(0.0,0.0);
		playerTask.player.rotvel = 0;
		playerTask.player.angle = dir;
		playerTask.player.flag = flags;
		return TaskFactoryV2::Instance()->GotoPosition(playerTask);
	}


	CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags)
	{
		static TaskT playerTask;
		playerTask.executor = num;
		playerTask.player.pos = target;
		playerTask.player.angle = dir;
		playerTask.player.vel = vel;
		playerTask.player.rotvel = rotvel;
		playerTask.player.flag = flags;
		return TaskFactoryV2::Instance()->GotoPosition(playerTask);
	}

    CPlayerTask* makeItSimpleGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed, const double maxRotSpeed, const int flags) {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.angle = dir;
        playerTask.player.vel = vel;
        playerTask.player.rotvel = rotVel;
        playerTask.player.max_acceleration = maxAcc;
        playerTask.player.max_rot_acceleration = maxRotAcc;
        playerTask.player.max_speed = maxSpeed;
        playerTask.player.max_rot_speed = maxRotSpeed;
        playerTask.player.flag = flags;
        return TaskFactoryV2::Instance()->GotoPosition(playerTask);
    }

    CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const int flags)
    {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.vel = CVector(0.0,0.0);
        playerTask.player.rotvel = 0;
        playerTask.player.angle = dir;
        playerTask.player.flag = flags;
        return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
    }

    CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotvel, const int flags)
    {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.angle = dir;
        playerTask.player.vel = vel;
        playerTask.player.rotvel = rotvel;
        playerTask.player.flag = flags;
        return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
    }

    CPlayerTask* makeItWDrag(const int num,  const CGeoPoint& target, const CGeoPoint& waitPos) {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.kickpower = waitPos.x();
        playerTask.player.chipkickpower = waitPos.y();
        return TaskFactoryV2::Instance()->WDrag(playerTask);
    }

    CPlayerTask* makeItWBack(const int num, const int guardNum, const int index, const int defendNUm, const int flag)
    {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.flag = flag;
        playerTask.player.rotdir = index;
        playerTask.player.kick_flag = guardNum;
        playerTask.ball.Sender = defendNUm;
        return TaskFactoryV2::Instance()->WBack(playerTask);
    }

    CPlayerTask* makeItWMarkEnemy(const int num, const int oppNum, const int priority, const int flag)
    {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.ball.Sender = oppNum;
        playerTask.ball.receiver = priority;
        playerTask.player.flag = flag;
        return TaskFactoryV2::Instance()->WMarking(playerTask);
    }


//    CPlayerTask* makeItGetBallV1(const int num, const int kick_flag, const CGeoPoint& pos, const CGeoPoint& waitPos, const double power, const double precision)
//    {
//        static TaskT playerTask;
//        playerTask.executor = num;
//        playerTask.player.kick_flag = kick_flag;
//        //playerTask.player.angle = targetdir;
//        playerTask.player.pos = pos;
//        playerTask.player.kickpower = waitPos.x();
//        playerTask.player.chipkickpower = waitPos.y();
//        playerTask.player.rotdir = power;
//        playerTask.player.kickprecision = precision;
//        return TaskFactoryV2::Instance()->GetBallV1(playerTask);
//    }

//    CPlayerTask* makeItGoAndTurnKick(const int num, const CGeoPoint& target, const double dir,const int flags) {
//        static TaskT playerTask;
//        playerTask.executor = num;
//        playerTask.player.pos = target;
//        playerTask.player.angle = dir;
//        //playerTask.player.vel = vel;
//        //playerTask.player.rotvel = rotVel;
//       // playerTask.player.max_acceleration = maxAcc;
//        //playerTask.player.max_rot_acceleration = maxRotAcc;
//        //playerTask.player.max_speed = maxSpeed;
//        //playerTask.player.max_rot_speed = maxRotSpeed;
//        playerTask.player.flag = flags;
//        //playerTask.ball.Sender = sender;
//        return TaskFactoryV2::Instance()->goAndTurnKick(playerTask);
//    }

    CPlayerTask* makeItWBreak(const int num, const CGeoPoint&target, const double power,const int kick_flag, const double precision, const bool is_penalty){
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.kickpower  = power;
        playerTask.player.kick_flag  = kick_flag;
        playerTask.player.kickprecision = precision;
        playerTask.player.ispass = is_penalty;
        return TaskFactoryV2::Instance()->WBreak(playerTask);
    }

    CPlayerTask* makeItStatic(const int num, const CGeoPoint&target, const double kpower,const double cpower){
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.kickpower  = kpower;
        playerTask.player.chipkickpower = cpower;
        return TaskFactoryV2::Instance()->StaticGetBall(playerTask);
    }

    CPlayerTask* makeItSmartGoto(const int num, const CGeoPoint& target, const double dir, const CVector& vel, const double rotVel, const double maxAcc, const double maxRotAcc, const double maxSpeed, const double maxRotSpeed, const int flags) {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.pos = target;
        playerTask.player.angle = dir;
        playerTask.player.vel = vel;
        playerTask.player.rotvel = rotVel;
        playerTask.player.max_acceleration = maxAcc;
        playerTask.player.max_rot_acceleration = maxRotAcc;
        playerTask.player.max_speed = maxSpeed;
        playerTask.player.max_rot_speed = maxRotSpeed;
        playerTask.player.flag = flags;
        return TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
    }

	CPlayerTask* makeItRun(const int num, const double speedX, const double speedY, const double rotSpeed, int flags)
	{
		static TaskT playerTask;
		playerTask.executor = num;
		playerTask.player.speed_x = speedX;
		playerTask.player.speed_y = speedY;
		playerTask.player.rotate_speed = rotSpeed;
		playerTask.player.flag = flags;
		return TaskFactoryV2::Instance()->Speed(playerTask);
	}
    CPlayerTask* makeItOpenRun(const int num, const double speedX, const double speedY, const double rotSpeed, int flags)
    {
        static TaskT playerTask;
        playerTask.executor = num;
        playerTask.player.speed_x = speedX;
        playerTask.player.speed_y = speedY;
        playerTask.player.rotate_speed = rotSpeed;
        playerTask.player.flag = flags;
        return TaskFactoryV2::Instance()->OpenSpeed(playerTask);
    }

}
