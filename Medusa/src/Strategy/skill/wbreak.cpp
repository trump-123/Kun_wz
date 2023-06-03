#include "wbreak.h"
#include "VisionModule.h"
#include "skill/Factory.h"
#include "utils.h"
#include "TaskMediator.h"
#include "RobotSensor.h"
#include "ShootModule.h"
#include "GDebugEngine.h"
#include "RobotSensor.h"

namespace  {
    double CHECK_OBSTCLE_DIST;
    double DRIBBLE_DIST;
    double SHOOT_ACCURACY;
    double PASS_ACCURACY;
    double MAX_ACC;
    double MAX_VEL;
    double MAX_ROT_ACC;
    double MAX_ROT_SPEED;

    bool VERBOSE;
    enum {
        SIDEMOVE = 1,
        DRIBBLE = 2,
    };
    int maxFrared = 210;
    const int MOD_NUM = 2;
    const int ANGEL_MOD[MOD_NUM + 1] = {0, 4 ,8};
    const double RADIUS = PARAM::Vehicle::V2::PLAYER_SIZE * 1.4;
    double SHOOT_TIME_DIFF_THRESHOLD;

    CGeoPoint calc_point(const CVisionModule* pVision, const int vecNumber,CGeoPoint&,const CGeoPoint&,const bool,bool&,bool&,const bool);
}
CWBreak::CWBreak():grabMode(DRIBBLE),last_mode(DRIBBLE){
    ZSS::ZParamManager::instance()->loadParam(VERBOSE,"Debug/WBreak",false);
    ZSS::ZParamManager::instance()->loadParam(CHECK_OBSTCLE_DIST,"WBreak/CHECK_OBSTCLE_DIST",150*10);
    ZSS::ZParamManager::instance()->loadParam(DRIBBLE_DIST,"WBreak/DRIBBLE_DIST",70*10);
    ZSS::ZParamManager::instance()->loadParam(SHOOT_TIME_DIFF_THRESHOLD,"WBreak/shootTimeDiffThreshold",0.2);
    ZSS::ZParamManager::instance()->loadParam(SHOOT_ACCURACY,"WBreak/SHOOT_ACCURACY",5);
    ZSS::ZParamManager::instance()->loadParam(PASS_ACCURACY,"WBreak/PASS_ACCURACY",3);
    ZSS::ZParamManager::instance()->loadParam(MAX_ACC,"WBreak/MAX_ACC",350*10);
    ZSS::ZParamManager::instance()->loadParam(MAX_VEL,"WBreak/MAX_VEL",150*10);
    ZSS::ZParamManager::instance()->loadParam(MAX_ROT_ACC,"WBreak/MAX_ROT_ACC",20);
    ZSS::ZParamManager::instance()->loadParam(MAX_ROT_SPEED,"WBreak/MAX_ROT_SPEED",20);
}

void CWBreak::plan(const CVisionModule *pVision){
    if(pVision->getCycle() - _lastCycle > PARAM::Vision::FRAME_RATE * 0.1){
        grabMode = DRIBBLE;
        last_mode = DRIBBLE;
        move_Point = CGeoPoint(-9999*10,-9999*10);
        dribblePoint = CGeoPoint(-9999*10,-9999*10);
        isDribble = false;
        fraredOn = 0;
        fraredOff = 0;
    }
    const MobileVisionT& ball = pVision->ball();
    CGeoPoint passTarget = task().player.pos;
    int vecNumber = task().executor;
    const PlayerVisionT& me = pVision->ourPlayer(vecNumber);
    int oppNum = ZSkillUtils::instance()->getTheirBestPlayer();
    const PlayerVisionT& enemy =  pVision->theirPlayer(oppNum);
    double power = task().player.kickpower;
    const bool isChip = (task().player.kick_flag & PlayerStatus::CHIP);
    const bool setKick = (task().player.kick_flag & PlayerStatus::KICK);
    const bool is_penalty = task().player.ispass;

    const CVector me2Ball = ball.Pos() - me.RawPos();
    const CVector me2enemy = enemy.Pos() - me.RawPos();
    const CVector me2target = passTarget - me.RawPos();

    CGeoPoint final_target = passTarget;

    GDebugEngine::Instance()->gui_debug_line(me.Pos(),passTarget,COLOR_GREEN);
    bool canShoot = true;
    bool needBreakThrough = false;
    bool shootGoal = Utils::InTheirPenaltyArea(passTarget,0);
    bool canshootGoal = false;
    if(shootGoal){
        final_target = CGeoPoint(PARAM::Field::PITCH_LENGTH/2,0);
        canshootGoal = ShootModule::Instance()->generateBestTarget(pVision,final_target,ball.Pos());
    }

    double precision = shootGoal ? SHOOT_ACCURACY : PASS_ACCURACY;

    bool frared = RobotSensor::Instance()->IsInfraredOn(vecNumber);
    dribblePoint = RobotSensor::Instance()->dribblePoint(vecNumber);

    if(frared){
        fraredOn = fraredOn >= maxFrared ? maxFrared : fraredOn + 1;
        if(fraredOn > 5) fraredOff = 0;
        if(!isDribble) isDribble = true;
    }else{
        fraredOff = fraredOff >= maxFrared ? maxFrared : fraredOff + 1;
        if(fraredOff > 5) {
            fraredOn = 0;
        }
        if(isDribble) isDribble = false;
    }

    TaskT grabTask(task());

    if(fraredOn > 50){
        grabMode = SIDEMOVE;
    } else if(fraredOff > 7){
        grabMode = DRIBBLE;
    }
    //cout<<fraredOn<<endl;

    if(frared && last_mode == SIDEMOVE) grabMode = SIDEMOVE;

    //红外误判，能看到球但球不在嘴里，且有红外信息，比如碰到别的机器人的屁股
    bool ballinfront = (me2Ball.mod() < 20*10 && fabs(me.Dir() - me2Ball.dir()) < PARAM::Math::PI/4);
    if(frared && !ballinfront && ball.Valid() && me2enemy.mod() < 24*10){
        frared = false;
        fraredOn = 0;
        fraredOff = 0;
        grabMode = DRIBBLE;
    }
    last_mode = grabMode;

    if(Utils::InOurPenaltyArea(passTarget,0)) canShoot = false;

    /**************** set subTask*********************************/
    if(grabMode == SIDEMOVE){
        if(VERBOSE) GDebugEngine::Instance()->gui_debug_msg((dribblePoint + Utils::Polar2Vector(25*10,-PARAM::Math::PI) + Utils::Polar2Vector(106*10,PARAM::Math::PI/2)),"break",COLOR_CYAN);
        if(is_penalty) move_Point = calc_point(pVision,vecNumber,final_target,dribblePoint,isChip,canShoot,needBreakThrough, is_penalty);
        else move_Point = calc_point(pVision,vecNumber,passTarget,dribblePoint,isChip,canShoot,needBreakThrough, is_penalty);

        if(VERBOSE) GDebugEngine::Instance()->gui_debug_arc(move_Point,150,0,360,COLOR_GREEN);

        grabTask.player.pos = move_Point;
        grabTask.player.angle = (final_target - me.Pos()).dir();
        if(Utils::InTheirPenaltyArea(grabTask.player.pos,32*10) || Utils::InTheirPenaltyArea(me.Pos(),32*10)){
            grabTask.player.pos = Utils::MakeOutOfTheirPenaltyArea(grabTask.player.pos,32*10);
        }
        if((shootGoal && canshootGoal) || !needBreakThrough){
            grabTask.player.flag = PlayerStatus::DRIBBLE;
        } else {
            grabTask.player.flag = PlayerStatus::DRIBBLE + PlayerStatus::BREAK_THROUGH;
        }

        grabTask.player.max_acceleration = MAX_ACC;
        grabTask.player.max_deceleration = MAX_ACC;
        grabTask.player.max_speed = MAX_VEL;
        grabTask.player.max_rot_acceleration = MAX_ROT_ACC;
        grabTask.player.max_rot_speed = MAX_ROT_SPEED;
        grabTask.player.path_plan_in_circle = true;
        grabTask.player.path_plan_circle_center = dribblePoint;
        grabTask.player.path_plan_circle_radius = 200*10;
        if(is_penalty) grabTask.player.need_rot_to_angle = false;
        else grabTask.player.need_rot_to_angle = canShoot && setKick;
        setSubTask(TaskFactoryV2::Instance()->SmartGotoPosition(grabTask));
    }

    if(grabMode == DRIBBLE) {
//        if(selfPass) grabTask.player.flag |= PlayerStatus::RUSH;
        move_Point = CGeoPoint(-9999*10, -9999*10);
        if (VERBOSE) GDebugEngine::Instance()->gui_debug_msg(me.Pos()+ Utils::Polar2Vector(300, -PARAM::Math::PI/1.5), "Dribble", COLOR_CYAN);
        CGeoPoint target;
        double dir;
        if(is_penalty)
        {
            target = final_target;
            dir = (target - me.Pos()).dir();
        }
        else
        {
            if(ball.Valid()) target = ball.Pos() + Utils::Polar2Vector(5*10, enemy.Dir());
            else target = enemy.Pos() + Utils::Polar2Vector(5*10, enemy.Dir());
            if(ball.Valid()) dir = me2Ball.dir();
            else dir = me2enemy.dir();
        }
//        dir = (target - me.Pos()).dir();
//        if(ball.Valid()) target = ball.Pos() + Utils::Polar2Vector(5*10, enemy.Dir());
//        else target = enemy.Pos() + Utils::Polar2Vector(5*10, enemy.Dir());
//        if(ball.Valid()) dir = me2Ball.dir();
//        else dir = me2enemy.dir();
//        if(me2Ball.mod() > 50*10 || ball.Vel().mod() > 50*10){
//            setSubTask(PlayerRole::makeItGetBallV4(vecNumber, PlayerStatus::RUSH|PlayerStatus::DRIBBLE|PlayerStatus::SAFE, target, CGeoPoint(999*10, 999*10), 0));
//        }
        //else{
            if(!Utils::IsInField(target,PARAM::Vehicle::V2::PLAYER_SIZE+PARAM::Field::BALL_SIZE*2))
                target = Utils::MakeInField(target,PARAM::Vehicle::V2::PLAYER_SIZE+PARAM::Field::BALL_SIZE*2);
            setSubTask(PlayerRole::makeItGoto(vecNumber, target, dir, ballinfront ? PlayerStatus::NOT_AVOID_THEIR_VEHICLE : 0));
        //}
    }
    bool enemyInFace = false;
    std::vector<CGeoPoint> enemy_points;
    for(int i = 0; i < PARAM::Field::MAX_PLAYER; i++){
        auto test_enemy = pVision->theirPlayer(i);
        if(test_enemy.Valid() && (test_enemy.Pos()-me.RawPos()).mod() < PARAM::Vehicle::V2::PLAYER_SIZE * 2.2 && !Utils::InTheirPenaltyArea(test_enemy.Pos(),0)) enemy_points.push_back(test_enemy.Pos());
    }
    for(auto test_enemy : enemy_points){
        auto me2enemy = test_enemy - me.RawPos();
        if(std::abs(Utils::Normalize(me2enemy.dir() - me.Dir())) < 45 / 180 * PARAM::Math::PI){
            enemyInFace = true;
            break;
        }
    }
    //if(!isChip && !enemyInFace) KickStatus::Instance()->setKick(vecNumber, power);
    //else if(fraredOn >= 20) KickStatus::Instance()->setChipKick(vecNumber, power);

    DribbleStatus::Instance()->setDribbleCommand(vecNumber,3);
    _lastCycle = pVision->getCycle();
    return CStatedTask::plan(pVision);
}

CPlayerCommand* CWBreak::execute(const CVisionModule *pVision){
    if(subTask()){
        return subTask()->execute(pVision);
    }
    return nullptr;
}

namespace  {
CGeoPoint calc_point(const CVisionModule* pVision,const int vecNumber,CGeoPoint &target,const CGeoPoint&dribblePoint,const bool isChip,bool &canShoot,bool &needBreakThrough, const bool is_penalty){
    enum breakmode{
        is_Penalty,shootGoal,normal
    }mode;
    canShoot = false;
    needBreakThrough = false;
    const PlayerVisionT& me = pVision->ourPlayer(vecNumber);
    double max_time_diff = -99999.0;
    double max_straight_dist = -9999*10;
    double to_target_dist = 9999*10;
    CGeoPoint move_point = me.RawPos();
    std::vector<CGeoPoint> enemy_points;
    CVector me2target = target - dribblePoint;
    for(int i = 0; i < PARAM::Field::MAX_PLAYER; i++){
        auto test_enemy = pVision->theirPlayer(i);
        if(test_enemy.Valid() && (test_enemy.Pos()-dribblePoint).mod() < CHECK_OBSTCLE_DIST && !Utils::InTheirPenaltyArea(test_enemy.Pos(),0)) enemy_points.push_back(test_enemy.Pos());
    }

    for(auto test_enemy : enemy_points){
        auto me2enemy = test_enemy - move_point;
        if(me2enemy.mod() < 60*10 && fabs(Utils::Normalize(me.Dir() - me2enemy.dir())) < PARAM::Math::PI/3){
            needBreakThrough = true;
            break;
        }
    }

    if(is_penalty) mode = is_Penalty;
    else{
        if(Utils::InTheirPenaltyArea(target, 0)) mode = shootGoal;
        else
            mode = normal;
    }

    switch(mode) {
        case is_Penalty:
        {
            for(int j = -10; j<=10; j++){
                if(VERBOSE) GDebugEngine::Instance()->gui_debug_msg((dribblePoint  + Utils::Polar2Vector(10*20,PARAM::Math::PI)+ Utils::Polar2Vector(106*10,-PARAM::Math::PI/2)),"penalty",COLOR_ORANGE);
                double X = me.Pos().x() + 200 > 2600 ? 2600.0 - me.Pos().x() : 200.0;
                X = X > 0.0 ? X : 0.0;
                CVector vec(X,200*j);
                CGeoPoint test_point = me.Pos() + vec;
                if((test_point - dribblePoint).mod() > 900) continue;
                if(!Utils::IsInField(test_point, 18*10)) test_point = Utils::MakeInField(test_point,18*10);
                if(Utils::InTheirPenaltyArea(test_point, 30*10)) test_point = Utils::MakeInField(test_point,18*10);
                CGeoPoint temp_target(PARAM::Field::PITCH_LENGTH/2,0);
                double temp_max_time_diff = ShootModule::Instance()->generateBestTimeDiff(pVision, temp_target, test_point);
                bool temp_canShoot = (temp_max_time_diff > 0);
                //if(VERBOSE) GDebugEngine::Instance()->gui_debug_x(test_point,COLOR_PURPLE);
                if(temp_canShoot) GDebugEngine::Instance()->gui_debug_arc(test_point,4*10,0,360,COLOR_CYAN);
                if(canShoot){
                    if(temp_canShoot){
                        if(temp_max_time_diff > max_time_diff){
                            max_time_diff = temp_max_time_diff;
                            move_point = test_point;
                            target = temp_target;
                        }
                    }
                } else {
                    if(temp_canShoot){
                        canShoot = true;
                        max_time_diff = temp_max_time_diff;
                        move_point = test_point;
                        target = temp_target;
                    }
                }
            }
            if(max_time_diff < 0.2)
                canShoot = false;
            break;
        }
    case shootGoal:
    {
        for(int j=MOD_NUM; j>0; j--){
            for(int i=-ANGEL_MOD[j]; i<ANGEL_MOD[j]; i++){
                if(VERBOSE) GDebugEngine::Instance()->gui_debug_msg((dribblePoint  + Utils::Polar2Vector(10*20,PARAM::Math::PI)+ Utils::Polar2Vector(106*10,-PARAM::Math::PI/2)),"shootgoal",COLOR_ORANGE);
                CVector vec = Utils::Polar2Vector(double(j*DRIBBLE_DIST/MOD_NUM), Utils::Normalize(me2target.dir() + i*PARAM::Math::PI/ANGEL_MOD[j]));
                CGeoPoint test_point = dribblePoint + vec;
                if(!Utils::IsInField(test_point, 18*10)) test_point = Utils::MakeInField(test_point,18*10);
                if(Utils::InTheirPenaltyArea(test_point, 30*10)) test_point = Utils::MakeOutOfTheirPenaltyArea(test_point,30*10);
                if (VERBOSE) GDebugEngine::Instance()->gui_debug_x(test_point, COLOR_PURPLE);
                CGeoPoint temp_target(PARAM::Field::PITCH_LENGTH/2,0);
                double temp_max_time_diff = ShootModule::Instance()->generateBestTimeDiff(pVision,temp_target,test_point);
    //            printf("point:%d    time diff:%lf\n",j,temp_max_time_diff);
                bool temp_canShoot = (temp_max_time_diff > 0);
                if(temp_canShoot)
                    GDebugEngine::Instance()->gui_debug_arc(test_point,4*10,0,360,COLOR_CYAN);
                // update
                if(canShoot){
                    if(temp_canShoot){
                        if(temp_max_time_diff > max_time_diff)
                        {
                            max_time_diff = temp_max_time_diff;
                            move_point = test_point;
                            target = temp_target;
                        }
                    }
                }else{
                    if(temp_canShoot)
                    {
                        canShoot = true;
                        max_time_diff = temp_max_time_diff;
                        move_point = test_point;
                        target = temp_target;
                    }
                }
            }
        }
//            printf("max time diff: %lf\n", max_time_diff);
        if(max_time_diff < SHOOT_TIME_DIFF_THRESHOLD)     canShoot = false;
        if(canShoot)    break;
    }
    default:
    {
        for(int i = MOD_NUM;i > 0; i--){
            for(int j = -ANGEL_MOD[i]; j < ANGEL_MOD[i] ;j++){
                if(VERBOSE) GDebugEngine::Instance()->gui_debug_msg((dribblePoint + Utils::Polar2Vector(10*30,PARAM::Math::PI)+ Utils::Polar2Vector(106*10,-PARAM::Math::PI/2)),"normal",COLOR_ORANGE);
                bool temp_canShoot = true;
                double temp_max_straight_dist = 9999*10;
                CVector vec = Utils::Polar2Vector(double(i*DRIBBLE_DIST/MOD_NUM),double(me2target.dir() + j* PARAM::Math::PI / ANGEL_MOD[i]));
                CGeoPoint temp_point = dribblePoint + vec;
                //if(VERBOSE) GDebugEngine::Instance()->gui_debug_x(temp_point,COLOR_PURPLE);
                if(!Utils::IsInField(temp_point,18*10)) temp_point =  Utils::MakeInField(temp_point,18*10);
                if(Utils::InTheirPenaltyArea(temp_point,30*10))temp_point= Utils::MakeOutOfTheirPenaltyArea(temp_point,30*10);
                auto test_seg = CGeoSegment(target,temp_point);
                double temp2targetdist = (temp_point - target).mod();
                for(auto temp_enemy : enemy_points){
                    auto projection = test_seg.projection(temp_enemy);
                    auto enemy2projection_dist = temp_enemy.dist(projection);
                    auto enemy2temp_dist = temp_enemy.dist(temp_point);
                    auto mp2projection_dist = move_point.dist(projection);
                    if(isChip){
                        if(test_seg.IsPointOnLineOnSegment(projection) && enemy2projection_dist < RADIUS && mp2projection_dist < RADIUS){
                            if(VERBOSE) GDebugEngine::Instance()->gui_debug_line(dribblePoint,temp_point,COLOR_GREEN);
                            GDebugEngine::Instance()->gui_debug_line(temp_enemy,temp_point,COLOR_YELLOW);
                            temp_canShoot = false;
                        }
                    }else{
                        if((test_seg.IsPointOnLineOnSegment(projection) && enemy2projection_dist < std::max(enemy2temp_dist*0.3,1.0*RADIUS)) || enemy2temp_dist < RADIUS){
                            if(VERBOSE) GDebugEngine::Instance()->gui_debug_line(dribblePoint,temp_point,COLOR_GREEN);
                            GDebugEngine::Instance()->gui_debug_line(temp_enemy,temp_point,COLOR_YELLOW);
                            temp_canShoot = false;
                        }
                    }
                    temp_max_straight_dist = std::min(temp_max_straight_dist,enemy2temp_dist);
                }
                if (temp_canShoot) GDebugEngine::Instance()->gui_debug_arc(temp_point,4*10,0,360,COLOR_CYAN);
                if(fabs(temp_max_straight_dist - 9999*10) < 0.1) temp_max_straight_dist = -9999*10;
                if(canShoot){
                    if(temp_canShoot){
                        if(max_straight_dist - temp_max_straight_dist < -0.1*10){
                            max_straight_dist = temp_max_straight_dist;
                            move_point = temp_point;
                            to_target_dist = temp2targetdist;
                        }else if(fabs(max_straight_dist - temp_max_straight_dist)<0.01*10 && to_target_dist > temp2targetdist){
                            max_straight_dist = temp_max_straight_dist;
                            move_point = temp_point;
                            to_target_dist = temp2targetdist;
                        }
                    }
                }else{
                    if(temp_canShoot){
                        canShoot = true;
                        max_straight_dist = temp_max_straight_dist;
                        move_point = temp_point;
                        to_target_dist = temp2targetdist;
                    }else if(max_straight_dist - temp_max_straight_dist < -0.1*10){
                        canShoot = false;
                        max_straight_dist = temp_max_straight_dist;
                        move_point = temp_point;
                        to_target_dist = temp2targetdist;
                    }else if(fabs(max_straight_dist - temp_max_straight_dist)<0.01*10 && to_target_dist > temp2targetdist){
                        canShoot = false;
                        max_straight_dist = temp_max_straight_dist;
                        move_point = temp_point;
                        to_target_dist = temp2targetdist;
                    }
                }
            }
        }
                canShoot = true;
                if(canShoot){
                    auto ball = pVision->ball();
                    CGeoPoint ball_pos = ball.Valid()?ball.RawPos():ball.Pos();
                    auto test_seg = CGeoSegment(ball_pos,target);
                    for(auto temp_enemy : enemy_points){
                        auto projection = test_seg.projection(temp_enemy);
                        auto enemy2projection_dist = temp_enemy.dist(projection);
                        auto enemy2temp_dist = temp_enemy.dist(ball_pos);
                        auto mp2projection_dist = move_point.dist(projection);
                        if(isChip){
                            if(test_seg.IsPointOnLineOnSegment(projection) && enemy2projection_dist < 1.0*RADIUS && mp2projection_dist < 1.0*RADIUS){
                                if(VERBOSE) GDebugEngine::Instance()->gui_debug_line(dribblePoint,ball_pos,COLOR_GREEN);
                                if(VERBOSE) GDebugEngine::Instance()->gui_debug_line(ball_pos,temp_enemy,COLOR_GREEN);
                                canShoot = false;
                                break;
                            }
                        }else{
                            if((test_seg.IsPointOnLineOnSegment(projection) && enemy2projection_dist < std::max(enemy2temp_dist*0.2,1.0*RADIUS))){
                                if(VERBOSE) GDebugEngine::Instance()->gui_debug_arc(temp_enemy,1.0*RADIUS,0,360,COLOR_RED);
                                if(VERBOSE) GDebugEngine::Instance()->gui_debug_line(temp_enemy,projection,COLOR_YELLOW);
                                canShoot = false;
                                break;
                            }
                        }
                    }
                }
            }
        }

    return move_point;
}
}
