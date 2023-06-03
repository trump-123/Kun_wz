module(..., package.seeall)

function specified(p)
	return function ()
		return p
	end
end

function full()
	return 8000
end

function touch()
	return 6500
end

function forReflect()
	return 8888
end

-- 当t有三种输入(userdate/point、role、function)
function toTarget(p,role1)
		return function(role)
			local target
			--local inSpeed = 4500 -- 需要的入嘴速度
			if type(p) == "function" then
				target = p() 
			elseif type(p) == "number" or type(p) == "string" then
				target = player.pos(p)
			elseif type(p) == "userdata" then
				target = p
			end
			local pw
			if IS_SIMULATION then
				--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),"0")
				if role1==nil then
					return  player.toPointDist(role, target) < 2500 and 2500 or player.toPointDist(role, target)--*1.2 --+ 1900
				else
					return  player.toPointDist(role1, target) < 2500 and 2500 or player.toPointDist(role1, target)--*1.2
				end
			else
				-- if player.num(role) == 1 then
				-- 	pw = player.toPointDist(role, target)*0.553167 + 0.0000345591*math.pow(player.toPointDist(role, target),2) + 987.004
				-- elseif player.num(role) == 0 or player.num(role) == 4 or player.num(role) == 5 or player.num(role) == 2 or player.num(role) == 3 then
				--     pw = player.toPointDist(role, target)*0.136966 + 0.000103831*math.pow(player.toPointDist(role, target),2) + 774.202 --* 1.5714 + 428.57
				-- end
				if role1 == nil then
					pw = player.toPointDist(role, target)*1.6
				else
					pw = player.toPointDist(role1,target)*1.6
				end
			end
			
			if pw < 3000 then    --50 --> 250 Modified by Soap, 2015/4/11
				pw = 3000 					--50 --> 250 Modified by Soap, 2015/4/11
			elseif pw > 5000 then
				pw = 5000
			end
			return pw
		end
end

function toTargetd(p)
	return function(role)
		local target
		--local inSpeed = 4500 -- 需要的入嘴速度
		if type(p) == "function" then
			target = p() 
		elseif type(p) == "userdata" then
			target = p
		else
			target = player.pos(p)
		end
		local pw
		if IS_SIMULATION then
			--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),"0")
			return  player.toPointDist(role, target)*1.2 + 1900
		else
			-- if player.num(role) == 1 then
			-- 	pw = player.toPointDist(role, target)*0.553167 + 0.0000345591*math.pow(player.toPointDist(role, target),2) + 987.004
			-- elseif player.num(role) == 0 or player.num(role) == 4 or player.num(role) == 5 or player.num(role) == 2 or player.num(role) == 3 then
			--     pw = player.toPointDist(role, target)*0.136966 + 0.000103831*math.pow(player.toPointDist(role, target),2) + 774.202 --* 1.5714 + 428.57
			-- end
			pw = player.toPointDist(role, target)
		end
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),pw)
		if pw < 3000 then    --50 --> 250 Modified by Soap, 2015/4/11
			pw = 3000 					--50 --> 250 Modified by Soap, 2015/4/11
		elseif pw > 5000 then
			pw = 5000
		end
		return pw
	end
end


function toTargetNormalPlay(p)
	return function(role)
		local target
		if type(p) == "function" then
			target = p()
		elseif type(p) == "userdata" then
			target = p
		else
			target = player.pos(p)
		end

		if IS_SIMULATION then
			pw =  player.toPointDist(role, target)*1.2 + 1500
		else
			pw = player.toPointDist(role, target)*1.5+428.75
		end
		if pw < 500 then    --50 --> 250 Modified by Soap, 2015/4/11
			pw = 500 					--50 --> 250 Modified by Soap, 2015/4/11
		elseif pw > 5000 then
			pw = 5000
		end
			return pw
			-- local pw = -0.0068*dist*dist + 5.5774*dist - 287.8
	end
end

-- function toPlayer(role1)
-- 	return function(role2)
-- 		if IS_SIMULATION then
-- 			return player.toPlayerDist(role1, role2)*1.2 + 150
-- 		else
-- 			local dist = player.toPlayerDist(role1, role2) * 1.2
-- 			if dist < 70 then
-- 				dist = 70
-- 			end
-- 			return dist
-- 		end
-- 	end
-- end

-- function toPoint(p)
-- 	return function(role)
-- 		local target
-- 		if type(p) == "function" then
-- 			target = p()
-- 		elseif type(p) == "userdata" then
-- 			target = p
-- 		end
-- 		if IS_SIMULATION then
-- 			return player.toPointDist(role, target)*1.2 + 150
-- 		else
-- 			local dist = player.toPointDist(role, target) * 1.2
-- 			if dist < 70 then
-- 				dist = 70
-- 			end
-- 			return dist
-- 		end
-- 	end
-- end