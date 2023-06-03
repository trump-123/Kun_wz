function goAndTurnKick(task)
	local mpos
	local ipower
	local mflag   = task.flag or 0
	local useInter = task.useInter or false
	matchPos = function(runner)
		-- if type(task.pos) == "function" then
		-- 	mpos = task.pos()
		-- else
		-- 	mpos = task.pos
		-- end
		-- return mpos
		local x,y
		if runner == nil then
			x,y = CWGetBallPos()
		else
			x,y = CWGetBallPos(runner)
		end
		return CGeoPoint:new_local(x,y)
	end

	execute = function(runner)
		if type(task.pos) == "function" then
			mpos = task.pos(runner)
		else
			mpos = task.pos
		end
		if type(task.power) == "function" then
			ipower = task.power()
		else
			ipower = task.power
		end
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,1000),"useInter : " .. (useInter and "T" or "F"))
		--debugEngine:gui_debug_msg(CGeoPoint:new_local(0,1000),ipower)
		return CgoAndTurnKick(runner, mpos:x(), mpos:y(), useInter,ipower,mflag)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "goAndTurnKick",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}