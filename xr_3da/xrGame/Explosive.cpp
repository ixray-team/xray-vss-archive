// Explosive.cpp: ��������� ��� ������������� ��������
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "explosive.h"

#include "PhysicsShell.h"
#include "entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"

//��� ������ ����������� ������� ��������� ���������
#include "Weapon.h"


CExplosive::CExplosive(void) 
{
	m_fBlastHit = 50.f;
	m_fBlastRadius = 10.f;
	m_iFragsNum = 20;
	m_fFragsRadius = 30.f;
	m_fFragHit = 50;

	m_fUpThrowFactor = 0.f;

	m_eSoundExplode = ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);
	m_eSoundRicochet = ESoundTypes(0);
	m_eSoundCheckout = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING);

	m_pLight = ::Render->light_create();
	m_pLight->set_shadow(true);
	hWallmark = 0;
}

CExplosive::~CExplosive(void) 
{
	hWallmark.destroy		();

	::Render->light_destroy	(m_pLight);

	SoundDestroy(sndExplode);
	SoundDestroy(sndCheckout);
	SoundDestroy(sndRicochet[0]);
	SoundDestroy(sndRicochet[1]);
	SoundDestroy(sndRicochet[2]);
	SoundDestroy(sndRicochet[3]);
	SoundDestroy(sndRicochet[4]);
}

void CExplosive::Load(LPCSTR section) 
{
	m_fBlastHit = pSettings->r_float(section,"blast");
	m_fBlastRadius = pSettings->r_float(section,"blast_r");
	m_iFragsNum = pSettings->r_s32(section,"frags");
	m_fFragsRadius = pSettings->r_float(section,"frags_r");
	m_fFragHit = pSettings->r_float(section,"frag_hit");

	m_fUpThrowFactor = pSettings->r_float(section,"up_throw_factor");

	LPCSTR	name = pSettings->r_string(section,"wm_name");
	pstrWallmark = name;
	fWallmarkSize = pSettings->r_float(section,"wm_size");

	string256 m_effectsSTR;
	strcpy(m_effectsSTR, pSettings->r_string(section,"effects"));
	char* l_effectsSTR = m_effectsSTR; R_ASSERT(l_effectsSTR);
	m_effects.clear(); m_effects.push_back(l_effectsSTR);
	
	while(*l_effectsSTR) 
	{
		if(*l_effectsSTR == ',') 
		{
			*l_effectsSTR = 0; ++l_effectsSTR;
			while(*l_effectsSTR == ' ' || *l_effectsSTR == '\t') ++l_effectsSTR;
			m_effects.push_back(l_effectsSTR);
		}
		++l_effectsSTR;
	}

	sscanf(pSettings->r_string(section,"light_color"), "%f,%f,%f", &m_lightColor.r, &m_lightColor.g, &m_lightColor.b);
	m_lightRange	= pSettings->r_float(section,"light_range");
	m_lightTime		= iFloor(pSettings->r_float(section,"light_time")*1000.f);

	//������ ��� ������� ��������
	tracerHeadSpeed		= pSettings->r_float		(section,"tracer_head_speed"	);
	tracerTrailCoeff	= pSettings->r_float		(section,"tracer_trail_scale"	);
	tracerStartLength	= pSettings->r_float		(section,"tracer_start_length"	);
	tracerWidth			= pSettings->r_float		(section,"tracer_width"			);


	SoundCreate(sndExplode, "explode", m_eSoundExplode);
	SoundCreate(sndCheckout, "checkout", m_eSoundCheckout);
	SoundCreate(sndRicochet[0], "ric1", m_eSoundRicochet);
	SoundCreate(sndRicochet[1], "ric2", m_eSoundRicochet);
	SoundCreate(sndRicochet[2], "ric3", m_eSoundRicochet);
	SoundCreate(sndRicochet[3], "ric4", m_eSoundRicochet);
	SoundCreate(sndRicochet[4], "ric5", m_eSoundRicochet);
}

BOOL CExplosive::net_Spawn(LPVOID DC) 
{
	if (0==pstrWallmark) 
		hWallmark	= 0; 
	else 
		hWallmark.create		("effects\\wallmark",*pstrWallmark);
	
	return TRUE;
}

void CExplosive::net_Destroy() 
{
	hWallmark.destroy		();
}

/////////////////////////////////////////////////////////
// ����� 
/////////////////////////////////////////////////////////
void CExplosive::Explode() 
{
	setVisible(false);

	//������ ���� ������
	Sound->play_at_pos(sndExplode, 0, Position(), false);
	
	//���������� �������
	CParticlesObject* pStaticPG; 
	int l_c = (s32)m_effects.size();

	for(s32 i = 0; i < l_c; ++i) 
	{
		pStaticPG = xr_new<CParticlesObject>(*m_effects[i],Sector()); 
		pStaticPG->play_at_pos(Position());
	}


	
	//�������� ��������� �� ������
	m_pLight->set_position(Position()); 
	m_pLight->set_active(true);

	//trace frags
	Fvector frag_dir; 
	setEnabled(false);
	
	//////////////////////////////
	//�������
	//////////////////////////////
	for(int i = 0; i < m_iFragsNum; ++i) 
	{
		frag_dir.set(::Random.randF(-.5f,.5f), 
				  ::Random.randF(-.5f,.5f), 
				  ::Random.randF(-.5f,.5f)); 
		frag_dir.normalize();
			
		// ...and trace line
		m_vEndPoint.mad(Position(), frag_dir, m_fFragsRadius);

		
		m_fCurrentFireDist = m_fFragsRadius;
		m_fCurrentHitPower = m_fFragHit;
		m_fCurrentHitImpulse = m_fFragHit;
		m_pCurrentCartridge = NULL;
		m_vCurrentShootDir = frag_dir;
		m_vCurrentShootPos = Position();
		
		Collide::ray_defs RD(Position(), frag_dir, m_fFragsRadius, 0,Collide::rqtBoth);
		Level().ObjectSpace.RayQuery(RD, firetrace_callback, dynamic_cast<CShootingObject*>(this));

		// �������� �������� ��� ������ ��������
		Level().Tracers.Add	(m_vCurrentShootPos,m_vEndPoint,tracerHeadSpeed,
							 tracerTrailCoeff,tracerStartLength,tracerWidth);
/*
		if(Level().ObjectSpace.RayPick(Position(), l_dir, m_fFragsRadius, Collide::rqtBoth, RQ)) 
		{
			Fvector l_end, l_bs_pos; 
			l_end.mad(Position(),l_dir,RQ.range); 
			l_bs_pos.set(0, 0, 0);
	

			if(RQ.O && Local()) 
			{
				f32 l_hit = m_fFragHit * (1.f - (RQ.range/m_fFragsRadius)*(RQ.range/m_fFragsRadius));
				CEntity* E = dynamic_cast<CEntity*>(RQ.O);
				if(E) l_hit *= E->HitScale(RQ.element);
				NET_Packet		P;
				u_EventGen		(P,GE_HIT,RQ.O->ID());
				P.w_u16			(u16(ID()));
				P.w_dir			(l_dir);
				P.w_float		(l_hit);
				P.w_s16			((s16)RQ.element);
				P.w_vec3		(l_bs_pos);
				P.w_float		(l_hit/(E?E->HitScale(RQ.element):1.f));
				P.w_u16			(eHitTypeWound);
				u_EventSend		(P);
			}
			FragWallmark(l_dir, l_end, RQ);
		}*/
	}	
	setEnabled(true);

	if (Remote()) return;
	
	/////////////////////////////////
	//�������� �����
	////////////////////////////////

	Fvector l_goPos; 
	Fvector l_dir; 
	float l_dst;

	m_blasted.clear();
	feel_touch_update(Position(), m_fBlastRadius);
	
	xr_list<s16>		l_elements;
	xr_list<Fvector>	l_bs_positions;
	
	while(m_blasted.size()) 
	{
		CGameObject *l_pGO = *m_blasted.begin();
		
		if(l_pGO->Visual()) 
			l_pGO->Center(l_goPos); 
		else 
			l_goPos.set(l_pGO->Position());
		
		l_dir.sub(l_goPos, Position()); 
		l_dst = l_dir.magnitude(); 
		l_dir.div(l_dst); 
		l_dir.y += m_fUpThrowFactor;
		//l_dir.normalize();
		f32 l_S = (l_pGO->Visual()?l_pGO->Radius()*l_pGO->Radius():0);
		
		if(l_pGO->Visual()) 
		{
			const Fbox &l_b1 = l_pGO->BoundingBox(); 
			Fbox l_b2; 
			l_b2.invalidate();
			Fmatrix l_m; l_m.identity(); 
			l_m.k.set(l_dir); 
			Fvector::generate_orthonormal_basis(l_m.k, l_m.i, l_m.j);
			
			for(int i = 0; i < 8; ++i) 
			{ 
				Fvector l_v; 
				l_b1.getpoint(i, l_v); 
				l_m.transform_tiny(l_v); 
				l_b2.modify(l_v); 
			}
			
			Fvector l_c, l_d; 
			l_b2.get_CD(l_c, l_d);
			l_S = l_d.x*l_d.y;
		}
		
		float l_impuls = m_fBlastHit * (1.f - (l_dst/m_fBlastRadius)*(l_dst/m_fBlastRadius)) * l_S;

		if(l_impuls > .001f) 
		{
			setEnabled(false);
			l_impuls *= l_pGO->ExplosionEffect(Position(), m_fBlastRadius, l_elements, l_bs_positions);
			setEnabled(true);
		}
		if(l_impuls > .001f) 
		{
			while(l_elements.size()) 
			{
				s16 l_element = *l_elements.begin();
				Fvector l_bs_pos = *l_bs_positions.begin();
				NET_Packet		P;
				u_EventGen		(P,GE_HIT,l_pGO->ID());
				P.w_u16			(u16(ID()));
				P.w_dir			(l_dir);
				P.w_float		(l_impuls);
				P.w_s16			(l_element);
				P.w_vec3		(l_bs_pos);
				P.w_float		(l_impuls);
				P.w_u16			(u16(eHitTypeWound));
				u_EventSend		(P);
				l_elements.pop_front();
				l_bs_positions.pop_front();
			}
		}
		m_blasted.pop_front();
	}	
}

void CExplosive::FragWallmark	(const Fvector& vDir, const Fvector &vEnd, Collide::rq_result& R) 
{
	if (!hWallmark)	return;
	
	if (R.O) 
	{
		if (R.O->CLS_ID==CLSID_ENTITY)
		{
#pragma todo("Oles to Yura: replace 'CPSObject' with 'CParticlesObject'")
			/*
			IRender_Sector* S	= R.O->Sector();
			Fvector D;	D.invert(vDir);

			LPCSTR ps_gibs		= "blood_1";//(Random.randI(5)==0)?"sparks_1":"stones";
			CPSObject* PS		= xr_new<CPSObject> (ps_gibs,S,true);
			PS->m_Emitter.m_ConeDirection.set(D);
			PS->play_at_pos		(vEnd);
//			*/
		}
	} 
	else 
	{
		R_ASSERT(R.element >= 0);
		::Render->add_Wallmark	(
			hWallmark,
			vEnd,
			fWallmarkSize,
			g_pGameLevel->ObjectSpace.GetStaticTris()+R.element,
			g_pGameLevel->ObjectSpace.GetStaticVerts()
			);
	}

	Sound->play_at_pos(sndRicochet[Random.randI(SND_RIC_COUNT)], 0, vEnd,false);
	
	if (!R.O) 
	{
/*		// particles
		Fvector N,D;
		CDB::TRI* pTri		= g_pGameLevel->ObjectSpace.GetStaticTris()+R.element;
		N.mknormal			(pTri->V(0),pTri->V(1),pTri->V(2));
		D.reflect			(vDir,N);
		
#pragma todo("Oles to Yura: replace 'CPSObject' with 'CParticlesObject'")
	
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
	}
}

void CExplosive::feel_touch_new(CObject* O) 
{
	CGameObject *pGameObject = dynamic_cast<CGameObject*>(O);
	if(pGameObject && this != pGameObject) m_blasted.push_back(pGameObject);
}

void CExplosive::UpdateCL() 
{
	if(m_expoldeTime > 0 && m_expoldeTime <= Device.dwTimeDelta) 
	{
		m_expoldeTime = 0;
		m_pLight->set_active(false);
		
		//������������� ��� ������ 
		NET_Packet			P;
		u_EventGen			(P,GE_DESTROY,ID());
		if (Local()) u_EventSend			(P);
	} 
	else if(m_expoldeTime>0 && m_expoldeTime < 0xffffffff) 
	{
		m_expoldeTime -= Device.dwTimeDelta;
		
		if(m_expoldeTime > (EXPLODE_TIME_MAX - m_lightTime)) 
		{
			f32 l_scale = f32(m_expoldeTime - (EXPLODE_TIME_MAX - m_lightTime))/f32(m_lightTime);
			m_pLight->set_color(m_lightColor.r*l_scale, 
								m_lightColor.g*l_scale, 
								m_lightColor.b*l_scale);
			
			m_pLight->set_range(m_lightRange*l_scale);
		} 
//		else m_pLight->set_range(0);
	} 
	else m_pLight->set_active(false);
}

void CExplosive::SoundCreate(ref_sound& dest, LPCSTR s_name, 
						   int iType, BOOL /**bCtrlFreq/**/) 
{
	string256	name,temp;
	strconcat	(name,"weapons\\",Name(),"_",s_name,".ogg");

	if (FS.exist(temp,"$game_sounds$",name)) 
	{
		dest.create		(TRUE,name,iType);
		return;
	}

	strconcat	(name,"weapons\\","generic_",s_name,".ogg");

	if (FS.exist(temp,"$game_sounds$",name))	
	{
		dest.create		(TRUE,name,iType);
		return;
	}
	Debug.fatal	("Can't find sound '%s' for weapon '%s'", name, Name());
}

void CExplosive::SoundDestroy(ref_sound& dest) 
{
	dest.destroy();
}