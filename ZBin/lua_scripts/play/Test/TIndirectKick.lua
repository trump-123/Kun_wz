if ball.refPosX() < 0 * param.pitchLength then
	gCurrentPlay = "Ref_FrontDefV1"
 	--dofile("./lua_scripts/play/Ref/FrontDef/Ref_FrontDefV1.lua")
else
	gCurrentPlay = "Ref_BackDefV1"
	--dofile("./lua_scripts/play/Ref/BackDef/Ref_BackDefV1.lua")
end
-- gPlayTable.CreatePlay{
-- firstState = "select",

-- ["select"] = {
-- 	switch = function()
-- 		 local x = "Ref_KickOffDefV1"
-- 		debugEngine:gui_debug_msg(CGeoPoint(0,0),x)
-- 		gCurrentPlay = x
-- 	end,
-- 	 --switch = function()
-- 	 	--return function()
-- 	-- if math.abs(ball.refPosY()) < 1/3 * param.pitchWidth then
-- 	--    dofile("./lua_scripts/play/Ref/CenterKick/CenterKick.lua")
--  --    end
-- -- 	if (ball.posY() > 0 * param.pitchWidth) then
-- -- 		if (ball.posX() > 2/3 * param.pitchLength) then
-- -- 		    gCurrentPlay = "Ref_CornerKickV1"
-- -- 	    end
-- -- 	    -- if (ball.posX() < -2/3 * param.pitchLength) then
-- -- 		   --  gCurrentPlay = "Ref_CornerKickV0"
-- -- 	    -- end
-- -- 	end
-- -- end

-- 	 -- and (ball.posX() > 2/3 * param.pitchLength)
-- 	-- elseif (ball.posY() > 0 * param.pitchWidth) and (ball.posX() < -2/3 * param.pitchLength) then
-- 	-- 	gCurrentPlay = "Ref_CornerKickV0"
-- 	-- end
-- --end,
--     ["Leader"]   = task.marking(-param.pitchLength/2,0,-param.pitchWidth/2,param.pitchWidth/2),
--     ["Special"]  = task.marking1(-param.pitchLength/2,param.pitchLength/4,-param.pitchWidth/2,param.pitchWidth/2),
--     ["Assister"] = task.assister(),
-- 	["Defender"] = task.tier1(),
-- 	["Middle"]   = task.tier(),
-- 	["Goalie"]   = task.goalie(),
-- 	match = "[LSAMD]"
-- },
-- name = "TIndirectKick",
-- applicable = {
-- 	exp = "a",
-- 	a = true
-- },
-- attribute = "attack",
-- timeout   = 99999
-- }