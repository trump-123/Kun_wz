local TOTAL = 4
local angle = 0
local radius = 500
local start = CGeoPoint:new_local(0,0)
local pos = function(num)
     return function()
      return enemy.pos(num)
    end  
    end  
local shootGeo = function(dist)
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
--local n2 = CGeoPoint:new_local(0,0) --enemy.Pos()
local p1 = function(n1,pos)
	return function()
	ipos = pos
	if type(pos) == 'function' then
	    ipos = pos()
	    else
	    ipos = pos	
	    end	
        return (ipos - player.pos(n1)):dir();
     end
end  
local p = function(n,TOTAL,pos)
	return function()
	ipos = pos
	if type(pos) == 'function' then
		ipos = pos()
	else
		ipos = pos
	end
	local centerX = ipos:x()
	local centerY = ipos:y()
	    return CGeoPoint:new_local(centerX+radius*math.cos(angle + n*math.pi*2/TOTAL),centerY+radius*math.sin(angle+n*math.pi*2/TOTAL))
	end
end
gPlayTable.CreatePlay{

firstState = "run1",

["run1"] = {
	switch = function()
	--angle = angle + math.pi*2/60 *1/4.0
	if bufcnt(JUDGE.BallInField(),10) then
		return "run2"
	end
    end,
  --[[ Assister = task.goCmuRush(p(1,TOTAL,pos(0)),p1("Assister",pos(0))),
   Middle   = task.goCmuRush(p(2,TOTAL,pos(0)),p1("Middle",pos(0))),
   Leader   = task.goCmuRush(p(3,TOTAL,pos(0)),p1("Leader",pos(0))),
   Special  = task.goCmuRush(p(4,TOTAL,pos(0)),p1("Special",pos(0))), 
   --]]
   Assister = task.goCmuRush(start,_,_,DSS_FLAG),
   match = "[AMLS]"
},

["run2"] = {
	switch = function()
	if bufcnt(player.toTargetDist("Assister")<50,10) then
		return "run3"
	end
	if bufcnt(not JUDGE.BallInField(),5) then
		return "run1"
    end
end,
  Assister = task.goCmuRush(shootGeo(150), _, _, DSS_FLAG),
  match = "[A]"
},

["run3"] = {
	switch = function()
		if player.infraredCount("Assister")>10 then
			return "run4"
		end
		if bufcnt(not JUDGE.BallInField(),5) then
			return "run1"
		end
	end,
	Assister = task.goCmuRush(shootGeo(100),_,_,flag.dribbling),
	match = "[A]"
},

["run4"] = {
	switch = function()
	if bufcnt(player.infraredCount("Assister")>1,10) then	
	return "run5"
	end	
	if bufcnt(not JUDGE.BallInField(),5) then
		return"run1"
	end
end,
Assister = task.goCmuRush(shootGeo(250), _, _,flag.dribbling),
match = "[A]"
},

["run5"] = {
	switch = function()
	if player.kickBall("Assister") then
		return "run1"
	end
	if bufcnt(not JUDGE.BallInField(),5) then
		return"run1"
	end	
end,
Assister = task.shoot(shootGeo(100)),
match = "[A]"
},
name = "test",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
