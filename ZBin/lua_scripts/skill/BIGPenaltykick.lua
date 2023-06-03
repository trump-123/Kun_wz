function BIGPenaltykick(task)
	matchPos = function()
		return ball.pos()
	end
	execute = function(runner)
	
		return CBIGpenaltyKick(runner)
	end

	return execute, matchPos
end
gSkillTable.CreateSkill{
	name = "BIGPenaltykick",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}