////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_inline.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _object_type,\
		bool	 _reverse_search,\
		typename _world_operator,\
		typename _condition_evaluator,\
		typename _world_operator_ptr,\
		typename _condition_evaluator_ptr\
	>

#define CPlanner				\
	CActionPlanner <\
		_object_type,\
		_reverse_search,\
		_world_operator,\
		_condition_evaluator,\
		_world_operator_ptr,\
		_condition_evaluator_ptr\
	>

TEMPLATE_SPECIALIZATION
IC	CPlanner::CActionPlanner			()
{
	m_initialized			= false;
#ifdef LOG_ACTION
	m_use_log				= false;
#endif
}

TEMPLATE_SPECIALIZATION
IC	CPlanner::~CActionPlanner			()
{
	m_object						= 0;
}

TEMPLATE_SPECIALIZATION
void CPlanner::setup					(_object_type *object)
{
	inherited::setup		();
	m_object				= object;
	m_current_action_id		= _action_id_type(-1);
	m_storage.clear			();
	m_initialized			= false;
}

TEMPLATE_SPECIALIZATION
IC	_object_type &CPlanner::object		() const
{
	VERIFY					(m_object);
	return					(*m_object);
}

TEMPLATE_SPECIALIZATION
void CPlanner::update				()
{
	solve						();

#ifdef LOG_ACTION
	// printing solution
	if (m_use_log) {
		if (m_solution_changed) {
			Msg						("%6d : Solution for object %s [%d vertices searched]",Device.dwTimeGlobal,object_name(),ai().graph_engine().solver_algorithm().data_storage().get_visited_node_count());
			for (int i=0; i<(int)solution().size(); ++i)
				Msg					("%s",action2string(solution()[i]));
		}
	}
#endif

#ifdef LOG_ACTION
	if (m_failed) {
		// printing current world state
		show					();
		{
			Msg					("! ERROR : there is no action sequence, which can transfer current world state to the target one");
			Msg					("Time : %6d",Device.dwTimeGlobal);
			Msg					("Object : %s",object_name());
			Msg					("Current world state :");
			EVALUATOR_MAP::const_iterator	I = evaluators().begin();
			EVALUATOR_MAP::const_iterator	E = evaluators().end();
			for ( ; I != E; ++I) {
				xr_vector<COperatorCondition>::const_iterator J = std::lower_bound(current_state().conditions().begin(),current_state().conditions().end(),CWorldProperty((*I).first,false));
				char			temp = '?';
				if ((J != current_state().conditions().end()) && ((*J).condition() == (*I).first)) {
					temp		= (*J).value() ? '+' : '-';
					Msg			("%5c : [%d][%s]",temp,(*I).first,property2string((*I).first));
				}
			}
		}
		// printing target world state
		{
			Msg					("Target world state :");
			EVALUATOR_MAP::const_iterator	I = evaluators().begin();
			EVALUATOR_MAP::const_iterator	E = evaluators().end();
			for ( ; I != E; ++I) {
				xr_vector<COperatorCondition>::const_iterator J = std::lower_bound(target_state().conditions().begin(),target_state().conditions().end(),CWorldProperty((*I).first,false));
				char			temp = '?';
				if ((J != target_state().conditions().end()) && ((*J).condition() == (*I).first)) {
					temp		= (*J).value() ? '+' : '-';
					Msg			("%5c : [%d][%s]",temp,(*I).first,property2string((*I).first));
				}
			}
		}
//		VERIFY2						(!m_failed,"Problem solver couldn't build a valid path - verify your conditions, effects and goals!");
	}
#endif

	THROW							(!solution().empty());

	if (initialized()) {
		if (current_action_id() != solution().front()) {
			current_action().finalize	();
			m_current_action_id			= solution().front();
			current_action().initialize	();
		}
	}
	else {
		m_initialized				= true;
		m_current_action_id			= solution().front();
		current_action().initialize	();
	}

	current_action().execute	();
}

TEMPLATE_SPECIALIZATION
IC	typename CPlanner::COperator &CPlanner::action	(const _action_id_type &action_id)
{
	return					(*get_operator(action_id));
}

TEMPLATE_SPECIALIZATION
IC	typename CPlanner::CConditionEvaluator &CPlanner::evaluator		(const _condition_type &evaluator_id)
{
	return					(*inherited::evaluator(evaluator_id));
}

TEMPLATE_SPECIALIZATION
IC	typename CPlanner::_action_id_type CPlanner::current_action_id	() const
{
	VERIFY					(initialized());
	return					(m_current_action_id);
}

TEMPLATE_SPECIALIZATION
IC	typename CPlanner::COperator &CPlanner::current_action	()
{
	return					(action(current_action_id()));
}

TEMPLATE_SPECIALIZATION
IC	bool CPlanner::initialized	() const
{
	return					(m_initialized);
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::add_condition	(_world_operator *action, _condition_type condition_id, _value_type condition_value)
{
	action->add_condition	(CWorldProperty(condition_id,condition_value));
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::add_effect		(_world_operator *action, _condition_type condition_id, _value_type condition_value)
{
	action->add_effect		(CWorldProperty(condition_id,condition_value));
}

#ifdef LOG_ACTION
TEMPLATE_SPECIALIZATION
LPCSTR CPlanner::action2string		(const _action_id_type &action_id)
{
	return			(action(action_id).m_action_name);
}

TEMPLATE_SPECIALIZATION
LPCSTR CPlanner::property2string	(const _condition_type &property_id)
{
	return			(evaluator(property_id).m_evaluator_name);//itoa(property_id,m_temp_string,10));
}

TEMPLATE_SPECIALIZATION
LPCSTR CPlanner::object_name		() const
{
	return			(*m_object->cName());
}
#endif

TEMPLATE_SPECIALIZATION
IC	void CPlanner::add_operator		(const _edge_type &operator_id,	_operator_ptr _operator)
{
	inherited::add_operator	(operator_id,_operator);
	_operator->setup		(m_object,&m_storage);
#ifdef LOG_ACTION
	_operator->set_use_log	(m_use_log);
#endif
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::add_evaluator	(const _condition_type &condition_id, _condition_evaluator_ptr evaluator)
{
	inherited::add_evaluator(condition_id,evaluator);
	evaluator->setup		(m_object,&m_storage);
}

#ifdef LOG_ACTION
TEMPLATE_SPECIALIZATION
IC	void CPlanner::set_use_log		(bool value)
{
	m_use_log							= value;
	OPERATOR_VECTOR::iterator			I = m_operators.begin();
	OPERATOR_VECTOR::iterator			E = m_operators.end();
	for ( ; I != E; ++I)
		(*I).get_operator()->set_use_log(m_use_log);
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::show				(LPCSTR offset)
{
	string256		temp;
	strconcat		(temp,offset,"    ");
	{
		Msg			("\n%sEVALUATORS : %d\n",offset,evaluators().size());
		EVALUATOR_MAP::const_iterator	I = evaluators().begin();
		EVALUATOR_MAP::const_iterator	E = evaluators().end();
		for ( ; I != E; ++I)
			Msg		("%sevaluator   [%d][%s]",offset,(*I).first,property2string((*I).first));
	}
	{
		Msg			("\n%sOPERATORS : %d\n",offset,operators().size());
		OPERATOR_VECTOR::const_iterator	I = operators().begin();
		OPERATOR_VECTOR::const_iterator	E = operators().end();
		for ( ; I != E; ++I) {
			Msg		("%soperator    [%d][%s]",offset,(*I).m_operator_id,(*I).m_operator->m_action_name);

			{
				xr_vector<COperatorCondition>::const_iterator	i = (*I).m_operator->conditions().conditions().begin();
				xr_vector<COperatorCondition>::const_iterator	e = (*I).m_operator->conditions().conditions().end();
				for ( ; i != e; ++i)
					Msg	("%s	condition [%d][%s] = %s",offset,(*i).condition(),property2string((*i).condition()),(*i).value() ? "TRUE" : "FALSE");
			}
			{
				xr_vector<COperatorCondition>::const_iterator	i = (*I).m_operator->effects().conditions().begin();
				xr_vector<COperatorCondition>::const_iterator	e = (*I).m_operator->effects().conditions().end();
				for ( ; i != e; ++i)
					Msg	("%s	effect    [%d][%s] = %s",offset,(*i).condition(),property2string((*i).condition()),(*i).value() ? "TRUE" : "FALSE");
			}

			(*I).m_operator->show(temp);
			Msg	(" ");
		}
	}
}
#endif

#undef TEMPLATE_SPECIALIZATION
#undef CPlanner