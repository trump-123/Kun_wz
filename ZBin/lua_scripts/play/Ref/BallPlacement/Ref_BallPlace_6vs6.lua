local IstandPos = function(p,role)
	return function()
	local idealdist = 600 + param.playerRadius
	local standdist = 700 + param.playerRadius
	local ipos = p
	local placeline = CGeoSegment:new_local(ball.placementPos(),ball.pos())
	local project   = placeline:projection(player.pos(role))
	local iposproject   = placeline:projection(ipos)
    local dist      = placeline:projection(player.pos(role)):dist(player.pos(role))
    local isprjon   = placeline:IsPointOnLineOnSegment(placeline:projection(player.pos(role)))
        if (ipos - iposproject):mod() > idealdist then
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
	CGeoPoint:new_local(param.pitchLength/4-500,0)
}

local ACC = 5000

local GET_FLAG = flag.dribbling + flag.our_ball_placement
local DSS_FLAG = flag.our_ball_placement + flag.dodge_ball

-- local COR_DEF_POS1 = CGeoPoint:new_local(-50*param.lengthRatio,-50*param.widthRatio)
-- local SIDE_POS, MIDDLE_POS, INTER_POS = pos.refStopAroundBall()

local temp = true
local diff = false
local Ldiff = false

local Assiserarrive = false

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

local LEADER_TASK = function()
	 if cond.ourBallPlace() then
		if Ldiff then
			if player.toBallDist("Leader") < 500 then
				f = f + flag.dribbling
			end 
			return task.goCmuRush(player.standpos("Leader"),player.toBallDir,_,f)
		end
		if cond.ballPlaceFinish() and not cond.ballPlaceUnfinish() then
			return task.goCmuRush(pos.LEADER_STOP_POS, player.toBallDir, _, flag.dodge_ball+flag.allow_dss)
		end
		return task.fetchball(ball.placementPos,_,Assiserarrive)
	elseif cond.theirBallPlace() then
		return task.goCmuRush(normalPos(standpos[6]),0,_,otherflag)
	else
	    return task.goCmuRush(normalPos(standpos[6]),0,_,otherflag)
	end
end

local receivePos = function()
	return ball.placementPos() + Utils.Polar2Vector(90,(ball.placementPos() -  ball.pos()):dir())
end


local ASSISTER_TASK = function()
	if diff or cond.theirBallPlace() then
		return task.goCmuRush(normalPos(standpos[4]),0)
	else
		if (ball.placementPos() - ball.pos()):mod() < 2000 then
			return task.goCmuRush(normalPos(standpos[4]),0,_,otherflag)
		end
		return task.goCmuRush(receivePos,player.toBallDir,ACC,GET_FLAG)
	end
end


gPlayTable.CreatePlay{

firstState = "beginning",

-- headback sideback defend middle
["beginning"] = {
	switch = function()
		if player.kickBall("Leader") then
			diff = true
			Ldiff = true
			local temp = gRoleNum["Leader"]
			gRoleNum["Leader"] = gRoleNum["Assister"]
			gRoleNum["Assister"] = temp
		end

		if bufcnt(player.infraredOn("Leader"),30) then
			Ldiff = false
		end

		if Ldiff and not player.infraredOn("Leader") and ball.velMod() < 200 then
			Ldiff = false
		end

		if bufcnt(true,5) then
			temp = false
		end

		if player.toPointDist("Assister",ball.placementPos()) < 20 then
			Assiserarrive = true
		else
			Assiserarrive = false
		end


		--debugEngine:gui_debug_msg(CGeoPoint:new_local(2000,0),cond.ballPlaceFinish() and 1 or 0 )
	end,
	Leader   = LEADER_TASK,
	Assister = ASSISTER_TASK,
	Special  = task.goCmuRush(normalPos(standpos[1]),0,_,otherflag),
	Middle   = task.goCmuRush(normalPos(standpos[2]),0,_,otherflag),
	Defender = task.goCmuRush(normalPos(standpos[3]),0,_,otherflag),
	Goalie   = task.goCmuRush(normalPos(standpos[5]),0,_,otherflag),
	match    = "{LASMD}"
},

name = "Ref_BallPlace_6vs6",
applicable ={
	exp = "a",
	a   = true
},
attribute = "defense",
timeout   = 99999
}