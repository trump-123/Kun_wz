--边线球接球机器人移位接球射门
local f = flag.dribbling + flag.allow_dss
local DSS_FLAG = flag.dodge_ball + flag.allow_dss
local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth
local ACC = 5000
local shootpower = 6500
local tobaldist = 120 --用于球速小于500，接球车力求距离判断
local isDnamic = false --true 为开启横向带球 对付强队用

local SpecialPos = {
	ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/8,param.pitchWidth/2-600)),
	ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/4 + 1000,param.pitchWidth/2-1500)),
	CGeoPoint:new_local(param.pitchLength/4,0),
	CGeoPoint:new_local(param.pitchLength/4,param.pitchWidth/2-600),
}

local AssisterPos = {
	ball.refSyntYPos(CGeoPoint:new_local(100,param.pitchWidth/2-400)),
	ball.refSyntYPos(CGeoPoint:new_local(param.pitchLength/4 + 500,param.pitchWidth/8)),
  ball.refSyntYPos(CGeoPoint:new_local(1500,param.pitchWidth/2-1000)),
}

local LeaderPos = {
	ball.refSyntYPos(CGeoPoint:new_local(180,1400)),
	ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/4 + 500,param.pitchWidth/8)),
}

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

local ToGoalPoint = {
  CGeoPoint:new_local(pl/2,0),
  ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
}

gPlayTable.CreatePlay{

  firstState = "start",

  ["start"] = {
    switch = function ()
      if bufcnt(player.kickBall("Leader") or ball.velMod() > 1000, 1, 60*6) then
            return "exit" 
      end
    end,
    Leader   = task.staticGetBall(getPassPos(),getPassVel()),
    Assister = task.goCmuRush(AssisterPos[1] ,player.toPointDir(ToGoalPoint[1]),_,DSS_FLAG + flag.free_kick),
    Special  = task.goCmuRush(SpecialPos[1] ,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG + flag.free_kick),
    Middle   = task.goCmuRush(SpecialPos[3] ,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG + flag.free_kick),
    Fronter  = task.goCmuRush(Defence_Pos, player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG + flag.free_kick),
    Kicker   = task.wmarking("Zero", flag.avoid_stop_ball_circle,getAttackerNum(0)),
    Center   = task.wmarking("First",flag.avoid_stop_ball_circle,getAttackerNum(1)),
    Defender = task.wback(3,1),
    Breaker  = task.wback(3,2),
    Receiver = task.wback(3,3),
    Goalie   = task.goalie(),
    match    = "[L][DBR][MAS][FKC]"
  },




name= "Ref_BackKickV1102",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}