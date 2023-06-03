IS_TEST_MODE      = true
IS_SIMULATION     = CGetIsSimulation()
USE_SWITCH        = false
USE_AUTO_REFEREE  = true
OPPONENT_NAME     = "8vs8"
SAO_ACTION        = CGetSettings("Alert/SaoAction","Int")
IS_YELLOW         = CGetSettings("ZAlert/IsYellow","Bool")
IS_RIGHT          = CGetSettings("ZAlert/IsRight", "Bool")
DEBUG_MATCH       = CGetSettings("Debug/RoleMatch","Bool")
USE_11vs11_SOLVER = CGetSettings("Messi/USE_11vs11SOLVER","Bool")

gStateFileNameString = string.format(os.date("%Y%m%d%H%M"))

gTestPlay = "TestTouchDir"--not IS_YELLOW and "ceyan" or "Ref_PenaltyDef_8vs8"

gRoleFixNum = {
        ["Assister"] = {},
        ["Breaker"]  = {},
        ["Center"]   = {},
        ["Defender"] = {},
        ["Leader"]   = {},
        ["Kicker"]   = {},
        ["Middle"]   = {},
        ["Special"]  = {},
        ["Tier"]     = {},
        ["Goalie"]   = {0},
        ["a"]        = {},
        ["e"]        = {},
        ["b"]        = {},
}

-- 用来进行定位球的保持




-- 在考虑智能性时用table来进行配置，用于OurIndirectKick
gOurIndirectTable = {
        -- 在OurIndirectKick控制脚本中可以进行改变的值
        -- 上一次定位球的Cycle
        lastRefCycle = 0
}

gSkill = {
        "SmartGoto",
        "SimpleGoto",
        "RunMultiPos",
        "Stop",
        "Naqiu",
        "Goalie",
        "Tier1",
        "Tier",
        "guan",
        "Touch",
        "OpenSpeed",
        "Speed",
        "GotoMatchPos",
        "GoCmuRush",
        "NoneZeroRush",
        "SpeedInRobot",
-----------------------------------------------
--                  MY Skill                 --
-----------------------------------------------
        "Leader",
        "Assister",
        "Special",
        "Defend_05",
        "Defend_07_0",
        "Defend_07_1",
        "Leader_getball",
        "Penaltykick",
        "PenaltyGoalie",
        "FetchBall",
        "Escape",
        "Marking",
        "LeftHalf",
        "RightHalf",
        "MiddleHalf",
        "MarkingFront",
        "MarkingTouch",
        "SingleTier",
        "ChaseKick",
        "Text",
        "Dnamicfindpos",
        "Uniquekillskillm",
        "Uniquekillskilll",
        "Uniquekillskillr",
        "goAndTurnKick",
        "WDrag",
        "WMarking",
        "WBack",
        "StaticGetBall",
        "BIGPenaltykick"
---------------------------------------------------------------------------------------
}

gRefPlayTable = {
-- GameHalt-----------------------------------------------------------------------------        
         "Ref/Ref_HaltV1",--
-- TimeOut------------------------------------------------------------------------------        
         "Ref/Ref_OurTimeoutV1",--
-- GameOver-----------------------------------------------------------------------------        
         "Ref/GameOver/Ref_GameOverV1",--6v6用V1
         "Ref/GameOver/Ref_GameOverV2",--8v8用V2
-- GameStop----------------------------------------------------------------------------        
         "Ref/GameStop/Ref_StopV2",
         "Ref/GameStop/Ref_StopV3",
         "Ref/GameStop/Ref_Stop4SideLine",
         "Ref/GameStop/Ref_Stop4CornerKick",
         "Ref/GameStop/Ref_Stop4CornerDef",

-- BallPlacement--------------------------------------------------------------------------
        "Ref/BallPlacement/Ref_BallPlace_6vs6",
        "Ref/BallPlacement/Ref_BallPlace_8vs8",
        "Ref/BallPlacement/Ref_BallPlace_11vs11",


-- Penalty--------------------------------------------------------------------------------
        --Def
        "Ref/PenaltyDef/Ref_PenaltyDef_6vs6",
        "Ref/PenaltyDef/Ref_PenaltyDef_8vs8",
        "Ref/PenaltyDef/Ref_PenaltyDef_11vs11",

        --Kick
        "Ref/PenaltyKick/Ref_PenaltyKick_6vs6",
        "Ref/PenaltyKick/Ref_PenaltyKick_8vs8",
        "Ref/PenaltyKick/Ref_PenaltyKick_11vs11",
        "Ref/PenaltyKick/Ref_BIGPenaltyKick_6vs6",
        "Ref/PenaltyKick/Ref_BIGPenaltyKick_8vs8",
        "Ref/PenaltyKick/Ref_BIGPenaltyKick_11vs11",

-- KickOff   开球-----------------------------------------------------------------------
         --KickOff_Def
         "Ref/KickOffDef/Ref_KickOffDef6vs6",
         "Ref/KickOffDef/Ref_KickOffDef8vs8",
         "Ref/KickOffDef/Ref_KickOffDef11vs11",

         --KickOff_Play
         "Ref/KickOff/Ref_KickOffV601",
         "Ref/KickOff/Ref_KickOffV801",
         "Ref/KickOff/Ref_KickOffV1101",
         "Ref/KickOff/Ref_KickOffV602",
         "Ref/KickOff/Ref_KickOffV802",
         "Ref/KickOff/Ref_KickOffV1102",

-- FreeKickDef-------------------------------------------------------------------
         --CornerDef
         "Ref/CornerDef/Ref_CornerDef8vs8",
         "Ref/CornerDef/Ref_CornerDef11vs11",

         --FrontDef
         "Ref/FrontDef/Ref_FrontDef8vs8",
         "Ref/FrontDef/Ref_FrontDef11vs11",

         --MiddleDef
         "Ref/MiddleDef/Ref_MiddleDef8vs8", 
         "Ref/MiddleDef/Ref_MiddleDef11vs11", 

         --BackDef
         "Ref/BackDef/Ref_BackDef8vs8",
         "Ref/BackDef/Ref_BackDef11vs11",

-- FreeKick------------------------------------------------------------------------
         --CornerKick
         "Ref/CornerKick/Ref_CornerKickV804",
         "Ref/CornerKick/Ref_CornerKickV805",
         "Ref/CornerKick/Ref_CornerKickV806",
         "Ref/CornerKick/Ref_CornerKickV807",
         "Ref/CornerKick/Ref_CornerKickV808",
         "Ref/CornerKick/Ref_CornerKickV809",
         "Ref/CornerKick/Ref_CornerKickV810",
         "Ref/CornerKick/Ref_CornerKickV1104",
         "Ref/CornerKick/Ref_CornerKickV1105",
         "Ref/CornerKick/Ref_CornerKickV1106",
         "Ref/CornerKick/Ref_CornerKickV1107",
         "Ref/CornerKick/Ref_CornerKickV1108",
         "Ref/CornerKick/Ref_CornerKickV1109",
         "Ref/CornerKick/Ref_CornerKickV1110",


         --FrontKick
        "Ref/FrontKick/Ref_FrontKickV802",--三车上前版
        "Ref/FrontKick/Ref_FrontKickV803",--三车上前版
        "Ref/FrontKick/Ref_FrontKickV804",
        "Ref/FrontKick/Ref_FrontKickV1102",--三车上前版
        "Ref/FrontKick/Ref_FrontKickV1103",--三车上前版
        "Ref/FrontKick/Ref_FrontKickV1104",


         --MiddleKick
         "Ref/MiddleKick/Ref_MiddleKickV803",
         "Ref/MiddleKick/Ref_MiddleKickV804",
         "Ref/MiddleKick/Ref_MiddleKickV805",
         "Ref/MiddleKick/Ref_MiddleKickV1103",
         "Ref/MiddleKick/Ref_MiddleKickV1104",
         "Ref/MiddleKick/Ref_MiddleKickV1105",

         --BackKick
         "Ref/BackKick/Ref_BackKickV802",
         "Ref/BackKick/Ref_BackKickV803",
         "Ref/BackKick/Ref_BackKickV1102",
         "Ref/BackKick/Ref_BackKickV1103",
}

gBayesPlayTable = {
        "Nor/NormalPlayMessi_6vs6",
        "Nor/NormalPlayMessi_8vs8",
        "Nor/NormalPlayMessi_11vs11",
        "Nor/NormalPlayMessi_8vs8toNBUT",
}

gTestPlayTable = {
        "Test/TestRun",
        "Test/testt",
        "Test/testback",
        "Test/TestSkill",
        "Test/testskill",
        "Test/TestDribble",
        "Test/TestTouchDir",
        "Test/test",
        "Test/chipkicktogoal",
        "Test/besideball",
        "Test/cornerpassshoot",
        "Test/kickoffv1",
        "Test/Def_",
        "Test/ceyan",
        "Test/sss",
        "Test/pretect",
        "Test/speed1",
        "Test/TestOneState",
        "Test/TestDribbleAndKick",
        "Test/RunMilitaryBoxing",
        "Test/IndirectKick",
        "Test/TestCalibrate",
        "Test/TestBallAcc",
        "Test/0113",
}
