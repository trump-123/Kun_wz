--enemy.updateCorrectGoaliePos()
--gCurrentPlay = gOppoConfig.PenaltyKick

if vision:getCycle() - gOurIndirectTable.lastRefCycle > 6 then
    if type(gOppoConfig.PenaltyKick) == "function" then
    	gCurrentPlay = cond.getOpponentScript("Ref_PenaltyKickV",gOppoConfig.PenaltyKick(),20)
    else
    	gCurrentPlay = cond.getOpponentScript("Ref_PenaltyKickV",gOppoConfig.PenaltyKick,20)
    end
    -- gCurrentPlay = "Ref_CornerKickMessi"
end

print("mcccc ",gCurrentPlay)
gOurIndirectTable.lastRefCycle = vision:getCycle()