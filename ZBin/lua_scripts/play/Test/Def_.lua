local ShootPos = CGeoPoint:new_local(3000,250)--助攻点
local postoball = 130
--主体
gPlayTable.CreatePlay{
	firstState = "def",

["def"] = {
	switch = function()
		if bufcnt(player.toPointDist("Leader",ball.pos()) < postoball,90) then
			return "getball"
		end
		if bufcnt(player.infraredCount("Leader")>1,10) then
			return "getball"
		end
		-- if bufcnt(player.infraredCount("Leader")>1,10) then
		-- 	return "解围"
		-- end
		-- if bufcnt(player.infraredCount("Leader")>1,10) then
		-- 	return "倒退"
		-- end
        -- if player.infraredCount("Leader")>20 or player.infraredCount("Special")>20 or player.infraredCount("Assister")>20 or player.infraredCount("Defender")>20 or player.infraredCount("Goalie")>20 or player.infraredCount("Middle")>20 then
        --         gCurrentPlay = "KickOff"
        -- end
		-- if bufcnt(player.toPointDist("Leader",ball.pos()) < postoball,90) then
		-- 	gCurrentPlay = "Ref_KickOffV1"
		-- end 
	end,
	["Leader"] = task.touchKick(ShootPos,_,8000),
	-- ["Leader"] = task.leader_getball(),
	["Special"] = task.special(),
	["Assister"] = task.assister(),
	["Defender"] = task.tier1(),
	["Middle"]   = task.tier(),
	["Goalie"]   = task.goalie(),
	match = "(LSAMD)"
	-- match = "[LSAMD]"
},
["getball"] = {
	switch = function()

    local num = -1
        for i=0,param.maxPlayer-1 do
            if enemy.valid(i) then
                if (enemy.pos(i) - ball.pos()):mod() < 105 then
                    num = 1
                end
            end
        end 
        if num == 1 then
           return "def"
        end
		-- if bufcnt(player.kickBall("Leader")) then--去抢球，踢出脚//就去追球
		-- 	return "def"
		-- end
        -- if player.infraredCount("Leader")>20 or player.infraredCount("Special")>20 or player.infraredCount("Assister")>20 or player.infraredCount("Defender")>20 or player.infraredCount("Goalie")>20 or player.infraredCount("Middle")>20 then
        --         gCurrentPlay = "KickOff"
        -- end
		
	end,
	["Leader"] = task.leader(),
	["Special"] = task.special(),
	["Assister"] = task.assister(),
	["Defender"] = task.tier1(),
	["Middle"]   = task.tier(),
	["Goalie"]   = task.goalie(),
	match = "[LSAMD]"
},

["解围"] = {
	switch = function()
		if bufcnt(player.toPointDist("Leader",ball.pos()) > postoball,90) then--没有带到球
			return "def"
		end
		if bufcnt(player.toPointDist("Leader",ball.pos()) < postoball,90) then
			gCurrentPlay = "Ref_KickOffV3"
		end
        -- if player.infraredCount("Leader")>20 or player.infraredCount("Special")>20 or player.infraredCount("Assister")>20 or player.infraredCount("Defender")>20 or player.infraredCount("Goalie")>20 or player.infraredCount("Middle")>20 then
        --         gCurrentPlay = "KickOff"
        -- end
		
	end,
	["Leader"]   = task.goCmuRush(task.shootGen(150),_,8000,flag.dribbling),
	["Special"]  = task.special(),
	["Assister"] = task.assister(),
	["Defender"] = task.tier1(),
	["Middle"]   = task.tier(),
	["Goalie"]   = task.goalie(),
	match = "{LSAMD}"
},

["倒退"] = {
	switch = function()
		if bufcnt(player.toPointDist("Leader",ball.pos()) > postoball,90) then--没有带到球
			return "def"
		end
		if bufcnt(player.toPointDist("Leader",ball.pos()) < postoball,90) then
			gCurrentPlay = "Ref_KickOffV3"
		end
        -- if player.infraredCount("Leader")>20 or player.infraredCount("Special")>20 or player.infraredCount("Assister")>20 or player.infraredCount("Defender")>20 or player.infraredCount("Goalie")>20 or player.infraredCount("Middle")>20 then
        --         gCurrentPlay = "KickOff"
        -- end
		
	end,
	["Leader"]   = task.goCmuRush(task.shootGen(150),_,8000,flag.dribbling),
	["Special"]  = task.special(),
	["Assister"] = task.assister(),
	["Defender"] = task.tier1(),
	["Middle"]   = task.tier(),
	["Goalie"]   = task.goalie(),
	match = "{LSAMD}"
},

name = "Def_",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}