#include "stdafx.h"
#include "Level.h"
#include "Level_Bullet_Manager.h"
#include "xrserver.h"
#include "xrmessages.h"
#include "game_cl_base.h"

void CLevel::net_Stop		()
{
	Msg							("- Disconnect");
	IGame_Level::net_Stop		();
	IPureClient::Disconnect		();

	BulletManager().Clear		();

	
	if (Server)
	{
		Server->Disconnect		();
		xr_delete				(Server);
	}
}

void CLevel::ClientSend	()
{
	if (!net_HasBandwidth()) return;
	NET_Packet				P;
	u32						start	= 0;
	//----------- for E3 -----------------------------
	if (OnClient()) 
	{
		if (!CurrentEntity()) return;
		CObject* pObj = CurrentEntity();
		if (pObj->getDestroy()) return;

		P.w_begin		(M_CL_UPDATE);

		u32			position;
		P.w_u16			(u16(pObj->ID())	);
		P.w_chunk_open8	(position);
		pObj->net_Export			(P);
		P.w_chunk_close8	(position);
		
		if (P.B.count>5)				Send	(P, net_flags(FALSE));
		return;
	};
	//-------------------------------------------------

	while (1)				{
		P.w_begin						(M_UPDATE);
		start	= Objects.net_Export	(&P, start, 48);
		if (P.B.count>2)				Send	(P, net_flags(FALSE));
		else							break	;
	}
}

void CLevel::Send		(NET_Packet& P, u32 dwFlags, u32 dwTimeout)
{
	// optimize the case when server located in our memory
	if (Server && game_configured && OnServer())	Server->OnMessage	(P,Game().local_svdpnid	);
	else											IPureClient::Send	(P,dwFlags,dwTimeout	);
}

void CLevel::net_Update	()
{
	// If we have enought bandwidth - replicate client data on to server
	Device.Statistic.netClient.Begin	();
	ClientSend							();
	Device.Statistic.netClient.End		();

	// If server - perform server-update
	if (Server && OnServer())	{
		Device.Statistic.netServer.Begin();
		Server->Update					();
		Device.Statistic.netServer.End	();
	}
}

struct _NetworkProcessor	: public pureFrame
{
	virtual void OnFrame	( )
	{
		if (g_pGameLevel)	g_pGameLevel->net_Update();
	}
}	NET_processor;

pureFrame*	g_pNetProcessor	= &NET_processor;
