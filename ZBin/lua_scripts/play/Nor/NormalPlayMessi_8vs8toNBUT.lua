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



local next_kicker = -1
local kick_number
local KICK_TASK = function()
	return function()
		return task.goandTurnKick(getPassPos(),_,getPassVel())
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

local ourBallJumpCond = function()
	local state = messi:nextState()
	local attackerAmout = defenceSquence:attackerAmount()
	if attackerAmout < 0 then
		 attackerAmout = 0
	end

	if BUSDEFENSE then
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
  		if attackerAmout > 5 then
  			attackerAmout = 5
  		end
  		local leader = leaderNum()
  		local receiver = receiverNum()
  		if getOurNum() > 5 then
  			if leader ~= receiver and player.valid(receiver) then
  				gRoleNum["Leader"] = leader
  				gRoleNum["Special"]= receiver
  			else
  				gRoleNum["Leader"] = leader
  			end
  		end
  		return "Pass"..attackerAmout

  elseif state == "GetBall"  then 	
  			if attackerAmout > 5 then
  				 attackerAmout = 5
  			end

  			if getOurNum() > 5 then
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
	Assister = task.goCmuRush(pos.getOtherPos(1)),
	Special  = task.goCmuRush(pos.getOtherPos(0)),
	Middle   = task.wback(4,3),
	Defender = task.wback(4,4),
	Breaker  = task.wback(4,1),
	Goalie   = task.goalie(goaliePassPos()),
	match    = "{LRMSABD}"--{LASMD}
},

["GetBall0"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wdrag(pos.getOtherPos(0)),
	Breaker  = task.wdrag(pos.getOtherPos(1)),
	Receiver = task.wmarking("Zero",_,enemy.nearNum),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][S][DA][RMB]"
},

["GetBall1"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",_,getAttackerNum(0)),
	Breaker  = task.wdrag(pos.getOtherPos(0)),
	Receiver = task.wmarking("Zero",_,enemy.nearNum),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][S][DA][RM][B]"
},

["GetBall2"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",_,getAttackerNum(0)),
	Breaker  = task.wmarking("First",_,getAttackerNum(1)),
	Receiver = task.wmarking("Zero",_,enemy.nearNum),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = "{L}[S][DA][RMB]"
},

["GetBall3"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = receiverTask(),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Receiver = task.wmarking("Zero",_,enemy.nearNum),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),	
	Goalie   = task.goalie(goaliePassPos()),
	match    = "{L}[S][DA][RMB]"
},

["GetBall4"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = task.wmarking("Zero",  _,enemy.nearNum),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Receiver = task.wmarking("Second",_,getAttackerNum(2)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),	
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][DA][SMBR]"
},

["GetBall5"]={
	switch = function ()
		return ourBallJumpCond()
	end,
	Leader   = KICK_TASK(),
	Special  = task.wmarking("Second",_,getAttackerNum(2)),
	Middle   = task.wmarking("Zero",  _,getAttackerNum(0)),
	Breaker  = task.wmarking("First", _,getAttackerNum(1)),
	Receiver = task.wmarking("Zero",_,enemy.nearNum),--task.wmarking("Second",_,getAttackerNum(2)),
	Defender = task.wback(2,1),--task.wmarking("Fourth",_,getAttackerNum(4)),
	Assister = task.wback(2,2),	
	Goalie   = task.goalie(goaliePassPos()),
	match    = "[L][DAR][MBS]"
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
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][DA][MBR]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[L][S][DA][MBR]"
			  	else
			  		 res = "{L}[S][DA][MBR]"
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
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][DA][MBR]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[L][S][DA][MBR]"
			  	else
			  		 res = "{L}[S][DA][MBR]"
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
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][DA][MBR]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[L][S][DA][MBR]"
			  	else
			  		 res = "{L}[S][DA][MBR]"
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
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][DA][MBR]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[L][S][DA][MBR]"
			  	else
			  		 res = "{L}[S][DA][MBR]"
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
	Middle   = task.wmarking("Zero",_,getAttackerNum(0)),
	Breaker  = task.wmarking("First",_,getAttackerNum(1)),
    Receiver = task.wdrag(pos.getOtherPos(0)),
	Defender = task.wback(2,1),
	Assister = task.wback(2,2),	
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][DA][RMB]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[L][S][DA][RMB]"
			  	else
			  		 res = "{L}[S][DA][RMB]"
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
	Middle   = task.wmarking("Zero",_,getAttackerNum(0)),
	Breaker  = task.wmarking("First",_,getAttackerNum(1)),
	Receiver = task.wback(3,3),
	Defender = task.wback(3,1),
	Assister = task.wback(3,2),
	Goalie   = task.goalie(goaliePassPos()),
	match    = function()
		local leader = leaderNum()
		local receiver = receiverNum()
			if getTheirNum() <= 5 then
				  res =  "[L][S][ADR][MB]"
			else
			  	if leader ~= receiver and player.valid(receiver) then
			   		 res = "[L][S][ADR][MB]"
			  	else
			  		 res = "{L}[S][ADR][MB]"
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
	match = "[L][A][S]"
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
	Defender = task.wmarking("Zero",_,getAttackerNum(0)),
	Goalie   = task.goalie(goaliePassPos()),
	match = "[L][A][M][S][D]"
},



name = "NormalPlayMessi_8vs8toNBUT",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}