local f = flag.dribbling + flag.allow_dss
local DSS_FLAG = flag.dodge_ball + flag.allow_dss --+ flag.avoid_shoot_line
local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth

local kicker_number = 0
local next_kicker = 0
local nkick = 0
local index = 1
local lastdist = 0
local needtouch = false
local kicked = false

local  max_power = 12500
local  min_power = 3000
local  step_power = 500

local get_kicker_num = function()
  return kicker_number
end

local power = function()
    return function()
      --if not IS_SIMULATION then 
      if index == 1 then
            nkick = nkick+1
      end
          if step_power*nkick + min_power > max_power then
              return max_power
          else
              return (step_power*nkick + min_power);
          end
      --end 
    --return 2000

  end
end


local posarray = {}

local exist_nums = {}

local shootorder = {}

local Init = function()
  exist_nums = {}
  index = 1
  for i = 0,param.maxPlayer-1 do
    if player.valid(i) then
       table.insert(exist_nums,i)
    end
  end

    for i = 1,table.getn(exist_nums) do
      if i%2 == 1 then
        table.insert(shootorder,i)
      end
    end
    for i = 1,table.getn(exist_nums) do
      if i%2 == 0 then
        table.insert(shootorder,i)
      end
    end
  if table.getn(exist_nums) <= 0 then
    print("Error : no robots in field?")
  end
  kicker_number = exist_nums[1]
end

local set_next_func = function()
  if kicked == true then
    return 
  end
  kicked = true
  --local next_kicker = 0
  local lens = table.getn(exist_nums)
  if lens == 3 then
     kicker_number = exist_nums[index] 
     index = index + 1
     if index > 3 then
       index = 1
     end
     next_kicker = exist_nums[index]
  elseif lens > 3 then
    kicker_number = exist_nums[shootorder[index]]
    index = index + 1
    if index > lens then
      index = 1
    end
      next_kicker = exist_nums[shootorder[index]]
  -- elseif lens == 5 then
  --   local shootorder = {1,3,5,2,4}
  --   kicker_number = exist_nums[shootorder[index]]
  --   index = index + 1
  --   if index > 5 then
  --     index = 1
  --   end
  --   next_kicker = exist_nums[shootorder[index]]
  -- elseif lens == 7 then
  --   local shootorder = {1,3,5,7,2,4,6}
  --   kicker_number = exist_nums[shootorder[index]]
  --   index = index + 1
  --   if index > 7 then
  --     index = 1
  --   end
  --   next_kicker = exist_nums[shootorder[index]]    
  end
end


local KICK_TASK = function()
  debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,0),next_kicker)
  gRoleNum["Leader"] = kicker_number
  debugEngine:gui_debug_msg(CGeoPoint:new_local(-2000,0),gRoleNum["Leader"])
  --debugEngine:gui_debug_msg(CGeoPoint:new_local(-2000,2000),ball.toPlayerDist(kicker_number)-lastdist)
  -- if  ball.velMod()>500 and math.abs(ball.velDir() - ball.toPlayerDir(kicker_number)) < math.pi/3 then
  --   --lastdist = ball.toPlayerDist(kicker_number)
  --   return task.touchKick(player.pos(next_kicker),_,ball.toPointDist(player.pos(next_kicker))*1.2<3000 and 3000 or ball.toPointDist(player.pos(next_kicker))*1.2)
  -- end
  -- lastdist = ball.toPlayerDist(kicker_number)
    return task.goandTurnKick(next_kicker,kicker_number)

end

local get_wait_num = function(inum)
  return function()
  local mynum = inum+1 
  
      if inum >= table.getn(exist_nums) then
        return -1
      end
      --local index = kicker_number + num
      --local num = right_side[index]
      local num = exist_nums[mynum]
      --debugEngine:gui_debug_msg(CGeoPoint:new_local(0,0),num)
      if num == kicker_number then return -1 end
      return num
    end
end

local get_wait_pos = function(inum)
  return function()
    local total = table.getn(exist_nums)
    local realdist
    if ball.toPointDist(CGeoPoint:new_local(0,0)) < 700 then
      realdist = 700
    else
      realdist = ball.toPointDist(CGeoPoint:new_local(0,0))
    end
    return CGeoPoint:new_local(0,0) + Utils.Polar2Vector(realdist,math.pi/2 + math.pi*2/total*inum)
  end
end



gPlayTable.CreatePlay{

firstState = "start",

["start"] = {
   switch = function()
      -- if bufcnt((player.toTargetDist("Leader")   < 150 or not player.myvalid("Leader")) and
      --           (player.toTargetDist("Assister") < 150 or not player.myvalid("Assister")) and
      --           (player.toTargetDist("Middle")   < 150 or not player.myvalid("Middle")) and
      --           (player.toTargetDist("Special")  < 150 or not player.myvalid("Special")) and
      --           (player.toTargetDist("Defender") < 150 or not player.myvalid("Defender")) 
      --           , 10, 300) then
      --   return "pass-each-other"
      -- end
          Init()
          set_next_func()
         -- waitPosinit(ball.toPointDist(CGeoPoint:new_local(0,0)))
          kicked = false
          return "pass-each-other"
   end,
   --["Leader"]   = task.goCmuRush(player.shootGen(250,idir),mydir,ACC,DSS_FLAG),
   --Assister = task.goCmuRush(ball.antiYPos(CGeoPoint:new_local(2000,2000)) ,0,ACC,DSS_FLAG),
   match = ""
},


["pass-each-other"] = {
   switch = function()
    --waitPosinit(ball.toPointDist(CGeoPoint:new_local(0,0)))
    --set_next_func()
    if player.kickBall(kicker_number) then
      -- time_after_kick = 0
      -- time_after_null = 0
      -- real_kicker = kicker_number
      -- carPos = player.pos(kicker_number)
      --lastPredictPos = ball.chipPredictPos()
      set_next_func()
      kicked = true
      --printDebug()
      return "pass-each-other"
    else
      kicked = false
    end
      -- if player.kickBall("Leader") then
      --   gRoleNum["Leader"] = 1
      -- end
      -- elseif player.kickBall("Assister") then
      --   return CGeoPoint:new_local(2000,2000)
      -- end
   end,
   ["Leader"]   = KICK_TASK,
   [get_wait_num(0)] = task.goCmuRush(get_wait_pos(1),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   --[get_wait_num(1)] = task.goCmuRush(get_wait_pos(2),0,ACC,DSS_FLAG),
   --["Assister"] = task.goCmuRush(get_wait_pos(1),0,ACC,DSS_FLAG),
   [get_wait_num(1)] = task.goCmuRush(get_wait_pos(2),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(2)] = task.goCmuRush(get_wait_pos(3),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(3)] = task.goCmuRush(get_wait_pos(4),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(4)] = task.goCmuRush(get_wait_pos(5),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(5)] = task.goCmuRush(get_wait_pos(6),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(6)] = task.goCmuRush(get_wait_pos(7),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(7)] = task.goCmuRush(get_wait_pos(8),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(8)] = task.goCmuRush(get_wait_pos(9),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(9)] = task.goCmuRush(get_wait_pos(10),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(10)] = task.goCmuRush(get_wait_pos(11),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(11)] = task.goCmuRush(get_wait_pos(12),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(12)] = task.goCmuRush(get_wait_pos(13),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(13)] = task.goCmuRush(get_wait_pos(14),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   [get_wait_num(14)] = task.goCmuRush(get_wait_pos(15),player.toPointDir(CGeoPoint:new_local(0,0)),ACC,DSS_FLAG),
   --Goalie   = task.goalie(),
   match = "[L]"
},

name= "TestOneState",
applicable={
  exp = "a",
  a = true
},
attribute = "attack",
timeout = 99999
}