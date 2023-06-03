module(..., package.seeall)

function instance(role)
	if type(role) == "number" then
		return vision:theirPlayer(role)
	else
		print("Invalid role in enemy.instance!!!")
	end
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


function toBallDir(role)
	return (ball.pos() - pos(role)):dir()
end

function toBallDist(role)
	return pos(role):dist(ball.pos())
end


function attackNum()
	return defenceInfo:getAttackNum()
end

local lastnum = 0
function myattackNum()
	local num = 0
	for i=0,param.maxPlayer-1 do
		if valid(i) then
			if posX(i) < param.pitchLength/8 then
				num = num + 1
			end
		end
	end
	lastnum = num
	if num == 0 then
		return 1
	else
		return num
	end
end

function myattackNum1()
	local num = 0
	for i=0,param.maxPlayer-1 do
		if valid(i) then
			if posX(i) < -param.pitchLength/8 then
				num = num + 1
			end
		end
	end
	return num
end

function situChanged()
 	return defenceInfo:getTriggerState()
end

function mysituChanged()
 	local num = 0
	for i=0,param.maxPlayer - 1 do
		if valid(i) then
			if posX(i) < -param.pitchLength/8 then
				num = num + 1
			end
		end
	end
	if lastnum~=num then
		return true
	else
		return false
	end
end

--need to modify
function isGoalie(role)
	if pos(role):dist(CGeoPoint:new_local(param.pitchLength / 2.0, 0)) < 850 then
		return true
	end
	return false
end
--need to modify
function isDefender(role)--是否是后场防守
	if pos(role):dist(CGeoPoint:new_local(param.pitchLength / 2.0, 0)) < 120 and not isGoalie(role) then
		return true
	end
	return false
end

function isMarking(role)
	if pos(role):dist() and not isDefender(role) then
		return true
	end
	return false
end

function isAttacker(role)  --判断敌方机器人是否是进攻机器人
	if posX(role) < 0 and not isMarking(role) then
		return true
	end
	return false
end

function isBallFacer(role) --是否是面向球的机器人
	if pos(role):dist(ball.pos()) < 600 then
		return true
	end
	return false
end

function hasReceiver() --有敌方接球者
	return CEnemyHasReceiver()
end

gEnemyMsg = {
	-- 门将的位置（包括消失处理）
	goaliePos = CGeoPoint:new_local(param.pitchLength/2.0,0)
}

function updateCorrectGoaliePos()--更新敌方门将坐标
	local theirGoalieNum = skillUtils:getTheirGoalie()
	if enemy.valid(skillUtils:getTheirGoalie()) then
		gEnemyMsg.goaliePos = enemy.pos(theirGoalieNum)
	end
	return gEnemyMsg.goaliePos
end

function getTheirGoaliePos()
	return gEnemyMsg.goaliePos
end

--get opp best to ball num
function nearest()
	local nearDist = 99999
	local nearNum = 0
	for i=0,param.maxPlayer-1 do
		local theDist = enemy.pos(i):dist(ball.pos())--到球距离
		if enemy.valid(i) and nearDist > theDist then
			nearDist = theDist
			nearNum = i
		end
	end
	return pos(nearNum), dir(nearNum)
end

local iNum = 0
function nearest1()
		local nearDist = 99999
		local nearNum = 0
		for i=0,param.maxPlayer-1 do
			local theDist = enemy.pos(i):dist(ball.pos())--到球距离
			if enemy.valid(i) and nearDist > theDist then
				nearDist = theDist
				nearNum = i
			end
		end
		iNum = nearNum
		return pos(nearNum)
end

function nearNum()
	local nearDist = 99999
	local nearNum = 0
	for i=0,param.maxPlayer-1 do
		local theDist = enemy.pos(i):dist(ball.pos())--到球距离
		if enemy.valid(i) and nearDist > theDist then
			nearDist = theDist
			nearNum = i
		end
	end
	return nearNum
end

function markPos()
	return pos(iNum)
end

function findgoalie() --寻找地方守门员的车号
	for i=0,param.maxPlayer-1 do
		if enemy.valid(i) then
			if enemy.isGoalie(i) then
				return i
			end
		end
	end
	return -1
end

function shootp() --射门判断 具体看我是怎么用的
	local pos
	local upgoal = CGeoPoint(param.pitchLength/2,param.goalWidth/2-100)
	local down   = CGeoPoint(param.pitchLength/2,-param.goalWidth/2+100)
	if posY(findgoalie())>0 then
	    pos = down
	else
		pos = upgoal
	end
	return pos
end

function togoaldirjud(role)--用作判断 具体查看我是怎么用的
	local dir
	local upgoal = CGeoPoint(param.pitchLength/2,param.goalWidth/2-100)
	local down   = CGeoPoint(param.pitchLength/2,-param.goalWidth/2+100)
	if enemy.posY(findgoalie())>0 then
	    dir = (down - player.pos(role)):dir()
	else
		dir = (upgoal - player.pos(role)):dir()
	end
	return dir
end

function isnearball()
	for i = 0,param.maxPlayer-1 do
		if valid(i) and (pos(i) - ball.pos()):mod()<300 then
			return true
		end
	end
	return false
end

function judgetbest()
	local up = 0
	local down = 0
	for i = 0,param.maxPlayer-1 do
		if valid(i) then 
		     if enemy.posY()<0 then
			     down = down+1
			 else
			 	up = up+1
			 end
		end
	end
	if down>up then
		return true
	else
		return false
	end
end

function judthierget()
	for i = 0,param.maxPlayer-1 do
		if valid(i) then
			if (enemy.pos(i) - ball.pos()):mod() < 105 
				and math.abs(enemy.dir(i) - (ball.pos() - enemy.pos(i)):dir()) < math.pi/8 then
					return true
			end
		end
	end
	return false
end

function penaltyjud(role,p)
	local p1 = player.pos(role)
	local p2
	local their = enemy.pos(findgoalie())
	if type(p) == "function" then
		p2 = p()
	elseif type(p) == "userdata" then
		p2 = p
	else
		p2 = player.pos(p)
	end
	local line = CGeoSegment:new_local(p1,p2)
	local dist = line:projection(their):dist(their)
	local isprjon = line:IsPointOnLineOnSegment(line:projection(their))
	if dist < 300 then
		return false
	else
		return true
	end
end

local lastnum = -1
function findenemy()
	if ball.posY() > 0 then
		for i = 0,param.maxPlayer-1 do
			if valid(i)and posY(i)<0 then
				lastnum = i
				return pos(i)
			end
		end
	else
		for i = 0,param.maxPlayer-1 do
			if valid(i)and posY(i)>0 then
				lastnum = i
				return pos(i)
			end
		end
	end
	return nil
end

 realnum = 16
function enemypos1()
	local num = -1
	local ipos
	--debugEngine:gui_debug_msg(CGeoPoint:new_local(100,0),realnum)
	if not isBallFacer(realnum) and realnum~=16 and enemy.posX(realnum) < param.pitchLength/8 then
	    return enemy.pos(realnum)
	else
		realnum = 16
	end
	if realnum == -1 then
		if enemy.findenemy() ~= nil then
			realnum = lastnum
			return enemy.findenemy()
		else
			for i=0,param.maxPlayer-1 do
				if enemy.valid(i) then
					if i~= enemy.findgoalie()then
						if enemy.posX(i) > 0 then
							if not enemy.isBallFacer(i) then
								realnum = i
						  	return enemy.pos(i)
						  end
						-- else
						-- 	if not enemy.isBallFacer(i) then
						-- 		num = i
						-- 		realnum = num
						-- 	end
						end
					end
				end
			end
		end
		if enemy.posX(realnum)<-6500  or enemy.isBallFacer(realnum) then
			return CGeoPoint:new_local(-200,param.pitchWidth/4)
		else
			debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,0),1)
			return enemy.pos(realnum)
		end
	else
		    return enemy.pos(realnum)
	end
end


function Leftpos()
	for i = 0,param.maxPlayer - 1 do
		if enemy.valid(i) then
			if enemy.posX(i) < param.pitchLength/8 then
				if enemy.posY(i) <= 0 and enemy.pos(i):dist(ball.pos()) > 1000 then
					return enemy.pos(i)
				end
			end
		end
	end
	return CGeoPoint:new_local(150,-param.pitchWidth/4-500)
end

function Rightpos()
	for i = 0,param.maxPlayer - 1 do
		if enemy.valid(i) then
			if enemy.posX(i) < param.pitchLength/8 then
				if enemy.posY(i) > 0 and enemy.pos(i):dist(ball.pos()) > 1000 then
					return enemy.pos(i)
				end
			end
		end
	end
	return CGeoPoint:new_local(150,param.pitchWidth/4+500)
end

function getneddpos(str,max)
	return function()
		local this = 0
		local topos = {}
		local num
		if str == "Zero" then
			num = 0
		elseif str == "First" then
			num = 1
		elseif str == "Second" then
			num = 2
		elseif str == "Third" then
			num = 3
		elseif str == "Fourth" then
			num = 4
		elseif str == "Fifth" then
			num = 5
		elseif str == "Sixth" then
			num = 6
		elseif str == "Seventh" then
			num = 7
		elseif str == "Eighth" then
			num = 8
		elseif str == "Nineth" then
			num = 9
		elseif str == "Tenth" then
			num = 10
		else
			print("Error Priority in Marking Skill!!!!!")
		end

		for i=0,param.maxPlayer-1 do
			if valid(i) then
				if not isBallFacer(i) and posX(i) < max then
					this = this+1
					topos[this] = pos(i)
				end
			end
		end
		return topos[num]
	end
end

--------------------没有用的函数--------------------
function best()
	return skillUtils:getTheirBestPlayer()
end

function bestVelMod()
	return velMod(best())
end

function bestPos()
	return pos(best())
end

function bestDir()
	return dir(best())
end

function bestToBallDist()
	return pos(best()):dist(ball.pos())
end

function bestToBallDir()
	return (ball.pos() - pos(best())):dir()
end
