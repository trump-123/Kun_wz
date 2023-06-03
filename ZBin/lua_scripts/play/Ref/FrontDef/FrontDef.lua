
if vision:getCycle() - gOurIndirectTable.lastRefCycle > 6 then
	if type(gOppoConfig.FrontDef) == "function" then
		gCurrentPlay = cond.getOpponentScript("Ref_FrontDefV",gOppoConfig.FrontDef(),7)
	else
		gCurrentPlay = cond.getOpponentScript("Ref_FrontDefV",gOppoConfig.FrontDef,7)
	end
end

