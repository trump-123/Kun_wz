function PenaltyGoalie(task)
	matchPos = function()
		return pos.ourGoal()
	end
	execute = function(runner)
	
		return CPenaltyGoalie(runner)
	end

	return execute, matchPos
end
gSkillTable.CreateSkill{
	name = "PenaltyGoalie",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}