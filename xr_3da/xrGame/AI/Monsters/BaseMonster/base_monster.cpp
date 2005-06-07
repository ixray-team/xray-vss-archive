#include "stdafx.h"
#include "base_monster.h"
#include "../../../PhysicsShell.h"
#include "../../../hit.h"
#include "../../../PHDestroyable.h"
#include "../../../CharacterPhysicsSupport.h"
#include "../../../game_level_cross_table.h"
#include "../../../game_graph.h"
#include "../../../phmovementcontrol.h"
#include "../ai_monster_squad_manager.h"
#include "../../../xrserver_objects_alife_monsters.h"
#include "../ai_monster_jump.h"
#include "../corpse_cover.h"
#include "../../../cover_evaluators.h"
#include "../../../seniority_hierarchy_holder.h"
#include "../../../team_hierarchy_holder.h"
#include "../../../squad_hierarchy_holder.h"
#include "../../../group_hierarchy_holder.h"
#include "../../../phdestroyable.h"
#include "../../../../skeletoncustom.h"
#include "../critical_action_info.h"
#include "../../../detail_path_manager.h"
#include "../../../hudmanager.h"
#include "../../../memory_manager.h"
#include "../../../visual_memory_manager.h"
#include "../monster_velocity_space.h"
#include "../../../entitycondition.h"
#include "../../../sound_player.h"
#include "../../../level.h"
#include "../../../ui/UIMainIngameWnd.h"
#include "../state_manager.h"
#include "../controlled_entity.h"

CBaseMonster::CBaseMonster()
{
	m_pPhysics_support=xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etBitting,this);
	
	m_pPhysics_support				->in_Init();

	// Components external init 
	
	m_control_manager				= xr_new<CControl_Manager>(this);


	EnemyMemory.init_external		(this, 20000);
	SoundMemory.init_external		(this, 20000);
	CorpseMemory.init_external		(this, 20000);
	HitMemory.init_external			(this, 50000);

	EnemyMan.init_external			(this);
	CorpseMan.init_external			(this);

	// ������������� ���������� ��������	

	StateMan						= 0;

	CriticalActionInfo				= xr_new<CCriticalActionInfo>();

	MeleeChecker.init_external		(this);
	Morale.init_external			(this);

	m_jumping						= 0;
	m_controlled					= 0;

	m_anim_base						= xr_new<CControlAnimationBase>		();
	m_move_base						= xr_new<CControlMovementBase>		();
	m_path_base						= xr_new<CControlPathBuilderBase>	();
	m_dir_base						= xr_new<CControlDirectionBase>		();

	control().add					(m_anim_base, ControlCom::eControlAnimationBase);
	control().add					(m_move_base, ControlCom::eControlMovementBase);
	control().add					(m_path_base, ControlCom::eControlPathBase);
	control().add					(m_dir_base,  ControlCom::eControlDirBase);

	control().set_base_controller	(m_anim_base, ControlCom::eControlAnimation);
	control().set_base_controller	(m_move_base, ControlCom::eControlMovement);
	control().set_base_controller	(m_dir_base,  ControlCom::eControlDir);
}


CBaseMonster::~CBaseMonster()
{
	xr_delete(m_pPhysics_support);
	xr_delete(m_corpse_cover_evaluator);
	xr_delete(m_enemy_cover_evaluator);
	xr_delete(m_cover_evaluator_close_point);
	
	xr_delete(CriticalActionInfo);

	xr_delete(m_control_manager);

	xr_delete(m_anim_base);
	xr_delete(m_move_base);
	xr_delete(m_path_base);
	xr_delete(m_dir_base);
}

void CBaseMonster::UpdateCL()
{
	inherited::UpdateCL();
	
	if (g_Alive()) {
		
		// �������� ��������� �������� (�����)
		AA_CheckHit							();

		// �������� �������� �������� ��������
		//movement().update_velocity			();

		// �������� ����������� �������, ��� ������� �������� ��������
		//dir().update_frame					();

		CStepManager::update				();
	}

	control().update_frame();

	m_pPhysics_support->in_UpdateCL();
}

void CBaseMonster::shedule_Update(u32 dt)
{
	// finalize initialization of the object on the first update
	if (!m_first_update_initialized) {
		on_first_update();
		m_first_update_initialized	= true;
	}

	inherited::shedule_Update	(dt);
	control().update_schedule	();

	Morale.update_schedule		(dt);
	
	m_pPhysics_support->in_shedule_Update(dt);

#ifdef DEBUG	
	show_debug_info();
#endif
}


//////////////////////////////////////////////////////////////////////
// Other functions
//////////////////////////////////////////////////////////////////////


void CBaseMonster::Die(CObject* who)
{
	inherited::Die(who);

	if (StateMan) StateMan->critical_finalize();
	
	if (is_special_killer(who))
		sound().play			(MonsterSpace::eMonsterSoundDieInAnomaly);
	else
		sound().play			(MonsterSpace::eMonsterSoundDie);

	monster_squad().remove_member((u8)g_Team(),(u8)g_Squad(), this);
	
	if (m_controlled)			m_controlled->on_die();
}


void CBaseMonster::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
{
	if (ignore_collision_hit && (hit_type == ALife::eHitTypeStrike)) return;
	
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

void CBaseMonster::PHHit(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /*=ALife::eHitTypeWound*/)
{
	m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

CPHDestroyable*	CBaseMonster::	ph_destroyable	()
{
	return smart_cast<CPHDestroyable*>(character_physics_support());
}

bool CBaseMonster::useful(const CItemManager *manager, const CGameObject *object) const
{
	const CEntityAlive *pCorpse = smart_cast<const CEntityAlive *>(object); 
	if (!pCorpse) return false;
	
	if (!pCorpse->g_Alive()) return true;
	return false;
}

float CBaseMonster::evaluate(const CItemManager *manager, const CGameObject *object) const
{
	return (0.f);
}

//////////////////////////////////////////////////////////////////////////

void CBaseMonster::ChangeTeam(int team, int squad, int group)
{
	if ((team == g_Team()) && (squad == g_Squad()) && (group == g_Group())) return;
	VERIFY2(g_Alive(), "Try to change team of a dead object");

	// remove from current team
	monster_squad().remove_member	((u8)g_Team(),(u8)g_Squad(), this);
	inherited::ChangeTeam			(team,squad,group);
	monster_squad().register_member	((u8)g_Team(),(u8)g_Squad(), this);
}


void CBaseMonster::SetTurnAnimation(bool turn_left)
{
	(turn_left) ? anim().SetCurAnim(eAnimStandTurnLeft) : anim().SetCurAnim(eAnimStandTurnRight);
}

bool CBaseMonster::IsVisibleObject(const CGameObject *object)
{
	return memory().visual().visible_now(object);
}

void CBaseMonster::set_state_sound(u32 type, bool once)
{
	if (once) {
	
		sound().play(type);
	
	} else {

		u32 delay = 0;
		switch (type) {
		case MonsterSpace::eMonsterSoundIdle : 
		
			delay = db().m_dwIdleSndDelay;
			break;
		case MonsterSpace::eMonsterSoundEat:
			delay = db().m_dwEatSndDelay;
			break;
		case MonsterSpace::eMonsterSoundAttack:
		case MonsterSpace::eMonsterSoundPanic:
			delay = db().m_dwAttackSndDelay;
			break;
		}

		sound().play(type, 0, 0, delay);
	}
}

BOOL CBaseMonster::feel_touch_on_contact	(CObject *O)
{
	return		(inherited::feel_touch_on_contact(O));
}

void CBaseMonster::TranslateActionToPathParams()
{
	bool bEnablePath = true;
	u32 vel_mask = 0;
	u32 des_mask = 0;

	switch (anim().m_tAction) {
	case ACT_STAND_IDLE: 
	case ACT_SIT_IDLE:	 
	case ACT_LIE_IDLE:
	case ACT_EAT:
	case ACT_SLEEP:
	case ACT_REST:
	case ACT_LOOK_AROUND:
	case ACT_ATTACK:
		bEnablePath = false;
		break;

	case ACT_WALK_FWD:
		if (m_bDamaged) {
			vel_mask = MonsterMovement::eVelocityParamsWalkDamaged;
			des_mask = MonsterMovement::eVelocityParameterWalkDamaged;
		} else {
			vel_mask = MonsterMovement::eVelocityParamsWalk;
			des_mask = MonsterMovement::eVelocityParameterWalkNormal;
		}
		break;
	case ACT_WALK_BKWD:
		break;
	case ACT_RUN:
		if (m_bDamaged) {
			vel_mask = MonsterMovement::eVelocityParamsRunDamaged;
			des_mask = MonsterMovement::eVelocityParameterRunDamaged;
		} else {
			vel_mask = MonsterMovement::eVelocityParamsRun;
			des_mask = MonsterMovement::eVelocityParameterRunNormal;
		}
		break;
	case ACT_DRAG:
		vel_mask = MonsterMovement::eVelocityParamsDrag;
		des_mask = MonsterMovement::eVelocityParameterDrag;

		anim().SetSpecParams(ASP_MOVE_BKWD);

		break;
	case ACT_STEAL:
		vel_mask = MonsterMovement::eVelocityParamsSteal;
		des_mask = MonsterMovement::eVelocityParameterSteal;
		break;
	}

	if (state_invisible) {
		vel_mask = MonsterMovement::eVelocityParamsInvisible;
		des_mask = MonsterMovement::eVelocityParameterInvisible;
	}

	if (m_force_real_speed) vel_mask = des_mask;

	if (bEnablePath) {
		path().set_velocity_mask	(vel_mask);
		path().set_desirable_mask	(des_mask);
		path().enable_path			();
	} else {
		path().disable_path			();
	}
}

u32 CBaseMonster::get_attack_rebuild_time()
{
	float dist = EnemyMan.get_enemy()->Position().distance_to(Position());
	return (100 + u32(50.f * dist));
}

void CBaseMonster::on_kill_enemy(const CEntity *obj)
{
	const CEntityAlive *entity	= smart_cast<const CEntityAlive *>(obj);
	
	// �������� � ������ ������	
	CorpseMemory.add_corpse		(entity);
	
	// ������� ��� ���������� � �����
	HitMemory.remove_hit_info	(entity);
}


void CBaseMonster::on_first_update()
{
	// HUD ��� ��������, ���������� �������������� ������
	HUD().GetUI()->UIMainIngameWnd->AddMonsterClawsEffect ("monster", "controller\\controller_blood_01");
}

CMovementManager *CBaseMonster::create_movement_manager	()
{
	m_movement_manager = xr_new<CControlPathBuilder>(this);

	control().add					(m_movement_manager, ControlCom::eControlPath);
	control().install_path_manager	(m_movement_manager);
	control().set_base_controller	(m_path_base, ControlCom::eControlPath);

	return			(m_movement_manager);
}

DLL_Pure *CBaseMonster::_construct	()
{
	inherited::_construct		();
	CStepManager::_construct	();
	return						(this);
}

void CBaseMonster::net_Relcase(CObject *O)
{
	inherited::net_Relcase(O);

	// TODO: do not clear, remove only object O
	if (g_Alive()) {
		EnemyMemory.clear	();
		SoundMemory.clear	();
		CorpseMemory.clear	();
		HitMemory.clear		();

		EnemyMan.reinit		();
		CorpseMan.reinit	();

		UpdateMemory		();
	}
}
	