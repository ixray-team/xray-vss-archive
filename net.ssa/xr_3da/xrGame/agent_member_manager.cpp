////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_member_manager.cpp
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent member manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_member_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "object_broker.h"
#include "agent_manager.h"
#include "agent_memory_manager.h"
#include "explosive.h"
#include "sound_player.h"

CAgentMemberManager::~CAgentMemberManager		()
{
	delete_data					(m_members);
}

void CAgentMemberManager::add					(CEntity *member)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(member);
	if (!stalker || !stalker->g_Alive())
		return;

	VERIFY2						(
		sizeof(squad_mask_type)*8 > members().size(),
		make_string(
			"too many stalkers in group ([team:%d][squad:%d][group:%d]!",
			m_members.front()->object().g_Team(),
			m_members.front()->object().g_Squad(),
			m_members.front()->object().g_Group()
		)
	);

	iterator					I = std::find_if(m_members.begin(),m_members.end(), CMemberPredicate(stalker));
	VERIFY						(I == m_members.end());
	m_members.push_back			(xr_new<CMemberOrder>(stalker));
}

void CAgentMemberManager::remove				(CEntity *member)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(member);
	if (!stalker)
		return;

	unregister_in_combat		(stalker);

	squad_mask_type							m = mask(stalker);
	object().memory().update_memory_masks	(m);
	object().memory().update_memory_mask	(m,m_combat_mask);

	iterator					I = std::find_if(m_members.begin(),m_members.end(), CMemberPredicate(stalker));
	VERIFY						(I != m_members.end());
	xr_delete					(*I);
	m_members.erase				(I);
}

void CAgentMemberManager::update				()
{
}

void CAgentMemberManager::remove_links			(CObject *object)
{
	MEMBER_STORAGE::iterator	I = m_members.begin();
	MEMBER_STORAGE::iterator	E = m_members.end();
	for ( ; I != E; ++I) {
		if ((*I)->grenade_reaction().m_grenade) {
			const CGameObject	*explosive  =smart_cast<const CGameObject*>((*I)->grenade_reaction().m_grenade);
			VERIFY				(explosive);
			if (explosive->ID() == object->ID())
				(*I)->grenade_reaction().clear();
		}

		if ((*I)->grenade_reaction().m_game_object && ((*I)->grenade_reaction().m_game_object->ID() == object->ID()))
			(*I)->grenade_reaction().clear();

		if ((*I)->member_death_reaction().m_member && ((*I)->member_death_reaction().m_member->ID() == object->ID()))
			(*I)->member_death_reaction().clear();
	}

}

void CAgentMemberManager::register_in_combat	(const CAI_Stalker *object)
{
	if (!object->group_behaviour())
		return;

	squad_mask_type				m = mask(object);
	m_actuality					= m_actuality && ((m_combat_mask | m) == m_combat_mask);
	m_combat_mask				|= m;
}

void CAgentMemberManager::unregister_in_combat	(const CAI_Stalker *object)
{
	if (!object->group_behaviour()) {
		VERIFY					(!registered_in_combat(object));
		return;
	}

	squad_mask_type				m = mask(object);
	m_actuality					= m_actuality && ((m_combat_mask & (squad_mask_type(-1) ^ m)) == m_combat_mask);
	m_combat_mask				&= squad_mask_type(-1) ^ m;
}

bool CAgentMemberManager::registered_in_combat	(const CAI_Stalker *object) const
{
	return						(!!(m_combat_mask & mask(object)));
}

CAgentMemberManager::MEMBER_STORAGE &CAgentMemberManager::combat_members	()
{
	if (m_actuality)
		return							(m_combat_members);

	m_actuality							= true;

	m_combat_members.clear				();
	MEMBER_STORAGE::iterator			I = members().begin();
	MEMBER_STORAGE::iterator			E = members().end();
	for ( ; I != E; ++I) {
		if (registered_in_combat(&(*I)->object()))
			m_combat_members.push_back	(*I);
	}

	return								(m_combat_members);
}

CAgentMemberManager::squad_mask_type CAgentMemberManager::non_combat_members_mask	() const
{
	squad_mask_type					result = 0;

	MEMBER_STORAGE::const_iterator	I = members().begin();
	MEMBER_STORAGE::const_iterator	E = members().end();
	for ( ; I != E; ++I) {
		if (!registered_in_combat(&(*I)->object()))
			result					|= mask(&(*I)->object());
	}

	return							(result);
}

u32 CAgentMemberManager::in_detour		() const
{
	u32									in_detour = 0;
	MEMBER_STORAGE::const_iterator		I = members().begin();
	MEMBER_STORAGE::const_iterator		E = members().end();
	for ( ; I != E; ++I)
		if ((*I)->detour())
			++in_detour;

	return								(in_detour);
}

bool CAgentMemberManager::can_detour	() const
{
	u32									in_detour_count = in_detour();
	return								(!in_detour_count || (in_detour_count < (members().size()/2)));
}

bool CAgentMemberManager::cover_detouring		() const
{
	MEMBER_STORAGE::const_iterator		I = members().begin();
	MEMBER_STORAGE::const_iterator		E = members().end();
	for ( ; I != E; ++I)
		if ((*I)->detour())
			return						(true);
	return								(false);
}

bool CAgentMemberManager::can_cry_noninfo_phrase() const
{
	MEMBER_STORAGE::const_iterator		I = members().begin();
	MEMBER_STORAGE::const_iterator		E = members().end();
	for ( ; I != E; ++I) {
		if (!registered_in_combat(&(*I)->object()))
			continue;

		if ((*I)->object().sound().active_sound_count(false))
			return						(false);
	}

	return								(true);
}
