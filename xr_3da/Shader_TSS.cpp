// Shader_TSS.cpp: implementation of the CShader_TSS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShader_TSS::CShader_TSS()
{

}

CShader_TSS::~CShader_TSS()
{

}

//////////////////////////////////////////////////////////////////////
void	CShader_TSS_Pass::Release		()
{
	Stages.clear	();
}
