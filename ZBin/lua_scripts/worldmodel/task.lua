module(..., package.seeall)
function intel()
    local ipos = function()
    	local res
    	if ball.velMod()<500 then
    		res = ball.pos() + Utils.Polar2Vector(-100,ball.toTheirGoalDir())
	    else
	    	res = ball.pos() + Utils.Polar2Vector(0.1*ball.velMod(),ball.velDir())
	    end
	    	debugEngine:gui_debug_msg(CGeoPoint:new_local(0,200),"dipos"..ball.velMod())
	    	return res
	    
	    end
	    local idir =0
	local idir = function(runner)
		local res
		local str
		str = '1'
    	if ball.velMod()<500 then
    		str='2'
    		res = ball.toTheirGoalDir() 
	    else
	    	str = '3'
	    	res = (ball.pos()-player.pos(runner)):dir()
	    	end
	    	    	debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),"ddir"..str,3)
	    	return res
	    end
     
    local mexe, mpos = GoCmuRush{pos = ipos, dir = idir, acc = a, flag = f,rec = r,vel = v}
	return {mexe, mpos}
    end

-- function marking(xmin,xmax,ymin,ymax)
-- 	local num = 0
-- 	local ourGoal = CGeoPoint:new_local(-param.pitchLength/2.0,0)
-- 	local between = function(a,min,max)
-- 		if a > max or a < min then
-- 			return false
-- 		else
-- 			return true
-- 		end
-- 	end
-- 	local  checknum = function()
-- 	     local num = -1
-- 	     --local number = 0
-- 		--debugEngine:gui_debug_msg(CGeoPoint:new_local(-1000,200*(7-i)),i.." "..(enemy.valid(i) and "true"or"fasle"))
-- 		for i = 0,param.maxPlayer-1 do
-- 			local dist = (enemy.pos(i) - ball.pos()):mod()
-- 			if(enemy.valid(i)) and between(enemy.posX(i),xmin,xmax) and between(enemy.posY(i),ymin,ymax) and dist < 200 then
-- 	           --number = number+1
-- 	           num = i
-- 			end
-- 		end
-- 		--debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,0),number)
-- 			return num
-- 	end
-- 	local idir = function(runner)
-- 		checknum()
-- 		num = checknum()
-- 		local res
-- 		local enemypos = enemy.pos(num)
-- 		local ball = ball.pos()
-- 		if(num >= 0) then
-- 		   res = (enemypos - ourGoal):dir()
-- 		else
-- 		   res = (ball- ourGoal):dir()
-- 		end
-- 	return res
-- end
--     local ipos = function(runner)
--     	num = checknum()
--     	local res
--     	local ball = ball.pos()
--     	local enemypos = enemy.pos(num)
--     	local l = (ourGoal - enemypos):mod()*0.2
--     	local l1 = (ourGoal - ball):mod()*0.5
-- 	    	if(num >=0) then
-- 	    	     res = enemypos + Utils.Polar2Vector(l,(ourGoal - enemypos):dir())
-- 	        else
-- 	             res = ball + Utils.Polar2Vector(l1,(ourGoal - ball):dir())
-- 	         end
--     return res
-- end
-- 	local mexe, mpos = GoCmuRush{pos = ipos, dir = idir, acc = a, flag = f,rec = r,vel = v}
-- 	return {mexe, mpos}
-- end

function marking1(xmin,xmax,ymin,ymax)
	local num = 0
	local ourGoal = CGeoPoint:new_local(-param.pitchLength/2.0,0)
	local between = function(a,min,max)
		if a > max or a < min then
			return false
		else
			return true
		end
	end
	local  checknum = function()
	     local num = -1
	     local x = -1
	     --local number = 0
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(-1000,200*(7-i)),i.." "..(enemy.valid(i) and "true"or"fasle"))
		for i = 0,param.maxPlayer-1 do
			local dist = (enemy.pos(i) - ball.pos()):mod()
			if(enemy.valid(i)) and between(enemy.posX(i),xmin,xmax) and between(enemy.posY(i),ymin,ymax) and dist > 300 then
	           --number = number+1
	           num = i
	       end
		end
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,0),number)
			return num
	end
	local idir = function(runner)
		checknum()
		num = checknum()
		local res
		local enemypos = enemy.pos(num)
		local ball = ball.pos()
		if(num >= 0) then
		   res = (enemypos - ourGoal):dir()
		else
		   res = (ball- ourGoal):dir()
		end
			return res
end
    local ipos = function(runner)
    	num = checknum()
    	local res
    	local ball = ball.pos()
    	local enemypos = enemy.pos(num)
    	local l = (ourGoal - enemypos):mod()*0.2
    	local l1 = (ourGoal - ball):mod()*0.6
	    	if(num >=0) then
	    	     res = enemypos + Utils.Polar2Vector(l,(ourGoal - enemypos):dir())
	        else
	             res = ball + Utils.Polar2Vector(l1,(ourGoal - ball):dir())
	        end
    	return res
end
	local mexe, mpos = GoCmuRush{pos = ipos, dir = idir, acc = a, flag = f,rec = r,vel = v}
	return {mexe, mpos}
end


function catchball()
	local num = 0
		for i=0,param.maxPlayer - 1 do
			if player.valid(i) then
				local tpos = player.pos(i)
				local ball 	= ball.pos()
				if (tpos - ball):mod() < 150 then
					num = i
				end
			end
		end
	local ipos = function(runner)
		local res
		local topos = player.pos(num)
		local mypos = player.pos(runner)
		local dir1 	= (mypos - topos):dir()
		local ball = ball.pos()

		if player.kickBall(n) then
			if ball.velMod()<500 then
    			res = ball.pos() + Utils.Polar2Vector(-50,ball.toTheirGoalDir())
	    	else
	    		res = ball.pos() + Utils.Polar2Vector(0.3*ball.velMod(),ball.velDir())
	    	end
	    else
	    	res = num
		end
		return res
	end
	local idir = function(runner)
		local res
		local kickpos = CGeoPoint:new_local(param.pitchLength /2,param.goalWidth/2-75)
		local topos = player.pos(num)
		local mypos = player.pos(runner)
		local todir = (topos - mypos):dir()
		if player.infraredCount(runner) > 10 then
			res = (kickpos - mypos):dir()
		else
			res = todir
		end
		return res
	end
local mexe, mpos = GoCmuRush{pos = ipos, dir = idir, acc = a, flag = f,rec = r,vel = v}
	return {mexe, mpos}
end


function CP_TASK(role,p)
  return function()
    if player.toBallDist(role) < 1500 then
       return task.shootV1(player.standpos(role),player.todir(role),_,8000)
    else
       return task.goCmuRush(p,player.todir(role),_,DSS_FLAG)
    end
  end
end

function tier()
	local mexe, mpos = Tier()	
	return {mexe, mpos}
end

function lefthalf()
	local f
	if not cond.isGameOn() then
		f = flag.dodge_ball
	end
	local mexe, mpos = LeftHalf{flag = flag.dodge_ball}	
	return {mexe, mpos}
end

function righthalf()
	local f
	if not cond.isGameOn() then
		f = flag.dodge_ball
	end
	local mexe, mpos = RightHalf{flag = flag.dodge_ball}	
	return {mexe, mpos}
end

function middlehalf()
	local mexe, mpos = MiddleHalf()	
	return {mexe, mpos}
end

function naqiu()
	local mexe, mpos = Naqiu()	
	return {mexe, mpos}
end	

--~		Play中统一处理的参数（主要是开射门）
--~		1 ---> task, 2 ---> matchpos, 3---->kick, 4 ---->dir,
--~		5 ---->pre,  6 ---->kp,       7---->cp,   8 ---->flag
------------------------------------- 射门相关的skill ---------------------------------------
-- TODO
------------------------------------ 跑位相关的skill ---------------------------------------
--~ p为要走的点,d默认为射门朝向
function goalie(p)
	--local DSS_FLAG = bit_or(flag.allow_dss,flag.dodge_ball)+flag.our_ball_placement 
	local mexe, mpos = Goalie{pos = p}
	return {mexe, mpos}
end

function Penaltygoalie()
	--local DSS_FLAG = bit_or(flag.allow_dss,flag.dodge_ball)+flag.our_ball_placement 
	local mexe, mpos = PenaltyGoalie()
	return {mexe, mpos}
end

function tier1()
	--local DSS_FLAG = bit_or(flag.allow_dss,flag.dodge_ball)+flag.our_ball_placement 
	local mexe,mpos = Tier1()
	return {mexe,mpos}
end

function singleTier()
	--local DSS_FLAG = bit_or(flag.allow_dss,flag.dodge_ball)+flag.our_ball_placement 
	local mexe,mpos = SingleTier()
	return {mexe,mpos}
end


--------------------------------
--         My SKILL           --
--------------------------------
local power = 50


function leader()
	local mexe, mpos = Leader()
	return {mexe, mpos}
end

function Guan()
	local mexe, mpos = guan()
	return {mexe, mpos}
end

function Sao()
	local mexe, mpos = sao()
	return {mexe, mpos}
end

function special()
	local mexe, mpos = Special()
	return {mexe, mpos}
end

function assister()
	local mexe, mpos = Assister()
	return {mexe, mpos}
end

function defend_05()
	local mexe, mpos = Defend_05()
	return {mexe, mpos}
end

function defend_07_0()
	local mexe, mpos = Defend_07_0()
	return {mexe, mpos}
end

function defend_07_1()
	local mexe, mpos = Defend_07_1()
	return {mexe, mpos}
end

function leader_getball()
	local mexe, mpos = Leader_getball()
	return {mexe, mpos}
end

function penaltykick()
	local mexe, mpos = Penaltykick()
	return {mexe, mpos}
end

function bigpenaltykick()
	local mexe, mpos = BIGPenaltykick()
	return {mexe, mpos}
end

function touch()
	local ipos = pos.ourGoal()
	local mexe, mpos = Touch{pos = ipos}
	return {mexe, mpos}
end

function text()
	--local ipos = pos.ourGoal()
	local mexe, mpos = Text()
	return {mexe, mpos}
end

function chaseKick()
	local mexe, mpos = ChaseKick()
	return {mexe, mpos}
end

function goandTurnKick(passpos,role,power,f)
	local ipos = function()
		if passpos ~= nil then
			if type(passpos) == "number" or type(passpos) == "string" then
				return player.pos(passpos)
			elseif type(passpos) == "function" then
				return passpos()
			else
				return passpos
			end
		else
			return pos.theirGoal()
		end	
	end
	local ipower = function()
		if power == nil then
			if passpos ~= nil then
				-- if type(passpos) == "number" or type(passpos) == "string" then
				-- 	return kp.toTarget(passpos)()
				-- else
				--debugEngine:gui_debug_msg(CGeoPoint:new_local(2500,0),kp.toTarget(passpos,role)())
					return kp.toTarget(passpos,role)()
				--end
			else
				return 6000
			end	
		else
			return power()
		end
	end
	--debugEngine:gui_debug_msg(CGeoPoint:new_local(3000,0),ipos:x())
	local mexe, mpos = goAndTurnKick{pos = ipos,power = ipower,flag = f}
	return {mexe, mpos}
end

function staticGetBall(passpos,kpower,cpower)
	local ipos = function()
		--local icpower = cpower and cpower or 0.0
		if passpos ~= nil then
			if type(passpos) == "number" or type(passpos) == "string" then
				return player.pos(passpos)
			elseif type(passpos) == "function" then
				return passpos()
			else
				return passpos
			end
		else
			return pos.theirGoal()
		end	
	end
	--debugEngine:gui_debug_msg(CGeoPoint:new_local(3000,0),ipos:x())
	local mexe, mpos = StaticGetBall{pos = ipos,kpower = kpower,cpower = cpower}
	return {mexe, mpos}
end

function touchKick(p,ifInter,power)
	local ipos = p or pos.theirGoal()
	local ipower --= power and power or 8000
	if type(power) == "function" then
		ipower = power
	else
		ipower = kp.specified(power)
	end
	--ipower = ipower and ipower or 8000
	--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,400),"now_power: "..ipower)
	-- local ipower = function(runner)
	-- 	if player.num(runner) == 1 then
	-- 		return power*0.553167 + 0.0000345591*math.pow(power,2) + 987.004
	-- 	else--if player.num(runner) == 0 or player.num(runner) == 4 or player.num(runner) == 5 or player.num(runner) == 2 or player.num(runner) == 3  then
	-- 		return power*0.136966 + 0.000103831 *math.pow(power,2) + 774.202
	-- 	end
	-- end

	local idir = function(runner)
		if type(ipos) == "function" then
			return (ipos() - player.pos(runner)):dir()
		else
			return (ipos - player.pos(runner)):dir()
		end
	end--kp.specified(ipower())
	local mexe, mpos = Touch{pos = ipos, useInter = ifInter}
	return {mexe, mpos, kick.flat, idir, pre.low,ipower ,cp.full, flag.nothing}
end

function fetchball(target,power,ifInter)
	local itarget
	local ipower

	if target ~= nil then
		if type(target) == "string" then
			itarget = function()
				return player.pos(target)
			end
		else
			itarget = target
		end
	else 
		itarget = CGeoPoint:new_local(0,0)
		print("ERROR FOR TARGET POS")
	end

	if power~=nil then
		ipower = kp.specified(power)
	elseif target ~= nil then
		ipower = kp.toTarget(target)
	else
		ipower = kp.full()
		print("ERROR FOR POWER")
	end
	local mexe, mpos = FetchBall{pos = itarget, power = ipower, useInter = ifInter}
	return {mexe, mpos}
end

function dnamicfindpos(p,d)
	local mexe, mpos = Dnamicfindpos{pos = p,dir = d}
	return {mexe, mpos}
end

function uniquekillskillm()--p,d
	local mexe, mpos = Uniquekillskillm()--{pos = p,dir = d}
	return {mexe, mpos}
end

function uniquekillskilll()--p,d
	local mexe, mpos = Uniquekillskilll()--{pos = p,dir = d}
	return {mexe, mpos}
end

function uniquekillskillr()--p,d
	local mexe, mpos = Uniquekillskillr()--{pos = p,dir = d}
	return {mexe, mpos}
end
function escape(p,ifInter)
	local ipos = p 
	local mexe, mpos = Escape{pos = ipos, useInter = ifInter}
	return {mexe, mpos}
end

function marking(p,ifInter,a)
	local mexe, mpos = Marking{pos = p, useInter = ifInter,acc = a}
	return {mexe, mpos}
end

function wmarking(p,f,n)
	local mexe,mpos = WMarking{pri = p,flag = f,num = n,srole = "marking"}
	return {mexe,mpos}
end

function markingFront(p,ifInter,a)
	local mexe, mpos = MarkingFront{pos = p, useInter = ifInter,acc = a}
	return {mexe, mpos}
end

function markingTouch(p,ifInter,a)
	local mexe, mpos = MarkingTouch{pos = p, useInter = ifInter,acc = a}
	return {mexe, mpos}
end

-- function touchKick(p,power)
-- 	local ikick = chip and kick.chip or kick.flat
-- 	local ipower = power and power or 8000
-- 	local idir = function(runner)
-- 	local mexe, mpos = GoCmuRush{pos = p, dir = idir, acc = a, flag = f,rec = r,vel = v}
-- 	return {mexe, mpos, kick.flat, idir, pre.low, kp.specified(ipower), cp.full, flag.nothing}
-- end

function goSpeciPos(p, d, f, a) -- 2014-03-26 增加a(加速度参数)
	local idir
	local iflag
	if d ~= nil then
		idir = d
	else
		idir = dir.shoot()
	end

	if f ~= nil then
		iflag = f
	else
		iflag = 0
	end

	local mexe, mpos = SmartGoto{pos = p, dir = idir, flag = iflag, acc = a}
	return {mexe, mpos}
end

function goSimplePos(p, d, f)
	local idir
	if d ~= nil then
		idir = d
	else
		idir = dir.shoot()
	end

	if f ~= nil then
		iflag = f
	else
		iflag = 0
	end

	local mexe, mpos = SimpleGoto{pos = p, dir = idir, flag = iflag}
	return {mexe, mpos}
end

function runMultiPos(p, c, d, idir, a)
	if c == nil then
		c = false
	end

	if d == nil then
		d = 20
	end

	if idir == nil then
		idir = dir.shoot()
	end

	local mexe, mpos = RunMultiPos{ pos = p, close = c, dir = idir, flag = flag.not_avoid_our_vehicle, dist = d, acc = a}
	return {mexe, mpos}
end

--~ p为要走的点,d默认为射门朝向 a 加速度 f 标志位 
function goCmuRush(p, d, a, f, r, v)
	local idir
	if d ~= nil then
		idir = d
	else
		idir = dir.shoot()
	end
	local mexe, mpos = GoCmuRush{pos = p, dir = idir, acc = a, flag = f,rec = r,vel = v}
	return {mexe, mpos}
end

function forcekick(p,d,chip,power)
	local ikick = chip and kick.chip or kick.flat
	local ipower = power and power or 8000
	local idir = d and d or dir.shoot()
	local mexe, mpos = GoCmuRush{pos = p, dir = idir, acc = a, flag = f,rec = r,vel = v}
	return {mexe, mpos, ikick, idir, pre.low, kp.specified(ipower), cp.full, flag.forcekick}
end

function shoot(p,d,chip,power)
	local ikick = chip and kick.chip or kick.flat--(power*0.136966 + 0.000103831*math.pow(power,2) + 774.202)
	local ipower = power and power or 8000--function(runner)
	-- 	if player.num(runner) == 1 then
	-- 		return power*0.553167 + 0.0000345591*math.pow(power,2) + 987.004
	-- 	else--if player.num(runner) == 0 or player.num(runner) == 4 or player.num(runner) == 5 or player.num(runner) == 2 or player.num(runner) == 3  then
	-- 		return power*0.136966 + 0.000103831 *math.pow(power,2) + 774.202
	-- 	end
	-- end 
	--power and power or 8000kp.specified(ipower())
	local idir = d and d or dir.shoot()
	local mexe, mpos = GoCmuRush{pos = p, dir = idir, acc = a, flag = f,rec = r,vel = v}
	return {mexe, mpos, ikick, idir, pre.low,kp.specified(ipower) , cp.specified(ipower), flag.nothing}
end

--PenaltyKick专用
function shootV1(p,d,chip,power)
	local ikick = chip and kick.chip or kick.flat
	--local ipower = power and (power*0.136966 + 0.000103831*math.pow(power,2) + 774.202) or 8000
	
	 local ipower = power and power or 8000 -- function(runner)
	-- 	if player.num(runner) == 1 then
	-- 		return power*0.553167 + 0.0000345591*math.pow(power,2) + 987.004
	-- 	else--if player.num(runner) == 0 or player.num(runner) == 4 or player.num(runner) == 5 or player.num(runner) == 2 or player.num(runner) == 3  then
	-- 		return power*0.136966 + 0.000103831 *math.pow(power,2) + 774.202
	-- 	end
	-- end 
--kp.specified(ipower())
	local idir = d and d or dir.shoot()
	local f = flag.dribbling--+ flag.our_ball_placement 
	local mexe, mpos = GoCmuRush{pos = p, dir = idir, acc = a, flag = f,rec = r,vel = v}
	return {mexe, mpos, ikick, idir, pre.low, kp.specified(ipower), cp.specified(ipower), flag.nothing}
end

function shootV2(p,d,chip,power)
	local ikick = chip and kick.chip or kick.flat
	--local ipower = power and (power*0.136966 + 0.000103831*math.pow(power,2) + 774.202) or 8000
	local topos = CGeoPoint:new_local(param.pitchLength/2,0)
	local ipower = power and power or 8000--local ipower = (0.0000973593*(math.pow(ball.toPointDist(topos),2)) + 1.07876 * ball.toPointDist(topos) + 1284.17)*0.5
--kp.specified(ipower())
	local idir = d and d or dir.shoot()
	local f = flag.our_ball_placement+flag.dribbling
	local mexe, mpos = GoCmuRush{pos = p, dir = idir, acc = a, flag = f,rec = r,vel = v}
	return {mexe, mpos, ikick, idir, pre.low, kp.specified(ipower), cp.specified(ipower), flag.nothing}
end

function wback(guardNum, index, f, idefendNum)
	local ipower
	local iflag = f or (flag.kick + flag.chip + flag.not_avoid_our_vehicle + flag.not_avoid_their_vehicle)
	
	if guardNum == nil and index == nil then
    	guardNum = 1
    	index = 1
    end
 --    if idefendNum == nil then
	--     print("task.lua  " .. "idefendNum nil")
	-- end
	local mexe, mpos = WBack{ guardNum = guardNum, index = index,  flag = iflag , defendNum = idefendNum, srole = "back"}
	return {mexe, mpos}
end

---------------------------------------------------------------------

function chippass(p,role,power)--role可以是一个function，userdate，player
	local ipower
	local idir
	if type(role) == "number" or type(role) == "string" then
		idir = player.toPlayerHeadDir(role)
	else
		idir = player.toPointDir(role)
	end

	-- local ipower = function(runner)
	-- 	--if player.num(runner) == 1  then
			if power == nil then
			     ipower =  cp.toTarget(role)
			else
				 ipower =  cp.specified(power)--(0.0000973593*(math.pow(power,2)) + 1.07876 * power + 1284.17)*0.75)
			end
	-- 	-- else--if player.num(runner) == 0 or player.num(runner) == 4 or player.num(runner) == 5 or player.num(runner) == 2 or player.num(runner) == 3 then
	-- 	-- 	return cp.specified(8000)
	-- 	-- end
	-- end
	local f = flag.dribbling
	local mexe, mpos = GoCmuRush{pos = p, dir = idir,flag = f}
	return {mexe, mpos, kick.chip, idir, pre.middle, kp.full, ipower , flag.nothing}
end

function flatpass(p,role,power)
	local ipower
	local idir
	if type(role) == "number" or type(role) == "string" then
		idir = player.toPlayerHeadDir(role)
	else
		idir = player.toPointDir(role)
	end

	-- local ipower = function(runner)
	-- 	if player.num(runner) == 1 then
		    if power == nil then
			     ipower =  kp.toTarget(role)
			else
				 ipower =  kp.specified(power)
			end
	-- 	else--if player.num(runner) == 0 or player.num(runner) == 2 or player.num(runner) == 3  or player.num(runner) == 4 or player.num(runner) == 5 then
	-- 		if power == nil then
	-- 		     return kp.toTarget(role)
	-- 		else
	-- 			 return kp.specified((power*0.136966 + 0.000103831*math.pow(power,2) + 774.202))
	-- 		end
	-- 	end
	-- end

	local flag1 = flag.dribbling
    local mexe, mpos = GoCmuRush{pos = p, dir = idir,flag = flag1}
	return {mexe, mpos, kick.flat, idir, pre.middle, ipower, cp.full, flag.nothing}
end	

function flatpassd(p,role,power)
	local ipower
	local idir
	if type(role) == "number" or type(role) == "string" then
		idir = player.toPlayerHeadDir(role)
	else
		idir = player.toPointDir(role)
	end

	-- local ipower = function(runner)
	-- 	if player.num(runner) == 1 then
		    if power == nil then
			     ipower =  kp.toTarget(role)
			else
				 ipower =  kp.specified(power)
			end
	-- 	else--if player.num(runner) == 0 or player.num(runner) == 2 or player.num(runner) == 3  or player.num(runner) == 4 or player.num(runner) == 5 then
	-- 		if power == nil then
	-- 		     return kp.toTarget(role)
	-- 		else
	-- 			 return kp.specified((power*0.136966 + 0.000103831*math.pow(power,2) + 774.202))
	-- 		end
	-- 	end
	-- end

	local flag1 = flag.dribbling
    local mexe, mpos = GoCmuRush{pos = p, dir = idir,flag = flag1}
	return {mexe, mpos, kick.flat, idir, pre.middle, ipower, cp.full, flag.nothing}
end	


------------------------------------ 防守相关的skill ---------------------------------------
-- TODO
----------------------------------------- 其他动作 --------------------------------------------

-- p为朝向，如果p传的是pos的话，不需要根据ball.antiY()进行反算
function goBackBall(p, d)
	local mexe, mpos = GoCmuRush{ pos = ball.backPos(p, d, 0), dir = ball.backDir(p), flag = flag.dodge_ball}
	return {mexe, mpos}
end

-- 带避车和避球
function goBackBallV2(p, d)
	local mexe, mpos = GoCmuRush{ pos = ball.backPos(p, d, 0), dir = ball.backDir(p), flag = bit:_or(flag.allow_dss,flag.dodge_ball)}
	return {mexe, mpos}
end

function stop()
	local mexe, mpos = Stop{}
	return {mexe, mpos}
end

function continue()
	return {["name"] = "continue"}
end

--技能包
function shootGen(dist)	--（是一个可以击球的点）球与门的3点一线  dist = 距离球的长度
	return function()
		local goalPos = CGeoPoint(param.pitchLength/2,0)
		local pos = ball.pos() + Utils.Polar2Vector(dist,(ball.pos() - goalPos):dir())
		return pos
	end
end

function ZshootGen(dist)	--（是一个可以击球的点）球与门的3点一线  dist = 距离球的长度
	return function()
		local SPos = -290
		local goalPos = CGeoPoint(param.pitchLength/2,-290)
		local pos = ball.pos() + Utils.Polar2Vector(dist,(ball.pos() - goalPos):dir())
		return pos
	end
end

function wdrag(p, targetPos)
	local ipos
	local itarget
	if type(p) == "function" or type(targetPos) == "userdata" then
		ipos = p
	elseif type(p) == "string" then
		ipos = function()
			return player.pos(p)
		end
	else
		ipos = CGeoPoint:new_local(9999*10,9999*10)
	end

	if type(targetPos) == "function" or type(targetPos) == "userdata" then
		itarget = targetPos
	elseif type(targetPos) == "string" then
		itarget = function()
			return player.pos(targetPos)
		end
	else
		itarget = CGeoPoint:new_local(9999*10,9999*10)
	end
	local mexe, mpos = WDrag{pos = p, target = itarget, srole = "drag"}
	return {mexe, mpos}
end
------------------------------------ 测试相关的skill ---------------------------------------

function openSpeed(vx, vy, vdir)
	local spdX = function()
		return vx
	end

	local spdY = function()
		return vy
	end
	
	local spdW = function()
		return vdir
	end

	local mexe, mpos = OpenSpeed{speedX = spdX, speedY = spdY, speedW = spdW}
	return {mexe, mpos}
end

function speed(vx, vy, vw,f)
	local ikick = kick.flat
	local idir = dir.specified(0)
	local ipre = pre.specified(10)
	local ikp = kp.specified(0)
	local icp = cp.specified(0)
	local iflag = f or flag.nothing

	-- local spdX = function()
	-- 	return vx
	-- end

	local spdX = vx
	local spdY = vy
	local spdW = vw


	-- local spdY = function()
	-- 	return vy
	-- end
	
	-- local spdW = function()
	-- 	return vdir
	-- end

	local mexe, mpos = Speed{speedX = spdX, speedY = spdY, speedW = spdW}
	return {mexe, mpos, ikick, idir, ipre, ikp, icp, iflag}
end
