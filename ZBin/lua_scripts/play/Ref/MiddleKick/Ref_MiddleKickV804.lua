-- 四车上前，选取最合适的
-- 一车虚晃，三车并排
-- by fjm 2022-02-08

local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth
local DSS_FLAG = flag.dodge_ball + flag.allow_dss
local f = flag.dribbling + flag.allow_dss
local PASS_ACC = 2000
local shootpower = 8000
local time = 750
local tobaldist = 120 --用于球速小于500，接球车力求距离判断
local isDnamic = false --true 为开启横向带球 对付强队用

local chipPower = 160

local LEADER_TASK = function(role1,role2)
  return function()
    if player.canFlatPassTo(role1,role2) then
     return task.flatpass(player.shootGen(90,player.balltoplayer(role2)),role2)
   else
      if player.infraredCount(role1) > 20 then
        return task.chippass(player.shootGen(90,player.balltoplayer(role2)),role2)
      else
        return task.goCmuRush(player.shootGen(90,player.balltoplayer(role2)),player.toPlayerHeadDir(role2),_,flag.dribbling)
      end
    end
  end
end

local SHOOT_TASK = function(role)
  return function()
    if player.infraredCount(role) > 5 then
      if player.canshoot(role) then
        return task.shoot(task.shootGen(90),player.todir(role),_,6500)
      else
        --if math.abs(player.dir(role) - player.toTheirGoalDir(role)) < math.pi/2 then
          return task.goCmuRush(task.shootGen(130),player.todir(role),_,f)
       --else
       --   return task.goCmuRush(player.shootGen(130,idir),dir.playerToBall,_,f)
       --end
      --return task.goCmuRush(task.shootGen(130),dir.playerToBall,ACC,DSS_FLAG)
      end
    else
        return task.chaseKick()
    end
  end
end

--9*6
--First stand 序号代表谁的优先级更高
-- local First  = ball.refAntiYPos(CGeoPoint:new_local(pl/2-ped+300,pew/2+290))
-- local Second = ball.refSyntYPos(CGeoPoint:new_local(3300,1400))
-- local Third  = ball.refSyntYPos(CGeoPoint:new_local(3000,1300))
-- local Fourth = ball.refSyntYPos(CGeoPoint:new_local(2700,1200))

-- local Real   = ball.refSyntYPos(CGeoPoint:new_local(1900,1000))

-- local ToGoalPoint = {
--   CGeoPoint:new_local(pl/2,0),
--   ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
--   ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
-- }

-- local second = {
--   ball.refSyntYPos(CGeoPoint:new_local(3300,2500)),
--   ball.refSyntYPos(CGeoPoint:new_local(3000,1300)),
-- }

--6*4
local First  = ball.refSyntYPos(CGeoPoint:new_local(pl/2 - ped - 300,pew/2+500))
local Second = ball.refAntiYPos(CGeoPoint:new_local(pl/4 - 500,pw/8))
local Third  = ball.refAntiYPos(CGeoPoint:new_local(pl/2 - ped-1000,pew/2+500))
local Fourth = ball.refAntiYPos(CGeoPoint:new_local(pl/2 - ped-2000,pew/2-200))

local Real  = ball.refSyntYPos(CGeoPoint:new_local(pl/2 - ped - 800,pew/2+500))

local Real1 = ball.refAntiYPos(CGeoPoint:new_local(pl/4 - 800,pw/8))

local movep ={
    ball.refSyntYPos(CGeoPoint:new_local(pl/2 - ped - 800,pew/2 + 500 -800)),
    ball.refAntiYPos(CGeoPoint:new_local(pl/4 - 800,pw/8 - 800))
} 

local ToGoalPoint = {
  CGeoPoint:new_local(pl/2,0),
  ball.refSyntYPos(CGeoPoint:new_local(pl/2,goal/2)),
  ball.refAntiYPos(CGeoPoint:new_local(pl/2,goal/2-200)),
}

local second = {
  ball.refSyntYPos(CGeoPoint:new_local(2000,1750)),
  ball.refSyntYPos(CGeoPoint:new_local(1890,900)),
}

local LeaderPos = {
  ball.refAntiYPos(CGeoPoint:new_local(pl/4+300,pw/2-500))
}


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
    switch = function ()
        --debugEngine:gui_debug_msg(CGeoPoint(0,0),getPassVel()())
        --debugEngine:gui_debug_x(getPassPos()())
      if bufcnt(player.kickBall("Leader") or ball.velMod() > 1000, 1, 60*6) then
          return "exit" 
      end
    end,
    Leader   = task.staticGetBall(getPassPos(),getPassVel()),
    Assister = task.goCmuRush(First ,player.toPointDir(ToGoalPoint[1]),_,DSS_FLAG + flag.free_kick),
    Special  = task.goCmuRush(Second,player.toPointDir(ToGoalPoint[2]),_,DSS_FLAG + flag.free_kick),
    Middle   = task.goCmuRush(Third ,player.toPointDir(ToGoalPoint[3]),_,DSS_FLAG + flag.free_kick),
    Defender = task.goCmuRush(Fourth,player.toPointDir(ToGoalPoint[2]),_,DSS_FLAG + flag.free_kick),
    Breaker  = task.wback(2,1),
    Receiver = task.wback(2,2),
    Goalie   = task.goalie(),
    match    = "[L][BR][ASMD]"
  },

  


  name = "Ref_MiddleKickV804",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}