////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.cpp
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_biting.h"
#include "ai_biting_space.h"

using namespace AI_Biting;

namespace AI_Biting {
	LPCSTR caStateNames			[] = {
		"stand_",
		"sit_",
		"lie_",
		0
	};

	LPCSTR caGlobalNames		[] = {
		"idle_",
		"idle_ls_",
		"walk_fwd_",
		"walk_bkwd_",
		"walk_ls_",
		"walk_rs_",
		"run_",
		"run_ls_",
		"run_rs_",
		"attack_",
		"attack2_",
		"attack_ls_",
		"eat_",
		"damage_",
		"scared_",
		"die_",		
		"lie_down_",
		"stand_up_",
		"scared_",	   //TODO: change it in check corpse
		0
	};
};

static void __stdcall vfPlayCallBack(CBlend* B)
{
	CAI_Biting *tpBiting = (CAI_Biting*)B->CallbackParam;
	tpBiting->OnAnimationEnd();
}

void CAI_Biting::OnAnimationEnd()
{
	m_tpCurrentGlobalAnimation = 0;

	if (Motion.m_tSeq.Playing) {		// Sequence activated?
		Motion.m_tSeq.Switch();	
		if (Motion.m_tSeq.Finished) {
			Motion.m_tSeq.Init();		// �����
			// ����� ������������ Seq, m_tParams �������� ����������� ������ ��� ��������� ��������
			Motion.m_tParams.ApplyData(this);
			// �������������� ������� ���������
			CurrentState->RestoreState(m_dwCurrentUpdate);
		}
	}
}

void CAI_Biting::SelectAnimation(const Fvector &_view, const Fvector &_move, float speed )
{
//	if (bShowDeath)	{
//		m_tpCurrentGlobalAnimation = m_tAnimations.A[ePostureStand].A[eActionDie].A[::Random.randI((int)m_tAnimations.A[ePostureStand].A[eActionDie].A.size())];
//		PKinematics(Visual())->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);
//		bShowDeath  = false;
//		return;
//	}
//	
//	if (g_Alive())
//		if (!m_tpCurrentGlobalAnimation && m_bActionFinished) {
//
//			int i1,i2;
//			MotionToAnim(m_tAnim,i1,i2);
//
//			m_tpCurrentGlobalAnimation = m_tAnimations.A[i1].A[i2].A[::Random.randI((int)m_tAnimations.A[i1].A[i2].A.size())];
//			PKinematics(Visual())->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);
//		}

}


// ����� �������� � ������������ � ������� ���������� �������
void CAI_Biting::vfSetAnimation(bool bForceChange)
{
//	EPostureAnim PostureAnim_old = m_tPostureAnim;
//	EActionAnim ActionAnim_old = m_tActionAnim;
//
//	// ��� ��������� ����
//	switch (m_tBodyState) {
//		case eBodyStateStand : 	// �����
//			m_tPostureAnim = ePostureStand;
//			break;
//		case eBodyStateSit : 	// �����
//			m_tPostureAnim = ePostureSit;
//			break;	
//		case eBodyStateLie : 	// �����
//			m_tPostureAnim = ePostureLie;
//			break;
//		default : NODEFAULT;
//	}
//
//	if ( m_tMovementType == eMovementTypeStand &&  
//		m_tStateType == eStateTypeNormal &&  
//		m_tMovementDir == eMovementDirectionNone &&
//		m_tActionType == eActionTypeStand)
//
//		//if (fHealth < 30) 
//		m_tActionAnim = eActionIdle;				// �� ����� / ��������
//
//	else if ( m_tMovementType == eMovementTypeStand &&  
//		m_tStateType == eStateTypeNormal &&  
//		m_tMovementDir == eMovementDirectionLeft &&
//		m_tActionType == eActionTypeTurn) 
//
//		m_tActionAnim = eActionIdleTurnLeft;		// ������� ����� �� �����
//
//	else if ( m_tMovementType == eMovementTypeStand &&  
//		m_tStateType == eStateTypeDanger &&  
//		m_tMovementDir == eMovementDirectionLeft &&
//		m_tActionType == eActionTypeTurn) 
//
//		m_tActionAnim = eActionAttackTurnLeft;		// ������� ����� �� ����� �� ����� ������ (�������)
//
//
//	else if (m_tMovementType == eMovementTypeWalk && 
//	//	m_tStateType == eStateTypeNormal &&  
//		m_tMovementDir == eMovementDirectionForward && 
//		m_tActionType == eActionTypeWalk) 
//
//		m_tActionAnim = eActionWalkFwd;				// �������� ������
//
//	else if (m_tMovementType == eMovementTypeWalk && 
//		m_tMovementDir == eMovementDirectionLeft && 
//		m_tActionType == eActionTypeTurn) 
//
//		m_tActionAnim = eActionWalkTurnLeft;		// ������� � ������ �����
//
//	else if (m_tMovementType == eMovementTypeWalk && 
//		m_tMovementDir == eMovementDirectionRight && 
//		m_tActionType == eActionTypeTurn) 
//
//		m_tActionAnim = eActionWalkTurnRight;		// ������� � ������ ������
//
//	else if (m_tMovementType == eMovementTypeRun && 
//		m_tActionType == eActionTypeRun) 
//
//		m_tActionAnim = eActionRun;					// ���
//
//	else if (m_tMovementType == eMovementTypeStand && 
//		m_tActionType == eActionTypeAttack &&
//		m_tStateType == eStateTypeDanger) 
//
//		m_tActionAnim = eActionAttack;				// ����������
//
//	else if (m_tMovementType == eMovementTypeStand && 
//		m_tActionType == eActionTypeAttack &&
//		m_tStateType == eStateTypeNormal) 
//
//		m_tActionAnim = eActionAttack2;				// ���������� �����
//
//	else if (m_tMovementType == eMovementTypeRun &&   
//		m_tMovementDir == eMovementDirectionLeft &&
//		m_tActionType == eActionTypeTurn ) 
//
//		m_tActionAnim = eActionRunTurnLeft;			// ������� �� ���� �����
//
//
//	else if (m_tMovementType == eMovementTypeRun &&   
//		m_tMovementDir == eMovementDirectionRight &&
//		m_tActionType == eActionTypeTurn ) 
//
//		m_tActionAnim = eActionRunTurnRight;			// ������� �� ���� ������
//
//	else if (m_tMovementType == eMovementTypeWalk &&   
//		m_tMovementDir == eMovementDirectionBack &&
//		m_tStateType == eStateTypeDanger &&
//		m_tActionType == eActionTypeWalk ) 
//
//		m_tActionAnim = eActionWalkBkwd;			// �������� �����
//
//	else if (m_tMovementType == eMovementTypeStand &&   
//		m_tMovementDir == eMovementDirectionNone &&
//		m_tStateType == eStateTypePanic &&
//		m_tActionType == eActionTypeStand ) 
//
//		m_tActionAnim = eActionScared;			// ������ ���������
//
//	else if (m_tMovementType == eMovementTypeStand &&   
//		m_tMovementDir == eMovementDirectionNone &&
//		m_tStateType == eStateTypeNormal &&
//		m_tActionType == eActionTypeEat ) 
//
//		m_tActionAnim = eActionEat;			// ����
//	else if (m_tMovementType == eMovementTypeStand &&   
//		m_tMovementDir == eMovementDirectionNone &&
//		m_tStateType == eStateTypeNormal &&
//		m_tActionType == eActionTypeLieDown ) 
//
//		m_tActionAnim = eActionLieDown;			// ����
//	else if (m_tMovementType == eMovementTypeStand &&   
//		m_tMovementDir == eMovementDirectionNone &&
//		m_tStateType == eStateTypeNormal &&
//		m_tActionType == eActionTypeLie ) 
//
//		m_tActionAnim = eActionIdle;			// ������
//
//
////*
//	// ���������� ����������� ����������
//	if (_CAction.Started) {
//		m_tPostureAnim = _CAction.it->Posture;
//		m_tActionAnim = _CAction.it->Action;
//		_CAction.Playing = true;
//		_CAction.Started = false;
//		_CAction.Finished = false;	
//		FORCE_ANIMATION_SELECT();
//	} 
//
//	if (_CAction.Playing) {
//		return;
//	}
////**
//
//
//	// ���������� ����������� ���������
//	if (_CA.Started) {
//		m_tPostureAnim = _CA.Posture;
//		m_tActionAnim = _CA.Action;
//		_CA.Playing = true;
//		_CA.Started = false;
//		_CA.Finished = false;	
//		FORCE_ANIMATION_SELECT();
//	}
//	if (_CA.Finished) {
//		_CA.Playing = false;
//	}
//	if (_CA.Playing) {
//		return;
//	}
//	
//
//	if ( ((PostureAnim_old != m_tPostureAnim) || (ActionAnim_old != m_tActionAnim)) && bForceChange) {
////		if (m_dwAnimLastSetTime + m_dwAnimFrameDelay <= m_dwCurrentUpdate) {
////			m_dwAnimLastSetTime = m_dwCurrentUpdate; 
//	
//		if (m_tActionAnim == eActionAttack) {
//			m_dwAttackMeleeTime = m_dwCurrentUpdate;
//		} else m_dwAttackMeleeTime = 0;
//
//		FORCE_ANIMATION_SELECT();
//	}
}


void CAI_Biting::MotionToAnim(EMotionAnim motion, int &index1, int &index2)
{
	switch(motion) {
		case eMotionStandIdle:		index1 = 0; index2 = 0;		break;
		case eMotionLieIdle:		index1 = 2; index2 = 0;		break;
		case eMotionStandTurnLeft:	index1 = 0; index2 = 1;		break;
		case eMotionWalkFwd:		index1 = 0; index2 = 2;		break;
		case eMotionWalkBkwd:		index1 = 0; index2 = 3; 	break;
		case eMotionWalkTurnLeft:	index1 = 0; index2 = 4; 	break;
		case eMotionWalkTurnRight:	index1 = 0; index2 = 5; 	break;
		case eMotionRun:			index1 = 0; index2 = 6; 	break;
		case eMotionRunTurnLeft:	index1 = 0; index2 = 7; 	break;
		case eMotionRunTurnRight:	index1 = 0; index2 = 8; 	break;
		case eMotionAttack:			index1 = 0; index2 = 9; 	break;
		case eMotionAttackRat:		index1 = 0; index2 = 10;	break;
		case eMotionFastTurnLeft:	index1 = 0; index2 = 11; 	break;
		case eMotionEat:			index1 = 2; index2 = 12; 	break;
		case eMotionDamage:			index1 = 0; index2 = 13; 	break;
		case eMotionScared:			index1 = 0; index2 = 14; 	break;
		case eMotionDie:			index1 = 0; index2 = 15; 	break;
		case eMotionLieDown:		index1 = 0; index2 = 16; 	break;
		case eMotionStandUp:		index1 = 2; index2 = 17; 	break;
		case eMotionCheckCorpse:	index1 = 0; index2 = 18; 	break;
	}
}
