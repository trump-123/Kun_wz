function WBack(task)
    local mflag = task.flag or 0
    mflag = mflag + flag.kick
    local mguardNum = task.guardNum or 1
    local mindex = task.index or 1
    --local mpower = task.power
    local mdefendNum = task.defendNum or -1
    local mrole = task.srole or ""

    execute = function(runner)
        if runner >=0 and runner < param.maxPlayer then
            if mrole ~= "" then
                CRegisterRole(runner, mrole)
            end
        else
            print("Error runner in GotoMatchPos", runner)
        end
        -- print(type(mdefendNum))
        if task.defendNum == nil then
            mdefendNum = -1
        elseif type(task.defendNum) == "function" then
            mdefendNum = task.defendNum()
            -- mdefendNum = task.defendNum()
        else
            mdefendNum = task.defendNum
        end
        -- print("defendNumfunction: ",mdefendNum)
        -- print("defendNum: ",mdefendNum)
        return CWBack(runner, mguardNum, mindex, mdefendNum, mflag)
    end

    matchPos = function(runner)
        if task.defendNum == nil then
            mdefendNum = -1
        elseif type(task.defendNum) == "function" then
            mdefendNum = task.defendNum()
            -- mdefendNum = task.defendNum()
        else
            mdefendNum = task.defendNum
        end
        -- 2021/05/18
        if runner == nil then
            runner = -1
        end
        return guardpos:backPos(mguardNum, mindex, runner, mdefendNum)
    end
    return execute, matchPos
end

gSkillTable.CreateSkill{
    name = "WBack",
    execute = function (self)
        print("This is in skill"..self.name)
    end
}
