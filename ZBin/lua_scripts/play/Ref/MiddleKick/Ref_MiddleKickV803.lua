--边线球接球机器人移位接球射门
local f = flag.dribbling + flag.allow_dss
local DSS_FLAG = flag.dodge_ball + flag.allow_dss
local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth
local ACC = 5000
local PASS_ACC = 2000
local shootpower = 8000
local directpower = 8000
local time = 750
local tobaldist = 120 --用于球速小于500，接球车力求距离判断
local isDnamic = false --true 为开启横向带球 对付强队用

local ToGoalPoint = {
  CGeoPoint:new_local(pl/2,0),
  ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2-100)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2-100)),
}

local AssiserPos = {
	ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/8,param.pitchWidth/4+600)),
	ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/4 + 500,param.pitchWidth/4-100)),
	CGeoPoint:new_local(param.pitchLength/4,0),
	CGeoPoint:new_local(param.pitchLength/4,param.pitchWidth/2-300),
}

local SpecialPos = {
	ball.refSyntYPos(CGeoPoint:new_local(param.pitchLength/8,param.pitchWidth/4+600)),
	ball.refSyntYPos(CGeoPoint:new_local(param.pitchLength/4 ,param.pitchWidth/4-100)),
	CGeoPoint:new_local(param.pitchLength/4,0),
	CGeoPoint:new_local(param.pitchLength/4,param.pitchWidth/2-300),
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
	 Special  = task.goCmuRush(SpecialPos[2],0,ACC,DSS_FLAG + flag.free_kick),
	 Middle   = task.goCmuRush(SpecialPos[3],0,ACC,DSS_FLAG + flag.free_kick),
	 Defender = task.wback(3,1),
     Breaker  = task.wback(3,2),
     Receiver = task.wback(3,3),
	 Goalie   = task.goalie(),
	 match = "[L][BDR][MAS]"
},



name= "Ref_MiddleKickV803",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}