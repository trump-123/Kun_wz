function Naqiu(task)
	local mpos
	local mdir
   -- debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,1000),"task.flag " ..task.flag)
	matchPos = function()
         return pos.ourGoal()
	end

	execute = function(runner)
		debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,1000),"useInter : " .. (useInter and "T" or "F"))
		return Cnaqiu(runner)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "Naqiu",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}