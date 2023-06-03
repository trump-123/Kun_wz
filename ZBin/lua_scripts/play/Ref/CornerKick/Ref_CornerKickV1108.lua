-- 四车上前，选取最合适的
-- 一车虚晃，三车并排
-- by fjm 2022-02-08

local f   = flag.dribbling + flag.allow_dss
local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth
local DSS_FLAG = flag.dodge_ball + flag.allow_dss
local PASS_ACC = 2000
local shootpower = 8000
local directpower = 6500
local time = 750
local tobaldist = 120 --用于球速小于500，接球车力求距离判断
local isDnamic = true --true 为开启横向带球 对付强队用

--9*6
--First stand 序号代表谁的优先级更高
-- local First  = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped+300,pew/2+290))
-- local Second = ball.refSyntYPos(CGeoPoint:new_local(3300,1400))
-- local Third  = ball.refSyntYPos(CGeoPoint:new_local(3000,1300))
-- local Fourth = ball.refSyntYPos(CGeoPoint:new_local(2700,1200))

-- local Real   = ball.refSyntYPos(CGeoPoint:new_local(1900,1000))

-- local ToGoalPoint = {
--   CGeoPoint:new_local(pl/2,0),
--   ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
--   ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
-- }

-- local second = {
--   ball.refSyntYPos(CGeoPoint:new_local(3300,2500)),
--   ball.refSyntYPos(CGeoPoint:new_local(3000,1300)),
-- }

--6*4
local First  = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-500,pew/2))
local Second = ball.refSyntYPos(CGeoPoint:new_local(pl/16,pw/4+290))
local Third  = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-900,pew/2-200))
local Fourth = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-1300,pew/2-400))

local Real  = ball.refAntiYPos(CGeoPoint:new_local(2400,500))

local Real1 = ball.refSyntYPos(CGeoPoint:new_local(pl/8+300,pw/4-500))

local ToGoalPoint = {
  CGeoPoint:new_local(pl/2,0),
  ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
}

local otherPos = ball.refSyntYPos(CGeoPoint:new_local(0,pw/2-1000))

local ANTIPos  = ball.refAntiYPos(CGeoPoint:new_local(pl/8-500,-500))

local Defence_Pos = function()
      if ball.posY() < 0 then
         return CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + param.playerRadius*1.5,param.penaltyWidth/2-param.playerRadius)
      else
         return CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + param.playerRadius*1.5,-param.penaltyWidth/2+param.playerRadius)
      end
end


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



gPlayTable.CreatePlay{

  firstState = "start",

  ["start"] = {
    switch = function ()
        --debugEngine:gui_debug_msg(CGeoPoint(0,0),getPassVel()())
        --debugEngine:gui_debug_x(getPassPos()())
      if bufcnt(player.kickBall("Leader") or ball.velMod() > 1000, 1, 60*6) then
          return "exit" 
      end
    end,
    Leader   = task.staticGetBall(getPassPos(),getPassVel()),
    Assister = task.goCmuRush(First ,player.toPointDir(ToGoalPoint[1]),_,DSS_FLAG + flag.free_kick),
    Special  = task.goCmuRush(Second,player.toPointDir(ToGoalPoint[2]),_,DSS_FLAG + flag.free_kick),
    Middle   = task.goCmuRush(Third ,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG + flag.free_kick),
    Defender = task.goCmuRush(Fourth,player.toPointDir(ToGoalPoint[2]),_,DSS_FLAG + flag.free_kick),
    Fronter  = task.goCmuRush(Defence_Pos,player.toPointDir(ToGoalPoint[2]),_,DSS_FLAG + flag.free_kick),
    Kicker   = task.goCmuRush(otherPos ,0,ACC,DSS_FLAG + flag.free_kick),
    Center   = task.goCmuRush(ANTIPos , 0,ACC,DSS_FLAG + flag.free_kick),
    Breaker  = task.wback(2,1),
    Receiver = task.wback(2,2),
    Goalie   = task.goalie(),
    match    = "[L][BR][ASMD][FKC]"
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

  


  name = "Ref_CornerKickV1108",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}