local center = CGeoPoint:new_local(0,0)

local standpos = {
	CGeoPoint:new_local(-700,0),
	CGeoPoint:new_local(700,0),
	CGeoPoint:new_local(0,0)+Utils.Polar2Vector(700,math.pi/3),
	CGeoPoint:new_local(0,0)+Utils.Polar2Vector(700,math.pi/3*2),
	CGeoPoint:new_local(0,0)+Utils.Polar2Vector(700,-math.pi/3),
	CGeoPoint:new_local(0,0)+Utils.Polar2Vector(700,-math.pi/3*2),
}

local stoppos = {
	CGeoPoint:new_local(-700, 100),
	CGeoPoint:new_local(-700,-100),
	CGeoPoint:new_local(-700, 300),
	CGeoPoint:new_local(-700,-300),
	CGeoPoint:new_local(-700, 500),
	CGeoPoint:new_local(-700,-500),
}

local gatherpos = function(role)
	return function()
		local ipos 
		local idir = (player.pos(role) - center):dir()
		ipos = center + Utils.Polar2Vector(200,idir)
		return ipos
	end
end

local distractpos = function(role)
	return function()
		local ipos 
		local idir = (player.pos(role) - center):dir()
		ipos = center + Utils.Polar2Vector(700,idir)
		return ipos
	end
end

local angle = 0

local THIH = function(total,my)
	return function()
		return CGeoPoint:new_local(700 * math.cos(angle + my * math.pi*2/total),700 * math.sin(angle + my * math.pi*2/total))
	end
end

local count = 0

local ACC = 5000

gPlayTable.CreatePlay{
firstState = "move",
["move"] = {
	switch = function()
		if bufcnt(
			player.toTargetDist("Assister") <20 and 
			player.toTargetDist("Leader")   <20 and 
			player.toTargetDist("Special")  <20 and 
			player.toTargetDist("Defender") <20 and 
			player.toTargetDist("Middle")   <20 and 
			player.toTargetDist("Center")   <20 
			,20,999) then
			return "turn"
		end
	end,
	Leader   = task.goCmuRush(standpos[1],player.toPointDir(center),ACC,flag.allow_dss),
	Special  = task.goCmuRush(standpos[2],player.toPointDir(center),ACC,flag.allow_dss),
	Middle   = task.goCmuRush(standpos[3],player.toPointDir(center),ACC,flag.allow_dss),
	Defender = task.goCmuRush(standpos[4],player.toPointDir(center),ACC,flag.allow_dss),
	Assister = task.goCmuRush(standpos[5],player.toPointDir(center),ACC,flag.allow_dss),
	Center   = task.goCmuRush(standpos[6],player.toPointDir(center),ACC,flag.allow_dss),
	match = "[LSAMCD]"
},

["turn"] = {
	switch = function()
		angle = angle + math.pi*2/(10*60) 
		if bufcnt(true,120) then
			return "gather"
		end
	end,
	Leader   = task.goCmuRush(THIH(6,1),player.toPointDir(center),ACC,flag.allow_dss),
	Special  = task.goCmuRush(THIH(6,2),player.toPointDir(center),ACC,flag.allow_dss),
	Middle   = task.goCmuRush(THIH(6,3),player.toPointDir(center),ACC,flag.allow_dss),
	Defender = task.goCmuRush(THIH(6,4),player.toPointDir(center),ACC,flag.allow_dss),
	Assister = task.goCmuRush(THIH(6,5),player.toPointDir(center),ACC,flag.allow_dss),
	Center   = task.goCmuRush(THIH(6,6),player.toPointDir(center),ACC,flag.allow_dss),
	match = "[LSAMCD]"
},

["gather"] = {
	switch = function()
		if bufcnt(
			player.toTargetDist("Assister") <20 and 
			player.toTargetDist("Leader")   <20 and 
			player.toTargetDist("Special")  <20 and 
			player.toTargetDist("Defender") <20 and 
			player.toTargetDist("Middle")   <20 and 
			player.toTargetDist("Center")   <20 
			,20,999) then
			return "distract"
		end
	end,
	Leader   = task.goCmuRush(gatherpos("Leader"),player.toPointDir(center),ACC,flag.allow_dss),
	Special  = task.goCmuRush(gatherpos("Special"),player.toPointDir(center),ACC,flag.allow_dss),
	Middle   = task.goCmuRush(gatherpos("Middle"),player.toPointDir(center),ACC,flag.allow_dss),
	Defender = task.goCmuRush(gatherpos("Defender"),player.toPointDir(center),ACC,flag.allow_dss),
	Assister = task.goCmuRush(gatherpos("Assister"),player.toPointDir(center),ACC,flag.allow_dss),
	Center   = task.goCmuRush(gatherpos("Center"),player.toPointDir(center),ACC,flag.allow_dss),
	match = "[LSAMCD]"
},

["distract"] = {
	switch = function()
		if bufcnt(
			player.toTargetDist("Assister") <20 and 
			player.toTargetDist("Leader")   <20 and 
			player.toTargetDist("Special")  <20 and 
			player.toTargetDist("Defender") <20 and 
			player.toTargetDist("Middle")   <20 and 
			player.toTargetDist("Center")   <20 
			,20,999) then
			count = count+1
			if count~=2 then
				return "turn"
			else
				return "stop"
			end
		end
	end,
	Leader   = task.goCmuRush(distractpos("Leader"),player.toPointDir(center),ACC,flag.allow_dss),
	Special  = task.goCmuRush(distractpos("Special"),player.toPointDir(center),ACC,flag.allow_dss),
	Middle   = task.goCmuRush(distractpos("Middle"),player.toPointDir(center),ACC,flag.allow_dss),
	Defender = task.goCmuRush(distractpos("Defender"),player.toPointDir(center),ACC,flag.allow_dss),
	Assister = task.goCmuRush(distractpos("Assister"),player.toPointDir(center),ACC,flag.allow_dss),
	Center   = task.goCmuRush(distractpos("Center"),player.toPointDir(center),ACC,flag.allow_dss),
	match = "[LSAMCD]"
},

["stop"] = {
	switch = function()
	end,
	Leader   = task.goCmuRush(stoppos[1],0,ACC,flag.allow_dss),
	Special  = task.goCmuRush(stoppos[2],0,ACC,flag.allow_dss),
	Middle   = task.goCmuRush(stoppos[3],0,ACC,flag.allow_dss),
	Defender = task.goCmuRush(stoppos[4],0,ACC,flag.allow_dss),
	Assister = task.goCmuRush(stoppos[5],0,ACC,flag.allow_dss),
	Center   = task.goCmuRush(stoppos[6],0,ACC,flag.allow_dss),
	match = "[LSAMCD]"
},

name = "Ref_GameOverV1",
applicable ={
	exp = "a",
	a   = true
},
attribute = "defense",
timeout   = 99999
}