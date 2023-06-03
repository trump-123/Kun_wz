local STOP_FLAG  = bit:_or(flag.slowly, flag.dodge_ball)
local STOP_DSS   = bit:_or(bit:_or(STOP_FLAG, flag.allow_dss),flag.avoid_stop_ball_circle)
local ACC = 2000
local RECEIVE_POS   = function ()
	if ball.posX() < 0 then
		return ball.refSyntYPos(CGeoPoint:new_local(ball.posX(), param.pitchWidth/3-200))()
	else
		return ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/6, param.pitchWidth/12))()
	end
end

local SIDE_POS,MIDDLE_POS,INTER_POS,SIDE2_POS,INTER2_POS = pos.refStopAroundBall()
--local MIDDLE_POS = CGeoPoint:new_local(0,0)
-- local KICK_DIR  = ball.antiYDir(1.57)

local OPPOSITE_SIDE_POS = function()
  local factor = ball.antiY()
  return CGeoPoint:new_local(INTER2_POS():x(), factor * param.pitchWidth * 0.3)
  --return CGeoPoint:new_local(ball.posX() - 50*param.lengthRatio, 160*factor*param.widthRatio)
end

local LEAER_POS = function(role)
      return ball.pos() + Utils.Polar2Vector(600,(pos.theirGoal() - ball.pos()):dir())
end

function getAttackerNum(i)
    return function()
        return defenceSquence:getAttackNum(i)
    end
end

local STOP_OTHER_PLAYER_POS = function(index)
	return function()
	  	return CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + param.playerRadius * 1.5 ,-param.penaltyWidth/2) + Utils.Polar2Vector(param.penaltyWidth/2*index,math.pi/2)
	end
end

function getFrontAttackerNum(i)
    return function()
        return defenceSquence:getFrontAttackNum(i)
    end
end

local Defence_Pos = function()
	  if ball.posY() < 0 then
 	     return CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth - param.playerRadius*1.5,param.penaltyWidth/2+param.playerRadius)
 	  else
 		 return CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth - param.playerRadius*1.5,-param.penaltyWidth/2-param.playerRadius)
 	  end
end

local STOP_FLAG = flag.avoid_stop_ball_circle + flag.allow_dss

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
			-- debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-1000),player.velMod("Assister"))
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
	end,
	Leader   = task.goCmuRush(LEAER_POS,dir.playerToBall,ACC,STOP_FLAG),
	Assister = task.wback(2,1),
	Special  = task.wback(2,2),
	Middle   = task.goCmuRush(Defence_Pos,player.toBallDir, ACC,STOP_FLAG),
	Defender = task.goCmuRush(STOP_OTHER_PLAYER_POS(0),player.toBallDir, ACC,STOP_FLAG),
	Breaker  = task.goCmuRush(STOP_OTHER_PLAYER_POS(1),player.toBallDir, ACC,STOP_FLAG),
	Receiver = task.goCmuRush(STOP_OTHER_PLAYER_POS(2),player.toBallDir, ACC,STOP_FLAG),
	Goalie   = task.goalie(),
	match    = "[L][AS][DBRM]"
},

["11vs11Solver"] = {
	switch = function()
		if cond.isGameOn() then
			return "exit"
		end
			-- debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-1000),player.velMod("Assister"))
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
	end,
	Leader   = task.goCmuRush(LEAER_POS,dir.playerToBall,ACC,STOP_FLAG),
	Assister = task.wback(2,1),
	Special  = task.wback(2,2),
	Middle   = task.goCmuRush(Defence_Pos,player.toBallDir, ACC,STOP_FLAG),
	Defender = task.goCmuRush(STOP_OTHER_PLAYER_POS(0),player.toBallDir, ACC,STOP_FLAG),
	Breaker  = task.goCmuRush(STOP_OTHER_PLAYER_POS(1),player.toBallDir, ACC,STOP_FLAG),
	Receiver = task.goCmuRush(STOP_OTHER_PLAYER_POS(2),player.toBallDir, ACC,STOP_FLAG),
	Fronter  = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getFrontAttackerNum(0)),
	Kicker   = task.wmarking("First", flag.avoid_stop_ball_circle,getFrontAttackerNum(1)),
	Center   = task.wmarking("Second",flag.avoid_stop_ball_circle,getFrontAttackerNum(2)),
	Goalie   = task.goalie(),
	match    = "[L][AS][FKC][DBRM]"
},

name = "Ref_Stop4CornerDef",
applicable = {
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}