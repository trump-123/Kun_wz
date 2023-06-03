local isUsePass = false
local f = flag.dribbling 
local DSS_FLAG = flag.dodge_ball + flag.allow_dss
local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth

local FAKE_DEFEND_POS = ball.antiYPos(CGeoPoint:new_local(250,50))
local FIRST_DEFEND_POS1 = ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/2-param.penaltyDepth-500,param.pitchWidth/4-1000))
local FIRST_DEFEND_POS2 = ball.refSyntYPos(CGeoPoint:new_local(param.pitchLength/2-param.penaltyDepth+300,param.penaltyWidth/2+300))
local FIRST_DEFEND_POS3 = ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/4-500,param.pitchWidth/2-1500))
local FIRST_DEFEND_POS4 = ball.refSyntYPos(CGeoPoint:new_local(param.pitchLength/4-500,param.pitchWidth/2-1500))
local BACK_STAND_POS1   = ball.refSyntYPos(CGeoPoint:new_local(300,param.pitchWidth/2-500))
local BACK_STAND_POS2   = ball.refAntiYPos(CGeoPoint:new_local(1000,0))
local BACK_SECOND_POS1   = ball.refSyntYPos(CGeoPoint:new_local(700,param.pitchWidth/2-1000))
local BACK_SECOND_POS2   = ball.refAntiYPos(CGeoPoint:new_local(800,400))
local SECOND_DEFEND_POS1 = ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/2-param.penaltyDepth-1000,param.pitchWidth/2-500))
local SECOND_DEFEND_POS2 = ball.refSyntYPos(CGeoPoint:new_local(param.pitchLength/2-param.penaltyDepth-200,param.penaltyWidth/2+300))
local SECOND_DEFEND_POS3 = ball.refAntiYPos(CGeoPoint:new_local(900,param.pitchWidth/2-900))
	
function getOurNum()
	return vision:getValidNum()
end


local lastCycle = 0

function getTheirNum()
	--debugEngine:gui_debug_msg(CGeoPoint:new_local(2000,0),2)
	return vision:getTheirValidNum()
end

local leaderNum = function()
	 return messi:leaderNum()
end

local receiverNum = function()
	return messi:receiverNum()
end

local LEADERDENCE_TASK = function(task1,task2,role,role1)
  return function()
    local myrole,myrole1
      if role == 1 then
        myrole = "Leader"
      elseif role == 2 then
        myrole = "Assister"
      elseif role == 3 then
        myrole = "Special"
      elseif role == 4 then
        myrole = "Middle"
      elseif role == 5 then
        myrole = "Defender"
      end

      if role1 == 1 then
        myrole1 = "Leader"
      elseif role1 == 2 then
        myrole1 = "Assister"
      elseif role1 == 3 then
        myrole1 = "Special"
      elseif role1 == 4 then
        myrole1 = "Middle"
      elseif role1 == 5 then
        myrole1 = "Defender"
      end

    if player.myvalid(role) and player.toPlayerDist(role,role1) < 300 then
      return task1()
    else
      return task2()
    end
  end
end

function getAttackerNum(i)
    return function()
        return defenceSquence:getAttackNum(i)
    end
end


local iflag = -1 



local next_kicker = -1
local kick_number
local KICK_TASK = function(role1,role2)
	return function()
		local now = vision:getCycle()
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-1000),now-lastCycle)
		--gRoleNum["Leader"] = leaderNum()
  	--gRoleNum["Special"] = receiverNum()
		
	  kick_number = gRoleNum["Leader"]
		 local temp
		
		-- if player.kickBall(kick_number) and next_kicker ~= -1 then
		-- 	 temp = gRoleNum["Leader"]
		-- 	 gRoleNum["Leader"] = gRoleNum[next_kicker]
		-- 	 gRoleNum[next_kicker] = temp
		-- end
		if (player.canFlatPassToPos(kick_number,player.BestShootPoint(kick_number)()) or not player.myvalid(role1) and player.posX(kick_number)>0) or (now - lastCycle)>1 then
			--debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,0),1)
			if now - lastCycle > 85 then
				lastCycle = now
			end
			return task.goandTurnKick(_,kicker_number)
		else
			lastCycle = now
			--debugEngine:gui_debug_msg(CGeoPoint:new_local(2000,0),2)
			if gCurrentFieldArea == "FrontField" then
				if player.posX(role1) > 0 and player.toPlayerDist(kick_number,role1)>1000 and player.myvalid(role1) and (player.canFlatPassTo(role1,kick_number) or not player.myvalid(role2)) then
					  next_kicker = role1
						return task.goandTurnKick(next_kicker,kick_number)
				elseif player.posX(role2) > 0 and player.toPlayerDist(kick_number,role2)>1000 and player.myvalid(role2) then
						next_kicker = role2
						return task.goandTurnKick(next_kicker,kick_number)
				else
					if player.myvalid(role1) then
					  next_kicker = role1
						return task.goandTurnKick(next_kicker,kick_number)
					else
						return task.goandTurnKick(_,kicker_number)
					end
				end
			elseif gCurrentFieldArea == "BackField" then
				if player.posX(role1) > 0 and player.toPlayerDist(kick_number,role1)>1000 and player.myvalid(role1) then
					  next_kicker = role1
						return task.goandTurnKick(next_kicker,kick_number)
				elseif player.posX(role2) > 0 and player.toPlayerDist(kick_number,role2)>1000 and player.myvalid(role2) then
						next_kicker = role2
						return task.goandTurnKick(next_kicker,kick_number)
				else
						if player.myvalid(role1) then
					    next_kicker = role1
						  return task.goandTurnKick(next_kicker,kick_number)
					  else
						  return task.goandTurnKick(_,kicker_number)
					  end
				end
			end
		end
	end
end


function receiverTask() 
	return function()
		if getOurNum() <= 3 and messi:nextState() == "GetBall" then
		  	return task.wmarking("Zero",_,getAttackerNum(0))
		 else
	    	return task.goCmuRush(pos.getReceivePos(),player.toBallDir,_,DSS_FLAG)
		 end
	end
end

local cantshoot = function(role)
	if player.canFlatPassToPos(role,ToGoalPoint[1]) or player.canFlatPassToPos(role,ToGoalPoint[2]) 
		or player.canFlatPassToPos(role,ToGoalPoint[3]) then
		return false
	end
	return true
end	

local ourBallJumpCond = function()
	local state = messi:nextState()
	local attackerAmout = defenceSquence:attackerAmount()
	if attackerAmout < 0 then
		 attackerAmout = 0
	end

	if BUSDEFENSE then
	end

	if getOurNum() == 3 then
  	gRoleNum["Leader"] = leaderNum()
  	return "4car"
  end

  if getOurNum() == 2 then
  	gRoleNum["Leader"] = leaderNum()
  	return "3cardefend"
  end

  if getOurNum() == 1 then
  	gRoleNum["Leader"] = leaderNum()
  	return "2cardefend"
  end

  if state == "Pass" then
  		if attackerAmout > 2 then
  			attackerAmout = 2
  		end
  		local leader = leaderNum()
  		local receiver = receiverNum()
  		if getOurNum() > 3 then
  			if leader ~= receiver and player.valid(receiver) then
  				gRoleNum["Leader"] = leader
  				gRoleNum["Special"]= receiver
  			else
  				gRoleNum["Leader"] = leader
  			end
  		end
  		return "Pass"..attackerAmout

  elseif state == "GetBall"  then 	
  			if attackerAmout > 2 then
  				 attackerAmout = 2
  			end

  			if getOurNum() > 3 then
  				gRoleNum["Leader"] = leaderNum()
  			end
  			return "GetBall"..attackerAmout

  end

end

gPlayTable.CreatePlay{
firstState = "initState",

["initState"] = {
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = task.goandTurnKick(_,"Leader"),
	Assister = task.goCmuRush(FIRST_DEFEND_POS1,dir.playerToBall),
	Special  = task.goCmuRush(FIRST_DEFEND_POS2,dir.playerToBall),
	Middle   = task.wback(2,1),
	Defender = task.wback(2,2),
	Goalie   = task.goalie(),
	match    = "{LASMD}"--{LASMD}
},

["GetBall0"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK("Special","Middle"),
	Assister = task.wback(2,2),
	Special  = receiverTask(),
	Middle   = task.wdrag(FIRST_DEFEND_POS2,FIRST_DEFEND_POS4),
	Defender = task.wback(2,1),
	Goalie   = task.goalie(),
	match    = function()
			 if getTheirNum() <= 4 then
			  	res =  "(D){L}[S](M)(A)"
			 else
			  	if getOurNum() <= 3 then
			   		res = "(DA){L}[S][M]"
			  	else
			  		res = "{L}[DA][S][M]"
			  	end
		   end 
		   return res
	end,
},

["GetBall1"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK("Special","Middle"),
	Assister = task.wback(2,2),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",_,getAttackerNum(0)),
	Defender = task.wback(2,1),
	Goalie   = task.goalie(),
	match    = function()
			if getTheirNum() <= 4 then
			  	res =  "(D){L}[S](M)(A)"
			else
			  	if getOurNum() <= 3 then
			   		 res = "(DA){L}[S][M]"
			  	else
			  		 res = "{L}[DA][M][S]"
			  	end
		  end 
			return res
	end,
},

["GetBall2"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK("Special","Middle"),
	Assister = task.wback(2,2),
	Special  = task.wmarking("First",_,getAttackerNum(1)),
	Middle   = task.wmarking("Zero",_,getAttackerNum(0)),
	Defender = task.wback(2,1),
	Goalie   = task.goalie(),
	match    = function()
			 if getTheirNum() <= 4 then
			  	res =  "(D){L}[S](M)(A)"
			 else
			  	if getOurNum() <= 3 then
			   		res = "(DA){L}[S][M]"
			  	else
			  		res = "{L}[DA][M][S]"
			  	end
		   end 
			 return res
	end,
},

-- ["GetBall3"]={
-- 	switch = function ()
-- 		return ourBallJumpCond()
-- 	end,
-- 	Leader   = KICK_TASK("Special","Middle"),
-- 	Assister = task.wback(2,2),
-- 	Special  = task.wmarking("First",_,getAttackerNum(1)),
-- 	Middle   = task.wmarking("Zero",_,getAttackerNum(0)),
-- 	Defender = task.wback(2,1),
-- 	Goalie   = task.goalie(),
-- 	match    = function()
-- 				 if getTheirNum() <= 4 then
-- 				  	res =  "(D){L}[S](M)(A)"
-- 				 else
-- 				  	if getOurNum() <= 3 then
-- 				   		res = "(DA){L}[S][M]"
-- 				  	else
-- 				  		res = "{L}[DA][M][S]"
-- 				  	end
-- 			   end 
-- 			   return res
-- 	end,
-- },

["Pass0"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK("Special","Middle"),
	Assister = task.wback(2,2),
	Special  = receiverTask(),
	Middle   = task.wdrag(FIRST_DEFEND_POS2,FIRST_DEFEND_POS4),
	Defender = task.wback(2,1),
	Goalie   = task.goalie(),
	match    = function()
			if getTheirNum() <= 4 then
				  res =  "(D){L}[S](M)(A)"
			else
			  	if getOurNum() <= 3 then
			   		 res = "(DA){L}[S][M]"
			  	else
			  		 res = "{L}[DA][S][M]"
			  	end
			end 
			   return res
	end,
},

["Pass1"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK("Special","Middle"),
	Assister = task.wback(2,2),
	Special  = receiverTask(),
	Middle   = task.wdrag(FIRST_DEFEND_POS2,FIRST_DEFEND_POS4),
	Defender = task.wback(2,1),
	Goalie   = task.goalie(),
	match    = function()
				 if getTheirNum() <= 4 then
				  	res =  "(D){L}[S](M)(A)"
				 else
				  	if getOurNum() <= 3 then
				   		res = "(DA){L}[S][M]"
				  	else
				  		res = "{L}[DA][S][M]"
				  	end
			   end 
			   return res
	end,
},

["Pass2"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK("Special","Middle"),
	Assister = task.wback(2,2),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",_,getAttackerNum(0)),
	Defender = task.wback(2,1),
	Goalie   = task.goalie(),
	match    = function()
			if getTheirNum() <= 4 then
				  res =  "(D){L}[S](M)(A)"
			else
				  if getOurNum() <= 3 then
				   	  res = "(DA){L}[S][M]"
				  else
				  		res = "{L}[DA][M][S]"
				  end
			end 
			   return res
	end,
},

["2cardefend"] = {
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader = KICK_TASK("Special","Middle"),
	Goalie   = task.goalie(),
	match = "[L]"
},

["3cardefend"] = {
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK("Special","Middle"),
	Special  = receiverTask(),
	Goalie   = task.goalie(),
	match = "{L}[S]"
},

["4car"] = {
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK("Special","Middle"),
	Special  = receiverTask(),
	Assister = task.wback(1,1),
	Goalie   = task.goalie(),
	match = "{L}(A)[S]"
},



name = "NormalPlayMessi_6vs6",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}