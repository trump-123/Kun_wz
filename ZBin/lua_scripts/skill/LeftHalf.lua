function LeftHalf(task)
	local mpos
	local mdir
	local mflag   = task.flag or 0
   -- debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,1000),"task.flag " ..task.flag)
	matchPos = function()
         return pos.ourGoal()
	end

	execute = function(runner)
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,1000),"useInter : " .. (useInter and "T" or "F"))
		return CLeftHalf(runner,mflag)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "LeftHalf",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}