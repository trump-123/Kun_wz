--角球挑射或平射射门
local f = flag.dribbling+flag.our_ball_placement
local DSS_FLAG = flag.allow_dss + flag.dodge_ball

local a_pos_s = {
      CGeoPoint:new_local(1700,1900),
      CGeoPoint:new_local(2000,1900),
}

local l_pos_s = {
      CGeoPoint:new_local(1500,-1900),
      CGeoPoint:new_local(2000,-1900),
}

gPlayTable.CreatePlay{

firstState = "t0",

["t0"] = {
	switch = function()
		return "t"
	end,
	["Leader"] = task.goCmuRush(ball.pos),
	["a"]      = task.goCmuRush(a_pos_s[1]),
	match = "{La}"
},

["t"] = {
	switch = function()
		--local x1 = player.num("a")
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x1)
		if player.toTargetDist("Leader") < 30 then
			return "t1" 
		end
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),0)
	end,
	["Leader"] = task.goCmuRush(player.shootGen(220,player.balltoplayer("a")),dir.playerToBall,10000,DSS_FLAG),
	["a"]      = task.goCmuRush(ball.antiYPos(a_pos_s[1]),player.toShootOrRobot("Leader"),10000,DSS_FLAG),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = "{Labc}"
},

["t1"] = {
	switch = function()
		if player.infraredCount("Leader")>5 and math.abs(player.toBallDir("Leader") - player.dir("Leader")) < math.pi/8  then
			return "t2"
		end
	end,
	["Leader"] = task.goCmuRush(player.shootGen(50,player.balltoplayer("a")),dir.playerToBall,10000,f),
	["a"]      = task.goCmuRush(ball.antiYPos(a_pos_s[2]),player.toShootOrRobot("Leader"),10000),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["t2"] = {
	switch = function()
		--math.abs(player.toTargetDir(a_pos_s[2],"Leader")() - player.dir("Leader")) < math.pi/45 and
	     if math.abs(player.toPlayerDir("Leader","a") - player.dir("Leader")) < math.pi/45 and player.canDirectShoot("Leader",6000,120) and player.toTargetDist("a") < 50  then
	     	return "t3"
	     end
	     if math.abs(player.toPlayerDir("Leader","a") - player.dir("Leader")) < math.pi/45 and not player.canDirectShoot("Leader",6000,120) and player.toTargetDist("a") < 50  then
	     	return "tt"
	     end
	end,
	["Leader"] = task.goCmuRush(player.pos,player.antiYDir(a_pos_s[2]),_,f),
	["a"]      = task.goCmuRush(ball.antiYPos(a_pos_s[2]),player.toShootOrRobot("Leader"),10000),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["t3"] = {
	switch = function()
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),chiporkick())
		if player.kickBall("Leader") or ball.velMod()>1000 then
			return "t4"
		end
	end,
	["Leader"] = task.flatpass(player.shootGen(60,player.balltoplayer("Leader")),"a"),
	["a"]      = task.goCmuRush(ball.antiYPos(a_pos_s[2]),player.todir("a")),
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
	["Leader"] = task.chippass(player.shootGen(60,player.balltoplayer("Leader")),"a"),
	["a"]      = task.goCmuRush(ball.antiYPos(a_pos_s[2]),player.todir("a")),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["t4"] = {
	switch = function()
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),chiporkick())
		if player.toBallDist("a") < 1000 or player.infraredCount("a")>5 then
			return "t5"
		end
	end,
	["Leader"] = task.stop(),
	["a"] = task.shoot(player.standpos("a"),player.todir("a"),_,650),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["t5"] = {

	switch = function()
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),chiporkick())
	end,
	["a"] = task.shoot(player.backballpos("a"),player.todir("a"),_,650),
	["Goalie"] = task.goalie(),
	["b"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},




name= "chipkicktogoal",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}