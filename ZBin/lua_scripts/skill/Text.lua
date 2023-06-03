function Text(task)
	matchPos = function()
		return pos.ourGoal()
	end

	execute = function(runner)
		if runner >=0 and runner < param.maxPlayer then
			if mrole ~= "" then
				CRegisterRole(runner, "goalie")
			end
		else
			print("Error runner in Goalie", runner)
		end

		return Ctext(runner)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "Text",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}