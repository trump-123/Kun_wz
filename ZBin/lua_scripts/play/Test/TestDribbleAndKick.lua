local standPos = CGeoPoint:new_local(0,0)
local shootGen = function(dist)
	return function()
		local goalPos = CGeoPoint(param.pitchLength/2,0)
		local pos = ball.pos() + Utils.Polar2Vector(dist,(ball.pos() - goalPos):dir())
		return pos
	end
end
local DSS_FLAG = flag.allow_dss + flag.dodge_ball

local JUDGE = {
	BallInField = function()
		local x = ball.posX()
		local y = ball.posY()
		local mx = param.pitchLength
		local my = param.pitchWidth
		if not ball.valid() then
			return false
		end
		if x > mx or x < -mx or y > my or y < -my then
			return false
		end
		if math.abs(y) < param.penaltyWidth/2 and x > (param.pitchLength/2 - param.penaltyDepth) then
			return false
		end
		return true
	end,
}
local x= 0
gPlayTable.CreatePlay{

firstState = "run_to_ball",

["run_to_zero"] = {
	switch = function()
		if bufcnt(JUDGE.BallInField(),10) then
			return "run_to_ball"
		end
	end,
	a = task.goCmuRush(standPos, _, _, DSS_FLAG),
	match = "[a]"
},

["run_to_ball"] = {
	switch = function()
		if bufcnt(player.toTargetDist("a")<50,10) then
			return "try_dribble"
		end
		-- if bufcnt(not JUDGE.BallInField(),5) then
		-- 	return "run_to_zero"
		-- end
	end,
	a = task.goCmuRush(shootGen(200), _, _, DSS_FLAG),
	match = "[a]"
},

["try_dribble"] = {
	switch = function()
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),player.infraredCount("a"))
		-- if player.infraredCount("a")>30 then
		-- 	return "try_kick"--"try_keep"
		-- end
		-- if bufcnt(not JUDGE.BallInField(),5) then
		-- 	return "run_to_zero"
		-- end
	end,
	a = task.goCmuRush(shootGen(50),_,_,flag.dribbling + flag.our_ball_placement),
	match = "[a]"
},

["try_keep"] = {
	switch = function()
		if bufcnt(player.infraredCount("a")>1,100) then
			return "try_kick"
		end
		-- if bufcnt(not JUDGE.BallInField(),5) then
		-- 	return "run_to_zero"
		-- end
	end,
	a = task.goCmuRush(shootGen(250),_,_,flag.dribbling + flag.our_ball_placement),
	match = "[a]"
},

["try_kick"] = {
	switch = function()
		local temp1,temp2 = math.modf(ball.velMod()/1000)
		local n = temp1*1000 + math.floor(ball.velMod()/100%10)*100
		if n > x then
			x = n
		end
		debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,0),x)
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),3000*0.136966 + 0.000103831*math.pow(3000,2) + 774.202)
		if player.kickBall("a") then
			--return "run_to_zero"
		end
		if bufcnt(not JUDGE.BallInField(),5) then
			--return "run_to_zero"
		end
	end,
	a = task.shoot(shootGen(50),_,kick.chip,2000),
	--a = task.shoot(_,_,3000),
	match = "[a]"
},

name = "TestDribbleAndKick",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
