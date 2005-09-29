#include "stdafx.h"
#include "../xr_ioconsole.h"
#include "customzone.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "actor.h"
#include "hudmanager.h"
#include "ParticlesObject.h"
#include "xrserver_objects_alife_monsters.h"
#include "../LightAnimLibrary.h"
#include "level.h"
#include "game_cl_base.h"
#include "../igame_persistent.h"
#include "artifact.h"
#include "ai_object_location.h"
#include "../skeletoncustom.h"
#include "zone_effector.h"

//////////////////////////////////////////////////////////////////////////
#define PREFETCHED_ARTEFACTS_NUM 4	//���������� �������������� ������������ ����������
#define WIND_RADIUS (4*Radius())	//���������� �� ������, ����� ���������� ����� 
#define FASTMODE_DISTANCE (50.f)	//distance to camera from sphere, when zone switches to fast update sequence

CCustomZone::CCustomZone(void) 
{
	m_zone_flags.zero			();

	m_fMaxPower					= 100.f;
	m_fAttenuation				= 1.f;
	m_dwPeriod					= 1100;
	m_fEffectiveRadius			= 0.75f;
//	m_bZoneReady				= false;
	m_bZoneActive				= false;
	m_eHitTypeBlowout			= ALife::eHitTypeWound;
	m_pLocalActor				= NULL;
	m_pIdleParticles			= NULL;
	m_pLight					= NULL;
	m_pIdleLight				= NULL;
	m_pIdleLAnim				= NULL;
	

	m_StateTime.resize(eZoneStateMax);
	for(int i=0; i<eZoneStateMax; i++)
		m_StateTime[i] = 0;


	m_dwAffectFrameNum			= 0;
	m_fArtefactSpawnProbability = 0.f;
	m_fThrowOutPower			= 0.f;
	m_fArtefactSpawnHeight		= 0.f;
	m_fBlowoutWindPowerMax = m_fStoreWindPower = 0.f;
	m_fDistanceToCurEntity		= flt_max;
	m_ef_weapon_type			= u32(-1);
	m_owner_id					= u32(-1);

	m_effector					= xr_new<CZoneEffector>();
}

CCustomZone::~CCustomZone(void) 
{	
	m_idle_sound.destroy		();
	m_accum_sound.destroy		();
	m_awaking_sound.destroy		();
	m_blowout_sound.destroy		();
	m_hit_sound.destroy			();
	m_entrance_sound.destroy	();
	m_ArtefactBornSound.destroy	();

	xr_delete					(m_effector);
}

void CCustomZone::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_iDisableHitTime		= pSettings->r_s32(section,				"disable_time");	
	m_iDisableHitTimeSmall	= pSettings->r_s32(section,				"disable_time_small");	
	m_iDisableIdleTime		= pSettings->r_s32(section,				"disable_idle_time");	
	m_fHitImpulseScale		= pSettings->r_float(section,			"hit_impulse_scale");
	m_fEffectiveRadius		= pSettings->r_float(section,			"effective_radius");
	m_eHitTypeBlowout		= ALife::g_tfString2HitType(pSettings->r_string(section, "hit_type"));

	m_zone_flags.set(eIgnoreNonAlive,	pSettings->r_bool(section,	"ignore_nonalive"));
	m_zone_flags.set(eIgnoreSmall,		pSettings->r_bool(section,	"ignore_small"));
	m_zone_flags.set(eIgnoreArtefact,	pSettings->r_bool(section,	"ignore_artefacts"));
	m_zone_flags.set(eVisibleByDetector,pSettings->r_bool(section,	"visible_by_detector"));
	



	//��������� ������� ��� ����
	m_StateTime[eZoneStateIdle]			= -1;
	m_StateTime[eZoneStateAwaking]		= pSettings->r_s32(section, "awaking_time");
	m_StateTime[eZoneStateBlowout]		= pSettings->r_s32(section, "blowout_time");
	m_StateTime[eZoneStateAccumulate]	= pSettings->r_s32(section, "accamulate_time");
	
//////////////////////////////////////////////////////////////////////////
	ISpatial*		self				=	smart_cast<ISpatial*> (this);
	if (self)		self->spatial.type	|=	(STYPE_COLLIDEABLE|STYPE_SHAPE);
//////////////////////////////////////////////////////////////////////////

	LPCSTR sound_str = NULL;
	
	if(pSettings->line_exist(section,"idle_sound")) 
	{
		sound_str = pSettings->r_string(section,"idle_sound");
		m_idle_sound.create(TRUE, sound_str, st_SourceType);
	}
	
	if(pSettings->line_exist(section,"accum_sound")) 
	{
		sound_str = pSettings->r_string(section,"accum_sound");
		m_accum_sound.create(TRUE, sound_str, st_SourceType);
	}
	if(pSettings->line_exist(section,"awake_sound")) 
	{
		sound_str = pSettings->r_string(section,"awake_sound");
		m_awaking_sound.create(TRUE, sound_str, st_SourceType);
	}
	
	if(pSettings->line_exist(section,"blowout_sound")) 
	{
		sound_str = pSettings->r_string(section,"blowout_sound");
		m_blowout_sound.create(TRUE, sound_str, st_SourceType);
	}
	
	
	if(pSettings->line_exist(section,"hit_sound")) 
	{
		sound_str = pSettings->r_string(section,"hit_sound");
		m_hit_sound.create(TRUE, sound_str, st_SourceType);
	}

	if(pSettings->line_exist(section,"entrance_sound")) 
	{
		sound_str = pSettings->r_string(section,"entrance_sound");
		m_entrance_sound.create(TRUE, sound_str, st_SourceType);
	}


	if(pSettings->line_exist(section,"idle_particles")) 
		m_sIdleParticles	= pSettings->r_string(section,"idle_particles");
	if(pSettings->line_exist(section,"blowout_particles")) 
		m_sBlowoutParticles = pSettings->r_string(section,"blowout_particles");

	if(pSettings->line_exist(section,"accum_particles")) 
		m_sAccumParticles = pSettings->r_string(section,"accum_particles");

	if(pSettings->line_exist(section,"awake_particles")) 
		m_sAwakingParticles = pSettings->r_string(section,"awake_particles");
	

	if(pSettings->line_exist(section,"entrance_small_particles")) 
		m_sEntranceParticlesSmall = pSettings->r_string(section,"entrance_small_particles");
	if(pSettings->line_exist(section,"entrance_big_particles")) 
		m_sEntranceParticlesBig = pSettings->r_string(section,"entrance_big_particles");

	if(pSettings->line_exist(section,"hit_small_particles")) 
		m_sHitParticlesSmall = pSettings->r_string(section,"hit_small_particles");
	if(pSettings->line_exist(section,"hit_big_particles")) 
		m_sHitParticlesBig = pSettings->r_string(section,"hit_big_particles");

	if(pSettings->line_exist(section,"idle_small_particles")) 
		m_sIdleObjectParticlesBig = pSettings->r_string(section,"idle_big_particles");
	if(pSettings->line_exist(section,"idle_big_particles")) 
		m_sIdleObjectParticlesSmall = pSettings->r_string(section,"idle_small_particles");

	if(pSettings->line_exist(section,"postprocess")) 
		m_effector->Load(pSettings->r_string(section,"postprocess"));



	if(pSettings->line_exist(section,"blowout_particles_time")) 
	{
		m_dwBlowoutParticlesTime = pSettings->r_u32(section,"blowout_particles_time");
		if (s32(m_dwBlowoutParticlesTime)>m_StateTime[eZoneStateBlowout])	{
			m_dwBlowoutParticlesTime=m_StateTime[eZoneStateBlowout];
			Msg("! ERROR: invalid 'blowout_particles_time' in '%s'",section);
		}
	}
	else
		m_dwBlowoutParticlesTime = 0;

	if(pSettings->line_exist(section,"blowout_light_time")) 
	{
		m_dwBlowoutLightTime = pSettings->r_u32(section,"blowout_light_time");
		if (s32(m_dwBlowoutLightTime)>m_StateTime[eZoneStateBlowout])	{
			m_dwBlowoutLightTime=m_StateTime[eZoneStateBlowout];
			Msg("! ERROR: invalid 'blowout_light_time' in '%s'",section);
		}
	}
	else
		m_dwBlowoutLightTime = 0;

	if(pSettings->line_exist(section,"blowout_sound_time")) 
	{
		m_dwBlowoutSoundTime = pSettings->r_u32(section,"blowout_sound_time");
		if (s32(m_dwBlowoutSoundTime)>m_StateTime[eZoneStateBlowout])	{
			m_dwBlowoutSoundTime=m_StateTime[eZoneStateBlowout];
			Msg("! ERROR: invalid 'blowout_sound_time' in '%s'",section);
		}
	}
	else
		m_dwBlowoutSoundTime = 0;

	if(pSettings->line_exist(section,"blowout_explosion_time"))	{
		m_dwBlowoutExplosionTime = pSettings->r_u32(section,"blowout_explosion_time"); 
		if (s32(m_dwBlowoutExplosionTime)>m_StateTime[eZoneStateBlowout])	{
			m_dwBlowoutExplosionTime=m_StateTime[eZoneStateBlowout];
			Msg("! ERROR: invalid 'blowout_explosion_time' in '%s'",section);
		}
	}
	else
		m_dwBlowoutExplosionTime = 0;

	m_zone_flags.set(eBlowoutWind,  pSettings->r_bool(section,"blowout_wind"));
	if( m_zone_flags.test(eBlowoutWind) ){
		m_dwBlowoutWindTimeStart = pSettings->r_u32(section,"blowout_wind_time_start"); 
		m_dwBlowoutWindTimePeak = pSettings->r_u32(section,"blowout_wind_time_peak"); 
		m_dwBlowoutWindTimeEnd = pSettings->r_u32(section,"blowout_wind_time_end"); 
		R_ASSERT(m_dwBlowoutWindTimeStart < m_dwBlowoutWindTimePeak);
		R_ASSERT(m_dwBlowoutWindTimePeak < m_dwBlowoutWindTimeEnd);

		if((s32)m_dwBlowoutWindTimeEnd < m_StateTime[eZoneStateBlowout]){
			m_dwBlowoutWindTimeEnd =u32( m_StateTime[eZoneStateBlowout]-1);
			Msg("! ERROR: invalid 'blowout_wind_time_end' in '%s'",section);
		}

		
		m_fBlowoutWindPowerMax = pSettings->r_float(section,"blowout_wind_power");
	}

	//��������� ��������� �������� ������� �� ������
	m_zone_flags.set(eBlowoutLight, pSettings->r_bool (section, "blowout_light"));

	if(m_zone_flags.test(eBlowoutLight) ){
		sscanf(pSettings->r_string(section,"light_color"), "%f,%f,%f", &m_LightColor.r, &m_LightColor.g, &m_LightColor.b);
		m_fLightRange			= pSettings->r_float(section,"light_range");
		m_fLightTime			= pSettings->r_float(section,"light_time");
		m_fLightTimeLeft		= 0;

		m_fLightHeight		= pSettings->r_float(section,"light_height");
	}

	//��������� ��������� idle ���������
	m_zone_flags.set(eIdleLight,	pSettings->r_bool (section, "idle_light"));
	if( m_zone_flags.test(eIdleLight) )
	{
		m_fIdleLightRange = pSettings->r_float(section,"idle_light_range");
		m_fIdleLightRangeDelta = pSettings->r_float(section,"idle_light_range_delta");
		LPCSTR light_anim = pSettings->r_string(section,"idle_light_anim");
		m_pIdleLAnim	 = LALib.FindItem(light_anim);
		m_fIdleLightHeight = pSettings->r_float(section,"idle_light_height");
	}


	//��������� ��������� ��� ������������� ����������
	m_zone_flags.set(eSpawnBlowoutArtefacts,	pSettings->r_bool(section,"spawn_blowout_artefacts"));
	if( m_zone_flags.test(eSpawnBlowoutArtefacts) )
	{
		m_fArtefactSpawnProbability	= pSettings->r_float (section,"artefact_spawn_probability");
		if(pSettings->line_exist(section,"artefact_spawn_particles")) 
			m_sArtefactSpawnParticles = pSettings->r_string(section,"artefact_spawn_particles");
		else
			m_sArtefactSpawnParticles = NULL;

		if(pSettings->line_exist(section,"artefact_born_sound"))
		{
			sound_str = pSettings->r_string(section,"artefact_born_sound");
			m_ArtefactBornSound.create(TRUE, sound_str, st_SourceType);
		}

		m_fThrowOutPower = pSettings->r_float (section,			"throw_out_power");
		m_fArtefactSpawnHeight = pSettings->r_float (section,	"artefact_spawn_height");

		LPCSTR						l_caParameters = pSettings->r_string(section, "artefacts");
		u16 m_wItemCount			= (u16)_GetItemCount(l_caParameters);
		R_ASSERT2					(!(m_wItemCount & 1),"Invalid number of parameters in string 'artefacts' in the 'system.ltx'!");
		m_wItemCount			>>= 1;

		m_ArtefactSpawn.clear();
		string512 l_caBuffer;

		float total_probability = 0.f;

		m_ArtefactSpawn.resize(m_wItemCount);
		for (u16 i=0; i<m_wItemCount; ++i) 
		{
			ARTEFACT_SPAWN& artefact_spawn = m_ArtefactSpawn[i];
			artefact_spawn.section = _GetItem(l_caParameters,i << 1,l_caBuffer);
			artefact_spawn.probability = (float)atof(_GetItem(l_caParameters,(i << 1) | 1,l_caBuffer));
			total_probability += artefact_spawn.probability;
		}

		R_ASSERT2(!fis_zero(total_probability), "The probability of artefact spawn is zero!");
		//��������������� �����������
		for(i=0; i<m_ArtefactSpawn.size(); ++i)
		{
			m_ArtefactSpawn[i].probability = m_ArtefactSpawn[i].probability/total_probability;
		}
	}

	m_ef_anomaly_type			= pSettings->r_u32(section,"ef_anomaly_type");
	m_ef_weapon_type			= pSettings->r_u32(section,"ef_weapon_type");
}

BOOL CCustomZone::net_Spawn(CSE_Abstract* DC) 
{
	if (!inherited::net_Spawn(DC))
		return					(FALSE);

	CSE_Abstract				*e = (CSE_Abstract*)(DC);
	CSE_ALifeCustomZone			*Z = smart_cast<CSE_ALifeCustomZone*>(e);
	VERIFY						(Z);
//	CSE_ALifeAnomalousZone		*C = smart_cast<CSE_ALifeAnomalousZone*>(e);
	
	m_fMaxPower					= Z->m_maxPower;
	m_fAttenuation				= Z->m_attn;
	m_dwPeriod					= Z->m_period;
	m_owner_id					= Z->m_owner_id;
	if (GameID() != GAME_SINGLE)
		m_zone_flags.set(eSpawnBlowoutArtefacts,	FALSE);

	m_TimeToDisable				= Z->m_enabled_time*1000;
	m_TimeToEnable				= Z->m_disabled_time*1000;
	m_TimeShift					= Z->m_start_time_shift*1000;
	m_StartTime					= Device.dwTimeGlobal;
	m_zone_flags.set			(eUseOnOffTime,	(m_TimeToDisable!=0)&&(m_TimeToEnable!=0) );

	//�������� ��������� �����
	if ( m_zone_flags.test(eIdleLight) ){
		m_pIdleLight = ::Render->light_create();
		m_pIdleLight->set_shadow(true);
	}
	else
		m_pIdleLight = NULL;

	if ( m_zone_flags.test(eBlowoutLight) ) {
		m_pLight = ::Render->light_create();
		m_pLight->set_shadow(true);
	}
	else
		m_pLight = NULL;

	shedule_register			();
	setEnabled					(TRUE);

	PlayIdleParticles			();

	m_eZoneState				= eZoneStateIdle;
	m_iPreviousStateTime		= m_iStateTime = 0;

	m_effector->SetRadius		(CFORM()->getSphere().R);

	m_dwLastTimeMoved			= Device.dwTimeGlobal;
	m_vPrevPos.set				(Position());


	PrefetchArtefacts			();

	m_fDistanceToCurEntity		= flt_max;
	m_bBlowoutWindActive		= false;

	o_fastmode					= TRUE;		// start initially with fast-mode enabled
	return						(TRUE);
}

void CCustomZone::net_Destroy() 
{
	StopIdleParticles		();

	inherited::net_Destroy	();

	StopWind				();

	m_pLight.destroy		();
	m_pIdleLight.destroy	();

	CParticlesObject::Destroy(m_pIdleParticles);

	m_effector->Stop		();
	//---------------------------------------------
	OBJECT_INFO_VEC_IT i=m_ObjectInfoMap.begin(),e=m_ObjectInfoMap.end();
	for(;e!=i;i++)exit_Zone(*i);
	m_ObjectInfoMap.clear();	
}

void CCustomZone::net_Import(NET_Packet& P)
{
	inherited::net_Import(P);
	P.r_u32				(m_owner_id);
}

void CCustomZone::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);
	P.w_u32				(m_owner_id);
}

bool CCustomZone::IdleState()
{
	UpdateOnOffState	();

	return false;
}

bool CCustomZone::AwakingState()
{
	if(m_iStateTime>=m_StateTime[eZoneStateAwaking])
	{
		SwitchZoneState(eZoneStateBlowout);
		return true;
	}
	return false;
}

bool CCustomZone::BlowoutState()
{
	if(m_iStateTime>=m_StateTime[eZoneStateBlowout])
	{
		SwitchZoneState(eZoneStateAccumulate);
		return true;
	}
	return false;
}
bool CCustomZone::AccumulateState()
{
	if(m_iStateTime>=m_StateTime[eZoneStateAccumulate])
	{
		if(m_bZoneActive)
			SwitchZoneState(eZoneStateBlowout);
		else
			SwitchZoneState(eZoneStateIdle);

		return true;
	}
	return false;
}

void CCustomZone::UpdateWorkload	(u32 dt)
{
	m_iPreviousStateTime	= m_iStateTime;
	m_iStateTime			+= (int)dt;

	if (!IsEnabled())		{
		if (EnableEffector())
			m_effector->Stop();
		return;
	};

	UpdateIdleLight			();

	switch(m_eZoneState)
	{
	case eZoneStateIdle:
		IdleState();
		break;
	case eZoneStateAwaking:
		AwakingState();
		break;
	case eZoneStateBlowout:
		BlowoutState();
		break;
	case eZoneStateAccumulate:
		AccumulateState();
		break;
	case eZoneStateDisabled:
		break;
	default: NODEFAULT;
	}

	//��������� ����� ������������ ����
	if(m_bZoneActive)	m_dwDeltaTime	+=	dt;
	else				m_dwDeltaTime	=	0;

	if(m_dwDeltaTime > m_dwPeriod)		{
		m_dwDeltaTime	= m_dwPeriod;
//		m_bZoneReady	= true;
	}

	if (Level().CurrentEntity()) {
		m_fDistanceToCurEntity = Level().CurrentEntity()->Position().distance_to(Position());

		if (EnableEffector())// && Level().CurrentEntity() && Level().CurrentEntity()->CLS_ID != CLSID_SPECTATOR)
			m_effector->Update(m_fDistanceToCurEntity);
	}

	if(m_pLight && m_pLight->get_active())
		UpdateBlowoutLight	();
}

// called only in "fast-mode"
void CCustomZone::UpdateCL		() 
{
	inherited::UpdateCL			();
	if (o_fastmode)				UpdateWorkload	(Device.dwTimeDelta);	
}

// called as usual
void CCustomZone::shedule_Update(u32 dt)
{
	m_bZoneActive			= false;
	const Fsphere& s		= CFORM()->getSphere();
	Fvector					P;
	XFORM().transform_tiny	(P,s.P);

	// update
	feel_touch_update		(P,s.R);
	if (IsEnabled())
	{
		//�������� �� ���� �������� � ����
		//� ��������� �� ���������
		for(OBJECT_INFO_VEC_IT it = m_ObjectInfoMap.begin(); 
			m_ObjectInfoMap.end() != it; ++it) 
		{
			CObject* pObject = (*it).object;
			if (!pObject) continue;
			CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(pObject);
			SZoneObjectInfo& info = (*it);

			info.time_in_zone += dt;

			if((!info.small_object && m_iDisableHitTime != -1 && (int)info.time_in_zone > m_iDisableHitTime) ||
				(info.small_object && m_iDisableHitTimeSmall != -1 && (int)info.time_in_zone > m_iDisableHitTimeSmall))
			{
				if(!pEntityAlive || !pEntityAlive->g_Alive())
					info.zone_ignore = true;
			}
			if(m_iDisableIdleTime != -1 && (int)info.time_in_zone > m_iDisableIdleTime)
			{
				if(!pEntityAlive || !pEntityAlive->g_Alive())
					StopObjectIdleParticles(smart_cast<CPhysicsShellHolder*>(pObject));
			}

			//���� ���� ���� �� ���� �� ���������� ������, ��
			//���� ��������� ��������
			if(info.zone_ignore == false) 
				m_bZoneActive = true;
		}
	};

	//� ���� ����� ������, ��������� ��
	if(m_bZoneActive && eZoneStateIdle ==  m_eZoneState)
		SwitchZoneState(eZoneStateAwaking);
	inherited::shedule_Update(dt);

	//////////////////////////////////////////////////////////////////////////
	// check "fast-mode" border
	float	cam_distance	= Device.vCameraPosition.distance_to(P)-s.R;
	if (cam_distance > FASTMODE_DISTANCE)	o_switch_2_slow	();
	else									o_switch_2_fast	();
	if (!o_fastmode)		UpdateWorkload	(dt);

	UpdateOnOffState	();
}

void CCustomZone::feel_touch_new	(CObject* O) 
{
	
#ifdef DEBUG
	if(bDebug) HUD().outMessage(0xffffffff,O->cName(),"entering a zone.");
#endif
	if(smart_cast<CActor*>(O) && O == Level().CurrentEntity())
					m_pLocalActor	= smart_cast<CActor*>(O);

	CGameObject*	pGameObject		= smart_cast<CGameObject*>(O);
	CEntityAlive*	pEntityAlive	= smart_cast<CEntityAlive*>(pGameObject);
	CArtefact*		pArtefact		= smart_cast<CArtefact*>(pGameObject);
	
	SZoneObjectInfo object_info		;
	object_info.object = pGameObject;

	if(pEntityAlive && pEntityAlive->g_Alive())
		object_info.nonalive_object = false;
	else
		object_info.nonalive_object = true;

	if(pGameObject->Radius()<SMALL_OBJECT_RADIUS)
		object_info.small_object = true;
	else
		object_info.small_object = false;

	if((object_info.small_object && m_zone_flags.test(eIgnoreSmall)) ||
		(object_info.nonalive_object && m_zone_flags.test(eIgnoreNonAlive)) || 
		(pArtefact && m_zone_flags.test(eIgnoreArtefact)))
		object_info.zone_ignore = true;
	else
		object_info.zone_ignore = false;
	enter_Zone(object_info);
	m_ObjectInfoMap.push_back(object_info);
	
	if (IsEnabled())
	{
		PlayEntranceParticles(pGameObject);
		PlayObjectIdleParticles(pGameObject);
	}
};

void CCustomZone::feel_touch_delete(CObject* O) 
{
#ifdef DEBUG
	if(bDebug) HUD().outMessage(0xffffffff,O->cName(),"leaving a zone.");
#endif

	if(smart_cast<CActor*>(O)) m_pLocalActor = NULL;
	CGameObject* pGameObject =smart_cast<CGameObject*>(O);
	if(!pGameObject->getDestroy())
	{
		StopObjectIdleParticles(pGameObject);
	}

	OBJECT_INFO_VEC_IT it = std::find(m_ObjectInfoMap.begin(),m_ObjectInfoMap.end(),pGameObject);
	if(it!=m_ObjectInfoMap.end())
	{	
		exit_Zone(*it);
		m_ObjectInfoMap.erase(it);
	}
}

BOOL CCustomZone::feel_touch_contact(CObject* O) 
{
	if (smart_cast<CCustomZone*>(O))				return FALSE;
	if (0==smart_cast<CKinematics*>(O->Visual()))	return FALSE;

	if (O->ID() == ID())
		return		(FALSE);

	CGameObject *object = smart_cast<CGameObject*>(O);
    if (!object || !object->IsVisibleForZones())
		return		(FALSE);

	if (!((CCF_Shape*)CFORM())->Contact(O))
		return		(FALSE);

	return			(object->feel_touch_on_contact(this));
}


float CCustomZone::RelativePower(float dist)
{
	float radius = effective_radius();
	float power = radius < dist ? 0 : (1.f - m_fAttenuation*(dist/radius)*(dist/radius));
	return power < 0 ? 0 : power;
}
float CCustomZone::effective_radius()
{
	return Radius()*m_fEffectiveRadius;
}

float CCustomZone::distance_to_center(CObject* O)
{
	Fvector P; 
	XFORM().transform_tiny(P,CFORM()->getSphere().P);
	return P.distance_to(O->Position());
}
float CCustomZone::Power(float dist) 
{
	return  m_fMaxPower * RelativePower(dist);
}

void CCustomZone::PlayIdleParticles()
{
	m_idle_sound.play_at_pos(this, Position(), true);

	if(*m_sIdleParticles)
	{
		if (!m_pIdleParticles)
		{
			m_pIdleParticles = CParticlesObject::Create(*m_sIdleParticles,FALSE);
			m_pIdleParticles->UpdateParent(XFORM(),zero_vel);
		}
		m_pIdleParticles->UpdateParent(XFORM(),zero_vel);
		m_pIdleParticles->Play();
	}

	StartIdleLight	();
}

void CCustomZone::StopIdleParticles()
{
	m_idle_sound.stop();

	if(m_pIdleParticles)
		m_pIdleParticles->Stop();

	StopIdleLight();
}


void  CCustomZone::StartIdleLight	()
{
	if(m_pIdleLight)
	{
		m_pIdleLight->set_range(m_fIdleLightRange);
		Fvector pos = Position();
		pos.y += m_fIdleLightHeight;
		m_pIdleLight->set_position(pos);
		m_pIdleLight->set_active(true);
	}
}
void  CCustomZone::StopIdleLight	()
{
	if(m_pIdleLight)
		m_pIdleLight->set_active(false);
}
void CCustomZone::UpdateIdleLight	()
{
	if(!m_pIdleLight || !m_pIdleLight->get_active())
		return;


	VERIFY(m_pIdleLAnim);

	int frame = 0;
	u32 clr					= m_pIdleLAnim->CalculateBGR(Device.fTimeGlobal,frame); // ���������� � ������� BGR
	Fcolor					fclr;
	fclr.set				((float)color_get_B(clr)/255.f,(float)color_get_G(clr)/255.f,(float)color_get_R(clr)/255.f,1.f);
	
	float range = m_fIdleLightRange + m_fIdleLightRangeDelta*::Random.randF(-1.f,1.f);
	m_pIdleLight->set_range	(range);
	m_pIdleLight->set_color	(fclr);

	Fvector pos		= Position();
	pos.y			+= m_fIdleLightHeight;
	m_pIdleLight->set_position(pos);
}


void CCustomZone::PlayBlowoutParticles()
{
	if(!m_sBlowoutParticles) return;

	CParticlesObject* pParticles;
	pParticles	= CParticlesObject::Create(*m_sBlowoutParticles,TRUE);
	pParticles->UpdateParent(XFORM(),zero_vel);
	pParticles->Play();
}

void CCustomZone::PlayHitParticles(CGameObject* pObject)
{
	m_hit_sound.play_at_pos(this, pObject->Position());

	shared_str particle_str = NULL;

	//������ �������� ��� �������� ������� �������
	if(pObject->Radius()<SMALL_OBJECT_RADIUS)
	{
		if(!m_sHitParticlesSmall) return;
		particle_str = m_sHitParticlesSmall;
	}
	else
	{
		if(!m_sHitParticlesBig) return;
		particle_str = m_sHitParticlesBig;
	}

	if( particle_str.size() ){
		// play particles
		CParticlesPlayer* PP = smart_cast<CParticlesPlayer*>(pObject);
		if (PP){
			u16 play_bone = PP->GetRandomBone(); 
			if (play_bone!=BI_NONE)
				PP->StartParticles	(particle_str,play_bone,Fvector().set(0,1,0), ID());
		}
	}
}

void CCustomZone::PlayEntranceParticles(CGameObject* pObject)
{
	m_entrance_sound.play_at_pos(this, pObject->Position());

	shared_str particle_str = NULL;

	//������ �������� ��� �������� ������� �������
	if(pObject->Radius()<SMALL_OBJECT_RADIUS)
	{
		if(!m_sEntranceParticlesSmall) return;
		particle_str = m_sEntranceParticlesSmall;
	}
	else
	{
		if(!m_sEntranceParticlesBig) return;
		particle_str = m_sEntranceParticlesBig;
	}

	Fvector vel;
	CPhysicsShellHolder* shell_holder=smart_cast<CPhysicsShellHolder*>(pObject);
	if(shell_holder)
		shell_holder->PHGetLinearVell(vel);
	else 
		vel.set(0,0,0);
	
	//������� ��������� �������� �� �������
	CParticlesPlayer* PP = smart_cast<CParticlesPlayer*>(pObject);
	if (PP){
		u16 play_bone = PP->GetRandomBone(); 
		if (play_bone!=BI_NONE){
			CParticlesObject* pParticles = CParticlesObject::Create(*particle_str,TRUE);
			Fmatrix xform;

			Fvector dir;
			if(fis_zero(vel.magnitude()))
				dir.set(0,1,0);
			else
			{
				dir.set(vel);
				dir.normalize();
			}

			PP->MakeXFORM			(pObject,play_bone,dir,Fvector().set(0,0,0),xform);
			pParticles->UpdateParent(xform, vel);
			if (IsEnabled())
				pParticles->Play		();
		}
	}
}


void CCustomZone::PlayBulletParticles(Fvector& pos)
{
	m_entrance_sound.play_at_pos(this, pos);

	if(!m_sEntranceParticlesSmall) return;
	
	CParticlesObject* pParticles;
	pParticles = CParticlesObject::Create(*m_sEntranceParticlesSmall,TRUE);
	
	Fmatrix M;
	M = XFORM();
	M.c.set(pos);

	pParticles->UpdateParent(M,zero_vel);
	pParticles->Play();
}

void CCustomZone::PlayObjectIdleParticles(CGameObject* pObject)
{
	CParticlesPlayer* PP = smart_cast<CParticlesPlayer*>(pObject);
	if(!PP) return;

	shared_str particle_str = NULL;

	//������ �������� ��� �������� ������� �������
	if(pObject->Radius()<SMALL_OBJECT_RADIUS)
	{
		if(!m_sIdleObjectParticlesSmall) return;
		particle_str = m_sIdleObjectParticlesSmall;
	}
	else
	{
		if(!m_sIdleObjectParticlesBig) return;
		particle_str = m_sIdleObjectParticlesBig;
	}

	
	//��������� �������� �� �������
	PP->StartParticles (particle_str, Fvector().set(0,1,0), ID());
	if (!IsEnabled())
		PP->StopParticles	(particle_str);
}

void CCustomZone::StopObjectIdleParticles(CGameObject* pObject)
{
	CParticlesPlayer* PP = smart_cast<CParticlesPlayer*>(pObject);
	if(!PP) return;


	OBJECT_INFO_VEC_IT it	= std::find(m_ObjectInfoMap.begin(),m_ObjectInfoMap.end(),pObject);
	if(m_ObjectInfoMap.end() == it) return;
	
	
	shared_str particle_str = NULL;
	//������ �������� ��� �������� ������� �������
	if(pObject->Radius()<SMALL_OBJECT_RADIUS)
	{
		if(!m_sIdleObjectParticlesSmall) return;
		particle_str = m_sIdleObjectParticlesSmall;
	}
	else
	{
		if(!m_sIdleObjectParticlesBig) return;
		particle_str = m_sIdleObjectParticlesBig;
	}

	//���������� ��������
	PP->StopParticles	(particle_str);
}


void  CCustomZone::Hit(float P, Fvector &dir,
					  CObject* who, s16 element,
					  Fvector position_in_object_space,
					  float impulse,
					  ALife::EHitType hit_type)
{
	Fmatrix M;
	M.identity();
	M.translate_over(position_in_object_space);
	M.mulA(XFORM());
	PlayBulletParticles(M.c);	
}

void CCustomZone::StartBlowoutLight		()
{
	if(!m_pLight || m_fLightTime<=0.f) return;
	
	m_fLightTimeLeft = m_fLightTime;

	m_pLight->set_color(m_LightColor.r, m_LightColor.g, m_LightColor.b);
	m_pLight->set_range(m_fLightRange);
	
	Fvector pos = Position();
	pos.y		+= m_fLightHeight;
	m_pLight->set_position(pos);
	m_pLight->set_active(true);

}

void  CCustomZone::StopBlowoutLight		()
{
	m_fLightTimeLeft = 0.f;
	m_pLight->set_active(false);
}

void CCustomZone::UpdateBlowoutLight	()
{
	if(m_fLightTimeLeft>0)
	{
		m_fLightTimeLeft -= Device.fTimeDelta;

		float scale = m_fLightTimeLeft/m_fLightTime;
		m_pLight->set_color(m_LightColor.r*scale, 
							m_LightColor.g*scale, 
							m_LightColor.b*scale);
		m_pLight->set_range(m_fLightRange*scale);

		Fvector pos = Position();
		pos.y += m_fLightHeight;
		m_pLight->set_position(pos);
	}
	else
		StopBlowoutLight ();
}

void CCustomZone::AffectObjects()
{
	if(m_dwAffectFrameNum == Device.dwFrame) return;

	m_dwAffectFrameNum = Device.dwFrame;

	OBJECT_INFO_VEC_IT it;
	for(it = m_ObjectInfoMap.begin(); m_ObjectInfoMap.end() != it; ++it) 
	{
		if( !(*it).object->getDestroy() )
			Affect( &(*it) );
	}

}

void CCustomZone::UpdateBlowout()
{
	if(m_dwBlowoutParticlesTime>=(u32)m_iPreviousStateTime && 
		m_dwBlowoutParticlesTime<(u32)m_iStateTime)
		PlayBlowoutParticles();

	if(m_dwBlowoutLightTime>=(u32)m_iPreviousStateTime && 
		m_dwBlowoutLightTime<(u32)m_iStateTime)
		StartBlowoutLight ();

	if(m_dwBlowoutSoundTime>=(u32)m_iPreviousStateTime && 
		m_dwBlowoutSoundTime<(u32)m_iStateTime)
		m_blowout_sound.play_at_pos	(this, Position());

	if(m_zone_flags.test(eBlowoutWind) && m_dwBlowoutWindTimeStart>=(u32)m_iPreviousStateTime && 
		m_dwBlowoutWindTimeStart<(u32)m_iStateTime)
		StartWind();

	UpdateWind();


	if(m_dwBlowoutExplosionTime>=(u32)m_iPreviousStateTime && 
		m_dwBlowoutExplosionTime<(u32)m_iStateTime)
	{
		AffectObjects();

		//���������� ��������
		BornArtefact();
	}
}

void  CCustomZone::OnMove	()
{
	if(m_dwLastTimeMoved == 0)
	{
		m_dwLastTimeMoved = Device.dwTimeGlobal;
		m_vPrevPos.set(Position());
	}
	else
	{
		float time_delta = float(Device.dwTimeGlobal - m_dwLastTimeMoved)/1000.f;
		m_dwLastTimeMoved = Device.dwTimeGlobal;
		//�������� ��������
		
		Fvector vel;
			
		if(fis_zero(time_delta))
			vel = zero_vel;
		else
		{
			vel.sub(Position(), m_vPrevPos);
			vel.div(time_delta);
		}

		if (m_pIdleParticles)
			m_pIdleParticles->UpdateParent(XFORM(), vel);

		if(m_pLight && m_pLight->get_active())
			m_pLight->set_position(Position());

		if(m_pIdleLight && m_pIdleLight->get_active())
			m_pIdleLight->set_position(Position());
     }
}

void	CCustomZone::OnEvent (NET_Packet& P, u16 type)
{	
	switch (type)
	{
		case GE_ZONE_STATE_CHANGE:
			{
				u8				S;
				P.r_u8			(S);
#ifdef DEBUG
				string128 s1,s2;
				sprintf(s2, " Time = [%u]", Device.dwTimeGlobal);

				switch (S)
				{
				case eZoneStateIdle:
					if(bDebug) HUD().outMessage(0xffffffff,*cName(),strconcat(s1,"zone switch to State Idle",s2));
					break;																
				case eZoneStateAwaking:													
					if(bDebug) HUD().outMessage(0xffffffff,*cName(),strconcat(s1,"zone switch to State Awaking",s2));
					break;																
				case eZoneStateBlowout:													
					if(bDebug) HUD().outMessage(0xffffffff,*cName(),strconcat(s1,"zone switch to State Blowout",s2));
					break;																
				case eZoneStateAccumulate:												
					if(bDebug) HUD().outMessage(0xffffffff,*cName(),strconcat(s1,"zone switch to State Accumulate",s2));
					break;
				case eZoneStateDisabled:												
					if(bDebug) HUD().outMessage(0xffffffff,*cName(),strconcat(s1,"zone switch to State Disabled",s2));
					break;
				};
#endif
				OnStateSwitch(EZoneState(S));
				break;
			}
		case GE_OWNERSHIP_TAKE : 
			{
				u16 id;
                P.r_u16(id);
				OnOwnershipTake(id);
				break;
			} 
         case GE_OWNERSHIP_REJECT : 
			 {
				 u16 id;
                 P.r_u16			(id);
                 CArtefact *artefact = smart_cast<CArtefact*>(Level().Objects.net_Find(id)); 
				 if(artefact)
				 {
					artefact->H_SetParent(NULL);
					ThrowOutArtefact(artefact);
				 }
                 break;
			}
	}
	inherited::OnEvent(P, type);
};
void CCustomZone::OnOwnershipTake(u16 id)
{
	CArtefact *artefact = smart_cast<CArtefact*>(Level().Objects.net_Find(id));  VERIFY(artefact);
	artefact->H_SetParent(this);
	
	artefact->setVisible(FALSE);
	artefact->setEnabled(FALSE);

	m_SpawnedArtefacts.push_back(artefact);
}

void CCustomZone::OnStateSwitch	(EZoneState new_state)
{
	if (eZoneStateDisabled == new_state)
		Disable();
	else
		Enable();

	if(m_eZoneState==eZoneStateIdle)
		StopIdleParticles();

	if(new_state==eZoneStateIdle)
		PlayIdleParticles();

	if(new_state==eZoneStateAccumulate)
		PlayAccumParticles();

	if(new_state==eZoneStateAwaking)
		PlayAwakingParticles();

	m_eZoneState			= new_state;
	m_iPreviousStateTime	= m_iStateTime = 0;
};

void CCustomZone::SwitchZoneState(EZoneState new_state)
{
	if (OnServer())
	{
		// !!! Just single entry for given state !!!
		NET_Packet		P;
		u_EventGen		(P,GE_ZONE_STATE_CHANGE,ID());
		P.w_u8			(u8(new_state));
		u_EventSend		(P);
	};

	m_iPreviousStateTime = m_iStateTime = 0;
}

bool CCustomZone::Enable()
{
	if (IsEnabled()) return false;

//	PlayIdleParticles();

	for(OBJECT_INFO_VEC_IT it = m_ObjectInfoMap.begin(); 
		m_ObjectInfoMap.end() != it; ++it) 
	{
		CGameObject* pObject = (*it).object;
		if (!pObject) continue;
		PlayEntranceParticles(pObject);
		PlayObjectIdleParticles(pObject);
	}
	return true;
};

bool CCustomZone::Disable()
{
	if (!IsEnabled()) return false;

//	StopIdleParticles();

	for(OBJECT_INFO_VEC_IT it = m_ObjectInfoMap.begin(); 
		m_ObjectInfoMap.end() != it; ++it) 
	{
		CGameObject* pObject = (*it).object;
		if (!pObject) continue;
		StopObjectIdleParticles(pObject);
	}
	return false;
};

void CCustomZone::ZoneEnable()
{
	SwitchZoneState(eZoneStateIdle);
};

void CCustomZone::ZoneDisable()
{
	SwitchZoneState(eZoneStateDisabled);
};


void CCustomZone::SpawnArtefact()
{
	//��������� �������� ������������� ������������
	//����� �������� �� ������ �������
	float rnd = ::Random.randF(.0f,1.f-EPS_L);
	float prob_threshold = 0.f;
	
	std::size_t i=0;
	for(; i<m_ArtefactSpawn.size(); i++)
	{
		prob_threshold += m_ArtefactSpawn[i].probability;
		if(rnd<prob_threshold) break;
	}
	R_ASSERT(i<m_ArtefactSpawn.size());

	Fvector pos;
	Center(pos);
	Level().spawn_item(*m_ArtefactSpawn[i].section, pos, ai_location().level_vertex_id(), ID());
}


void CCustomZone::BornArtefact()
{
	if(!m_zone_flags.test(eSpawnBlowoutArtefacts) || m_SpawnedArtefacts.empty()) return;

	if(::Random.randF(0.f, 1.f)> m_fArtefactSpawnProbability) return;

	CArtefact* pArtefact = m_SpawnedArtefacts.back(); VERIFY(pArtefact);
	m_SpawnedArtefacts.pop_back			();

	if (Local())	{
		if (pArtefact->H_Parent() && (pArtefact->H_Parent()->ID() == this->ID())  )	//. todo: need to remove on actual message parsing
		{
			NET_Packet						P;
			u_EventGen						(P,GE_OWNERSHIP_REJECT,ID());
			P.w_u16							(pArtefact->ID());
			u_EventSend						(P);
		}
	}

	PrefetchArtefacts ();
}

void CCustomZone::ThrowOutArtefact(CArtefact* pArtefact)
{
	pArtefact->XFORM().c.set(Position());
	pArtefact->XFORM().c.y += m_fArtefactSpawnHeight;

	if(*m_sArtefactSpawnParticles)
	{
		CParticlesObject* pParticles;
		pParticles = CParticlesObject::Create(*m_sArtefactSpawnParticles,TRUE);
		pParticles->UpdateParent(pArtefact->XFORM(),zero_vel);
		pParticles->Play();
	}

	m_ArtefactBornSound.play_at_pos(pArtefact, pArtefact->Position());

	Fvector dir;
	dir.random_dir();
	pArtefact->m_pPhysicsShell->applyImpulse (dir, m_fThrowOutPower);
}

void CCustomZone::PrefetchArtefacts()
{
	if (FALSE==m_zone_flags.test(eSpawnBlowoutArtefacts) || m_ArtefactSpawn.empty()) return;

	for(u32 i = m_SpawnedArtefacts.size(); i < PREFETCHED_ARTEFACTS_NUM; ++i)
		SpawnArtefact();
}

void CCustomZone::StartWind()
{
	if(m_fDistanceToCurEntity>WIND_RADIUS) return;

	m_bBlowoutWindActive = true;
	m_fStoreWindPower = g_pGamePersistent->Environment.wind_strength_factor;
	clamp(g_pGamePersistent->Environment.wind_strength_factor, 0.f, 1.f);
}

void CCustomZone::StopWind()
{
	if(!m_bBlowoutWindActive) return;
	m_bBlowoutWindActive = false;
	g_pGamePersistent->Environment.wind_strength_factor = m_fStoreWindPower;
}

void CCustomZone::UpdateWind()
{
	if(!m_bBlowoutWindActive) return;

	if(m_fDistanceToCurEntity>WIND_RADIUS || m_dwBlowoutWindTimeEnd<(u32)m_iStateTime)
	{
		StopWind();
		return;
	}

	if(m_dwBlowoutWindTimePeak > (u32)m_iStateTime)
	{
		g_pGamePersistent->Environment.wind_strength_factor = m_fBlowoutWindPowerMax + ( m_fStoreWindPower - m_fBlowoutWindPowerMax)*
								float(m_dwBlowoutWindTimePeak - (u32)m_iStateTime)/
								float(m_dwBlowoutWindTimePeak - m_dwBlowoutWindTimeStart);
		clamp(g_pGamePersistent->Environment.wind_strength_factor, 0.f, 1.f);
	}
	else
	{
		g_pGamePersistent->Environment.wind_strength_factor = m_fBlowoutWindPowerMax + (m_fStoreWindPower - m_fBlowoutWindPowerMax)*
			float((u32)m_iStateTime - m_dwBlowoutWindTimePeak)/
			float(m_dwBlowoutWindTimeEnd - m_dwBlowoutWindTimePeak);
		clamp(g_pGamePersistent->Environment.wind_strength_factor, 0.f, 1.f);
	}
}

u32	CCustomZone::ef_anomaly_type	() const
{
	return	(m_ef_anomaly_type);
}

u32	CCustomZone::ef_weapon_type		() const
{
	VERIFY	(m_ef_weapon_type != u32(-1));
	return	(m_ef_weapon_type);
}

void CCustomZone::CreateHit	(	u16 id_to, 
								u16 id_from, 
								const Fvector& hit_dir, 
								float hit_power, 
								s16 bone_id, 
								const Fvector& pos_in_bone, 
								float hit_impulse, 
								ALife::EHitType hit_type)
{
	if (OnServer())
	{
		if(m_owner_id != u32(-1) )
			id_to	= (u16)m_owner_id;

		NET_Packet	l_P;
		u_EventGen	(l_P,GE_HIT, id_to);
		l_P.w_u16	(id_from);
		l_P.w_u16	(id_from);
		l_P.w_dir	(hit_dir);
		l_P.w_float	(hit_power);
		l_P.w_s16	(bone_id);
		l_P.w_vec3	(pos_in_bone);
		l_P.w_float	(hit_impulse);
		l_P.w_u16	( (u16)hit_type );
		u_EventSend	(l_P);
	};
}

void CCustomZone::net_Relcase(CObject* O)
{
	CGameObject* GO = smart_cast<CGameObject*>(O);
	OBJECT_INFO_VEC_IT it = std::find(m_ObjectInfoMap.begin(),m_ObjectInfoMap.end(), GO);
	if(it!=m_ObjectInfoMap.end()){
		StopObjectIdleParticles(GO);
		exit_Zone(*it);
		m_ObjectInfoMap.erase(it);
	}
	if(GO->ID()==m_owner_id)	m_owner_id = u32(-1);

	inherited::net_Relcase(O);
}

void CCustomZone::PlayAccumParticles()
{
	if(m_sAccumParticles.size()){
		CParticlesObject* pParticles;
		pParticles	= CParticlesObject::Create(*m_sAccumParticles,TRUE);
		pParticles->UpdateParent(XFORM(),zero_vel);
		pParticles->Play();
	}

	if(m_accum_sound._handle())
		m_accum_sound.play_at_pos	(this, Position());
}

void CCustomZone::PlayAwakingParticles()
{
	if(m_sAwakingParticles.size()){
		CParticlesObject* pParticles;
		pParticles	= CParticlesObject::Create(*m_sAwakingParticles,TRUE);
		pParticles->UpdateParent(XFORM(),zero_vel);
		pParticles->Play();
	}

	if(m_awaking_sound._handle())
		m_awaking_sound.play_at_pos	(this, Position());
}

void CCustomZone::UpdateOnOffState	()
{
	if(!m_zone_flags.test(eUseOnOffTime)) return;
	
	bool dest_state;
	u32 t = (Device.dwTimeGlobal-m_StartTime) % (m_TimeToEnable+m_TimeToDisable);
	t		+= m_TimeShift;
	if	(t < m_TimeToEnable) 
		dest_state=true;
	else
	if	(t >(m_TimeToEnable+m_TimeToDisable) ) 
		dest_state=true;
	else{
		dest_state=false;
		VERIFY(t<(m_TimeToEnable+m_TimeToDisable));
	}

	if( (eZoneStateDisabled==m_eZoneState) && dest_state){
			//switch to idle	
			NET_Packet P;
			u_EventGen		(P,GE_ZONE_STATE_CHANGE,ID());
			P.w_u8			(u8(eZoneStateIdle));
			u_EventSend		(P);
		}else
	if( (eZoneStateIdle==m_eZoneState) && !dest_state){
			//switch to disable	
			NET_Packet P;
			u_EventGen		(P,GE_ZONE_STATE_CHANGE,ID());
			P.w_u8			(u8(eZoneStateDisabled));
			u_EventSend		(P);
		}
}
