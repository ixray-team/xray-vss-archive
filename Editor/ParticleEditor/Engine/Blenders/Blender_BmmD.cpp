// BlenderDefault.cpp: implementation of the CBlender_BmmD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "blender_BmmD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_BmmD::CBlender_BmmD	()
{
	description.CLS		= B_BmmD;
	strcpy				(oT2_Name,	"$null");
	strcpy				(oT2_xform,	"$null");
}

CBlender_BmmD::~CBlender_BmmD	()
{
	
}

void	CBlender_BmmD::Save		(CFS_Base& FS )
{
	CBlender::Save	(FS);
	xrPWRITE_MARKER	(FS,"Detail map");
	xrPWRITE_PROP	(FS,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(FS,"Transform",		xrPID_MATRIX,	oT2_xform);
}

void	CBlender_BmmD::Load		(CStream& FS, WORD version )
{
	CBlender::Load	(FS,version);
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(FS,xrPID_MATRIX,	oT2_xform);
}

void	CBlender_BmmD::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	if (C.bEditor)	{
		C.RS.PassBegin		();
		{
			C.RS.PassSET_ZB		(TRUE,TRUE);
			C.RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
			C.RS.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Base texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.RS.Stage_Texture		(oT_Name,		C.L_textures		);
				C.RS.Stage_Matrix		(oT_xform,		C.L_matrices,		0);
				C.RS.Stage_Constant		("$null",		C.L_constants		);
			}
			C.RS.StageEnd			();
			
			// Stage2 - Second texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_CURRENT);
				C.RS.Stage_Texture		(oT2_Name,		C.L_textures		);
				C.RS.Stage_Matrix		(oT2_xform,		C.L_matrices,		0);
				C.RS.Stage_Constant		("$null",		C.L_constants		);
			}
			C.RS.StageEnd			();
		}
		C.RS.PassEnd			();
	} else {
		C.RS.PassBegin		();
		{
			C.RS.PassSET_ZB		(TRUE,TRUE);
			C.RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
			C.RS.PassSET_LightFog	(FALSE,TRUE);
			
			// Stage1 - Base texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.RS.Stage_Texture		(oT_Name,		C.L_textures		);
				C.RS.Stage_Matrix		(oT_xform,		C.L_matrices,		0);
				C.RS.Stage_Constant		("$null",		C.L_constants		);
			}
			C.RS.StageEnd			();
			
			// Stage2 - Second texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_CURRENT);
				C.RS.Stage_Texture		(oT2_Name,		C.L_textures		);
				C.RS.Stage_Matrix		(oT2_xform,		C.L_matrices,		0);
				C.RS.Stage_Constant		("$null",		C.L_constants		);
			}
			C.RS.StageEnd			();
		}
		C.RS.PassEnd			();
	}
}
