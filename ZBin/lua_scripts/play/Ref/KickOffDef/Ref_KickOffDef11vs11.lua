local Leftpos = function()
	for i = 0,param.maxPlayer - 1 do
		if enemy.valid(i) then
			if enemy.posX(i) < param.pitchLength/8 then
				if enemy.posY(i) < -500 then
					return enemy.pos(i)
				end
			end
		end
	end
	return CGeoPoint:new_local(150,-param.pitchWidth/4-500)
end

local Rightpos = function()
	for i = 0,param.maxPlayer - 1 do
		if enemy.valid(i) then
			if enemy.posX(i) < param.pitchLength/8 then
				if enemy.posY(i) > 500 then
					return enemy.pos(i)
				end
			end
		end
	end
	return CGeoPoint:new_local(150,param.pitchWidth/4+500)
end

local SIDE_POS,MIDDLE_POS,INTER_POS,SIDE2_POS,INTER2_POS = pos.refStopAroundBall()

local OTHER_POS = function()
	local leftnum = 0
	local rightnum = 0
	for i = 0,param.maxPlayer-1 do
		if enemy.valid(i) then
			if enemy.posY(i) > 0 then
				rightnum = rightnum + 1
			else
				leftnum  = leftnum + 1
			end
		end
	end

	if rightnum > leftnum then
		return CGeoPoint(-500,param.pitchWidth/4)
	else
		return CGeoPoint(-500,-param.pitchWidth/4)
	end
end

local DSS_FLAG = flag.allow_dss

gPlayTable.CreatePlay{
firstState = "start",

	switch = function ()
		if cond.isGameOn() then
			return "exit"
		end
	end,

["start"] = {
	Leader   = task.goSpeciPos(MIDDLE_POS,player.toBallDir, DSS_FLAG),
	Special  = task.marking(Leftpos),
	Assister = task.marking(Rightpos),
	Breaker  = task.goSpeciPos(INTER2_POS,player.toBallDir, DSS_FLAG),
	Receiver = task.goSpeciPos(SIDE2_POS, player.toBallDir, DSS_FLAG),
	Kicker   = task.goSpeciPos(OTHER_POS, player.toBallDir, DSS_FLAG),
	Middle   = task.wback(4,1),
	Defender = task.wback(4,2),
	Center   = task.wback(4,3),
	Fronter  = task.wback(4,4),
	Goalie   = task.goalie(),
	match    = "[L][AS][DCFM][BRK]"
},

name = "Ref_KickOffDef11vs11",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
