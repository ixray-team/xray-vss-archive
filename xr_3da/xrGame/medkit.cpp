///////////////////////////////////////////////////////////////
// Medkit.cpp
// Medkit - �������, ���������� ��������
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "medkit.h"
#include "PhysicsShell.h"

CMedkit::CMedkit(void) 
{
}

CMedkit::~CMedkit(void) 
{
}

BOOL CMedkit::net_Spawn(LPVOID DC) 
{
	return		(inherited::net_Spawn(DC));
}

void CMedkit::Load(LPCSTR section) 
{
	inherited::Load(section);
}

void CMedkit::net_Destroy() 
{
	inherited::net_Destroy();
}

void CMedkit::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);

}

void CMedkit::UpdateCL() 
{
	inherited::UpdateCL();
}


void CMedkit::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CMedkit::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

void CMedkit::renderable_Render() 
{
	if(getVisible() && !H_Parent()) 
	{
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}