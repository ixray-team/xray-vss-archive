#include "stdafx.h"
#include "xrServer.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "xrserver_objects.h"
#include "game_base.h"
#include "level.h"
#include "game_cl_base.h"
#include "ai_space.h"
#include "alife_object_registry.h"

void xrServer::Process_event_destroy	(NET_Packet& P, ClientID sender, u32 time, u16 ID, NET_Packet* pEPack)
{
	u32								MODE = net_flags(TRUE,TRUE);
	// Parse message
	u16								id_dest	= ID;
	CSE_Abstract*					e_dest = game->get_entity_from_eid	(id_dest);	// ��� ������ ���� ���������
	if (!e_dest) {
		Msg							("!SV:ge_destroy: [%d]",id_dest);
		return;
	};

	R_ASSERT						(e_dest);
	xrClientData					*c_dest = e_dest->owner;				// ������, ��� ����
	R_ASSERT						(c_dest);
	xrClientData					*c_from = ID_to_client(sender);	// ������, ��� �������
	R_ASSERT						(c_dest == c_from);							// assure client ownership of event
	u16								parent_id = e_dest->ID_Parent;

	//---------------------------------------------
	NET_Packet	P2, *pEventPack = pEPack;
	P2.w_begin	(M_EVENT_PACK);
	//---------------------------------------------
	// check if we have children 
	if (!e_dest->children.empty())
	{
		if (!pEventPack) pEventPack = &P2;

		while (!e_dest->children.empty())
			Process_event_destroy		(P,sender,time,*e_dest->children.begin(), pEventPack);
	};

	if (0xffff == parent_id && NULL == pEventPack)
	{
		ClientID clientID;clientID.setBroadcast();
		SendBroadcast				(clientID,P,MODE);
	}
	else
	{
		NET_Packet	tmpP;
		if (0xffff != parent_id && Process_event_reject(P,sender,time,parent_id,ID,false))
		{
			game->u_EventGen(tmpP, GE_OWNERSHIP_REJECT, parent_id);
			tmpP.w_u16(id_dest);
		
			if (!pEventPack) pEventPack = &P2;
			
			pEventPack->w_u8(u8(tmpP.B.count));
			pEventPack->w(&tmpP.B.data, tmpP.B.count);
		};
		
 		game->u_EventGen(tmpP, GE_DESTROY, id_dest);
		
		pEventPack->w_u8(u8(tmpP.B.count));
		pEventPack->w(&tmpP.B.data, tmpP.B.count);
	};

	if (NULL == pEPack && NULL != pEventPack)
	{
		ClientID clientID;clientID.setBroadcast();
		SendBroadcast				(clientID, *pEventPack, MODE);
	}

	// Everything OK, so perform entity-destroy
	if (e_dest->m_bALifeControl && ai().get_alife()) {
		game_sv_Single				*_game = smart_cast<game_sv_Single*>(game);
		VERIFY						(_game);
		if (ai().alife().objects().object(id_dest,true))
			_game->alife().release	(e_dest,false);
	}

	if (game)
		game->OnDestroyObject		(e_dest->ID);

	entity_Destroy					(e_dest);
}
