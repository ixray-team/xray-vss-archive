////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extension.h
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script extensions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_script_space.h"
//struct CLuaVirtualMachine;

namespace Script {
#ifndef ENGINE_BUILD
	bool		bfPrintOutput				(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR	caScriptName, int iErorCode = 0);
	void		vfExportGlobals				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportFvector				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportFmatrix				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportGame				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportLevel				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportDevice				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportParticles			(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportSound				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportHit					(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportActions				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportObject				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportEffector			(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportArtifactMerger		(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfLoadStandardScripts		(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportToLua				(CLuaVirtualMachine *tpLuaVirtualMachine);
	bool		bfLoadBuffer				(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR	caBuffer,		size_t	tSize,				LPCSTR	caScriptName);
	LPCSTR		cafEventToString			(int				iEventCode);
	void		vfPrintError				(CLuaVirtualMachine *tpLuaVirtualMachine, int		iErrorCode);
	bool		bfListLevelVars				(CLuaVirtualMachine *tpLuaVirtualMachine, int		iStackLevel);
	void		LuaHookCall					(CLuaVirtualMachine *tpLuaVirtualMachine, lua_Debug *tpLuaDebug);
	int			LuaPanic					(CLuaVirtualMachine *tpLuaVirtualMachine);
#endif
	bool		bfLoadFile					(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR	caScriptName,	bool	bCall = true);
	bool		bfLoadFileIntoNamespace		(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR	caScriptName,	LPCSTR	caNamespaceName,	bool	bCall);
};
