-- if ball.refPosX() < -param.pitchLength/3 and math.abs(ball.refPosY()) > param.pitchWidth/3 then
-- 	gCurrentPlay = "Ref_Stop4CornerDef"
-- elseif ball.refPosX() > param.pitchLength/3 and math.abs(ball.refPosY()) > param.pitchWidth/3 then
-- 	gCurrentPlay = "Ref_Stop4CornerKick"
-- else
-- 	gCurrentPlay = "Ref_StopV1"
-- end

--gCurrentPlay = "Ref_StopV1"
-- if ball.refPosX() < -param.pitchLength * 0.4 and math.abs(ball.refPosY()) > param.pitchWidth *0.33 then
--  gCurrentPlay = "Ref_Stop4CornerDef"
-- elseif ball.refPosX() > param.pitchLength * 0.4 and math.abs(ball.refPosY()) > param.pitchWidth *0.33 then
--  gCurrentPlay = "Ref_Stop4CornerKick"-- by fjm 2022-01-27
-- elseif math.abs(ball.refPosY()) > param.pitchWidth/2 * 0.78 then
--  gCurrentPlay = "Ref_Stop4SideLine" -- by fjm 2022-01-27
-- elseif ball.refPosX() < -(param.pitchLength/2 - param.penaltyDepth - 1000) and -(param.pitchLength/2 - param.penaltyDepth + 500)<= ball.refPosX() and math.abs(ball.refPosY()) < param.penaltyWidth/2 then--+ param.pitchWidth/6 then
--  gCurrentPlay = "Ref_StopV3"-- by fjm 2022-01-27
-- else
--  gCurrentPlay = "Ref_StopV2"
-- end  
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
