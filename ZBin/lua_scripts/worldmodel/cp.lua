module(..., package.seeall)

function specified(p)
	return function ()
		return p
	end
end

function full()
	return function()
	     return 5500
    end
end

function touch()
	return 4800
end

function slight()
	return function()
    	return 1800
  end
end

function middle()
	return function()
	    return 2200
	end
end

function normal()
	return function()
	    return 2400 
    end
end
-- ´¿±Õ°üº¯Êý£¬Õâ¸öº¯ÊýÖ»ÊÇÓÃÔÚ¿ªÉäÃÅµÄÌõ¼þÖÐ
-- role1 Îª½ÓÇò³µ

function toPlayer(role1)
	return function(role2)
		local dist = player.toPlayerDist(role1, role2)
		if IS_SIMULATION then
			return dist*dist/9800.0
		else
			return dist
		end
	end
end

function toTarget(p)
	return function()
		local tmpP
		if type(p) == "function" then
			tmpP = p()
		elseif type(p) == "number" or type(p) == "string" then
			tmpP = player.pos(p)
		elseif type(p) == "userdata" then
			tmpP = p
		end

		local dist = ball.toPointDist(tmpP) * 0.6--(0.0000973593*(math.pow(ball.toPointDist(tmpP),2)) + 1.07876 * ball.toPointDist(tmpP) + 1284.17)*0.8
		if IS_SIMULATION then
		     return dist --* 0.58
		else
			if dist <= 1500 then
				dist = 1500
			end
		return dist
	    end
    end
end


function toTargetFreeKick(p)
	return toTarget(p)
end

function toTargetTime(p)
	if(p == nil) then
		return function(p)
			local tmpP
			if type(p) == "function" then
				tmpP = p()
			else
				tmpP = p
			end
			local power = toTarget(p)()
			local targetDist = ball.toPointDist(tmpP)
			local time = world:chipTimePredict(power, targetDist) * param.frameRate
			local fixTime = world:chipJumpTimePredict(power) * param.frameRate
			return time, fixTime, power
		end
	else
		local tmpP
		if type(p) == "function" then
			tmpP = p()
		else
			tmpP = p
		end
		local power = toTarget(p)()
		local targetDist = ball.toPointDist(tmpP)
		local time = world:chipTimePredict(power, targetDist) * param.frameRate
		local fixTime = world:chipJumpTimePredict(power) * param.frameRate
		return time, fixTime, power
	end
end

function getFixBuf(p)
	if type(p) == "function" then
		p = p()
	end
	local dist = ball.toPointDist(p)
	local chipPower = toTarget(p)()
	local buf = (-0.0000023171*chipPower*chipPower + 0.0031183100*chipPower + 0.1231603074) * param.frameRate;
	return buf
end