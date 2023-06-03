local ShootPos = CGeoPoint:new_local(3000,250)--助攻点
local postoball = 130
--主体
gPlayTable.CreatePlay{
	firstState = "def",

["def"] = {
	switch = function()
	end,
	Leader = task.wback(4,1),
	-- ["Leader"] = task.leader_getball(),
	Special = task.wback(4,2),
	Assister = task.wback(4,3),
	Defender = task.wback(4,4),
	--Middle   = task.wback(5,5),
	Goalie   = task.goalie(),
	match = "[LDSAM]"
	-- match = "[LSAMD]"
},


name = "testback",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}