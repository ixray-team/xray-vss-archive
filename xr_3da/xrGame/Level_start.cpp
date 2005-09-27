#include "stdafx.h"
#include "level.h"
//#include "LevelFogOfWar.h"
#include "Level_Bullet_Manager.h"
#include "xrserver.h"
#include "game_cl_base.h"
#include "xrmessages.h"
#include "xrGameSpyServer.h"
#include "../x_ray.h"
#include "../device.h"

BOOL CLevel::net_Start	( LPCSTR op_server, LPCSTR op_client )
{
	net_start_result_total				= TRUE;

	pApp->LoadBegin				();

	//make Client Name if options doesn't have it
	if (!strstr(op_client,"/name="))
	{
		string512 tmp;
		strcpy(tmp, op_client);
		strcat(tmp, "/name=");
		strcat(tmp, Core.CompName);
		m_caClientOptions			= tmp;
	} else {
		m_caClientOptions			= op_client;
	};
	m_caServerOptions			    = op_server;

	g_loading_events.push_back	(LOADING_EVENT(this,&CLevel::net_start1));
	g_loading_events.push_back	(LOADING_EVENT(this,&CLevel::net_start2));
	g_loading_events.push_back	(LOADING_EVENT(this,&CLevel::net_start3));
	g_loading_events.push_back	(LOADING_EVENT(this,&CLevel::net_start4));
	g_loading_events.push_back	(LOADING_EVENT(this,&CLevel::net_start5));
	g_loading_events.push_back	(LOADING_EVENT(this,&CLevel::net_start6));
	
	return net_start_result_total;
/*
	// Start client and server if need it
	if (op_server)
	{
		pApp->LoadTitle			("SERVER: Starting...");

		// Connect
		Server					= xr_new<xrGameSpyServer>();

		if (!strstr(*m_caServerOptions,"/alife")) {
			string64			l_name = "";
			const char* SOpts = *m_caServerOptions;
			strncpy(l_name, *m_caServerOptions, strchr(SOpts, '/') - SOpts);
			// Activate level
			if (strchr(l_name,'/'))
				*strchr(l_name,'/')	= 0;

			m_name				= l_name;

			int					id = pApp->Level_ID(l_name);

			if (id<0) {
				pApp->LoadEnd	();
				Log				("Can't find level: ",l_name);
				return			FALSE;
			}
			pApp->Level_Set		(id);
		}
		Server->Connect			(m_caServerOptions);	
		Server->SLS_Default		();
		m_name					= Server->level_name(m_caServerOptions);
	}
	//=============================================================================
	//add server port if don't have one in options
	if (!strstr(m_caClientOptions.c_str(), "port=") && Server)
	{
		string64	PortStr;
		sprintf(PortStr, "/port=%d", Server->GetPort());

		string4096	tmp;
		strcpy(tmp, m_caClientOptions.c_str());
		strcat(tmp, PortStr);
		
		m_caClientOptions = tmp;
	}
	//add password string to client, if don't have one
	if (strstr(m_caServerOptions.c_str(), "psw=") && !strstr(m_caClientOptions.c_str(), "psw="))
	{
		string64	PasswordStr = "";
		char* PSW = strstr(m_caServerOptions.c_str(), "psw=") + 4;
		if (strchr(PSW, '/')) 
			strncpy(PasswordStr, PSW, strchr(PSW, '/') - PSW);
		else
			strcpy(PasswordStr, PSW);

		string4096	tmp;
		sprintf(tmp, "%s/psw=%s", m_caClientOptions.c_str(), PasswordStr);
		m_caClientOptions = tmp;
	};
	//=============================================================================
	// Start client
	net_start_result_total						= net_Start_client(*m_caClientOptions);
	// Send Ready message to server
	if (net_start_result_total)
	{
		NET_Packet		NP;
		NP.w_begin		(M_CLIENTREADY);
		Send(NP,net_flags(TRUE,TRUE));

		if (OnClient() && Server)
		{
			Server->SLS_Clear();
		};
	};


	//init bullet manager
	BulletManager().Clear		();
	BulletManager().Load		();

	pApp->LoadEnd				();

	return						net_start_result_total;
*/
}

bool CLevel::net_start1				()
{
	// Start client and server if need it
	if (m_caServerOptions.size())
	{
		pApp->LoadTitle			("SERVER: Starting...");

		// Connect
		Server					= xr_new<xrGameSpyServer>();

		if (!strstr(*m_caServerOptions,"/alife")) {
			string64			l_name = "";
			const char* SOpts = *m_caServerOptions;
			strncpy(l_name, *m_caServerOptions, strchr(SOpts, '/') - SOpts);
			// Activate level
			if (strchr(l_name,'/'))
				*strchr(l_name,'/')	= 0;

			m_name					= l_name;

			int						id = pApp->Level_ID(l_name);

			if (id<0) {
				pApp->LoadEnd				();
				Log							("Can't find level: ",l_name);
				net_start_result_total		= FALSE;
				return true;
			}
			pApp->Level_Set			(id);
		}
	}
	return true;
}

bool CLevel::net_start2				()
{
	if (net_start_result_total && m_caServerOptions.size())
	{
		Server->Connect			(m_caServerOptions);	
		Server->SLS_Default		();
		m_name					= Server->level_name(m_caServerOptions);
	}
	return true;
}

bool CLevel::net_start3				()
{
	if(!net_start_result_total) return true;
	//add server port if don't have one in options
	if (!strstr(m_caClientOptions.c_str(), "port=") && Server)
	{
		string64	PortStr;
		sprintf(PortStr, "/port=%d", Server->GetPort());

		string4096	tmp;
		strcpy(tmp, m_caClientOptions.c_str());
		strcat(tmp, PortStr);
		
		m_caClientOptions = tmp;
	}
	//add password string to client, if don't have one
	if (strstr(m_caServerOptions.c_str(), "psw=") && !strstr(m_caClientOptions.c_str(), "psw="))
	{
		string64	PasswordStr = "";
		char* PSW = strstr(m_caServerOptions.c_str(), "psw=") + 4;
		if (strchr(PSW, '/')) 
			strncpy(PasswordStr, PSW, strchr(PSW, '/') - PSW);
		else
			strcpy(PasswordStr, PSW);

		string4096	tmp;
		sprintf(tmp, "%s/psw=%s", m_caClientOptions.c_str(), PasswordStr);
		m_caClientOptions = tmp;
	};
	return true;
}

bool CLevel::net_start4				()
{
	if(!net_start_result_total) return true;

	g_loading_events.pop_front();

	g_loading_events.push_front	(LOADING_EVENT(this,&CLevel::net_start_client6));
	g_loading_events.push_front	(LOADING_EVENT(this,&CLevel::net_start_client5));
	g_loading_events.push_front	(LOADING_EVENT(this,&CLevel::net_start_client4));
	g_loading_events.push_front	(LOADING_EVENT(this,&CLevel::net_start_client3));
	g_loading_events.push_front	(LOADING_EVENT(this,&CLevel::net_start_client2));
	g_loading_events.push_front	(LOADING_EVENT(this,&CLevel::net_start_client1));

	return false;
}

bool CLevel::net_start5				()
{
	if (net_start_result_total)
	{
		NET_Packet		NP;
		NP.w_begin		(M_CLIENTREADY);
		Send			(NP,net_flags(TRUE,TRUE));

		if (OnClient() && Server)
		{
			Server->SLS_Clear();
		};
	};
	return true;
}

bool CLevel::net_start6				()
{
	//init bullet manager
	BulletManager().Clear		();
	BulletManager().Load		();

	pApp->LoadEnd				();

	if(net_start_result_total){
		if (strstr(Core.Params,"-$")) {
			string256				buf,cmd,param;
			sscanf					(strstr(Core.Params,"-$")+2,"%[^ ] %[^ ] ",cmd,param);
			strconcat				(buf,cmd," ",param);
			Console->Execute		(buf);
		}
	}else{
		Msg				("! Failed to start client. Check the connection or level existance.");
		DEL_INSTANCE	(g_pGameLevel);
		Console->Execute("main_menu on");
	}
	return true;
}

void CLevel::InitializeClientGame	(NET_Packet& P)
{
	string256 game_type_name;
	P.r_stringZ(game_type_name);
	if(game && !xr_strcmp(game_type_name, game->type_name()) )
		return;
	
	xr_delete(game);
	CLASS_ID clsid			= game_GameState::getCLASS_ID(game_type_name,false);
	game					= smart_cast<game_cl_GameState*> ( NEW_INSTANCE ( clsid ) );
	game->set_type_name(game_type_name);
	game->Init();
}

