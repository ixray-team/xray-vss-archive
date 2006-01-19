#pragma once
#include "../control_combase.h"
#include "../../../../SkeletonAnimated.h"

class CPsyHitEffectorCam;
class CPsyHitEffectorPP;	

class CControllerPsyHit : public CControl_ComCustom<> {
	typedef CControl_ComCustom<> inherited;

	MotionID			m_stage[4];
	u8					m_current_index;

	CPsyHitEffectorCam	*m_effector_cam;
	CPsyHitEffectorPP	*m_effector_pp;

	enum ESoundState{
		eStart,
		ePull,
		eHit,
		eNone
	} m_sound_state;

public:
	virtual void	load					(LPCSTR section);
	virtual	void	reinit					();
	virtual	void	update_frame			();
	virtual bool	check_start_conditions	();
	virtual void	activate				();
	virtual void	deactivate				();
	
	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);
private:

			void	play_anim				();
			void	death_glide_start			();
			void	death_glide_end			();

			void	set_sound_state			(ESoundState state);
};

