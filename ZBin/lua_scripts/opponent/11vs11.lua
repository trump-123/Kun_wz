gOppoConfig = {
-------------------------Play--------------------------------------------
	CornerKick  = {1110,1104,1105,1106,1107,1108,1109},--已调试
	FrontKick   = {1102,1103,1104},        --完成
	MiddleKick  = {1105,1103,1104},        --完成
	BackKick    = {1102,1103},             --完成
	KickOff     = {1101,1102},             --接近球也是开球启动程序


-----------------------Def-----------------------------------------------
	CornerDef   = "Ref_CornerDef11vs11",   --已调试 --弱队V5 强队 V6
	BackDef     = "Ref_BackDef11vs11",     --已调试  弱队V2 强队V3
	MiddleDef   = "Ref_MiddleDef11vs11",   --完成
	FrontDef    = "Ref_FrontDef11vs11",    --完成
	KickOffDef  = "Ref_KickOffDef11vs11",  --完成


---------------------Penalty and Normal-----------------------------------
	PenaltyKick = "Ref_PenaltyKick_11vs11", --强队用V0， 弱队用V1 大点球用V3 已调试,1
	
	PenaltyDef  = "Ref_PenaltyDef_11vs11",  --大点球用V1 小点球V0
	
	NorPlay     = "NormalPlayMessi_11vs11", 

	BallPlace   = "Ref_BallPlace_11vs11",


------------------- Other ------------------------------------------------
	GameOver    = "Ref_GameOverV2",         --省赛用V1 国赛用V2
	IfHalfField = false,
	USE_ZPASS   = false
}

