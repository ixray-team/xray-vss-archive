#pragma once

#include "..\\biting\\ai_biting.h"
#include "ai_bloodsucker_state.h"
#include "ai_bloodsucker_bones.h"
#include "ai_bloodsucker_misc.h"


class CAI_Bloodsucker : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
							CAI_Bloodsucker	();
	virtual					~CAI_Bloodsucker();	

	virtual void			Init					();

	virtual void			Think					();
	virtual void			UpdateCL				();
	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual	void			Load					(LPCSTR section);
			void			vfAssignBones			();

	static	void __stdcall	BoneCallback	(CBoneInstance *B);

	virtual	void			MotionToAnim			(EMotionAnim motion, int &index1, int &index2, int &index3);
	virtual	void			LoadAttackAnim			();

			CBoneInstance*	GetBone					(LPCTSTR bone_name);
			CBoneInstance*	GetBone					(int bone_id);

			void			LookDirection			(Fvector to_dir, float bone_turn_speed);
			void			LookPosition			(Fvector to_point, float bone_turn_speed);

	// Flesh-specific FSM
	CBloodsuckerRest		*stateRest;
	CBloodsuckerEat			*stateEat;
	CBloodsuckerAttack		*stateAttack;
	CBitingPanic			*statePanic;
	CBloodsuckerHearDNE		*stateHearDNE;
	CBloodsuckerHearNDE		*stateHearNDE;

	friend	class			CBloodsuckerRest;
	friend	class			CBloodsuckerEat;
	friend	class			CBloodsuckerAttack;
	friend	class			CBitingPanic;
	friend	class			CBloodsuckerHearDNE;
	friend	class			CBloodsuckerHearNDE;
	
	bonesManipulation		Bones;

	CBloodsuckerVisibility	m_tVisibility;

	bool					flagEatNow;						// true - ������ ������ ���
	
	float					m_fInvisibilityDist;			// ��������� �� �����, �� ������� ������ ����� ����������� ���������
	float					m_ftrPowerDown;					// �����������, �� ������� ������ ���� ������� ��� ��������� ����������� 
	float					m_fPowerThreshold;				// ����� ����, ����� �������� ������ ����� ���������

	CAI_NodeEvaluatorTemplate<aiSearchRange | aiEnemyDistance> m_tSelectorHearSnd;
};


