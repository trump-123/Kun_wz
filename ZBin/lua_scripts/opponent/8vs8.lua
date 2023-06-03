gOppoConfig = {
-------------------------Play--------------------------------------------
	CornerKick  = {810,804,805,806,807,808,809},--已调试
	FrontKick   = {802,803,804},      --完成
	MiddleKick  = {805,803,804},        --完成
	BackKick    = {802,803},      --完成
	KickOff     = {801,802},--接近球也是开球启动程序


-----------------------Def-----------------------------------------------
	CornerDef   = "Ref_CornerDef8vs8",  --已调试 --弱队V5 强队 V6
	BackDef     = "Ref_BackDef8vs8",    --已调试  弱队V2 强队V3
	MiddleDef   = "Ref_MiddleDef8vs8",  --完成
	FrontDef    = "Ref_FrontDef8vs8",   --完成
	KickOffDef  = "Ref_KickOffDef8vs8", --完成
	-- KickOffDef  = "Ref_KickOffDefV1",


---------------------Penalty and Normal-----------------------------------
	PenaltyKick = "Ref_PenaltyKick_8vs8",    --强队用V0， 弱队用V1 大点球用V3 已调试,1
	
	PenaltyDef  = "Ref_PenaltyDef_8vs8",     --大点球用V1 小点球V0
	
	NorPlay     = "NormalPlayMessi_8vs8",

	BallPlace   = "Ref_BallPlace_8vs8",


------------------- Other ------------------------------------------------
	GameOver    = "Ref_GameOverV2",   --省赛用V1 国赛用V2
	IfHalfField = false,
	USE_ZPASS = false
}

