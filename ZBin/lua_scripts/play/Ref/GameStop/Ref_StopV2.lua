-- by FJM 2022-03-8
-- need modify
local PENALTY_THRESHOLD_DIST = 5500--math.sqrt(math.pow(param.pitchLength*3/10,2) + math.pow(param.goalWidth/2,2))
local MIDDLE_THRESHOLD_Y = 0.125 * param.pitchWidth --75

local topX = -(param.pitchLength/2 - param.penaltyDepth - 2 * param.playerRadius)
local sideY = param.penaltyWidth/2 + 2 * param.playerRadius
local RIGHTDEF = {
  CGeoPoint:new_local(topX - 0, 0),
  CGeoPoint:new_local(topX - 0,    sideY-400),
  CGeoPoint:new_local(topX - 400,  sideY),
  CGeoPoint:new_local(topX - 400, -sideY),
  CGeoPoint:new_local(topX - 600,  sideY),
  CGeoPoint:new_local(topX - 1000, sideY)
}
local LEFTDEF = {
  CGeoPoint:new_local(topX - 0, 0),
  CGeoPoint:new_local(topX - 0,   -sideY+400),
  CGeoPoint:new_local(topX - 400, -sideY),
  CGeoPoint:new_local(topX - 400,  sideY),
  CGeoPoint:new_local(topX - 600, -sideY),
  CGeoPoint:new_local(topX - 1000,-sideY)
}
local standPos = LEFTDEF
local standFunc = function(num)
  return function()
    return standPos[num]
  end
end

local SIDE_POS, MIDDLE_POS, INTER_POS, SIDE2_POS, INTER2_POS = pos.refStopAroundBall()
--local ONE_POS, TWO_POS, THREE_POS, FOUR_POS, FIVE_POS, SIX_POS = penaltyStop()--pos.stopPoses()

local STOP_FLAG = flag.dodge_ball
local STOP_DSS = bit:_or(bit:_or(STOP_FLAG, flag.allow_dss),flag.avoid_stop_ball_circle)

local gBallPosXInStop = 0
local gBallPosYInStop = 0

local OTHER_SIDE_POS1 = ball.antiYPos(CGeoPoint:new_local(-100,120))
local OTHER_SIDE_POS2 = ball.antiYPos(CGeoPoint:new_local(-100,-120))

local OPPOSITE_SIDE_POS = function()
  local factor = 1
  if ball.posY() > 0 then
    factor = -1
  end
  return CGeoPoint:new_local(INTER2_POS():x(), factor * param.pitchWidth * 0.3)
  --return CGeoPoint:new_local(ball.posX() - 50*param.lengthRatio, 160*factor*param.widthRatio)
end

local getBufTime = function()
  if IS_SIMULATION then
    return 9999
  else
    return 240
  end
end

local RECEIVE_POS   = function ()
  if ball.posX() < 0 then
    return ball.refSyntYPos(CGeoPoint:new_local(ball.posX(), param.pitchWidth/3-200))()
  else
    return ball.refAntiYPos(CGeoPoint:new_local(param.pitchLength/6, param.pitchWidth/12))()
  end
end

local STOP_OTHER_PLAYER_POS = function(index)
    return function()
        return CGeoPoint:new_local(-param.pitchLength/4 + 500,-param.pitchWidth/4) + Utils.Polar2Vector(param.pitchWidth/2*index,math.pi/2)
    end
end

local ACC = 2000

function getFrontAttackerNum(i)
    return function()
        return defenceSquence:getFrontAttackNum(i)
    end
end


gPlayTable.CreatePlay {

firstState = "initState",

["initState"] = {
  switch = function()
    if USE_11vs11_SOLVER then
      return "11vs11Solver"
    else
      return "8vs8Solver"
    end
  end,
  match = ""
},

["8vs8Solver"] = {
  switch = function()
    if cond.isGameOn() then
      return "exit"
    end
    -- gBallPosXInStop = ball.posX()
    -- gBallPosYInStop = ball.posY()
    -- if bufcnt(cond.isGameOn(),5) then
    --   return "exit"
    -- elseif ball.toOurGoalPostDistSum() < PENALTY_THRESHOLD_DIST then
    --   return "standByPenalty"
    -- elseif ball.posY() > MIDDLE_THRESHOLD_Y or ball.posY() < -MIDDLE_THRESHOLD_Y then
    --   return "standByLine"
    --elseif ball.posY() < MIDDLE_THRESHOLD_Y and ball.posY() > -MIDDLE_THRESHOLD_Y then
     -- return "standInMiddle"
    -- else
    --   --return "exit"
    -- end
    -- if player.myvalid("Defender") then
    --   gCurrentPlay = "Ref_StopV2D"
    -- else
      if ball.refPosX() < -param.pitchLength/2 + param.penaltyDepth and math.abs(ball.refPosY()) > param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4CornerDef"
      elseif ball.refPosX() > param.pitchLength/2 - param.penaltyDepth and math.abs(ball.refPosY()) > param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4CornerKick"-- by fjm 2022-03-22
      elseif math.abs(ball.refPosY()) >= param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4SideLine" -- by fjm 2022-03-22
      elseif ball.refPosX() < -(param.pitchLength/2 - param.penaltyDepth - 1000) and -(param.pitchLength/2+400) <= ball.refPosX() and math.abs(ball.refPosY()) <= param.penaltyWidth/2 + 300 then--+ param.pitchWidth/6 then
       gCurrentPlay = "Ref_StopV3"-- by fjm 2022-03s-22
      else
       gCurrentPlay = "Ref_StopV2"
      end    
    --end
  end,

  Leader   = task.goCmuRush(MIDDLE_POS , dir.playerToBall, ACC, STOP_DSS),
  Assister = task.goCmuRush(INTER2_POS , dir.playerToBall, ACC, STOP_DSS),
  -- Breaker  = task.goCmuRush(standFunc(1), dir.playerToBall, ACC, STOP_DSS),
  -- Crosser  = task.goCmuRush(standFunc(2), dir.playerToBall, ACC, STOP_DSS),
  -- Engine   = task.goCmuRush(standFunc(3), dir.playerToBall, ACC, STOP_DSS), 
  -- Hawk     = task.goCmuRush(standFunc(4), dir.playerToBall, ACC, STOP_DSS),
  -- Finisher = task.goCmuRush(standFunc(5), dir.playerToBall, ACC, STOP_DSS),
  Special  = task.goCmuRush(SIDE2_POS, dir.playerToBall, ACC, STOP_DSS),
  Middle   = task.wback(2,1),--task.defendMiddle4Stop(),--TODO
  Defender = task.wback(2,2),
  Breaker  = task.goCmuRush(STOP_OTHER_PLAYER_POS(0),player.toBallDir,ACC,STOP_DSS),
  Receiver = task.goCmuRush(STOP_OTHER_PLAYER_POS(1),player.toBallDir,ACC,STOP_DSS),
  Goalie   = task.goalie(),
  match    = "[L][MD][ASBR]"
},

["11vs11Solver"] = {
  switch = function()
    if cond.isGameOn() then
      return "exit"
    end
    -- gBallPosXInStop = ball.posX()
    -- gBallPosYInStop = ball.posY()
    -- if bufcnt(cond.isGameOn(),5) then
    --   return "exit"
    -- elseif ball.toOurGoalPostDistSum() < PENALTY_THRESHOLD_DIST then
    --   return "standByPenalty"
    -- elseif ball.posY() > MIDDLE_THRESHOLD_Y or ball.posY() < -MIDDLE_THRESHOLD_Y then
    --   return "standByLine"
    --elseif ball.posY() < MIDDLE_THRESHOLD_Y and ball.posY() > -MIDDLE_THRESHOLD_Y then
     -- return "standInMiddle"
    -- else
    --   --return "exit"
    -- end
    -- if player.myvalid("Defender") then
    --   gCurrentPlay = "Ref_StopV2D"
    -- else
      if ball.refPosX() < -param.pitchLength/2 + param.penaltyDepth and math.abs(ball.refPosY()) > param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4CornerDef"
      elseif ball.refPosX() > param.pitchLength/2 - param.penaltyDepth and math.abs(ball.refPosY()) > param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4CornerKick"-- by fjm 2022-03-22
      elseif math.abs(ball.refPosY()) >= param.penaltyWidth/2 then
       gCurrentPlay = "Ref_Stop4SideLine" -- by fjm 2022-03-22
      elseif ball.refPosX() < -(param.pitchLength/2 - param.penaltyDepth - 1000) and -(param.pitchLength/2+400) <= ball.refPosX() and math.abs(ball.refPosY()) <= param.penaltyWidth/2 + 300 then--+ param.pitchWidth/6 then
       gCurrentPlay = "Ref_StopV3"-- by fjm 2022-03s-22
      else
       gCurrentPlay = "Ref_StopV2"
      end     
    --end
  end,

  Leader   = task.goCmuRush(MIDDLE_POS , dir.playerToBall, ACC, STOP_DSS),
  Assister = task.goCmuRush(INTER2_POS , dir.playerToBall, ACC, STOP_DSS),
  -- Breaker  = task.goCmuRush(standFunc(1), dir.playerToBall, ACC, STOP_DSS),
  -- Crosser  = task.goCmuRush(standFunc(2), dir.playerToBall, ACC, STOP_DSS),
  -- Engine   = task.goCmuRush(standFunc(3), dir.playerToBall, ACC, STOP_DSS), 
  -- Hawk     = task.goCmuRush(standFunc(4), dir.playerToBall, ACC, STOP_DSS),
  -- Finisher = task.goCmuRush(standFunc(5), dir.playerToBall, ACC, STOP_DSS),
  Special  = task.goCmuRush(SIDE2_POS, dir.playerToBall, ACC, STOP_DSS),
  Middle   = task.wback(2,1),--task.defendMiddle4Stop(),--TODO
  Defender = task.wback(2,2),
  Breaker  = task.goCmuRush(STOP_OTHER_PLAYER_POS(0),player.toBallDir,ACC,STOP_DSS),
  Receiver = task.goCmuRush(STOP_OTHER_PLAYER_POS(1),player.toBallDir,ACC,STOP_DSS),
  Fronter  = task.wmarking("Zero",  flag.avoid_stop_ball_circle,getFrontAttackerNum(0)),
  Kicker   = task.wmarking("First", flag.avoid_stop_ball_circle,getFrontAttackerNum(1)),
  Center   = task.wmarking("Second",flag.avoid_stop_ball_circle,getFrontAttackerNum(2)),
  Goalie   = task.goalie(),
  match    = "[L][MD][FKC][ASBR]"
},

-- ["standInMiddle"] = {
--   switch = function()
--      if ball.refPosX() < -param.pitchLength * 0.4 and math.abs(ball.refPosY()) > param.pitchWidth *0.33 then
--      gCurrentPlay = "Ref_Stop4CornerDef"
--     elseif ball.refPosX() > param.pitchLength * 0.4 and math.abs(ball.refPosY()) > param.pitchWidth *0.33 then
--      gCurrentPlay = "Ref_Stop4CornerKick"-- by fjm 2022-01-27
--     elseif math.abs(ball.refPosY()) > param.pitchWidth/2 * 0.78 then
--      gCurrentPlay = "Ref_Stop4SideLine" -- by fjm 2022-01-27
--     elseif ball.refPosX() < -(param.pitchLength/2 - param.penaltyDepth - 1000) and -(param.pitchLength/2 - param.penaltyDepth + 500)<= ball.refPosX() and math.abs(ball.refPosY()) < param.penaltyWidth/2 then--+ param.pitchWidth/6 then
--      gCurrentPlay = "Ref_StopV3"-- by fjm 2022-01-27
--     else
--      gCurrentPlay = "Ref_StopV2"
--     end  
--     if bufcnt(cond.isGameOn(),5) then
--       return "exit"
--     --elseif ball.toOurGoalDist() < PENALTY_THRESHOLD_DIST then
--     -- elseif ball.posX() < -(param.pitchLength/2 - param.penaltyDepth - 1000) then
--     --   -- gBallPosXInStop = ball.posX()
--     --   -- gBallPosYInStop = ball.posY()
--     --   return "standByPenalty"
--     -- elseif ball.posY() < MIDDLE_THRESHOLD_Y and ball.posY() > -MIDDLE_THRESHOLD_Y then
--     --   --if (math.abs(gBallPosXInStop - ball.posX()) >= 80 or math.abs(gBallPosYInStop - ball.posY()) >= 80) then
--     --     -- gBallPosXInStop = ball.posX()
--     --     -- gBallPosYInStop = ball.posY()
--     --     return "standInMiddle"
--     --   --end
--     --   -- if bufcnt( ((gRoleNum["Assister"] ~= 0) and (player.toTargetDist("Assister") > 80)) or
--     --   --            ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") > 80)) or
--     --   --            ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") > 80)) or
--     --   --            ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") > 80)) or
--     --   --            ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") > 80)), getBufTime()) then
--     --   --   return "reDoStop"
--     --   -- end
--     --   -- if bufcnt( ((gRoleNum["Assister"] ~= 0) and (player.toTargetDist("Assister") < 100)) and
--     --   --            ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") < 100)) and
--     --   --            ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") < 100)) and
--     --   --            ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") < 100)) and
--     --   --            ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") < 100)), 15) then
--     --   --   gBallPosXInStop = ball.posX()
--     --   --   gBallPosYInStop = ball.posY()
--     --   --   return nil
--     --   -- end
--     -- elseif ball.posY() > MIDDLE_THRESHOLD_Y  or ball.posY() < -MIDDLE_THRESHOLD_Y  then
--     --   -- gBallPosXInStop = ball.posX()
--     --   -- gBallPosYInStop = ball.posY()
--     --   return "standByLine"
--     -- else
--     --   --return "exit"
--     end  
--   end,

--   Leader   = task.goCmuRush(MIDDLE_POS, dir.playerToBall, ACC, STOP_DSS),
--   Assister = task.goCmuRush(SIDE2_POS , dir.playerToBall, ACC, STOP_DSS),
--   -- Breaker  = task.goCmuRush(standFunc(1), dir.playerToBall, ACC, STOP_DSS),
--   -- Crosser  = task.goCmuRush(standFunc(2), dir.playerToBall, ACC, STOP_DSS),
--   -- Engine   = task.goCmuRush(standFunc(3), dir.playerToBall, ACC, STOP_DSS), 
--   -- Hawk     = task.goCmuRush(standFunc(4), dir.playerToBall, ACC, STOP_DSS),
--   -- Finisher = task.goCmuRush(standFunc(5), dir.playerToBall, ACC, STOP_DSS),
--   Special  = task.goCmuRush(INTER2_POS , dir.playerToBall, ACC, STOP_DSS),
--   Middle   = task.tier(),--task.defendMiddle4Stop(),--TODO
--   Defender = task.tier1(),
--   Goalie   = task.goalie(),
--   match    = ""
-- },

-- ["standByPenalty"] = {
--   switch = function()
--     if ball.refPosX() < -param.pitchLength * 0.4 and math.abs(ball.refPosY()) > param.pitchWidth *0.33 then
--      gCurrentPlay = "Ref_Stop4CornerDef"
--     elseif ball.refPosX() > param.pitchLength * 0.4 and math.abs(ball.refPosY()) > param.pitchWidth *0.33 then
--      gCurrentPlay = "Ref_Stop4CornerKick"-- by fjm 2022-01-27
--     elseif math.abs(ball.refPosY()) > param.pitchWidth/2 * 0.78 then
--      gCurrentPlay = "Ref_Stop4SideLine" -- by fjm 2022-01-27
--     elseif ball.refPosX() < -(param.pitchLength/2 - param.penaltyDepth - 1000) and -(param.pitchLength/2 - param.penaltyDepth + 500)<= ball.refPosX() and math.abs(ball.refPosY()) < param.penaltyWidth/2 then--+ param.pitchWidth/6 then
--      gCurrentPlay = "Ref_StopV3"-- by fjm 2022-01-27
--     else
--      gCurrentPlay = "Ref_StopV2"
--     end  
--     if bufcnt(cond.isGameOn(),5) then
--       return "exit"
--     --elseif ball.toOurGoalDist() < PENALTY_THRESHOLD_DIST then
--     elseif ball.posX() < -(param.pitchLength/2 - param.penaltyDepth - 1000) then
--       -- gBallPosXInStop = ball.posX()
--       -- gBallPosYInStop = ball.posY()
--       return "standByPenalty"
--     elseif ball.posY() < MIDDLE_THRESHOLD_Y and ball.posY() > -MIDDLE_THRESHOLD_Y then
--       --if (math.abs(gBallPosXInStop - ball.posX()) >= 80 or math.abs(gBallPosYInStop - ball.posY()) >= 80) then
--         -- gBallPosXInStop = ball.posX()
--         -- gBallPosYInStop = ball.posY()
--         return "standInMiddle"
--       --end
--       -- if bufcnt( ((gRoleNum["Assister"] ~= 0) and (player.toTargetDist("Assister") > 80)) or
--       --            ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") > 80)) or
--       --            ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") > 80)) or
--       --            ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") > 80)) or
--       --            ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") > 80)), getBufTime()) then
--       --   return "reDoStop"
--       -- end
--       -- if bufcnt( ((gRoleNum["Assister"] ~= 0) and (player.toTargetDist("Assister") < 100)) and
--       --            ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") < 100)) and
--       --            ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") < 100)) and
--       --            ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") < 100)) and
--       --            ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") < 100)), 15) then
--       --   gBallPosXInStop = ball.posX()
--       --   gBallPosYInStop = ball.posY()
--       --   return nil
--       -- end
--     elseif ball.posY() > MIDDLE_THRESHOLD_Y  or ball.posY() < -MIDDLE_THRESHOLD_Y  then
--       -- gBallPosXInStop = ball.posX()
--       -- gBallPosYInStop = ball.posY()
--       return "standByLine"
--     else
--       --return "exit"
--     end  
--   end,

--   Assister = task.goCmuRush(standFunc(1), dir.playerToBall, ACC, STOP_DSS),
--   Special  = task.goCmuRush(standFunc(2), dir.playerToBall, ACC, STOP_DSS),
--   Leader   = task.goCmuRush(standFunc(3), dir.playerToBall, ACC, STOP_DSS),
--   Defender = task.goCmuRush(standFunc(4), dir.playerToBall, ACC, STOP_DSS),
--   Middle   = task.goCmuRush(standFunc(5), dir.playerToBall, ACC, STOP_DSS),
--   -- Fronter  = task.goCmuRush(standFunc(6), dir.playerToBall, ACC, STOP_DSS),
--   -- Center   = task.stop(),--task.goCmuRush(SEVEN_POS, dir.playerToBall, ACC, STOP_DSS),
--   Goalie   = task.goalie(),
--   match    = ""
-- },

-- ["standByLine"] = {
--   switch = function()
--      if ball.refPosX() < -param.pitchLength * 0.4 and math.abs(ball.refPosY()) > param.pitchWidth *0.33 then
--      gCurrentPlay = "Ref_Stop4CornerDef"
--     elseif ball.refPosX() > param.pitchLength * 0.4 and math.abs(ball.refPosY()) > param.pitchWidth *0.33 then
--      gCurrentPlay = "Ref_Stop4CornerKick"-- by fjm 2022-01-27
--     elseif math.abs(ball.refPosY()) > param.pitchWidth/2 * 0.78 then
--      gCurrentPlay = "Ref_Stop4SideLine" -- by fjm 2022-01-27
--     elseif ball.refPosX() < -(param.pitchLength/2 - param.penaltyDepth - 1000) and -(param.pitchLength/2 - param.penaltyDepth + 500)<= ball.refPosX() and math.abs(ball.refPosY()) < param.penaltyWidth/2 then--+ param.pitchWidth/6 then
--      gCurrentPlay = "Ref_StopV3"-- by fjm 2022-01-27
--     else
--      gCurrentPlay = "Ref_StopV2"
--     end  
--     if bufcnt(cond.isGameOn(),5) then
--       return "exit"
--     --elseif ball.toOurGoalDist() < PENALTY_THRESHOLD_DIST then
--     elseif ball.posX() < -(param.pitchLength/2 - param.penaltyDepth - 1000) then
--       -- gBallPosXInStop = ball.posX()
--       -- gBallPosYInStop = ball.posY()
--       return "standByPenalty"
--     elseif ball.posY() < MIDDLE_THRESHOLD_Y and ball.posY() > -MIDDLE_THRESHOLD_Y then
--       --if (math.abs(gBallPosXInStop - ball.posX()) >= 80 or math.abs(gBallPosYInStop - ball.posY()) >= 80) then
--         -- gBallPosXInStop = ball.posX()
--         -- gBallPosYInStop = ball.posY()
--         return "standInMiddle"
--       --end
--       -- if bufcnt( ((gRoleNum["Assister"] ~= 0) and (player.toTargetDist("Assister") > 80)) or
--       --            ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") > 80)) or
--       --            ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") > 80)) or
--       --            ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") > 80)) or
--       --            ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") > 80)), getBufTime()) then
--       --   return "reDoStop"
--       -- end
--       -- if bufcnt( ((gRoleNum["Assister"] ~= 0) and (player.toTargetDist("Assister") < 100)) and
--       --            ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") < 100)) and
--       --            ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") < 100)) and
--       --            ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") < 100)) and
--       --            ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") < 100)), 15) then
--       --   gBallPosXInStop = ball.posX()
--       --   gBallPosYInStop = ball.posY()
--       --   return nil
--       -- end
--     elseif ball.posY() > MIDDLE_THRESHOLD_Y  or ball.posY() < -MIDDLE_THRESHOLD_Y  then
--       -- gBallPosXInStop = ball.posX()
--       -- gBallPosYInStop = ball.posY()
--       return "standByLine"
--     else
--       --return "exit"
--     end  
--   end,

--   Leader   = task.goCmuRush(MIDDLE_POS, dir.playerToBall, ACC, STOP_DSS),
--   Assister = task.goCmuRush(OPPOSITE_SIDE_POS, dir.playerToBall, ACC, STOP_DSS),
--   Special  = task.goCmuRush(RECEIVE_POS, dir.playerToBall, ACC, STOP_DSS),
--   Middle   = task.tier(),--task.defendMiddle4Stop(),--TODO
--   --Center   = task.stop(),
--   --Fronter = task.stop(),
--   Defender = task.tier1(),
--   Goalie   = task.goalie(),
--   match    = ""
-- },

-- ["reDoStop"] = {
--   switch = function()
--     if bufcnt(true, 20) then
--       return "start"
--     end
--   end,

--   Assister = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(-1400,-1200)), 0, ACC, STOP_DSS),
--   Special  = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(-1400, 1200)), 0, ACC, STOP_DSS),
--   Leader   = task.goCmuRush(CGeoPoint:new_local(-1200, 0), 0, ACC, STOP_DSS),
--   Defender = task.goCmuRush(CGeoPoint:new_local(-2900, -1300), 0, ACC, STOP_DSS),
--   Middle   = task.goCmuRush(CGeoPoint:new_local(-2900, 1300), 0, ACC, STOP_DSS),
--   --Fronter  = task.goCmuRush(CGeoPoint:new_local(-290,260),dir.playerToBall,ACC,STOP_DSS),
--   --Center   = task.goCmuRush(CGeoPoint:new_local(-290,-260),dir.playerToBall,ACC,STOP_DSS),
--   Goalie   = task.goalie(),
--   match    = ""
-- },

name = "Ref_StopV2",
applicable = {
  exp = "a",
  a = true
},
attribute = "attack",
timeout = 99999
}