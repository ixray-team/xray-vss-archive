#include "stdafx.h"
#include "control_path_builder_base.h"
#include "BaseMonster/base_monster.h"

//////////////////////////////////////////////////////////////////////////
// Method: prepare_builder
// Desc: reset path builder params to defaults
//////////////////////////////////////////////////////////////////////////
void CControlPathBuilderBase::prepare_builder() 
{
	m_time						= 0;
	m_distance_to_path_end		= 1.f;
	m_failed					= false;
	m_cover_info.use_covers		= false;

	m_target_actual				= false;

	m_target_set.init			();

	set_target_accessible		(m_target_found, m_object->Position());
}

//////////////////////////////////////////////////////////////////////////
// Method: set_target_point
// Desc: just set desirable position and update actuality 
// all checkings will be made on update stage
//////////////////////////////////////////////////////////////////////////
void CControlPathBuilderBase::set_target_point(const Fvector &position, u32 node)
{
	// �������� ������������
	m_target_actual = m_target_actual && (m_target_set.position.similar(position) && (m_target_set.node == node));

	// ���������� �������
	m_target_set.set	(position,node);

	// ���������� ���������� ��������� ������������
	m_target_type		= eMoveToTarget;

	set_level_path_type	();
}

void CControlPathBuilderBase::set_retreat_from_point(const Fvector &position)
{
	// �������� ������������
	m_target_actual = m_target_actual && (m_target_set.position.similar(position));

	// ���������� �������
	m_target_set.set	(position,u32(-1));	

	// ���������� ���������� ��������� ������������
	m_target_type		= eRetreatFromTarget;

	set_level_path_type	();
}

