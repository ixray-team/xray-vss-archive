////////////////////////////////////////////////////////////////////////////
//	Module 		: script_export_space.h
//	Created 	: 22.09.2003
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Script export space 
////////////////////////////////////////////////////////////////////////////

#ifndef SCRIPT_EXPORT_SPACE_H
#define SCRIPT_EXPORT_SPACE_H

#include <typelist.h>

struct lua_State;

#ifdef SCRIPT_REGISTRATOR
#	define	script_type_list		Loki::NullType
#	define	add_to_type_list(type)	typedef Loki::Typelist<type,script_type_list> TypeList_##type;
#	define	save_type_list(type)	TypeList_##type
#else
#	define	script_type_list		
#	define	add_to_type_list(type)	;
#	define	save_type_list(type)		
#endif

#define DECLARE_SCRIPT_REGISTER_FUNCTION public: static void script_register(lua_State *);

#ifndef SCRIPT_REGISTRATOR
	template <typename T> struct enum_exporter{DECLARE_SCRIPT_REGISTER_FUNCTION};
	template <typename T> struct class_exporter{DECLARE_SCRIPT_REGISTER_FUNCTION};
#endif

#endif // SCRIPT_EXPORT_SPACE_H