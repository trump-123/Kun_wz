-- 四车上前，选取最合适的
-- 一车虚晃，三车并排
-- by fjm 2022-02-08

local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth
local DSS_FLAG = flag.dodge_ball + flag.allow_dss
local f = flag.dribbling + flag.allow_dss
local PASS_ACC = 2000
local shootpower = 6500
local tobaldist = 120 --用于球速小于500，接球车力求距离判断
local isDnamic = false --true 为开启横向带球 对付强队用

local chipPower = 160


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
local First  = ball.refSyntYPos(CGeoPoint:new_local(pl/2-ped/2-300,pw/2-500))
local Second = ball.refAntiYPos(CGeoPoint:new_local(1000,860))
local Third  = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-500,pw/4))
local Fourth = ball.refSyntYPos(CGeoPoint:new_local(200,0))

local Real1  = ball.refSyntYPos(CGeoPoint:new_local(pl/2-ped/2-1300,pw/2-500))

local Real = ball.refSyntYPos(CGeoPoint:new_local(790,670))

local ToGoalPoint = {
  CGeoPoint:new_local(pl/2,0),
  ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
}

local second = {
  ball.refSyntYPos(CGeoPoint:new_local(2000,1750)),
  ball.refSyntYPos(CGeoPoint:new_local(1890,900)),
}

local LeaderPos = {
  ball.refSyntYPos(CGeoPoint:new_local(pl/4+300,pw/2-500))
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

  


  name = "Ref_BackKickV803",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}