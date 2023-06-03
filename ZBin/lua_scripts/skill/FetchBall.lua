function FetchBall(task)
	local mpos
	local mpower = task.power
	local useInter = task.useInter or false
	matchPos = function()
		return ball.pos()
	end

	execute = function(runner)
		if type(task.pos) == "function" then
			mpos = task.pos(runner)
		else
			mpos = task.pos
		end

		if type(task.power) == "function" then
			mpower = task.power(runner)
		end
		
		return CFetchBall(runner, mpos:x(), mpos:y(), mpower, useInter)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "FetchBall",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}