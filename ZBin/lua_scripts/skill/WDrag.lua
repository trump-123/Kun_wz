function WDrag(task)
  local mpos
  local mtarget
  local mrole = task.srole or ""

  matchPos = function()
    if type(task.pos) == "function" then 
      mpos = task.pos()
    else
      mpos = task.pos
    end
    return mpos
  end
  
  execute = function(runner)
    if type(task.pos) == "function" then 
      mpos = task.pos()
    else
      mpos = task.pos
    end
    if type(task.target) == "function" then 
      mtarget = task.target()
    else
      mtarget = task.target
    end

    if runner >= 0 and runner < param.maxPlayer then
      if mrole ~= "" then
        CRegisterRole(runner, mrole)
      end
    end

    return CWDrag(runner, mpos:x(), mpos:y(), mtarget:x(), mtarget:y())
  end

  return execute, matchPos
end

gSkillTable.CreateSkill{
  name = "WDrag",
  execute = function (self)
    print("This is in skill"..self.name)
  end
}