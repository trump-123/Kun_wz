
if vision:getCycle() - gOurIndirectTable.lastRefCycle > 6 then
    if type(gOppoConfig.CornerDef) == "function" then
    	gCurrentPlay = cond.getOpponentScript("Ref_CornerDefV",gOppoConfig.CornerDef(),20)
    else
    	gCurrentPlay = cond.getOpponentScript("Ref_CornerDefV",gOppoConfig.CornerDef,20)
    end
end

