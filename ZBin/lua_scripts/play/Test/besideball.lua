--边线球接球机器人移位接球射门
local f = flag.dribbling+flag.our_ball_placement
local DSS_FLAG = flag.allow_dss + flag.dodge_ball+flag.dribbling

local idir1 = function(runner)
	local dir1 = Utils.Normalize((ball.pos() - player.pos(runner)):dir())
	return dir1
end

local idir = function()
    return (player.pos("a") - ball.pos()):dir()
end

local idir2 = function()
    return player.toPlayerDir("Leader","a")
end


local standpos = function()
	local pos
	local line1 = CGeoLine:new_local(ball.pos(),ball.velDir())
	local line2 = CGeoLine:new_local(player.pos("a"),Utils.Normalize(ball.velDir() + math.pi/2))
	local line3 = CGeoLineLineIntersection:new_local(line2,line1)
	debugEngine:gui_debug_msg(CGeoPoint:new_local(0,1000),1)
	if ball.velMod()>500 then
	 pos = line3:IntersectPoint()
	else
	 pos = player.pos("a")
	end
	return pos
end

local shootGen = function(dist)
	return function()
		local goalPos = CGeoPoint(param.pitchLength/2,0)
		local pos = ball.pos() + Utils.Polar2Vector(dist,(ball.pos() - goalPos):dir())
		return pos
	end
end

local findgoalie = function()
	for i = 0,param.maxPlayer-1 do
	    if enemy.valid(i) then
	       if enemy.isGoalie(i) then
	       	return i
	       end
	   end
	end
	return -1
end

local todir = function()
	local goalPos   = CGeoPoint(param.pitchLength/2,0)
	local upgoalPos = CGeoPoint(param.pitchLength/2,param.goalWidth/2 - 100)
	local down      = CGeoPoint(param.pitchLength/2,-param.goalWidth/2 + 100)
	local upgoalPos1 = CGeoPoint(param.pitchLength/2,param.goalWidth/2 - 75)
	local down1      = CGeoPoint(param.pitchLength/2,-param.goalWidth/2 + 75)
	local upgoalPos2 = CGeoPoint(param.pitchLength/2,param.goalWidth/2 - 25)
	local down2      = CGeoPoint(param.pitchLength/2,-param.goalWidth/2 + 25)
	local dir
	local upgoal   = CGeoPoint(param.pitchLength/2,param.goalWidth/2)
	local downgoal = CGeoPoint(param.pitchLength/2,-param.goalWidth/2)
	local x = findgoalie()
	local n = player.canFlatPassToPos("a",upgoalPos2)--修改点2021.10.30
	local m = player.canFlatPassToPos("a",down2)--修改点2021.10.30
	if x==-1 then
	   dir = (goalPos - player.pos("a")):dir()
	else
		if enemy.posY(x) > 0 then
			if (enemy.pos(x) - upgoal):mod() > 140 and n then--修改点2021.10.30
			   dir =  (upgoal - player.pos("a")):dir()
			else
		       dir = (down - player.pos("a")):dir()
		    end
		else
			if (enemy.pos(x) - downgoal):mod() > 140 and m then--修改点2021.10.30
			   dir =  (downgoal - player.pos("a")):dir()
			else
		       dir = (upgoalPos - player.pos("a")):dir()
		    end
		end
    end
	return dir
end



local playerPos = function(num)
	return function()
		return player.pos(num)
	end
end

local shootGen1 = function(dist,pos)
	    return function()
	    	ipos = pos 
	    	if type(pos) == "function" then 
	    		ipos = pos()
	    	else
	    		ipos = pos
	    	end
		        local x = ball.pos()
	          local pos1 = ball.pos() + Utils.Polar2Vector(dist,(x - ipos):dir())
	          return pos1
	     end
end


local pos2 = function(dist)
	return function()
	     local dir1 = (ball.pos() - pos.theirGoal()):dir()
	     local pos
	     pos = ball.pos() + Utils.Polar2Vector(dist,dir1)
	     return pos
    end
end

local pos1 = function()
	local pos
	if ball.posY() > 0 then
		pos = CGeoPoint:new_local(300,-1500)--适应大场时需要修改
	else
		pos = CGeoPoint:new_local(300,1500)
	end
	return pos
end

local pos3 = function()
	local pos
	if ball.posY() > 0 then
		pos = CGeoPoint:new_local(1400,-1500)--适应大场时需要修改
	else
		pos = CGeoPoint:new_local(1400,1500)
	end
	return pos
end

local pos4 = function()
	return function()
	local pos
	if ball.posY() > 0 then
		pos = CGeoPoint:new_local(1400,-1500)--适应大场时需要修改
	else
		pos = CGeoPoint:new_local(1400,1500)
	end
	return pos
end
end

local idir3 = function()
	return (pos3()-ball.pos()):dir()
end

local pospos = function()
	local pos
	if ball.posY() > 0 then
		pos = CGeoPoint:new_local(2000,-1200)--适应大场时需要修改
	else
		pos = CGeoPoint:new_local(2000,1200)
	end
	return pos
end

local ddir = function()
	--return function()
	local x = pos1() + Utils.Polar2Vector(param.playerFrontToCenter,player.dir("a"))
		return player.toPointDir(x,"Leader")
	--end
end

local dir11 = function()--到接球机器人吸球嘴的距离
	local dir1 = player.dir("a")
	local pos  = player.pos("a") + Utils.Polar2Vector(param.playerFrontToCenter,dir1)
	local toposdir 
	toposdir = (pos - player.pos("Leader")):dir()
	return toposdir
end

local standpos1 = function()
	local pos
	local line1 = CGeoLine:new_local(ball.pos(),ball.velDir())
	local line2 = CGeoLine:new_local(player.pos("a"),ball.velDir()+math.pi/2)
	local line3 = CGeoLineLineIntersection:new_local(line2,line1)
	debugEngine:gui_debug_msg(CGeoPoint:new_local(0,1000),1)
	if ball.velMod()>500 then
	 pos = line3:IntersectPoint() + Utils.Polar2Vector(param.playerFrontToCenter,Utils.Normalize(math.pi + player.dir("a")))
	else
	 pos = player.pos("a")
	end
	return pos
end

gPlayTable.CreatePlay{

firstState = "t0",

["t0"] = {
	switch = function()
		return "t"
	end,
	["Leader"] = task.goCmuRush(ball.pos),
	["a"]      = task.goCmuRush(CGeoPoint:new_local(0,0)),
	match = "{La}"
},

["t"] = {
	switch = function()
		--local x1 = player.num("a")
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x1)
		if player.toTargetDist("Leader") < 50 and player.toTargetDist("a") < 50 then
			return "t1" 
		end
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),0)
	end,
	["Leader"] = task.goCmuRush(shootGen1(200,pos4()),idir3,_,DSS_FLAG),
	["a"]      = task.goCmuRush(pos1,dir.playerToBall,_,f),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = "{Labc}"
},

["t1"] = {
	switch = function()
		if bufcnt(player.toBallDist("Leader") < 105 and math.abs(player.toBallDir("Leader") - player.dir("Leader"))<math.pi/5,10) and player.toTargetDist("a") < 50 then
			return "t2"
		end
	end,
	["Leader"] = task.goCmuRush(shootGen1(50,pos4()),idir3,_,f),
	["a"] = task.goCmuRush(pos3,dir.playerToBall,_,f),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["t2"] = {
	switch = function()
	     if math.abs(ddir() - player.dir("Leader")) < math.pi/2 and player.canDirectShoot("Leader",6000,120) then
	     	return "t3"
	     end
	     if math.abs(ddir() - player.dir("Leader")) < math.pi/45 and not player.canDirectShoot("Leader",6000,120) then
	     	return "tt"
	     end
	end,
	["Leader"] = task.goCmuRush(player.pos,idir3,_,f),
	["a"]      = task.goCmuRush(pos3,dir.playerToBall,_,f),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["t3"] = {
	switch = function()
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),idir2())
		if player.kickBall("Leader") or ball.velMod()>1000 then
			return "t4"
		end
	end,
	["Leader"] = task.shoot(shootGen1(60,pos4()),idir2,_,500),
	["a"] = task.goCmuRush(standpos,dir.playerToBall,_,f),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["tt"] = {
	switch = function()
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),chiporkick())
		if player.kickBall("Leader") or ball.velMod()>1000 then
			return "t4"
		end
	end,
	["Leader"] = task.shoot(shootGen1(60,pos4()),dir11,kick.chip,6500),
	["a"] = task.goCmuRush(standpos,dir.playerToBall,_,f),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["t4"] = {
	switch = function()
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),chiporkick())
		if player.toBallDist("a") < 105 then
			return "t5"
		end
	end,
	["Leader"] = task.stop(),
	["a"] = task.goCmuRush(standpos,dir.playerToBall,_,f),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["t5"] = {
	switch = function()
		if bufcnt(math.abs(todir() - player.dir("a")) < math.pi/90 and (player.pos("a") - ball.pos()):mod() < 105  and math.abs(dir.playerToBall("a") - player.dir("a")) < math.pi/4.0,5) then 
			return "t6"
		end
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),chiporkick())
	end,
	["a"] = task.goCmuRush(shootGen(50),todir,_,f),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["t6"] = {
	switch = function()
	end,
	["a"] = task.shoot(shootGen(50),todir,_,650),
	["Goalie"] = task.goalie(),
    match = ""
},




name= "besideball",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}