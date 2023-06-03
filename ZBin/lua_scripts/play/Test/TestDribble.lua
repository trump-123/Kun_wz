local f = flag.dribbling+flag.our_ball_placement
local p = CGeoPoint:new_local(4500,0)
local isrorate = true
local vx = 0
local xa = 0
local vw = 0
local wa = 0
local judfiled = 1

if isrorate then
	max_acc = 2
	min_acc = 1
	step_acc= 0.25
	nacc = 0
else
	max_acc = 1000
	min_acc = 500
	step_acc= 100
	nacc = 0
end

local judtestcontent = function(speed,rorate)
	return function()
		if isrorate == true then
			return task.speed(0,0,rorate,f)
		else
			return task.speed(speed,0,0,f)
		end
	end
end

local idir = function()
	if ball.posX() > 0 then
		return math.pi
	else
		return 0
	end
end

local mdir = function()
	if ball.posX() < 0 then
		return math.pi
	else
		return 0
	end
end

local addacc = function(e)
	return function()
		return vx
	end
end 

local addacc1 = function(e)
	return function()
		return vw
	end
end

local jud = false

gPlayTable.CreatePlay{

firstState = "GetBall",
["temp"] = {
	switch = function()
	end,
	Leader = task.openSpeed(0,0,10),
	match = "{L}"
},
["GetBall"] = {
	switch = function()
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-2000),player.infraredCount("Leader"))
		if bufcnt(player.infraredCount("Leader")>1,50) then
			if isrorate then
				--if not jud then
					return "Test"
			    -- else
			    -- 	return "Test1"
			    --end 
			else
				return "Turn"
			end
		end 
	end,
	Leader = task.goandTurnKick(_,"Leader"),
	match = "{L}"
},

["Turn"] = {
	switch = function()
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-2000),player.infraredCount("Leader"))
		if ball.posX() > 0 then
			judfiled = -1
		else
			judfiled = 1
		end
		if bufcnt(true,60) then
			vx = -vx
			if judfiled == -1 then	
				return "Test1"
			else
				return "Test"
			end
		end
	end,
	Leader = task.goCmuRush(player.shootGen(100,idir),mdir,_,f),
	match = ""
},

["Test"] = {
	switch = function()
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-2000),player.infraredCount("Leader"))
		--vx = 2000--(vx + min_acc/60)*judfiled
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),vx)
		if isrorate then
			vw = (vw + (step_acc*nacc + min_acc)/73)*judfiled
		else
			vx = (vx + (step_acc*nacc + min_acc)/73)*judfiled
		end
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),"rotatespeed:"..vw)
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-100),"speed:"..vx)
		if  player.posX("Leader") > param.pitchLength/2 - 1000 then
			return "Turn"
		end
		if player.infraredCount("Leader") == 0 or (ball.valid() and player.toBallDist("Leader") > 150) then
			return "Recording"
		end 
	end,
	Leader = judtestcontent(addacc(1),addacc1(2)),--task.speed(0,0,1,f),
	match = ""
},

["Test1"] = {
	switch = function()
		--vx = 2000--(vx + min_acc/60)*judfiled
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),vx)
		if isrorate then
			vw = (vw - (step_acc*nacc + min_acc)/73)
		else
			vx = (vx - (step_acc*nacc + min_acc)/73)
		end
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),"rotatespeed:"..vw)
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-100),"speed:"..vx)
		if  player.posX("Leader") < -param.pitchLength/2 + 1000 then
			return "Turn"
		end
		if player.infraredCount("Leader") == 0 or (ball.valid() and player.toBallDist("Leader") > 150) then
			return "Recording"
		end 
	end,
	Leader = judtestcontent(addacc(1),addacc1(2)),
	match = ""
},


["Recording"] = {
	switch = function()
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-2000),player.infraredCount("Leader"))
		if isrorate then
			jud = not jud
			--if gLastState == "Test" then
				 file=io.open("data/new改嵌入式改电机旋转.txt", "a")
				--file=io.open("data/new啥都没改.txt", "a")
				file:write(step_acc*nacc + min_acc," ",string.format("%.2f",math.abs(vw)), " " , "\n")
				file:close()
				vw = 0
				nacc = nacc + 1
			-- else
			-- 	file=io.open("data/antirorate.txt", "a")
			-- 	file:write(" now_rorateacc: ",step_acc*nacc + min_acc," maximum_speed: ",vw, " " , "\n")
			-- 	file:close()
			-- 	vw = 0
			-- 	
			--end
		else
			if gLastState == "Test1" then
				vx = -vx
			end
			file=io.open("data/改吸球改嵌入式后退.txt", "a")
			file:write(" now_acc: ",step_acc*nacc + min_acc," maximum speed: ",vx, " " , "\n")
			file:close()
			vx = 0
			nacc = nacc + 1
		end

		return "GetBall"
	end,
	--Leader = task.goandTurnKick(_,"Leader"),
	match = ""
},


name = "TestDribble",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
