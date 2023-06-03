--后场防守，加入Leader有优先级 2022.3.09 范佳明
--模仿德国队伍ER-FORCE 禁区前机器人要求较高
local COR_DEF_POS1 = ball.refAntiYPos(CGeoPoint:new_local(-500,600))
local COR_DEF_POS2 = CGeoPoint:new_local(-param.pitchLength/4,-param.pitchWidth/4+500)
local COR_DEF_POS3 = CGeoPoint:new_local(-param.pitchLength/4,param.pitchWidth/4-500)

local SIDE_POS,MIDDLE_POS,INTER_POS,SIDE2_POS,INTER2_POS = pos.refStopAroundBall()

local Defence_Pos = function()
	  if ball.posY() < 0 then
	 	   return CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + param.playerRadius*1.5,param.penaltyWidth/2+param.playerRadius)
	 	else
	 		 return CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + param.playerRadius*1.5,-param.penaltyWidth/2-param.playerRadius)
	 	end
end

local getAttactNum = function(num)
	return function()
		return defenceSquence:getAttackNum(num)
	end
end

local getFrontAttactNum = function(num)
	return function()
		return defenceSquence:getFrontAttackNum(num)
	end
end

gPlayTable.CreatePlay{

firstState = "beginning",

switch = function()
	--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),defenceSquence:getFrontAttackNum(0))
	--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,100),defenceSquence:getFrontAttackNum(1))
	if gCurrentState == "beginning" and 
		defenceSquence:attackerAmount()  <= 11 and defenceSquence:attackerAmount()  > 0 then
		-- if cond.ourvalidNum() <= 4 and enemy.myattackNum() > 3 then
		-- 	return "attacker3" 
		-- else
			return "attacker"..defenceSquence:attackerAmount() 
		--end
	-- elseif gCurrentState == "norPass" then
	-- 	if bufcnt(ball.velMod() < gNorPass2NorDefBallVel 
	-- 		or not enemy.hasReceiver(), 2) then
	-- 		return "norDef"
	-- 	end
	-- elseif gCurrentState == "norDef" then
	-- 	if bufcnt(cond.canDefenceExit(), 2,60) then
	-- 		return "finish"
	-- 	end
	-- else
	-- 	if bufcnt(cond.canDefenceExit(), 2) then
	-- 		return "finish"
		elseif bufcnt(cond.isGameOn(),1,750) then
			--if cond.canexitDef() then
				return "exit"
			--end
		-- 	return "norPass"
	-- 	end
		else 
			if defenceSquence:attackerAmount()  <= 11 and defenceSquence:attackerAmount()  > 0 then
				return "attacker"..defenceSquence:attackerAmount()
			end
	end
end,

["beginning"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getFrontAttactNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getFrontAttactNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getFrontAttactNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getFrontAttactNum(3)),
	Middle   = task.goSpeciPos(COR_DEF_POS1,player.toBallDir,flag.allow_dss + flag.avoid_stop_ball_circle),
	Defender = task.goSpeciPos(Defence_Pos, player.toBallDir,flag.allow_dss + flag.avoid_stop_ball_circle),
	Breaker  = task.wback(3,1),
	Receiver = task.wback(3,2),
	Center   = task.wback(3,3),
	Goalie   = task.goalie(),
	match    = "[L][RBC][ASFKDM]"
},

["attacker1"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getFrontAttactNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getFrontAttactNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getFrontAttactNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getFrontAttactNum(3)),
	Middle   = task.goSpeciPos(COR_DEF_POS1,player.toBallDir,flag.allow_dss + flag.avoid_stop_ball_circle),
	Defender = task.goSpeciPos(Defence_Pos, player.toBallDir,flag.allow_dss + flag.avoid_stop_ball_circle),
	Breaker  = task.wback(3,1),
	Receiver = task.wback(3,2),
	Center   = task.wback(3,3),
	Goalie   = task.goalie(),
	match    = "[L][RBC][ASFKDM]"
},

["attacker2"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getFrontAttactNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getFrontAttactNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getFrontAttactNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getFrontAttactNum(3)),
	Middle   = task.goSpeciPos(COR_DEF_POS1,player.toBallDir,flag.allow_dss + flag.avoid_stop_ball_circle),
	Defender = task.goSpeciPos(Defence_Pos, player.toBallDir,flag.allow_dss + flag.avoid_stop_ball_circle),
	Breaker  = task.wback(3,1),
	Receiver = task.wback(3,2),
	Center   = task.wback(3,3),
	Goalie   = task.goalie(),
	match    = "[L][RBC][ASFKDM]"
},

["attacker3"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getFrontAttactNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getFrontAttactNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getFrontAttactNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getFrontAttactNum(3)),
	Middle   = task.goSpeciPos(COR_DEF_POS1,player.toBallDir,flag.allow_dss + flag.avoid_stop_ball_circle),
	Defender = task.goSpeciPos(Defence_Pos, player.toBallDir,flag.allow_dss + flag.avoid_stop_ball_circle),
	Breaker  = task.wback(3,1),
	Receiver = task.wback(3,2),
	Center   = task.wback(3,3),
	Goalie   = task.goalie(),
	match    = "[L][RBC][ASFKDM]"
},

["attacker4"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getAttackerNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getAttackerNum(3)),
	Middle   = task.goSpeciPos(COR_DEF_POS1,player.toBallDir,flag.allow_dss + flag.avoid_stop_ball_circle),
	Defender = task.goSpeciPos(Defence_Pos, player.toBallDir,flag.allow_dss + flag.avoid_stop_ball_circle),
	Breaker  = task.wback(3,1),
	Receiver = task.wback(3,2),
	Center   = task.wback(3,3),
	Goalie   = task.goalie(),
	match    = "[L][RBC][ASFKDM]"
},

["attacker5"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getAttackerNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getAttackerNum(3)),
	Middle   = task.wmarking("Fourth",flag.avoid_stop_ball_circle,getAttackerNum(4)),
	Defender = task.goSpeciPos(Defence_Pos, player.toBallDir,flag.allow_dss + flag.avoid_stop_ball_circle),
	Breaker  = task.wback(3,1),
	Receiver = task.wback(3,2),
	Center   = task.wback(3,3),
	Goalie   = task.goalie(),
	match    = "[L][RBC][ASFKMD]"
},

["attacker6"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getAttackerNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getAttackerNum(3)),
	Middle   = task.wmarking("Fourth",flag.avoid_stop_ball_circle,getAttackerNum(4)),
	Defender = task.wmarking("Fifth", flag.avoid_stop_ball_circle,getAttackerNum(5)),
	Breaker  = task.wback(3,1),
	Receiver = task.wback(3,2),
	Center   = task.wback(3,3),
	Goalie   = task.goalie(),
	match    = "[L][RBC][ASFKMD]"
},

["attacker7"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getAttackerNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getAttackerNum(3)),
	Middle   = task.wmarking("Fourth",flag.avoid_stop_ball_circle,getAttackerNum(4)),
	Defender = task.wmarking("Fifth", flag.avoid_stop_ball_circle,getAttackerNum(5)),
	Breaker  = task.wmarking("Sixth", flag.avoid_stop_ball_circle,getAttackerNum(6)),
	Receiver = task.wback(2,1),
	Center   = task.wback(2,2),
	Goalie   = task.goalie(),
	match    = "[L][RC][ASFKMDB]"
},

["attacker8"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getAttackerNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getAttackerNum(3)),
	Middle   = task.wmarking("Fourth",flag.avoid_stop_ball_circle,getAttackerNum(4)),
	Defender = task.wmarking("Fifth", flag.avoid_stop_ball_circle,getAttackerNum(5)),
	Breaker  = task.wmarking("Sixth", flag.avoid_stop_ball_circle,getAttackerNum(6)),
	Receiver = task.wback(2,1),
	Center   = task.wback(2,2),
	Goalie   = task.goalie(),
	match    = "[L][RC][ASFKMDB]"
},

["attacker9"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getAttackerNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getAttackerNum(3)),
	Middle   = task.wmarking("Fourth",flag.avoid_stop_ball_circle,getAttackerNum(4)),
	Defender = task.wmarking("Fifth", flag.avoid_stop_ball_circle,getAttackerNum(5)),
	Breaker  = task.wmarking("Sixth", flag.avoid_stop_ball_circle,getAttackerNum(6)),
	Receiver = task.wback(2,1),
	Center   = task.wback(2,2),
	Goalie   = task.goalie(),
	match    = "[L][RC][ASFKMDB]"
},

["attacker10"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getAttackerNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getAttackerNum(3)),
	Middle   = task.wmarking("Fourth",flag.avoid_stop_ball_circle,getAttackerNum(4)),
	Defender = task.wmarking("Fifth", flag.avoid_stop_ball_circle,getAttackerNum(5)),
	Breaker  = task.wmarking("Sixth", flag.avoid_stop_ball_circle,getAttackerNum(6)),
	Receiver = task.wback(2,1),
	Center   = task.wback(2,2),
	Goalie   = task.goalie(),
	match    = "[L][RC][ASFKMDB]"
},

["attacker11"] = {
	Leader   = task.marking(enemy.nearest1,true,5000),
	Assister = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getAttackerNum(0)),
	Special  = task.wmarking("First", flag.avoid_stop_ball_circle,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",flag.avoid_stop_ball_circle,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", flag.avoid_stop_ball_circle,getAttackerNum(3)),
	Middle   = task.wmarking("Fourth",flag.avoid_stop_ball_circle,getAttackerNum(4)),
	Defender = task.wmarking("Fifth", flag.avoid_stop_ball_circle,getAttackerNum(5)),
	Breaker  = task.wmarking("Sixth", flag.avoid_stop_ball_circle,getAttackerNum(6)),
	Receiver = task.wback(2,1),
	Center   = task.wback(2,2),
	Goalie   = task.goalie(),
	match    = "[L][RC][ASFKMDB]"
},

-- ["norPass"] = {
-- 	Leader   = task.advance(),
-- 	Special  = task.markingFront("First"),
-- 	Middle   = task.markingFront("Second"),
-- 	Defender = task.defendMiddleHead(),
-- 	Assister = task.singleBack(),
-- 	Goalie   = task.goalie(),
-- 	match    = "[L][S][D][MA]"
-- },

-- ["norDef"] = {
-- 	Leader   = task.advance(),
-- 	Special  = task.markingFront("First"),
-- 	Middle   = task.markingFront("Second"),
-- 	Defender = task.defendMiddleHead(),
-- 	Assister = task.singleBack(),
-- 	Goalie   = task.goalie(),
-- 	match    = "[L][S][D][MA]"
-- },

name = "Ref_MiddleDef11vs11",
applicable ={
	exp = "a",
	a   = true
},
attribute = "defense",
timeout   = 99999
}