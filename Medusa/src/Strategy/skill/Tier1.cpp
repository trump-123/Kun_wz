#include "Tier1.h"
#include "skill/Factory.h"
#include "TaskMediator.h"
#include "WorldDefine.h"
#include "VisionModule.h"
#include "staticparams.h"
#include "parammanager.h"
#include "KickStatus.h"

namespace  {
   bool DEBUG_SWITCH;
   const CGeoPoint OUR_GOAL(-PARAM::Field::PITCH_LENGTH/2,0);
   const CGeoPoint LEFT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,-PARAM::Field::GOAL_WIDTH/2 - 2);
   const CGeoPoint RIGHT_GOAL_POST(-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::GOAL_WIDTH/2 + 2);
   const CGeoSegment GOAL_LINE(LEFT_GOAL_POST,RIGHT_GOAL_POST);
   const static int MIN_X = -PARAM::Field::PITCH_LENGTH/2;

   double get_defence_direction(const CGeoPoint & pos){
       double leftPostToBallDir = (pos - LEFT_GOAL_POST).dir();
       double rightPostToBallDir = (pos - RIGHT_GOAL_POST).dir();
       if(DEBUG_SWITCH){
           GDebugEngine::Instance()->gui_debug_line(pos,LEFT_GOAL_POST,6);
           GDebugEngine::Instance()->gui_debug_line(pos,RIGHT_GOAL_POST,6);
       }
       return Utils::Normalize((leftPostToBallDir + rightPostToBallDir) / 2 );//+ PARAM::Math::PI
   }
}
CTier1::CTier1(){
     ZSS::ZParamManager::instance()->loadParam(DEBUG_SWITCH, "DEBUG/Tier1", false);
}
void CTier1::plan(const CVisionModule* pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE*0.1){
    }
    const int vecNumber = task().executor;
    const PlayerVisionT& self = pVision->ourPlayer(vecNumber);

    auto ball = pVision->ball().Pos();
    auto pi = PARAM::Math::PI;
    CGeoRectangle stand_rectangle(-PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH + PARAM::Field::MAX_PLAYER_SIZE/2+120,-PARAM::Field::PENALTY_AREA_WIDTH/2 - PARAM::Field::MAX_PLAYER_SIZE/2 - 120,-PARAM::Field::PITCH_LENGTH/2,PARAM::Field::PENALTY_AREA_WIDTH/2 + PARAM::Field::MAX_PLAYER_SIZE/2 + 120);
    CGeoLineRectangleIntersection stand_intersection(CGeoLine(ball,get_defence_direction(ball)),stand_rectangle);
    CGeoPoint stand_pos;

    if(stand_intersection.intersectant()){
        if(ball.dist2(stand_intersection.point1())<ball.dist2(stand_intersection.point2()))
            stand_pos = stand_intersection.point1()+ Utils::Polar2Vector(PARAM::Field::MAX_PLAYER_SIZE/2+10,Utils::Normalize(get_defence_direction(ball)+pi*1/2));
        else
            stand_pos = stand_intersection.point2()+ Utils::Polar2Vector(PARAM::Field::MAX_PLAYER_SIZE/2+10,Utils::Normalize(get_defence_direction(ball)+pi*1/2));
        if(stand_pos.x() < MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE)
            stand_pos.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE);
    }else{
        stand_pos=CGeoPoint(-PARAM::Field::PITCH_LENGTH+PARAM::Field::PENALTY_AREA_DEPTH+PARAM::Vehicle::V2::PLAYER_SIZE,0);
    }

    if(ball.y()<-PARAM::Field::PENALTY_AREA_WIDTH/2)
    {
        if(stand_pos.x()<MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE*3)
        {
            stand_pos.setX(MIN_X + PARAM::Vehicle::V2::PLAYER_SIZE*3);
        }
    }

    int taskFlag;
//    if((self.Pos() - stand_pos).mod() > PARAM::Vehicle::V2::PLAYER_SIZE*1)
//    {
//        taskFlag = PlayerStatus::ALLOW_DSS;
//    }
//    else
//    {
        taskFlag = PlayerStatus::NOT_AVOID_OUR_VEHICLE | PlayerStatus::NOT_AVOID_THEIR_VEHICLE;
//    }

      for(int i = 1;i<6;i++)
      {
          if(i!=vecNumber && (self.Pos().y())>0)
          {
             const PlayerVisionT& our = pVision->ourPlayer(i);
             if((our.Pos() - self.Pos()).mod() < 185)
             {
                taskFlag = PlayerStatus::ALLOW_DSS;
             }
          }
      }
//    if((self.Pos() - stand_pos).mod() > 700 || self.X() > -PARAM::Field::PITCH_LENGTH/2 + PARAM::Field::PENALTY_AREA_DEPTH + 700)
//    {
//       taskFlag = PlayerStatus::ALLOW_DSS + PlayerStatus::DODGE_BALL;
//    }

//    if((self.X()>-PARAM::Field::PITCH_LENGTH/2+PARAM::Field::PENALTY_AREA_DEPTH-10)&&abs(self.Y())>(PARAM::Field::PENALTY_AREA_WIDTH/2-10))
//    {
//        taskFlag |= PlayerStatus::NOT_AVOID_PENALTY;
//    }

    TaskT newTask(task());
    newTask.player.pos = stand_pos;
    newTask.player.angle = (self.Pos() - OUR_GOAL).dir()+0.1;
    newTask.player.max_acceleration = 5000;
    newTask.player.flag = taskFlag;

    setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(newTask));
    //setSubTask(PlayerRole::makeItSmartGoto(vecNumber,CGeoPoint(100,100),0,0));
    _lastCycle = pVision->getCycle();
    CPlayerTask::plan(pVision);
}
