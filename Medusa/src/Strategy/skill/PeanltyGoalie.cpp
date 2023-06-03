#include "PeanltyGoalie.h"
#include "skill/Factory.h"
#include "TaskMediator.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "parammanager.h"
#include "RobotSensor.h""
#include "KickStatus.h"
#include "WorldModel.h"
#include "math.h"
using namespace std;
namespace{
    bool DEBUG_SWITCH;
    int maxTime = 1024;
    int fraredOn = 0;
    int fraredOff = 0;
    const CGeoPoint OUR_GOAL(-PARAM::Field::PITCH_LENGTH/2,0);
    const CGeoPoint THEIR_GOAL(PARAM::Field::PITCH_LENGTH/2,200);
    const CGeoPoint LEFT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2);
    const CGeoPoint RIGHT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2);
    const CGeoSegment GOAL_LINE(LEFT_GOAL_POST,RIGHT_GOAL_POST);
    const static int MIN_X = -PARAM::Field::PITCH_LENGTH/2;
    const static int MAX_X = -PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH;
    const static int MIN_Y = -PARAM::Field::PENALTY_AREA_WIDTH/2;
    const static int MAX_Y = PARAM::Field::PENALTY_AREA_WIDTH/2;

    double get_defence_direction(const CGeoPoint & pos){
        double leftPostToBallDir = (pos - LEFT_GOAL_POST).dir();
        double rightPostToBallDir = (pos - RIGHT_GOAL_POST).dir();
        if(DEBUG_SWITCH){
            GDebugEngine::Instance()->gui_debug_line(pos,LEFT_GOAL_POST,6);
            GDebugEngine::Instance()->gui_debug_line(pos,RIGHT_GOAL_POST,6);
        }
        return Utils::Normalize((leftPostToBallDir + rightPostToBallDir) / 2 + PARAM::Math::PI);
    }
    bool in_our_penalty(const CGeoPoint& pos,double padding){
        if(pos.x() > MIN_X + padding && pos.x() < MAX_X - padding && pos.y() > MIN_Y+padding &&pos.y()< MAX_Y -padding)
            return true;
        else
            return false;
    }

    int findtheirkick(const CVisionModule* pVision)
    {
        CGeoPoint ball = pVision->ball().Pos();
        for(int i = 0;i<PARAM::Field::MAX_PLAYER;i++)
        {
            const PlayerVisionT & their = pVision->theirPlayer(i);
            CGeoPoint theirpos = their.Pos();
            if((ball - theirpos).mod() < 500)
            {
                return i;
            }
        }
        return -1;
    }
    auto player = PARAM::Field::NOW_PLAYER;
    auto maxplayer = PARAM::Field::MAX_PLAYER;
}
CPeanltyGoalie::CPeanltyGoalie():_state(CPeanltyGoalie::NOTHING){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH,"Debug/PeanltyGoalie",false);
}
void CPeanltyGoalie::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        _state = CPeanltyGoalie::NOTHING;
        _judege = false;
    }
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);
    auto pi=PARAM::Math::PI;
    auto ball = pVision->ball().Pos();
    auto ball_vel_mod = pVision->ball().Vel().mod();
    auto ball_vel = pVision->ball().Vel().mod();//球移动距离
    auto ball_vel_dir = pVision->ball().Vel().dir();//球运动角度
    int taskFlag = PlayerStatus::NOTHING;
    float taskacc = 10000;

    // SAVE
    auto oppose_ball_dir = Utils::Normalize(ball_vel_dir+PARAM::Math::PI);
    CGeoSegment ball_line(ball,ball + Utils::Polar2Vector(99999,ball_vel_dir));
    double d = PARAM::Math::PI;
    double stand_dir = (ball-self.Pos()).dir();//防守角度
    bool need_clear = Utils::InOurPenaltyArea(ball,0)&&(ball_vel<50*10||fraredOn>1) && !((ball.x() < -PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH + 300)&&(ball.x() > -PARAM::Field::PITCH_LENGTH/2 +PARAM::Field::PENALTY_AREA_DEPTH - 150) && (abs(ball.y()) < PARAM::Field::PENALTY_AREA_WIDTH / 2 ));//清球
    CGeoPoint clear_pos = ball + Utils::Polar2Vector(90,(OUR_GOAL - ball).dir());//清球点
    CGeoPoint push_pos  = ball + Utils::Polar2Vector(150,(OUR_GOAL - ball).dir());//清球点

    CGeoLineLineIntersection danger_intersection1(ball_line, CGeoLine(self.Pos(),ball_vel_dir + d/2));//求机器人最近防守点
    CGeoLineLineIntersection danger_intersection2(ball_line, CGeoLine(self.Pos(),ball_vel_dir - d/2));//同上
    CGeoLineLineIntersection danger_intersection(ball_line,GOAL_LINE);
    bool danger_to_our_goal = danger_intersection.Intersectant() && ball_line.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) && GOAL_LINE.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) && (ball_vel_dir - (danger_intersection.IntersectPoint() - ball).dir()) < PARAM::Math::PI/18;

    CGeoPoint stand_pos;
    CGeoPoint TargetPos,RealTargetPos;
    double TargetDir;
    int Flag;


    bool frared = RobotSensor::Instance()->IsInfraredOn(vecNumber);
    if(frared){
        fraredOn = fraredOn > maxTime ? maxTime : fraredOn + 1;
        fraredOff = 0;
    } else {
        fraredOff = fraredOff > maxTime ? maxTime : fraredOff + 1;
        fraredOn = 0;
    }

    TaskT newTask(task());


    if(danger_to_our_goal && ball_vel_mod > 1000)
    {
         //cout<<5<<endl;
        stand_pos = danger_intersection.IntersectPoint();
        if(stand_pos.y() > RIGHT_GOAL_POST.y() - PARAM::Vehicle::V2::PLAYER_SIZE )
        {
            stand_pos.setY(RIGHT_GOAL_POST.y() - PARAM::Vehicle::V2::PLAYER_SIZE );
        }
        else if(stand_pos.y() < LEFT_GOAL_POST.y() + PARAM::Vehicle::V2::PLAYER_SIZE )
        {
            stand_pos.setY(LEFT_GOAL_POST.y() + PARAM::Vehicle::V2::PLAYER_SIZE);
        }
        TargetPos = stand_pos;
        TargetDir = 0;
        Flag      = PlayerStatus::QUICKLY;
        KickStatus::Instance()->setChipKick(vecNumber,6000);
    }
    else if(need_clear){//清球


            if((abs((ball - self.Pos()).dir() - self.Dir()) < pi/8 && abs(self.Dir() - 0)<0.15&&fraredOn>1)||fraredOn>450)
            {
                        TargetDir = 0 ;//- alpha + beta
                        TargetPos = ball + Utils::Polar2Vector(90, pi);
                        DribbleStatus::Instance()->setDribbleOff(vecNumber);
                        KickStatus::Instance()->setChipKick(vecNumber,5000);
            }
            else
            {
                if(fraredOn > 30&&((ball-self.Pos()).mod() < 130&&fabs(Utils::Normalize(Utils::Normalize((ball-self.Pos()).dir())-self.Dir())<pi/6)))
                {
                    if((ball.x() + PARAM::Field::PITCH_LENGTH/2) < PARAM::Field::PENALTY_AREA_DEPTH / 2 * 1)
                    {
                         DribbleStatus::Instance()->setDribbleCommand(vecNumber,3);
                         TargetDir = 0;
                         TargetPos = ball + Utils::Polar2Vector(105, pi);
                    }
                    else
                    {
                        DribbleStatus::Instance()->setDribbleCommand(vecNumber, 3);
                        TargetPos = push_pos;
                        TargetDir = stand_dir;
                    }
                 DribbleStatus::Instance()->setDribbleCommand(vecNumber,3);
                 newTask.player.angle = 0;
                 newTask.player.pos = ball + Utils::Polar2Vector(90,pi);
                }
                else
                {
                //cout<<2<<endl;
                        DribbleStatus::Instance()->setDribbleCommand(vecNumber, 3);
                        TargetPos = clear_pos;
                        TargetDir     = stand_dir;
                        taskacc = 3000;
                        if((ball - OUR_GOAL).mod()<(self.Pos()-OUR_GOAL).mod())
                        {
                            taskFlag |= PlayerStatus::DODGE_BALL;
                        }
                }
            }
    }
    else
    {

            if(findtheirkick(pVision)!=-1 /*&& !need_clear*/) {
                int num = findtheirkick(pVision);
                const PlayerVisionT &their1 = pVision->theirPlayer(num);
                CGeoPoint theirpos1 = their1.Pos();
                double    theirdir  = their1.Dir();
                CGeoLine shootline(theirpos1,theirdir);
                CGeoLineLineIntersection judshoot(shootline,GOAL_LINE);
                CGeoPoint defindpos;
                if(judshoot.Intersectant()){//求最近交点
                     defindpos = judshoot.IntersectPoint();
                }else{
                     defindpos = CGeoPoint(-PARAM::Field::PITCH_LENGTH/2+PARAM::Vehicle::V2::PLAYER_SIZE,0);
                }

                if(defindpos.y() > RIGHT_GOAL_POST.y() - PARAM::Vehicle::V2::PLAYER_SIZE - 60)
                {
                    defindpos.setY(RIGHT_GOAL_POST.y() - PARAM::Vehicle::V2::PLAYER_SIZE - 60);
                }
                else if(defindpos.y() < LEFT_GOAL_POST.y() + PARAM::Vehicle::V2::PLAYER_SIZE + 60)
                {
                    defindpos.setY(LEFT_GOAL_POST.y() + PARAM::Vehicle::V2::PLAYER_SIZE + 60);
                }

                TargetPos = defindpos;
                TargetDir = (ball - self.Pos()).dir();
                Flag = PlayerStatus::QUICKLY;
            }
            else {
                    TargetPos = OUR_GOAL;
                    TargetDir = (ball - self.Pos()).dir();
                    Flag      = PlayerStatus::QUICKLY;
            }
     }
    if((self.Pos() - TargetPos).mod() < 500 && (TargetPos - self.Pos()).mod() > 300){
        RealTargetPos = self.Pos() + Utils::Polar2Vector(300,(TargetPos - self.Pos()).dir());
    }
    else if((self.Pos() - TargetPos).mod() >= 500){
        RealTargetPos = self.Pos() + Utils::Polar2Vector(1000,(TargetPos - self.Pos()).dir());
    }
    else
    {
        RealTargetPos = TargetPos;
    }


            newTask.player.pos   = RealTargetPos;
            newTask.player.angle = TargetDir;
            newTask.player.max_acceleration = taskacc;
            newTask.player.flag  = taskFlag|PlayerStatus::NOT_AVOID_PENALTY;
            setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
//    if(...)
//        setSubTask(...)
//    elseif(...)
//        setSubTask(...)

    if(DEBUG_SWITCH){
        switch(_state){
        case NOTHING:
            debug_state = "NOTHING";
            break;
        case STAND:
            debug_state = "NORMAL STAND";
            break;
        case CLEAR:
            debug_state = "CLEAR BALL";
            break;
        case SAVE:
            debug_state = "SAVE";
            break;
        default:
            debug_state = "ERROR STATE";
        }
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2),debug_state.c_str());
    }
    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
