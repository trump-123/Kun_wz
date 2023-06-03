local Assisterpos = {
	CGeoPoint:new_local(1300,400),
}

local Leaderpos = {
	CGeoPoint:new_local(2300,-1800),

}

local DODGE = flag.dodge_ball
local f     = flag.dribbling

gPlayTable.CreatePlay{

firstState = "rolematch",

["rolematch"] = {
	switch = function()
		return "gotopos"
	end,
	["Leader"] = task.goCmuRush(ball.pos),
	["Assister"]= task.goCmuRush(Assisterpos[1]),
	match = "{LA}"
},

["gotopos"] = {
	switch = function()
		if bufcnt(player.toTargetDist("Leader") < 50,30) then
			return "getball"
		end
	end,
	["Leader"]   = task.goCmuRush(player.shootGen(200,player.balltoplayer("Assister")),dir.playerToBall,_,DODGE),
	["Assister"] = task.goCmuRush(Assisterpos[1],dir.playerToBall),
	match = "{AL}"
},

["getball"] = {
	switch = function()
		if bufcnt(player.toBallDist("Leader")<105
			and player.infraredCount("Leader")>1
			and player.toTargetDist("Assister") < 100,10) then
			if player.canFlatPassTo("Leader","Assister") then
			   return "flatpass"
			else
			   return "chippass"
			end 
		end
	end,
	["Leader"]   = task.goCmuRush(player.shootGen(90,player.balltoplayer("Assister")),dir.playerToBall,_,f),
	["Assister"] = task.goCmuRush(Assisterpos[1],dir.playerToBall),
	match = "{AL}"
},

["flatpass"] = {
	switch = function()
		if player.kickBall("Leader") then
		   return "receiverball"
		end
	end,
	["Leader"] = task.flatpass(player.shootGen(90,player.balltoplayer("Assister")),"Assister"),
	["Assister"] = task.goCmuRush(Assisterpos[1],dir.playerToBall),
	match = "{AL}"
},

["chippass"] = {
	switch = function()
		if player.kickBall("Leader") then
			return "receiverball"
		end
	end,
	["Leader"] = task.chippass(player.shootGen(90,player.balltoplayer("Assister")),"Assister"),
	["Assister"] = task.goCmuRush(Assisterpos[1],dir.playerToBall),
	match = ""
},

["receiverball"] = {
	switch = function()
		if bufcnt(player.toBallDist("Assister")<105
			and player.infraredCount("Assister")>1,10) then
			return "turn"
		end
	end,
	["Leader"] = task.goCmuRush(Leaderpos[1],player.toShootOrRobot("Assister")),
	["Assister"]= task.goCmuRush(player.standpos("Assister"),dir.playerToBall,_,f),
	match = "{AL}"
},

["turn"] = {
	switch = function()
		if bufcnt(player.canshoot("Assister"),10) then
			return "shoot"
		end
    end,
    ["Leader"] = task.goCmuRush(Leaderpos[1],dir.playerToBall),
    ["Assister"]=task.goCmuRush(player.shootGen(90),player.toTheirGoalDir,_,f),
    match = ""
},

["shoot"] = {
	switch = function()
	end,
	["Leader"] = task.goCmuRush(Leaderpos[1],dir.playerToBall),
	["Assister"] = task.shoot(player.shootGen(90),_,_,8000),
	match = "{LA}"
},

name = "0113",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}