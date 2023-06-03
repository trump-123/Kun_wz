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


gPlayTable.CreatePlay{
firstState = "start",

	switch = function ()
		if bufcnt(cond.ballMoved(),1) then
			return "exit"
		end
	end,

["start"] = {
	Leader   = task.goSpeciPos(CGeoPoint:new_local(-600, 0)),
	Special  = task.marking(Leftpos),
	Assister = task.marking(Rightpos),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match    = "[L][AS][MD]"
},

name = "Ref_KickOffDef6vs6",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
