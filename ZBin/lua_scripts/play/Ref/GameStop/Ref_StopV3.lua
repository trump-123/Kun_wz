-- 球在前场中侧的停球站位，尽量分散，避免跑位过程中撞到球
-- by fjm 2022-01-27


local STOP_FLAG  = bit:_or(flag.slowly, flag.dodge_ball)
local STOP_DSS   = bit:_or(bit:_or(STOP_FLAG, flag.allow_dss),flag.avoid_stop_ball_circle)
local OUR_NOT_DSS= bit:_or(flag.not_avoid_our_vehicle,flag.dodge_ball)

local DEFX    = -(param.pitchLength/2 - param.penaltyDepth -3 * param.playerRadius -300)
local DEFY    = param.penaltyWidth/2 + 3 * param.playerRadius - 50
local GOALIEX = -param.pitchLength/2 + param.playerRadius*2
local GOALIEY = 0

local DEF_POS1 = ball.refSyntYPos(CGeoPoint:new_local(DEFX, DEFY))
--local DEF_POS2 = ball.syntYPos(CGeoPoint:new_local(DEFX, DEFY + 3 * param.playerRadius))
local DEF_POS2 = CGeoPoint:new_local(1500, 0)
local DEF_POS3 = ball.refAntiYPos(CGeoPoint:new_local(DEFX, DEFY))
local DEF_POS4 = ball.refSyntYPos(CGeoPoint:new_local(DEFX, DEFY-param.playerRadius*3))
local DEF_POS5 = ball.refAntiYPos(CGeoPoint:new_local(DEFX, DEFY-param.playerRadius*3))
local DEF_POS6 = CGeoPoint:new_local(1500,  param.pitchWidth / 4)
local DEF_POS7 = CGeoPoint:new_local(1500, -param.pitchWidth / 4)
local DEF_POS8 = CGeoPoint:new_local(GOALIEX, GOALIEY)
local DEF_POS9 = ball.refSyntYPos(CGeoPoint:new_local(DEFX, DEFY-param.playerRadius*6))
local DEF_POS10= ball.refAntiYPos(CGeoPoint:new_local(DEFX, DEFY-param.playerRadius*6))
local DEF_POS11= CGeoPoint:new_local(-1500, 0)

local ACC = 2000

function getFrontAttackerNum(i)
    return function()
        return defenceSquence:getFrontAttackNum(i)
    end
end


gPlayTable.CreatePlay{

firstState = "initState",

["initState"] = {
  switch = function()
    if USE_11vs11_SOLVER then
      return "11vs11Solver"
    else
      return "8vs8Solver"
    end
  end,
  match = ""
},

["8vs8Solver"] = {
  switch = function()
    if cond.isGameOn() then
      return "exit"
    end
    -- if player.myvalid("Defender") then
    --   gCurrentPlay = "Ref_StopV3D"
    -- else
      if ball.refPosX() < -param.pitchLength/2 + param.penaltyDepth and math.abs(ball.refPosY()) > param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4CornerDef"
      elseif ball.refPosX() > param.pitchLength/2 - param.penaltyDepth and math.abs(ball.refPosY()) > param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4CornerKick"-- by fjm 2022-03-22
      elseif math.abs(ball.refPosY()) >= param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4SideLine" -- by fjm 2022-03-22
      elseif ball.refPosX() < -(param.pitchLength/2 - param.penaltyDepth - 1000) and -(param.pitchLength/2+400) <= ball.refPosX() and math.abs(ball.refPosY()) <= param.penaltyWidth/2 + 300 then--+ param.pitchWidth/6 then
       gCurrentPlay = "Ref_StopV3"-- by fjm 2022-03s-22
      else
       gCurrentPlay = "Ref_StopV2"
      end    
    --end  
  end,
  Defender = task.goCmuRush(DEF_POS1, 0,  ACC, STOP_DSS),
  Middle   = task.goCmuRush(DEF_POS2, 0,  ACC, STOP_DSS),
  Special  = task.goCmuRush(DEF_POS3, 0,  ACC, STOP_DSS),
  Leader   = task.goCmuRush(DEF_POS4, 0,  ACC, OUR_NOT_DSS),
  Assister = task.goCmuRush(DEF_POS5, 0,  ACC, OUR_NOT_DSS),
  Breaker  = task.goCmuRush(DEF_POS6, 0,  ACC, STOP_DSS),
  Receiver = task.goCmuRush(DEF_POS7, 0,  ACC, STOP_DSS),
  Goalie   = task.goCmuRush(DEF_POS8, 0,  ACC, STOP_DSS),
  match    = "[LDMSABR]"
},

["11vs11Solver"] = {
  switch = function()
    if cond.isGameOn() then
      return "exit"
    end
    -- if player.myvalid("Defender") then
    --   gCurrentPlay = "Ref_StopV3D"
    -- else
      if ball.refPosX() < -param.pitchLength/2 + param.penaltyDepth and math.abs(ball.refPosY()) > param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4CornerDef"
      elseif ball.refPosX() > param.pitchLength/2 - param.penaltyDepth and math.abs(ball.refPosY()) > param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4CornerKick"-- by fjm 2022-03-22
      elseif math.abs(ball.refPosY()) >= param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4SideLine" -- by fjm 2022-03-22
      elseif ball.refPosX() < -(param.pitchLength/2 - param.penaltyDepth - 1000) and -(param.pitchLength/2+400) <= ball.refPosX() and math.abs(ball.refPosY()) <= param.penaltyWidth/2 + 300 then--+ param.pitchWidth/6 then
       gCurrentPlay = "Ref_StopV3"-- by fjm 2022-03s-22
      else
       gCurrentPlay = "Ref_StopV2"
      end    
    --end  
  end,
  Defender = task.goCmuRush(DEF_POS1, 0,  ACC, STOP_DSS),
  Middle   = task.goCmuRush(DEF_POS2, 0,  ACC, STOP_DSS),
  Special  = task.goCmuRush(DEF_POS3, 0,  ACC, STOP_DSS),
  Leader   = task.goCmuRush(DEF_POS4, 0,  ACC, OUR_NOT_DSS),
  Assister = task.goCmuRush(DEF_POS5, 0,  ACC, OUR_NOT_DSS),
  Breaker  = task.goCmuRush(DEF_POS6, 0,  ACC, STOP_DSS),
  Receiver = task.goCmuRush(DEF_POS7, 0,  ACC, STOP_DSS),
  Goalie   = task.goCmuRush(DEF_POS8, 0,  ACC, STOP_DSS),
  Fronter  = task.goCmuRush(DEF_POS9, 0,  ACC, STOP_DSS),
  Kicker   = task.goCmuRush(DEF_POS10,0,  ACC, STOP_DSS),
  Center   = task.goCmuRush(DEF_POS11,0,  ACC, STOP_DSS),
  match    = "[LDMSABRFKC]"
},

name = "Ref_StopV3",
applicable = {
  exp = "a",
  a = true
},
attribute = "attack",
timeout = 99999
}