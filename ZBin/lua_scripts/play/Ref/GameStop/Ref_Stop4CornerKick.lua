-- 角球定位球在一开始就进行站位
-- by fjm 2022-01-27

local STOP_FLAG  = bit:_or(flag.slowly, flag.dodge_ball)
local STOP_DSS   = bit:_or(bit:_or(STOP_FLAG, flag.allow_dss),flag.avoid_stop_ball_circle)
local KICK_POS   = function ()
	return ball.pos() + Utils.Polar2Vector(700,3.14)
end

--local KICK_DIR  = ball.antiYDir(1.57)

local MIDDLE_POS = ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/4 + 200,param.pitchWidth/12))

local FRONT_POS1 = ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/2 - param.penaltyDepth*0.8, param.penaltyWidth / 2 + param.playerRadius + 300))
local FRONT_POS2 = ball.refAntiYPos(CGeoPoint:new_local(250, 75))

local BACK_POS1  = ball.refSyntYPos(CGeoPoint:new_local(-param.pitchLength/8, param.pitchWidth/3))
local BACK_POS2  = ball.refAntiYPos(CGeoPoint:new_local(-param.pitchLength/8+300, param.pitchWidth/3))

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
	Leader   = task.goCmuRush(KICK_POS, dir.playerToBall, ACC, STOP_DSS),
	Assister = task.goCmuRush(FRONT_POS1, _, ACC, STOP_DSS),
	Special  = task.goCmuRush(MIDDLE_POS, _, ACC, STOP_DSS),
	Middle   = task.goCmuRush(BACK_POS1 , _, ACC, STOP_DSS),
	Defender = task.goCmuRush(BACK_POS2 , _, ACC, STOP_DSS),
	Breaker  = task.wback(2,1),
	Receiver = task.wback(2,2),
	Goalie   = task.goalie(),
	match    = "[L][BR][ASMD]"
},

["11vs11Solver"] = {
	switch = function()
		if cond.isGameOn() then
			return "exit"
		end

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
	Leader   = task.goCmuRush(KICK_POS, dir.playerToBall, ACC, STOP_DSS),
	Assister = task.goCmuRush(FRONT_POS1, _, ACC, STOP_DSS),
	Special  = task.goCmuRush(MIDDLE_POS, _, ACC, STOP_DSS),
	Middle   = task.goCmuRush(BACK_POS1 , _, ACC, STOP_DSS),
	Defender = task.goCmuRush(BACK_POS2 , _, ACC, STOP_DSS),
	Breaker  = task.wback(2,1),
	Receiver = task.wback(2,2),
	Fronter  = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getFrontAttackerNum(0)),
	Kicker   = task.wmarking("First", flag.avoid_stop_ball_circle,getFrontAttackerNum(1)),
	Center   = task.wmarking("Second",flag.avoid_stop_ball_circle,getFrontAttackerNum(2)),
	Goalie   = task.goalie(),
	match    = "[L][BR][FKC][ASMD]"
},

name = "Ref_Stop4CornerKick",
applicable = {
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}