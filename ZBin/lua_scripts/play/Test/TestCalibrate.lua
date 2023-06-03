local f =  flag.allow_dss + flag.our_ball_placement
local DSS_FLAG = flag.dodge_ball + flag.allow_dss + flag.our_ball_placement + flag.avoid_stop_ball_circle
local pl  = param.pitchLength
local pw  = param.pitchWidth
local pew = param.penaltyWidth
local ped = param.penaltyDepth
local goal= param.goalWidth

local kicker_number = 0
local receiver_number = 0
local nkick = 0
local index = 1
local playerindex = 1
local diff = 1
local lastdist = 0
local count = 0
local maxballvel = 0
local lastpower = 0
local beginvel = 0
local endvel = 0
local spanframe = 0
local ballplace = false
local kicked = false
local Ldiff = false
local recorddiff = true
local Flag
local ballLast = CGeoPoint:new_local(0,0)
local ballpos = CGeoPoint:new_local(0,0)

local  max_power = 8500
local  min_power = 2000
local  step_power = 500

local pos1 = CGeoPoint(0,-param.pitchWidth/2)
local pos2 = CGeoPoint(-param.pitchLength/2,param.pitchWidth/2)
local pos3 = CGeoPoint(-param.pitchLength/4,0)
local vec =  pos1 - pos2

local get_kicker_num = function()
  return kicker_number
end

local get_receiver_num = function()
  return receiver_number
end

local power = function()
    return function()
      --if not IS_SIMULATION then 
      if index == 1 then
            --nkick = nkick+1
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

local pos_nums = {}

local Init = function()
  exist_nums = {}
  index = 1
  for i = 0,param.maxPlayer-1 do
    if player.valid(i) then
       table.insert(exist_nums,i)
    end
  end

    -- for i = 1,table.getn(exist_nums) do
    --   table.insert(shootorder,i)
    --   if i%2 == 1 then
    --     table.insert(array_nums,shootorder)
    --     for j = 1,table.getn(shootorder) do
    --         table.remove(shootorder,j)
    --     end
    --   end
    -- end

  if table.getn(exist_nums) <= 0 then
    print("Error : no robots in field?")
  end
  --kicker_number = array_nums[1][1]

  if ball.posY() > 0 then
     Flag = true
     pos_nums[1] =  pos3 + Utils.Polar2Vector(1000 + 100*nkick,vec:dir()) 
     pos_nums[2] =  pos3 + Utils.Polar2Vector(1000 + 100 * nkick,math.pi + vec:dir()) 
  else
     Flag = false
     pos_nums[1] =  pos3 + Utils.Polar2Vector(1000 + 100 * nkick,math.pi + vec:dir()) 
     pos_nums[2] =  pos3 + Utils.Polar2Vector(1000 + 100*nkick,vec:dir()) 
  end

  kicker_number = exist_nums[1]
  receiver_number = exist_nums[2]
end

local set_next_func = function()
  if kicked == true then
    return 
  end
  kicked = true

  local temp = receiver_number
  receiver_number = kicker_number
  kicker_number = temp

  count = count + 1

  if count == 2 then
    count = 0
    nkick = nkick + 1
  else
    lastpower = power()()
  end


end

local get_receiver_pos = function()
    return function()
        return pos_nums[diff]
    end
end


local getpredictPos = function(index)
  return function()
    return skillutils:getOurInterPoint(index)
  end
end



local KICK_TASK = function()
  --debugEngine:gui_debug_msg(CGeoPoint:new_local(1000,0),next_kicker)
  --gRoleNum["Leader"] = kicker_number
  --debugEngine:gui_debug_msg(CGeoPoint:new_local(-2000,0),gRoleNum["Leader"])
  --debugEngine:gui_debug_msg(CGeoPoint:new_local(-2000,2000),ball.toPlayerDist(kicker_number)-lastdist)
  -- if  ball.velMod()>500 and math.abs(ball.velDir() - ball.toPlayerDir(kicker_number)) < math.pi/3 then
  --   --lastdist = ball.toPlayerDist(kicker_number)
  --   return task.touchKick(player.pos(next_kicker),_,ball.toPointDist(player.pos(next_kicker))*1.2<3000 and 3000 or ball.toPointDist(player.pos(next_kicker))*1.2)
  -- end
  -- lastdist = ball.toPlayerDist(kicker_number)
    --return task.goandTurnKick(next_kicker,kicker_number)
  ballpos = ball.valid() and ball.pos() or ballLast
  if player.infraredOn(kicker_number) and not ball.valid() then
     ballpos = player.pos(kicker_number) + Utils.Polar2Vector(param.playerFrontToCenter + 17,player.dir(kick_number))
  end

  if ball.velMod() or player.infraredOn(kicker_number) then
     ballLast = ballpos
  end

  if not ball.valid() and not player.infraredOn(kicker_number) then
    ballpos = ballLast + (ballLast - player.pos(kicker_number)):unit() * 20
  end


  local idir = function()
    return (ballpos - get_receiver_pos()()):dir()
  end

  local direct = function()
    return (get_receiver_pos()() - ballpos):dir()
  end


  -- if Ldiff then
  --     return task.goCmuRush(getpredictPos(kicker_number),player.toBallDir,_,f)
  -- end

  if power()() == 8000 and count == 1 then
    if not ballplace then
       return task.goCmuRush(getpredictPos(kicker_number),player.toBallDir,_,f)
    else
       return task.fetchball(pos_nums[3],0,false)
    end
  end

  -- if player.toPointDist(receiver_number,get_receiver_pos()()) > 20 then
  --   if player.infraredCount(kicker_number) > 20 then
  --      return task.goCmuRush(player.shootGen(90,idir),direct,_,f)
  --   -- else
  --   --    return task.goCmuRush(player.shootGen(200,idir),direct,_,DSS_FLAG)
  --   end
  -- else
  if player.toPointDist(receiver_number,get_receiver_pos()()) < 50 * 10 then
    return task.goandTurnKick(get_receiver_pos(),_,power(),flag.our_ball_placement)
  else
    if player.infraredCount(kicker_number) > 20 then
      return task.goCmuRush(player.pos,ball.backDir(get_receiver_pos()),_,f)
    else
      return task.goandTurnKick(get_receiver_pos(),_,0,flag.our_ball_placement)
    end
  end
  --end

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
      if num == kicker_number or num == receiver_number then return -1 end
      return num
    end
end

local get_wait_pos = function(inum)
  return function()
    --local mynum = inum + 1 

    local num = exist_nums[inum]
    -- local total = table.getn(exist_nums)
    -- local realdist
    -- if ball.toPointDist(CGeoPoint:new_local(0,0)) < 700 then
    --   realdist = 700
    -- else
    --   realdist = ball.toPointDist(CGeoPoint:new_local(0,0))
    -- end
    -- return CGeoPoint:new_local(0,0) + Utils.Polar2Vector(realdist,math.pi/2 + math.pi*2/total*inum)
    local basePos = CGeoPoint:new_local(-param.pitchLength/2,-param.pitchWidth/2 - 150)
        return basePos + Utils.Polar2Vector(210 * (inum - 2),0)
   end
end

local get_wait_angle =function(inum)
  return function()
    local mynum = inum+1 

    local num = exist_nums[mynum]
    if num == receiver_number then
      return player.toBallDir(num)
    else
      if player.posY(num) < 0 then
        return math.pi/2
      else
        return -math.pi/2
      end
    end
  end
end

local Print_Debug = function()
  debugEngine:gui_debug_msg(CGeoPoint(-300,param.pitchWidth/2-200),"excute players: ".."kicker->"..kicker_number..", ".."receiver->"..receiver_number,5)
  debugEngine:gui_debug_msg(CGeoPoint(-300,param.pitchWidth/2-400),"now_power: "..power()(),5)
  debugEngine:gui_debug_msg(CGeoPoint(-300,param.pitchWidth/2-600),"ballmax_vel: "..string.format("%.2f",maxballvel),5)
  -- debugEngine:gui_debug_msg(CGeoPoint(-300,param.pitchWidth/2-800),"ball_begin_vel: "..beginvel,5)
  -- debugEngine:gui_debug_msg(CGeoPoint(-300,param.pitchWidth/2-1000),"ball_end_vel: "..endvel,5)
  -- debugEngine:gui_debug_msg(CGeoPoint(-300,param.pitchWidth/2-1200),"spanframe: "..spanframe,5)
  --debugEngine:gui_debug_msg(CGeoPoint(-300,param.pitchWidth/2-1400),"acc: "..(beginvel - endvel) / 10 *60,5)
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
          --set_next_func()
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
    Print_Debug()
    if player.kickBall(kicker_number) then
      -- time_after_kick = 0
      -- time_after_null = 0
      -- real_kicker = kicker_number
      -- carPos = player.pos(kicker_number)
      --lastPredictPos = ball.chipPredictPos()
      set_next_func()
      diff = (diff + 1) % 2 == 1 and 1 or 2
      kicked = true
      Ldiff = true
      return "pass-each-other"
    else
      kicked = false
      
    end


    if ball.toPlayerDist(kicker_number) < 150 then
      Ldiff = false
    end

    if player.toPointDist(kicker_number,ballpos) < 50 * 10 then
      f = f + flag.dribbling
    else
      f = flag.allow_dss + flag.our_ball_placement
    end

    if bufcnt(power()() == 8000 and count == 1 and player.infraredOn(kicker_number),30) then
       if ball.posY() < 0 then
          pos_nums[3] =  pos3 + Utils.Polar2Vector(1000 ,vec:dir()) 
       else
          pos_nums[3] =  pos3 + Utils.Polar2Vector(1000 ,math.pi + vec:dir())
       end
       ballplace = true
    end

    if bufcnt(ballplace and ball.toPointDist(pos_nums[3]) < 100,30) then
       playerindex = playerindex + 2
       if exist_nums[playerindex] == nil then
          kicker_number = -1
          receiver_number = -1
       else
          if exist_nums[playerindex + 1] ~= nil then
            kicker_number = exist_nums[playerindex]
            receiver_number = exist_nums[playerindex + 1]
          else
            kicker_number = exist_nums[playerindex]
            receiver_number = exist_nums[1]
          end
       end
       ballplace = false
       nkick = 0
    end

    if Flag then
      pos_nums[1] =  pos3 + Utils.Polar2Vector(1000 + 100 * nkick,vec:dir()) 
      pos_nums[2] =  pos3 + Utils.Polar2Vector(1000 + 100 * nkick,math.pi + vec:dir()) 
    else 
      pos_nums[1] =  pos3 + Utils.Polar2Vector(1000 + 100 * nkick,math.pi + vec:dir()) 
      pos_nums[2] =  pos3 + Utils.Polar2Vector(1000 + 100 * nkick,vec:dir()) 
    end

    if IS_SIMULATION then
       file = io.open("data/level_of_kick/data_simu.txt","a")
    else
       file = io.open("data/level_of_kick/data_real.txt","a")
    end
    if Ldiff then
      if ball.velMod() > maxballvel then
        maxballvel = ball.velMod()
      end
      recorddiff = true
    else
      if maxballvel ~= 0 then 
        file:write(string.format("%d",lastpower)," ",string.format("%.2f",maxballvel)," ",receiver_number," ","\n")
        file:close()
        recorddiff = false
      end
      if not recorddiff then
        maxballvel = 0
      end
    end

   end,
   [get_kicker_num]   = KICK_TASK,
   [get_receiver_num] = task.goCmuRush(get_receiver_pos(),player.toBallDir,ACC,DSS_FLAG),
   [get_wait_num(0 )] = task.goCmuRush(get_wait_pos( 1),get_wait_angle( 0),ACC,DSS_FLAG),
   [get_wait_num(1 )] = task.goCmuRush(get_wait_pos( 2),get_wait_angle( 1),ACC,DSS_FLAG),
   [get_wait_num(2 )] = task.goCmuRush(get_wait_pos( 3),get_wait_angle( 2),ACC,DSS_FLAG),
   [get_wait_num(3 )] = task.goCmuRush(get_wait_pos( 4),get_wait_angle( 3),ACC,DSS_FLAG),
   [get_wait_num(4 )] = task.goCmuRush(get_wait_pos( 5),get_wait_angle( 4),ACC,DSS_FLAG),
   [get_wait_num(5 )] = task.goCmuRush(get_wait_pos( 6),get_wait_angle( 5),ACC,DSS_FLAG),
   [get_wait_num(6 )] = task.goCmuRush(get_wait_pos( 7),get_wait_angle( 6),ACC,DSS_FLAG),
   [get_wait_num(7 )] = task.goCmuRush(get_wait_pos( 8),get_wait_angle( 7),ACC,DSS_FLAG),
   [get_wait_num(8 )] = task.goCmuRush(get_wait_pos( 9),get_wait_angle( 8),ACC,DSS_FLAG),
   [get_wait_num(9 )] = task.goCmuRush(get_wait_pos(10),get_wait_angle( 9),ACC,DSS_FLAG),
   [get_wait_num(10)] = task.goCmuRush(get_wait_pos(11),get_wait_angle(10),ACC,DSS_FLAG),
   [get_wait_num(11)] = task.goCmuRush(get_wait_pos(12),get_wait_angle(11),ACC,DSS_FLAG),
   [get_wait_num(12)] = task.goCmuRush(get_wait_pos(13),get_wait_angle(12),ACC,DSS_FLAG),
   [get_wait_num(13)] = task.goCmuRush(get_wait_pos(14),get_wait_angle(13),ACC,DSS_FLAG),
   [get_wait_num(14)] = task.goCmuRush(get_wait_pos(15),get_wait_angle(14),ACC,DSS_FLAG),
   [get_wait_num(15)] = task.goCmuRush(get_wait_pos(16),get_wait_angle(15),ACC,DSS_FLAG),
   match = ""
},

name= "TestCalibrate",
applicable={
  exp = "a",
  a = true
},
attribute = "attack",
timeout = 99999
}