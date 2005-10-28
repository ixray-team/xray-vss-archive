#pragma once
#include "control_combase.h"

#include "anim_triple.h"
#include "control_jump.h"
#include "control_rotation_jump.h"


class CAnimationSequencer;
class CControlRotationJump;
class CControlRunAttack;
class CControlThreaten;

class CControlManagerCustom : public CControl_ComBase {
	typedef					CControl_ComBase	inherited;

	CAnimationSequencer		*m_sequencer;
	CAnimationTriple		*m_triple_anim;

	CControlRotationJump	*m_rotation_jump;
	CControlJump			*m_jump;
	CControlRunAttack		*m_run_attack;
	CControlThreaten		*m_threaten;

	DEFINE_VECTOR			(SControlRotationJumpData, ROT_JUMP_DATA_VEC, ROT_JUMP_DATA_VEC_IT);
	ROT_JUMP_DATA_VEC		m_rot_jump_data;

public:
					CControlManagerCustom	();
					~CControlManagerCustom	();

	virtual void	reinit					();
	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);
	virtual void	on_start_control		(ControlCom::EControlType type);
	virtual void	on_stop_control			(ControlCom::EControlType type);
	virtual void	update_frame			();
	virtual void	update_schedule			();

			void	add_ability				(ControlCom::EControlType);

	//-------------------------------------------------------------------------------
	// Sequencer
	void		seq_init				();
	void		seq_add					(MotionID motion);
	void		seq_switch				();					// ������� � ��������� ���������, ���� �������� �� ������� - ���������
	void		seq_run					(MotionID motion);

	//-------------------------------------------------------------------------------
	// Triple Animation
	void		ta_activate				(const SAnimationTripleData &data);
	void		ta_pointbreak			();
	void		ta_fill_data			(SAnimationTripleData &data, LPCSTR s1, LPCSTR s2, LPCSTR s3, bool execute_once, bool skip_prep, u32 capture_type = ControlCom::eCaptureDir | ControlCom::eCapturePath | ControlCom::eCaptureMovement);
	bool		ta_is_active			();
	bool		ta_is_active			(const SAnimationTripleData &data);
	void		ta_deactivate			();
	
	//-------------------------------------------------------------------------------
	// Jump
	void		jump					(CObject *obj, const SControlJumpData &ta);
	void		jump					(const SControlJumpData &ta);
	void		jump					(const Fvector &position);
	void		load_jump_data			(LPCSTR s1, LPCSTR s2, LPCSTR s3, LPCSTR s4, u32 vel_mask_prepare, u32 vel_mask_ground, u32 flags);
	
	bool		script_jump				(CObject *obj);

	//-------------------------------------------------------------------------------
	// Rotation Jump
	void		add_rotation_jump_data	(LPCSTR left1,LPCSTR left2,LPCSTR right1,LPCSTR right2, float angle);

private:

	void		check_attack_jump		();
	void		check_jump_over_physics	();
	void		check_rotation_jump		();
	void		check_run_attack		();
	void		check_threaten			();
};

