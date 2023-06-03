local ShootPos = CGeoPoint:new_local(3000,250)--助攻点
local postoball = 130

local totaldir = 0
local slavedir = 0

local main = function(radius,total,num)
	return function()
		return CGeoPoint:new_local(0,0) + Utils.Polar2Vector(radius,totaldir + math.pi * 2.0 * num/total)
	end
end

local slave = function(main,radius,total,num)
	return function()
		local mainpos
		if type(main) == "function" then
			mainpos = main()
		else
			mainpos = main
		end


		return mainpos + Utils.Polar2Vector(radius,slavedir + math.pi * 2.0 * num/total)

	end
end

--主体
gPlayTable.CreatePlay{
	firstState = "def",

["def"] = {
	switch = function()
		totaldir = totaldir + math.pi*2/(60*24)
		slavedir = slavedir + math.pi*2/(60*12)

		for i = 1, 4 do
			debugEngine:gui_debug_arc(main(2000,4,i)(),700,0,360,6)
		end
		debugEngine:gui_debug_arc(CGeoPoint:new_local(0,0),2000,0,360,6)
	end,
	Leader = task.goCmuRush(slave(main(2000,4,1),700,4,1)),
	Special = task.goCmuRush(slave(main(2000,4,1),700,4,2)),
	Assister = task.goCmuRush(slave(main(2000,4,1),700,4,3)),
	Defender = task.goCmuRush(slave(main(2000,4,1),700,4,4)),
	a = task.goCmuRush(slave(main(2000,4,2),700,4,1)),
	b = task.goCmuRush(slave(main(2000,4,2),700,4,2)),
	c = task.goCmuRush(slave(main(2000,4,2),700,4,3)),
	d = task.goCmuRush(slave(main(2000,4,2),700,4,4)),
	e = task.goCmuRush(slave(main(2000,4,3),700,4,1)),
	f = task.goCmuRush(slave(main(2000,4,3),700,4,2)),
	g = task.goCmuRush(slave(main(2000,4,3),700,4,3)),
	h = task.goCmuRush(slave(main(2000,4,3),700,4,4)),
	i = task.goCmuRush(slave(main(2000,4,4),700,4,1)),
	j = task.goCmuRush(slave(main(2000,4,4),700,4,2)),
	k = task.goCmuRush(slave(main(2000,4,4),700,4,3)),
	l = task.goCmuRush(slave(main(2000,4,4),700,4,4)),
	match = "[LDSAabcdefghijkl]"
	-- match = "[LSAMD]"
},


name = "testskill",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}