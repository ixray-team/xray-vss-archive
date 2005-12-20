#pragma once

#include "bloodsucker_vampire_execute.h"
#include "../states/state_hide_from_point.h"
#include "bloodsucker_vampire_approach.h"
#include "bloodsucker_vampire_hide.h"
#include "../../../clsid_game.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBloodsuckerVampireAbstract CStateBloodsuckerVampire<_Object>

#define RUN_AWAY_DISTANCE			50.f

TEMPLATE_SPECIALIZATION
CStateBloodsuckerVampireAbstract::CStateBloodsuckerVampire(_Object *obj) : inherited(obj)
{
	add_state	(eStateVampire_ApproachEnemy,	xr_new<CStateBloodsuckerVampireApproach<_Object> >	(obj));
	add_state	(eStateVampire_Execute,			xr_new<CStateBloodsuckerVampireExecute<_Object> >	(obj));
	add_state	(eStateVampire_RunAway,			xr_new<CStateMonsterHideFromPoint<_Object> >		(obj));
	add_state	(eStateVampire_Hide,			xr_new<CStateBloodsuckerVampireHide<_Object> >		(obj));
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::reinit()
{
	inherited::reinit	();
	
	m_time_last_vampire	= 0;
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::initialize()
{
	inherited::initialize						();

	object->CInvisibility::set_manual_switch	();
	object->CInvisibility::manual_activate		();

	enemy	= object->EnemyMan.get_enemy		();

	object->sound().play						(CAI_Bloodsucker::eVampireStartHunt);
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::reselect_state()
{
	u32 state_id = u32(-1);
		
	// check if we can start execute
	if (prev_substate == eStateVampire_ApproachEnemy) {
		if (get_state(eStateVampire_Execute)->check_start_conditions())		state_id = eStateVampire_Execute;
	}

	// check if we executed 
	if (prev_substate == eStateVampire_Execute)
		state_id = eStateVampire_Hide;
	
	// check if reach time in vampire state is out - then hide
	if ((prev_substate == eStateVampire_ApproachEnemy) && (time_state_started + object->m_vampire_reach_time < time())) 
		state_id = eStateVampire_Hide;

	// check if we hiding - then hide again
	if ( prev_substate == eStateVampire_Hide) 
		state_id = eStateVampire_Hide;

	// else just 
	if (state_id == u32(-1)) state_id = eStateVampire_ApproachEnemy;

	select_state(state_id);	
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::check_force_state()
{
	// check if we can start execute
	if (prev_substate == eStateVampire_ApproachEnemy) {
		if ((get_state(eStateVampire_Execute)->check_start_conditions()) ||	
			(time_state_started + object->m_vampire_reach_time < time()))
		 
			current_substate = u32(-1);
	}
}


TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::finalize()
{
	inherited::finalize();

	object->CInvisibility::set_manual_switch	(false);
	m_time_last_vampire							= Device.dwTimeGlobal;
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::critical_finalize()
{
	inherited::critical_finalize();
	
	object->CInvisibility::set_manual_switch	(false);
	m_time_last_vampire							= Device.dwTimeGlobal;
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireAbstract::check_start_conditions()
{
	if (!object->WantVampire()) return false;
	
	// �������� �� ���� �������
	const CEntityAlive *enemy = object->EnemyMan.get_enemy();
	if (enemy->CLS_ID != CLSID_OBJECT_ACTOR)		return false;
	if (!object->EnemyMan.see_enemy_now())			return false;
	if (object->CControlledActor::is_controlling())	return false;

	const CActor *actor = smart_cast<const CActor *>(enemy);
	VERIFY(actor);
	if (actor->input_external_handler_installed()) return false;

	if (m_time_last_vampire + object->m_vampire_min_delay > Device.dwTimeGlobal) return false;

	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireAbstract::check_completion()
{
	// ���� ������
	if ((current_substate == eStateVampire_Hide) && 
		get_state_current()->check_completion())	return true;

	// ���� ���� ���������
	if (enemy != object->EnemyMan.get_enemy())		return true;
	
	// ���� ������ ��� ��������� ������ ��������
	if ((current_substate != eStateVampire_Execute) && 
		object->CControlledActor::is_controlling())	return true;

	return false;
}


TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateVampire_RunAway) {

		SStateHideFromPoint		data;
		data.point				= object->EnemyMan.get_enemy_position();
		data.accelerated		= true;
		data.braking			= false;
		data.accel_type			= eAT_Aggressive;
		data.distance			= RUN_AWAY_DISTANCE;
		data.action.action		= ACT_RUN;
		data.action.sound_type	= MonsterSound::eMonsterSoundAggressive;
		data.action.sound_delay = object->db().m_dwAttackSndDelay;
		data.action.time_out	= 15000;

		state->fill_data_with(&data, sizeof(SStateHideFromPoint));

		return;
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateBloodsuckerVampireAbstract

