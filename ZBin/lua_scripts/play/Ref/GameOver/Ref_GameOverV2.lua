local center = CGeoPoint:new_local(0,0)

local stoppos = {
	CGeoPoint:new_local(-1000, 100),
	CGeoPoint:new_local(-1000,-100),
	CGeoPoint:new_local(-1000, 300),
	CGeoPoint:new_local(-1000,-300),
	CGeoPoint:new_local(-1000, 500),
	CGeoPoint:new_local(-1000,-500),
	CGeoPoint:new_local(-1000, 700),
	CGeoPoint:new_local(-1000,-700),
}

local standpos = {
	CGeoPoint:new_local(-1000,0),
	CGeoPoint:new_local(1000,0),
	CGeoPoint:new_local(0,0)+Utils.Polar2Vector(1000,math.pi/4),
	CGeoPoint:new_local(0,0)+Utils.Polar2Vector(1000,math.pi/4*2),
	CGeoPoint:new_local(0,0)+Utils.Polar2Vector(1000,math.pi/4*3),
	CGeoPoint:new_local(0,0)+Utils.Polar2Vector(1000,-math.pi/4),
	CGeoPoint:new_local(0,0)+Utils.Polar2Vector(1000,-math.pi/4*2),
	CGeoPoint:new_local(0,0)+Utils.Polar2Vector(1000,-math.pi/4*3),
}

local gatherpos = function(role)
	return function()
		local ipos 
		local idir = (player.pos(role) - center):dir()
		ipos = center + Utils.Polar2Vector(400,idir)
		return ipos
	end
end

local distractpos = function(role)
	return function()
		local ipos 
		local idir = (player.pos(role) - center):dir()
		ipos = center + Utils.Polar2Vector(1000,idir)
		return ipos
	end
end

local angle = 0

local THIH = function(total,my)
	return function()
		return CGeoPoint:new_local(1000 * math.cos(angle + my * math.pi*2/total),1000 * math.sin(angle + my * math.pi*2/total))
	end
end

local count = 0

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
			player.toTargetDist("Center")   <20 and 
			player.toTargetDist("Breaker")  <20 and
			player.toTargetDist("Fronter")
			,20,999) then
			return "turn"
		end
	end,
	Leader   = task.goCmuRush(standpos[1],player.toPointDir(center),_,flag.allow_dss),
	Special  = task.goCmuRush(standpos[2],player.toPointDir(center),_,flag.allow_dss),
	Middle   = task.goCmuRush(standpos[3],player.toPointDir(center),_,flag.allow_dss),
	Defender = task.goCmuRush(standpos[4],player.toPointDir(center),_,flag.allow_dss),
	Assister = task.goCmuRush(standpos[5],player.toPointDir(center),_,flag.allow_dss),
	Center   = task.goCmuRush(standpos[6],player.toPointDir(center),_,flag.allow_dss),
	Breaker  = task.goCmuRush(standpos[7],player.toPointDir(center),_,flag.allow_dss),
	Fronter  = task.goCmuRush(standpos[8],player.toPointDir(center),_,flag.allow_dss),
	match = "{LSADMCBF}"
},

["turn"] = {
	switch = function()
		angle = angle + math.pi*2/(8*60) 
		if bufcnt(true,120) then
			return "gather"
		end
	end,
	Leader   = task.goCmuRush(THIH(8,1),player.toPointDir(center),_,flag.allow_dss),
	Special  = task.goCmuRush(THIH(8,2),player.toPointDir(center),_,flag.allow_dss),
	Middle   = task.goCmuRush(THIH(8,3),player.toPointDir(center),_,flag.allow_dss),
	Defender = task.goCmuRush(THIH(8,4),player.toPointDir(center),_,flag.allow_dss),
	Assister = task.goCmuRush(THIH(8,5),player.toPointDir(center),_,flag.allow_dss),
	Center   = task.goCmuRush(THIH(8,6),player.toPointDir(center),_,flag.allow_dss),
	Breaker  = task.goCmuRush(THIH(8,7),player.toPointDir(center),_,flag.allow_dss),
	Fronter  = task.goCmuRush(THIH(8,8),player.toPointDir(center),_,flag.allow_dss),
	match = "[LSADMCBF]"
},

["gather"] = {
	switch = function()
		if bufcnt(
			player.toTargetDist("Assister") <20 and 
			player.toTargetDist("Leader")   <20 and 
			player.toTargetDist("Special")  <20 and 
			player.toTargetDist("Defender") <20 and 
			player.toTargetDist("Middle")   <20 and 
			player.toTargetDist("Center")   <20 and 
			player.toTargetDist("Breaker")  <20 and
			player.toTargetDist("Fronter")
			,20,999) then
			return "distract"
		end
	end,
	Leader   = task.goCmuRush(gatherpos("Leader"),player.toPointDir(center),_,flag.allow_dss),
	Special  = task.goCmuRush(gatherpos("Special"),player.toPointDir(center),_,flag.allow_dss),
	Middle   = task.goCmuRush(gatherpos("Middle"),player.toPointDir(center),_,flag.allow_dss),
	Defender = task.goCmuRush(gatherpos("Defender"),player.toPointDir(center),_,flag.allow_dss),
	Assister = task.goCmuRush(gatherpos("Assister"),player.toPointDir(center),_,flag.allow_dss),
	Center   = task.goCmuRush(gatherpos("Center"),player.toPointDir(center),_,flag.allow_dss),
	Breaker  = task.goCmuRush(gatherpos("Breaker"),player.toPointDir(center),_,flag.allow_dss),
	Fronter  = task.goCmuRush(gatherpos("Fronter"),player.toPointDir(center),_,flag.allow_dss),
	match = "[LSADMCBF]"
},

["distract"] = {
	switch = function()
		if bufcnt(
			player.toTargetDist("Assister") <20 and 
			player.toTargetDist("Leader")   <20 and 
			player.toTargetDist("Special")  <20 and 
			player.toTargetDist("Defender") <20 and 
			player.toTargetDist("Middle")   <20 and 
			player.toTargetDist("Center")   <20 and 
			player.toTargetDist("Breaker")  <20 and
			player.toTargetDist("Fronter")
			,20,999) then
			count = count+1
			if count ~= 2 then
				return "turn"
			else
				return "stop"
			end
		end
	end,
	Leader   = task.goCmuRush(distractpos("Leader"),player.toPointDir(center),_,flag.allow_dss),
	Special  = task.goCmuRush(distractpos("Special"),player.toPointDir(center),_,flag.allow_dss),
	Middle   = task.goCmuRush(distractpos("Middle"),player.toPointDir(center),_,flag.allow_dss),
	Defender = task.goCmuRush(distractpos("Defender"),player.toPointDir(center),_,flag.allow_dss),
	Assister = task.goCmuRush(distractpos("Assister"),player.toPointDir(center),_,flag.allow_dss),
	Center   = task.goCmuRush(distractpos("Center"),player.toPointDir(center),_,flag.allow_dss),
	Breaker  = task.goCmuRush(distractpos("Breaker"),player.toPointDir(center),_,flag.allow_dss),
	Fronter  = task.goCmuRush(distractpos("Fronter"),player.toPointDir(center),_,flag.allow_dss),
	match = "[LSADMCBF]"
},

["stop"] = {
	switch = function()
	end,
	Leader   = task.goCmuRush(stoppos[1],0,_,flag.allow_dss),
	Special  = task.goCmuRush(stoppos[2],0,_,flag.allow_dss),
	Middle   = task.goCmuRush(stoppos[3],0,_,flag.allow_dss),
	Defender = task.goCmuRush(stoppos[4],0,_,flag.allow_dss),
	Assister = task.goCmuRush(stoppos[5],0,_,flag.allow_dss),
	Center   = task.goCmuRush(stoppos[6],0,_,flag.allow_dss),
	Breaker  = task.goCmuRush(stoppos[7],0,_,flag.allow_dss),
	Fronter  = task.goCmuRush(stoppos[8],0,_,flag.allow_dss),
	match = "[LSADMCBF]"
},

name = "Ref_GameOverV2",
applicable ={
	exp = "a",
	a   = true
},
attribute = "defense",
timeout   = 99999
}