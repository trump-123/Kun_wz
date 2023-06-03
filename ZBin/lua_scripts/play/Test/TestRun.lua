local testPos = {
	CGeoPoint:new_local(2000,-1500),
	CGeoPoint:new_local(1900,1600),
	CGeoPoint:new_local(0,0),
	CGeoPoint:new_local(param.pitchLength/2.0,0),
}
local getpos = function(x,y) --获取机器人行进点 这个是静态点 后续需添加算法赋动态点
	return function()
		local pos
		    pos = CGeoPoint:new_local(x,y)
		return pos
	end
end	

local ballvector = function(role)
	return function()
		local dir = (ball.pos()- player.pos(role)):dir()
		return ball.pos() + Utils.Polar2Vector(50,dir)
	end
end

local TOTAL = 4
local speed = 0
local radius = 300
local enemyPos = function(num)
	
	return function()
		return enemy.pos(num)
	end
end

local standpos = function(role)
	return function()
		local x = CGeoLine:new_local(ball.pos(),ball.velDir()):projection(player.pos(role))
		return x
	end
end

local start = function(n,TOTAL,pos)
	
	return function()
		ipos = pos
		if type(pos) == 'function' then
			ipos = pos()
		else
			ipos = pos
		end

		local  centerX = ipos:x()
		local  centerY = ipos:y()
		return CGeoPoint:new_local(centerX + radius * math.cos(n*math.pi*2/TOTAL),centerY + radius * math.sin(n*math.pi*2/TOTAL))
	end
end

gPlayTable.CreatePlay{

firstState = "attack1",

["attack1"] = {
	switch = function()
	if ball.posX() < 100 then
		return "defend"
	end
	debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),player.infraredCount(3))
	if player.infraredCount("Leader") > 5 then
		return "wori"
	end  
		
end,
	["Leader"] = task.naqiu(),
	["Assister"] = task.goCmuRush(testPos[1]),
	--Middle = task.shoot(start(2,TOTAL,enemyPos(0))),
	match = "{LAM}"
},

["wori"] = {
    switch = function()
	 if ball.posX() < 100 then
			return "defend"
	 	else
		    return "attack"
	    end
end,
	Leader = task.shoot(ball.pos),
	Assister = task.shoot(ball.pos),
	match = "{L}"
},

["defend"] = {
    switch = function()
		
		if player.infraredCount("Leader") > 5 then
		return "chuanA"
		end 
	end,
	["Leader"] = task.naqiu(),
	["Assister"] = task.goCmuRush(testPos[2],player.toShootOrRobot("Leader"),5000,f),
	--["Middle"] = task.goCmuRush(getpos(0,-1900),player.todir("Middle"),5000),
	match = "{LAM}"
},

["chuanA"] = {
	switch = function()
		if player.kickBall("Leader") or ball.velMod()>1000 then
			return "Anaqiu"
		end
	end,
	["Leader"] = task.flatpass(player.shootGen(50,player.balltoplayer("Assister")),"Assister"),
	["Assister"] = task.goCmuRush(testPos[2],player.toShootOrRobot("Leader"),5000,f),
	--["Middle"] = task.shoot(start(2,TOTAL,enemyPos(0))),
	match = "{LAM}"
},

["Anaqiu"] = {
	switch = function()
		if player.toPointDist("Assister",ball.pos()) < 100 then
			return "Azhuanshen"
		end
	end,
	["Leader"] = task.shoot(start(2,TOTAL,enemyPos(0))),
	["Assister"] = task.goCmuRush(player.standpos("Assister"),player.toShootOrRobot("Leader"),10000,f),
	--["Middle"] = task.shoot(start(2,TOTAL,enemyPos(1))),
	match = "{LAM}"
},

["Azhuanshen"] = {
	switch = function()
		 if player.infraredCount("Assister") > 5 then
		 	return "wori"
		 end
	end,
	["Leader"] = task.shoot(start(2,TOTAL,enemyPos(0))),
	["Assister"] = task.naqiu(),
	--["Middle"] = task.goCmuRush(getpos(0,-1900),player.todir("Middle"),5000),
	match = "{LAM}"
},


name = "TestRun",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
