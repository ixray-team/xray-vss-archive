////////////////////////////////////////////////////////////////////////////
//	Module 		: game_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_path_manager.h"
#include "game_graph.h"
#include "ai_object_location.h"

template <
	typename _VertexEvaluator,
	typename _vertex_id_type,
	typename _index_type
>
class 
	CBasePathManager<
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> :
	public CAbstractPathManager<
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	>,
	virtual public CAI_ObjectLocation
{
	typedef CAbstractPathManager<
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type,
		_index_type
	> inherited;
protected:
	IC			bool	actual						() const;
};

#include "game_path_manager_inline.h"
