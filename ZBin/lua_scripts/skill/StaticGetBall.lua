function StaticGetBall(task)
    local mpos
    local mkpower
    local mcpower
    execute = function(runner)
        -- print(type(mdefendNum))
        if type(task.pos) == "function" then
            mpos = task.pos()
        else
            mpos = task.pos
        end
        if type(task.kpower) == "function" then
            mkpower = task.kpower()
        else
            mkpower = task.kpower
        end

        if task.cpower == nil then
            mcpower = 0.0
        else
            if type(task.cpower) == "function" then
                mcpower = task.cpower()
            else
                mcpower = task.cpower
            end
        end
        -- print("defendNumfunction: ",mdefendNum)
        -- print("defendNum: ",mdefendNum)
        return CStaticGetBall(runner, mpos:x(), mpos:y(), mkpower,mcpower)
    end

    matchPos = function(runner)
        local ipos
        if type(task.pos) == "function" then
            ipos = task.pos()
        else
            ipos = task.pos
        end
        ballpos = CGeoPoint:new_local(ball.posX(),ball.posY())
        local mapos = ballpos + Utils.Polar2Vector(90, (ballpos - ipos):dir())
        return mapos
    end
    return execute, matchPos
end

gSkillTable.CreateSkill{
    name = "StaticGetBall",
    execute = function (self)
        print("This is in skill"..self.name)
    end
}
