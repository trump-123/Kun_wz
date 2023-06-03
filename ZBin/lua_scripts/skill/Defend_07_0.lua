function Defend_07_0(task)
	matchPos = function()
		return pos.ourGoal()
	end
debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),1)
	execute = function(runner)
	
		return CDefend_07_0(runner)
	end

	return execute, matchPos
end
gSkillTable.CreateSkill{
	name = "Defend_07_0",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}