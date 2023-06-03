--边线球接球机器人移位接球射门
local f = flag.dribbling + flag.allow_dss
local DSS_FLAG = flag.dodge_ball + flag.allow_dss
local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth
local ACC = 5000
local PASS_ACC   = 2000
local shootpower = 6500
local time = 750
local tobaldist = 120 --用于球速小于500，接球车力求距离判断
local jud = true

local First    = ball.refSyntYPos(CGeoPoint:new_local(pl/2-ped-1200,goal/2 + 300))
local Second   = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped - 500,pew/2+300))
local Third    = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped - 800,pew/2-100))
local Fourth   = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped - 1100,pew/2-500))

local ReFirst  = ball.refSyntYPos(CGeoPoint:new_local(pl/2-ped-700,-900))

local Real1 = ball.refAntiYPos(CGeoPoint:new_local(2400,670))

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



gPlayTable.CreatePlay{

  firstState = "start",

  ["start"] = {
    switch = function ()
        --debugEngine:gui_debug_msg(CGeoPoint(0,0),getPassVel()())
        debugEngine:gui_debug_x(getPassPos()())
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
    Assister = task.goCmuRush(First ,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG),
    Special  = task.goCmuRush(Second,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG),
    Middle   = task.goCmuRush(Third ,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG),
    Defender = task.goCmuRush(Fourth,player.toPointDir(ToGoalPoint[2]),_,DSS_FLAG),
    Breaker  = task.wback(2,1),
    Receiver = task.wback(2,2),
    Goalie   = task.goalie(),
    match    = "[L][BR][ASMD]"
  },



name= "Ref_CornerKickV810",
applicable={
  exp = "a",
  a = true
},
attribute = "attack",
timeout = 99999
}