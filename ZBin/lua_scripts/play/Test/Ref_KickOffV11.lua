local f = flag.dribbling + flag.allow_dss
local DSS_FLAG = flag.allow_dss + flag.dodge_ball --+ flag.dribbling
local shootpower = 650
local passpower = 300

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

gPlayTable.CreatePlay{

firstState = "advance",

--advance这个脚本是因为程序开始前需要进行车号配对，但由于第一次配对前车是没有固定车号的，没有固定车号那么就不能进行第一次跑点操作，
--这个状态的意义在于给车赋初始车号使后续可以进行球员实时配对  {}是匹配一次车号 []是实时配对车号 使用者可以尝试一下 在状态最下栏match处修改
["advance"]={
	switch = function()
		return "getball"
	end,
	["Leader"] = task.goCmuRush(ball.pos),
	["a"]      = task.goCmuRush(getpos(-300,1900),player.togoaldir1,5000,DSS_FLAG),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,DSS_FLAG),
	match = "{abL}"
},
--leader 拿球并转身传给a车，判断条件自己看，为不作具体解答，以下都自己看
["getball"] = {
	switch = function()
		-- local x = (player.pos("Leader") - player.pos("a")):dir() - player.toShootOrRobot("a")("Leader") 
		-- debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x)
		-- if player.toTargetDist("Leader")<30 then
		-- 	return "touchball"
		-- end
		-- if ball.velMod()>1000 then
		-- --	return "a_getball"
		-- end
	end,
	["Leader"] = task.goCmuRush(player.shootGen(220,player.balltoplayer("a")),dir.playerToBall,5000,DSS_FLAG),
	["a"]      = task.goCmuRush(getpos(-300,1900),player.toShootOrRobot("Leader"),5000,DSS_FLAG),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,DSS_FLAG),
	match = "{abL}"
},

["touchball"] = {
	switch = function()
		if player.infraredCount("Leader")>5  then
			return "waitpass"
		end
	end,
	["Leader"] = task.goCmuRush(player.shootGen(50,player.balltoplayer("a")),dir.playerToBall,5000,f),
	["a"]      = task.goCmuRush(getpos(-300,1900),player.toShootOrRobot("Leader"),5000,DSS_FLAG),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,DSS_FLAG),
	match = ""
},

["waitpass"] = {
	switch = function()
	-- 	if cond.isNormalStart() then
	-- 	if player.toTargetDist("a")<50 then
	-- 			if player.canFlatPassTo("Leader","a") then
	-- 			     return "flatpass"
	-- 			else
	-- 				 return "chippass"
	-- 			end
	-- 	end
	-- end
	end,
	["Leader"] = task.goCmuRush(player.pos,ball.toPlayerHeadDir("a"),5000,f),
	["a"]      = task.goCmuRush(getpos(-300,1900),player.toShootOrRobot("Leader"),5000,DSS_FLAG),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,DSS_FLAG),
	match = ""
},

--leader 平射传球给a车
["flatpass"] = {
	switch = function()
		--local x = kp.toTarget(1)
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x)
		if player.kickBall("Leader") or ball.velMod()>1000 then
			return "waitshoot"
		end
	end,
	["Leader"] = task.flatpass(player.shootGen(50,player.balltoplayer("a")),"a"),
	["a"]      = task.goCmuRush(player.pos,player.toShootOrRobot("Leader"),5000,f),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000),
	match = ""
},

--leader 挑射传球给a车
["chippass"] = {
	switch = function()
		--local x = kp.toTarget(1)
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x)
		if player.kickBall("Leader") or ball.velMod()>1000 then
			return "waitshoot"
		end
	end,
	["Leader"] = task.chippass(player.shootGen(50,player.balltoplayer("a")),"a"),
	["a"]      = task.goCmuRush(player.pos,player.toShootOrRobot("Leader"),5000,f),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000),
	match = ""
},

--a接球等待射门
["waitshoot"] = {
	switch = function()
		if player.infraredCount("a")>5 and player.toBallDist("a")<105 then
		     if player.canFlatPassToPos("a",enemy.shootp()) then--检测是否能直接射门，能就转向射门
                 return "turnhead"
		     else                                         --不能就传球
		     	if player.jgetbmod() then                        --静态拿球
			        return "a_antigetball"
			    else                                      --动态拿球
                    return "a_getball"
                end
			 end
		end
		if bufcnt(player.toBallDist("a")>105 or math.abs(player.toBallDir("a"))>math.pi/6,60) then
			if player.jgetbmod() then                        --静态拿球
                return "a_antigetball"
			else                                      --动态拿球
                return "a_getball"
            end
		end
	end,
	["Leader"] = task.goCmuRush(player.pos),
	["a"]      = task.goCmuRush(player.standpos("a"),player.toShootOrRobot("Leader"),10000,f),
	["b"]      = task.goCmuRush(getpos(param.pitchLength/4,-1900),player.todir("b")),
	match = ""
},

--a转向射门
["turnhead"] = {
		switch = function()
		if math.abs(player.dir("a")- enemy.togoaldirjud("a"))<math.pi/90 then
			return "shoot"
		end
		if bufcnt(player.toBallDist("a")>105 or math.abs(player.toBallDir("a"))>math.pi/8,20) then
			if player.jgetbmod() then                        --静态拿球
                return "a_antigetball"
			else                                      --动态拿球
                return "a_getball"
            end
		end
	end,
	["Leader"] = task.goCmuRush(player.pos),
	["a"]      = task.goCmuRush(player.shootGen(50,player.togoaldir("a")),player.togoaldir1,10000,f),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,DSS_FLAG),
	match = "{LB}[a]"
},

--a射门
["shoot"] = {
	switch = function()
		if bufcnt(player.toBallDist("a")>105 or math.abs(player.toBallDir("a"))>math.pi/8,50) then
			if player.jgetbmod() then                        --静态拿球
                return "a_antigetball"
			else                                      --动态拿球
                return "a_getball"
            end
		end
		if player.kickBall("a") and ball.velMod()>1000 then
		   return "getball"
		end
	end,
	["a"] = task.shoot(player.shootGen(50,player.togoaldir("a")),player.togoaldir1,_,shootpower),
	["Leader"] = task.goCmuRush(getpos(param.pitchLength/4,param.pitchWidth/2+200),player.toShootOrRobot("a"),6000,f),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,f),
	match = ""
},

--静态拿球
["a_antigetball"] = {
	switch = function()
		local x = player.infraredCount("a")
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x)
		if  player.toTargetDist("a") < 50 and player.toTargetDist("b")<100 then
			return "a_getball"
		end
	end,
	["a"] = task.goCmuRush(player.shootGen(220,player.balltoplayer("b")),ball.toPlayerHeadDir("b"),5000,DSS_FLAG),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.toShootOrRobot("a")),
	match = "{a}[Lb]"
},

--动态拿球
["a_getball"] = {
	switch = function()
		local x = player.infraredCount("a")
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x)
		if player.infraredCount("a")>5  and player.toBallDist("a")<105 then
			return "a_wait"
		end
	end,
	["a"] = task.goCmuRush(ballvector("a"),dir.playerToBall,5000,f),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.toShootOrRobot("a")),
	match = "{a}[Lb]"
},

--a判断直接射门或者传球
["a_wait"] = {
	switch = function()
		if player.canFlatPassToPos("a",enemy.shootp()) then 
			if math.abs(player.dir("a") - player.toTheirGoalDir("a"))<math.pi/18*8 then
			     return "atg"
			else
				 return "slowtg"
			end
		else
		    if player.toTargetDist("b")<100 then
			    return "a_passball"
			end
		end
	end,
	["a"] = task.goCmuRush(player.pos,dir.playerToBall,5000,f),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.toShootOrRobot("a")),
	match = ""
},

--a直接转射门
["atg"] = {
	switch = function()
		if math.abs(player.dir("a") - enemy.togoaldirjud("a"))<math.pi/70 then
			return "shoot"
		end
	end,
	["a"] = task.goCmuRush(task.shootGen(50),player.togoaldir1,_,f),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.toShootOrRobot("a")),
	match = ""
},

["slowtg"] = {
	switch = function()
		if math.abs(player.dir("a") - player.toTheirGoalDir("a"))<math.pi/3 then
			return "atg"
		end
	end,
	["a"] = task.goCmuRush(task.shootGen(50),dir.playerToBall,_,f),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.toShootOrRobot("a")),
	match = ""
},

--a车射门或者传球
["a_passball"] = {
	switch = function()
		if math.abs(player.toPlayerDir("a","b") - player.dir("a"))<math.pi/90 then
			if player.shootorpass("a","b") then
				if player.canFlatPassTo("a","b") then
			        return "a_realpass"--直接传球转向射门
			    else
			    	return "achiptob"
			    end
		    else
		    	if player.canFlatPassTo("a","b") then
		    	     return "b_rcrball" --a射门成功率很低 传给b b等待射门
		    	else
		    		 return "achiptob"
		    	end
		    end
		end
	end,
	["a"] = task.goCmuRush(player.shootGen(50,player.balltoplayer("b")),ball.toPlayerHeadDir("b"),_,f),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.todir("b")),
	match = ""
},

["achiptob"] = {
	switch = function()
		if player.kickBall("a") or ball.velMod()>1000 then
			return "btg"
		end
	end,
	["b"] = task.goCmuRush(player.pos,player.toShootOrRobot("a"),_,f),
	["a"] = task.chippass(player.shootGen(50,player.balltoplayer("b")),"b"),
	["Leader"] = task.goCmuRush(player.pos),
	match = ""
},
--b接球
["b_rcrball"] = {
	switch = function()
		if player.kickBall("a") or ball.velMod()>1000 then
			return "btg"
		end
	end,
	["b"] = task.goCmuRush(player.pos,player.toShootOrRobot("a"),_,f),
	["a"] = task.flatpass(player.shootGen(50,player.balltoplayer("b")),"b"),
	["Leader"] = task.goCmuRush(player.pos),
	match = ""
},

--b接球 和上面a传球分开写是为了避开a传球后追球  追球原因看a的task.shoot中给的点位 自己理解一下
--后期要考虑接球车前面有人的情况
["btg"] = {
	switch = function()
		if player.toBallDist("b") < 105 then
			return "b_turnshoot"
		end
	end,
	["b"] = task.goCmuRush(player.standpos("b"),player.toShootOrRobot("a"),10000,f),
	["a"] = task.goCmuRush(getpos(param.pitchLength/4,param.pitchWidth/2-200),player.todir("a")),
	["Leader"] = task.goCmuRush(player.pos),
	match = ""
},

--b转向球门准备射门
["b_turnshoot"] = {
	switch = function()
		if math.abs(player.dir("b") - player.todir("b")())<math.pi/90 then--修改处
			return "b_shoot"
		end
	end,
	["b"] = task.goCmuRush(task.shootGen(50),player.todir("b"),_,f),
	["a"] = task.goCmuRush(getpos(param.pitchLength/4,param.pitchWidth/2-200),player.todir("a")),
	["Leader"] = task.goCmuRush(player.pos),
	match = ""
},

--a传球 并且b不改变他的射门角度
["a_realpass"] = {
	switch = function()
		if player.kickBall("a") and ball.velMod()>1000 then
			return "bwait_shoot"
		end
	end,
	["a"] = task.flatpass(player.shootGen(50,player.balltoplayer("b")),"b"),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(player.pos,player.todir("b")),
	match = ""
},

--b等待射门
["bwait_shoot"] = {
	switch = function()
		if player.toBallDist("b")<105 and player.infraredCount("b")>5 then
			return "b_shoot"
		end
	end,
	["Leader"] = task.goCmuRush(player.pos),
	["a"]      = task.goCmuRush(player.pos),
	["b"]      = task.shoot(player.standpos("b"),player.todir("b"),_,shootpower),
	match = ""
},

--b射门
["b_shoot"] = {
	switch = function()
	end,
	--["Leader"] = task.goCmuRush(),
	--["a"]      = task.goCmuRush(),
	["b"]      = task.shoot(task.shootGen(50),player.todir("b"),_,shootpower),
	match = ""
},

name = "kickoffv1",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}local f = flag.dribbling + flag.allow_dss
local DSS_FLAG = flag.allow_dss + flag.dodge_ball --+ flag.dribbling
local shootpower = 650
local passpower = 300

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

gPlayTable.CreatePlay{

firstState = "advance",

--advance这个脚本是因为程序开始前需要进行车号配对，但由于第一次配对前车是没有固定车号的，没有固定车号那么就不能进行第一次跑点操作，
--这个状态的意义在于给车赋初始车号使后续可以进行球员实时配对  {}是匹配一次车号 []是实时配对车号 使用者可以尝试一下 在状态最下栏match处修改
["advance"]={
	switch = function()
		return "getball"
	end,
	["Leader"] = task.goCmuRush(ball.pos),
	["a"]      = task.goCmuRush(getpos(-300,1900),player.togoaldir1,5000,DSS_FLAG),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,DSS_FLAG),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = "{Labcd}"
},
--leader 拿球并转身传给a车，判断条件自己看，为不作具体解答，以下都自己看
["getball"] = {
	switch = function()
		if player.toTargetDist("Leader")<30 then
			return "touchball"
		end
		if ball.velMod()>1000 then
			return "a_getball"
		end
	end,
	["Leader"] = task.goCmuRush(player.shootGen(220,player.balltoplayer("a")),dir.playerToBall,5000,DSS_FLAG),
	["a"]      = task.goCmuRush(getpos(-300,1900),player.togoaldir1,5000,DSS_FLAG),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,DSS_FLAG),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = "{abL}"
},

["touchball"] = {
	switch = function()
		if player.infraredCount("Leader")>5  then
			return "waitpass"
		end
	end,
	["Leader"] = task.goCmuRush(player.shootGen(50,player.balltoplayer("a")),dir.playerToBall,5000,f),
	["a"]      = task.goCmuRush(getpos(-300,1900),player.togoaldir1,5000,DSS_FLAG),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,DSS_FLAG),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["waitpass"] = {
	switch = function()
		if player.toTargetDist("a")<50 then
			if player.canFlatPassTo("Leader","a") then
			     return "flatpass"
			else
				 return "chippass"
			end
		end
	end,
	["Leader"] = task.goCmuRush(player.pos,ball.toPlayerHeadDir("a"),5000,f),
	["a"]      = task.goCmuRush(getpos(-300,1900),player.togoaldir1,5000,DSS_FLAG),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,DSS_FLAG),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

--leader 平射传球给a车
["flatpass"] = {
	switch = function()
		--local x = kp.toTarget(1)
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x)
		if player.kickBall("Leader") or ball.velMod()>1000 then
			return "waitshoot"
		end
	end,
	["Leader"] = task.flatpass(player.shootGen(50,player.balltoplayer("a")),"a"),
	["a"]      = task.goCmuRush(player.pos,player.toShootOrRobot("Leader"),5000,f),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

--leader 挑射传球给a车
["chippass"] = {
	switch = function()
		--local x = kp.toTarget(1)
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x)
		if player.kickBall("Leader") or ball.velMod()>1000 then
			return "waitshoot"
		end
	end,
	["Leader"] = task.chippass(player.shootGen(50,player.balltoplayer("a")),"a"),
	["a"]      = task.goCmuRush(player.pos,player.toShootOrRobot("Leader"),5000,f),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

--a接球等待射门
["waitshoot"] = {
	switch = function()
		if player.infraredCount("a")>5 and player.toBallDist("a")<105 then
		     if player.canFlatPassToPos("a",enemy.shootp()) then--检测是否能直接射门，能就转向射门
                 return "turnhead"
		     else                                         --不能就传球
		     	if player.jgetbmod() then                        --静态拿球
			        return "a_antigetball"
			    else                                      --动态拿球
                    return "a_getball"
                end
			 end
		end
		if bufcnt(player.toBallDist("a")>105 or math.abs(player.toBallDir("a"))>math.pi/6,60) then
			if player.jgetbmod() then                        --静态拿球
                return "a_antigetball"
			else                                      --动态拿球
                return "a_getball"
            end
		end
	end,
	["Leader"] = task.goCmuRush(player.pos),
	["a"]      = task.goCmuRush(player.standpos("a"),player.toShootOrRobot("Leader"),10000,f),
	["b"]      = task.goCmuRush(getpos(param.pitchLength/4,-1900),player.todir("b")),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

--a转向射门
["turnhead"] = {
		switch = function()
		if math.abs(player.dir("a")- enemy.togoaldirjud("a"))<math.pi/90 then
			return "shoot"
		end
		if bufcnt(player.toBallDist("a")>105 or math.abs(player.toBallDir("a"))>math.pi/8,20) then
			if player.jgetbmod() then                        --静态拿球
                return "a_antigetball"
			else                                      --动态拿球
                return "a_getball"
            end
		end
	end,
	["Leader"] = task.goCmuRush(player.pos),
	["a"]      = task.goCmuRush(player.shootGen(50,player.togoaldir("a")),player.togoaldir1,10000,f),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,DSS_FLAG),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = "{LB}[a]"
},

--a射门
["shoot"] = {
	switch = function()
		if bufcnt(player.toBallDist("a")>105 or math.abs(player.toBallDir("a"))>math.pi/8,50) then
			if player.jgetbmod() then                        --静态拿球
                return "a_antigetball"
			else                                      --动态拿球
                return "a_getball"
            end
		end
		if player.kickBall("a") and ball.velMod()>1000 then
		   return "getball"
		end
	end,
	["a"] = task.shoot(player.shootGen(50,player.togoaldir("a")),player.togoaldir1,_,shootpower),
	["Leader"] = task.goCmuRush(getpos(param.pitchLength/4,param.pitchWidth/2+200),player.toShootOrRobot("a"),6000,f),
	["b"]      = task.goCmuRush(getpos(0,-1900),player.todir("b"),5000,f),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

--静态拿球
["a_antigetball"] = {
	switch = function()
		local x = player.infraredCount("a")
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x)
		if  player.toTargetDist("a") < 50 and player.toTargetDist("b")<100 then
			return "a_getball"
		end
	end,
	["a"] = task.goCmuRush(player.shootGen(220,player.balltoplayer("b")),ball.toPlayerHeadDir("b"),5000,DSS_FLAG),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.toShootOrRobot("a")),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = "{a}[Lb]"
},

--动态拿球
["a_getball"] = {
	switch = function()
		local x = player.infraredCount("a")
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x)
		if player.infraredCount("a")>5  and player.toBallDist("a")<105 then
			return "a_wait"
		end
	end,
	["a"] = task.goCmuRush(ballvector("a"),dir.playerToBall,5000,f),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.toShootOrRobot("a")),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = "{a}[Lb]"
},

--a判断直接射门或者传球
["a_wait"] = {
	switch = function()
		if player.canFlatPassToPos("a",enemy.shootp()) then 
			if math.abs(player.dir("a") - player.toTheirGoalDir("a"))<math.pi/18*8 then
			     return "atg"
			else
				 return "slowtg"
			end
		else
		    if player.toTargetDist("b")<100 then
			    return "a_passball"
			end
		end
	end,
	["a"] = task.goCmuRush(player.pos,dir.playerToBall,5000,f),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.toShootOrRobot("a")),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

--a直接转射门
["atg"] = {
	switch = function()
		if math.abs(player.dir("a") - enemy.togoaldirjud("a"))<math.pi/70 then
			return "shoot"
		end
	end,
	["a"] = task.goCmuRush(task.shootGen(50),player.togoaldir1,_,f),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.toShootOrRobot("a")),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["slowtg"] = {
	switch = function()
		if math.abs(player.dir("a") - player.toTheirGoalDir("a"))<math.pi/3 then
			return "atg"
		end
	end,
	["a"] = task.goCmuRush(task.shootGen(50),dir.playerToBall,_,f),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.toShootOrRobot("a")),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

--a车射门或者传球
["a_passball"] = {
	switch = function()
		if math.abs(player.toPlayerDir("a","b") - player.dir("a"))<math.pi/90 then
			if player.shootorpass("a","b") then
				if player.canFlatPassTo("a","b") then
			        return "a_realpass"--直接传球转向射门
			    else
			    	return "achiptob"
			    end
		    else
		    	if player.canFlatPassTo("a","b") then
		    	     return "b_rcrball" --a射门成功率很低 传给b b等待射门
		    	else
		    		 return "achiptob"
		    	end
		    end
		end
	end,
	["a"] = task.goCmuRush(player.shootGen(50,player.balltoplayer("b")),ball.toPlayerHeadDir("b"),_,f),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(getpos(2000,-1900),player.todir("b")),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

["achiptob"] = {
	switch = function()
		if player.kickBall("a") or ball.velMod()>1000 then
			return "btg"
		end
	end,
	["b"] = task.goCmuRush(player.pos,player.toShootOrRobot("a"),_,f),
	["a"] = task.chippass(player.shootGen(50,player.balltoplayer("b")),"b"),
	["Leader"] = task.goCmuRush(player.pos),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},
--b接球
["b_rcrball"] = {
	switch = function()
		if player.kickBall("a") or ball.velMod()>1000 then
			return "btg"
		end
	end,
	["b"] = task.goCmuRush(player.pos,player.toShootOrRobot("a"),_,f),
	["a"] = task.flatpass(player.shootGen(50,player.balltoplayer("b")),"b"),
	["Leader"] = task.goCmuRush(player.pos),
	match = ""
},

--b接球 和上面a传球分开写是为了避开a传球后追球  追球原因看a的task.shoot中给的点位 自己理解一下
--后期要考虑接球车前面有人的情况
["btg"] = {
	switch = function()
		if player.toBallDist("b") < 105 then
			return "b_turnshoot"
		end
	end,
	["b"] = task.goCmuRush(player.standpos("b"),player.toShootOrRobot("a"),10000,f),
	["a"] = task.goCmuRush(getpos(param.pitchLength/4,param.pitchWidth/2-200),player.todir("a")),
	["Leader"] = task.goCmuRush(player.pos),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

--b转向球门准备射门
["b_turnshoot"] = {
	switch = function()
		if math.abs(player.dir("b") - player.todir("b")())<math.pi/90 then--修改处
			return "b_shoot"
		end
	end,
	["b"] = task.goCmuRush(task.shootGen(50),player.todir("b"),_,f),
	["a"] = task.goCmuRush(getpos(param.pitchLength/4,param.pitchWidth/2-200),player.todir("a")),
	["Leader"] = task.goCmuRush(player.pos),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

--a传球 并且b不改变他的射门角度
["a_realpass"] = {
	switch = function()
		if player.kickBall("a") and ball.velMod()>1000 then
			return "bwait_shoot"
		end
	end,
	["a"] = task.flatpass(player.shootGen(50,player.balltoplayer("b")),"b"),
	["Leader"] = task.goCmuRush(getpos(2000,1900),player.todir("Leader")),
	["b"] = task.goCmuRush(player.pos,player.todir("b")),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

--b等待射门
["bwait_shoot"] = {
	switch = function()
		if player.toBallDist("b")<105 and player.infraredCount("b")>5 then
			return "b_shoot"
		end
	end,
	["Leader"] = task.goCmuRush(player.pos),
	["a"]      = task.goCmuRush(player.pos),
	["b"]      = task.shoot(player.standpos("b"),player.todir("b"),_,shootpower),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

--b射门
["b_shoot"] = {
	switch = function()
	end,
	--["Leader"] = task.goCmuRush(),
	--["a"]      = task.goCmuRush(),
	["b"]      = task.shoot(task.shootGen(50),player.todir("b"),_,shootpower),
	["Goalie"] = task.goalie(),
	["d"]      = task.tier(),
	["c"]      = task.tier1(),
	match = ""
},

name = "Ref_KickOffV11",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}