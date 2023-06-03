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

local DSS_FLAG = flag.dodge_ball + flag.allow_dss

gPlayTable.CreatePlay{
firstState = "start",

	switch = function ()
		if cond.isGameOn() then
			return "exit"
		end
	end,

["start"] = {
	Leader   = task.goSpeciPos(MIDDLE_POS,player.toBallDir,DSS_FLAG),
	Special  = task.marking(Leftpos),
	Assister = task.marking(Rightpos),
	Breaker  = task.goSpeciPos(INTER2_POS,player.toBallDir,DSS_FLAG),
	Receiver = task.goSpeciPos(SIDE2_POS,player.toBallDir,DSS_FLAG),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match    = "[L][AS][MD][BR]"
},

name = "Ref_KickOffDef8vs8",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
