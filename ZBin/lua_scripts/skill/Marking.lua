function Marking(task)
	local mpos
	local useInter = task.useInter or false
	local macc     = task.acc or 0

	--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-1000),useInter and "T" or"F")
	execute = function(runner)
		if type(task.pos) == "function" then
			mpos = task.pos(runner)
		else
			mpos = task.pos
		end
		return CMarking(runner, mpos:x(), mpos:y(), useInter, macc)
	end

	matchPos = function()
		if type(task.pos) == "function" then
			return task.pos(runner)
		else
			return task.pos
		end
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "Marking",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}