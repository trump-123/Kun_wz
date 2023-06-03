function MarkingTouch(task)
	local mpos
	local useInter = task.useInter or false
	local macc     = task.acc or 0

	matchPos = function()
		return ball.pos()
	end
	execute = function(runner)
		if type(task.pos) == "function" then
			mpos = task.pos(runner)
		else
			mpos = task.pos
		end
		return CMarkingTouch(runner)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "MarkingTouch",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}