////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_space.h
//	Created 	: 22.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script space
////////////////////////////////////////////////////////////////////////////

#pragma once

// Lua
#pragma warning(disable:4244)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#pragma warning(disable:4267)

//#define LUABIND_NO_EXCEPTIONS

#pragma comment(lib,"x:\\xrLUA.lib")

//extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
	__declspec(dllimport) LPSTR g_ca_stdout;
//}

// Lua-bind
#include <luabind\\luabind.hpp>
#include <luabind\\object.hpp>

#pragma warning(default:4244)
#pragma warning(default:4995)
#pragma warning(default:4530)
#pragma warning(default:4267)

typedef lua_State CLuaVirtualMachine;

#include "ai_script_lua_space.h"