#include "leader_getball.h"
#include "skill/Factory.h"
#include "TaskMediator.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "parammanager.h"
#include "KickStatus.h"
#include "game_state.h"
#include "string.h"
#include "RefereeBoxIf.h"
namespace  {
   bool DEBUG_SWITCH;
   const CGeoPoint OUR_GOAL(-PARAM::Field::PITCH_LENGTH/2,0);
   const CGeoPoint LEFT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2 - 2);
   const CGeoPoint RIGHT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2 + 2);
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
       if(pos.x() > MIN_X + padding && pos.x() < MAX_X - padding + PARAM::Field::MAX_PLAYER_SIZE/2)
       {
          if(pos.y() > -175 && pos.y() < 175)
          {
                 return true;
          }
           else if(pow((pos.x() + PARAM::Field::PITCH_LENGTH/2),2)+pow((pos.y() - PARAM::Field::GOAL_WIDTH/2),2) < 900*900||pow((pos.x() + PARAM::Field::PITCH_LENGTH/2),2)+pow((pos.y() + PARAM::Field::GOAL_WIDTH/2),2) < 900*900)
           return true;
           else
           return false;
       }
       return false;
   }
}

CLeader_getball::CLeader_getball():_state(CLeader_getball::NOTHING){
     ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH, "DEBUG/Leaader", true);
}
void CLeader_getball::plan(const CVisionModule* pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1)
    {_state = CLeader_getball::NOTHING;}
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);

    auto taskFlag = task().player.flag;
    auto taskmaxspeed = task().player.max_rot_speed;
    auto ball = pVision->ball().Pos();
    CGeoEllipse stand_ellipse(OUR_GOAL,PARAM::Field::PENALTY_AREA_DEPTH + PARAM::Field::MAX_PLAYER_SIZE,PARAM::Field::PENALTY_AREA_WIDTH/2+PARAM::Field::MAX_PLAYER_SIZE/2);
    CGeoLineEllipseIntersection stand_intersection(CGeoLine(ball,get_defence_direction(ball)),stand_ellipse);
    CGeoPoint stand_pos,stand_pos1;

    if(stand_intersection.intersectant()){
        if(ball.dist2(stand_intersection.point1())<ball.dist2(stand_intersection.point2()))
            stand_pos = stand_intersection.point1();
        else
            stand_pos = stand_intersection.point2();
        if(stand_pos.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
            stand_pos.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
    }else{
        stand_pos=CGeoPoint(-PARAM::Field::PITCH_LENGTH+PARAM::Field::PENALTY_AREA_DEPTH+PARAM::Vehicle::V2::PLAYER_SIZE+10,0);
    }
//cout<<"0"<<endl;

    bool inthepenalty = in_our_penalty(self.Pos(),-16*10);
    double stand_dir = (ball - self.Pos()).dir();

    static double po5[6];
    int i=0;
    static int num1[6];
    int Validplayernum = 0;
    for(int num=0;num<12;num++)
    { //获取敌方有效车号
        const PlayerVisionT& pos = pVision->theirPlayer(num);

        if(pos.Valid())
        {
            num1[i] =num;
            po5[i] =  (ball - pos.Pos()).mod();
           // cout<<i<<"     "<<po5[i]<<endl;
            i++;
            Validplayernum = i;
        }
    }
    static  int n = 0;
    double q=po5[0];
     for(int i=0;i<Validplayernum;i++)
     {
         if(po5[i]<= q)
         {
              q = po5[i];
              n = num1[i];
         }
     }
    cout<<"leader_getball:"<<n<<endl;
    double speedx = task().player.speed_x;
    double speedy = task().player.speed_y;
    auto pi = PARAM::Math::PI;
    CGeoPoint we = pVision->rawBall().Pos();
    double todir = (we - self.Pos()).dir();
    const PlayerVisionT& pos1= pVision->theirPlayer(n);
    CGeoPoint p = pos1.Pos();
    //CGeoPoint defend(p + Utils::Polar2Vector(100,Utils::Normalize(pos1.Dir())));

    //CGeoPoint defend(ball + Utils::Polar2Vector(100,Utils::Normalize(pos1.Dir())));
    auto angle1 = (self.Pos()- ball).dir();//朝向持球人的角度

    CGeoPoint defend(ball + Utils::Polar2Vector(90,angle1));

    auto dir = pos1.Dir();
    auto di = Utils::Normalize((ball - self.Pos()).dir()-pi/2.0);
    auto d = Utils::Normalize(dir-pi/2.0);//和敌方机器人嘴方向垂直线段角度
    auto ang= Utils::Normalize((ball-self.Pos()).dir());//朝向球
    CGeoPoint pos2 = stand_pos + Utils::Polar2Vector(PARAM::Field::MAX_PLAYER_SIZE/2+10,di);//改变处

    CGeoEllipse stand_ellipse1(ball,PARAM::Field::CENTER_CIRCLE_R + PARAM::Field::MAX_PLAYER_SIZE,PARAM::Field::CENTER_CIRCLE_R+PARAM::Field::MAX_PLAYER_SIZE);//创造一个圆
    CGeoLineEllipseIntersection stand_intersection1(CGeoLine(p,dir),stand_ellipse1);

    if(stand_intersection1.intersectant()){
        if(p.dist2(stand_intersection1.point1())<p.dist2(stand_intersection1.point2()))
            stand_pos1 = stand_intersection1.point1();
        else
            stand_pos1 = stand_intersection1.point2();
        if(stand_pos1.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
            stand_pos1.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
    }else{
        stand_pos1 = CGeoPoint(-PARAM::Field::PITCH_LENGTH+PARAM::Field::PENALTY_AREA_DEPTH+PARAM::Vehicle::V2::PLAYER_SIZE+10,0);
    }
    CGeoPoint pos0 = stand_pos1 + Utils::Polar2Vector(PARAM::Field::MAX_PLAYER_SIZE/2.0+10,d);//改变处
    _state = STAND;
    TaskT newTask(task());

    double dir1 = Utils::Normalize((LEFT_GOAL_POST - pos1.Pos()).dir());
    double dir2 = Utils::Normalize((RIGHT_GOAL_POST - pos1.Pos()).dir());
    double stand_dir1 = (pos1.Pos() - self.Pos()).dir();
    double distance = fabs((p - self.Pos()).mod());
    double ball_vel_dir =pVision-> ball().Vel().dir();
    CGeoPoint speed1(0,0);
    CGeoSegment ball_line(ball,ball + Utils::Polar2Vector(99999,ball_vel_dir));
    CGeoLineLineIntersection danger_intersection(ball_line,GOAL_LINE);
    bool danger_to_our_goal = danger_intersection.Intersectant() && ball_line.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) && GOAL_LINE.IsPointOnLineOnSegment(danger_intersection.IntersectPoint()) && (ball_vel_dir - (danger_intersection.IntersectPoint() - ball).dir()) < PARAM::Math::PI/18;

//    if(danger_to_our_goal)
//    {
        if(distance < 500)
        {
            speed1 = CGeoPoint(0,0) + Utils::Polar2Vector(6500,(pos2-self.Pos()).dir());
            speedx = speed1.x();
            speedy = speed1.y();
        }
        else if(distance < 1000)
        {
            speed1 = CGeoPoint(0,0) + Utils::Polar2Vector(5000,(pos2-self.Pos()).dir());
            speedx = speed1.x();
            speedy = speed1.y();
        }
        else if(distance < 2000)
        {
            speed1 = CGeoPoint(0,0) + Utils::Polar2Vector(4500,(pos2-self.Pos()).dir());
            speedx = speed1.x();
            speedy = speed1.y();
        }
        else if(distance < 3000)
        {
            speed1 = CGeoPoint(0,0) + Utils::Polar2Vector(4200,(pos2-self.Pos()).dir());
            speedx = speed1.x();
            speedy = speed1.y();
        }
        else
        {
            speed1 = CGeoPoint(0,0) + Utils::Polar2Vector(4000,(pos2-self.Pos()).dir());
            speedx = speed1.x();
            speedy = speed1.y();
        }
/*
        newTask.player.angle = stand_dir1;
        newTask.player.pos = defend;
    }
    else
    {
        if(pos1.Y()>RIGHT_GOAL_POST.y()||pos1.Y()<LEFT_GOAL_POST.y())
        {
            if(dir>dir2 && dir<dir1)
            {
               // cout<<"dir="<<x<<endl;
                newTask.player.pos = pos0;
                newTask.player.angle = stand_dir1;
            }
            else{
                newTask.player.pos = pos2;
                if((ball - self.Pos()).mod()<700)
                {
                    newTask.player.angle = stand_dir1;
                }
                else
                {
                 newTask.player.angle = ang;
                }
            }
        }
        else
        {
            if(dir>dir2 || dir<dir1)
            {
               // cout<<"dir="<<theirdir<<endl;
                newTask.player.pos = pos0;
                newTask.player.angle = stand_dir1;
            }
            else{
                newTask.player.pos = pos2;
                if((ball - self.Pos()).mod()<700)
                {
                    newTask.player.angle = stand_dir1;
                }
                else
                {
                newTask.player.angle = ang;
                }
            }
        }
    }*/
    //if(((p.x() - ball.x()) < 200)||((p.y() - ball.y()) < 200))
    //{
        //KickStatus::Instance()->setKick(vecNumber,8000);//踢球
        newTask.player.pos = defend;
        newTask.player.angle = Utils::Normalize(angle1 + PARAM::Math::PI);
        DribbleStatus::Instance()->setDribbleCommand(vecNumber, 3);
        //taskFlag |= PlayerStatus::NOT_AVOID_PENALTY;
        newTask.player.flag = taskFlag;
        newTask.player.max_rot_speed = taskmaxspeed;
        newTask.player.speed_x = speedx;
        newTask.player.speed_y = speedy;
    //}
//newTask.player.pos = CGeoPoint(100,0);






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

    setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
    //setSubTask(PlayerRole::makeItSmartGoto(vecNumber,CGeoPoint(100,100),0,0));
    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
