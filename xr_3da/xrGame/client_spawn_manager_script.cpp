////////////////////////////////////////////////////////////////////////////
//	Module 		: client_spawn_manager_script.cpp
//	Created 	: 08.10.2004
//  Modified 	: 08.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Client spawn manager script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "client_spawn_manager.h"

using namespace luabind;

void CClientSpawnManager::script_register(lua_State *L)
{
	module(L)
	[
		class_<CClientSpawnManager>("client_spawn_manager")
			.def("add",		(void (CClientSpawnManager::*)(ALife::_OBJECT_ID,ALife::_OBJECT_ID,const luabind::object &,LPCSTR))(CClientSpawnManager::add))
			.def("add",		(void (CClientSpawnManager::*)(ALife::_OBJECT_ID,ALife::_OBJECT_ID,const luabind::functor<void> &))(CClientSpawnManager::add))
			.def("remove",	(void (CClientSpawnManager::*)(ALife::_OBJECT_ID,ALife::_OBJECT_ID))(CClientSpawnManager::remove))
	];
}
