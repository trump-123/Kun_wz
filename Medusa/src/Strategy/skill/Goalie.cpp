#include "Goalie.h"
#include "skill/Factory.h"
#include "TaskMediator.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "parammanager.h"
#include "KickStatus.h"
#include "WorldModel.h"
#include "messidecition.h"
#include "math.h"
#include "RobotSensor.h"
#include "ballmodel.h"
#include "SkillUtils.h"
#include "messidecition.h"
using namespace std;
namespace{
    bool DEBUG_SWITCH;
    double FRICTION;
    bool IS_SIMULATION;
    int maxTime = 1024;
    int fraredOn = 0;
    int fraredOff = 0;
    const CGeoPoint OUR_GOAL(-PARAM::Field::PITCH_LENGTH/2,0);
    const CGeoPoint THEIR_GOAL(PARAM::Field::PITCH_LENGTH/2,200);
    const CGeoPoint LEFT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2 - 90);
    const CGeoPoint RIGHT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2 + 90);
    const CGeoPoint LEFT_GOAL_POST1(-PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2);
    const CGeoPoint RIGHT_GOAL_POST1(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2);
    const CGeoPoint LEFT_PENALTY_POST(-PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::PENALTY_AREA_WIDTH/2 - 2);
    const CGeoPoint RIGHT_PENALTY_POST(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PENALTY_AREA_WIDTH/2 + 2);
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
    bool opp_tend_our_goal(double dir,const CGeoPoint& pos)
    {
            if(dir>Utils::Normalize((RIGHT_GOAL_POST - pos).dir())&&dir<Utils::Normalize((LEFT_GOAL_POST - pos).dir()))
              return false;
            else
              return true;
    }
    auto player = PARAM::Field::NOW_PLAYER;
    auto maxplayer = PARAM::Field::MAX_PLAYER;
    int flag = 0;
}
CGoalie::CGoalie()
    :_lastCycle(0)
    ,_flytimediff(false)
    ,interPos(CGeoPoint(0,0))
    ,_state(CGoalie::NOTHING){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH,"Debug/Goalie",false);
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION, "Alert/IsSimulation", false);

    if (IS_SIMULATION)
        ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Sim",800);
    else
        ZSS::ZParamManager::instance()->loadParam(FRICTION,"AlertParam/Friction4Real",1520);

}
void CGoalie::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
        _state = CGoalie::NOTHING;
        fraredOn = 0;
        fraredOff = 0;
        _flytimediff = false;
        interPos = CGeoPoint(0,0);
    }
    const int vecNumber = task().executor;
    const CGeoPoint target = task().player.pos;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);
    const auto mouse = self.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,self.Dir());
    auto ball = pVision->ball().Valid() ? pVision->ball().Pos() : MessiDecision::Instance()->getBallPos();//pVision->ball().Pos();
    auto ball_vel = pVision->ball().Vel().mod();//球移动距离
    auto ball_vel_dir = pVision->ball().Vel().dir();//球运动角度
    CGeoEllipse stand_ellipse(OUR_GOAL,PARAM::Field::PENALTY_AREA_DEPTH/2-200,PARAM::Field::GOAL_WIDTH/2+90);
    CGeoLineEllipseIntersection stand_intersection(CGeoLine(ball,get_defence_direction(ball)),stand_ellipse);//建立一个椭圆防守区
    CGeoPoint stand_pos;//防球
    CGeoPoint stand_pos1;
    CGeoPoint stand_pos2;
    CGeoPoint stand_pos3;
    CGeoPoint Targetpos,real_Pos;
    if(stand_intersection.intersectant()){//求最近交点
        if(ball.dist2(stand_intersection.point1())<ball.dist2(stand_intersection.point2()))
            stand_pos = stand_intersection.point1();
        else
            stand_pos = stand_intersection.point2();
        if(stand_pos.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
            stand_pos.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
    }else{
        stand_pos = CGeoPoint(-PARAM::Field::PITCH_LENGTH/2+PARAM::Vehicle::V2::PLAYER_SIZE,0);
    }

      //cout<<time<<endl;

    double stand_dir = (ball-self.Pos()).dir();//防守角度
    bool need_clear = Utils::InOurPenaltyArea(ball,0)&&(ball_vel<50*10||fraredOn>1);//清球
    if(need_clear)
    {
        flag = 1;
    }
    else
    {
        flag = 0;
    }

    CGeoPoint clear_pos = ball + Utils::Polar2Vector(90,(OUR_GOAL - ball).dir());//清球点
    CGeoPoint push_pos  = ball + Utils::Polar2Vector(150,(OUR_GOAL - ball).dir());//清球点
    interPos = ZSkillUtils::instance()->getOurInterPoint(vecNumber);

    // SAVE
    auto oppose_ball_dir = Utils::Normalize(ball_vel_dir+PARAM::Math::PI);
    CGeoSegment ball_line(ball,ball + Utils::Polar2Vector(99999,ball_vel_dir));
    double d = PARAM::Math::PI;

    CGeoLineLineIntersection danger_intersection1(ball_line, CGeoLine(self.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,self.Dir()),ball_vel_dir + d/2));//求机器人最近防守点
    CGeoLineLineIntersection danger_intersection2(ball_line, CGeoLine(self.Pos(),ball_vel_dir - d/2));//同上
    CGeoLineLineIntersection danger_intersection(ball_line,GOAL_LINE);
    bool danger_to_our_goal = danger_intersection.Intersectant() && ball_line.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) && GOAL_LINE.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) && (ball_vel_dir - (danger_intersection.IntersectPoint() - ball).dir()) < PARAM::Math::PI/18;

    if(RobotSensor::Instance()->IsKickerOn(vecNumber) > 0){
        MessiDecision::Instance()->setisflyTime(true);
        if(_flytimediff){
            _flytimediff = false;
            MessiDecision::Instance()->setfirstChipDir(self.Dir());
            MessiDecision::Instance()->setfirstChipPos(ball);
        }
    } else {
        _flytimediff = true;
    }


    auto pi=PARAM::Math::PI;

    CVector me2ball = ball - self.Pos();

    double inVel;
    if(pVision->ball().Vel().mod2() > FRICTION * me2ball.mod())
        inVel = sqrt(pVision->ball().Vel().mod2() - FRICTION * me2ball.mod());
    else
        inVel = pVision->ball().Vel().mod();

    double power = 6000;//BallModel::instance()->chipPassVel(pVision,target);
    double beta = Utils::Normalize(pVision->ball().Vel().dir() + PARAM::Math::PI - (target - self.RawPos()).dir());
    double alpha = atan2(power * sin(beta) / 0.42 + 1e-5, inVel + power * cos(beta) / 0.42 + 1e-5);



    bool frared = RobotSensor::Instance()->IsInfraredOn(vecNumber);
    if(frared){
        fraredOn = fraredOn > maxTime ? maxTime : fraredOn + 1;
        fraredOff = 0;
    } else {
        fraredOff = fraredOff > maxTime ? maxTime : fraredOff + 1;
        fraredOn = 0;
    }
    //GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,-1000),QString("%1").arg(fraredOff).toLatin1());

    TaskT newTask(task());
//    cout<<time<<endl;
//    cout<<flag<<endl;
    if(danger_to_our_goal && flag == 0&&ball_vel>100){//紧急防球
        _state = SAVE;
        if(ball.dist(self.Pos()) < 50*10){
            DribbleStatus::Instance()->setDribbleCommand(vecNumber, 3);
        }

        if(fabs((target - self.RawPos()).dir() - (ball - self.RawPos()).dir()) < pi / 3){
            Targetpos = danger_intersection1.IntersectPoint() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,(target - self.RawPos()).dir() + pi);
            newTask.player.angle = (target - self.RawPos()).dir();
        }else{
            Targetpos = danger_intersection2.IntersectPoint();
            newTask.player.angle = (ball - self.RawPos()).dir();
        }


        if(Utils::InOurPenaltyArea(interPos,0) && interPos.x() >= -PARAM::Field::PITCH_LENGTH/2){
            real_Pos = interPos;
        }
        else if(Utils::IsInField(interPos,0)){
            real_Pos = Targetpos;
        }else{
            if((Targetpos - self.Pos()).mod() > 300&&(Targetpos - self.Pos()).mod() < 500)
            {
                real_Pos = self.Pos() + Utils::Polar2Vector(300,(Targetpos - self.Pos()).dir());
            }
            else if((Targetpos - self.Pos()).mod() >= 500){
                real_Pos = self.Pos() + Utils::Polar2Vector(700,(Targetpos - self.Pos()).dir());
            }
            else
            {
                real_Pos = Targetpos;
            }
        }

        newTask.player.pos = real_Pos;
        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
    }
    else if(need_clear){//清球
        _state = CLEAR;
//        CWorldModel p;
//        int time = p.InfraredOnCount(vecNumber);

        if((abs((ball - self.Pos()).dir() - self.Dir()) < pi/8 && abs(self.Dir() - (target - self.RawPos()).dir())<0.15&&fraredOn>1)||fraredOn>450)
        {
                newTask.player.angle = (target - self.Pos()).dir() /*- alpha + beta*/;
                newTask.player.pos = ball + Utils::Polar2Vector(90,(target - self.Pos()).dir() + pi);
                DribbleStatus::Instance()->setDribbleOff(vecNumber);
                KickStatus::Instance()->setChipKick(vecNumber,power);
        }
        else
        {
            if(fraredOn > 30&&((ball-self.Pos()).mod() < 130&&fabs(Utils::Normalize(Utils::Normalize((ball-self.Pos()).dir())-self.Dir())<pi/6)))
            {
                if((ball.x() + PARAM::Field::PITCH_LENGTH/2) < PARAM::Field::PENALTY_AREA_DEPTH / 2 * 1)
                {
                     DribbleStatus::Instance()->setDribbleCommand(vecNumber,3);
                     newTask.player.angle = (target - self.Pos()).dir();
                     newTask.player.pos = ball + Utils::Polar2Vector(105,(target - self.Pos()).dir() + pi);
                }
                else
                {
                    DribbleStatus::Instance()->setDribbleCommand(vecNumber, 3);
                    newTask.player.pos = push_pos;
                    newTask.player.angle = stand_dir;
                }
//                 DribbleStatus::Instance()->setDribbleCommand(vecNumber,3);
//                 newTask.player.angle = 0;
//                 newTask.player.pos = ball + Utils::Polar2Vector(90,pi);
            }
            else
            {
                //cout<<2<<endl;
                    DribbleStatus::Instance()->setDribbleCommand(vecNumber, 3);
                    newTask.player.pos   = clear_pos;
                    newTask.player.angle = stand_dir;
                    newTask.player.max_acceleration = 3000;
                    if((ball - OUR_GOAL).mod()<(self.Pos()-OUR_GOAL).mod())
                    {
                        newTask.player.flag |= PlayerStatus::DODGE_BALL;
                    }
            }
        }
        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
    }else{//基本防球
        _state = STAND;
        //const CGeoPoint Pos[6];
        int i=0;
        double dis = 99999;
        for(int num=0;num<PARAM::Field::MAX_PLAYER-1;num++)
        { //获取敌方有效车号
                const PlayerVisionT& pos= pVision->theirPlayer(num);
                if(pos.Valid())
                {
                   if(pos.Pos().dist(ball) < dis)
                   {
                        dis = pos.Pos().dist(ball);
                        i = num;
                   }
                }
        }

        const PlayerVisionT& pos1= pVision->theirPlayer(i);
        double x = Utils::Normalize(pos1.Dir());
        CGeoLineEllipseIntersection stand_intersection1(CGeoLine(pos1.Pos(),x),stand_ellipse);
            if(stand_intersection1.intersectant()){
                if(pos1.Pos().dist2(stand_intersection1.point1())<pos1.Pos().dist2(stand_intersection1.point2()))
                    stand_pos1 = stand_intersection1.point1();
                else
                    stand_pos1 = stand_intersection1.point2();
                if(stand_pos1.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
                    stand_pos1.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
            }else{
            stand_pos1 = CGeoPoint(-PARAM::Field::PITCH_LENGTH/2+PARAM::Vehicle::V2::PLAYER_SIZE,0);
            }
        double dir1 = Utils::Normalize((LEFT_GOAL_POST - pos1.Pos()).dir());
        double dir2 = Utils::Normalize((RIGHT_GOAL_POST - pos1.Pos()).dir());
        double stand_dir1 = (pos1.Pos() - self.Pos()).dir();
        //CGeoLine pass_line(ball,ball + Utils::Polar2Vector(99999,ball_vel_dir));

//        static int Num1=0;
//        for(int x = 0;x<6;x++)
//        {
//           double q = 0;
//           const PlayerVisionT& pos= pVision->theirPlayer(x);
//           CGeoLineLineIntersection distance(pass_line,CGeoLine(pos.Pos(),Utils::Normalize(ball_vel_dir+d/2)));
//           if(distance.Intersectant()){
//               q = (distance.IntersectPoint() - pos.Pos()).mod();
//           }
//           else{
//               q = (ball - pos.Pos()).mod();
//           }
//           if(fabs((pos.Pos() - ball).dir() - ball_vel_dir) < d/8&& q < 300)
//           {
//               Num1 = x;
//           }
//        }

//        const PlayerVisionT& resever= pVision->theirPlayer(Num1);
//        double stand_dir2 = (resever.Pos() - self.Pos()).dir();




//        CGeoLineEllipseIntersection stand_intersection2(CGeoLine(resever.Pos(),OUR_GOAL),stand_ellipse);
//        if(stand_intersection2.intersectant()){
//            if(resever.Pos().dist2(stand_intersection2.point1())<resever.Pos().dist2(stand_intersection2.point2()))
//                stand_pos2 = stand_intersection2.point1();
//            else
//                stand_pos2 = stand_intersection2.point2();
//            if(stand_pos2.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
//                stand_pos2.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
//        }else{
//        stand_pos2 = CGeoPoint(-PARAM::Field::PITCH_LENGTH/2+PARAM::Vehicle::V2::PLAYER_SIZE,0);
//        }



//        CGeoLineEllipseIntersection stand_intersection3(CGeoLine(resever.Pos(),resever.Dir()),stand_ellipse);
//        if(stand_intersection3.intersectant()){
//            if(resever.Pos().dist2(stand_intersection3.point1())<resever.Pos().dist2(stand_intersection3.point2()))
//                stand_pos3 = stand_intersection3.point1();
//            else
//                stand_pos3 = stand_intersection3.point2();
//            if(stand_pos3.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
//                stand_pos3.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
//        }else{
//        stand_pos3 = CGeoPoint(-PARAM::Field::PITCH_LENGTH/2+PARAM::Vehicle::V2::PLAYER_SIZE,0);
//        }



//        if((ball_vel > 1000||(ball - pos1.Pos()).mod()>300) && opp_tend_our_goal(x,pos1.Pos()) && (ball - pos1.Pos()).mod()<6000)
//        {
//            //cout<<x<<endl;

//            if(opp_tend_our_goal(resever.Dir(),resever.Pos()))
//            {
//                Targetpos = stand_pos;
//                newTask.player.angle = stand_dir;
//            }
//            else
//            {
//                Targetpos = stand_pos3;
//                newTask.player.angle = stand_dir2;
//            }
//        }
//        else
//        {
             //
//            if(ball.y()<LEFT_GOAL_POST.y()||ball.y()>RIGHT_GOAL_POST.y())
//            {
//                if(x>dir2 && x<dir1)
//                {
//                   // cout<<"dir="<<x<<endl;
//                    Targetpos = stand_pos1;
//                    newTask.player.angle = stand_dir1;
//                }
//                else{
//                    Targetpos = stand_pos;
//                    newTask.player.angle = stand_dir;
//                }
//            }
//            else
//            {
//                if((x > dir2 && x <= d)|| (x < dir1 && x >= -d))
//                {
//                   // cout<<dir2<<endl;
//                   // cout<<"dir="<<x<<endl;
//                    Targetpos = stand_pos1;
//                    newTask.player.angle = stand_dir1;
//                }
//                else{
                    Targetpos = stand_pos;
                    newTask.player.angle = stand_dir;
               // }
            //}
        //}
        if((Targetpos - self.Pos()).mod() < 300 && (Targetpos - self.Pos()).mod() > 100)
        {
            real_Pos = self.Pos() + Utils::Polar2Vector(300,(Targetpos - self.Pos()).dir());
        }
        else
        {
            real_Pos = Targetpos;
        }
        newTask.player.pos = real_Pos;
        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
    }

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
            GDebugEngine::Instance()->gui_debug_x(clear_pos,0);
            GDebugEngine::Instance()->gui_debug_msg(clear_pos,"cpos",0);
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
