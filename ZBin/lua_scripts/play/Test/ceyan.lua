local OppnPos1 = CGeoPoint:new_local(-1130,1650)--助攻点
local OppnPos2 = CGeoPoint:new_local(0,0)--射门位置

local DSS_FLAG = flag.allow_dss + flag.dodge_ball

local waitPos = CGeoPoint:new_local(3000,-330)--助攻点

local leaderFlag = flag.dodge_ball

local f = flag.dribbling

--local angle = 0
-- local rorat = function()
-- 		local ipos = ball.pos()
-- 		return ipos + Utils.Polar2Vector(200,angle)
-- end
-- local angle1 = 0
-- local THIH = function(role,role1)
-- 	return function()
-- 		local ipos = ball.pos() + Utils.Polar2Vector(180,(ball.pos() - player.pos(role1)):dir())
-- 		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),(player.pos(role) - ipos):mod())
-- 		--if (player.pos(role) - ipos):mod() > 40 then
-- 		if math.abs(player.toBallDir(role) - player.toPlayerDir(role,role1))>math.pi/10 then
-- 		   return ball.pos() + Utils.Polar2Vector(200,angle1)
-- 		else
-- 			return ipos
-- 		end
-- 	end
-- end

local angle1 = 0

local rorate = function(role,role1)
	return function()
	   local ipos = ball.pos() + Utils.Polar2Vector(200,player.toBallDir(role1))
	   if math.abs(player.toBallDir(role) - player.toPlayerDir(role,role1)) > math.pi/9 then
	       return ball.pos() + Utils.Polar2Vector(200,angle1)
	   else
	   	   return ipos
	   end
	end
end

local dir_change = function(role,role1)
	 local idir1 = (ball.pos() - player.pos(role1)):dir()
	 local idir2 = (player.pos(role) - ball.pos()):dir()
	 if Utils.Normalize(idir1 - idir2) < 0 then
	 	angle1 = Utils.Normalize(player.toBallDir(role) + math.pi - math.pi*2/(4*1.4))
	 else
	 	angle1 = Utils.Normalize(player.toBallDir(role) + math.pi + math.pi*2/(4*1.4))
	 end 
end	

-- local angle_dir = function(role1,role2)
-- 	local idir = (player.pos(role1) - ball.pos()):dir()
-- 	local idir1= (ball.pos() - player.pos(role2)):dir()
-- 	if Utils.Normalize(idir1 - idir) < 0 then
-- 	   angle1 = Utils.Normalize(player.toBallDir(role1)+ math.pi - math.pi*2/(4*1.4)) 
-- 	else
-- 	   angle1 = Utils.Normalize(player.toBallDir(role1)+ math.pi + math.pi*2/(4*1.4)) 
-- 	end
-- end

local idir = function()
  if ball.posX()>0 then
    return math.pi
  else
    return 0
  end
end

local mydir = function()
  if ball.posX()>0 then
    return 0
  else
    return math.pi
  end
end 

local mydir1 = function(role)
	--return function()
	--if player.posY(role) > 200 then
		 return player.pos(role) + Utils.Polar2Vector(300,1.57)
	--elseif player.posY(role) <-200 then
	--	 return player.pos(role) + Utils.Polar2Vector(200,1.57)
	--end
--end
end

local mydir2 = function(role)
	--return function()
	--if player.posY(role) > 200 then
		 return player.pos(role) + Utils.Polar2Vector(300,-1.57)
	--elseif player.posY(role) <-200 then
	--	 return player.pos(role) + Utils.Polar2Vector(200,1.57)
	--end
--end
end

local idir1 = function(func)
	return function()
		return func
	end
end

local FIRST_DEFEND_POS4 = CGeoPoint:new_local(param.pitchLength/4-500,param.pitchWidth/2-1500)

local SINGLETIER_TASK = function(p,role)
    return function()
      local myrole 
      if role == 1 then
        myrole = "Leader"
      elseif role == 2 then
        myrole = "Assister"
      elseif role == 3 then
        myrole = "Special"
      elseif role == 4 then
        myrole = "Middle"
      elseif role == 5 then
        myrole = "Defender"
      end
      
      if player.myvalid(myrole) then
         return task.goCmuRush(p,_,_,DSS_FLAG)
      else
         return task.singleTier()
      end
    end
end

local UniqueSkill = function(task1,task2)
	return function()
		if cond.isGameOn() and ball.posX() < -param.pitchLength/4 + 500 then
			return task2()
		else
			return task1()
		end
	end
end

local BACK_DEFEND_TASK = function(defendnum,mytask)
	return function()
		if defendnum <= enemy.myattackNum1() then
			if defendnum == 1 then
			   return task.marking1()
			elseif defendnum == 2 then
				if enemy.myattackNum1() ~=2 then
			       return task.marking2()
			    else
                   if ball.posY() < -param.penaltyWidth/2 then
                       return task.righthalf()
                   elseif ball.posY() > param.penaltyWidth/2 then
                       return task.lefthalf()
                   else
                   	   return task.middlehalf()
                   end
                end
			elseif defendnum == 3 then
			   return task.marking3()
			elseif defendnum == 4 then
			   return task.marking4()
			end      
		else
			return mytask()
		end
	end
end



local x = 0
-- local enemyPos = function()
-- 	return enemy.pos(0)
-- end

local t = 0


local getPassVel = function()
	return function()
		return messi:freepassVel()
	end
end
local getPassPos = function()
	return function()
		return messi:freeKickPos()
	end
end

local getotherPos = function(index)
	return function()
		return pos.getOtherPos(index)
	end
end

gPlayTable.CreatePlay{

firstState = "ttt",

["ttt"] = {
	switch = function()
        --world:getBallStatus(vision:getCycle())
  --       debugEngine:gui_debug_msg(CGeoPoint:new_local(100,0),player.infraredCount("Defender"))--cond.judgeFieldArea()
		-- if ball.velMod()>6500 then
		-- 	t = 1
		-- end
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,0),player.toBallDist(4))
	--	debugEngine:gui_debug_x(messi:freeKickPos(),5)
		--dir_change("Leader","Assister")
		 --angle_dir("Leader","Assister")
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,0),t)
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,1000),player.toTargetDist("a") < 50 and "T"or"F")
		-- if bufcnt(player.toPointDist("a",ball.pos()+Utils.Polar2Vector(200,(ball.pos() - OppnPos2):dir())) < 50,30) then
		-- 	--return "t0"
		-- end
		if player.posX("e") > 750 then
			--return "t0"
		end
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),ball.velMod())
		--if player.advance() then
		--end
	end,
	--["Leader"] = task.goCmuRush(rorat,dir.playerToBall),--goCmuRush(pos.fakeDown(CGeoPoint:new_local(0,0)),ball.backDir(CGeoPoint:new_local(0,0)+Utils.Polar2Vector(1000,math.pi),false)),
	-- ["a"]      = task.staticGetBall(getPassPos(),getPassVel()),
	-- ["Assister"] = task.goCmuRush(pos.theirGoal,_,_,flag.dodge_ball+flag.free_kick),
	--["b"]	= task.wdrag(pos.getOtherPos(0)),
	--["c"] = task.singleTier(),
	Leader = task.wback(2,1),
	Assister = task.wback(2,2),
	--["Leader"] = task.goCmuRush(THIH("Leader","Assister"),player.toPlayerHeadDir("Assister")),
	--["e"] = task.goCmuRush(mydir1,idir1(0),_,f),--task.goCmuRush(rorate("Leader","Assister")),
	--["Defender"] = task.dnamicfindpos(),
	--["Goalie"] = task.Penaltygoalie(),
	match = "[LA]"
},

--ball.jaminner(1000)


["t"] = {
	switch = function()
		local temp1,temp2 = math.modf(ball.velMod()/1000)
		local n = temp1*1000 + math.floor(ball.velMod()/100%10)*100
		if n > x then
			x = n
		end
		---debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x)
		--local ipos = ball.pos() + Utils.Polar2Vector(200,(ball.pos() - player.pos("Assister")):dir())
		if bufcnt(player.infraredCount("e")>1,30) then
		  --return "ttt"
     end
     debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),player.valid("Leader")and "1" or "2")
	end,-- ,5)())
	--["e"] = UniqueSkill(BACK_DEFEND_TASK(6,SINGLETIER_TASK(ball.antiYPos(FIRST_DEFEND_POS4),5)),task.uniquekillskilll),--task.goCmuRush(player.shootGen(200,idir),mydir,_,DSS_FLAG),
	--["Defender"] = task.uniquekillskillr(),
--	["Assister"] = task.goCmuRush(OppnPos2),
	  ["Leader"]	   = task.tier(),
  	["a"]	   = task.tier1(),
		["b"]	   = task.uniquekillskillr(),
	--["Goalie"] = task.goalie(),
	--["e"] = task.uniquekillskillm(),
	match = "{Lab}"
},

--各自跑点,优先判断拿球的机器人是否跑到点
["t0"] = {
	switch = function()
		--if player.toTargetDist("Leader") < 30 then
			--return "getball"
		--end
		if player.posY("e") < -750 then
			return "ttt"
		end
		--return "t"
		--local x1 = player.pos("a")
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),shootGen1(200,player.pos("a")):x())
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x1:x())
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),0)
	end,

	["e"] = task.goCmuRush(mydir2,idir1(0),_,f),--task.uniquekillskillm(mydir2,idir1(0),_,f),
	--["Leader"] = task.goCmuRush(shootGen1(200, playerPos("e")),idir,_,DSS_FLAG),
	--["Leader"] = task.goCmuRush(ball.pos,dir.playerToBall,5000,f),
	--["Goalie"] = task.goalie(),
	--["Leader"] = task.goCmuRush(player.shootGen(220,player.balltoplayer("a")),player.toPlayerDir("a"),5000,DSS_FLAG),
	--["a"]      = task.goCmuRush(OppnPos2,player.toPlayerDir("Leader")),--toplayerdir("f","d")),
	match = "{La}"
},

-- 一个机器人拿球
["getball"] = {
	switch = function()
		if player.infraredCount("Leader")>5 then
			return "t1"
		end
	end,
	["Leader"] = task.goCmuRush(ball.pos,dir.playerToBall,5000,f),
	["a"]      = task.goCmuRush(OppnPos2,player.toPlayerDir("Leader")),
	match = ""
},

["t1"] = {
	switch = function()
		if player.toTargetDist("a")<100 then
			return "t4"
		end
	end,
	["Leader"] = task.goCmuRush(player.pos,dir.playerToBall,5000,f),
	["a"]      = task.goCmuRush(OppnPos2,player.toPlayerDir("Leader")),
	match = ""
},

["t4"] = {
	switch = function()
		if player.kickBall("Leader") and player.toBallDist("Leader")>500 then
			return "t2"
		end
	end,
	["Leader"] = task.flatpass(player.shootGen(50,player.balltoplayer("a")),"a"),
	["a"]      = task.goCmuRush(player.standpos("a"),player.toPlayerDir("Leader")),
	match = ""
},

["t2"] = {
	switch = function()
		if player.infraredCount("a") > 5 and
			player.toBallDist("a") < 105 
		then
			if math.abs(player.dir("a") - player.toTheirGoalDir("a")) > math.pi/3 then
				return "slowturn"
			else
			     return "t3"
			end
		end
	end,
	["Leader"] = task.stop(),--flatpass(player.shootGen(50,player.balltoplayer("a")),"a"),
	["a"]      = task.goCmuRush(player.standpos("a"),player.toPlayerDir("Leader")),
	match = ""
},

--曼转
["slowturn"] = {
    switch = function()
		if math.abs(player.dir("a") - player.toTheirGoalDir("a")) < math.pi/3 then
			return "t3"
		end
	end,
	["Leader"] = task.goCmuRush(CGeoPoint:new_local(0,0)),
	["a"]      = task.goCmuRush(task.shootGen(90),dir.playerToBall,5000,f),
	match = ""		
},


--快转
["t3"] = {
    switch = function()
		if math.abs(player.dir("a") - player.toTheirGoalDir("a")) < math.pi/80 then
			return "t5"
		end
	end,
	["Leader"] = task.goCmuRush(CGeoPoint:new_local(0,0)),
	["a"]      = task.goCmuRush(task.shootGen(90),_,5000,f),--不用给dir 函数里自带方向
	match = ""	
},

["t5"] = {
	switch = function()
	end,
	["a"] = task.shoot(task.shootGen(90)),
 	match = ""
},

--["t"] = {
	--switch = function()
		--if player.infraredCount("Leader")>5 then
			--return "t1"
		--end
		--local x1 = player.pos("a")
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),shootGen1(200,player.pos("a")):x())
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x1:x())
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),0)
	--end,

	--["e"] = task.goCmuRush(pos1,_,_,f),
	--["Leader"] = task.goCmuRush(shootGen1(200, playerPos("e")),idir,_,DSS_FLAG),
	--["Leader"] = task.goCmuRush(ball.pos,dir.playerToBall,5000,f),
	--["Goalie"] = task.goalie(),
	--["Leader"] = task.goCmuRush(ball.syntYPos(OppnPos1)),
	--["a"] = task.goCmuRush(),--toplayerdir("f","d")),
	--match = "[La]"
--},

--[[["t1"] = {
	switch = function()
	--	if player.toTargetDist("Leader")<10 then
			return "t"
	--	end
		--local x1 = player.pos("a")
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),shootGen1(200,player.pos("a")):x())
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),x1:x())
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),0)
	end,

	["a"] = task.goCmuRush(pos1,_,_,f),
	--["Leader"] = task.goCmuRush(shootgen(50,dir.theirGoalToBall),dir.playerToBall,_,f),
	--["Leader"] = task.goCmuRush(CGeoPoint:new_local(1000,0),_,_,allow_dss),
	--["Goalie"] = task.goalie(),
	match = "{La}"
},--]]

name = "ceyan",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}