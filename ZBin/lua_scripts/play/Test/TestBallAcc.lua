local count = 0
local startpos = CGeoPoint:new_local(0,0) --球运动第50帧记录的位置
local endpos = CGeoPoint:new_local(0,0) --球运动第70帧记录的位置
local startvel = 0
local endvel = 0
local acc
local stoppos = CGeoPoint(0,0)

gPlayTable.CreatePlay{

  firstState = "start",

  ["start"] = {
    switch = function ()
        if ball.velMod() > 0 then
            count = count + 1
        else
            count = 0
        end

        if count == 50  then
            startpos = CGeoPoint(ball.posX(),ball.posY())
            startvel = ball.velMod()
        elseif count == 70  then
            endpos = CGeoPoint(ball.posX(),ball.posY())
            endvel = ball.velMod()
        end

        acc = (math.pow(startvel,2) - math.pow(endvel,2))/(2*startpos:dist(endpos))

        local s = math.pow(ball.velMod(),2) / (2*1037)
        stoppos = ball.pos() + Utils.Polar2Vector(s,ball.velDir())

        debugEngine:gui_debug_msg(CGeoPoint(0,-200),s)
        debugEngine:gui_debug_msg(CGeoPoint(0,0),acc)
        debugEngine:gui_debug_x(startpos,3)
        debugEngine:gui_debug_x(endpos,4)
        debugEngine:gui_debug_msg(CGeoPoint(0,150),count)
        debugEngine:gui_debug_msg(CGeoPoint(0,300),startvel)
        debugEngine:gui_debug_msg(CGeoPoint(0,450),endvel)
        debugEngine:gui_debug_arc(stoppos,100,0.0,360.0,2)
    end,
    --Leader = task.goandTurnKick(CGeoPoint(0,0),_,2000),
    match = "[L]"
  },




name= "TestBallAcc",
applicable={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}