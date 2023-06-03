local isUsePass = false
local f = flag.dribbling 
local DSS_FLAG = flag.dodge_ball + flag.allow_dss
local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth

function getOurNum()
	return vision:getValidNum()
end


function goaliePassPos()
    local gPos = function ()
        return messi:goaliePassPos()
    end
    return gPos
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

function getPassPos()
    local rPos = function ()
        return messi:passPos()
    end
    return rPos
end

function getPassVel()
    local vel = function ()
        if messi:needChip() then 
            return messi:passVel() * 1
        end

        return messi:passVel() * 1
    end
    return vel
end

function getAttackerNum(i)
    return function()
        return defenceSquence:getAttackNum(i)
    end
end


local iflag = -1 



local next_kicker = -1
local kick_number
local KICK_TASK = function()
	return function()
		return task.goandTurnKick(getPassPos(),_,getPassVel())
	end
end


function receiverTask() 
	return function()
		local attackerAmount = defenceSquence:attackerAmount()
		if getOurNum() <= 3 and messi:nextState() == "GetBall" and attackerAmount > 1 then
		  	return task.wmarking("Zero",_,getAttackerNum(0))
		 else
	    	return task.goCmuRush(pos.getReceivePos(),player.toBallDir,_,DSS_FLAG)
		 end
	end
end

function otherTask(defendindex , posindex)
	return function()
		local pri
		if defendindex == 0 then
			pri = "Zero"
		elseif defendindex == 1 then
			pri = "First"
		elseif defendindex == 2 then
			pri = "Second"
		elseif defendindex == 3 then
			pri = "Third"
		else
			pri = "Fourth"
		end

		local Special = function(num)
			return function()
				return num
			end
		end

		local attackerAmout = defenceSquence:attackerAmount()
		if attackerAmout > defendindex + 1 and messi:nextState() == "GetBall" then
		  	return task.wmarking(pri,_,getAttackerNum(Special(defendindex)))
		 else
	    	return task.wdrag(pos.getOtherPos(posindex))
		 end
	end
end


local ourBallJumpCond = function()
	local state = messi:nextState()
	local attackerAmout = defenceSquence:attackerAmount()
	if attackerAmout < 0 then
		 attackerAmout = 0
	end

	if BUSDEFENSE then
	end

	if getOurNum() == 7 then
  	gRoleNum["Leader"] = leaderNum()
  	return "8car"
  end

	if getOurNum() == 6 then
  	gRoleNum["Leader"] = leaderNum()
  	return "7car"
  end

	if getOurNum() == 5 then
  	gRoleNum["Leader"] = leaderNum()
  	return "6car"
  end

	if getOurNum() == 5 then
  	gRoleNum["Leader"] = leaderNum()
  	return "6car"
  end

	if getOurNum() == 4 then
  	gRoleNum["Leader"] = leaderNum()
  	return "5car"
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
  		if attackerAmout > 8 then
  			 attackerAmout = 8
  		end
  		local leader = leaderNum()
  		local receiver = receiverNum()
  		if getOurNum() > 8 then
  			if leader ~= receiver and player.valid(receiver) then
  				gRoleNum["Leader"] = leader
  				gRoleNum["Special"]= receiver
  			else
  				gRoleNum["Leader"] = leader
  			end
  		end
  		return "Pass"..attackerAmout

  elseif state == "GetBall"  then 	
  			if attackerAmout > 8 then
  				 attackerAmout = 8
  			end

  			if getOurNum() > 8 then
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
	Receiver = task.wback(4,2),
	Assister = task.goCmuRush(pos.getOtherPos(0)),
	Special  = task.goCmuRush(pos.getOtherPos(1)),
	Fronter  = task.goCmuRush(pos.getOtherPos(2)),
	Kicker   = task.goCmuRush(pos.getOtherPos(3)),
	Center   = task.goCmuRush(pos.getOtherPos(4)),
	Middle   = task.wback(4,3),
	Defender = task.wback(4,4),
	Breaker  = task.wback(4,1),
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[LRMASBDFKC]"
},

["GetBall0"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wdrag(pos.getOtherPos(0)),
	Breaker  = task.wdrag(pos.getOtherPos(1)),
	Receiver = task.wdrag(pos.getOtherPos(2)),
	Fronter  = task.wdrag(pos.getOtherPos(3)),
	Kicker   = task.wdrag(pos.getOtherPos(4)),
	Center   = task.wdrag(pos.getOtherPos(5)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][S][DA][MBRFKC]"
},

["GetBall1"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",_,getAttackerNum(0)),
	Breaker  = task.wdrag(pos.getOtherPos(0)),
	Receiver = task.wdrag(pos.getOtherPos(1)),
	Fronter  = task.wdrag(pos.getOtherPos(2)),
	Kicker   = task.wdrag(pos.getOtherPos(3)),
	Center   = task.wdrag(pos.getOtherPos(4)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][S][DA][M][BRFKC]"
},

["GetBall2"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero", _,getAttackerNum(0)),
	Breaker  = task.wmarking("First",_,getAttackerNum(1)),
	Receiver = task.wdrag(pos.getOtherPos(0)),
	Fronter  = task.wdrag(pos.getOtherPos(1)),
	Kicker   = task.wdrag(pos.getOtherPos(2)),
	Center   = task.wdrag(pos.getOtherPos(3)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][S][DA][MB][RFKC]"
},

["GetBall3"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Receiver = task.wmarking("Second",_,getAttackerNum(2)),
	Fronter  = task.wdrag(pos.getOtherPos(0)),
	Kicker   = task.wdrag(pos.getOtherPos(1)),
	Center   = task.wdrag(pos.getOtherPos(2)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),	
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][S][DA][MBR][FKC]"
},

["GetBall4"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Receiver = task.wmarking("Second",_,getAttackerNum(2)),
	Fronter  = task.wmarking("Third", _,getAttackerNum(3)),
	Kicker   = task.wdrag(pos.getOtherPos(0)),
	Center   = task.wdrag(pos.getOtherPos(1)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),	
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][S][DA][MBRF][KC]"
},

["GetBall5"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Receiver = task.wmarking("Second",_,getAttackerNum(2)),
	Fronter  = task.wmarking("Third", _,getAttackerNum(3)),
	Kicker   = task.wmarking("Fourth",_,getAttackerNum(4)),
	Center   = task.wdrag(pos.getOtherPos(0)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),	
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][S][DA][MBRFK][C]"
},

["GetBall6"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Receiver = task.wmarking("Second",_,getAttackerNum(2)),
	Fronter  = task.wmarking("Third", _,getAttackerNum(3)),
	Kicker   = task.wmarking("Fourth",_,getAttackerNum(4)),
	Center   = task.wdrag(pos.getOtherPos(0)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),	
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][S][DA][MBRFK][C]"
},

["GetBall7"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Receiver = task.wmarking("Second",_,getAttackerNum(2)),
	Fronter  = task.wmarking("Third", _,getAttackerNum(3)),
	Kicker   = task.wmarking("Fourth",_,getAttackerNum(4)),
	Center   = task.wmarking("Fifth", _,getAttackerNum(5)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),	
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][S][DA][MBRFKC]"
},

["GetBall8"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Receiver = task.wmarking("Second",_,getAttackerNum(2)),
	Fronter  = task.wmarking("Third", _,getAttackerNum(3)),
	Kicker   = task.wmarking("Fourth",_,getAttackerNum(4)),
	Center   = task.wmarking("Fifth", _,getAttackerNum(5)),
	Special  = task.wmarking("Sixth", _,getAttackerNum(6)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),	
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][DA][MBRFKCS]"
},

["Pass0"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wdrag(pos.getOtherPos(0)),
	Breaker  = task.wdrag(pos.getOtherPos(1)),
	Receiver = task.wdrag(pos.getOtherPos(2)),
	Fronter  = task.wdrag(pos.getOtherPos(3)),
	Kicker   = task.wdrag(pos.getOtherPos(4)),
	Center   = task.wdrag(pos.getOtherPos(5)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][DA][MBRFKC]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[LS][DA][MBRKFC]"
			  	else
			  		 res = "{L}[S][DA][MBRFKC]"
			  	end
			end 
			   return res
	end,
},

["Pass1"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wdrag(pos.getOtherPos(0)),
	Breaker  = task.wdrag(pos.getOtherPos(1)),
	Receiver = task.wdrag(pos.getOtherPos(2)),
	Fronter  = task.wdrag(pos.getOtherPos(3)),
	Kicker   = task.wdrag(pos.getOtherPos(4)),
	Center   = task.wdrag(pos.getOtherPos(5)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 8 then
				  res =  "[L][S][DA][MBRFKC]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[LS][DA][MBRFKC]"
			  	else
			  		 res = "{L}[S][DA][MBRFKC]"
			  	end
			end 
			   return res
	end,
},

["Pass2"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",_,getAttackerNum(0)),
	Breaker  = task.wdrag(pos.getOtherPos(0)),
	Receiver = task.wdrag(pos.getOtherPos(1)),
	Fronter  = task.wdrag(pos.getOtherPos(2)),
	Kicker   = task.wdrag(pos.getOtherPos(3)),
	Center   = task.wdrag(pos.getOtherPos(4)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 8 then
				  res =  "[L][S][DA][MBRFKC]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[LS][DA][MBRFKC]"
			  	else
			  		 res = "{L}[S][DA][MBRFKC]"
			  	end
			end 
			   return res
	end,
},

["Pass3"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",_,getAttackerNum(0)),
	Breaker  = task.wmarking("First",_,getAttackerNum(1)),
	Receiver = task.wdrag(pos.getOtherPos(0)),
	Fronter  = task.wdrag(pos.getOtherPos(1)),
	Kicker   = task.wdrag(pos.getOtherPos(2)),
	Center   = task.wdrag(pos.getOtherPos(3)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][DA][MBRFKC]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[LS][DA][MBRFKC]"
			  	else
			  		 res = "{L}[S][DA][MBRFKC]"
			  	end
			end 
			   return res
	end,
},

["Pass4"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",_,getAttackerNum(2)),
	Kicker   = task.wdrag(pos.getOtherPos(0)),
	Center   = task.wdrag(pos.getOtherPos(1)),
	Receiver = task.wback(3,3),
	Defender = task.wback(3,1),
	Assister = task.wback(3,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][ADR][MBFKC]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[LS][ADR][MBFKC]"
			  	else
			  		 res = "{L}[S][ADR][MBFKC]"
			  	end
			end 
			   return res
	end,
},

["Pass5"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",_,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", _,getAttackerNum(3)),
	Center   = task.wdrag(pos.getOtherPos(0)),
	Receiver = task.wback(3,3),
	Defender = task.wback(3,1),
	Assister = task.wback(3,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][ADR][MBFCK]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[LS][ADR][MBFCK]"
			  	else
			  		 res = "{L}[S][ADR][MBFCK]"
			  	end
			end 
			   return res
	end,
},

["Pass6"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",_,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", _,getAttackerNum(3)),
	Center   = task.wdrag(pos.getOtherPos(0)),
	Receiver = task.wback(3,3),
	Defender = task.wback(3,1),
	Assister = task.wback(3,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][ADR][MBFKC]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[LS][ADR][MBFKC]"
			  	else
			  		 res = "{L}[S][ADR][MBFCK]"
			  	end
			end 
			   return res
	end,
},

["Pass7"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",_,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", _,getAttackerNum(3)),
	Center   = task.wmarking("Fourth",_,getAttackerNum(4)),
	Receiver = task.wback(3,3),
	Defender = task.wback(3,1),
	Assister = task.wback(3,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][ADR][MBFCK]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[LS][ADR][MBFCK]"
			  	else
			  		 res = "{L}[S][ADR][MBFCK]"
			  	end
			end 
			   return res
	end,
},

["Pass8"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Fronter  = task.wmarking("Second",_,getAttackerNum(2)),
	Kicker   = task.wmarking("Third", _,getAttackerNum(3)),
	Center   = task.wmarking("Fourth",_,getAttackerNum(4)),
	Receiver = task.wmarking("Fifth", _,getAttackerNum(5)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][AD][MBFKCR]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[LS][AD][MBFKCR]"
			  	else
			  		 res = "{L}[S][AD][MBFKCR]"
			  	end
			end 
			   return res
	end,
},

["2cardefend"] = {
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader = KICK_TASK(),
	Goalie   = task.goalie(goaliePassPos()),
	match = "[L]"
},

["3cardefend"] = {
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Goalie   = task.goalie(goaliePassPos()),
	match = "[L][S]"
},

["4car"] = {
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Assister = task.wback(1,1),
	Goalie   = task.goalie(goaliePassPos()),
	match = "[L](A)[S]"
},

["5car"] = {
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Assister = task.wback(2,1),
	Middle   = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match = "[L][A][M][S]"
},

["6car"] = {
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Assister = task.wback(2,1),
	Middle   = task.wback(2,2),
	Defender = otherTask(0,0),
	Goalie   = task.goalie(goaliePassPos()),
	match = "[L][AM][S][D]"
},

["7car"] = {
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Assister = task.wback(2,1),
	Middle   = task.wback(2,2),
	Defender = otherTask(0,1),
	Breaker  = task.wdrag(pos.getOtherPos(0)),
	Goalie   = task.goalie(goaliePassPos()),
	match = "[L][AM][S][B][D]"
},

["8car"] = {
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Assister = task.wback(2,1),
	Middle   = task.wback(2,2),
	Defender = otherTask(0,1),
	Receiver = otherTask(1,2),
	Breaker  = task.wdrag(pos.getOtherPos(0)),
	Goalie   = task.goalie(goaliePassPos()),
	match = "[L][AM][S][B][DR]"
},




name = "NormalPlayMessi_11vs11",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}