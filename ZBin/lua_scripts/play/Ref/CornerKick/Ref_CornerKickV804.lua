-- 四车上前，选取最合适的
-- 四车分散站位
-- by fjm 2022-02-07
-- 缺点 选中第四车时接球不稳 need modify
local f = flag.dribbling + flag.allow_dss
local DSS_FLAG = flag.dodge_ball + flag.allow_dss 
local NOT_FLAG = flag.dodge_ball + flag.allow_dss + flag.not_avoid_our_vehicle + flag.free_kick
local PASS_ACC = 2000
local shootpower = 6500
local directpower = 8000
local time = 750
local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth
local isDnamic = false --true 为开启横向带球 对付强队用
local jud = true
local tobaldist = 120

--6*4
--First stand 序号代表谁的优先级更高
-- local First  = ball.refSyntYPos(CGeoPoint:new_local(800, 250))--ball.refSyntYPos(CGeoPoint:new_local(pl/4+200, 0))
-- local Second = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-800,pew/4))
-- local Third  = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-300,pew/2+290))
-- local Fourth = ball.refSyntYPos(CGeoPoint:new_local(pl/4-500, pw/4))

--9*6
local First  = ball.refSyntYPos(CGeoPoint:new_local(1200, 175))--ball.refSyntYPos(CGeoPoint:new_local(pl/4+200, 0))
local Second = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-1200,pew/4))
local Third  = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-300,pew/2+290))
local Fourth = ball.refSyntYPos(CGeoPoint:new_local(pl/4-500, pw/4))

local ReFirst  = ball.refSyntYPos(CGeoPoint:new_local(2400, -475))--ball.refSyntYPos(CGeoPoint:new_local(pl/4+200, 0))
local ReSecond = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-1000,pew/4-500))
local ReThird  = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-800,pew/2+350))
local ReFourth = ball.refSyntYPos(CGeoPoint:new_local(pl/4-500, pw/4-1000))

local ToGoalPoint = {
  CGeoPoint:new_local(pl/2,0),
  ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
}

local getPassPos = function()
    return function()
        return messi:freeKickPos()
    end
end

local getPassVel = function()
    return function()
        return messi:freepassVel()--*1.4
    end
end

local Confirm_Leader = function()
    gRoleNum["Leader"] = messi:leaderNum()
end

local temp = false
local staticpos

local LEADER_POS = function()
    if temp and staticpos~=nil then
        return staticpos
    else
        staticpos = CGeoPoint:new_local(math.random(pl/8,pl/4),math.random(pw/8,pw/4))
        temp = true
        return staticpos
    end
end


gPlayTable.CreatePlay{

  firstState = "start",

  ["start"] = {
    switch = function ()
      if bufcnt(player.kickBall("Leader") or ball.velMod() > 1000, 1, 60*6) then
            return "exit" 
      end
    end,
    Leader   = task.staticGetBall(getPassPos(),getPassVel()),
    Assister = task.goCmuRush(First ,player.toPointDir(ToGoalPoint[1]),_,DSS_FLAG + flag.free_kick),
    Special  = task.goCmuRush(Second,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG + flag.free_kick),
    Middle   = task.goCmuRush(Third ,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG + flag.free_kick),
    Defender = task.goCmuRush(Fourth,player.toPointDir(ToGoalPoint[2]),_,DSS_FLAG + flag.free_kick),
    Breaker  = task.wback(2,1),
    Receiver = task.wback(2,2),
    Goalie   = task.goalie(),
    match    = "[L][BR][ASMD]"
  },

  ["shoot"] = {
    switch = function ()
        Confirm_Leader()
        if bufcnt(player.kickBall("Leader"),1,750) then
            return "exit"
        end
        if bufcnt(messi:nextState() == "GetBall",20) then
            return "exit"
        end
    end,
    Leader   = task.goandTurnKick(pos.theirGoal,_,6000),
    Assister = task.goCmuRush(First,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG),
    Special  = task.goCmuRush(Second,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG),
    Middle   = task.goCmuRush(Third ,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG),
    Defender = task.goCmuRush(Fourth,player.toPointDir(ToGoalPoint[2]),_,DSS_FLAG),
    Breaker  = task.wback(2,1),
    Receiver = task.wback(2,2),
    Goalie   = task.goalie(),
    match    = "[L][BR][ASMD]"
  },

  

  name = "Ref_CornerKickV804",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}