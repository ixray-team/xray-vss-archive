//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "UI_Main.h"
#include "leftbar.h"
#include "EditObject.h"
#include "xr_tokens.h"
#include "PropertiesList.h"
#include "motion.h"
#include "bone.h"
#include "BodyInstance.h"
#include "fmesh.h"
//---------------------------------------------------------------------------

bool CActorTools::EngineModel::UpdateGeometryStream(CEditableObject* source)
{
	m_GeometryStream.clear();
	return (source&&source->PrepareSVGeometry(m_GeometryStream));
}

bool CActorTools::EngineModel::UpdateMotionsStream(CEditableObject* source)
{
	m_MotionsStream.clear();
	return (source&&source->PrepareSVMotions(m_MotionsStream));
}

bool CActorTools::EngineModel::UpdateVisual(CEditableObject* source, bool bUpdGeom, bool bUpdMotions)
{
	if (bUpdGeom) 		UpdateGeometryStream(source);
	if (bUpdMotions) 	UpdateMotionsStream(source);
	if (!m_GeometryStream.size()||!m_MotionsStream.size()) return false;
	CFS_Memory F;
    F.write(m_GeometryStream.pointer(),m_GeometryStream.size());
    F.write(m_MotionsStream.pointer(),m_MotionsStream.size());
    CStream R(F.pointer(), F.size());
	Device.Models.Delete(m_pVisual);
    m_pVisual = Device.Models.Create(&R);
    m_pBlend = 0;
    return true;
}
//---------------------------------------------------------------------------

void CActorTools::EngineModel::Render(const Fmatrix& mTransform)
{
    // render visual
    Device.SetTransform	(D3DTS_WORLD,mTransform);
    switch (m_pVisual->Type)
    {
    case MT_SKELETON:{
        CKinematics* pV					= (CKinematics*)m_pVisual;
        vector<CVisual*>::iterator I,E;
        I = pV->chields.begin			();
        E = pV->chields.end				();
        for (; I!=E; I++)
        {
            CVisual* V					= *I;
            Device.Shader.set_Shader	(V->hShader);
            V->Render					(m_fLOD);
        }
    }break;
    case MT_HIERRARHY:{
        FHierrarhyVisual* pV			= (FHierrarhyVisual*)m_pVisual;
        vector<CVisual*>::iterator 		I,E;
        I = pV->chields.begin			();
        E = pV->chields.end				();
        for (; I!=E; I++)
        {
            CVisual* V					= *I;
            Device.Shader.set_Shader	(V->hShader);
            V->Render					(m_fLOD);
        }
    }break;
    default:
        Device.Shader.set_Shader		(m_pVisual->hShader);
        m_pVisual->Render				(m_fLOD);
        break;
    }
}
//---------------------------------------------------------------------------

void CActorTools::MotionModified(){
	m_bMotionModified = true;
	UI.Command(COMMAND_UPDATE_CAPTION);
    if (fraLeftBar->ebRenderEngineStyle->Down)
    	if (m_RenderObject.UpdateVisual(m_pEditObject,false,true))
        	PlayMotion();
}
//---------------------------------------------------------------------------

bool CActorTools::AppendMotion(LPCSTR name, LPCSTR fn)
{
	VERIFY(m_pEditObject);
    if (m_pEditObject->AppendSMotion(name,fn)){
		MotionModified();
		return true;
    }
	return false;
}

bool CActorTools::RemoveMotion(LPCSTR name)
{
	VERIFY(m_pEditObject);
    if (m_pEditObject->RemoveSMotion(name)){
		MotionModified();
		return true;
    }
	return false;
}

bool CActorTools::LoadMotions(LPCSTR name)
{
	VERIFY(m_pEditObject);
    if (m_pEditObject->LoadSMotions(name)){
		MotionModified();
		return true;
    }
	return false;
}

bool CActorTools::SaveMotions(LPCSTR name)
{
	VERIFY(m_pEditObject);
    return (m_pEditObject->SaveSMotions(name));
}

void CActorTools::MakePreview()
{
	if (m_pEditObject){
        CFS_Memory F;
    	if (m_RenderObject.UpdateVisual(m_pEditObject,true,true)){
            PlayMotion();
        }else{
        	m_RenderObject.DeleteVisual();
	        fraLeftBar->SetRenderStyle(false);
        }
    }else{
    	ELog.DlgMsg(mtError,"Scene empty. Load object first.");
    }
}

xr_token					tfx_token		[ ]={
	{ "Cycle",				0				},
	{ "FX",					1				},
	{ 0,					0				}
};

void CActorTools::MotionOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	R_ASSERT(edit_val);
    if (0==*(LPDWORD)edit_val){
	    m_pCycleNode->Hidden	= false;
    	m_pFXNode->Hidden		= true;
    }else{
	    m_pCycleNode->Hidden	= true;
    	m_pFXNode->Hidden		= false;
    }
}

void CActorTools::FillMotionProperties()
{
	R_ASSERT(m_pEditObject);
	CSMotion* SM = m_pEditObject->GetActiveSMotion();
    m_MotionProps->BeginFillMode();
    if (SM){
        m_MotionProps->AddItem(0,PROP_MARKER2, 		"Name", 	(LPVOID)SM->Name());
        m_MotionProps->AddItem(0,PROP_FLOAT, 		"Speed", 	m_MotionProps->MakeFloatValue(&SM->fSpeed,	0.f,20.f,0.01f,2));
        m_MotionProps->AddItem(0,PROP_FLOAT, 		"Accrue", 	m_MotionProps->MakeFloatValue(&SM->fAccrue,	0.f,20.f,0.01f,2));
        m_MotionProps->AddItem(0,PROP_FLOAT, 		"Falloff", 	m_MotionProps->MakeFloatValue(&SM->fFalloff,	0.f,20.f,0.01f,2));
        TokenValue* TV = m_MotionProps->MakeTokenValue(&SM->bFX,tfx_token,MotionOnAfterEdit);
        m_MotionProps->AddItem(0,PROP_TOKEN, 		"Type", 	TV);
        m_pCycleNode = m_MotionProps->AddItem(0,PROP_MARKER, "Cycle");
        {
            AStringVec lst;
            lst.push_back("--none--");
            for (BPIt it=m_pEditObject->FirstBonePart(); it!=m_pEditObject->LastBonePart(); it++) lst.push_back(it->alias);
            m_MotionProps->AddItem(m_pCycleNode,PROP_TOKEN2, 	"Bone part",m_MotionProps->MakeTokenValue2(&SM->iBoneOrPart,&lst));
            m_MotionProps->AddItem(m_pCycleNode,PROP_BOOL,	"Stop at end",&SM->bStopAtEnd);
        }
        m_pFXNode = m_MotionProps->AddItem(0,PROP_MARKER, "FX");
        {
            AStringVec lst;
            for (BoneIt it=m_pEditObject->FirstBone(); it!=m_pEditObject->LastBone(); it++) lst.push_back((*it)->Name());
            m_MotionProps->AddItem(m_pFXNode,PROP_TOKEN2,	"Start bone",m_MotionProps->MakeTokenValue2(&SM->iBoneOrPart,&lst));
            m_MotionProps->AddItem(m_pFXNode,PROP_FLOAT, 	"Power", 	m_MotionProps->MakeFloatValue(&SM->fPower,	0.f,20.f,0.01f,2));
        }
        MotionOnAfterEdit(m_pFXNode,TV,TV->val);
    }else{
		m_pCycleNode=0;
        m_pFXNode	=0;
    }
	m_MotionProps->EndFillMode();
}

void CActorTools::PlayMotion()
{
	if (m_pEditObject)
    	if (fraLeftBar->ebRenderEditorStyle->Down) m_pEditObject->SkeletonPlay();
        else if (fraLeftBar->ebRenderEngineStyle->Down) {
            CSMotion* M=m_pEditObject->GetActiveSMotion();
            if (M&&m_RenderObject.IsRenderable())
            	if (M->bFX) m_RenderObject.m_pBlend = PKinematics(m_RenderObject.m_pVisual)->PlayFX(M->Name());
                else		m_RenderObject.m_pBlend = PKinematics(m_RenderObject.m_pVisual)->PlayCycle(M->Name(),fraLeftBar->ebMixMotion->Down);
        }
}

void CActorTools::StopMotion()
{
	if (m_pEditObject)
    	if (fraLeftBar->ebRenderEditorStyle->Down) m_pEditObject->SkeletonStop();
        else if (fraLeftBar->ebRenderEngineStyle->Down&&m_RenderObject.m_pBlend) {
        	m_RenderObject.m_pBlend->playing	 = false;
        	m_RenderObject.m_pBlend->timeCurrent = 0;
        }
}

void CActorTools::PauseMotion()
{
	if (m_pEditObject)
    	if (fraLeftBar->ebRenderEditorStyle->Down) m_pEditObject->SkeletonPause();
        else if (fraLeftBar->ebRenderEngineStyle->Down&&m_RenderObject.m_pBlend) {
        	m_RenderObject.m_pBlend->playing=!m_RenderObject.m_pBlend->playing;
        }
}

bool CActorTools::RenameMotion(LPCSTR old_name, LPCSTR new_name)
{
	R_ASSERT(m_pEditObject);
	CSMotion* M = m_pEditObject->FindSMotionByName(old_name);
    R_ASSERT(M);
	CSMotion* MN = m_pEditObject->FindSMotionByName(new_name);
    R_ASSERT(!MN);
    M->SetName(new_name);
    return true;
}

