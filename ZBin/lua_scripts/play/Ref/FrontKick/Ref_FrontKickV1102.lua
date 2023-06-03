--三车上前，一车拿球，两车站位，站位车选个合适的
--方案一判断传边线机器人，方案二传中场机器人
--方案二传中场会判断能否直接射门
local f = flag.dribbling + flag.allow_dss
local DSS_FLAG = flag.allow_dss + flag.dodge_ball
local ACC = 5000
local PASS_ACC = 2000
local shootpower = 8000
local directpower = 6500
local time = 750
local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth
local tobaldist = 120 --用于球速小于500，接球车力求距离判断
local isDnamic = true --true 为开启横向带球 对付强队用


local AssiserPos = {
  ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/8,param.pitchWidth/2-400)),
  ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/4 + 500,param.pitchWidth/2-1500)),
  CGeoPoint:new_local(param.pitchLength/4,0),
  CGeoPoint:new_local(param.pitchLength/4,param.pitchWidth/2-300),
}

local SpecialPos = {
  ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/8,param.pitchWidth/8)),
  ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/8 + 700,param.pitchWidth/8 + 500)),
}

local LeaderPos = {
  ball.refSyntYPos(CGeoPoint:new_local(param.pitchLength/4 + 500,param.pitchWidth/2-1500)),
  ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/4 + 500,param.pitchWidth/8)),
}

local movep = {
  ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/4 + 500,param.pitchWidth/2-1500-800)),
  ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/8 + 700,param.pitchWidth/8-800+500)),
  ball.refSyntYPos(CGeoPoint:new_local(param.pitchLength/4 + 500,param.pitchWidth/2-800-1500)),
}

local ToGoalPoint = {
  CGeoPoint:new_local(pl/2,0),
  ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
}

local backPos = {
    ball.refAntiYPos(CGeoPoint:new_local(-pl/4,pw/4)),
    ball.refSyntYPos(CGeoPoint:new_local(-pl/4,pw/4))
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


gPlayTable.CreatePlay{

firstState = "start",

["start"] = {
	 switch = function()
      if bufcnt(player.kickBall("Leader") or ball.velMod() > 1000, 1, 60*6) then
          return "exit" 
      end
	 end,
	 Leader   = task.staticGetBall(getPassPos(),getPassVel()),
	 Assister = task.goCmuRush(AssiserPos[2],0,ACC,DSS_FLAG + flag.free_kick),	
	 Special  = task.goCmuRush(SpecialPos[1],0,ACC,DSS_FLAG + flag.free_kick),
	 Middle   = task.goCmuRush(AssiserPos[3],0,ACC,DSS_FLAG + flag.free_kick),
	 Defender = task.goCmuRush(LeaderPos[1] ,0,ACC,DSS_FLAG + flag.free_kick),
     Fronter  = task.goCmuRush(Defence_Pos,player.toPointDir(ToGoalPoint[2]),_,DSS_FLAG + flag.free_kick),
     Kicker   = task.goCmuRush(backPos[1] ,0,ACC,DSS_FLAG + flag.free_kick),
     Center   = task.goCmuRush(backPos[2] ,0,ACC,DSS_FLAG + flag.free_kick),
     Breaker  = task.wback(2,1),
     Receiver = task.wback(2,2),
	 Goalie   = task.goalie(),
	 match = "[L][BR][ASMD][FKC]"
},  


name= "Ref_FrontKickV1102",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}