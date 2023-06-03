local f = flag.dribbling+flag.our_ball_placement

local waitTouchPos = CGeoPoint:new_local(0,0)

local last_Kicker
local kick_number
local indir  = 0
local realindir = 0
local inVel  = 0
local outDir = 0
local powermode = false
local FRICTION = 1520
local ballAcc = FRICTION / 2


local touchDir = 0
local shootPos = function(role)
	return function()
		return player.pos(role) + Utils.Polar2Vector(2000,touchDir)
	end
end

local power = 0
local min_power = 3000
local step_power = 1000
local nkick = 1
local nturn = 0
local min_dir = math.pi/12.0
local step_dir = (math.pi/2 - min_dir)/10

local shoot_power = function()
	return min_power + nkick * step_power
end
local special = function()
		return CGeoPoint(0,0)--best:getBestFlatPass()
end

-- V(in)  角度in 角度out idea角度

gPlayTable.CreatePlay{

firstState = "GetBall",

["GetBall"] = {
	switch = function()
		--best:calculateBestPass()
		debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,0),best:getBestFlatPass():x(),1)
		--debugEngine:gui_debug_arc(player.pos("Assister"),1770,0,360,7)
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,1000),"now_dir_level: "..nturn)
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,800),"now_power_level: "..nkick)
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,600),"now_power: "..shoot_power())
		if player.kickBall("Leader") then
			-- file=io.open("data/touchDirtest.txt", "a")
			-- file:write(" 角度单位：弧度制   ", "\n")
			-- file:close()
			indir = player.toPlayerDir("Assister","Leader")
			touchDir = Utils.Normalize(indir + min_dir + nturn * step_dir)
			temp = gRoleNum["Assister"]
			gRoleNum["Assister"] = gRoleNum["Leader"]
			gRoleNum["Leader"] = temp
			return "Touch"
		end
		
	end,
	Leader   = task.goandTurnKick(waitTouchPos,"Leader"),
	Assister = task.goCmuRush(special,player.toPointDir(shootPos("Leader"))),
	Special  = task.stop(),
	match = "[LA]"
},

["Touch"] = {
	switch = function()
		debugEngine:gui_debug_msg(CGeoPoint:new_local(2000,0),ball.velMod())
		if player.toBallDist("Leader") < 300 and player.toBallDist("Leader") > 200 then
			inVel = math.sqrt(ball.vel():mod2() - FRICTION * ball.toPlayerDist("Leader"))
			realindir = Utils.Normalize(ball.velDir() + math.pi)
		end
		if player.kickBall("Leader") then
			return "Recording"
		end
	end,
	Leader   = task.touchKick(shootPos("Leader"),_,shoot_power),
	--Assister = task.stop(waitTouchPos),
	Assister = task.goCmuRush(special,player.toPointDir(shootPos("Leader"))),
	Special  = task.stop(),
	match = "{LA}"
},

["Recording"] = {
	switch = function()
		if ball.toPlayerDist("Leader") > 2000 then
			outDir = player.toBallDir("Leader")
			local outspeed = math.sqrt(ball.vel():mod2() + FRICTION*(player.toBallDist("Leader") - 93))
			--if bufcnt(true,10) then
			-- if powermode then

			-- else	
				if nturn > 7 then
					nturn = 0
				else
					nturn = nturn + 1
					file=io.open("data/anglevariable4000.txt", "a")
					file:write(string.format("%.2f",inVel)," ",string.format("%.2f",math.abs(Utils.Normalize(realindir - outDir)))," ",string.format("%.2f",outspeed * math.sin(math.abs(Utils.Normalize(outDir - player.dir("Leader")))))," ",string.format("%.2f",outspeed)," ",string.format("%.2f",math.abs(Utils.Normalize(outDir - player.dir("Leader")))), "\n")
					file:close()
					return "GetBall"
					--nkick = nkick + 1
				end
			--end
				-- if nkick == 5 then
				-- 	nkick = 0
				-- 	nturn = 0
				-- end
			--end
		end
	end,
	Leader   = task.stop(),
	Assister = task.stop(waitTouchPos),
	Special  = task.stop(),
	match = "{LA}"
},

name = "TestTouchDir",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
