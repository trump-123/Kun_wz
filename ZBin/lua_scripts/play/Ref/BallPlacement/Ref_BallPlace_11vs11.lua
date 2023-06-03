local IstandPos = function(p,role)
	return function()
	local idealdist = 600 + param.playerRadius
	local standdist = 700 + param.playerRadius
	local ipos = p
	local placeline = CGeoSegment:new_local(ball.placementPos(),ball.pos())
    local me2target = CGeoSegment:new_local(player.pos(role),ipos)
	local project   = placeline:projection(player.pos(role))
	local iposproject   = placeline:projection(ipos)
	local isprj     = CGeoLineLineIntersection:new_local(placeline,me2target)
    local dist      = placeline:projection(player.pos(role)):dist(player.pos(role))
    local isprjon   = placeline:IsPointOnLineOnSegment(placeline:projection(player.pos(role)))
        if Utils.InTheirPenaltyArea(player.pos(role),35*10) then
        	return CGeoPoint:new_local(-param.pitchLength/2 + 290,0)
        elseif (ipos - iposproject):mod() > idealdist and not isprj:Intersectant() then
    		return ipos
        else
            if dist > idealdist then
           	    return player.pos(role) 
           	else
                if isprjon then
    	            return project + Utils.Polar2Vector(idealdist,(player.pos(role) - project):dir())
    	        else
    	    	    if (player.pos(role) - ball.pos()):mod() > (player.pos(role) - ball.placementPos()):mod() then
		    	    	if (player.pos(role) - ball.placementPos()):mod() > idealdist then
		    	    		return player.pos(role)
		    	    	else
		                    return ball.placementPos() + Utils.Polar2Vector(standdist,(player.pos(role) - ball.placementPos()):dir())
		                end
	                else
		            	if (player.pos(role) - ball.pos()):mod() > idealdist then
		    	    		return player.pos(role)
		    	    	else
		            	    return ball.pos() + Utils.Polar2Vector(standdist,(player.pos(role) - ball.pos()):dir())
		                end
		            end
    	        end
            end
        end
    end
end

local standpos = {
	CGeoPoint:new_local(-param.pitchLength/2+param.penaltyDepth+200,-700),
	CGeoPoint:new_local(-param.pitchLength/2+param.penaltyDepth+200,700),
	CGeoPoint:new_local(param.pitchLength/4-500,1000),
	CGeoPoint:new_local(param.pitchLength/4-500,-1000),
	CGeoPoint:new_local(-param.pitchLength/2-90,0),
	CGeoPoint:new_local(param.pitchLength/4-500,0),
	CGeoPoint:new_local(-param.pitchLength/2+param.penaltyDepth+200,-1000),
	CGeoPoint:new_local(-param.pitchLength/2+param.penaltyDepth+200,1000),
	CGeoPoint:new_local(-param.pitchLength/2+param.penaltyDepth+200,-1300),
	CGeoPoint:new_local(-param.pitchLength/2+param.penaltyDepth+200,1300),
	CGeoPoint:new_local(-param.pitchLength/2+param.penaltyDepth+200,1600),
}

local ACC = 5000

local GET_FLAG = flag.dribbling + flag.our_ball_placement
local DSS_FLAG = flag.our_ball_placement + flag.dodge_ball

-- local COR_DEF_POS1 = CGeoPoint:new_local(-50*param.lengthRatio,-50*param.widthRatio)
-- local SIDE_POS, MIDDLE_POS, INTER_POS = pos.refStopAroundBall()

local temp = true
local diff = false
local Ldiff = false

local normalPos = function(p)
	return function(role)
		if temp then
			return p
		else
			return IstandPos(p,role)()
		end
	end
end

local f = flag.our_ball_placement + flag.allow_dss

local otherflag = flag.our_ball_placement

local Assiserarrive = false

local LEADER_TASK = function()
	return function()
		-- if cond.ourBallPlace() then
			if Ldiff  then
				if player.toBallDist("Leader") < 35 * 10 then
					f = f + flag.dribbling
				else
      				f = flag.allow_dss + flag.our_ball_placement
				end 
				return task.goCmuRush(player.standposV1("Leader"),player.toBallDir,_,f)
			end
			if cond.ballPlaceFinish() and not cond.ballPlaceUnfinish() then
				return task.goCmuRush(pos.LEADER_STOP_POS, player.toBallDir, _, flag.dodge_ball + flag.allow_dss + flag.our_ball_placement)
			end
			return task.fetchball(ball.placementPos,_,Assiserarrive)
		-- elseif cond.theirBallPlace() then
		-- 	return task.goCmuRush(IstandPos(standpos[4],"Leader"),0,_,otherflag)
		-- else
		--     return task.goCmuRush(IstandPos(standpos[4],"Leader"),0,_,otherflag)
		-- end
	end
end

local receivePos = function()
	return ball.placementPos() + Utils.Polar2Vector(90,(ball.placementPos() -  ball.pos()):dir())
end


local ASSISTER_TASK = function()
	return function()
		-- if diff or cond.theirBallPlace() then
		-- 	return task.goCmuRush(IstandPos(standpos[6],"Assister"),0,_,otherflag)
		-- else
			if (ball.placementPos() - ball.pos()):mod() < 2000 then
				return task.goCmuRush(IstandPos(standpos[6],"Assister"),0,_,otherflag)
			end
			return task.goCmuRush(receivePos,player.toBallDir,ACC,otherflag)
		--end
	end
end


gPlayTable.CreatePlay{

firstState = "beginning",


["beginning"] = {
	switch = function()
		    temp = true
			diff = false
			Ldiff = false
			Assiserarrive = false
			if cond.ourBallPlace() then
				return "ourballplace"
			elseif cond.theirBallPlace() then
				return "theirballPlace"
			else
				return "theirballPlace"
			end

		--debugEngine:gui_debug_msg(CGeoPoint:new_local(2000,0),cond.ballPlaceFinish() and 1 or 0 )
	end,
	Leader   = task.goCmuRush(pos.LEADER_STOP_POS,0,_,otherflag),
	Assister = task.goCmuRush(standpos[6],0,_,otherflag),
	Special  = task.goCmuRush(standpos[1],0,_,otherflag),
	Middle   = task.goCmuRush(standpos[2],0,_,otherflag),
	Defender = task.goCmuRush(standpos[3],0,_,otherflag),
	Breaker  = task.goCmuRush(standpos[7],0,_,otherflag),
	Receiver = task.goCmuRush(standpos[8],0,_,otherflag),
	Fronter  = task.goCmuRush(standpos[9],0,_,otherflag),
	Kicker   = task.goCmuRush(standpos[10],0,_,otherflag),
	Center   = task.goCmuRush(standpos[11],0,_,otherflag),
	Goalie   = task.goCmuRush(standpos[5],0,_,otherflag),
	match    = "{LASMDBRFKC}"
},


["ourballplace"] = {
	switch = function()
		if player.kickBall("Leader") then
			diff = true
			Ldiff = true
			local t = gRoleNum["Leader"]
			gRoleNum["Leader"] = gRoleNum["Assister"]
			gRoleNum["Assister"] = t
		end

		--if bufcnt(Ldiff,10) then
			if player.infraredOn("Leader") then
				Ldiff = false
			end
		--end

		if Ldiff and not player.infraredOn("Leader") and ball.velMod() < 100 then
			Ldiff = false
		end

		-- if bufcnt(true,5) then
		-- 	temp = false
		-- else
		-- 	temp = true
		-- 	diff = false
		-- 	Ldiff = false
		-- 	Assiserarrive = false
		-- end

		if player.toTargetDist("Assister") < 20 then
			Assiserarrive = true
		else
			Assiserarrive = false
		end

		--debugEngine:gui_debug_msg(CGeoPoint:new_local(2000,0),Assiserarrive and 1 or 0 )
	end,
	Leader   = LEADER_TASK(),
	Assister = ASSISTER_TASK(),
	Special  = task.goCmuRush(IstandPos(standpos[1],"Special"),0,_,otherflag),
	Middle   = task.goCmuRush(IstandPos(standpos[2],"Middle"),0,_,otherflag),
	Defender = task.goCmuRush(IstandPos(standpos[3],"Defender"),0,_,otherflag),
	Breaker  = task.goCmuRush(IstandPos(standpos[7],"Breaker"),0,_,otherflag),
	Receiver = task.goCmuRush(IstandPos(standpos[8],"Receiver"),0,_,otherflag),
	Fronter  = task.goCmuRush(normalPos(standpos[9],"Fronter"),0,_,otherflag),
	Kicker   = task.goCmuRush(normalPos(standpos[10],"Kicker"),0,_,otherflag),
	Center   = task.goCmuRush(normalPos(standpos[11],"Center"),0,_,otherflag),
	Goalie   = task.goCmuRush(IstandPos(standpos[5],"Goalie"),0,_,otherflag),
	match    = "{LASMDBRFKC}"
},

["theirballPlace"] = {
	switch = function()
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(2000,0),Assiserarrive and 1 or 0 )
	end,
	Leader   = task.goCmuRush(IstandPos(standpos[4],"Leader"),0,_,otherflag),
	Assister = task.goCmuRush(IstandPos(standpos[6],"Assister"),0,_,otherflag),
	Special  = task.goCmuRush(IstandPos(standpos[1],"Special"),0,_,otherflag),
	Middle   = task.goCmuRush(IstandPos(standpos[2],"Middle"),0,_,otherflag),
	Defender = task.goCmuRush(IstandPos(standpos[3],"Defender"),0,_,otherflag),
	Breaker  = task.goCmuRush(IstandPos(standpos[7],"Breaker"),0,_,otherflag),
	Receiver = task.goCmuRush(IstandPos(standpos[8],"Receiver"),0,_,otherflag),
	Fronter  = task.goCmuRush(normalPos(standpos[9],"Fronter"),0,_,otherflag),
	Kicker   = task.goCmuRush(normalPos(standpos[10],"Kicker"),0,_,otherflag),
	Center   = task.goCmuRush(normalPos(standpos[11],"Center"),0,_,otherflag),
	Goalie   = task.goCmuRush(IstandPos(standpos[5],"Goalie"),0,_,otherflag),
	match    = "{LASMDBRFKC}"
},

name = "Ref_BallPlace_11vs11",
applicable ={
	exp = "a",
	a   = true
},
attribute = "defense",
timeout   = 99999
}