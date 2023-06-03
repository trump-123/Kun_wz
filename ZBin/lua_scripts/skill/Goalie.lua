function Goalie(task)
	local mpos 
	local mflag =  flag.nothing 
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

		if task.pos == nil then
			mpos = CGeoPoint(500,0)
		else
			if type(task.pos) == "function" then
				mpos = task.pos()
			else
				mpos = task.pos
			end
		end

		return CGoalie(runner,mflag,mpos:x(),mpos:y())
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "Goalie",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}