function Uniquekillskillr(task)
	local mpos
	local mdir
	matchPos = function()
		return pos.ourGoal()
	end
	execute = function(runner)
	 --    if type(task.pos) == "function" then
		-- 	mpos = task.pos(runner)
		-- else
		-- 	mpos = task.pos
		-- end

		-- if type(task.dir) == "function" then
		-- 	mdir = task.dir(runner)
		-- else
		-- 	mdir = task.dir
		-- end

		return CUniquekillr(runner)--, mpos:x(), mpos:y(),midr
	end

	return execute, matchPos
end
gSkillTable.CreateSkill{
	name = "Uniquekillskillr",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}