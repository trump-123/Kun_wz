#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "skill/Factory.h"
#include "LuaModule.h"
#include "geometry.h"
#include "TaskMediator.h"
#include "BufferCounter.h"
#include "Global.h"
#include "messidecition.h"
#include <QString>

#ifndef _WIN32
#include <libgen.h>
#endif

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "tolua++.h"
TOLUA_API int  tolua_zeus_open (lua_State* tolua_S);

extern "C" {
	typedef struct 
	{
		const char *name;
		int (*func)(lua_State *);
	}luaDef;
}

extern luaDef GUIGlue[];

namespace{
	bool IS_SIMULATION = false;
    bool LUA_DEBUG = true;
}

CLuaModule::CLuaModule():m_pScriptContext(nullptr)
{
	m_pErrorHandler = NULL;

	m_pScriptContext = lua_open();
	luaL_openlibs(m_pScriptContext);
	tolua_zeus_open(m_pScriptContext);
    InitLuaGlueFunc();
    ZSS::ZParamManager::instance()->loadParam(LUA_DEBUG,"Debug/A_LuaDebug",true);

}

void CLuaModule::InitLuaGlueFunc()
{
	for(int i=0; GUIGlue[i].name; i++) {
		AddFunction(GUIGlue[i].name, GUIGlue[i].func);
	}
}

CLuaModule::~CLuaModule()
{
	if(m_pScriptContext)
		lua_close(m_pScriptContext);
}

static std::string findScript(const char *pFname)
{
	FILE *fTest;
#if defined(_WIN32)
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
    _splitpath( pFname, drive, dir, fname, ext );
    std::string scriptsString = "Scripts\\";
#else
    std::string pname = pFname;
    int l = pname.length();
    char name[l+1];
    strcpy(name,pname.c_str());
    std::string drive = "";
    std::string dir = dirname(name);
    std::string fname = basename(name);// Mark : not work /error
    std::string ext = "";
    std::string scriptsString = "Scripts/";
#endif
    std::string strTestFile(pFname);
    fTest = fopen(strTestFile.c_str(), "r");
    if(fTest == NULL)
    {
        //not that one...
        strTestFile = (std::string) drive + dir + scriptsString + fname + ".lua";
        fTest = fopen(strTestFile.c_str(), "r");
    }
	if(fTest == NULL)
	{
		//not that one...
        strTestFile = (std::string) drive + dir + scriptsString + fname + ".LUA";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if(fTest == NULL)
	{
		//not that one...
		strTestFile = (std::string) drive + dir + fname + ".LUB";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if(fTest == NULL)
	{
		//not that one...
		//not that one...
		strTestFile = (std::string) drive + dir + fname + ".LUA";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if(fTest != NULL)
	{
		fclose(fTest);
	}

	return strTestFile;
}

bool CLuaModule::RunScript(const char *pFname)
{
    std::string strFilename = findScript(pFname);
    const char *pFilename = strFilename.c_str();

	if (0 != luaL_loadfile(m_pScriptContext, pFilename))
    {
        double x = (ZSS::ZParamManager::instance()->value("ZAlert/IsRight").toBool()?1:-1)*(PARAM::Field::PITCH_LENGTH/2);
        qDebug() << QString("Lua Error - Script Run\nScript Name:%1\nError Message:%2\n").arg(pFilename).arg(luaL_checkstring(m_pScriptContext, -1));
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(x,-200),QString("Lua Error - Script Load").toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(x,-400),QString("Name:%1").arg(pFilename).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(x,-600),QString("Error Message:%1").arg(luaL_checkstring(m_pScriptContext, -1)).toLatin1());
        GDebugEngine::Instance()->send(!ZSS::ZParamManager::instance()->value("ZAlert/IsYellow").toBool());
        if(LUA_DEBUG){
            std::cout << "Press enter to continue ...";
            std::cin.get();
        }
            return false;
    }
	if (0 != lua_pcall(m_pScriptContext, 0, LUA_MULTRET, 0))
    {
        double x = (ZSS::ZParamManager::instance()->value("ZAlert/IsRight").toBool()?1:-1)*(PARAM::Field::PITCH_LENGTH/2+2000);//lua上看不到问题改这里
        qDebug() << QString("Lua Error - Script Run\nScript Name:%1\nError Message:%2\n").arg(pFilename).arg(luaL_checkstring(m_pScriptContext, -1));
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(x,-200),QString("Lua Error - Script Load").toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(x,-400),QString("Name:%1").arg(pFilename).toLatin1());
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(x,-600),QString("Error Message:%1").arg(luaL_checkstring(m_pScriptContext, -1)).toLatin1());
        GDebugEngine::Instance()->send(!ZSS::ZParamManager::instance()->value("ZAlert/IsYellow").toBool());
        if(LUA_DEBUG){
            std::cout << "Press enter to continue ...";
            std::cin.get();
        }
        return false;
    }
	return true;

}

bool CLuaModule::RunString(const char *pCommand)
{
	if (0 != luaL_loadbuffer(m_pScriptContext, pCommand, strlen(pCommand), NULL))
	{
		if(m_pErrorHandler)
		{
			char buf[256];
			sprintf(buf, "Lua Error - String Load\nString:%s\nError Message:%s\n", pCommand, luaL_checkstring(m_pScriptContext, -1));
			m_pErrorHandler(buf);
		}

		return false;
	}
	if (0 != lua_pcall(m_pScriptContext, 0, LUA_MULTRET, 0))
	{
		if(m_pErrorHandler)
		{
			char buf[256];
			sprintf(buf, "Lua Error - String Run\nString:%s\nError Message:%s\n", pCommand, luaL_checkstring(m_pScriptContext, -1));
			m_pErrorHandler(buf);
		}

		return false;
	}
	return true;
}

const char *CLuaModule::GetErrorString(void)
{
	return luaL_checkstring(m_pScriptContext, -1);
}


bool CLuaModule::AddFunction(const char *pFunctionName, LuaFunctionType pFunction)
{
	lua_register(m_pScriptContext, pFunctionName, pFunction);
	return true;
}

const char *CLuaModule::GetStringArgument(int num, const char *pDefault)
{
	return luaL_optstring(m_pScriptContext, num, pDefault);

}

double CLuaModule::GetNumberArgument(int num, double dDefault)
{
	return luaL_optnumber(m_pScriptContext, num, dDefault);
}

bool CLuaModule::GetBoolArgument(int num)
{
	return lua_toboolean(m_pScriptContext, num);
}

CGeoPoint* CLuaModule::GetPointArgument(int num)
{
	return (CGeoPoint*)(lua_touserdata(m_pScriptContext, num));
}

void CLuaModule::PushString(const char *pString)
{
	lua_pushstring(m_pScriptContext, pString);
}

void CLuaModule::PushNumber(double value)
{
	lua_pushnumber(m_pScriptContext, value);
}

void CLuaModule::PushBool(bool value)
{
	lua_pushboolean(m_pScriptContext, value);
}

////////////////////////////////////////////////////////////////////
///                  Foundamental Skill                          ///
////////////////////////////////////////////////////////////////////
extern "C" int Skill_NoneZeroGoCmuRush(lua_State *L)
{
    TaskT playerTask;
    playerTask.player.is_specify_ctrl_method = true;
    playerTask.player.specified_ctrl_method = CMU_TRAJ;
    int runner = LuaModule::Instance()->GetNumberArgument(1, NULL);
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2, NULL);
    double y = LuaModule::Instance()->GetNumberArgument(3, NULL);
    playerTask.player.pos = CGeoPoint(x, y);
    playerTask.player.angle = LuaModule::Instance()->GetNumberArgument(4, NULL);
    playerTask.player.flag = LuaModule::Instance()->GetNumberArgument(5, NULL);
    playerTask.ball.Sender = LuaModule::Instance()->GetNumberArgument(6, NULL);
    playerTask.player.max_acceleration = LuaModule::Instance()->GetNumberArgument(7, NULL);
    playerTask.player.needdribble = LuaModule::Instance()->GetNumberArgument(8, NULL);
    double velX = LuaModule::Instance()->GetNumberArgument(9, NULL);
    double velY = LuaModule::Instance()->GetNumberArgument(10, NULL);
    playerTask.player.vel = CVector(velX, velY);

    CPlayerTask* pTask = TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_SimpleGotoPoint(lua_State *L)
{
    int runner = LuaModule::Instance()->GetNumberArgument(1, NULL);
    double x = LuaModule::Instance()->GetNumberArgument(2, NULL);
    double y = LuaModule::Instance()->GetNumberArgument(3, NULL);
    double angle = LuaModule::Instance()->GetNumberArgument(4, NULL);
    int flag = LuaModule::Instance()->GetNumberArgument(5, NULL);
    CPlayerTask* pTask = PlayerRole::makeItSimpleGoto(runner, CGeoPoint(x, y), angle, flag);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_WDrag(lua_State* L){
    int runner    = LuaModule::Instance()->GetNumberArgument(1, 0.0);
    double x      = LuaModule::Instance()->GetNumberArgument(2, 0.0);
    double y      = LuaModule::Instance()->GetNumberArgument(3, 0.0);
    double tx     = LuaModule::Instance()->GetNumberArgument(4, 0.0);
    double ty     = LuaModule::Instance()->GetNumberArgument(5, 0.0);
    int runner_int = static_cast<int>( runner );
    CGeoPoint pos = CGeoPoint(x, y);
    CGeoPoint target = CGeoPoint(tx, ty);
    CPlayerTask* pTask = PlayerRole::makeItWDrag(runner_int, pos, target);
    TaskMediator::Instance()->setPlayerTask(runner_int, pTask, 1);
    return 0;
}

extern "C" int Skill_WBreak(lua_State * L){
    int runner  = LuaModule::Instance()->GetNumberArgument(1, 0.0);
    double px   = LuaModule::Instance()->GetNumberArgument(2, 0.0);
    double py   = LuaModule::Instance()->GetNumberArgument(3, 0.0);
    double power= LuaModule::Instance()->GetNumberArgument(4, 0.0);
    int kick_flag    = LuaModule::Instance()->GetNumberArgument(5, 0.0);
    double precision = LuaModule::Instance()->GetNumberArgument(6, 0.0);
    int is_penalty   = LuaModule::Instance()->GetNumberArgument(7, 0.0);
    CGeoPoint target(px,py);
    CPlayerTask *pTask = PlayerRole::makeItWBreak(runner,target,power,kick_flag,precision,is_penalty);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);
    return 0;
}


extern "C" int Skill_StaticGetBall(lua_State * L){
    int runner  = LuaModule::Instance()->GetNumberArgument(1, 0.0);
    double px   = LuaModule::Instance()->GetNumberArgument(2, 0.0);
    double py   = LuaModule::Instance()->GetNumberArgument(3, 0.0);
    double kpower= LuaModule::Instance()->GetNumberArgument(4, 0.0);
    double cpower= LuaModule::Instance()->GetNumberArgument(5, 0.0);
    CGeoPoint target(px,py);
    CPlayerTask *pTask = PlayerRole::makeItStatic(runner,target,kpower,cpower);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);
    return 0;
}

extern "C" int Skill_Stop(lua_State *L)
{
    int runner = LuaModule::Instance()->GetNumberArgument(1, NULL);
    CPlayerTask* pTask = PlayerRole::makeItStop(runner);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Register_Role(lua_State *L)
{
    int num = LuaModule::Instance()->GetNumberArgument(1, NULL);
    string role = LuaModule::Instance()->GetStringArgument(2, NULL);
    TaskMediator::Instance()->setRoleInLua(num, role);
    return 0;
}

extern "C" int Skill_Speed(lua_State *L)
{
    int runner = LuaModule::Instance()->GetNumberArgument(1,NULL);
    double speedX = LuaModule::Instance()->GetNumberArgument(2,NULL);
    double speedY = LuaModule::Instance()->GetNumberArgument(3,NULL);
    double rotSpeed = LuaModule::Instance()->GetNumberArgument(4,NULL);
    CPlayerTask* pTask = PlayerRole::makeItRun(runner, speedX, speedY, rotSpeed, 0);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);
    return 0;
}
extern "C" int Skill_OpenSpeed(lua_State *L)
{
    int runner = LuaModule::Instance()->GetNumberArgument(1,NULL);
    double speedX = LuaModule::Instance()->GetNumberArgument(2,NULL);
    double speedY = LuaModule::Instance()->GetNumberArgument(3,NULL);
    double rotSpeed = LuaModule::Instance()->GetNumberArgument(4,NULL);
    CPlayerTask* pTask = PlayerRole::makeItOpenRun(runner, speedX, speedY, rotSpeed, 0);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);
    return 0;
}

extern "C" int FUNC_TimeOut(lua_State* L)
{
    bool cond = LuaModule::Instance()->GetBoolArgument(1);
    int buf = LuaModule::Instance()->GetNumberArgument(2, NULL);
    int cnt = LuaModule::Instance()->GetNumberArgument(3, 9999);

    if(BufferCounter::Instance()->isClear(vision->getCycle())){
        BufferCounter::Instance()->startCount(vision->getCycle(), cond, buf, cnt);
    }

    if(BufferCounter::Instance()->timeOut(vision->getCycle(), cond)){
        LuaModule::Instance()->PushNumber(1);
    } else{
        LuaModule::Instance()->PushNumber(0);
    }
    return 1;
}

extern "C" int FUNC_GetIsSimulation(lua_State* L)
{
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION,"Alert/IsSimulation",false);

    LuaModule::Instance()->PushBool(IS_SIMULATION);
    return 1;
}

extern "C" int FUNC_GetSettings(lua_State* L){
    QString key(LuaModule::Instance()->GetStringArgument(1, NULL));
    QString type(LuaModule::Instance()->GetStringArgument(2, NULL));
    if(type == "Bool"){
        bool temp;
        ZSS::ZParamManager::instance()->loadParam(temp,key);
        LuaModule::Instance()->PushBool(temp);
    }else if(type == "Int"){
        int temp;
        ZSS::ZParamManager::instance()->loadParam(temp,key);
        LuaModule::Instance()->PushNumber(temp);
    }else if(type == "Double"){
        double temp;
        ZSS::ZParamManager::instance()->loadParam(temp,key);
        LuaModule::Instance()->PushNumber(temp);
    }else{
        QString temp;
        ZSS::ZParamManager::instance()->loadParam(temp,key);
        LuaModule::Instance()->PushString(temp.toLatin1());
    }
    return 1;
}

extern "C" int FUNC_PrintString(lua_State* L) {
    const char* str = LuaModule::Instance()->GetStringArgument(1, NULL);
    printf("%s\n",str);
    fflush(stdout);
    return 0;
}
extern "C" int Skill_SpeedInRobot(lua_State* L){
    int runner = LuaModule::Instance()->GetNumberArgument(1,NULL);
    double speedX = LuaModule::Instance()->GetNumberArgument(2,NULL);
    double speedY = LuaModule::Instance()->GetNumberArgument(3,NULL);
    double rotSpeed = LuaModule::Instance()->GetNumberArgument(4,NULL);
    CVector localVel(speedX, speedY);
    CVector globalVel = localVel.rotate(vision->ourPlayer(runner).Dir());
    DribbleStatus::Instance()->setDribbleCommand(runner,3);
    CPlayerTask* pTask = PlayerRole::makeItRun(runner, globalVel.x(), globalVel.y(), rotSpeed, 0);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);
    return 0;
}

extern "C" int Skill_SmartGotoPoint(lua_State *L)
{
	TaskT playerTask;
	int runner = LuaModule::Instance()->GetNumberArgument(1, NULL);
	playerTask.executor = runner;
	double x = LuaModule::Instance()->GetNumberArgument(2, NULL);
	double y = LuaModule::Instance()->GetNumberArgument(3, NULL);
	playerTask.player.pos = CGeoPoint(x,y);
	playerTask.player.rotvel = 0;
	playerTask.player.angle = LuaModule::Instance()->GetNumberArgument(4, NULL);
	playerTask.player.flag = LuaModule::Instance()->GetNumberArgument(5, NULL);
	playerTask.ball.Sender = LuaModule::Instance()->GetNumberArgument(6,NULL);
	playerTask.player.max_acceleration = LuaModule::Instance()->GetNumberArgument(7,NULL);
    double vx = LuaModule::Instance()->GetNumberArgument(8, 0.0);
    double vy = LuaModule::Instance()->GetNumberArgument(9, 0.0);
    playerTask.player.vel = CVector(vx,vy);

	CPlayerTask* pTask = TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
	TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);
	
	return 0;
}

extern "C" int Skill_GoCmuRush(lua_State *L)
{
	TaskT playerTask;
	playerTask.player.is_specify_ctrl_method = true;
    playerTask.player.specified_ctrl_method = CMU_TRAJ;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
	playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2, 0);
    double y = LuaModule::Instance()->GetNumberArgument(3, 0);
	playerTask.player.pos = CGeoPoint(x,y);
    playerTask.player.angle = LuaModule::Instance()->GetNumberArgument(4, 0);
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(5, 0));
    playerTask.ball.Sender = int(LuaModule::Instance()->GetNumberArgument(6,0));
    playerTask.player.max_acceleration = LuaModule::Instance()->GetNumberArgument(7,0);
    playerTask.player.needdribble = int(LuaModule::Instance()->GetNumberArgument(8,0));
    double vx = LuaModule::Instance()->GetNumberArgument(9,0.0);
    double vy = LuaModule::Instance()->GetNumberArgument(10,0.0);
    playerTask.player.vel = CVector(vx,vy);

	CPlayerTask* pTask = TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
	TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

	return 0;
}
////////////////////////////////////////////////////////////////////
///                        My Skill                              ///
////////////////////////////////////////////////////////////////////

//extern "C" int Skill_GetBallV1(lua_State *L)
//{
//    int runner = LuaModule::Instance()->GetNumberArgument(1, NULL);
//    //double angle = LuaModule::Instance()->GetNumberArgument(2, NULL);
//    double px = LuaModule::Instance()->GetNumberArgument(2, NULL);
//    double py = LuaModule::Instance()->GetNumberArgument(3, NULL);
//    double wpx = LuaModule::Instance()->GetNumberArgument(4, NULL);
//    double wpy = LuaModule::Instance()->GetNumberArgument(5, NULL);
//    double power = LuaModule::Instance()->GetNumberArgument(6, NULL);
//    int kick_flag = LuaModule::Instance()->GetNumberArgument(7, NULL);
//    double precision = LuaModule::Instance()->GetNumberArgument(8, NULL);
//    CGeoPoint pos = CGeoPoint(px, py);
//    CGeoPoint waitPos = CGeoPoint(wpx, wpy);

//    CPlayerTask* pTask = PlayerRole::makeItGetBallV1(runner, kick_flag, pos, waitPos, power, precision);
//    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

//    return 0;
//}

extern "C" int Skill_Tier(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->Tier(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_Tier1(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->Tier1(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_WBack(lua_State *L){
    int runner = LuaModule::Instance()->GetNumberArgument(1,0.0);
    int guardNum = LuaModule::Instance()->GetNumberArgument(2,0.0);
    int index = LuaModule::Instance()->GetNumberArgument(3,0.0);
    int defendNum = LuaModule::Instance()->GetNumberArgument(4,0.0);
    int flag = LuaModule::Instance()->GetNumberArgument(5,0.0);
    if(defendNum < 0) defendNum = -1;
    CPlayerTask* pTask = PlayerRole::makeItWBack(runner,guardNum,index,defendNum,flag);
    TaskMediator::Instance()->setPlayerTask(runner,pTask,1);
    return 0;
}

extern "C" int Skill_Uniquekillm(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->Uniqueskillm(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_Uniquekilll(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->Uniqueskilll(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_Uniquekillr(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->Uniqueskillr(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_SingleTier(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->SingleTier(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_LeftHalf(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    playerTask.player.max_acceleration = int(LuaModule::Instance()->GetNumberArgument(3, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->LeftHalf(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_RightHalf(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    playerTask.player.max_acceleration = int(LuaModule::Instance()->GetNumberArgument(3, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->RightHalf(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_MiddleHalf(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    playerTask.player.max_acceleration = int(LuaModule::Instance()->GetNumberArgument(3, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->MiddleHalf(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}


extern "C" int Skill_Goalie(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    double x = LuaModule::Instance()->GetNumberArgument(3,0);
    double y = LuaModule::Instance()->GetNumberArgument(4,0);
    playerTask.player.pos = CGeoPoint(x,y);//.dist(CGeoPoint(-99999,-99999)) < 1e-8 ? CGeoPoint(500,0) : CGeoPoint(x,y);
    CPlayerTask* pTask = TaskFactoryV2::Instance()->Goalie(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_PenaltyGoalie(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->PenaltyGoalie(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_defend(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->defend(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_naqiu(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(2, 0));
    CPlayerTask* pTask = TaskFactoryV2::Instance()->naqiu(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_Touch(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2, 0);
    double y = LuaModule::Instance()->GetNumberArgument(3, 0);
    playerTask.player.pos = CGeoPoint(x,y);
    bool useInter = LuaModule::Instance()->GetBoolArgument(4);
    playerTask.player.is_specify_ctrl_method = useInter;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->Touch(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_ChaseKick(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2, 0);
    double y = LuaModule::Instance()->GetNumberArgument(3, 0);
    playerTask.player.pos = CGeoPoint(x,y);
    bool useInter = LuaModule::Instance()->GetBoolArgument(4);
    playerTask.player.is_specify_ctrl_method = useInter;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->ChaseKick(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_goAndTurnKick(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2, 0);
    double y = LuaModule::Instance()->GetNumberArgument(3, 0);
    playerTask.player.pos = CGeoPoint(x,y);
    bool useInter = LuaModule::Instance()->GetBoolArgument(4);
    double power = LuaModule::Instance()->GetNumberArgument(5,0);
    int flag = LuaModule::Instance()->GetNumberArgument(6,0);
    playerTask.player.angle = power;
    playerTask.player.flag = flag;
    playerTask.player.is_specify_ctrl_method = useInter;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->goAndTurnKick(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}


extern "C" int Skill_PenaltyKick(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1,0));
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2,0);
    double y = LuaModule::Instance()->GetNumberArgument(3,0);
    playerTask.player.pos = CGeoPoint(x,y);
    bool useInter = LuaModule::Instance()->GetNumberArgument(4);
    playerTask.player.is_specify_ctrl_method = useInter;
    CPlayerTask *pTask = TaskFactoryV2::Instance()->PenaltyKick(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_BIGpenaltyKick(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1,0));
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2,0);
    double y = LuaModule::Instance()->GetNumberArgument(3,0);
    playerTask.player.pos = CGeoPoint(x,y);
    bool useInter = LuaModule::Instance()->GetNumberArgument(4);
    playerTask.player.is_specify_ctrl_method = useInter;
    CPlayerTask *pTask = TaskFactoryV2::Instance()->BIGpenaltyKick(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_FetchBall(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2, 0);
    double y = LuaModule::Instance()->GetNumberArgument(3, 0);
    playerTask.player.pos = CGeoPoint(x,y);
    double power = LuaModule::Instance()->GetNumberArgument(4,0);
    playerTask.player.kickpower = power;
    bool useInter = LuaModule::Instance()->GetBoolArgument(5);
    playerTask.player.is_specify_ctrl_method = useInter;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->FetchBall(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_Escape(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2, 0);
    double y = LuaModule::Instance()->GetNumberArgument(3, 0);
    playerTask.player.pos = CGeoPoint(x,y);
    bool useInter = LuaModule::Instance()->GetBoolArgument(4);
    playerTask.player.is_specify_ctrl_method = useInter;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->Escape(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_Marking(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2, 0);
    double y = LuaModule::Instance()->GetNumberArgument(3, 0);
    playerTask.player.pos = CGeoPoint(x,y);
    bool useInter = LuaModule::Instance()->GetBoolArgument(4);
    playerTask.player.is_specify_ctrl_method = useInter;
    playerTask.player.max_acceleration = LuaModule::Instance()->GetNumberArgument(5,0);
    CPlayerTask* pTask = TaskFactoryV2::Instance()->Marking(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_MarkingFront(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2, 0);
    double y = LuaModule::Instance()->GetNumberArgument(3, 0);
    playerTask.player.pos = CGeoPoint(x,y);
    bool useInter = LuaModule::Instance()->GetBoolArgument(4);
    playerTask.player.is_specify_ctrl_method = useInter;
    playerTask.player.max_acceleration = LuaModule::Instance()->GetNumberArgument(5,0);
    CPlayerTask* pTask = TaskFactoryV2::Instance()->MarkingFront(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_MarkingTouch(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2, 0);
    double y = LuaModule::Instance()->GetNumberArgument(3, 0);
    playerTask.player.pos = CGeoPoint(x,y);
    bool useInter = LuaModule::Instance()->GetBoolArgument(4);
    playerTask.player.is_specify_ctrl_method = useInter;
    playerTask.player.max_acceleration = LuaModule::Instance()->GetNumberArgument(5,0);
    CPlayerTask* pTask = TaskFactoryV2::Instance()->MarkingTouch(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

//extern "C" int Skill_Defend(lua_State *L)
//{
//    TaskT playerTask;
//    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
//    playerTask.executor = runner;
//    double x = LuaModule::Instance()->GetNumberArgument(2, 0);
//    double y = LuaModule::Instance()->GetNumberArgument(3, 0);
//    playerTask.player.pos = CGeoPoint(x,y);
//    bool useInter = LuaModule::Instance()->GetBoolArgument(4);
//    playerTask.player.is_specify_ctrl_method = useInter;
//    CPlayerTask* pTask = TaskFactoryV2::Instance()->Defend(playerTask);
//    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

//    return 0;
//}


///////////////////////////////////////////////////////////////////////
///                       MY SKILL                                  //
//////////////////////////////////////////////////////////////////////
extern "C" int Skill_Leader(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->leader(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}
extern "C" int Skill_Assister(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->assister(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_sao(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->assister(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_Special(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->special(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}
extern "C" int Skill_Defend_05(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->defend_05(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}
extern "C" int Skill_Defend_07_0(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->defend_07_0(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_guan(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    playerTask.player.needkick = false;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->guan(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_Defend_07_1(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->defend_07_1(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}
extern "C" int Skill_Leader_getball(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    CPlayerTask* pTask = TaskFactoryV2::Instance()->leader_getball(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_DnamicfindPos(lua_State *L)
{
    TaskT playerTask;
    int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
    playerTask.executor = runner;
    double x = LuaModule::Instance()->GetNumberArgument(2, 0);
    double y = LuaModule::Instance()->GetNumberArgument(3, 0);
    playerTask.player.pos = CGeoPoint(x,y);
    playerTask.player.angle = LuaModule::Instance()->GetNumberArgument(4, 0);
    CPlayerTask* pTask = TaskFactoryV2::Instance()->Dnamicfindpos(playerTask);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

    return 0;
}

extern "C" int Skill_WMarking(lua_State *L)
{
    int runner = LuaModule::Instance()->GetNumberArgument(1,NULL);
    int pri    = LuaModule::Instance()->GetNumberArgument(2,NULL);
    int flag   = LuaModule::Instance()->GetNumberArgument(3,NULL);
    int num    = LuaModule::Instance()->GetNumberArgument(4,NULL);
    int bestEnemy = DefenceInfo::Instance()->getAttackOppNumByPri(0);
    if (DefenceInfo::Instance()->getOppPlayerByNum(bestEnemy)->isTheRole("RReceiver")) {
        if (pri > 0) {
            pri -= 1;
        }
    }
    int oppNum = num >= 0 ? num : DefenceInfo::Instance()->getSteadyAttackOppNumByPri(pri);
    CPlayerTask* pTask = PlayerRole::makeItWMarkEnemy(runner, oppNum, pri, flag);
    TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);
    return 0;
}

extern "C" int FUNC_GetWMarkingPos(lua_State *L){
    int robotNum = LuaModule::Instance()->GetNumberArgument(1,NULL);
    int pri = LuaModule::Instance()->GetNumberArgument(2,NULL);
    int flag = LuaModule::Instance()->GetNumberArgument(3,NULL);
    int num  = LuaModule::Instance()->GetNumberArgument(4,NULL);
    CGeoPoint markPos;
    int bestEnemy = DefenceInfo::Instance()->getAttackOppNumByPri(0);

    if(DefenceInfo::Instance()->getOppPlayerByNum(bestEnemy)->isTheRole("RReceiver")){
        if(pri>0){
            pri = -1;
        }
    }
    int oppNum = num >=0 ? num : DefenceInfo::Instance()->getSteadyAttackOppNumByPri(pri);
    markPos = ZSkillUtils::instance()->getZMarkingPos(vision,robotNum,oppNum,pri,flag);
    LuaModule::Instance()->PushNumber(markPos.x());
    LuaModule::Instance()->PushNumber(markPos.y());
    return 2;
}

extern "C" int FUNC_WGetBallPos(lua_State *L){
    int robotNum = LuaModule::Instance()->GetNumberArgument(1,0);
    if(robotNum == 0)
        robotNum = ZSkillUtils::instance()->getOurBestPlayer();
    CGeoPoint matchPos;
    if(Utils::InOurPenaltyArea(MessiDecision::Instance()->getBallPos(),9*10))
    {
        matchPos = CGeoPoint(0-500,90);
    }
    else if(Utils::InTheirPenaltyArea(MessiDecision::Instance()->getBallPos(),1*10))
    {
        matchPos = CGeoPoint(0+500,90);
    }else {
        matchPos = ZSkillUtils::instance()->getOurInterPoint(robotNum);
    }
    LuaModule::Instance()->PushNumber(matchPos.x());
    LuaModule::Instance()->PushNumber(matchPos.y());
    return 2;
}

luaDef GUIGlue[] = 
{
    {"SmartGotoPos",		Skill_SmartGotoPoint},
	{"SimpleGotoPos",		Skill_SimpleGotoPoint},
    {"StopRobot",			Skill_Stop},
    {"CRegisterRole",		Register_Role},
    {"CTimeOut",			FUNC_TimeOut},
	{"CGetIsSimulation",	FUNC_GetIsSimulation},
    {"CGetSettings",        FUNC_GetSettings},
    {"CGetWMarkingPos",     FUNC_GetWMarkingPos},
    {"CWGetBallPos",        FUNC_WGetBallPos},
    {"CSpeed",				Skill_Speed},
    {"COpenSpeed",			Skill_OpenSpeed},
    {"CGoCmuRush",			Skill_GoCmuRush},
    {"CGoalie",             Skill_Goalie},
    {"CTouch",              Skill_Touch},
    {"CChaseKick",          Skill_ChaseKick},
    {"CgoAndTurnKick",      Skill_goAndTurnKick},
    //{"CGetBallV1",          Skill_GetBallV1},
    {"CNoneZeroGoCmuRush",	Skill_NoneZeroGoCmuRush},
    {"CSpeedInRobot",		Skill_SpeedInRobot},
    //////////////////////////////////
    ///         MY SKILL           ///
    /////////////////////////////////
    {"CLeader",             Skill_Leader},
    {"CAssister",           Skill_Assister},
    {"CSpecial",            Skill_Special},
    {"Csao",                Skill_sao},
    {"CDefend_05",          Skill_Defend_05},
    {"CDefend_07_0",        Skill_Defend_07_0},
    {"CDefend_07_1",        Skill_Defend_07_1},
    {"CLeader_getball",     Skill_Leader_getball},
    {"CPenaltyKick",        Skill_PenaltyKick},
    {"CTier",               Skill_Tier},
    {"CWBack",              Skill_WBack},
    {"Cguan",				Skill_guan},
    {"CTier1",              Skill_Tier1},
    {"CUniquekillm",        Skill_Uniquekillm},
    {"CUniquekilll",        Skill_Uniquekilll},
    {"CUniquekillr",        Skill_Uniquekillr},
    {"CSingleTier",         Skill_SingleTier},
    {"Cnaqiu",              Skill_naqiu},
    {"Cdefend",             Skill_defend},
    {"CPenaltyGoalie",      Skill_PenaltyGoalie},
    {"CFetchBall",          Skill_FetchBall},
    {"CEscape",             Skill_Escape},
    {"CMarking",            Skill_Marking},
    {"CMarkingFront",       Skill_MarkingFront},
    {"CMarkingTouch",       Skill_MarkingTouch},
    {"CLeftHalf",           Skill_LeftHalf},
    {"CRightHalf",          Skill_RightHalf},
    {"CMiddleHalf",         Skill_MiddleHalf},
    {"CDnamicfindpos",      Skill_DnamicfindPos},
    {"CWDrag",              Skill_WDrag},
    {"CWMarking",           Skill_WMarking},
    {"CWBreak",             Skill_WBreak},
    {"CBIGpenaltyKick",     Skill_BIGpenaltyKick},
    {"CStaticGetBall",      Skill_StaticGetBall},
//    {"CDefend",             Skill_Defend},
    ///////////////////////////////////////
	{NULL, NULL}
};
