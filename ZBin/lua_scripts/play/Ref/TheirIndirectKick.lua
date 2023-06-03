-- 在进入每一个定位球时，需要在第一次进时进行保持
--need to modify
gNorPass2NorDefBallVel = 150
gMarkFront2Finish = 180
gMarkFront2Def = 160
gNorPass2NorDefCouter = 8
-- if ball.refPosX() < -0.3 * param.pitchLength then
-- 	dofile("./lua_scripts/play/Ref/CornerDef/CornerDef.lua")
-- elseif ball.refPosX() > 0.2 * param.pitchLength then
-- 	dofile("./lua_scripts/play/Ref/FrontDef/FrontDef.lua")
-- elseif ball.refPosX() > -5 then
-- 	dofile("./lua_scripts/play/Ref/MiddleDef/MiddleDef.lua")
-- else
-- 	dofile("./lua_scripts/play/Ref/BackDef/BackDef.lua")
-- end
--gCurrentPlay = "TIndirectKick"
-- if ball.refPosX() < 0 * param.pitchLength then
-- 	gCurrentPlay = "Ref_FrontDefV1"
--  	--dofile("./lua_scripts/play/Ref/FrontDef/Ref_FrontDefV1.lua")
-- else
-- 	gCurrentPlay = "Ref_BackDefV1"
-- 	--dofile("./lua_scripts/play/Ref/BackDef/Ref_BackDefV1.lua")
-- end
--gNorPass2NorDefBallVel = 80
if ball.refPosX() < -param.pitchLength * 0.4 then
	dofile("./lua_scripts/play/Ref/CornerDef/CornerDef.lua")
elseif ball.refPosX() > param.pitchLength*0.28 then
	dofile("./lua_scripts/play/Ref/FrontDef/FrontDef.lua")
elseif ball.refPosX() > -50 then
	dofile("./lua_scripts/play/Ref/MiddleDef/MiddleDef.lua")
else
	dofile("./lua_scripts/play/Ref/BackDef/BackDef.lua")
end
