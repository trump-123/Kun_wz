-- 三车上前，打配合，选取第三车或者第四车射门
-- by fjm 2022-02-11
-- PASS1是传禁区对面的那个 2是传离球最近的 3 4依次目的在于取出没被盯的球员

local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth
local PASS_ACC = 2000
local ACC = 5000

local f = flag.dribbling + flag.allow_dss
local DSS_FLAG = flag.allow_dss + flag.dodge_ball 
local shootpower = 6500
local passpower = 300

local LeaderPos  = {
	CGeoPoint:new_local(pl/4,-150),
}

local SpecialPos  = {
	CGeoPoint:new_local(-190,-pw/6*2),
	CGeoPoint:new_local(pl/4,-pw/6*2),
}

local AssiserPos = {
	CGeoPoint:new_local(-700,pw/10*3),
	CGeoPoint:new_local(pl/8,pw/10*3),
	CGeoPoint:new_local(pl/8*3,pw/10*3),
}

local BreakPos = {
	CGeoPoint:new_local(-pl/4,pw/10*3-500),
}

local ReceiverPos = {
	CGeoPoint:new_local(-pl/4,-pw/10*3+500),
}

local DSHOOT_TASK = function(role)
	return function()
		if player.DisMarked(role) then
			return task.shoot(task.shootGen(90),_,kick.chip,6000)
		else
			return task.shoot(task.shootGen(90),_,_,6000)
		end
	end
end


gPlayTable.CreatePlay{

firstState = "start",

["start"] = {
	switch = function()
		if cond.isNormalStart() then
			if cond.ourvalidNum()==2 then
			  return "directshoot"
			else
			  return "ready"
			end
		end
	end,
	Leader   = task.goCmuRush(player.shootGen(200,math.pi),dir.playerToBall,ACC,DSS_FLAG),
	Assister = task.goCmuRush(AssiserPos[1],0,ACC,DSS_FLAG),
	Special  = task.goCmuRush(SpecialPos[1],0,ACC,DSS_FLAG),
	Breaker  = task.goSpeciPos(BreakPos[1],_,DSS_FLAG),
	Receiver = task.goSpeciPos(ReceiverPos[1],_,DSS_FLAG),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match = "[L][AS][MD][BR]"
},

["ready"] = {
	switch = function()
		 if bufcnt(player.kickBall("Leader") or ball.velMod() > 1000,1,60*6) then
		 		return "exit"
		 end
		 -- if bufcnt(cond.getBallStatus() == "TheirBall",10) then
		 -- 	return "exit"
		 -- end
		 if bufcnt(messi:nextState() == "GetBall",10) then
			 return "exit"
		 end
	end,
	Leader   = task.staticGetBall(AssiserPos[1],kp.toTarget(AssiserPos[1],"Leader"),cp.toTarget("Assister")),
	Assister = task.goCmuRush(AssiserPos[1],player.toPlayerDir("Leader"),ACC,DSS_FLAG),
	Special  = task.goCmuRush(SpecialPos[1],0,5000,DSS_FLAG),
	Breaker  = task.goSpeciPos(BreakPos[1],_,DSS_FLAG),
	Receiver = task.goSpeciPos(ReceiverPos[1],_,DSS_FLAG),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match = "[L][AS][MD][BR]"
},


["receive"] = {
	switch = function()
		if bufcnt(messi:nextState() == "GetBall",10) then
			 return "exit"
		end
		if bufcnt(player.kickBall("Assister"),1,100) then
			return "receive2"
		end
		-- if bufcnt(cond.getBallStatus() == "TheirBall",10) then
		--  	return "exit"
		-- end
	end,
	Leader   = task.goCmuRush(LeaderPos[1],dir.playerToBall),
	Assister = task.goandTurnKick(LeaderPos[1],_,kp.toTarget(LeaderPos[1],"Assister")),
	Special  = task.goCmuRush(SpecialPos[2],0,ACC,DSS_FLAG),
	Breaker  = task.goSpeciPos(BreakPos[1]),
	Receiver = task.goSpeciPos(ReceiverPos[1]),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match = "[L][AS][MD][BR]"
},


["receive2"] = {
	switch = function()
		if bufcnt(messi:nextState() == "GetBall",10) then
			 return "exit"
		end
		if bufcnt(player.kickBall("Leader"),1,180) then
			return "exit"
		end
		-- if bufcnt(cond.getBallStatus() == "TheirBall",10) then
		--  	return "exit"
		-- end
	end,
	Leader   = task.goandTurnKick(pos.theirGoal,_,6000),
	Assister = task.goCmuRush(AssiserPos[3],dir.playerToBall),
	Special  = task.goCmuRush(SpecialPos[2],0,ACC,DSS_FLAG),
	Breaker  = task.goSpeciPos(BreakPos[1]),
	Receiver = task.goSpeciPos(ReceiverPos[1]),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match = "[L][AS][MD][BR]"
},


["directshoot"] = {
	switch = function()
		if bufcnt(player.kickBall("Leader"),1,180) then
			return "exit"
		end
	end,
	Leader   = DSHOOT_TASK("Leader"),
	Assister = task.goCmuRush(AssiserPos[3],dir.playerToBall),
	Special  = task.goCmuRush(SpecialPos[2],0,ACC,DSS_FLAG),
	Breaker  = task.goSpeciPos(BreakPos[1]),
	Receiver = task.goSpeciPos(ReceiverPos[1]),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match = "[L][AS][MD][BR]"
},

name = "Ref_KickOffV801",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}