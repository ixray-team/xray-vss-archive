////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_torso_animation.cpp
//	Created 	: 19.11.2004
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Torso animations for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "weapon.h"
#include "missile.h"
#include "clsid_game.h"
#include "object_handler_space.h"
#include "object_handler_planner.h"
#include "stalker_movement_manager.h"
#include "entitycondition.h"
#include "stalker_animation_data.h"
#include "stalker_animation_manager_impl.h"

void CStalkerAnimationManager::torso_play_callback	(CBlend *blend)
{
	VERIFY							(blend->CallbackParam);
	CAI_Stalker						*stalker = (CAI_Stalker*)blend->CallbackParam;
	CStalkerAnimationManager		&animation = stalker->animation();

	CPropertyStorage				*setup_storage = animation.setup_storage();
	if (setup_storage) {
		setup_storage->set_property	(animation.property_id(),animation.property_value());
		return;
	}

	animation.torso().make_inactual	();
}

MotionID CStalkerAnimationManager::no_object_animation(const EBodyState &body_state) const
{
	const CAI_Stalker				&stalker = object();
	const CStalkerMovementManager	&movement = stalker.movement();
	const xr_vector<CAniVector>		&animation = m_data_storage->m_part_animations.A[body_state].m_torso.A[0].A;

	if (eMentalStateFree == movement.mental_state()) {
		VERIFY3						(
			eBodyStateStand == movement.body_state(),
			"Cannot run FREE animations, when body state is not stand!",
			*stalker.cName()
		);

		if (standing())
			return					(animation[9].A[1]);

		if (stalker.conditions().IsLimping())
            return					(animation[9].A[0]);

		return						(animation[7 + movement.movement_type()].A[1]);
	}
		
	if (standing())
		return						(animation[6].A[0]);

	if (eMovementTypeWalk == movement.movement_type())
		return						(animation[6].A[2]);

	VERIFY							(eMovementTypeRun == movement.movement_type());
	return							(animation[6].A[3]);
}

MotionID CStalkerAnimationManager::unknown_object_animation(u32 slot, const EBodyState &body_state) const
{
	// animation shortcuts
	typedef CStalkerAnimationState	STATE;
	const xr_vector<STATE>			&part_animations = m_data_storage->m_part_animations.A;
	const xr_vector<CAniVector>		&animation = part_animations[body_state].m_torso.A[slot].A;
	const xr_vector<CAniVector>		&animation_stand = part_animations[eBodyStateStand].m_torso.A[slot].A;
	
	// stalker shortcuts
	const CAI_Stalker				&stalker = object();
	const CStalkerMovementManager	&movement = stalker.movement();
	u32								id = stalker.CObjectHandler::planner().current_action_state_id();

	switch (id) {
		case ObjectHandlerSpace::eWorldOperatorFire1:
		case ObjectHandlerSpace::eWorldOperatorFire2:
		case ObjectHandlerSpace::eWorldOperatorAim1:
		case ObjectHandlerSpace::eWorldOperatorAim2:
		case ObjectHandlerSpace::eWorldOperatorAimingReady1:
		case ObjectHandlerSpace::eWorldOperatorAimingReady2:
		case ObjectHandlerSpace::eWorldOperatorQueueWait1:
		case ObjectHandlerSpace::eWorldOperatorQueueWait2:	{
			if (eBodyStateStandDamaged == body_state) {
				if	((movement.body_state() == eBodyStateStand) && !standing())
					return			(animation[9].A[1]);

				return				(animation[9].A[0]);
			}

			if (standing())
				return				(animation[6].A[0]);

			if (eMovementTypeWalk == movement.movement_type())
				return				(animation[6].A[2]);

			VERIFY					(eMovementTypeRun == movement.movement_type());
			return					(animation[6].A[3]);
		}

		// we do use only STAND body state here, since we do not have enough animations and will
		case ObjectHandlerSpace::eWorldOperatorStrapping		:
			return					(animation_stand[11].A[0]);
		case ObjectHandlerSpace::eWorldOperatorUnstrapping		:
			return					(animation_stand[12].A[0]);
		case ObjectHandlerSpace::eWorldOperatorStrapping2Idle	:
			return					(animation_stand[11].A[1]);
		case ObjectHandlerSpace::eWorldOperatorUnstrapping2Idle	:
			return					(animation_stand[12].A[1]);
	}

	if (eMentalStateFree == movement.mental_state()) {
		VERIFY3								(
			eBodyStateStand == movement.body_state(),
			"Cannot run FREE animation when body state is not stand!",
			*object().cName()
		);

		if (standing() || stalker.conditions().IsLimping())
			return					(animation[9].A[1]);

		return						(animation[7 + movement.movement_type()].A[1]);
	}

	if (standing())  {
		if (stalker.conditions().IsLimping())
			return					(animation[9].A[0]);

		return						(animation[6].A[0]);
	}

	if (eMovementTypeWalk == movement.movement_type()) {
//		if (eBodyStateStand == movement.body_state())
//			return					(animation[7].A[0]);
//
//		return						(animation[6].A[0]);
		return						(animation[6].A[2]);
	}

	VERIFY							(eMovementTypeRun == movement.movement_type());

	if (eBodyStateStand == movement.body_state()) {
//		if (stalker.conditions().IsLimping())
//			return					(animation[7].A[0]);
//
//		return						(animation[8].A[0]);
		return						(animation[6].A[3]);
	}

	return							(animation[6].A[3]);
}

MotionID CStalkerAnimationManager::weapon_animation	(u32 slot, const EBodyState &body_state) const
{
	const xr_vector<CAniVector>		&animation = m_data_storage->m_part_animations.A[body_state].m_torso.A[slot].A;

	switch (m_weapon->STATE) {
		case CWeapon::eReload : {
			switch (m_weapon->GetReloadState()){
				case CWeapon::eSubstateReloadBegin:
					return			(animation[4].A[0]);
				case CWeapon::eSubstateReloadInProcess:
					return			(animation[4].A[1]);
				case CWeapon::eSubstateReloadEnd:
					return			(animation[4].A[2]);

				default:			NODEFAULT;
			}
#ifdef DEBUG
			return					(animation[4].A[0]);
#endif
		}
		case CWeapon::eShowing :
			return					(animation[0].A[0]);
		case CWeapon::eHiding :
			return					(animation[3].A[0]);
		case CWeapon::eFire:
		case CWeapon::eFire2 : {
			CAI_Stalker				&stalker = object();
			CStalkerMovementManager	&movement = stalker.movement();
			if (standing()) {
				if ((eBodyStateStand == movement.body_state()))
					return			(animation[1].A[1]);
			
				return				(animation[1].A[0]);
			}

			if (eMovementTypeWalk == movement.movement_type())
				return				(animation[1].A[2]);

			VERIFY					(eMovementTypeRun == movement.movement_type());
			return					(animation[1].A[3]);
		}
	}

	return							(unknown_object_animation(slot,body_state));
}

MotionID CStalkerAnimationManager::missile_animation	(u32 slot, const EBodyState &body_state) const
{
	VERIFY							(m_missile);

	if (body_state == eBodyStateCrouch)
		slot						= 0;

	const xr_vector<CAniVector>		&animation = m_data_storage->m_part_animations.A[body_state].m_torso.A[slot].A;

	switch (m_missile->State()) {
		case MS_SHOWING	 :
			return					(animation[0].A[0]);
		case MS_HIDING	 :
			return					(animation[3].A[0]);
		case MS_THREATEN :
			return					(animation[1].A[0]);
		case MS_READY	 :
			return					(animation[1].A[1]);
		case MS_THROW	 :
			return					(animation[1].A[2]);
		case MS_END		 :
			return					(animation[1].A[2]);
		case MS_PLAYING	 :
			return					(animation[1].A[2]);
		case MS_IDLE	 :
		case MS_HIDDEN	 :
		case MS_EMPTY	 :
		default			 :
			return					(animation[6].A[0]);
	}
}

MotionID CStalkerAnimationManager::assign_torso_animation	()
{
	EBodyState						body_state	= this->body_state();

	if (!object().inventory().ActiveItem())
		return						(no_object_animation(body_state));

	fill_object_info				();

	if (m_weapon) {
		if (!strapped())
			return					(weapon_animation(object_slot(),body_state));

		return						(no_object_animation(body_state));
	}

	if (m_missile)
		return						(missile_animation(object_slot(),body_state));

	return							(unknown_object_animation(object_slot(),body_state));
}
