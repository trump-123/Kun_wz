#include "Dnamicfindpos.h"
#include "skill/Factory.h"
#include "parammanager.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "DribbleStatus.h"
#include "WorldModel.h"
namespace{
    const CGeoPoint THEIR_GOAL = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0);
    const CGeoPoint THEIR_GOAL_LEFT = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2+270);
    const CGeoPoint THEIR_GOAL_RIGHT= CGeoPoint(PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2-270);
    const CGeoPoint THEIR_GOAL_LEFTj = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2);
    const CGeoPoint THEIR_GOAL_RIGHTj= CGeoPoint(PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2);
    const CGeoPoint LEFT_GOAL_POST  = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2+150);
    const CGeoPoint RIGHT_GOAL_POST = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2-150);
    bool DEBUG_SWITCH;
    bool canshoot(double dir,CGeoPoint pos)
    {
        if(dir > (RIGHT_GOAL_POST - pos).dir() ||dir<(LEFT_GOAL_POST - pos).dir())
           return false;
        else
           return true;
    }

    CGeoPoint bestshootpos(const CVisionModule* pVision,CGeoPoint pos)
    {
        //CGeoPoint p[6];
        int num = -1;
        double dis = 99999;
        //CGeoSegment seg(pos,THEIR_GOAL);

        for(int i = 0;i<PARAM::Field::MAX_PLAYER-1;i++)
        {
            const PlayerVisionT &enemy = pVision->theirPlayer(i);
            if(enemy.Valid())
            {
                if(enemy.Pos().dist(THEIR_GOAL) < dis)
                {
                    dis = enemy.Pos().dist(THEIR_GOAL);
                    num = i;
                }
            }
        }

        const PlayerVisionT &enemy = pVision->theirPlayer(num);
//        if(enemy.Y() > 0)
//        {
//            CGeoSegment seg(pos,THEIR_GOAL_LEFT);
//            CGeoSegment seg1(pos,THEIR_GOAL_RIGHT);
//           // if(seg.projection(enemy.Pos()).dist(enemy.Pos()) > 140)
//           // {
//                return THEIR_GOAL_LEFT;
//         //   }
////            else if(seg1.projection(enemy.Pos()).dist(enemy.Pos()) > 140)
////            {
////                return THEIR_GOAL_RIGHT;
////            }
////            else
////            {
////                return THEIR_GOAL;
////            }
//        }
//        else if(enemy.Y() <= 0)
//        {
//            CGeoSegment seg(pos,THEIR_GOAL_RIGHT);
//            CGeoSegment seg1(pos,THEIR_GOAL_LEFT);
////            if(seg.projection(enemy.Pos()).dist(enemy.Pos()) > 140)
////            {
//                return THEIR_GOAL_RIGHT;
////            }
////            else if(seg1.projection(enemy.Pos()).dist(enemy.Pos()) > 140)
////            {
////                return THEIR_GOAL_LEFT;
////            }
////            else
////            {
////                return THEIR_GOAL;
////            }
//        }
        return THEIR_GOAL;
//        for(int i = 0;i<6;i++)
//        {
//            if(seg.IsPointOnLineOnSegment(seg.projection(p[i])) && seg.projection(p[i]).dist(p[i]) < 120)
//            {
//                jud = false;
//            }
//        }
//        if(jud)
//            return THEIR_GOAL;
//        jud = true;
//        CGeoSegment seg1(pos,THEIR_GOAL_LEFTj);
//        for(int i = 0;i<6;i++)
//        {
//            if(seg1.IsPointOnLineOnSegment(seg1.projection(p[i])) && seg1.projection(p[i]).dist(p[i]) < 120)
//            {
//                jud = false;
//            }
//        }
//        if(jud)
//            return THEIR_GOAL_LEFT;
//        jud = true;
//        CGeoSegment seg2(pos,THEIR_GOAL_RIGHTj);
//        for(int i = 0;i<6;i++)
//        {
//            if(seg2.IsPointOnLineOnSegment(seg2.projection(p[i])) && seg2.projection(p[i]).dist(p[i]) < 120)
//            {
//                jud = false;
//            }
//        }
//        if(jud)
//            return THEIR_GOAL_RIGHT;
//        return THEIR_GOAL;
    }

    bool icanshoot(const CVisionModule* pVision, double dir,CGeoPoint pos)
    {
        CGeoPoint p[6];
        int num=0;
        bool jud = true;
        CGeoLine seg(pos,dir);
        for(int i = 0;i<16;i++)
        {
            const PlayerVisionT &enemy = pVision->theirPlayer(i);
            if(enemy.Valid())
            {
                p[num] = enemy.Pos();
                num++;
            }
        }
        for(int i = 0;i<6;i++)
        {
            if(seg.projection(p[i]).dist(p[i]) < 145)
            {
                jud = false;
            }
        }
        return jud;
//        if(jud)
//            return THEIR_GOAL;
//        jud = true;
//        CGeoSegment seg1(pos,THEIR_GOAL_LEFTj);
//        for(int i = 0;i<6;i++)
//        {
//            if(seg1.IsPointOnLineOnSegment(seg1.projection(p[i])) && seg1.projection(p[i]).dist(p[i]) < 120)
//            {
//                jud = false;
//            }
//        }
//        if(jud)
//            return THEIR_GOAL_LEFT;
//        jud = true;
//        CGeoSegment seg2(pos,THEIR_GOAL_RIGHTj);
//        for(int i = 0;i<6;i++)
//        {
//            if(seg2.IsPointOnLineOnSegment(seg2.projection(p[i])) && seg2.projection(p[i]).dist(p[i]) < 120)
//            {
//                jud = false;
//            }
//        }
//        if(jud)
//            return THEIR_GOAL_RIGHT;
//        return THEIR_GOAL;
    }
}
CDnamicfindpos::CDnamicfindpos(){
    ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH,"Debug/Touch",false);
}
void CDnamicfindpos::plan(const CVisionModule* pVision){
    if (pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
    }

    const int runner = task().executor; //获取当前角色
    const CGeoPoint targetpos = task().player.pos;
    const double targetdir = task().player.angle;
    int taskflag;
    const PlayerVisionT& me = pVision->ourPlayer(runner);//获取当前角色的视觉信息
    const auto mousePos = me.Pos() + Utils::Polar2Vector(PARAM::Vehicle::V2::PLAYER_CENTER_TO_BALL_CENTER,me.Dir());
    const MobileVisionT& ball = pVision->ball();
    const double ballVelDir = ball.Vel().dir();
    const CGeoPoint& ballPos = ball.RawPos();
    const CGeoLine ballVelLine(ballPos, ballVelDir);
    const double ballVelMod = ball.Vel().mod();
    const CGeoPoint projectionPos = ballVelLine.projection(mousePos);
    const double toBallDist = (mousePos - ballPos).mod();
    const double toBallDir  = (ballPos - mousePos).dir();
    CGeoPoint targetPos;
    double targetDir;

//    CGeoPoint pos1(900,100);
//    CGeoPoint pos2(1500,100);
//    CGeoPoint pos3(2300,100);

//    if(toBallDist>700)
//    {
//        targetPos = ball.Pos() + Utils::Polar2Vector(1000,(ball.Pos() - me.Pos()).dir());
//        targetDir = (ball.Pos() - me.Pos()).dir();
//        taskflag = PlayerStatus::DRIBBLING;
//    }
//    else
//    {
//        targetPos = ball.Pos() + Utils::Polar2Vector(80,(ball.Pos() - me.Pos()).dir());
//        targetDir = (ball.Pos() - me.Pos()).dir();
//        taskflag = PlayerStatus::DRIBBLING;
//    }
    CWorldModel p;
    int time = p.InfraredOnCount(runner);
//      if(time>20 &&toBallDist < 150)//&& abs(toBallDir - me.Dir()) < 3.14/6)
//      {
          //cout<<1<<endl;
          if(canshoot(me.Dir(),me.Pos())&&((me.Pos() - targetpos).mod() < 100 || icanshoot(pVision,me.Dir(),me.Pos())))
          {
              targetPos = ball.Pos() + Utils::Polar2Vector(110,toBallDir+3.14);
              targetDir = (bestshootpos(pVision,me.Pos()) - targetpos).dir();
              KickStatus::Instance()->setKick(runner,6300);
              DribbleStatus::Instance()->setDribbleOff(runner);
              taskflag = PlayerStatus::NOTHING;
          }
          else
          {
              //CGeoPoint sht(3000,-2000);
              targetPos = me.Pos() + Utils::Polar2Vector(800,(targetpos - me.Pos()).dir());
              targetDir = (bestshootpos(pVision,me.Pos()) - me.Pos()).dir();
              taskflag = PlayerStatus::DRIBBLING;
          }
//      }
//      else
//      {
//          if(toBallDist>700)
//          {
//              targetPos = ball.Pos() + Utils::Polar2Vector(1000,(ball.Pos() - me.Pos()).dir());
//              targetDir = (ball.Pos() - me.Pos()).dir();
//              taskflag = PlayerStatus::DRIBBLING;
//          }
//          else
//          {
//              targetPos = ball.Pos() + Utils::Polar2Vector(80,(ball.Pos() - me.Pos()).dir());
//              targetDir = (ball.Pos() - me.Pos()).dir();
//              taskflag = PlayerStatus::DRIBBLING;
//          }
//      }

      taskflag |=  PlayerStatus::ALLOW_DSS;//+ PlayerStatus::NOT_AVOID_PENALTY;

    // add avoid ball flag


    TaskT newTask(task());
    newTask.player.pos = targetPos;
    newTask.player.angle = targetDir;
    newTask.player.flag = taskflag;
    setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));

    if(DEBUG_SWITCH){
        auto endPos = ballPos + Utils::Polar2Vector(ballVelMod,ballVelDir);
        GDebugEngine::Instance()->gui_debug_line(ballPos,endPos,4);
    }

    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
