-- 四车上前，选取最合适的
-- 四车分散站位
-- by fjm 2022-02-07
-- 缺点 选中第四车时接球不稳 need modify

local f = flag.dribbling + flag.allow_dss
local DSS_FLAG = flag.dodge_ball + flag.allow_dss
local PASS_ACC = 2000
local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth
local shootpower = 8000
local directpower = 6500
local time = 750
local tobaldist = 120 --用于球速小于500，接球车力求距离判断
local isDnamic = true --true 为开启横向带球 对付强队用

local chipPower = 160
local LEADER_TASK = function(role1,role2)
  return function()
    if player.canFlatPassTo(role1,role2) then
      return task.flatpass(player.shootGen(85,player.balltoplayer(role2)),role2)
    else
      if player.infraredCount(role1) > 20 then
        return task.chippass(player.shootGen(85,player.balltoplayer(role2)),role2)
      else
        return task.goCmuRush(player.shootGen(90,player.balltoplayer(role2)),player.toPlayerHeadDir(role2),_,flag.dribbling)
      end
    end
  end
end




--First stand 序号代表谁的优先级更高
local First  = ball.refAntiYPos(CGeoPoint:new_local(pl/4-500,pew/4+400))--ball.refSyntYPos(CGeoPoint:new_local(pl/4+200, 0))
local Second = ball.refAntiYPos(CGeoPoint:new_local(pl/2 - ped-300, pew/2 + 300))
local Third  = ball.refSyntYPos(CGeoPoint:new_local(pl/2 - ped/2-200, pew/2 + 300))
local Fourth = ball.refSyntYPos(CGeoPoint:new_local(1000, -200))

local movep = {
      ball.refAntiYPos(CGeoPoint:new_local(pl/4 - 500,pew/4+400-800)),
      ball.refAntiYPos(CGeoPoint:new_local(pl/2 - ped-300, pew/2 + 300-800)),
      ball.refSyntYPos(CGeoPoint:new_local(pl/2 - ped/2-200, pew/2 + 300-800)),
      ball.refSyntYPos(CGeoPoint:new_local(1000, -200+800))
}

local ToGoalPoint = {
  CGeoPoint:new_local(pl/2,0),
  ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
}

local second = {
  ball.refSyntYPos(CGeoPoint:new_local(pl/4+600, 500)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-400,pew/4+500)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped-1000,pew/2+290)),
}

local LeaderPos = {
  ball.refSyntYPos(CGeoPoint:new_local(pl/4,pw/2-500))
}

local backPos = {
    ball.refAntiYPos(CGeoPoint:new_local(-pl/4,pw/4)),
    ball.refSyntYPos(CGeoPoint:new_local(-pl/4,pw/4))
}

local otherPos = ball.refSyntYPos(CGeoPoint:new_local(pl/2-2000,pw/4+200))

local Defence_Pos = function()
      if ball.posY() < 0 then
         return CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + param.playerRadius*1.5,param.penaltyWidth/2-param.playerRadius)
      else
         return CGeoPoint:new_local(-param.pitchLength/2 + param.penaltyDepth + param.playerRadius*1.5,-param.penaltyWidth/2+param.playerRadius)
      end
end


local getPassPos = function()
    return function()
        return messi:freeKickPos()
    end
end

local getPassVel = function()
    return function()
        return messi:freepassVel()--*1.4
    end
end

gPlayTable.CreatePlay{

  firstState = "start",

  ["start"] = {
     switch = function()
        if bufcnt(player.kickBall("Leader") or ball.velMod() > 1000, 1, 60*6) then
            return "exit" 
        end
     end,
     Leader   = task.staticGetBall(getPassPos(),getPassVel()),
     Assister = task.goCmuRush(First ,0,ACC,DSS_FLAG + flag.free_kick),  
     Special  = task.goCmuRush(Second,0,ACC,DSS_FLAG + flag.free_kick),
     Middle   = task.goCmuRush(Third ,0,ACC,DSS_FLAG + flag.free_kick),
     Defender = task.goCmuRush(Fourth,0,ACC,DSS_FLAG + flag.free_kick),
     Fronter  = task.goCmuRush(Defence_Pos,player.toPointDir(ToGoalPoint[2]),_,DSS_FLAG + flag.free_kick),
     Kicker   = task.goCmuRush(otherPos ,0,ACC,DSS_FLAG + flag.free_kick),
     Center   = task.goCmuRush(backPos[1] ,0,ACC,DSS_FLAG + flag.free_kick),
     Breaker  = task.wback(2,1),
     Receiver = task.wback(2,2),
     Goalie   = task.goalie(),
     match = "[L][BR][ASMD][FKC]"
  },  

  

  name = "Ref_FrontKickV1103",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}