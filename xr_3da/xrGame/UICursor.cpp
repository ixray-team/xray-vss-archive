// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:11:58 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "uicursor.h"

#include "..\CustomHUD.h"

#define C_DEFAULT	D3DCOLOR_XRGB(0xff,0xff,0xff)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUICursor::CUICursor()
{    
	vPos.set		(0.f,0.f);

	hVS			= Device.Shader.CreateGeom	(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
	hShader		= Device.Shader.Create		("hud\\cursor","ui\\ui_cursor",FALSE);
}
//--------------------------------------------------------------------
CUICursor::~CUICursor	()
{
	Device.Shader.Delete	(hShader);
	Device.Shader.DeleteGeom(hVS);
}
//--------------------------------------------------------------------
void CUICursor::Render	()
{
	_VertexStream*	Stream	= &RCache.Vertex; 
	// actual rendering
	u32			vOffset;
	FVF::TL*		pv	= (FVF::TL*)Stream->Lock(4,hVS->vb_stride,vOffset);
	float			size= 2 * Device.dwWidth * 0.015f;
	
	// Convert to screen coords
	float cx					= (vPos.x+1)*(Device.dwWidth/2);
	float cy					= (vPos.y+1)*(Device.dwHeight/2);
	
	u32 C						= C_DEFAULT;

	pv->set(cx, cy+size,		.0001f,.9999f, C, 0, 1); pv++;
	pv->set(cx, cy,				.0001f,.9999f, C, 0, 0); pv++;
	pv->set(cx+size, cy+size,	.0001f,.9999f, C, 1, 1); pv++;
	pv->set(cx+size, cy,		.0001f,.9999f, C, 1, 0); pv++;
	
	// unlock VB and Render it as triangle list
	Stream->Unlock					(4,hVS->vb_stride);
	RCache.set_Shader		(hShader);
	RCache.set_Geometry		(hVS);
	RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,4,0,2);
}
//--------------------------------------------------------------------
