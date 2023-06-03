function ChaseKick(task)
	local mpos
	--local useInter = task.useInter or false
	matchPos = function()
		return ball.pos()
	end

	execute = function(runner)
		-- if type(task.pos) == "function" then
		-- 	mpos = task.pos(runner)
		-- else
		-- 	mpos = task.pos
		-- end
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,1000),"useInter : " .. (useInter and "T" or "F"))
		return CChaseKick(runner)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "ChaseKick",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}