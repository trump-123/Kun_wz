
if type(gOppoConfig.MiddleKick) == "function" then
	gCurrentPlay = cond.getOpponentScript("Ref_MiddleKickV", gOppoConfig.MiddleKick(), 4)
else
	gCurrentPlay = cond.getOpponentScript("Ref_MiddleKickV", gOppoConfig.MiddleKick, 4)
end

--gCurrentPlay = "Ref_MiddleKickV1"