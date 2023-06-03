local testPos  = {
	CGeoPoint:new_local(1000,1000),
}
gPlayTable.CreatePlay{

firstState = "run1",

["run1"] = {
	switch = function()
		-- pos.playerBest()
		-- if bufcnt(player.toTargetDist("Kicker")<5,time) then
		-- 	return "run"..2--math.random(4)
		-- end
	end,
	Assister = task.marking(-3000,0,-2000,2000,0),
    Leader = task.marking(-3000,0,-2000,2000,1),
    a = task.marking(-3000,0,-2000,2000,2),
    b = task.marking(-3000,0,-2000,2000,3),
    c = task.marking(-3000,0,-2000,2000,4),
    d = task.marking(-3000,0,-2000,2000,5),
	match = "[ALabcd]"
},

name = "pretect",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
