////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "movement_manager_space.h"
#include "game_location_selector.h"
#include "level_location_selector.h"
#include "game_path_manager.h"
#include "level_path_manager.h"
#include "detail_path_manager.h"
#include "patrol_path_manager.h"
#include "xrmessages.h"
#include "ai_object_location.h"
#include "custommonster.h"
#include "location_manager.h"
#include "level_path_builder.h"
#include "detail_path_builder.h"

using namespace MovementManager;

const float verify_distance = 15.f;

CMovementManager::CMovementManager	(CCustomMonster *object)
{
	VERIFY						(object);
	m_object					= object;

	m_base_game_selector		= xr_new<CBaseParameters>		 ();
	m_base_level_selector		= xr_new<CBaseParameters>		 ();
	
	m_restricted_object			= xr_new<CRestrictedObject>		 (m_object);
	m_selector_manager			= xr_new<CSelectorManager>		 (m_object);
	m_location_manager			= xr_new<CLocationManager>		 (m_object);

	m_game_location_selector	= xr_new<CGameLocationSelector	>(m_restricted_object,m_selector_manager,m_location_manager);
	m_game_path_manager			= xr_new<CGamePathManager		>(m_restricted_object);
	m_level_location_selector	= xr_new<CLevelLocationSelector	>(m_restricted_object,m_selector_manager);
	m_level_path_manager		= xr_new<CLevelPathManager		>(m_restricted_object);
	m_detail_path_manager		= xr_new<CDetailPathManager		>(m_restricted_object);
	m_patrol_path_manager		= xr_new<CPatrolPathManager		>(m_restricted_object,m_object);

	m_level_path_builder		= xr_new<CLevelPathBuilder>(this);
	m_detail_path_builder		= xr_new<CDetailPathBuilder>(this);

	extrapolate_path			(false);

	m_wait_for_distributed_computation = false;
}

CMovementManager::~CMovementManager	()
{
	xr_delete					(m_base_game_selector		);
	xr_delete					(m_base_level_selector		);
	
	xr_delete					(m_restricted_object		);
	xr_delete					(m_selector_manager			);
	xr_delete					(m_location_manager			);
	
	xr_delete					(m_game_location_selector	);
	xr_delete					(m_game_path_manager		);
	xr_delete					(m_level_location_selector	);
	xr_delete					(m_level_path_manager		);
	xr_delete					(m_detail_path_manager		);
	xr_delete					(m_patrol_path_manager		);

	xr_delete					(m_level_path_builder		);
	xr_delete					(m_detail_path_builder		);
}

void CMovementManager::Load			(LPCSTR section)
{
	locations().Load			(section);
}

void CMovementManager::reinit		()
{
	m_time_work						= 300*CPU::cycles_per_microsec;
	m_speed							= 0.f;
	m_path_type						= ePathTypeNoPath;
	m_path_state					= ePathStateDummy;
	m_path_actuality				= true;
	m_speed							= 0.f;
	m_selector_path_usage			= false;//true;
	m_old_desirable_speed			= 0.f;
	m_refresh_rate					= 0;
	m_last_update					= Device.dwTimeGlobal;
	m_build_at_once					= false;

	enable_movement					(true);
	game_selector().reinit			(&ai().game_graph());
	level_selector().reinit			(ai().get_level_graph());
	detail().reinit					();
	game_path().reinit				(&ai().game_graph());
	level_path().reinit				(ai().get_level_graph());
	patrol().reinit					();
	selectors().reinit				();

	game_selector().set_dest_path	(game_path().m_path);
	level_selector().set_dest_path	(level_path().m_path);
	level_selector().set_dest_vertex(level_path().m_dest_vertex_id);
}

void CMovementManager::reload		(LPCSTR section)
{
	locations().reload				(section);
}

BOOL CMovementManager::net_Spawn	(CSE_Abstract* data)
{
	return							(restrictions().net_Spawn(data));
}

void CMovementManager::net_Destroy	()
{
	level_path_builder().remove		();
	detail_path_builder().remove	();
}

CMovementManager::EPathType CMovementManager::path_type() const
{
	VERIFY					(m_path_type != MovementManager::ePathTypeDummy);
	return					(m_path_type);
}

void CMovementManager::set_game_dest_vertex		(const GameGraph::_GRAPH_ID &game_vertex_id)
{
	game_path().set_dest_vertex(game_vertex_id);
	m_path_actuality		= m_path_actuality && game_path().actual();
}

GameGraph::_GRAPH_ID CMovementManager::game_dest_vertex_id() const
{
	return					(GameGraph::_GRAPH_ID(game_path().dest_vertex_id()));
}

void CMovementManager::set_level_dest_vertex	(const u32 level_vertex_id)
{
	VERIFY2					(restrictions().accessible(level_vertex_id),*object().cName());
	level_path().set_dest_vertex(level_vertex_id);
	m_path_actuality		= m_path_actuality && level_path().actual();
}

u32	 CMovementManager::level_dest_vertex_id		() const
{
	return					(level_path().dest_vertex_id());
}

const xr_vector<DetailPathManager::STravelPathPoint>	&CMovementManager::path	() const
{
	return					(detail().path());
}

void CMovementManager::update_path				()
{
	if	(
			!enabled() || 
			wait_for_distributed_computation() || 
			((m_last_update > Device.dwTimeGlobal) && !path_completed())
		)
		return;

	m_last_update			= Device.dwTimeGlobal + m_refresh_rate;

	time_start				();
	
	if (!game_path().evaluator())
		game_path().set_evaluator	(base_game_params());

	if (!level_path().evaluator())
		level_path().set_evaluator	(base_level_params());

#pragma todo("Optimize this in case of slowdown or not intended behaviour")
	if (!restrictions().actual(true)) {
		m_path_actuality	= false;
	}

	restrictions().actual	(true);

	if (!actual()) {

		game_path().make_inactual();
		level_path().make_inactual();
		patrol().make_inactual();
		switch (m_path_type) {
			case ePathTypeGamePath : {
				m_path_state	= ePathStateSelectGameVertex;
				break;
			}
			case ePathTypeLevelPath : {
				m_path_state	= ePathStateSelectLevelVertex;
				if (!restrictions().accessible(level_path().dest_vertex_id())) {
					Fvector							temp;
					level_path().set_dest_vertex	(restrictions().accessible_nearest(detail().dest_position(),temp));
					detail().set_dest_position		(temp);
				}
				break;
			}
			case ePathTypePatrolPath : {
				m_path_state	= ePathStateSelectPatrolPoint;
				break;
			}
			case ePathTypeNoPath : {
				m_path_state	= ePathStateDummy;
				break;
			}
			default :			NODEFAULT;
		}
		m_path_actuality	= true;
	}

	switch (m_path_type) {
		case ePathTypeGamePath : {
			process_game_path	();
			break;
		}
		case ePathTypeLevelPath : {
			process_level_path	();
			break;
		}
		case ePathTypePatrolPath : {
			process_patrol_path();
			break;
		}
		case ePathTypeNoPath : {
			break;
		}
		default :				NODEFAULT;
	}

	if (restrictions().accessible(object().Position()))
		verify_detail_path		();

	m_build_at_once				= false;
}

bool CMovementManager::actual_all				() const
{
	if (!m_path_actuality)
		return			(false);
	switch (m_path_type) {
		case ePathTypeGamePath : 
			return			(
				game_path().actual() && 
				level_path().actual() &&
				detail().actual()
			);
		case ePathTypeLevelPath :
			return			(
				level_path().actual() &&
				detail().actual()
			);
		case ePathTypePatrolPath :
			return			(
				patrol().actual() && 
				level_path().actual() &&
				detail().actual()
			);
		case ePathTypeNoPath :
			return			(
				detail().actual()
			);
		default : NODEFAULT;
	}
#ifdef DEBUG
	return					(true);
#endif
}

void CMovementManager::teleport					(u32 game_vertex_id)
{
	NET_Packet				net_packet;
	GameGraph::_GRAPH_ID	_game_vertex_id = (GameGraph::_GRAPH_ID)game_vertex_id;
	u32						_level_vertex_id = ai().game_graph().vertex(_game_vertex_id)->level_vertex_id();
	Fvector					position = ai().game_graph().vertex(_game_vertex_id)->level_point();
	object().u_EventGen		(net_packet,GE_TELEPORT_OBJECT,object().ID());
	net_packet.w			(&_game_vertex_id,sizeof(_game_vertex_id));
	net_packet.w			(&_level_vertex_id,sizeof(_level_vertex_id));
	net_packet.w_vec3		(position);
	Level().Send			(net_packet,net_flags(TRUE,TRUE));
}

void CMovementManager::clear_path				()
{
	m_detail_path_manager->m_path.clear();
}

bool CMovementManager::distance_to_destination_greater	(const float &distance_to_check) const
{
	if (path().size() < 2)
		return				(true);

	if (path_completed())
		return				(true);

	float					accumulator = 0.f;
	for (u32 i = detail().curr_travel_point_index(), n=detail().path().size()-1; i<n; ++i) {
		accumulator			+= detail().path()[i].position.distance_to(detail().path()[i+1].position);
		if (accumulator >= distance_to_check)
			return			(true);
	}

	return					(false);
}

void CMovementManager::verify_detail_path		()
{
	if (detail().path().empty() || detail().completed(detail().dest_position()))
		return;

	if (restrictions().out_restrictions().size())
		return;

	float distance = 0.f;
	for (u32 i=detail().curr_travel_point_index() + 1, n=detail().path().size(); i<n; ++i) {
		if (!restrictions().accessible(detail().path()[i].position,EPS_L)) {
			m_path_actuality	= false;
			return;
		}
		distance	+= detail().path()[i].position.distance_to(detail().path()[i-1].position);
		if (distance >= verify_distance)
			break;
	}
}

void CMovementManager::on_restrictions_change	()
{
	m_path_actuality			= false;
}