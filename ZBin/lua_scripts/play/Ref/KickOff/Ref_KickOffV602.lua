-- 三车上前，打配合，选取第三车或者第四车射门
-- by fjm 2022-02-11
-- PASS1是传禁区对面的那个 2是传离球最近的 3 4依次目的在于取出没被盯的球员

local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth

local f = flag.dribbling + flag.allow_dss
local DSS_FLAG = flag.allow_dss + flag.dodge_ball 
local PASS_ACC = 2000
local shootpower = 6500
local passpower = 300

local LeaderPos  = {
	CGeoPoint:new_local(pl/4,-150),
}

local AssiserPos  = {
	CGeoPoint:new_local(-190,pw/6*2+200),
	CGeoPoint:new_local(pl/4,pw/6*2),
}

local SpecialPos = {
	CGeoPoint:new_local(-190,-pw/6*2-200),
	CGeoPoint:new_local(pl/8,-pw/6*2-200),
	CGeoPoint:new_local(pl/8*3,-pw/10*3),
}

local ToGoalPoint = {
  CGeoPoint:new_local(pl/2,0),
  ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2-100)),
}


local PASS_TASK = function(role1,role2,pos)
	return function()
		if math.abs(player.dir(role1) - player.toPointDir(pos,role1)) < math.pi/8 then
			if player.canFlatPassTo(role1,role2) then
				return task.flatpass(player.shootGen(90,player.balltoplayer(role2)),role2)
			else
				return task.chippass(player.shootGen(90,player.balltoplayer(role2)),role2)
			end
		else
			return task.goCmuRush(player.shootGen(90,player.balltoplayer(role2)),player.toPointDir(pos),_,f)
		end
	end
end



gPlayTable.CreatePlay{

firstState = "start",

["start"] = {
	switch = function()
		if cond.isNormalStart() then
			return "ready"
		end
	end,
	Leader   = task.goCmuRush(player.shootGen(200,math.pi),dir.playerToBall,ACC,DSS_FLAG),
	Assister = task.goCmuRush(AssiserPos[1],0,ACC,DSS_FLAG),
	Special  = task.goCmuRush(SpecialPos[1],0,ACC,DSS_FLAG),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match = "[L][AS][MD]"
},

["ready"] = {
	switch = function()
		if bufcnt(player.kickBall("Leader") or ball.velMod()>1000,1,60*10) then
			return "receive0"
		end
	end,
	Leader   = task.staticGetBall("Assister",kp.toTarget("Assister","Leader")),
	Assister = task.goCmuRush(AssiserPos[1],player.toPlayerDir("Leader"),ACC,DSS_FLAG),
	Special  = task.goCmuRush(SpecialPos[1],0,ACC,DSS_FLAG),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match = "[L][AS][MD]"
},

["receive0"] = {
	switch = function()
		if bufcnt(cond.getBallStatus() == "TheirBall",10) then
			return "exit"
		end
		if bufcnt(player.kickBall("Assister"),1,100) then
			return "receive"
		end
	end,
	Leader   = task.goCmuRush(LeaderPos[1],dir.playerToBall),
	Assister = task.goandTurnKick(SpecialPos[2],_,kp.toTarget(SpecialPos[2],"Assister")),
	Special  = task.goCmuRush(SpecialPos[2],0,ACC,DSS_FLAG),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match = "[L][AS][MD]"
},


["receive"] = {
	switch = function()
		if bufcnt(cond.getBallStatus() == "TheirBall",10) then
			return "exit"
		end
		if bufcnt(player.kickBall("Special"),1) then
			return "exit"
		end
		if bufcnt(player.toBallDist("Special") < 110
			and math.abs(player.dir("Special") - player.toBallDir("Special"))<math.pi/6,10,180) then
			if not (player.canFlatPassToPos("Special",ToGoalPoint[1]) or player.canFlatPassToPos("Special",ToGoalPoint[2])
				or player.canFlatPassToPos("Special",ToGoalPoint[3])) then
			    return "pass3"
		    end
		end
	end,
	Leader   = task.goCmuRush(LeaderPos[1],dir.playerToBall),
	Assister = task.goCmuRush(AssiserPos[2],player.dir,ACC),
	Special  = task.goandTurnKick(pos.theirGoal,_,6000),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match = "[L][AS][MD]"
},

["pass3"] = {
	switch = function()
		if bufcnt(player.kickBall("Special") or ball.velMod()>1000,1,100) then
			return "receive2"
		end
	end,
	Leader   = task.goCmuRush(LeaderPos[1],dir.playerToBall),
	Assister = task.goCmuRush(AssiserPos[2],player.toPlayerDir("Special"),ACC,DSS_FLAG),
	Special  = PASS_TASK("Special","Assister",AssiserPos[2]),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match = "[L][AS][MD]"
},

["receive2"] = {
	switch = function()
		if bufcnt(cond.getBallStatus() == "TheirBall",10) then
			return "exit"
		end
		if bufcnt(player.kickBall("Assister"),1) then
			return "exit"
		end
	end,
	Leader   = task.goCmuRush(SpecialPos[2],dir.playerToBall),
	Assister = task.goandTurnKick(pos.theirGoal,_,6000),
	Special  = task.goCmuRush(SpecialPos[3],0,ACC,DSS_FLAG),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match = "[L][AS][MD]"
},

name = "Ref_KickOffV602",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}