--9*6
-- local our_player_stand_pos = {

--       CGeoPoint:new_local(-4000, 1100),
--       CGeoPoint:new_local(-4000,-1100),
--       CGeoPoint:new_local(-3400, 0),
--       CGeoPoint:new_local(-3400,-500),
--       CGeoPoint:new_local(-3400, 500),

-- }

--6*4
local our_player_stand_pos = {

      CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + 500, 1350),
      CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + 500,-1350),
      CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + 500, 0),
      CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + 500,-1000),
      CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + 500, 1000),
      CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + 500,-1700),
      CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + 500, 1700),

}

local DSS_FLAG = flag.allow_dss + flag.dodge_ball --+ flag.our_ball_placement


local f = flag.dribbling --+ flag.our_ball_placement
local catchf = flag.dribbling + flag.our_ball_placement


local kickpos = {
   CGeoPoint:new_local(param.pitchLength/2,-param.goalWidth/2+150),
   CGeoPoint:new_local(param.pitchLength/2,param.goalWidth/2-150)
}
local willshootPos = function()
  local ballPos = ball.pos()
  local idir = (pos.ourGoal() - ballPos):dir()
  local pos = ballPos + Utils.Polar2Vector(350+param.playerFrontToCenter,idir)
  return pos
end

gPlayTable.CreatePlay{

firstState = "WaitStart",

["WaitStart"] = {
  switch = function()
    if cond.isNormalStart() then
      return "PenaltyKick"
    end
    -- if cond.isGameOn() then
    --   return "exit"
    -- end
  end,
  Leader   = task.goCmuRush(our_player_stand_pos[7],0,_,DSS_FLAG),
  Assister = task.goCmuRush(our_player_stand_pos[1],0,_,DSS_FLAG),
  Special  = task.goCmuRush(our_player_stand_pos[2],0,_,DSS_FLAG),
  Defender = task.goCmuRush(our_player_stand_pos[4],0,_,DSS_FLAG),
  Middle   = task.goCmuRush(our_player_stand_pos[5],0,_,DSS_FLAG),
  Breaker  = task.goCmuRush(our_player_stand_pos[6],0,_,DSS_FLAG),
  Receiver = task.goCmuRush(willshootPos,dir.playerToBall,_,DSS_FLAG),
  Goalie   = task.goalie(),
  match = "[LASMDBR]"
},

["PenaltyKick"] = {
  switch = function()
    -- if cond.isGameOn() then
    --  -- return "exit"
    -- end
    -- if  bufcnt( player.kickBall("Leader"), "normal", 750) then
    --   return "exit"
    -- end
  end,
  Leader   = task.goalie(),
  Assister = task.goCmuRush(our_player_stand_pos[1],0,_,DSS_FLAG),
  Special  = task.goCmuRush(our_player_stand_pos[2],0,_,DSS_FLAG),
  Defender = task.goCmuRush(our_player_stand_pos[4],0,_,DSS_FLAG),
  Middle   = task.goCmuRush(our_player_stand_pos[5],0,_,DSS_FLAG),
  Breaker  = task.goCmuRush(our_player_stand_pos[6],0,_,DSS_FLAG),
  Receiver = task.goCmuRush(our_player_stand_pos[7],0,_,DSS_FLAG),
  Goalie   = task.bigpenaltykick(),
  match = "[LASMDBR]"
},



name = "Ref_BIGPenaltyKick_8vs8",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
