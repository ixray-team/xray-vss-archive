//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "d3dutils.h"
#include "ParticleGroup.h"
#include "engine/particles/papi.h"
#include "engine/particles/general.h"
#include "TLSprite.h"

CParticleGroup::CParticleGroup()
{
	m_HandleGroup 		= pGenParticleGroups(1, 100);
    m_HandleActionList	= pGenActionLists();
    m_Shader			= 0;
    m_ShaderName		= 0;
    m_TextureName		= 0;
}
CParticleGroup::~CParticleGroup()
{
	OnDeviceDestroy		();
	pDeleteParticleGroups(m_HandleGroup);
	pDeleteActionLists	(m_HandleActionList);
}

void CParticleGroup::OnDeviceCreate()
{
	if (m_ShaderName&&m_TextureName)
    	m_Shader 		= Device.Shader.Create(m_ShaderName,m_TextureName);
}

void CParticleGroup::OnDeviceDestroy()
{
	Device.Shader.Delete(m_Shader);
}   

void CParticleGroup::pSprite(LPCSTR sh_name, LPCSTR tex_name)
{
	OnDeviceDestroy		();
    xr_free(m_ShaderName);	m_ShaderName	= xr_strdup(sh_name);
    xr_free(m_TextureName);	m_TextureName	= xr_strdup(tex_name);
	OnDeviceCreate		();
}

// action
void CParticleGroup::pFrame(BOOL random_frame, u32 frame_count, u32 tex_width, u32 tex_height, u32 frame_width, u32 frame_height)
{
	m_Flags.set			(flFramed,TRUE);
	m_Flags.set			(flRandomFrame,random_frame);
	m_Animation.Set		(frame_count, tex_width, tex_height, frame_width, frame_height);
}
void CParticleGroup::pAnimate(float speed, BOOL random_playback)
{
	m_Animation.m_fSpeed= speed;
	m_Flags.set			(flAnimated,TRUE);
	m_Flags.set			(flRandomPlayback,random_playback);
}
void CParticleGroup::pFrameExec(Particle *m)
{
    if (m_Flags.test(flRandomFrame))
    	m->frame	= Random.randI(m_Animation.m_iFrameCount);
    if (m_Flags.test(flAnimated)&&m_Flags.test(flRandomPlayback)&&Random.randI(2))	
    	m->flags.set(Particle::ANIMATE_CCW,TRUE);
}
void CParticleGroup::pAnimateExec(ParticleGroup *group)
{
	float speedFac = m_Animation.m_fSpeed * Device.fTimeDelta;
	for(int i = 0; i < group->p_count; i++){
		Particle &m = group->list[i];
		m.frame						+= ((m.flags.is(Particle::ANIMATE_CCW))?-1.f:1.f)*speedFac;
		if (m.frame>m_Animation.m_iFrameCount)	m.frame-=m_Animation.m_iFrameCount;
		if (m.frame<0.f)						m.frame+=m_Animation.m_iFrameCount;
	}
}

static float phase = 0.f;

void CParticleGroup::OnFrame()
{
    pTimeStep			(Device.fTimeDelta);
	pCurrentGroup		(m_HandleGroup);

    Fmatrix M;
    phase				+= 0.5f*Device.fTimeDelta;
	if (phase>10) phase = -10;
    M.translate			(phase,0,0);
//    M.rotateY			(phase);
//    M.translate_over	(10*sin(phase),0,10*cos(phase));
//    Log("M.c: ",M.c);
    pSetActionListTransform(m_HandleActionList,M);
//	Fvector src;
    
//	PASource* source	= ;
    
    // execute action list
	pCallActionList		(m_HandleActionList);
    // 
	ParticleGroup *pg = _GetGroupPtr(m_HandleGroup);
    if (m_Flags.is(flFramed)&&m_Flags.is(flAnimated)) 
    	pAnimateExec(pg);

	for(int i = 0; i < pg->p_count; i++){
        Particle &m 	= pg->list[i];
        if (m.flags.is(Particle::DYING)){}
        if (m.flags.is(Particle::BIRTH)){
	        if (m_Flags.is(flFramed)) pFrameExec(&m);
			m.flags.set(Particle::BIRTH,FALSE);
        }
	}
}
          
void CParticleGroup::Render()
{
	// Get a pointer to the particles in gp memory
	ParticleGroup *pg = _GetGroupPtr(m_HandleGroup);

	if(pg == NULL)		return; // ERROR
	if(pg->p_count < 1)	return;

	Device.SetShader	(m_Shader);
    Device.SetTransform	(D3DTS_WORLD,Fidentity);
    CTLSprite 			m_Sprite;
    for(int i = 0; i < pg->p_count; i++){
        Particle &m = pg->list[i];

        Fvector p;
        Fcolor c;
        p.set(m.pos.x,m.pos.y,m.pos.z);
        c.set(m.color.x,m.color.y,m.color.z,m.alpha);
		if (m_Flags.is(flFramed)){
//        	||m_Flags.test(flAnimated)){
			Fvector2 lt,rb;
        	m_Animation.CalculateTC(m.frame,lt,rb);
			m_Sprite.Render(p,c.get(),m.size.x,m.rot.x,lt,rb);
        }else
			m_Sprite.Render(p,c.get(),m.size.x,m.rot.x);
//		DU::DrawCross(p,m.size.x,m.size.y,m.size.z,m.size.x,m.size.y,m.size.z,c.get(),false);
    }
}

