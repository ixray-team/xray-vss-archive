// xrGame.cpp : Defines the entry point for the DLL application.
//
 
#include "stdafx.h"
#include "gamepersistent.h"

#pragma comment(lib,"ode.lib" )

#include "..\xr_ioconsole.h"
#include "..\xr_ioc_cmd.h"
#include "..\customhud.h"
#include "ai_console.h"
#include "Actor_Flags.h"
#include "ai\\crow\\ai_crow.h"
#include "ai\\rat\\ai_rat.h"
#include "ai\\soldier\\ai_soldier.h"
#include "ai\\zombie\\ai_zombie.h"
#include "car.h"
#include "dummyobject.h"
#include "customtarget.h"
#include "..\fdemorecord.h"
#include "..\fdemoplay.h"
#include "a_star.h"
#include "game_sv_single.h"
    
ENGINE_API extern u32		psAlwaysRun;
ENGINE_API extern float		psHUD_FOV;
extern int	 				psPhysicsFPS;
extern float				psSqueezeVelocity;

// console commands
class CCC_Spawn : public CConsoleCommand
{
public:
	CCC_Spawn(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		R_ASSERT(pCreator);

		char	Name[128];	Name[0]=0;
		sscanf	(args,"%s", Name);
		Level().g_cl_Spawn	(Name,0xff,M_SPAWN_OBJECT_ACTIVE | M_SPAWN_OBJECT_LOCAL);
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"name,team,squad,group"); 
	}
};
class CCC_Restart : public CConsoleCommand {
public:
	CCC_Restart(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if(Level().Server) {
			Level().Server->game->OnRoundEnd("GAME_restarted");
			Level().Server->game->round = -1;
			// Level().Server->game->OnRoundStart();
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"restart game"); 
	}
};
class CCC_Team : public CConsoleCommand {
public:
	CCC_Team(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		u32 l_team = 2;
		sscanf(args, "%d", &l_team);
		CObject *l_pObj = Level().CurrentEntity();
		CActor *l_pActor = dynamic_cast<CActor*>(l_pObj);
		if(l_pActor) {
			if(l_pActor->g_Team() == s32(l_team)) return;
			Fvector l_dir; l_dir.set(0, -1.f, 0);
			Fvector position_in_bone_space;
			position_in_bone_space.set(0.f,0.f,0.f);
			NET_Packet		P;
			l_pActor->u_EventGen		(P,GE_HIT,l_pActor->ID());
			P.w_u16			(u16(l_pActor->ID()));
			P.w_dir			(l_dir);
			P.w_float		(10000.f);
			P.w_s16			((s16)0);
			P.w_vec3		(position_in_bone_space);
			l_pActor->u_EventSend		(P);
			//NET_Packet		P;
			//l_pActor->u_EventGen		(P,GE_DIE,l_pActor->ID()	);
			//P.w_u16			(u16(l_pActor->ID())	);
			//P.w_u32			(0);
			//l_pActor->u_EventSend		(P);
		}
		CGameObject *l_pPlayer = dynamic_cast<CGameObject*>(l_pObj);
		if(l_pPlayer) {
			NET_Packet		P;
			l_pPlayer->u_EventGen		(P,GEG_PLAYER_CHANGE_TEAM,l_pPlayer->ID()	);
			P.w_u16			(u16(l_pPlayer->ID())	);
			P.w_s16			(s16(l_team));
			P.w_s16			((s16)0);
			//P.w_u32			(0);
			l_pPlayer->u_EventSend		(P);
		}
	}
	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"change team"); 
	}
};
class CCC_Money : public CConsoleCommand {
public:
	CCC_Money(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		s32 l_money;
		if(sscanf(args, "%d", &l_money) == 1) {
			if(Level().Server) {
				u32 l_cnt = Level().Server->game->get_count();
				while(l_cnt--) {
					game_PlayerState *l_pPs = Level().Server->game->get_it(l_cnt);
					l_pPs->money_total = l_money;
				}
			}
		}
	}
	virtual void Info(TInfo& I) {
		strcpy(I,"give money"); 
	}
};

class CCC_ALifePath : public CConsoleCommand {
public:
	CCC_ALifePath(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (!Level().AI.bfCheckIfGraphLoaded())
			Msg("! there is no graph!");
		else {
			int id1=-1, id2=-1;
			sscanf(args ,"%d %d",&id1,&id2);
			if ((id1 != -1) && (id2 != -1))
				if (_max(id1,id2) > (int)Level().AI.GraphHeader().dwVertexCount - 1)
					Msg("! there are only %d vertexes!",Level().AI.GraphHeader().dwVertexCount);
				else
					if (min(id1,id2) < 0)
						Msg("! invalid vertex number (%d)!",min(id1,id2));
					else {
						Sleep				(1);
						u64 t1x = CPU::GetCycleCount();
						float fValue = Level().AI.m_tpAStar->ffFindMinimalPath(id1,id2);
						u64 t2x = CPU::GetCycleCount();
						t2x -= t1x;
						Msg("* %7.2f[%d] : %11I64u cycles (%.3f microseconds)",fValue,Level().AI.m_tpAStar->m_tpaNodes.size(),t2x,CPU::cycles2microsec*t2x);
					}
			else
				Msg("! not enough parameters!");
		}
	}
};

class CCC_ALifeSave : public CConsoleCommand {
public:
	CCC_ALifeSave(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				tpGame->m_tALife.Save();
				Log("* ALife simulation is successfully saved!");
			}
			else
				Log("!ALife simulation cannot be saved!");
		}
		else
			Log("!Not a single player game!");
	}
};

#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
class CCC_ALifeListAll : public CConsoleCommand {
public:
	CCC_ALifeListAll(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				tpGame->m_tALife.vfListObjects();
				tpGame->m_tALife.vfListEvents();
				tpGame->m_tALife.vfListTasks();
				tpGame->m_tALife.vfListLocations();
				tpGame->m_tALife.vfListTerrain();
				tpGame->m_tALife.vfListSpawnPoints();
			}
			else
				Log("!ALife simulation cannot be saved!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeListObjects : public CConsoleCommand {
public:
	CCC_ALifeListObjects(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				tpGame->m_tALife.vfListObjects();
			}
			else
				Log("!ALife simulation cannot be saved!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeListEvents : public CConsoleCommand {
public:
	CCC_ALifeListEvents(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				tpGame->m_tALife.vfListEvents();
			}
			else
				Log("!ALife parameters are not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeListTasks : public CConsoleCommand {
public:
	CCC_ALifeListTasks(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				tpGame->m_tALife.vfListTasks();
			}
			else
				Log("!ALife parameters are not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeListLocations : public CConsoleCommand {
public:
	CCC_ALifeListLocations(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				tpGame->m_tALife.vfListLocations();
			}
			else
				Log("!ALife parameters are not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeListTerrain : public CConsoleCommand {
public:
	CCC_ALifeListTerrain(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				tpGame->m_tALife.vfListTerrain();
			}
			else
				Log("!ALife parameters are not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeListSpawns : public CConsoleCommand {
public:
	CCC_ALifeListSpawns(LPCSTR N) : CConsoleCommand(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				tpGame->m_tALife.vfListSpawnPoints();
			}
			else
				Log("!ALife parameters are not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeObjectInfo : public CConsoleCommand {
public:
	CCC_ALifeObjectInfo(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				ALife::_OBJECT_ID id1 = ALife::_OBJECT_ID(-1);
				sscanf(args ,"%d",&id1);
				if (id1 >= tpGame->m_tALife.m_tObjectID)
					Msg("Invalid object ID! (%d)",id1);
				else
					tpGame->m_tALife.vfObjectInfo(id1);
			}
			else
				Log("!ALife parameters are not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeEventInfo : public CConsoleCommand {
public:
	CCC_ALifeEventInfo(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				ALife::_EVENT_ID id1 = ALife::_EVENT_ID(-1);
				sscanf(args ,"%d",&id1);
				if (id1 >= tpGame->m_tALife.m_tEventID)
					Msg("Invalid event ID! (%d)",id1);
				else
					tpGame->m_tALife.vfEventInfo(id1);
			}
			else
				Log("!ALife parameters are not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeTaskInfo : public CConsoleCommand {
public:
	CCC_ALifeTaskInfo(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				ALife::_TASK_ID id1 = ALife::_TASK_ID(-1);
				sscanf(args ,"%d",&id1);
				if (id1 >= tpGame->m_tALife.m_tTaskID)
					Msg("Invalid task ID! (%d)",id1);
				else
					tpGame->m_tALife.vfTaskInfo(id1);
			}
			else
				Log("!ALife parameters are not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeSpawnInfo : public CConsoleCommand {
public:
	CCC_ALifeSpawnInfo(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				u32 id1 = u32(-1);
				sscanf(args ,"%d",&id1);
				if (id1 >= tpGame->m_tALife.m_tpSpawnPoints.size())
					Msg("Invalid task ID! (%d)",id1);
				else {
					ALife::_SPAWN_ID id = ALife::_SPAWN_ID(id1);
					tpGame->m_tALife.vfSpawnPointInfo(id);
				}
			}
			else
				Log("!ALife parameters are not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeGraphInfo : public CConsoleCommand {
public:
	CCC_ALifeGraphInfo(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				u32 id1 = u32(-1);
				sscanf(args ,"%d",&id1);
				if (id1 >= Level().AI.GraphHeader().dwVertexCount)
					Msg("Invalid task ID! (%d)",id1);
				else {
					ALife::_GRAPH_ID id = ALife::_GRAPH_ID(id1);
					tpGame->m_tALife.vfGraphVertexInfo(id);
				}
			}
			else
				Log("!ALife parameters are not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

class CCC_ALifeTimeFactor : public CConsoleCommand {
public:
	CCC_ALifeTimeFactor(LPCSTR N) : CConsoleCommand(N)  { };
	virtual void Execute(LPCSTR args) {
		if (Level().game.type == GAME_SINGLE) {
			game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
			if (tpGame && tpGame->m_tALife.m_bLoaded) {
				float id1 = 0.0f;
				sscanf(args ,"%f",&id1);
				if (id1 < EPS_L)
					Msg("Invalid time factor! (%.4f)",id1);
				else
					tpGame->m_tALife.vfSetTimeFactor(id1);
			}
			else
				Log("!ALife parameters are not loaded!");
		}
		else
			Log("!Not a single player game!");
	}
};

#endif
//-----------------------------------------------------------------------
class CCC_DemoRecord : public CConsoleCommand
{
public:
	CCC_DemoRecord(LPCSTR N) : CConsoleCommand(N) {};
	virtual void Execute(LPCSTR args) {
		Console.Hide	();
		char fn[256]; strconcat(fn,args,".xrdemo");
		pCreator->Cameras.AddEffector(new CDemoRecord(fn));
	}
};
class CCC_DemoPlay : public CConsoleCommand
{
public:
	CCC_DemoPlay(LPCSTR N) : 
	  CConsoleCommand(N) 
	  { bEmptyArgsHandled = TRUE; };
	  virtual void Execute(LPCSTR args) {
		  Console.Hide();
		  char fn[256]; strconcat(fn,args,".xrdemo");
		  pCreator->Cameras.AddEffector(new CDemoPlay(fn,1.3f));
	  }
};

BOOL APIENTRY DllMain( HANDLE hModule, 
                       u32  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
		// game
		CMD3(CCC_Mask,				"g_always_run",			&psActorFlags,	AF_ALWAYSRUN);
		CMD3(CCC_Mask,				"g_god",				&psActorFlags,	AF_GODMODE	);
		CMD1(CCC_Spawn,				"g_spawn"				);
		CMD1(CCC_Restart,			"g_restart"				);
		CMD1(CCC_Money,				"g_money"				);
		CMD1(CCC_Team,				"g_change_team"			);
		
		// alife
		CMD1(CCC_ALifePath,			"al_path"				);		// build path
		CMD1(CCC_ALifeSave,			"al_save"				);		// save alife state
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
		CMD1(CCC_ALifeListAll,		"al_la"					);		// list all (objects, events and tasks)
		CMD1(CCC_ALifeListObjects,	"al_lo"					);		// list objects
		CMD1(CCC_ALifeListEvents,	"al_le"					);		// list events
		CMD1(CCC_ALifeListTasks,	"al_lt"					);		// list tasks
		CMD1(CCC_ALifeListLocations,"al_ll"					);		// list location owners
		CMD1(CCC_ALifeListTerrain,	"al_lr"					);		// list terrain
		CMD1(CCC_ALifeListSpawns,	"al_ls"					);		// list spawnpoints
		CMD1(CCC_ALifeObjectInfo,	"al_io"					);		// object info
		CMD1(CCC_ALifeEventInfo,	"al_ie"					);		// event info
		CMD1(CCC_ALifeTaskInfo,		"al_it"					);		// task info
		CMD1(CCC_ALifeSpawnInfo,	"al_is"					);		// spawn-point info
		CMD1(CCC_ALifeGraphInfo,	"al_ig"					);		// spawn-point info
		CMD1(CCC_ALifeTimeFactor,	"al_time_factor"		);		// set time factor
#endif

		// hud
		CMD3(CCC_Mask,				"hud_crosshair",		&psHUD_Flags,	HUD_CROSSHAIR);
		CMD3(CCC_Mask,				"hud_crosshair_dist",	&psHUD_Flags,	HUD_CROSSHAIR_DIST);
		CMD3(CCC_Mask,				"hud_weapon",			&psHUD_Flags,	HUD_WEAPON);
		CMD3(CCC_Mask,				"hud_info",				&psHUD_Flags,	HUD_INFO);
		CMD3(CCC_Mask,				"hud_draw",				&psHUD_Flags,	HUD_DRAW);
		CMD2(CCC_Float,				"hud_fov",				&psHUD_FOV);

		// Demo
#ifdef DEBUG
		CMD1(CCC_DemoRecord,		"demo_record"			);
#endif
		CMD1(CCC_DemoPlay,			"demo_play"				);

		// ai
		CMD3(CCC_Mask,				"ai_debug",				&psAI_Flags,	aiDebug);
		CMD3(CCC_Mask,				"ai_dbg_brain",			&psAI_Flags,	aiBrain);
		CMD3(CCC_Mask,				"ai_dbg_motion",		&psAI_Flags,	aiMotion);
		CMD3(CCC_Mask,				"ai_dbg_frustum",		&psAI_Flags,	aiFrustum);

		// physics
		CMD4(CCC_Integer,			"ph_fps",				&psPhysicsFPS,	10,1000);
		CMD4(CCC_Float,				"ph_squeeze_velocity",	&psHUD_FOV,		0,500);
		}
		GamePersistent		= new CGamePersistent();
		break;
	case DLL_PROCESS_DETACH:
		xr_delete(GamePersistent);
		break;
	}
    return TRUE;
}

// Factory
#include "actor.h"
#include "customitem.h"
#include "flyer.h"
#include "customevent.h"
#include "dummyobject.h"
#include "mercuryball.h"
#include "customdoor.h"
#include "customlift.h"
#include "demoactor.h"
#include "hudmanager.h"
#include "targetassault.h"
#include "targetcsbase.h"
#include "targetcs.h"
#include "targetcscask.h"

#include "weaponM134.h"
#include "weaponAK74.h"
#include "weaponLR300.h"
#include "weaponFN2000.h"
#include "weaponHPSA.h"
#include "weaponPM.h"
#include "weaponFORT.h"
#include "weaponBINOCULARS.h"
#include "weaponShotgun.h"
#include "weaponsvd.h"
#include "weaponsvu.h"
#include "Spectator.h"

#include "customzone.h"

extern "C" {
	DLL_API DLL_Pure*	__cdecl xrFactory_Create		(CLASS_ID cls)
	{
		DLL_Pure*	P	= 0;
		switch (cls)
		{
		case CLSID_LEVEL:				P = new CLevel();				break;
		case CLSID_HUDMANAGER:			P = new CHUDManager();			break;
		case CLSID_OBJECT_ACTOR:		P = new CActor();				break;
		case CLSID_TARGET:				P = new CCustomTarget();		break;
		case CLSID_SPECTATOR:			P = new CSpectator();			break;

		case CLSID_AI_RAT:				P = new CAI_Rat();				break;
		case CLSID_AI_SOLDIER:			P =	new CAI_Soldier();			break;
		case CLSID_AI_ZOMBIE:			P = new CAI_Zombie();			break;
		case CLSID_AI_CROW:				P = new CAI_Crow();				break;
		case CLSID_CAR_NIVA:			P = new CCar();					break;

		// Artifacts
		case CLSID_AF_MERCURY_BALL:		P = new CMercuryBall();			break;

		// Targets
		case CLSID_TARGET_ASSAULT:		P = new CTargetAssault();		break;
		case CLSID_TARGET_CS_BASE:		P = new CTargetCSBase();		break;
		case CLSID_TARGET_CS_CASK:		P = new CTargetCSCask();		break;
		case CLSID_TARGET_CS:			P = new CTargetCS();			break;

		case CLSID_OBJECT_DUMMY:		P = new CDummyObject();			break;

		case CLSID_OBJECT_W_FN2000:		P = new CWeaponFN2000();		break;
		case CLSID_OBJECT_W_AK74:		P = new CWeaponAK74();			break;
		case CLSID_OBJECT_W_LR300:		P = new CWeaponLR300();			break;
		case CLSID_OBJECT_W_HPSA:		P = new CWeaponHPSA	();			break;
		case CLSID_OBJECT_W_PM:			P = new CWeaponPM	();			break;
		case CLSID_OBJECT_W_FORT:		P = new CWeaponFORT	();			break;
		case CLSID_OBJECT_W_BINOCULAR:	P = new CWeaponBinoculars();	break;
		case CLSID_OBJECT_W_SHOTGUN:	P = new CWeaponShotgun();		break;
		case CLSID_OBJECT_W_SVD:		P = new CWeaponSVD();			break;
		case CLSID_OBJECT_W_SVU:		P = new CWeaponSVU();			break;

		// Zones
		case CLSID_ZONE:				P = new CCustomZone();			break;
		}
		R_ASSERT		(P);
		P->SUB_CLS_ID	= cls;
		return P;
	}
	DLL_API void		__cdecl	xrFactory_Destroy		(DLL_Pure* O)
	{
		xr_delete(O);
	}
};

/*
//		case CLSID_OBJECT_DUMMY:		P = new CDummyObject();			break;
//		case CLSID_EVENT:				P = new CCustomEvent();			break;
//		case CLSID_OBJECT_W_M134:		P = new CWeaponM134();			break;
//		case CLSID_OBJECT_W_RAIL:	P = new	CWeaponRail();		break;
//		case CLSID_OBJECT_ITEM_STD:	P = new CCustomItem();		break;
//		case CLSID_OBJECT_FLYER:	P = new CFlyer();			break;
//		case CLSID_OBJECT_DOOR:		P = new CCustomDoor();		break;
//		case CLSID_OBJECT_LIFT:		P = new CCustomLift();		break;
//		case CLSID_OBJECT_DACTOR:	P = new CDemoActor();		break;
//		case CLSID_AI_HUMAN:		P = new CAI_Human();		break;
*/
// -- cool defines --------------------------------------------------
#define _MANOWAR_SUPER_PROGRAMMER_
#define _TOP_SECRET_
#define _COOL_
#define _AMD_RULEZ_FOREVER_
#define _MS_BUGS_OFF_
// -- includes ------------------------------------------------------
#include "stdafx.h"
#include "..\skeletonX.h"
#include "..\bodyinstance.h"
// -- offsets -------------------------------------------------------
#define M11 0
#define M12 4
#define M13 8
#define M14 12
#define M21 16
#define M22 20
#define M23 24
#define M24 28
#define M31 32
#define M32 36
#define M33 40
#define M34 44
#define M41 48
#define M42 52
#define M43 56
#define M44 60
// ------------------------------------------------------------------
void __stdcall xrSkin1W_SSE(	vertRender*		D,
								vertBoned1W*	S,
								u32				vCount,
								CBoneInstance*	Bones) 
{__asm
{
// ------------------------------------------------------------------
	mov			ecx,vCount		; ecx = vCount
// ------------------------------------------------------------------
	; esi		= source vector [S]
	; edi		= result vector [D]
	; eax		= transform matrix [m]
// ------------------------------------------------------------------
	mov			edi,D			; edi = D
	mov			esi,S			; esi = S
// ------------------------------------------------------------------
	ALIGN		16				;
	new_dot:					; _new cycle iteration
// ------------------------------------------------------------------
// data prefetching
// ------------------------------------------------------------------
	prefetcht0	[esi+(TYPE vertBoned1W)*3]S.P		;
//	prefetchnta	[edi+(TYPE vertRender)*6]D.P		;
// ------------------------------------------------------------------
// calculating transformation matrix address
// ------------------------------------------------------------------
	mov			eax,TYPE CBoneInstance			;
	mul			DWORD PTR [esi]S.matrix			;
	add			eax,Bones.mTransform			;
// ------------------------------------------------------------------
// loading 128-bit ALIGNED matrix
// ------------------------------------------------------------------
	movaps		xmm4,XMMWORD PTR [eax+M11]		; xmm4 = _14 | _13 | _12 | _11
	movaps		xmm5,XMMWORD PTR [eax+M21]		; xmm5 = _24 | _23 | _22 | _21
	movaps		xmm6,XMMWORD PTR [eax+M31]		; xmm6 = _34 | _33 | _32 | _31
	movaps		xmm7,XMMWORD PTR [eax+M41]		; xmm6 = _44 | _43 | _42 | _41
// ------------------------------------------------------------------
// => xmm4-xmm7		: Transform matrix
// ------------------------------------------------------------------
// transform tiny
// ------------------------------------------------------------------
	movups		xmm0,XMMWORD PTR [esi]S.P		; xmm0 = ?.? | v.z | v.y | v.x
	
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x

	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x
	shufps		xmm2,xmm2,01010101b				; xmm2 = v.y | v.y | v.y | v.y
	
	mulps		xmm1,xmm4						; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm2,xmm5						; xmm2 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm1,xmm2						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x
	shufps		xmm2,xmm2,10101010b				; xmm2 = v.z | v.z | v.z | v.z
	mulps		xmm2,xmm6						; xmm2 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm2,xmm7						; xmm2 = v.z*_34+_44 | v.z*_33+_43 | 
												;		 v.z*_32+_42 | v.z*_31+_41
	addps		xmm2,xmm1						; xmm2 = v.x*_14+v.y*_24+v.z*_34+_44 | v.x*_13+v.y*_23+v.z*_33+_43 |
												;		 v.x*_12+v.y*_22+v.z*_32+_42 | v.x*_11+v.y*_21+v.z*_31+_41
// ------------------------------------------------------------------
// => xmm2			: transform tiny result		: ?.? | td.z | td.y | td.x
// ------------------------------------------------------------------
// transform dir
// ------------------------------------------------------------------	
	movups		xmm0,XMMWORD PTR [esi]S.N		; xmm0 = ?.? | v.z | v.y | v.x
	
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x
	movaps		xmm3,xmm0						; xmm3 = ?.? | v.z | v.y | v.x

	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x
	shufps		xmm3,xmm3,01010101b				; xmm3 = v.y | v.y | v.y | v.y

	mulps		xmm1,xmm4						; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm3,xmm5						; xmm3 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm1,xmm3						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	movaps		xmm3,xmm0						; xmm3 = ?.? | v.z | v.y | v.x
	shufps		xmm3,xmm3,10101010b				; xmm3 = v.z | v.z | v.z | v.z
	mulps		xmm3,xmm6						; xmm3 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm3,xmm1						; xmm3 = v.x*_14+v.y*_24+v.z*_34 | v.x*_13+v.y*_23+v.z*_33 |
												;		 v.x*_12+v.y*_22+v.z*_32 | v.x*_11+v.y*_21+v.z*_31
// ------------------------------------------------------------------
// => xmm3			: transform dir result		: ?.? | dd.z | dd.y | dd.x
// ------------------------------------------------------------------	
// ------------------------------------------------------------------	
//	movlps		xmm0,[esi]S.u					;
	movntps		XMMWORD PTR [edi]D.P,xmm2		;
	movntps		XMMWORD PTR [edi+4]D.N,xmm3		;
//	movlps		[edi]D.u,xmm0					;
// ------------------------------------------------------------------	
	add			esi,TYPE vertBoned1W	;
	add			edi,TYPE vertRender		;
// ------------------------------------------------------------------
// ------------------------------------------------------------------	
	dec			ecx						; ecx = ecx - 1
	jnz			new_dot					; ecx==0 ? goto new_dot : exit
// ------------------------------------------------------------------
// ------------------------------------------------------------------
}
}
