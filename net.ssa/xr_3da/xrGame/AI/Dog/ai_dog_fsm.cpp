////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_dog_fsm.cpp
//	Created 	: 25.04.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Dog"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_dog.h"
#include "..\\ai_monsters_misc.h"

#undef	WRITE_TO_LOG
#define WRITE_TO_LOG(s) m_bStopThinking = true;
/**
#define WRITE_TO_LOG(s) {\
	Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\
	if (!feel_visible.size())\
		Msg("* No objects in frustum",feel_visible.size());\
	else {\
		Msg("* Objects in frustum (%d) :",feel_visible.size());\
		for (int i=0; i<(int)feel_visible.size(); i++)\
			Msg("*   %s",feel_visible[i].O->cName());\
		feel_vision_get(m_tpaVisibleObjects);\
		if (!m_tpaVisibleObjects.size())\
			Msg("* No visible objects");\
		else {\
			Msg("* Visible objects (%d) :",m_tpaVisibleObjects.size());\
			for (int i=0; i<(int)m_tpaVisibleObjects.size(); i++)\
				Msg("*   %s (distance %7.2fm)",m_tpaVisibleObjects[i]->cName(),vPosition.distance_to(m_tpaVisibleObjects[i]->Position()));\
		}\
	}\
	m_bStopThinking = true;\
}
/**/

#ifndef DEBUG
	#undef	WRITE_TO_LOG
	#define WRITE_TO_LOG(s) m_bStopThinking = true;
#endif

void CAI_Dog::Think()
{
	vfUpdateMorale();
	m_bStopThinking = false;
	do {
		m_ePreviousState = m_eCurrentState;
		switch(m_eCurrentState) {
			case aiDogDie : {
				Death();
				break;
			}
			case aiDogFreeHuntingActive : {
				FreeHuntingActive();
				break;
			}
			case aiDogFreeHuntingPassive : {
				FreeHuntingPassive();
				break;
			}
			case aiDogAttackFire : {
				AttackFire();
				break;
			}
			case aiDogAttackRun : {
				AttackRun();
				break;
			}
			case aiDogTurn : {
				Turn();
				break;
			}
			case aiDogUnderFire : {
				UnderFire();
				break;
			}
			case aiDogRetreat : {
				Retreat();
				break;
			}
			case aiDogPursuit : {
				Pursuit();
				break;
			}
			case aiDogFreeRecoil : {
				FreeRecoil();
				break;
			}
			case aiDogReturnHome : {
				ReturnHome();
				break;   
			}
			case aiDogEatCorpse : {
				EatCorpse();
				break;
			}
		}
		m_bStateChanged = m_ePreviousState != m_eCurrentState;
	}
	while (!m_bStopThinking);
//	if (m_fSpeed > EPS_L) {
//		AI_Path.TravelPath.resize(3);
//		AI_Path.TravelPath[0].floating = AI_Path.TravelPath[1].floating = AI_Path.TravelPath[2].floating = FALSE;
//		AI_Path.TravelPath[0].P = m_tOldPosition;
//		AI_Path.TravelPath[1].P = vPosition;
//		Fvector tTemp;
//		tTemp.setHP(r_torso_current.yaw,r_torso_current.pitch);
//		tTemp.normalize_safe();
//		tTemp.mul(10.f);
//		AI_Path.TravelPath[2].P.add(vPosition,tTemp);
//		AI_Path.TravelStart = 0;
//		vPosition = m_tOldPosition;
//	}
//	else {
//		AI_Path.TravelPath.clear();
//		AI_Path.TravelStart = 0;
//	}
}

void CAI_Dog::Death()
{
	//WRITE_TO_LOG("Dying...");
	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	m_bStopThinking = true;

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
	AI_Path.TravelPath.clear();

	if (m_fFood <= 0) {
		if (m_dwLastRangeSearch <= m_dwDeathTime)
			m_dwLastRangeSearch = Level().timeServer();
		setVisible(false);
		if (Level().timeServer() - m_dwLastRangeSearch > 10000) {
			setEnabled(false);
//			NET_Packet			P;
//			u_EventGen			(P,GE_DESTROY,ID());
//			u_EventSend			(P);
		}
	}
}

void CAI_Dog::Turn()
{
	WRITE_TO_LOG("Turning...");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	mRotate.setHPB(m_tHPB.x = -r_torso_current.yaw,m_tHPB.y,m_tHPB.z);
	UpdateTransform();

	CHECK_IF_GO_TO_PREV_STATE(getAI().bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw, PI_DIV_6))
	
	INIT_SQUAD_AND_LEADER
	
	r_torso_speed = PI_MUL_2;

	m_fSpeed = 0;
}

void CAI_Dog::FreeHuntingActive()
{
	WRITE_TO_LOG("Free hunting active");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	SelectEnemy(m_Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(((m_Enemy.Enemy) && ((m_Enemy.Enemy->Position().distance_to(m_tSafeSpawnPosition) < m_fMaxPursuitRadius) || (vPosition.distance_to(m_tSafeSpawnPosition) > m_fMaxHomeRadius))),aiDogAttackRun);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(m_fMorale < m_fMoraleNormalValue,aiDogUnderFire);
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((!m_tLastSound.tpEntity) || (m_tLastSound.tpEntity->g_Team() != g_Team())) && (!m_Enemy.Enemy)) {
		if (m_tLastSound.tpEntity)
			m_tSavedEnemy = m_tLastSound.tpEntity;
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_dwLostEnemyTime = Level().timeServer();
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiDogFreeRecoil);
	}
    
	SelectCorp(m_Enemy);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(m_Enemy.Enemy,aiDogEatCorpse);
	
	m_tSpawnPosition.set(m_tSafeSpawnPosition);
	m_fGoalChangeDelta		= m_fSafeGoalChangeDelta;
	m_tVarGoal.set			(m_tGoalVariation);
	m_fASpeed				= m_fAngleSpeed;
	
	if (bfCheckIfGoalChanged()) {
		if (m_bStateChanged || (vPosition.distance_to(m_tSpawnPosition) > m_fStableDistance) || (::Random.randF(0,1) > m_fChangeActiveStateProbability))
			if (vPosition.distance_to(m_tSafeSpawnPosition) > m_fMaxHomeRadius)
				m_fSpeed = m_fSafeSpeed = m_fMaxSpeed;
			else
				vfChooseNewSpeed();
		else {
			vfRemoveActiveMember();
		}
	}

	if (m_bStateChanged)
		vfChooseNewSpeed();

	vfUpdateTime(m_fTimeUpdateDelta);

	if (bfComputeNewPosition(false))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiDogTurn);
	
	if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
		u32 dwCurTime = Level().timeServer();
		if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
			m_tpSoundBeingPlayed = 0;
			m_dwLastVoiceTalk = dwCurTime;
		}
		if ((dwCurTime - m_dwLastSoundRefresh > m_fVoiceRefreshDoge) && ((dwCurTime - m_dwLastVoiceTalk > m_fMaxVoiceIinterval) || ((dwCurTime - m_dwLastVoiceTalk > m_fMinVoiceIinterval) && (::Random.randF(0,1) > (dwCurTime - m_dwLastVoiceTalk - m_fMinVoiceIinterval)/(m_fMaxVoiceIinterval - m_fMinVoiceIinterval))))) {
			m_dwLastSoundRefresh = dwCurTime;
			// Play voice-sound
			m_tpSoundBeingPlayed = &(m_tpaSoundVoice[Random.randI(SND_VOICE_COUNT)]);
			
			if (!m_tpSoundBeingPlayed->feedback)
				::Sound->play_at_pos(*m_tpSoundBeingPlayed,this,eye_matrix.c);
			else
				m_tpSoundBeingPlayed->set_position(eye_matrix.c);
		}
	}
	else
		if (m_tpSoundBeingPlayed && m_tpSoundBeingPlayed->feedback)
			m_tpSoundBeingPlayed->feedback->set_position(eye_matrix.c);
}

void CAI_Dog::FreeHuntingPassive()
{
	WRITE_TO_LOG("Free hunting passive");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy) {
		m_fGoalChangeTime = 0;
		vfAddActiveMember(true);
		m_bStopThinking = false;
		return;
	}

	if (m_fMorale < m_fMoraleNormalValue) {
		vfAddActiveMember(true);
		m_bStopThinking = false;
		return;
	}
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((!m_tLastSound.tpEntity) || (m_tLastSound.tpEntity->g_Team() != g_Team()))) {
		vfAddActiveMember(true);
		m_bStopThinking = false;
		return;
	}
	
	m_fSpeed = 0.f;

	UpdateTransform();

	vfAddStandingMember();
	vfAddActiveMember();
}

void CAI_Dog::UnderFire()
{
	WRITE_TO_LOG("UnderFire");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy) {
		GO_TO_NEW_STATE_THIS_UPDATE(aiDogAttackRun);
	}
	else {
		if (m_tLastSound.dwTime >= m_dwLastUpdateTime) {
			if (m_tLastSound.tpEntity && (m_tLastSound.tpEntity->g_Team() != g_Team()) && (!bfCheckIfSoundFrightful())) {
				m_tSavedEnemy = m_tLastSound.tpEntity;
				m_dwLostEnemyTime = Level().timeServer();
				SWITCH_TO_NEW_STATE(aiDogAttackRun);
			}
			m_dwLastRangeSearch = Level().timeServer();
			Fvector tTemp;
			tTemp.setHP(r_torso_current.yaw,r_torso_current.pitch);
			tTemp.normalize_safe();
			tTemp.mul(m_fUnderFireDistance);
			m_tSpawnPosition.add(vPosition,tTemp);
		}
		if (m_fMorale >= m_fMoraleNormalValue - EPS_L) {
			GO_TO_PREV_STATE;
		}
	}

	vfUpdateTime(m_fTimeUpdateDelta);

	if ((Level().timeServer() - m_dwLastRangeSearch > TIME_TO_GO) || !m_dwLastRangeSearch)
		m_tGoalDir = m_tSpawnPosition;
	
	if (bfComputeNewPosition(true,true))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiDogTurn);
}

void CAI_Dog::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	SelectEnemy(m_Enemy);
	
	//EDogStates eState = EDogStates(dwfChooseAction(m_dwActionRefreshDoge,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),m_eCurrentState,m_eCurrentState,aiDogRetreat));
	//if (eState != m_eCurrentState)
	//	GO_TO_NEW_STATE_THIS_UPDATE(eState);

	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	if (!(m_Enemy.Enemy) && m_tSavedEnemy && (Level().timeServer() - m_dwLostEnemyTime < m_dwLostMemoryTime))
		m_Enemy.Enemy = m_tSavedEnemy;

	CHECK_IF_GO_TO_PREV_STATE(!(m_Enemy.Enemy));
		
	CHECK_IF_GO_TO_NEW_STATE((m_Enemy.Enemy->Position().distance_to(vPosition) > m_fAttackDistance),aiDogAttackRun)

	Fvector tTemp;
	tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);
	
	CHECK_IF_GO_TO_NEW_STATE(!getAI().bfTooSmallAngle(r_torso_current.yaw,sTemp.yaw,m_fAttackAngle),aiDogAttackRun)
		
	Fvector tDistance;
	tDistance.sub(Position(),m_Enemy.Enemy->Position());

	AI_Path.TravelPath.clear();
	
	vfSaveEnemy();

	vfAimAtEnemy();

	vfSetFire(true,Level().get_group(g_Team(),g_Squad(),g_Group()));
	
	vfSetMovementType(m_cBodyState,0);
}

void CAI_Dog::AttackRun()
{
	WRITE_TO_LOG("Attack enemy");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	SelectEnemy(m_Enemy);

	EDogStates eState = EDogStates(dwfChooseAction(m_dwActionRefreshDoge,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),m_eCurrentState,m_eCurrentState,m_eCurrentState,aiDogRetreat,aiDogRetreat));
	if (eState != m_eCurrentState) {
		GO_TO_NEW_STATE_THIS_UPDATE(eState);
	}

	if (!(m_Enemy.Enemy) && m_tSavedEnemy && (Level().timeServer() - m_dwLostEnemyTime < m_dwLostMemoryTime))
		m_Enemy.Enemy = m_tSavedEnemy;

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(m_Enemy.Enemy && (m_tSafeSpawnPosition.distance_to(m_Enemy.Enemy->Position()) > m_fMaxPursuitRadius),aiDogReturnHome);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!m_Enemy.Enemy);

	vfSaveEnemy();

	Fvector tDistance;
	tDistance.sub(Position(),m_Enemy.Enemy->Position());
	
	Fvector tTemp;
	tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE((m_Enemy.Enemy->Position().distance_to(vPosition) <= m_fAttackDistance) && (getAI().bfTooSmallAngle(r_torso_target.yaw, sTemp.yaw,m_fAttackAngle)),aiDogAttackFire)

	if ((Level().timeServer() - m_dwLastRangeSearch > TIME_TO_GO) || !m_dwLastRangeSearch)
		m_tGoalDir.set(m_Enemy.Enemy->Position());
	
	vfUpdateTime(m_fTimeUpdateDelta);

	m_fSpeed = m_fAttackSpeed;

	if (bfComputeNewPosition(true,true))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiDogTurn);
}

void CAI_Dog::Retreat()
{
	WRITE_TO_LOG("Retreat");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	SelectEnemy(m_Enemy);

	if (m_Enemy.Enemy && m_Enemy.Enemy->g_Alive()) {
		vfSaveEnemy();
		EDogStates eState = EDogStates(dwfChooseAction(m_dwActionRefreshDoge,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),aiDogAttackRun,aiDogAttackRun,aiDogAttackRun,aiDogRetreat,aiDogRetreat));
		if (eState != m_eCurrentState)
			GO_TO_NEW_STATE_THIS_UPDATE(eState);
		m_dwLostEnemyTime = Level().timeServer();
		
		Fvector tTemp;
		tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
		vfNormalizeSafe(tTemp);
		SRotation sTemp;
		mk_rotation(tTemp,sTemp);

		CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE((m_Enemy.Enemy->Position().distance_to(vPosition) <= m_fAttackDistance) && (!getAI().bfTooSmallAngle(r_torso_target.yaw, sTemp.yaw,m_fAttackAngle)),aiDogTurn);

		CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE((m_Enemy.Enemy->Position().distance_to(vPosition) <= m_fAttackDistance) && (getAI().bfTooSmallAngle(r_torso_target.yaw, sTemp.yaw,m_fAttackAngle)),aiDogAttackFire)

		tTemp.sub(vPosition,m_Enemy.Enemy->Position());
		tTemp.normalize_safe();
		tTemp.mul(m_fRetreatDistance);
		m_tSpawnPosition.add(vPosition,tTemp);
	}
	else
		if ((m_Enemy.Enemy && (!m_Enemy.Enemy->g_Alive())) || ((Level().timeServer() - m_dwLostEnemyTime > m_dwRetreatTime) && ((m_tLastSound.dwTime < m_dwLastUpdateTime) || !m_tLastSound.tpEntity || (m_tLastSound.tpEntity->g_Team() == g_Team()) || (!bfCheckIfSoundFrightful())))) {
			GO_TO_PREV_STATE;
		}

	vfUpdateTime(m_fTimeUpdateDelta);
	
	m_fSpeed = m_fAttackSpeed;

	if ((Level().timeServer() - m_dwLastRangeSearch > TIME_TO_GO) || !m_dwLastRangeSearch)
		m_tGoalDir = m_tSpawnPosition;

	if (bfComputeNewPosition(true,true))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiDogTurn);
}

void CAI_Dog::Pursuit()
{
	WRITE_TO_LOG("Pursuit something");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	SelectEnemy(m_Enemy);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(m_Enemy.Enemy,aiDogAttackRun);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE((m_fMorale < m_fMoraleNormalValue),aiDogUnderFire);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(Level().timeServer() - m_dwLostEnemyTime >= m_dwLostMemoryTime);
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_RICOCHET) == SOUND_TYPE_WEAPON_BULLET_RICOCHET)) {
		if (m_tLastSound.tpEntity)
			m_tSavedEnemy = m_tLastSound.tpEntity;
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE_THIS_UPDATE(aiDogFreeRecoil);
	}

	if ((Level().timeServer() - m_dwLastRangeSearch > TIME_TO_GO) || !m_dwLastRangeSearch)
		m_tGoalDir.set(m_tSavedEnemyPosition);
	
	vfUpdateTime(m_fTimeUpdateDelta);

	m_fSpeed = m_fAttackSpeed;

	if (bfComputeNewPosition(true,true))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiDogTurn);
}

void CAI_Dog::FreeRecoil()
{
	WRITE_TO_LOG("Free hunting : Recoil from something");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	SelectEnemy(m_Enemy);
		
	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(m_Enemy.Enemy);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(m_fMorale < m_fMoraleNormalValue);

	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((!m_tLastSound.tpEntity) || (m_tLastSound.tpEntity->g_Team() != g_Team()))) {
		m_dwLostEnemyTime = Level().timeServer();
		Fvector tTemp;
		tTemp.setHP(r_torso_current.yaw,r_torso_current.pitch);
		tTemp.normalize_safe();
		tTemp.mul(m_fUnderFireDistance);
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_tSpawnPosition.add(vPosition,tTemp);
	}
	
	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(Level().timeServer() - m_dwLostEnemyTime >= m_dwLostRecoilTime,aiDogPursuit);
	
	if ((Level().timeServer() - m_dwLastRangeSearch > TIME_TO_GO) || !m_dwLastRangeSearch)
		m_tGoalDir.set(m_tSpawnPosition);
	
	m_fSafeSpeed = m_fSpeed = m_fMaxSpeed;
	
	vfUpdateTime(m_fTimeUpdateDelta);

	if (bfComputeNewPosition(true,true))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiDogTurn);
}

void CAI_Dog::ReturnHome()
{
	WRITE_TO_LOG("Returning home");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy && (m_tSafeSpawnPosition.distance_to(m_Enemy.Enemy->Position()) < m_fMaxPursuitRadius)) {
		SelectEnemy(m_Enemy);
		m_fGoalChangeTime = 0;
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiDogAttackRun)
	}
 
	if (m_Enemy.Enemy) {
		Fvector tTemp;
		tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
		vfNormalizeSafe(tTemp);
		SRotation sTemp;
		mk_rotation(tTemp,sTemp);

		CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE((m_Enemy.Enemy->Position().distance_to(vPosition) <= m_fAttackDistance) && (!getAI().bfTooSmallAngle(r_torso_target.yaw, sTemp.yaw,m_fAttackAngle)),aiDogTurn);

		CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE((m_Enemy.Enemy->Position().distance_to(vPosition) <= m_fAttackDistance) && (getAI().bfTooSmallAngle(r_torso_target.yaw, sTemp.yaw,m_fAttackAngle)),aiDogAttackFire)
	}

	CHECK_IF_GO_TO_PREV_STATE(!m_Enemy.Enemy || !m_Enemy.Enemy->g_Alive() || vPosition.distance_to(m_tSafeSpawnPosition) < m_fMaxHomeRadius);

	m_tSpawnPosition.set	(m_tSafeSpawnPosition);
	m_fGoalChangeDelta		= m_fSafeGoalChangeDelta;
	m_tVarGoal.set			(m_tGoalVariation);
	m_fASpeed				= m_fAngleSpeed;
	m_fSpeed = m_fSafeSpeed = m_fAttackSpeed;

	if ((Level().timeServer() - m_dwLastRangeSearch > TIME_TO_GO) || !m_dwLastRangeSearch)
		m_tGoalDir.set			(m_tSafeSpawnPosition);

	vfUpdateTime(m_fTimeUpdateDelta);

	m_fSpeed = m_fAttackSpeed;

	if (bfComputeNewPosition())
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiDogTurn);
}

void CAI_Dog::EatCorpse()
{
	WRITE_TO_LOG("Eating a corpse");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	SelectEnemy(m_Enemy);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(m_Enemy.Enemy);

	SelectCorp(m_Enemy);

	if (!m_Enemy.Enemy) {
		vfChangeGoal();
		m_fGoalChangeTime = 10.f;
	}

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!m_Enemy.Enemy || (m_fMorale < m_fMoraleNormalValue));

	vfSaveEnemy();
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((!m_tLastSound.tpEntity) || (m_tLastSound.tpEntity->g_Team() != g_Team())) && (!m_Enemy.Enemy)) {
		if (m_tLastSound.tpEntity)
			m_tSavedEnemy = m_tLastSound.tpEntity;
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_dwLostEnemyTime = Level().timeServer();
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiDogFreeRecoil);
	}
	
	if ((Level().timeServer() - m_dwLastRangeSearch > TIME_TO_GO) || !m_dwLastRangeSearch)
		m_tGoalDir.set(m_Enemy.Enemy->Position());
	
	vfUpdateTime(m_fTimeUpdateDelta);

	m_fSpeed = m_fMaxSpeed;

	if (bfComputeNewPosition(true,true))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiDogTurn);

	Fvector tTemp;
	m_Enemy.Enemy->clCenter(tTemp);
	if (tTemp.distance_to(vPosition) <= m_fAttackDistance) {
		m_fSpeed = 0;
		if (Level().timeServer() - m_dwLastRangeSearch > m_dwHitInterval) {
			m_dwLastRangeSearch = Level().timeServer();
			m_Enemy.Enemy->m_fFood -= m_fHitPower/10.f;
		}
		m_bFiring = true;
	}
}