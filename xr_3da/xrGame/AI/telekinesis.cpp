#include "stdafx.h"
#include "telekinesis.h"
#include "../entity_alive.h"
#include "../PhysicsShell.h"

void	CTelekinesis::activate			(CPhysicsShellHolder *obj, float strength, float height, u32 max_time_keep)
{
	active = true;

	CTelekineticObject* tele_object=alloc_tele_object();		
	if (!tele_object->init(this,obj,strength, height,max_time_keep)) return;

	// �������� ������
	objects.push_back(tele_object);

	if (!objects.empty()) CPHUpdateObject::Activate();
}


void CTelekinesis::deactivate()
{
	active			= false;

	// ��������� ��� �������
	// 
	for (u32 i = 0; i < objects.size(); i++) 
	{

		objects[i]->release();
		xr_delete(objects[i]);
	}
	objects.clear	();

	CPHUpdateObject::Deactivate();
}

void CTelekinesis::clear_deactivate()
{
	active			= false;

	// ��������� ��� �������
	// 
	for (u32 i = 0; i < objects.size(); i++) 
	{

		objects[i]->switch_state(TS_None);
		xr_delete(objects[i]);
	}
	objects.clear	();

	CPHUpdateObject::Deactivate();
}

struct SFindPred
{
	CPhysicsShellHolder *obj;
	SFindPred(CPhysicsShellHolder *aobj){obj=aobj;}
	bool operator	() (CTelekineticObject * tele_obj)
	{
		return tele_obj->get_object()==obj;
	}
};
void CTelekinesis::deactivate(CPhysicsShellHolder *obj)
{
	// ����� ������
	
	TELE_OBJECTS_IT it = std::find_if(objects.begin(), objects.end(), SFindPred(obj));
	if (it == objects.end()) return;

	// ��������� ������
	(*it)->release();

	//remove from list, delete...
	remove_object(it);
}

void CTelekinesis::remove_object		(CPhysicsShellHolder *obj)
{
	// ����� ������
	TELE_OBJECTS_IT it = std::find_if(objects.begin(), objects.end(), SFindPred(obj));
	if (it == objects.end()) return;
	//remove from list, delete...
	remove_object(it);
}

void CTelekinesis::remove_object		(TELE_OBJECTS_IT it)
{
	// release memory
	xr_delete(*it);

	// �������
	objects.erase(it);

	// ��������� �� ������ �����������
	if (objects.empty()) {
		CPHUpdateObject::Deactivate();
		active = false;
	}
}
void CTelekinesis::fire(const Fvector &target)
{
	if (!active) return;

	for (u32 i = 0; i < objects.size(); i++) objects[i]->fire(target);

	deactivate();
}

void CTelekinesis::fire(CPhysicsShellHolder *obj, const Fvector &target)
{
	// ����� ������
	TELE_OBJECTS_IT it = std::find_if(objects.begin(), objects.end(),SFindPred(obj));
	if (it == objects.end()) return;

	// ������� ������
	(*it)->fire(target);
}

// ������� ������ 'obj' � ������� 'target' � ������ ���� ���� 
void CTelekinesis::fire(CPhysicsShellHolder *obj, const Fvector &target, float power)
{
	// ����� ������

	TELE_OBJECTS_IT it = std::find_if(objects.begin(), objects.end(),SFindPred(obj));
	if (it == objects.end()) return;

	// ������� ������
	(*it)->fire(target,power);
}


bool CTelekinesis::is_active_object(CPhysicsShellHolder *obj)
{
	// ����� ������
	TELE_OBJECTS_IT it = std::find_if(objects.begin(), objects.end(), SFindPred(obj));
	if (it == objects.end()) return false;

	return true;
}

void CTelekinesis::schedule_update()
{
	if (!active) return;

	// �������� ��������� ��������
	for (u32 i = 0; i < objects.size(); i++) {

		CTelekineticObject *cur_obj = objects[i];
		cur_obj->update_state();
		if(cur_obj->is_released())	remove_object(objects.begin()+i);
	}
}

void CTelekinesis::PhDataUpdate(dReal step)
{
	if (!active) return;

	for (u32 i = 0; i < objects.size(); i++) {
		switch (objects[i]->get_state()) {
		case TS_Raise:	objects[i]->raise(step); break;
		case TS_Keep:	objects[i]->keep(); break;
		case TS_None:	break;
		}
	}
}

bool RemovePred(CTelekineticObject *tele_object)
{
	return (!tele_object->get_object() || 
		tele_object->get_object()->getDestroy() ||
		!tele_object->get_object()->PPhysicsShell() ||
		!tele_object->get_object()->PPhysicsShell()->bActive);
}


void  CTelekinesis::PhTune(dReal step)
{
	if (!active) return;

	//������ ��� ������ �� ������� �����������
	TELE_OBJECTS_IT it = remove_if(objects.begin(),objects.end(), RemovePred);
	objects.erase(it, objects.end());
	
	for (u32 i = 0; i < objects.size(); i++) {
		switch (objects[i]->get_state()) {
		case TS_Raise:	
		case TS_Keep:	objects[i]->enable();
		case TS_None:	break;
		}
	}
}

u32 CTelekinesis::get_objects_count() 
{
	u32 count = 0;
	for (u32 i=0;i<objects.size(); i++) {
		ETelekineticState state = objects[i]->get_state();
		if ((state == TS_Raise) || (state == TS_Keep)) count++;
	}

	return count;
}

