--经典挑传接球射门
local f = flag.dribbling+flag.our_ball_placement

local idir1 = function(runner)
	local dir1 = Utils.Normalize((ball.pos() - player.pos(runner)):dir())
	return dir1
end

local idir = function()
    return player.toPlayerDir("Leader","Assister")
end

local standpos = function()
	local pos
	local line1 = CGeoLine:new_local(ball.pos(),ball.velDir())
	local line2 = CGeoLine:new_local(player.pos("Assister"),Utils.Normalize(ball.velDir() + math.pi/2))
	local line3 = CGeoLineLineIntersection:new_local(line2,line1)
	debugEngine:gui_debug_msg(CGeoPoint:new_local(0,1000),1)
	if ball.velMod()>500 then
	 pos = line3:IntersectPoint()
	else
	 pos = player.pos("Assister")
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

local shootGen1 = function(dist,pos2,pos3)
	return function()
	     local pos1 = ball.pos() + Utils.Polar2Vector(dist,(pos3 - pos2):dir())
	     return pos1
    end
end

local pos1 = function()
     return player.num("Assister")
end

gPlayTable.CreatePlay{

firstState = "t",

["t"] = {
	switch = function()
	  --idir = (ball.pos() - player.pos("Assister")):dir()
		if bufcnt((player.pos("Leader") - ball.pos()):mod() < 100 and math.abs((dir.playerToBall("Leader") - (ball.pos()-player.pos("Leader")):dir())) < math.pi/5.0,40) and player.toTargetDist("Assister")<150 then
	        return "t1"
	    end
	end,
	["Assister"] = task.goCmuRush(CGeoPoint:new_local(1900,-1200),idir1),
	["Leader"]   = task.naqiu(),
	match = "{AL}"
},

["t1"] = {
	switch = function()
		--player.toPlayerDir("Leader","Assister")
		  debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),idir())
          if math.abs(player.toPlayerDir("Leader","Assister") - player.dir("Leader")) < 0.2 then
          	return "t2"
          end
    end,
    ["Assister"] = task.goCmuRush(CGeoPoint:new_local(1900,-1200),idir1),
    ["Leader"]   = task.goCmuRush(shootGen1(50,player.pos("Assister"),player.pos("Leader")),idir,_,f),
    match = ""
},

["t2"] = {
	switch = function()
		local x= pos1()
		if player.kickBall("Leader") then
			return "t6"
		end
	end,
	["Leader"]   = task.shoot(shootGen1(50,player.pos("Assister"),player.pos("Leader")),idir,kick.chip,8000),
	["Assister"] = task.goCmuRush(standpos,dir.playerToBall,_,f),
	match = ""
},

["t6"] = {
	switch = function()
		local x= pos1()
		if bufcnt((player.pos("Assister") - ball.pos()):mod()<130,10) then
			return "t3"
		end
	end,
	["Leader"]   = task.stop(),
	["Assister"] = task.goCmuRush(standpos,dir.playerToBall,_,f),
	match = ""
},

["t3"] = {
	switch = function()
		if math.abs(player.toTheirGoalDir("Assister") - player.dir("Assister")) < 0.2 and (player.pos("Assister") - ball.pos()):mod() < 105  and math.abs((dir.playerToBall("Assister") - (ball.pos()-player.pos("Assister")):dir())) < math.pi/4.0 then
          	return "t4"
         end
         if(bufcnt(true,50)) then
         	return "naqiu"
         end
	end,
	["Assister"] = task.goCmuRush(shootGen(70),player.toTheirGoalDir,_,f),
	["Leader"] = task.stop(),
	match = ""
},
["naqiu"] = {
	switch = function()
	  --idir = (ball.pos() - player.pos("Assister")):dir()
		if (player.pos("Assister") - ball.pos()):mod() < 115 and math.abs((dir.playerToBall("Assister") - (ball.pos()-player.pos("Assister")):dir())) < math.pi/4.0  then
	        return "t4"
	    end
	end,
	["Assister"]   = task.naqiu(),
	match = ""	
},

["t4"] = {
	switch = function()
	   	if (player.pos("Leader") - ball.pos()):mod() > 115  or math.abs((dir.playerToBall("Assister") - (ball.pos()-player.pos("Assister")):dir())) > math.pi/4.0 then
       	   return "naqiu"
        end
	end,
	["Assister"] = task.shoot(shootGen(50),_,_,600),
	["Leader"] = task.stop(),
	match = ""
},

["t5"] = {
	switch = function()
	end,
	["Assister"] = task.stop(),
	["Leader"] = task.stop(),
	match = ""
},

name= "testt",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}