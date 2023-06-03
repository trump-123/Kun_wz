local function PriToNum(str)
	if str == "Zero" then
		return 0
	elseif str == "First" then
		return 1
	elseif str == "Second" then
		return 2
	elseif str == "Third" then
		return 3
	elseif str == "Fourth" then
		return 4
	elseif str == "Fifth" then
		return 5
	elseif str == "Sixth" then
		return 6
	elseif str == "Seventh" then
		return 7
	else
		print("Error Priority in WMarking Skill")
	end
end

function WMarking(task)
	local mpri
	local mflag = task.flag or 0
	local num
	local mrole = task.srole

	matchPos = function(runner)
		local x,y = CGetWMarkingPos(runner,mpri,mflag,num)
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-500),x)
		return CGeoPoint:new_local(x,y)
	end
	--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,-1000),useInter and "T" or"F")
	execute = function(runner)
		if task.pri == nil then
			print("No priority in WMarking skill")
		elseif task.pri == "string" then
			mpri = PriToNum(task.pri)
		end
		
		if runner >= 0 and runner < param.maxPlayer then
			if mrole ~= "" then
				CRegisterRole(runner,mrole)
			end
		end

		if task.num == nil then
			num = -1
		elseif type(task.num) == "function" then
			num = task.num()
		else
			num = task.num
		end
		return CWMarking(runner, mpri, mflag, num)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "WMarking",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}