////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_inline.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker" (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	BOOL CAI_Stalker::UsedAI_Locations							()
{
	return					(TRUE);
}

IC	CStalkerAnimationManager &CAI_Stalker::animation			() const
{
	VERIFY					(m_animation_manager);
	return					(*m_animation_manager);
}

IC	CStalkerPlanner &CAI_Stalker::brain			() const
{
	VERIFY					(m_brain);
	return					(*m_brain);
}

IC	CSightManager &CAI_Stalker::sight							() const
{
	VERIFY					(m_sight_manager);
	return					(*m_sight_manager);
}

IC	LPCSTR CAI_Stalker::Name									() const
{
	return					(CInventoryOwner::Name());
}

IC	CStalkerMovementManager	&CAI_Stalker::movement				() const
{
	VERIFY					(m_movement_manager);
	return					(*m_movement_manager);
}

IC	bool CAI_Stalker::frame_check								(u32 &frame)
{
	if (Device.dwFrame == frame)
		return				(false);

	frame					= Device.dwFrame;
	return					(true);
}

IC	bool CAI_Stalker::group_behaviour							() const
{
	return					(m_group_behaviour);
}

IC	bool CAI_Stalker::can_kill_member							()
{
	update_can_kill_info	();
	return					(m_can_kill_member);
}

IC	bool CAI_Stalker::can_kill_enemy							()
{
	update_can_kill_info	();
	return					(m_can_kill_enemy);
}

IC	CWeaponShotEffector &CAI_Stalker::weapon_shot_effector		() const
{
	VERIFY					(m_weapon_shot_effector);
	return					(*m_weapon_shot_effector);
}
