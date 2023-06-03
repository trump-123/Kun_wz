gPlayTable.CreatePlay{
firstState = "select",

["select"] = {
	switch = function()
		 local x = "Ref_KickOffV"..math.random(3,4)
		debugEngine:gui_debug_msg(CGeoPoint(0,0),x)
		if math.abs(ball.posX())<1000 then
		     gCurrentPlay = "Ref_MiddleKickV2"
	    else
             gCurrentPlay = "Ref_CornerKickV3"
        end
	end,
	 --switch = function()
	 	--return function()
	-- if math.abs(ball.refPosY()) < 1/3 * param.pitchWidth then
	--    dofile("./lua_scripts/play/Ref/CenterKick/CenterKick.lua")
 --    end
-- 	if (ball.posY() > 0 * param.pitchWidth) then
-- 		if (ball.posX() > 2/3 * param.pitchLength) then
-- 		    gCurrentPlay = "Ref_CornerKickV1"
-- 	    end
-- 	    -- if (ball.posX() < -2/3 * param.pitchLength) then
-- 		   --  gCurrentPlay = "Ref_CornerKickV0"
-- 	    -- end
-- 	end
-- end

	 -- and (ball.posX() > 2/3 * param.pitchLength)
	-- elseif (ball.posY() > 0 * param.pitchWidth) and (ball.posX() < -2/3 * param.pitchLength) then
	-- 	gCurrentPlay = "Ref_CornerKickV0"
	-- end
--end,
	["Leader"]   = task.stop(),
	["Special"]  = task.stop(),
	["Assister"] = task.stop(),
	["Defender"] = task.tier1(),
	["Middle"]   = task.tier(),
	["Goalie"]   = task.goalie(),
	match = "[LSAMD]"
},
name = "IndirectKick",
applicable = {
	exp = "a",
	a = true
},
attribute = "attack",
timeout   = 99999
}