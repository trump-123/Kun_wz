module(..., package.seeall)

function instance(role)
	local realIns
	if type(role) == "string" then
		realIns = vision:ourPlayer(gRoleNum[role])
	elseif type(role) == "number" then
--	and	role >= 1 and role <= param.maxPlayer then
		realIns = vision:ourPlayer(role)
	else
		print("Invalid role in player.instance!!!2222222")
		return nil
	end
	return realIns
end

function num(role)
	local retNum
	if type(role) == "string" then
		retNum = gRoleNum[role]
	elseif type(role) == "number" then
		retNum = role
	else
		print("Invalid role in player.instance!!!11111")
	end
	return retNum
end

function pos(role)
	return instance(role):Pos()
end

function posX(role)
	return instance(role):X()
end

function posY(role)
	return instance(role):Y()
end

function dir(role)
	return instance(role):Dir()
end

function vel(role)
	return instance(role):Vel()
end

function velDir(role)
	return vel(role):dir()
end

function velMod(role)
	return vel(role):mod()
end

function rotVel(role)
	return instance(role):RotVel()
end

function valid(role)
	return instance(role):Valid()
end

function valid1(role)
	return function()
		return valid(role)
	end
end

function myvalid(role)
	if num(role) ~= -1 then
		return true
	end
	return false
end

function toBallDist(role)
	return pos(role):dist(ball.pos())
end

function toBallDir(role)
	return (ball.pos() - pos(role)):dir()
end

function backBallDir(role)
	return (pos(role) - ball.pos()):dir()
end

function toTheirGoalDist(role)
	return pos(role):dist(CGeoPoint:new_local(param.pitchLength / 2.0, 0))
end

function toOurGoalDist(role)
	return pos(role):dist(CGeoPoint:new_local(-param.pitchLength / 2.0, 0))
end

function toTheirGoalDir(role)
	return (CGeoPoint:new_local(param.pitchLength / 2.0, 0) - pos(role)):dir()
end

function toPlayerDir(role1, role2)--role2不写role1是目标 写了role2 role2是目标
	if role2 == nil then
		return function(role2)
			return (player.pos(role1) - player.pos(role2)):dir()
		end
	else
		return (player.pos(role2) - player.pos(role1)):dir()
	end
end

function toPlayerHeadDir(role1, role2)
	if role2 == nil then
		return function(role2)
			local tmpPlayerHead = player.pos(role1) + Utils.Polar2Vector(76, player.dir(role1))
			return (tmpPlayerHead - player.pos(role2)):dir()
		end
	else
		local tmpPlayerHead = player.pos(role2) + Utils.Polar2Vector(76, player.dir(role2))
		return (tmpPlayerHead - player.pos(role1)):dir()
	end
end

function toPlayerDist(role1, role2)
	if role2 == nil then
		return function(role2)
			return (player.pos(role1) - player.pos(role2)):mod()
		end
	else
		return (player.pos(role2) - player.pos(role1)):mod()
	end
end

function toTargetTime(p,role)
	if role == nil then
		return function(role)
			local target
			if type(p) == "function" then
				target = p()
			else
				target = p
			end
			return world:timeToTarget(player.num(role), target) * param.frameRate
		end
	else
		local target
		if type(p) == "function" then
			target = p()
		else
			target = p
		end
		return world:timeToTarget(player.num(role), target) * param.frameRate
	end
end

function toTargetDist(role)
	local p
	if type(gRolePos[role]) == "function" then
		p = gRolePos[role]()
	else
		p = gRolePos[role]
	end
	return player.pos(role):dist(p)
end


function blocked()                     --2022-7-5 被对方拦截的情况  by应敏辉
   local pl = param.pitchLength
   local num  = -1
   local dist = 99999
   for i=0,param.maxPlayer-1 do
     if enemy.valid(i) then
       if  ( CGeoPoint:new_local(pl/2,0) - player.pos(i)):mod() < dist  then
          dist = (CGeoPoint:new_local(pl/2,0) - player.pos(i)):mod()
          num = i
       end
     end
   end

   if  enemy.toBallDist(num) < 120 
        and math.abs(enemy.toBallDir(num) - enemy.dir(num)) < math.pi/5 then
        return true
   end
        return false
end



function toTargetDir(p, role)--模板
	if role == nil then
		return function(role)
			return (p - player.pos(role)):dir()
		end
	else
		return function()
			if type(p) == "function" then
				return (p() - player.pos(role)):dir()
			else
				return (p - player.pos(role)):dir()
			end
		end
	end
end
--need to modify
function toTheirPenaltyDist(role)--到敌方禁区距离
	local tmpToGoalDist = (CGeoPoint:new_local(param.pitchLength/2.0,0) - player.pos(role)):mod()
	return tmpToGoalDist - 80
end

function toPointDist(role, p)
	local pos
	if type(p) == "function" then
		pos = p()
	else
		pos = p
	end
	return player.pos(role):dist(pos)
end

-- 用在冲球时求出传球点后面的一个点
function backShootPos(p)
	local tmpShootDir = (p - CGeoPoint:new_local(param.pitchLength/2.0,0)):dir()
	return p + Utils.Polar2Vector(9, tmpShootDir)
end

function toPointDir(p, role)
	if role == nil then
		if type(p) == "function" then
			return function ( role1 )
				return (p() - player.pos(role1)):dir()
			end
		else
			return function ( role1 )
				return (p - player.pos(role1)):dir()
			end
		end
	else
		return (p - player.pos(role)):dir()
	end
end

function kickBall(role)
	return world:IsBallKicked(num(role))
end

function infraredOn( role )--bool
	return world:IsInfraredOn(num(role))
end

function infraredCount( role )--num
	return world:InfraredOnCount(num(role))
end

function infraredOffCount( role )--num
	return world:InfraredOffCount(num(role))
end
-- role1为传球车
function toShootOrRobot(role1)--射门或者接球
	return function(role2)--runner
		local shootDir = ( CGeoPoint:new_local(param.pitchLength / 2.0, 0) - pos(role2) ):dir()
		local faceDir
		if toBallDist(role1) > 50 then
			faceDir = (ball.pos() - pos(role2)):dir()
		else
			faceDir = (pos(role1) - pos(role2)):dir()
		end
		if math.abs(Utils.Normalize(shootDir - faceDir)) > math.pi * 30 / 180 then
			return faceDir
		else
			return shootDir
		end
	end
end

function canBreak(role)--是否可以突破
	for i=0,param.maxPlayer-1 do
		if enemy.valid(i) then
			local p
			if type(gRolePos[role]) == "function" then
				p = gRolePos[role]()
			else
				p = gRolePos[role]
			end
			local breakSeg = CGeoSegment:new_local(player.pos(role), p)
			local projP = breakSeg:projection(enemy.pos(i))
			if breakSeg:IsPointOnLineOnSegment(projP) then
				if enemy.pos(i):dist(projP) < 400 then
					return false
				end
			end
		end
	end

	return true
end

function isMarked(role)--是否是被防守
	local closestDist = 99999
	local defennum
	for i=0,param.maxPlayer-1 do
		if enemy.valid(i) then
			local dir1 = player.toPointDir(CGeoPoint:new_local(param.pitchLength / 2.0, 0),role)--球员到敌方球门的角度
			local dirDiff = Utils.Normalize( dir1- player.toPointDir(enemy.pos(i),role))--到敌方球门角度减去到敌方机器人角度
			if math.abs(dirDiff) < math.pi/2 then
				local tmpDist = player.toPointDist(role,enemy.pos(i))
				if tmpDist < closestDist then
					closestDist = tmpDist
					defennum = i
				end
			end
		end
	end

	if closestDist < 400 then
		return true
	end
	return false
end

function DisMarked(role)--是否是被防守
	local closestDist = 99999
	local defennum
	for i=0,param.maxPlayer-1 do
		if enemy.valid(i) then
			local dir1 = player.toPointDir(CGeoPoint:new_local(param.pitchLength / 2, 0),role)--球员到敌方球门的角度
			local dirDiff = Utils.Normalize( dir1- player.toPointDir(enemy.pos(i),role))--到敌方球门角度减去到敌方机器人角度
			if math.abs(dirDiff) < math.pi/2 then
				local tmpDist = player.toPointDist(role,enemy.pos(i))
				if tmpDist < closestDist then
					closestDist = tmpDist
					defennum = i
				end
			end
		end
	end

	if closestDist < 800 then
		return true
	end
	return false
end

function testPassPos(role)--前后摇摆
	local factor
	return function ()
		if posX(role) > 0 then
			factor = -1
		else
			factor = 1
		end

		return CGeoPoint:new_local(150*factor, 0)
	end
end

function canFlatPassTo(role1, role2)--能否直接球员间传球
	local p1 = player.pos(role1)
	local p2 = player.pos(role2) + Utils.Polar2Vector(param.playerFrontToCenter,player.dir(role2))
	local seg = CGeoSegment:new_local(p1, p2)
	for i = 0, param.maxPlayer-1 do
		if enemy.valid(i) then
			local dist = seg:projection(enemy.pos(i)):dist(enemy.pos(i))
			local isprjon = seg:IsPointOnLineOnSegment(seg:projection(enemy.pos(i)))
			if dist < 200 and isprjon then
				return false
			end
		end
	end

	return true
end
--for autoball placement
function ifBlockBallLine( role, originpos, targetpos)--判断是否在放球胶囊区
	local p1
	local p2
	if type(originpos) == "function" then
		p1 = originpos()
	else
		p1 = originpos
	end
	if type(targetpos) == "function" then
		p2 = targetpos()
	else
		p2 = targetpos
	end
	local seg = CGeoSegment:new_local(p1,p2)
	local dist = seg:projection(player.pos(role)):dist(player.pos(role))
	local isprjon = seg:IsPointOnLineOnSegment(seg:projection(player.pos(role)))
	--print (role , dist,isprjon)
	if dist < 20 and isprjon then
		return true
	end
	return false
end

--for dynamicKick
function canFlatPassToPos(role, targetpos)--能否直接传球
	local p1 = player.pos(role)
	local p2
	local id
	if type(role) == "string" then
		id = player.num(role)
	else
		id = role
	end
	if type(targetpos) == "function" then
		p2 = targetpos()
	else
		p2 = targetpos
	end
	local seg = CGeoSegment:new_local(p1, p2)
	for i = 0, param.maxPlayer-1 do
		if enemy.valid(i) then
			local dist = seg:projection(enemy.pos(i)):dist(enemy.pos(i))
			local isprjon = seg:IsPointOnLineOnSegment(seg:projection(enemy.pos(i)))
			if dist < 140 and isprjon then
				return false
			end
		end
	end
	for j = 0, param.maxPlayer-1 do
		if player.valid(j) and j ~= id and player.pos(j):dist(p2) > 200  then--and j ~= gRoleNum["Leader"] and player.pos(j):dist(p2) > 20
			local dist = seg:projection(player.pos(j)):dist(player.pos(j))
			local isprjon = seg:IsPointOnLineOnSegment(seg:projection(player.pos(j)))
			if dist < 120 and isprjon then
				return false
			end
		end
	end
	return true
end

function GetBestPower(role1,role2)
	local p1 = player.pos(role1)
	local p2
	if type(role2) == "function" then
		p2 = role2()
	elseif type(role2) == "userdata" then
		p2 = role2
	else
		p2 = player.pos(role2)
	end
	local defence 
	local seg = CGeoSegment:new_local(p1, p2)
	for i = 0, param.maxPlayer-1 do
		if enemy.valid(i) then
			local dist = seg:projection(enemy.pos(i)):dist(enemy.pos(i))
			local isprjon = seg:IsPointOnLineOnSegment(seg:projection(enemy.pos(i)))
			if dist < 120 and isprjon then
				defence = i
			end
		end
	end
	if (enemy.pos(defence) - ball.pos()):mod() < 800 then
		return 250
	else
		return 300
	end
end

function canFlatReceive(passer,receiver)
	local p1 = player.pos(passer)
	local p2
	if type(receiver) == "function" then
		p2 = receiver()
	elseif type(receiver) == "string" or type(receiver) == "number" then
		p2 = player.pos(receiver) + Utils.Polar2Vector(param.playerFrontToCenter,player.dir(receiver))
	else
		p2 = receiver
	end
	p1 = player.pos(passer) + Utils.Polar2Vector(1000,(p2 - p1):dir())
	local seg = CGeoSegment:new_local(p1, p2)
	for i = 0, param.maxPlayer-1 do
		if enemy.valid(i) then
			local dist = seg:projection(enemy.pos(i)):dist(enemy.pos(i))
			local isprjon = seg:IsPointOnLineOnSegment(seg:projection(enemy.pos(i)))
			--debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,0),seg:projection(enemy.pos(5)):dist(enemy.pos(5)))
			if dist < 200 and isprjon then
				return false
			end
		end
	end
	-- for j = 0, param.maxPlayer-1 do
	-- 	if player.valid(j) and j ~= gRoleNum["Leader"] and player.pos(j):dist(p2) > 20 then
	-- 		local dist = seg:projection(player.pos(j)):dist(player.pos(j))
	-- 		local isprjon = seg:IsPointOnLineOnSegment(seg:projection(player.pos(j)))
	-- 		if dist < 120 and isprjon then
	-- 			return false
	-- 		end
	-- 	end
	-- end
	return true
end

-- d为前方的距离 proj_d是对方车投影到射门线的距离阈值
function canDirectShoot(role1, d, proj_d)--能否直接射门
	if d == nil then
		d = 70
	end
	if proj_d == nil then
		proj_d = 12
	end
	local p1 = player.pos(role1)
	local p2 = player.pos(role1) + Utils.Polar2Vector(d,player.dir(role1))
	local seg = CGeoSegment:new_local(p1, p2)
	for i = 0, param.maxPlayer-1 do
		if enemy.valid(i) then
			local dist = seg:projection(enemy.pos(i)):dist(enemy.pos(i))
			local isprjon = seg:IsPointOnLineOnSegment(seg:projection(enemy.pos(i)))
			if dist < proj_d and isprjon then
				return false
			end
		end
	end

	return true
end

function canChipPassTo(role1, role2)--能够挑射传球
	local p1 = player.pos(role1)
	local p2 = player.pos(role2)
	for i = 0, param.maxPlayer-1 do
		if enemy.valid(i) then
			local dist1 = enemy.pos(i):dist(p1)
			local dist2 = enemy.pos(i):dist(p2)
			if dist1 < 300 or dist2 < 300 then
				return false
			end
		end
	end

	return true
end

------------------------------------------------------
function isBallPassed(role1,role2)--是否正常传球 粗判断
	local p1 = player.pos(role1)
	local p2 = player.pos(role2)
	local ptrDir = ( p2 - p1 ):dir()
	if (math.abs(Utils.Normalize(ball.velDir() - ptrDir)) < math.pi / 18) and
	   (ball.velMod() > 800) then
		return true
	else
		return false
	end
end

function isBallPassedNormalPlay(role1,role2)--是否是正常传球
	local passerDir = player.dir(role1)
	local p1 = player.pos(role1)
	local p2 = player.pos(role2)
	local ptrDir = ( p2 - p1 ):dir()
	if (math.abs(Utils.Normalize(ball.velDir() - ptrDir)) < math.pi / 18) and
	   (math.abs(Utils.Normalize(ball.velDir() - passerDir)) < math.pi / 10) and
	   (ball.velMod() > 1600)  then
		return true
	else
		return false
	end
end

function passIntercept(role)--判断是否要拦截球
	local receiver = player.pos(role)
	local ptrDir = ( receiver - ball.pos()):dir()
	if ball.toPointDist(receiver) >50 then
		if math.abs(Utils.Normalize(ball.velDir() - ptrDir)) > math.pi / 10 or
			ball.velMod() < 120 then
			return true
		else
			return false
		end
	else
		return false
	end
end

------------------------------------------------------

-- p为传入的点
function antiYDir(p)
	return function (role)
		if type(p) == "userdata" then
			return (ball.antiYPos(p)() - player.pos(role)):dir()
		end
	end
end

-- function realNumExist(n)
-- 	if player.valid(CGetStrategyNum(n)) then
-- 		return true
-- 	end
-- 	return false
-- end

function faceball2target(role,t,diff)
  local target
  local d

  if diff == nil then
  	d = 0.2
  elseif type(diff) == "function" then
    d = diff()
  else
    d = diff
  end

  if type(t) == "function" then
    target = t()
  else
    target = t
  end

  local temp = ball.toPointDir(target)
   -- print(temp())
   --  print(player.dir(role))
   if math.abs(temp()-player.dir(role))<=d or math.abs(temp()-player.dir(role))>=6.28-d then
    return true
  else
    return false
  end
end

function faceball2target(role,t,diff)
  local target
  local d

  if diff == nil then
  	d = 0.2
  elseif type(diff) == "function" then
    d = diff()
  else
    d = diff
  end

  if type(t) == "function" then
    target = t()
  else
    target = t
  end

  local temp = ball.toPointDir(target)
   -- print(temp())
   --  print(player.dir(role))
   if math.abs(temp()-player.dir(role))<=d or math.abs(temp()-player.dir(role))>=6.28-d then
    return true
  else
    return false
  end
end

function isInForbiddenZone4ballplace(role)
  local thereShouldDist=60--规则要求50cm，这里设定大于规则
  local p1= CGeoPoint:new_local(ball.placementPos():x(), ball.placementPos():y())
  local p2 =ball.pos()
  local seg = CGeoSegment:new_local(p1,p2)
  local dist = seg:projection(player.pos(role)):dist(player.pos(role))
  local isprjon = seg:IsPointOnLineOnSegment(seg:projection(player.pos(role)))
   if player.toBallDist(role)<=thereShouldDist or player.toPointDist(role,p1)<=thereShouldDist or (dist<=thereShouldDist and isprjon) then
     return true
  else
    return false
  end
end

function stayPos4ballplace(role)
		local thereShouldDist=70
        local myposX
        local myposY
        local TargetPos = function ()
        	return CGeoPoint:new_local(ball.placementPos():x(), ball.placementPos():y())
		end
        local getPos = function()
                if player.isInForbiddenZone4ballplace(role) then
                   local seg = CGeoSegment:new_local(ball.pos(),TargetPos())
                   local projectPoint=CGeoPoint:new_local(seg:projection(player.pos(role)):x(),seg:projection(player.pos(role)):y())
                    myposX = (projectPoint + Utils.Polar2Vector(-thereShouldDist,player.toPointDir(projectPoint,role))):x()
                    myposY =(projectPoint + Utils.Polar2Vector(-thereShouldDist,player.toPointDir(projectPoint,role))):y()
                    if ((math.abs(myposX)>(param.pitchLength/2-param.penaltyDepth)) and 
                      math.abs(myposY)<(param.penaltyWidth/2))
                      or (math.abs(myposX) > param.pitchLength/2) 
                      or (math.abs(myposY) > param.pitchWidth/2) 
                      then 
                      myposX = (projectPoint + Utils.Polar2Vector(thereShouldDist,player.toPointDir(projectPoint,role))):x()
                      myposY =(projectPoint + Utils.Polar2Vector(thereShouldDist,player.toPointDir(projectPoint,role))):y()
                    end
                else
                        myposX = player.posX(role)
                        myposY = player.posY(role)

                end
        end
        local compute = function()
                getPos()
                return CGeoPoint:new_local(myposX,myposY)
        end
        return compute
end

----------------------------自己封装的函数--------------------------------

function shootGen(dist,dir)--与球有关站在球附近的位置 dist为与球距离 dir为在球的什么方向
	return function()
		local theirgoal = CGeoPoint:new_local(param.pitchLength/0,0)
		local pos
		local idir
		if dir == nil then
			idir = (ball.pos() - theirgoal):dir()
		else
			if type(dir) == "function" then --判断传进来的dir是不是一个function lua的function相当于c语言的函数 及变量 不进行判断dir默认当常量处理变量传进来会出错
				  idir = dir()
			else
				  idir = dir
			end
		end
	    pos  =  ball.pos() + Utils.Polar2Vector(dist,idir)--utils库中具体查看polar2vctor的作用，和向量有关，朝dir方向位移dist距离
	       return pos
    end
end

function jshootGen(dist,dir)--与球有关站在球附近的位置 dist为与球距离 dir为在球的什么方向
		local theirgoal = CGeoPoint:new_local(param.pitchLength/0,0)
		local pos
		local idir
		if dir == nil then
			idir = (ball.pos() - theirgoal):dir()
		else
			if type(dir) == "function" then --判断传进来的dir是不是一个function lua的function相当于c语言的函数 及变量 不进行判断dir默认当常量处理变量传进来会出错
				  idir = dir()
			else
				  idir = dir
			end
		end
	    pos  =  ball.pos() + Utils.Polar2Vector(dist,idir)--utils库中具体查看polar2vctor的作用，和向量有关，朝dir方向位移dist距离
	       return pos
end

function toplayerdir1(receiver)--两机器人之间的朝向  为role1朝向role2的角度,用作运行函数内
	return function(passer)
		local goalPos = CGeoPoint(param.pitchLength/2,0)--4500
		local dir 
		    dir = toPlayerDir(passer,receiver)
		return dir
	end
end

function togoaldir(role) --中场射门机器人需要站的位置的向量角度 具体自己稍微理解一下
	return function()
	local dir
	local upgoal = CGeoPoint(param.pitchLength/2,param.goalWidth/2-200)
	local down   = CGeoPoint(param.pitchLength/2,-param.goalWidth/2+200)
	if enemy.posY(enemy.findgoalie())>0 then
	    dir = (pos(role) - down):dir()
	else
		  dir = (pos(role) - upgoal):dir()
	end
	return dir
end
end

local togoaldir1 = function() --中场射门角度判断，前场不适用
	return function(role)
	local dir
	local upgoal = CGeoPoint(param.pitchLength/2,param.goalWidth/2-200)
	local down   = CGeoPoint(param.pitchLength/2,-param.goalWidth/2+200)
	if enemy.posY(findgoalie())>0 then
	    dir = (down - pos(role)):dir()
	else
		  dir = (upgoal - pos(role)):dir()
	end
	return dir
end
end

local targetpos = function()
	local upgoal = CGeoPoint(param.pitchLength/2,param.goalWidth/2-200)
	local down   = CGeoPoint(param.pitchLength/2,-param.goalWidth/2+200)
	local gate   = CGeoPoint(param.pitchLength/2,0)
	if enemy.posY(findgoalie())>0 then
	    return down 
	elseif enemy.posY(findgoalie())<0 then 
		return upgoal 
	else
		return gate
	end
end

function todir(role) --前场射门角度 具体为什么写这么多是因为考虑到射死角的问题 引用函数建议ctrl p 搜索一下查看 比如ball.，player.
	return function()
	local goalPos     = CGeoPoint(param.pitchLength/2,0)
	local up          = CGeoPoint(param.pitchLength/2,param.goalWidth/2 - 50)
	local down        = CGeoPoint(param.pitchLength/2,-param.goalWidth/2 + 50)
	local upgoalPos   = CGeoPoint(param.pitchLength/2,param.goalWidth/2 - 150)
	local downgoalPos = CGeoPoint(param.pitchLength/2,-param.goalWidth/2 + 150)

  if enemy.findgoalie() == -1 then
    if player.canFlatPassToPos(role,goalPos) then
			return (goalPos - player.pos(role)):dir()
		elseif player.canFlatPassToPos(role,up) then
			return (upgoalPos - player.pos(role)):dir()
		elseif player.canFlatPassToPos(role,down) then
			return (downgoalPos - player.pos(role)):dir()
		else
	    return (goalPos - player.pos(role)):dir()
    end
  else
  	if enemy.posY(enemy.findgoalie()) > 0 then
  		 return (downgoalPos - player.pos(role)):dir()
  	else
  		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),1)
  		 return (upgoalPos - player.pos(role)):dir()
  	end
  end
  end
end

function BestShootPoint(role) --前场射门角度 具体为什么写这么多是因为考虑到射死角的问题 引用函数建议ctrl p 搜索一下查看 比如ball.，player.
	return function()
	local goalPos     = CGeoPoint(param.pitchLength/2,0)
	local up          = CGeoPoint(param.pitchLength/2,param.goalWidth/2 - 200)
	local down        = CGeoPoint(param.pitchLength/2,-param.goalWidth/2 + 50)
	local upgoalPos   = CGeoPoint(param.pitchLength/2,param.goalWidth/2 - 200)
	local downgoalPos = CGeoPoint(param.pitchLength/2,-param.goalWidth/2 + 200)

  if enemy.findgoalie() == -1 then
    if player.canFlatPassToPos(role,goalPos) then
			return goalPos 
		elseif player.canFlatPassToPos(role,up) then
			return upgoalPos
		elseif player.canFlatPassToPos(role,down) then
			return downgoalPos
		else
	    return goalPos 
    end
  else
  	if enemy.posY(enemy.findgoalie()) > 0 then
  		 return downgoalPos
  	else
  		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),1)
  		 return upgoalPos
  	end
  end
  end
end


function standpos(role) --动态接球站位
	return function()
	  local standjud
	  local stand
	  local standline
	  local line1 = CGeoLine(ball.pos(),ball.velDir()) 
	  local pos   = line1:projection(player.pos(role)+Utils.Polar2Vector(param.playerFrontToCenter+30,Utils.Normalize(player.dir(role))))
      standjud = pos + Utils.Polar2Vector(param.playerFrontToCenter+30,Utils.Normalize(player.dir(role) + math.pi))
	  if math.abs(standjud:y()) > param.pitchWidth/2 then
	  	if standjud:y() > param.pitchWidth/2 then
	  		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),"1")
	  	     standline = CGeoLine:new_local(CGeoPoint:new_local(0,param.pitchWidth/2-50),0)
	  	     stand = CGeoLineLineIntersection:new_local(standline,line1):IntersectPoint()
	    else
	    	debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),"2")
	    	 standline = CGeoLine:new_local(CGeoPoint:new_local(0,-param.pitchWidth/2+50),0)
	  	     stand = CGeoLineLineIntersection:new_local(standline,line1):IntersectPoint()
	    end
	  elseif math.abs(standjud:x()) > param.pitchLength/2 then
	  	if standjud:x() > param.pitchLength/2 then
	  	     standline = CGeoLine:new_local(CGeoPoint:new_local(param.pitchLength/2-50,0),math.pi/2)
	  	     stand = CGeoLineLineIntersection:new_local(standline,line1):IntersectPoint()
	  	else
	  		 standline = CGeoLine:new_local(CGeoPoint:new_local(-param.pitchLength/2+50,0),math.pi/2)
	  	     stand = CGeoLineLineIntersection:new_local(standline,line1):IntersectPoint()
	  	end
	  else
	  	stand = standjud
	  end
	  return stand
    end
end

function standposV1(role) --动态接球站位
	return function()
	  local ipos
	  local line1 = CGeoLine(ball.pos(),ball.velDir()) 
	  local pos   = line1:projection(player.pos(role))
	 -- if player.toBallDist(role) > 150 then
	      ipos = pos
	  -- else
	  -- 	  ipos = ball.pos()
	  -- end
	  return ipos
    end
end

function standposV2(role) --动态接球站位
	return function()
	  local ipos
	  local ballpos = ball.pos()
	  local line1   = CGeoLine(ball.pos(),ball.velDir()) 
	  local pos     = line1:projection(player.pos(role) + Utils.Polar2Vector(90,ball.velDir() + math.pi))
	  if ball.velMod() > 1000 then
	      ipos = pos
	  else
	  	  ipos = ball.pos()
	  end
	  return ipos
    end
end

function standpos1(role)--同上
	return function()
	  local ipos 
	  local line1 = CGeoLine(ball.pos(),ball.velDir()) 
	  local pos   = line1:projection(player.pos(role))
	  local stand = pos+Utils.Polar2Vector(param.playerFrontToCenter,Utils.Normalize(player.dir(role) + math.pi))
	  if player.toBallDist(role) > 150 then
	      ipos = stand
	  else
	  	  ipos = ball.pos()
	  end
	  return stand
    end
end

function shootorpass(role1,role2)--role1为传球车
	local goal = CGeoPoint(param.pitchLength/2,0)
	local idir
	local shootdir = (goal - player.pos(role2)):dir()
	if canshoot(role2) then
	    idir = player.dir(role2)
	else
        idir = shootdir
	end
	
	local facedir
	if player.toBallDist(role1) > 105 then
		facedir = (ball.pos() - (player.pos(role2) + Utils.Polar2Vector(105,player.dir(role2)))):dir()
	else
		facedir = (player.pos(role1) - (player.pos(role2) + Utils.Polar2Vector(105,player.dir(role2)))):dir()
	end
	--debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,1000),facedir)
	if math.abs(Utils.Normalize(idir - facedir)) > math.pi*60/180 then--math.pi=3.14/180*60
		return false
	else
		return true
	end
end

function balltoplayer(role)
	local goal = CGeoPoint:new_local(param.pitchLength/2,0)
	local ipos
	local idir
	if role ~= nil then
		return function()
			if type(role) == "function" then
				ipos = role()
			elseif type(role) == "userdata" then
				ipos = role
			else
				ipos = player.pos(role) + Utils.Polar2Vector(76,player.dir(role))
			end
			idir =  (ball.pos() - ipos):dir()
			return idir
		end
	else
		return function()
			    idir =  (ball.pos()-goal):dir()
			    return idir
		end
	end
end

--fasle 是动态拿球 true是静态拿球 静态拿球比动态拿球快 但是很容易失球
function jgetbmod()
	for i=0,param.maxPlayer-1 do
		if enemy.valid(i) then
			if (enemy.pos(i) - ball.pos()):mod()<200 or ball.velMod()>1000 then
				return false
			end
		end
	end
	return true
end

function realNumExist(n)
	if valid(n) then
		return true
	end
	  return false
end

function backballpos(role)
return function()
	  --local line1 = CGeoLine(ball.pos(),ball.velDir()) 
	  --local pos   = line1:projection(player.pos(role)+Utils.Polar2Vector(param.playerFrontToCenter,Utils.Normalize(player.dir(role))))
	  local stand = ball.pos()+Utils.Polar2Vector(param.playerFrontToCenter,Utils.Normalize(player.dir(role) + math.pi))
	  return stand
    end
end


function canshoot(role)
	local ipos = player.pos(role)
	local idir = player.dir(role)
	local upgoal = CGeoPoint:new_local(param.pitchLength/2,param.goalWidth/2)
	local downgoal = CGeoPoint:new_local(param.pitchLength/2,-param.goalWidth/2)
	if idir >  (downgoal - ipos):dir() and idir < (upgoal - ipos):dir() then 
		return true
	else
		return false
	end
end

-- function Best()
-- 	local best
-- 	for i=0,param.maxPlayer-1 do
-- 		if valid(i) then
-- 			if 
-- 		end
-- 	end
-- end
-- function BestShootPoint(role)          --2022-7-5  by应敏辉
--    local pl = param.pitchLength
--    local ToGoalPoint = {
--     CGeoPoint:new_local(pl/2,250),
--     CGeoPoint:new_local(pl/2,-250),  
-- }

--    local num  = -1
--    local dist = 99999
--    for i=0,param.maxPlayer-1 do
--      if enemy.valid(i) then
--        if  ( CGeoPoint:new_local(pl/2,0) - player.pos(i)):mod() < dist  then
--           dist = (CGeoPoint:new_local(pl/2,0) - player.pos(i)):mod()
--           num = i
--        end
--      end
--    end


--      if enemy.posY(num) >0  then
--         return ToGoalPoint[1]
--      else
--         return ToGoalPoint[2]
--      end


-- end


-----------------------没有用的函数---------------------------


function InfoControlled(role)
	local roleNum = player.num(role)
	--local isVisionBallControlled = skillUtils:withBall() and (skillUtils:getOurBestPlayer() == roleNum)
	--print("withBall: ", skillUtils:withBall())
	--print("BestPlayer: ", skillUtils:getOurBestPlayer() == roleNum)
	--if skillUtils:getOurBestPlayer() == roleNum then
	--	return true
	--else
	--	return false
	--end
	return skillUtils:getOurBestPlayer() == roleNum
end