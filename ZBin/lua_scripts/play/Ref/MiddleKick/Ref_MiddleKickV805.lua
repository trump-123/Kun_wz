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
local shootpower = 6500
local time = 750
local tobaldist = 120 --用于球速小于500，接球车力求距离判断
local jud = true

local First  = ball.refSyntYPos(CGeoPoint:new_local(pl/2-ped-400,pew/2+290))
local Second = ball.refAntiYPos(CGeoPoint:new_local(pl/4,pew/2-300))
local Third  = ball.refAntiYPos(CGeoPoint:new_local(pl/4+300,pew/2+0))
local Fourth = ball.refAntiYPos(CGeoPoint:new_local(pl/4+600,pew/2+300))
local ReFirst  = ball.refSyntYPos(CGeoPoint:new_local(pl/2-ped-700,pew/2+100))

local Real1 = ball.refAntiYPos(CGeoPoint:new_local(790,670))

local ToGoalPoint = {
  CGeoPoint:new_local(pl/2,0),
  ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2-100)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2)),
}

local AssiserPos = {
	ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/8,param.pitchWidth/2-100)),
	ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/4 + 500,param.pitchWidth/2-100)),
	CGeoPoint:new_local(param.pitchLength/4,0),
	CGeoPoint:new_local(param.pitchLength/4,param.pitchWidth/2-300),
}

local SpecialPos = {
	ball.refSyntYPos(CGeoPoint:new_local(param.pitchLength/8,param.pitchWidth/2-100)),
	ball.refSyntYPos(CGeoPoint:new_local(param.pitchLength/4 + 500,param.pitchWidth/2-100)),
	CGeoPoint:new_local(param.pitchLength/4,0),
	CGeoPoint:new_local(param.pitchLength/4,param.pitchWidth/2-300),
}

local movep = {
  ball.refSyntYPos(CGeoPoint:new_local(pl/2-ped-400,pew/2-600)),
  ball.refSyntYPos(CGeoPoint:new_local(pl/2-ped-400,pew/2-600))
}

local movep1 = {
  ball.refAntiYPos(CGeoPoint:new_local(1100,800-900)),
  ball.refAntiYPos(CGeoPoint:new_local(790,670-800))
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
    Breaker  = task.wback(2,1),
    Receiver = task.wback(2,2),
    Goalie   = task.goalie(),
    match    = "[L][BR][ASMD]"
  },



name= "Ref_MiddleKickV805",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}