#pragma once

#include "game_sv_teamdeathmatch.h"

class	game_sv_ArtefactHunt			: public game_sv_TeamDeathmatch
{
private:
	typedef game_sv_TeamDeathmatch inherited;
protected:
	u16			m_dwArtefactID;
	
	BOOL		m_delayedRoundEnd;
	u32			m_roundEndDelay;

	s32			m_dwArtefactRespawnDelta;
	s32			m_dwArtefactSpawnTime;

	u16			m_ArtefactsSpawnedTotal;

	s32			m_dwArtefactStayTime;
	s32			m_dwArtefactRemoveTime;

	xr_vector<RPoint>				Artefact_rpoints;
    	
public:
	virtual		void				Create					(LPSTR &options);

	// Events	
	virtual		void				OnRoundStart			();							// ����� ������
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);

	virtual		void				OnTimelimitExceed		();

	virtual		u32					RP_2_Use				(CSE_Abstract* E);

	virtual		void				LoadTeams				();

	virtual		void				OnPlayerBuyFinished		(u32 id_who, NET_Packet& P);

	virtual		void				OnObjectEnterTeamBase	(u16 id, u16 id_zone);
	virtual		void				OnObjectLeaveTeamBase	(u16 id, u16 id_zone);
	
	bool							IsBuyableItem			(CSE_Abstract* pItem);
	BOOL							CheckUpgrades			(CSE_Abstract* pItem, u8 IItem);	
	void							RemoveItemFromActor		(CSE_Abstract* pItem);
	void							OnArtefactOnBase		(u32 id_who);

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);
	virtual		void				OnCreate				(u16 id_who);


	virtual		void				Update					();

				void				OnDelayedRoundEnd		(LPCSTR /**reason/**/);
				void				SpawnArtefact			();
				void				RemoveArtefact			();
				void				Assign_Artefact_RPoint	(CSE_Abstract* E);

	virtual		void				net_Export_State		(NET_Packet& P, u32 id_to);				// full state
				bool				ArtefactSpawn_Allowed	();
};
