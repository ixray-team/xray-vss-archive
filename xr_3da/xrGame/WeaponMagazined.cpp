#include "stdafx.h"
#include "effectorshot.h"
#include "PSObject.h"
#include "hudmanager.h"

#include "WeaponHUD.h"
#include "WeaponMagazined.h"
#include "entity.h"
#include "actor.h"
#include "actor.h"
#include "ParticlesObject.h"

#include "scope.h"
#include "silencer.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponMagazined::CWeaponMagazined(LPCSTR name, ESoundTypes eSoundType) : CWeapon(name)
{
	m_eSoundShow		= ESoundTypes(SOUND_TYPE_WEAPON_TAKING | eSoundType);
	m_eSoundHide		= ESoundTypes(SOUND_TYPE_WEAPON_HIDING | eSoundType);
	m_eSoundShot		= ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING | eSoundType);
	m_eSoundEmptyClick	= ESoundTypes(SOUND_TYPE_WEAPON_EMPTY_CLICKING | eSoundType);
	m_eSoundReload		= ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING | eSoundType);
	m_eSoundRicochet	= ESoundTypes(SOUND_TYPE_WEAPON_BULLET_RICOCHET | eSoundType);
	fTime				= 0;
	m_queueSize = m_shotNum = 0;
}

CWeaponMagazined::~CWeaponMagazined()
{
	MediaUNLOAD		();
}

void CWeaponMagazined::net_Destroy()
{
	inherited::net_Destroy();

	// sounds
	SoundDestroy		(sndShow		);
	SoundDestroy		(sndHide		);
	SoundDestroy		(sndShot		);
	SoundDestroy		(sndEmptyClick	);
	SoundDestroy		(sndReload		);
	SoundDestroy		(sndRicochet[0]	);
	SoundDestroy		(sndRicochet[1]	);
	SoundDestroy		(sndRicochet[2]	);
	SoundDestroy		(sndRicochet[3]	);
	SoundDestroy		(sndRicochet[4]	);
}


void CWeaponMagazined::Load	(LPCSTR section)
{
	inherited::Load		(section);
	bFlame				= FALSE;
	// Sounds
	SoundCreate			(sndShow,		"draw"    ,m_eSoundShow);
	SoundCreate			(sndHide,		"holster" ,m_eSoundHide);
	SoundCreate			(sndShot,		"shoot"   ,m_eSoundShot);
	SoundCreate			(sndEmptyClick,	"empty"   ,m_eSoundEmptyClick);
	SoundCreate			(sndReload,		"reload"  ,m_eSoundReload);
	SoundCreate			(sndRicochet[0],"ric1"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[1],"ric2"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[2],"ric3"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[3],"ric4"    ,m_eSoundRicochet);
	SoundCreate			(sndRicochet[4],"ric5"    ,m_eSoundRicochet);
	
	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGet				(mhud_idle,		pSettings->r_string(*hud_sect, "anim_idle"));
	animGet				(mhud_reload,	pSettings->r_string(*hud_sect, "anim_reload"));
	animGet				(mhud_show,		pSettings->r_string(*hud_sect, "anim_draw"));
	animGet				(mhud_hide,		pSettings->r_string(*hud_sect, "anim_holster"));
	animGet				(mhud_shots,	pSettings->r_string(*hud_sect, "anim_shoot"));

	MediaLOAD	();
}

void CWeaponMagazined::FireStart		()
{
	if(dynamic_cast<CActor*>(H_Parent())) m_queueSize = 0;
	if(IsValid() && !IsMisfire()) 
	{
		if(!IsWorking())
		{
			if(STATE==eReload) return;
			if(STATE==eShowing) return;
			if(STATE==eHiding) return;
			if(STATE==eMisfire) return;

			inherited::FireStart();
			
			if (iAmmoElapsed == 0) 
				OnMagazineEmpty();
			else
				SwitchState(eFire);
		}
	} 
	else 
	{
		if(eReload!=STATE && eMisfire!=STATE) OnMagazineEmpty();
	}
}

void CWeaponMagazined::FireEnd() 
{
	m_shotNum = 0;
	
	if(IsWorking()) 
	{
		CWeapon::FireEnd();
		SwitchState(eIdle);
	}

	if(!iAmmoElapsed) Reload();
}

void CWeaponMagazined::Reload() 
{
	inherited::Reload();
	TryReload();
}

void CWeaponMagazined::TryReload() 
{
	if(m_pInventory) 
	{
		m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[m_ammoType],
											!dynamic_cast<CActor*>(H_Parent())));

		
		if(IsMisfire())
		{
			bPending = true;
			SwitchState(eReload); 
			return;
		}

		if(m_pAmmo) 
		{
			bPending = true;
			SwitchState(eReload); 
			return;
		} 
		else for(u32 i = 0; i < m_ammoTypes.size(); ++i) 
		{
			m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[i],
																   !dynamic_cast<CActor*>(H_Parent())));
			if(m_pAmmo) 
			{ 
				m_ammoType = i; 
				bPending = true;
				SwitchState(eReload); 
				return; 
			}
		}
	}
	SwitchState(eIdle);

	////if(m_pInventory) for(PPIItem l_ppIItem = m_pInventory->m_belt.begin(); m_pInventory->m_belt.end() != l_ppIItem; ++l_ppIItem) {
	////	//for(u32 i = 0; i < m_ammoTypes.size(); ++i) if(!strcmp((*l_ppIItem)->cName(), m_ammoTypes[i])) {
	////	if(!strcmp((*l_ppIItem)->cName(), m_ammoTypes[m_ammoType])) {
	////		SwitchState(eReload);
	////		return;
	////	}
	////}
	////SwitchState(eIdle);
	//if(m_pAmmo && m_pAmmo->CanReload()) SwitchState(eReload);
	//else SwitchState(eIdle);
}

bool CWeaponMagazined::IsAmmoAvailable()
{
	if (dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[m_ammoType],!dynamic_cast<CActor*>(H_Parent()))))
		return	(true);
	else
		for(u32 i = 0; i < m_ammoTypes.size(); ++i)
			if (dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[i],!dynamic_cast<CActor*>(H_Parent()))))
				return	(true);
	return		(false);
}

void CWeaponMagazined::OnMagazineEmpty() 
{
	bPending = true;
	SwitchState(eMagEmpty); 
	inherited::OnMagazineEmpty();
}

void CWeaponMagazined::UnloadMagazine() 
{
	xr_map<LPCSTR, u16> l_ammo;
	
	while(!m_magazine.empty()) 
	{
		CCartridge &l_cartridge = m_magazine.top();
		xr_map<LPCSTR, u16>::iterator l_it;
		for(l_it = l_ammo.begin(); l_ammo.end() != l_it; ++l_it) 
		{
            if(!strcmp(*l_cartridge.m_ammoSect, l_it->first)) 
            { 
				 ++(l_it->second); 
				 break; 
			}
		}

		if(l_it == l_ammo.end()) l_ammo[*l_cartridge.m_ammoSect] = 1;
		m_magazine.pop(); 
		--iAmmoElapsed;
	}
	
	xr_map<LPCSTR, u16>::iterator l_it;
	for(l_it = l_ammo.begin(); l_ammo.end() != l_it; ++l_it) 
	{
		CWeaponAmmo *l_pA = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(l_it->first,
														!dynamic_cast<CActor*>(H_Parent())));
		if(l_pA) 
		{
			u16 l_free = l_pA->m_boxSize - l_pA->m_boxCurr;
			l_pA->m_boxCurr = l_pA->m_boxCurr + (l_free < l_it->second ? l_free : l_it->second);
			l_it->second = l_it->second - (l_free < l_it->second ? l_free : l_it->second);
		}
		if(l_it->second) SpawnAmmo(l_it->second, l_it->first);
	}
}

void CWeaponMagazined::ReloadMagazine() 
{
	SwitchState(eIdle);

	//��������� ������ ��� �����������
	if(IsMisfire())	bMisfire = false;
	
	//���������� ��������� �������������
	//������ ������ ����� ��������
	static bool l_lockType = false;
	if(!l_lockType) m_ammoName = NULL;
	
	if(m_pInventory) 
	{
		//���������� ����� � ��������� ������� �������� ���� 
		m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[m_ammoType],
														   !dynamic_cast<CActor*>(H_Parent())));
		
		if(!m_pAmmo && !l_lockType) 
		{
			for(u32 i = 0; i < m_ammoTypes.size(); ++i) 
			{
				//��������� ������� ���� ���������� �����
				m_pAmmo = dynamic_cast<CWeaponAmmo*>(m_pInventory->Get(*m_ammoTypes[i],
													!dynamic_cast<CActor*>(H_Parent())));
				if(m_pAmmo) 
				{ 
					m_ammoType = i; 
					break; 
				}
			}
		}
	}

	//��� �������� ��� �����������
	if(!m_pAmmo) return;

	//��������� �������, ���� ��������� ��������� ������� ����
	if(!l_lockType && !m_magazine.empty() && 
		(!m_pAmmo || strcmp(m_pAmmo->cNameSect(), 
					 *m_magazine.top().m_ammoSect)))
		UnloadMagazine();
	
	CCartridge l_cartridge;
	while(iAmmoElapsed < iMagazineSize && m_pAmmo->Get(l_cartridge)) 
	{
		++iAmmoElapsed;
		m_magazine.push(l_cartridge);
	}
	m_ammoName = m_pAmmo->m_nameShort;

	//�������� ������� ��������, ���� ��� ������
	if(!m_pAmmo->m_boxCurr) m_pAmmo->Drop();

	if(iMagazineSize > iAmmoElapsed) 
	{ 
		l_lockType = true; 
		ReloadMagazine(); 
		l_lockType = false; 
	}
}

void CWeaponMagazined::OnStateSwitch	(u32 S)
{
	switch (S)
	{
	case eIdle:
		bFlame			= FALSE;
		switch2_Idle	();
		break;
	case eFire:
		switch2_Fire	();
		break;
	case eFire2:
		switch2_Fire2	();
		break;
	case eMisfire:
		bFlame			= FALSE;
		break;
	case eMagEmpty:
		switch2_Empty	();
		break;
	case eReload:
		bFlame			= FALSE;
		switch2_Reload	();
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
	}
	STATE = S;
	if(Remote()) NEXT_STATE = S;
}

void CWeaponMagazined::UpdateCL			()
{
	inherited::UpdateCL	();
	float dt = Device.fTimeDelta;

	// cycle update
	switch (NEXT_STATE)
	{
	case eShowing:
	case eHiding:
	case eReload:
		PSkeletonAnimated		(m_pHUD->Visual())->Update();
	case eIdle:
		fTime			-=	dt;
		if (fTime<0)	fTime = 0;
		break;
	case eFire:			state_Fire		(dt);	break;
	case eMisfire:		state_Misfire	(dt);	break;
	case eMagEmpty:		state_MagEmpty	(dt);	break;
	case eHidden:		break;
	}

	UpdateSounds		();
}

void CWeaponMagazined::UpdateSounds	()
{
	// ref_sound positions
	if (sndShow.feedback || sndHide.feedback || sndShot.feedback || sndReload.feedback || sndEmptyClick.feedback)
	{
		UpdateFP					();

		if (sndShow.feedback)		sndShow.set_position		(vLastFP);
		if (sndHide.feedback)		sndHide.set_position		(vLastFP);
		if (sndShot.feedback)		sndShot.set_position		(vLastFP);
		if (sndReload.feedback)		sndReload.set_position		(vLastFP);
		if (sndEmptyClick.feedback)	sndEmptyClick.set_position	(vLastFP);
	}
}

void CWeaponMagazined::state_Fire	(float dt)
{
	UpdateFP				();

	fTime					-=dt;
	Fvector					p1, d; 
	p1.set(vLastFP);
	d.set(vLastFD);

	if(H_Parent()) 
		dynamic_cast<CEntity*>	(H_Parent())->g_fireParams	(p1,d);
	else 
		return;
	
	while (fTime<0)
	{
		bFlame			=	TRUE;
		fTime			+=	fTimeToFire;

		++m_shotNum;
		OnShot			();
		FireTrace		(p1,vLastFP,d);
	}

	UpdateSounds			();

	//��� �������� ��������� ���������, 
	//� ������� ������� �������� �����
	if(m_shotNum == m_queueSize) FireEnd();
}

void CWeaponMagazined::state_Misfire	(float /**dt/**/)
{
	UpdateFP				();
	OnEmptyClick			();
	SwitchState				(eIdle);
	
	bMisfire = true;

	UpdateSounds			();
}

void CWeaponMagazined::state_MagEmpty	(float /**dt/**/)
{
	UpdateFP				();
	OnEmptyClick			();
	SwitchState				(eIdle);
	UpdateSounds			();
}

void CWeaponMagazined::renderable_Render	()
{
	inherited::renderable_Render();
	UpdateXForm();
	
	CActor *pActor = dynamic_cast<CActor*>(H_Parent());
	
	if (pActor && pActor->HUDview() && m_pHUD)
	{ 
		// HUD render
		if(!m_pHUD->IsHidden())
		{
			::Render->set_Transform		(&m_pHUD->Transform());
			::Render->add_Visual		(m_pHUD->Visual());
		}
	}
	else if(!pActor || !pActor->HUDview())
	{
		// Actor render
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
	
	if (((eFire==STATE) || (eReload==STATE))&& bFlame) 
	{
		UpdateFP	();
		OnDrawFlame	();
	}
}

void CWeaponMagazined::SetDefaults	()
{
	CWeapon::SetDefaults		();
	////iAmmoElapsed				= 0;
}

void CWeaponMagazined::Hide		()
{
	// add shot effector
	SwitchState(eHiding);
}
void CWeaponMagazined::Show		()
{
	SwitchState(eShowing);
}

void CWeaponMagazined::FireShotmark(const Fvector &vDir, const Fvector &vEnd, Collide::rq_result& R, u16 target_material) 
{
	inherited::FireShotmark		(vDir, vEnd, R, target_material);
}

void CWeaponMagazined::MediaLOAD		()
{
	if (hFlames.size())		return;

/*	// flame textures
	LPCSTR S		= pSettings->r_string	(cNameSect(),"flame");
	u32 scnt		= _GetItemCount(S);
	string256		name;
	for (u32 i=0; i<scnt; ++i)
	{
		hFlames.push_back	(ref_shader());
		ShaderCreate		(hFlames.back(),"effects\\flame",_GetItem(S,i,name));
	}*/
}

void CWeaponMagazined::MediaUNLOAD	()
{
	for (u32 i=0; i<hFlames.size(); ++i)
		ShaderDestroy(hFlames[i]);
	hFlames.clear();
}

void CWeaponMagazined::OnShellDrop	()
{
	// shells
	/*
	CPSObject* PS				= xr_new<CPSObject>("weapons\\shells_generic",H_Parent()->Sector(),true);
	Fmatrix M;
	M.setHPB(PS->m_Emitter.m_ConeHPB.x, PS->m_Emitter.m_ConeHPB.y, PS->m_Emitter.m_ConeHPB.z);
	Fvector V; M.transform_dir(V,vLastFD);
	PS->m_Emitter.m_ConeDirection.set(V);
	PS->play_at_pos				(vLastSP);
	*/
}

void CWeaponMagazined::OnShot		()
{
	// Sound
	Sound->play_at_pos			(sndShot,H_Root(),vLastFP);

	// Camera
	if (hud_mode)	
	{
		CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
		if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(xr_new<CEffectorShot> (camMaxAngle,camRelaxSpeed));
		R_ASSERT				(S);
		S->Shot					(camDispersion);
	}
	
	// Animation
	PlayAnimShoot();
		
	// Flames
	fFlameTime					= .1f;
	
	// Shell Drop
	OnShellDrop					();
	
	// ����� �� ������
	if(m_pFlameParticles && !m_pFlameParticles->IsLooped()) StopFlameParticles();
	StartFlameParticles	();

	//��� �� ������
	StartSmokeParticles	();
}

void CWeaponMagazined::OnShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::rq_result& R)
{
//	Device.Statistic.TEST3.Begin();
	Sound->play_at_pos		(sndRicochet[Random.randI(SND_RIC_COUNT)], 0, vEnd,false);
//	Device.Statistic.TEST3.End();
	
	if (!R.O) 
	{
		// particles
		Fvector N,D;
		Fvector*	pVerts	= g_pGameLevel->ObjectSpace.GetStaticVerts();
		CDB::TRI*	pTri	= g_pGameLevel->ObjectSpace.GetStaticTris()+R.element;
		N.mknormal			(pVerts[pTri->verts[0]],pVerts[pTri->verts[1]],pVerts[pTri->verts[2]]);
		D.reflect			(vDir,N);
		
#pragma todo("Oles to Yura: replace 'CPSObject' with 'CParticlesObject'")
		/*
		IRender_Sector* S	= ::Render->getSector(pTri->sector);
		
		// smoke
		LPCSTR ps_gibs		= (Random.randI(5)==0)?"sparks_1":"stones";
		CPSObject* PS		= xr_new<CPSObject> (ps_gibs,S,true);
		PS->m_Emitter.m_ConeDirection.set(D);
		PS->play_at_pos		(vEnd);
		
		// stones
		PS					= xr_new<CPSObject> ("stones",S,true);
		PS->m_Emitter.m_ConeDirection.set(D);
		PS->play_at_pos		(vEnd);
		*/
//		IRender_Sector* S	= ::Render->getSector(pTri->sector);
		
		// smoke
//		LPCSTR ps_gibs		= (Random.randI(5)==0)?"sparks_1":"stones";
//		CParticlesObject* PS		= xr_new<CParticlesObject> (ps_gibs,S,true);
		//PS->m_Emitter.m_ConeDirection.set(D);
//		PS->SetTransform(XFORM());
//		PS->play_at_pos		(vEnd);

		// stones
		//PS					= xr_new<CParticlesObject> ("stones",S,true);
		//PS->m_Emitter.m_ConeDirection.set(D);
		//PS->play_at_pos		(vEnd);

		
	}
}

void CWeaponMagazined::OnEmptyClick	()
{
	Sound->play_at_pos	(sndEmptyClick,H_Root(),vLastFP);
	if (sndEmptyClick.feedback)
		sndEmptyClick.feedback->set_volume(.2f);
}
void CWeaponMagazined::OnAnimationEnd() 
{
	switch(STATE) 
	{
		case eReload:	ReloadMagazine();		break;	// End of reload animation
		case eHiding:	SwitchState(eHidden);   break;	// End of Hide
		case eShowing:	SwitchState(eIdle);		break;	// End of Show
		case eIdle:		switch2_Idle();			break;  // Keep showing idle
	}
}
void CWeaponMagazined::switch2_Idle	()
{
	bPending = false;
	PlayAnimIdle();
}
void CWeaponMagazined::switch2_Fire	()
{
}
void CWeaponMagazined::switch2_Empty()
{
}
void CWeaponMagazined::switch2_Reload()
{
	Sound->play_at_pos		(sndReload,H_Root(),vLastFP);
	
	if (sndReload.feedback)	sndReload.feedback->set_volume(.2f);
	
	PlayAnimReload();
	bPending = true;
}
void CWeaponMagazined::switch2_Hiding()
{
	CWeapon::FireEnd();
	
	Sound->play_at_pos(sndHide,H_Root(),vLastFP);
	if (sndHide.feedback) sndHide.feedback->set_volume(.2f);
	

	PlayAnimHide();
	bPending = true;

	if (Local()) Level().Cameras.RemoveEffector	(cefShot);
}
void CWeaponMagazined::switch2_Hidden()
{
	signal_HideComplete		();
}
void CWeaponMagazined::switch2_Showing()
{
	Sound->play_at_pos		(sndShow,H_Root(),vLastFP);
	if (sndShow.feedback)	sndShow.feedback->set_volume(.3f);

	bPending = true;
	PlayAnimShow();
}

bool CWeaponMagazined::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;

	switch(cmd) 
	{
	case kWPN_RELOAD:
		{
			if(flags&CMD_START) 
				if(iAmmoElapsed < iMagazineSize || IsMisfire()) 
					Reload();
					//TryReload();
					//if(m_pAmmo && m_pAmmo->CanReload()) m_pAmmo->Reload();
		} 
		return true;
	}
	return false;
}

bool CWeaponMagazined::CanAttach(PIItem pIItem)
{
	CScope* pScope = dynamic_cast<CScope*>(pIItem);
	CSilencer* pSilencer = dynamic_cast<CSilencer*>(pIItem);
	
	if(pScope &&
	   m_eScopeStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope) == 0 &&
	   !strcmp(*m_sScopeName, pIItem->cNameSect()))
       return true;
	else if(pSilencer &&
	   m_eSilencerStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) == 0 &&
	   !strcmp(*m_sSilencerName, pIItem->cNameSect()))
       return true;
	else
		return inherited::CanAttach(pIItem);
}

bool CWeaponMagazined::CanDetach(const char* item_section_name)
{
	if( m_eScopeStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope) &&
	   strcmp(*m_sScopeName, item_section_name))
       return true;
	else if(m_eSilencerStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) &&
	   strcmp(*m_sSilencerName, item_section_name))
       return true;
	else
		return inherited::CanDetach(item_section_name);
}

bool CWeaponMagazined::Attach(PIItem pIItem)
{
	bool result = false;

	CScope* pScope = dynamic_cast<CScope*>(pIItem);
	CSilencer* pSilencer = dynamic_cast<CSilencer*>(pIItem);
	
	if(pScope &&
	   m_eScopeStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope) == 0 &&
	   !strcmp(*m_sScopeName, pIItem->cNameSect()))
	{
		m_flagsAddOnState |= CSE_ALifeItemWeapon::eWeaponAddonScope;
		result = true;
	}
	else if(pSilencer &&
	   m_eSilencerStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) == 0 &&
	   !strcmp(*m_sSilencerName, pIItem->cNameSect()))
	{
		m_flagsAddOnState |= CSE_ALifeItemWeapon::eWeaponAddonSilencer;
		result = true;
	}

	if(result)
	{
 		//���������� �������������� ���� �� ���������
		pIItem->Drop();
		NET_Packet P;
		u_EventGen(P,GE_DESTROY,pIItem->ID());
		P.w_u16(u16(pIItem->ID()));
		u_EventSend(P);

		UpdateAddonsVisibility();

		return true;
	}
	else
        return inherited::Attach(pIItem);
}


bool CWeaponMagazined::Detach(const char* item_section_name)
{
	if(m_eScopeStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonScope) &&
	   !strcmp(*m_sScopeName, item_section_name))
	{
		m_flagsAddOnState &= ~CSE_ALifeItemWeapon::eWeaponAddonScope;
		UpdateAddonsVisibility();

		return CInventoryItem::Detach(item_section_name);
	}
	else if(m_eSilencerStatus == CSE_ALifeItemWeapon::eAddonAttachable &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonSilencer) &&
	   !strcmp(*m_sSilencerName, item_section_name))
	{
		m_flagsAddOnState &= ~CSE_ALifeItemWeapon::eWeaponAddonSilencer;

		UpdateAddonsVisibility();
		return CInventoryItem::Detach(item_section_name);
	}
	else
		return inherited::Detach(item_section_name);;
}


//����������� ������� ��� ������������ �������� HUD
void CWeaponMagazined::PlayAnimShow()
{
	m_pHUD->animPlay(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
}

void CWeaponMagazined::PlayAnimHide()
{
	m_pHUD->animPlay (mhud_hide[Random.randI(mhud_hide.size())],FALSE,this);
}

void CWeaponMagazined::PlayAnimReload()
{
	m_pHUD->animPlay(mhud_reload[Random.randI(mhud_reload.size())],FALSE,this);
}

void CWeaponMagazined::PlayAnimIdle()
{
	m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
}
void CWeaponMagazined::PlayAnimShoot()
{
	m_pHUD->animPlay(mhud_shots[Random.randI(mhud_shots.size())],TRUE,this);
}