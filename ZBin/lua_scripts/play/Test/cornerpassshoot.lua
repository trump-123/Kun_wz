--角球往己方半区传球回传射门
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
    return player.toPlayerDir("a","Leader")
end

local idir33 = function()
    return player.toPlayerDir("b","Leader")
end

local idir44 = function()
	return player.toPlayerDir("Leader","a")
end

local idir55 = function()
	return player.toPlayerDir("b","Leader")
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

local receiver = function()
	return function()
		local pos
		if ball.posY() > 0 then
			pos = CGeoPoint:new_local(-1500,1500)
		else
			pos = CGeoPoint:new_local(-1500,-1500)
		end
		return pos
	end
end

local receiver1 = function()
		local pos
		if ball.posY() > 0 then
			pos = CGeoPoint:new_local(-1500,1500)
		else
			pos = CGeoPoint:new_local(-1500,-1500)
		end
		return pos
end

local pos1 = function()
		local pos 
		if ball.posY()>0 then
			pos = CGeoPoint:new_local(2100,-1500)
		else
			pos = CGeoPoint:new_local(2100,1500)
		end
		return pos
end

local pos4 = function()
	return function()
		local pos
		if ball.posY() > 0 then
			pos = CGeoPoint:new_local(-1500,1500)
		else
			pos = CGeoPoint:new_local(-1500,-1500)
		end
		return pos
	end
end

local idir22 = function()
	return (receiver1()-ball.pos()):dir()
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

local idir3 = function()
	return (pos3()-ball.pos()):dir()
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

local standpos4 = function()
	local pos
	local line1 = CGeoLine:new_local(ball.pos(),ball.velDir())
	local line2 = CGeoLine:new_local(player.pos("b"),Utils.Normalize(ball.velDir() + math.pi/2))
	local line3 = CGeoLineLineIntersection:new_local(line2,line1)
	debugEngine:gui_debug_msg(CGeoPoint:new_local(0,1000),1)
	if ball.velMod()>500 then
	 pos = line3:IntersectPoint()
	else
	 pos = player.pos("b")
	end
	return pos
end

local standpos2 = function()
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

local standpos3 = function()
	local pos
	local line1 = CGeoLine:new_local(ball.pos(),ball.velDir())
	local line2 = CGeoLine:new_local(player.pos("Leader"),Utils.Normalize(ball.velDir() + math.pi/2))
	local line3 = CGeoLineLineIntersection:new_local(line2,line1)
	debugEngine:gui_debug_msg(CGeoPoint:new_local(0,1000),1)
	if ball.velMod()>500 then
	 pos = line3:IntersectPoint()
	else
	 pos = player.pos("Leader")
	end
	return pos
end


local leaderpos = function()
	local pos 
	if ball.posY()>0 then
		pos = CGeoPoint:new_local(2300,1500)
	else
		pos = CGeoPoint:new_local(2300,-1500)
	end
	return pos
end

local leaderpos1 = function()
	return function()
		local pos 
		if ball.posY()>0 then
			pos = CGeoPoint:new_local(2300,1500)
		else
			pos = CGeoPoint:new_local(2300,-1500)
		end
		return pos
    end
end


gPlayTable.CreatePlay{

firstState = "t",

["t"] = {
	switch = function()
		if player.toTargetDist("Leader") < 30 and player.toTargetDist("b") < 50 then
			return "t1" 
		end
	end,
	["Leader"] = task.goCmuRush(shootGen1(200,playerPos("b")),idir22,_,DSS_FLAG),
	["a"]      = task.goCmuRush(pos1,dir.playerToBall,_,f),
	["b"]      = task.goCmuRush(receiver1,dir.playerToBall),
	["Goalie"] = task.goalie(),
	match = "{Lb}"
},

["t1"] = {
	switch = function()
		if bufcnt(player.toBallDist("Leader") < 105 and math.abs(player.toBallDir("Leader") - player.dir("Leader"))<math.pi/5,10) and player.toTargetDist("b") < 50 then
			return "t2"
		end
		--if bufcnt(player.toBallDist("Leader") < 105 and math.abs(player.toBallDir("Leader") - player.dir("Leader"))>math.pi/10*3,10) then
		--	return "t"
		--end
	end,
	["Leader"] = task.goCmuRush(shootGen1(50,pos4()),idir22,_,f),
	["a"] = task.goCmuRush(pos1,dir.playerToBall),
	["b"] = task.goCmuRush(receiver1,dir.playerToBall,_,f),
	["Goalie"] = task.goalie(),
	match = ""
},

["t2"] = {
	switch = function()
	     if math.abs((receiver1() - player.pos("Leader")):dir() - player.dir("Leader")) < math.pi/45 and player.canDirectShoot("Leader",6000,120) then
	     	return "t3"
	     end
	     if math.abs((receiver1() - player.pos("Leader")):dir() - player.dir("Leader")) < math.pi/45 and not player.canDirectShoot("Leader",6000,120) then
	     	return "tt"
	     end
	end,
	["Leader"] = task.goCmuRush(player.pos,idir22,_,f),
	["a"]      = task.goCmuRush(pos1,dir.playerToBall),
	["b"]      = task.goCmuRush(standpos4,dir.playerToBall),
	["Goalie"] = task.goalie(),
	match = ""
},

["t3"] = {
	switch = function()
		if player.kickBall("Leader") or ball.velMod()>1000 then
			return "t4"
		end
	end,
	["Leader"] = task.shoot(shootGen1(60,pos4()),idir22,_,300),
	["a"]      = task.goCmuRush(pos1,dir.playerToBall),
	["b"]      = task.goCmuRush(standpos4,dir.playerToBall,_,f),
	["Goalie"] = task.goalie(),
	match = ""
},

["tt"] = {
	switch = function()
		if player.kickBall("Leader") or ball.velMod()>1000 then
			return "t4"
		end
	end,
	["Leader"] = task.shoot(shootGen1(60,pos4()),idir22,kick.chip,6500),
	["a"]      = task.goCmuRush(pos1,dir.playerToBall),
	["b"]      = task.goCmuRush(standpos4,dir.playerToBall,_,f),
	["Goalie"] = task.goalie(),
	match = ""
},

--b接球
["t4"] = {
	switch = function()
		if bufcnt(player.toBallDist("b") < 105,10) then
			return "ttt"
		end
		if bufcnt(player.toBallDir("b")>math.pi/10*3 or player.toBallDist("b")>105,100) then
			return "getball"
		end
	end,
	["Leader"] = task.goCmuRush(player.pos,dir.playerToBall,_,f),
	["a"]      = task.goCmuRush(pos1,dir.playerToBall),
	["b"]      = task.goCmuRush(standpos4,dir.playerToBall,_,f),
	["Goalie"] = task.goalie(),
	match = ""
},


--掉球拿球
["getball"] = {
	switch = function()
		if player.toTargetDist("b") < 30  then
			return "ttt" 
		end
	end,
	["b"]      = task.goCmuRush(shootGen1(200,playerPos("Leader")),idir55,_,DSS_FLAG),
	["a"]      = task.goCmuRush(pos1,dir.playerToBall,_,f),
	["Leader"] = task.goCmuRush(player.pos,dir.playerToBall),
	["Goalie"] = task.goalie(),
	match = "{Lb}"
},

--转向
["ttt"] = {
	switch =function()
	  if math.abs(player.toPlayerDir("b","Leader") - player.dir("b"))<math.pi/45 and math.abs(dir.playerToBall("b") - player.dir("b"))<math.pi/5 and player.toBallDist("b")<105 then
	     return "t7"
	 end
	  if player.toBallDist("b") < 105 and math.abs(player.dir("b") - dir.playerToBall("b"))>math.pi/10*3 then
	  	return "getball"
	  end
	 end,
	["Leader"] = task.goCmuRush(player.pos,dir.playerToBall),--固定点后续需要改进
	["a"]      = task.goCmuRush(pos1,dir.playerToBall),
	["b"]      = task.goCmuRush(shootGen1(50,playerPos("Leader")),idir55,_,f),
	match = ""
},

--b传leader
["t7"] = {
	switch = function()
		if player.kickBall("b") then
			return "t17"
		end
		if bufcnt(player.toBallDist("b")>105 or math.abs(player.toBallDir("b") - player.dir("b"))>math.pi/5,100) then
	  	    return "getball"
	    end
	end,
	["Leader"] = task.goCmuRush(standpos3,dir.playerToBall,_,f),
	["a"]      = task.goCmuRush(pos1,dir.playerToBall),
	["b"]      = task.shoot(shootGen1(50,leaderpos1()),idir33,_,300),
    match = ""
},

--leader捡球
["Leader_getball"] = {
	switch = function()
		if player.toTargetDist("Leader")<30 then
			return "t8"
		end
	end,
	["Leader"] = task.goCmuRush(shootGen1(200,playerPos("a")),idir44,DSS_FLAG),
	["a"]      = task.goCmuRush(pos1,dir.playerToBall,_,f),
	["b"]      = task.goCmuRush(receiver1),
	match = ""
},


--leader接球
["t17"] = {
	switch = function()
		if player.toBallDist("Leader")<105 and math.abs(player.dir("Leader") - dir.playerToBall("Leader"))<math.pi/5 then
			return "t8"
		end
	end,
	["Leader"] = task.goCmuRush(standpos3,dir.playerToBall,_,f),
	["a"]      = task.goCmuRush(pos1,dir.playerToBall),
	["b"]      = task.goCmuRush(receiver1),
	match = ""
},

--leader转向
["t8"] = {
	switch = function()
		if math.abs(player.toPlayerDir("Leader","a") - player.dir("Leader"))<math.pi/45 and math.abs(dir.playerToBall("Leader") - player.dir("Leader"))<math.pi/5 and player.toBallDist("Leader")<105 then
			return "t9"
		end
		if player.toBallDist("Leader")<105 and math.abs(player.dir("Leader") - dir.playerToBall("Leader"))>math.pi/10*3 then
			return "Leader_getball"
		end
	end,
	["Leader"] = task.goCmuRush(shootGen1(50,playerPos("a")),idir44,_,f),
	["a"]      = task.goCmuRush(pos1,dir.playerToBall),
	["b"]      = task.goCmuRush(receiver1),
    match = ""
},

--leader传a
["t9"] = {
    switch = function()
    	if player.kickBall("Leader") then
			return "t111"
		end
		if bufcnt(true,100) then
			return "Leader_getball"
		end
    end,
    ["Leader"] = task.shoot(shootGen1(50,playerPos("a")),idir44,_,300),
    ["a"]      = task.goCmuRush(standpos2,dir.playerToBall,_,f),
	["b"]      = task.goCmuRush(receiver1),
    match = ""
},

["t111"] = {
	switch = function()
		if player.toBallDist("a")<105  then
			return "t5"
		end
	end,
	["a"] = task.goCmuRush(standpos2,dir.playerToBall,_,f),
	match = ""
},

--a拿球
["a_getball"] = {
	switch = function()
		if player.toTargetDist("a") < 30  then
			return "t5" 
		end
	end,
	["a"]      = task.goCmuRush(shootGen(200),todir,_,DSS_FLAG),
	["b"]      = task.goCmuRush(player.pos,dir.playerToBall,_,f),
	["Leader"] = task.goCmuRush(player.pos,dir.playerToBall),
	["Goalie"] = task.goalie(),
	match = "{Lb}"
},


["t5"] = {
	switch = function()
		if bufcnt(math.abs(todir() - player.dir("a")) < math.pi/90 and (player.pos("a") - ball.pos()):mod() < 105  and math.abs(dir.playerToBall("a") - player.dir("a")) < math.pi/4.0,5) then 
			return "t6"
		end
		if player.toBallDist("a")<105 and math.abs(player.dir("a") - dir.playerToBall("a"))>math.pi/10*3 then
			return "a_getball"
		end
	end,
	["a"] = task.goCmuRush(shootGen(50),todir,_,f),
	["Goalie"] = task.goalie(),
	match = ""
},

["t6"] = {
	switch = function()
	end,
	["a"] = task.shoot(shootGen(50),todir,_,650),
	["Goalie"] = task.goalie(),
    match = ""
},




name= "cornerpassshoot",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}