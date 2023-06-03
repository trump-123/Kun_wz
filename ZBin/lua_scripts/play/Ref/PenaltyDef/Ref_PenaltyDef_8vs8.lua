local STOP_FLAG  = bit:_or(flag.slowly, flag.dodge_ball)
local STOP_DSS   = bit:_or(STOP_FLAG, flag.allow_dss)

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
local DEF_POS6 = ball.refAntiYPos(CGeoPoint:new_local(DEFX, DEFY-param.playerRadius*6))
local DEF_POS7 = ball.refSyntYPos(CGeoPoint:new_local(DEFX, DEFY-param.playerRadius*6))
local DEF_POS8 = CGeoPoint:new_local(GOALIEX, GOALIEY)


gPlayTable.CreatePlay{

firstState = "Ready",

["Ready"] = {
  switch = function()
    debugEngine:gui_debug_arc(ball.pos(),500,0,360,1)
    if cond.isNormalStart() then
      return "Go"
    end
    -- if cond.isGameOn() then
    --   return "exit"
    -- end
  end,
  Defender = task.goCmuRush(DEF_POS1, 0,  ACC, STOP_DSS),
  Middle   = task.goCmuRush(DEF_POS2, 0,  ACC, STOP_DSS),
  Special  = task.goCmuRush(DEF_POS3, 0,  ACC, STOP_DSS),
  Leader   = task.goCmuRush(DEF_POS4, 0,  ACC, STOP_DSS),
  Assister = task.goCmuRush(DEF_POS5, 0,  ACC, STOP_DSS),
  Breaker  = task.goCmuRush(DEF_POS6, 0,  ACC, STOP_DSS),
  Receiver = task.goCmuRush(DEF_POS7, 0,  ACC, STOP_DSS),
  Goalie   = task.Penaltygoalie(),
  match = "{LASMDBR}"
},

["Go"] = {
  switch = function()
    debugEngine:gui_debug_arc(ball.pos(),500,0,360,1)
    -- if cond.isGameOn() then
    --   return "exit"
    -- end
  end,
  Defender = task.goCmuRush(DEF_POS1, 0,  ACC, STOP_DSS),
  Middle   = task.goCmuRush(DEF_POS2, 0,  ACC, STOP_DSS),
  Special  = task.goCmuRush(DEF_POS3, 0,  ACC, STOP_DSS),
  Leader   = task.goCmuRush(DEF_POS4, 0,  ACC, STOP_DSS),
  Assister = task.goCmuRush(DEF_POS5, 0,  ACC, STOP_DSS),
  Breaker  = task.goCmuRush(DEF_POS6, 0,  ACC, STOP_DSS),
  Receiver = task.goCmuRush(DEF_POS7, 0,  ACC, STOP_DSS),
  Goalie   = task.Penaltygoalie(),
  match = ""
},


name = "Ref_PenaltyDef_8vs8",
applicable = {
  exp = "a",
  a = true
},
attribute = "attack",
timeout = 99999
}