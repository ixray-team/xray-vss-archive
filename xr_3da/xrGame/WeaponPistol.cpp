#include "stdafx.h"
#include "weaponpistol.h"
#include "WeaponHUD.h"
#include "effectorshot.h"

CWeaponPistol::CWeaponPistol(LPCSTR name) : CWeaponCustomPistol(name)
{
	m_eSoundClose		= ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING /*| eSoundType*/);
	m_opened = false;
}

CWeaponPistol::~CWeaponPistol(void)
{
}

void CWeaponPistol::net_Destroy()
{
	inherited::net_Destroy();

	// sounds
	SoundDestroy		(sndClose		);
}

void CWeaponPistol::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
	m_opened = false;
}

void CWeaponPistol::switch2_Idle	()
{
	if(m_opened) m_pHUD->animPlay(mhud_empty[Random.randI(mhud_empty.size())]);
	else m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
}
void CWeaponPistol::switch2_Hiding()
{
	if(m_opened) {
		Sound->PlayAtPos		(sndClose,H_Root(),vLastFP);
		m_pHUD->animPlay		(mhud_close[Random.randI(mhud_close.size())],TRUE,this);
	} else inherited::switch2_Hiding	();
}
void CWeaponPistol::switch2_Reload()
{
	m_opened = false;
	inherited::switch2_Reload();
}
void CWeaponPistol::OnAnimationEnd()
{
	if(STATE == eHiding && m_opened) {
		m_opened = false;
		switch2_Hiding();
	} else inherited::OnAnimationEnd();
}

void CWeaponPistol::Load	(LPCSTR section)
{
	inherited::Load		(section);

	SoundCreate			(sndClose,		"close"  ,m_eSoundClose);

	animGet				(mhud_empty,	"empty");
	animGet				(mhud_shot_l,	"shoot_last");
	animGet				(mhud_close,	"close");
}

void CWeaponPistol::OnShot		()
{
	// Sound
	Sound->PlayAtPos			(sndShot,H_Root(),vLastFP);

	// Camera
	if (hud_mode)	
	{
		CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
		if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(xr_new<CEffectorShot>(camMaxAngle,camRelaxSpeed));
		R_ASSERT				(S);
		S->Shot					(camDispersion);
	}
	
	// Animation
	if(iAmmoElapsed > 1) m_pHUD->animPlay			(mhud_shots[Random.randI(mhud_shots.size())],FALSE);
	else { m_pHUD->animPlay			(mhud_shot_l[Random.randI(mhud_shot_l.size())],FALSE); m_opened = true; }
	
	// Flames
	fFlameTime					= .1f;
	
	// Shell Drop
	OnShellDrop					();
}
