function sao(task)
	matchPos = function()
		return pos.ourGoal()
	end
debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),1)
	execute = function(runner)
	
		return Csao(runner)
	end

	return execute, matchPos
end
gSkillTable.CreateSkill{
	name = "sao",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}