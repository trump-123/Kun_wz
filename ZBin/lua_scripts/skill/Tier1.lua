function Tier1(task)
   -- local mflag   = task.flag or 0
	matchPos = function()
		return CGeoPoint:new_local(-param.pitchLength/2.0,0)
	end

	execute = function(runner)
		return CTier1(runner)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "Tier1",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}