#include "stdafx.h"

#include "WeaponKnife.h"
#include "WeaponHUD.h"
#include "Entity.h"
#include "Actor.h"
#include "level.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "../skeletonanimated.h"
#include "gamemtllib.h"
#include "level_bullet_manager.h"

#define KNIFE_MATERIAL_NAME "objects\\knife"

CWeaponKnife::CWeaponKnife() : CWeapon("KNIFE") 
{
	m_attackStart			= false;
	m_bShotLight			= false;
	STATE					= eHidden;
	NEXT_STATE				= eHidden;
	knife_material_idx		= (u16)-1;
}

void CWeaponKnife::Load	(LPCSTR section)
{
	// verify class
	inherited::Load		(section);

	fWallmarkSize = pSettings->r_float(section,"wm_size");

	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGet				(mhud_idle,		pSettings->r_string(*hud_sect,"anim_idle"));
	animGet				(mhud_hide,		pSettings->r_string(*hud_sect,"anim_hide"));
	animGet				(mhud_show,		pSettings->r_string(*hud_sect,"anim_draw"));
	animGet				(mhud_attack,	pSettings->r_string(*hud_sect,"anim_shoot1_start"));
	animGet				(mhud_attack2,	pSettings->r_string(*hud_sect,"anim_shoot2_start"));
	animGet				(mhud_attack_e,	pSettings->r_string(*hud_sect,"anim_shoot1_end"));
	animGet				(mhud_attack2_e,pSettings->r_string(*hud_sect,"anim_shoot2_end"));

	
	knife_material_idx =  GMLib.GetMaterialIdx(KNIFE_MATERIAL_NAME);
}

void CWeaponKnife::OnStateSwitch	(u32 S)
{
	switch (S)
	{
	case eIdle:
		switch2_Idle	();
		break;
	case eShowing:
		switch2_Showing	();
		break;
	case eHiding:
		switch2_Hiding	();
		break;
	case eHidden:
		switch2_Hidden	();
		break;
	case eFire:
	case eFire2:
		switch2_Attacking	(S);
		break;
	}
	STATE = S;
	NEXT_STATE = S;
}
	
void CWeaponKnife::UpdateCL	()
{
	inherited::UpdateCL	();

	// cycle update
	switch (STATE){
	case eShowing:
	case eHiding:{
//.		smart_cast<CKinematicsAnimated*>(m_pHUD->Visual())->UpdateTracks();
		}break;
	case eFire:
		//state_Attacking	(dt);
		break;
	}
}

void CWeaponKnife::KnifeStrike(const Fvector& pos, const Fvector& dir)
{
	CCartridge						cartridge; 
	cartridge.m_buckShot			= 1;				
	cartridge.m_impair				= 1;
	cartridge.m_kDisp				= 1;
	cartridge.m_kHit				= 1;
	cartridge.m_kImpulse			= 1;
	cartridge.m_kPierce				= 1;
	cartridge.m_flags.set			(CCartridge::cfTracer, FALSE);
	cartridge.m_flags.set			(CCartridge::cfRicochet, FALSE);
	cartridge.fWallmarkSize			= fWallmarkSize;
	cartridge.bullet_material_idx	= knife_material_idx;

	while(m_magazine.size() < 2)	m_magazine.push(cartridge);
	iAmmoElapsed					= m_magazine.size();
	bool SendHit					= SendHitAllowed(H_Parent());

	Level().BulletManager().AddBullet(	pos, 
										dir, 
										m_fStartBulletSpeed, 
										fHitPower, 
										fHitImpulse, 
										H_Parent()->ID(), 
										ID(), 
										m_eHitType, 
										fireDistance, 
										cartridge, 
										SendHit);
}


void CWeaponKnife::OnAnimationEnd()
{
	switch (STATE)
	{
	case eHiding:	SwitchState(eHidden);	break;
	case eFire: 
	case eFire2: 
		{
            if(m_attackStart) 
			{
				m_attackStart = false;
				if(STATE==eFire)
					m_pHUD->animPlay(mhud_attack_e[Random.randI(mhud_attack_e.size())], TRUE, this);
				else
					m_pHUD->animPlay(mhud_attack2_e[Random.randI(mhud_attack2_e.size())], TRUE, this);

				Fvector	p1, d; 
				p1.set(get_LastFP()); 
				d.set(get_LastFD());

				if(H_Parent()) 
					smart_cast<CEntity*>(H_Parent())->g_fireParams(this, p1,d);
				else break;

				KnifeStrike(p1,d);
			} 
			else 
				SwitchState(eIdle);
		}break;
	case eShowing:
	case eIdle:	
		SwitchState(eIdle);		break;	
	}
}

void CWeaponKnife::state_Attacking	(float)
{
}

void CWeaponKnife::switch2_Attacking	(u32 state)
{
	if(m_bPending)	return;

	if(state==eFire)
		m_pHUD->animPlay(mhud_attack[Random.randI(mhud_attack.size())],		FALSE, this);
	else //eFire2
		m_pHUD->animPlay(mhud_attack2[Random.randI(mhud_attack2.size())],		FALSE, this);

	m_attackStart	= true;
	m_bPending		= true;
}

void CWeaponKnife::switch2_Idle	()
{
	m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
	m_bPending = false;
}

void CWeaponKnife::switch2_Hiding	()
{
	FireEnd					();
	m_pHUD->animPlay		(mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);
	m_bPending				= true;
}

void CWeaponKnife::switch2_Hidden()
{
	signal_HideComplete		();
}

void CWeaponKnife::switch2_Showing	()
{
	m_pHUD->animPlay		(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
	m_bPending				= true;
}


void CWeaponKnife::FireStart()
{
	//-------------------------------------------
	m_eHitType		= m_eHitType_1;
	fHitPower		= fHitPower_1;
	fHitImpulse		= fHitImpulse_1;
	//-------------------------------------------
	inherited::FireStart();
	SwitchState(eFire);
}



void CWeaponKnife::Fire2Start () 
{
	//-------------------------------------------
	m_eHitType		= m_eHitType_2;
	fHitPower		= fHitPower_2;
	fHitImpulse		= fHitImpulse_2;
	//-------------------------------------------
	inherited::Fire2Start();
	SwitchState(eFire2);
}


bool CWeaponKnife::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) 
	{
		case kWPN_ZOOM : 
			if(flags&CMD_START) Fire2Start();
			else Fire2End();
			return true;
	}
	return false;
}

void CWeaponKnife::LoadFireParams(LPCSTR section, LPCSTR prefix)
{
	inherited::LoadFireParams(section, prefix);

	string256			full_name;
	fHitPower_1			= fHitPower;
	fHitImpulse_1		= fHitImpulse;
	m_eHitType_1		= ALife::g_tfString2HitType(pSettings->r_string(section, "hit_type"));
	
	fHitPower_2			= pSettings->r_float	(section,strconcat(full_name, prefix, "hit_power_2"));
	fHitImpulse_2		= pSettings->r_float	(section,strconcat(full_name, prefix, "hit_impulse_2"));
	m_eHitType_2		= ALife::g_tfString2HitType(pSettings->r_string(section, "hit_type_2"));
}

void CWeaponKnife::StartIdleAnim()
{
	m_pHUD->animDisplay(mhud_idle[Random.randI(mhud_idle.size())], TRUE);
}

#include "script_space.h"

using namespace luabind;

void CWeaponKnife::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponKnife,CGameObject>("CWeaponKnife")
			.def(constructor<>())
	];
}
