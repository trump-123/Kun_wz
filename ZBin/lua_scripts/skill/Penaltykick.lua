function Penaltykick(task)
	matchPos = function()
		return ball.pos()
	end
	execute = function(runner)
	
		return CPenaltyKick(runner)
	end

	return execute, matchPos
end
gSkillTable.CreateSkill{
	name = "Penaltykick",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}