#pragma once

typedef struct
{
	const char* szDesc;
	const char* szFile;
	int nLine;
} StackTrace;

typedef struct
{
	const char* szName;
	const char* szType;
	const char* szValue;
} Variable;

#define _DMSG_FIRST_MSG			DMSG_WRITE_DEBUG

#define DMSG_WRITE_DEBUG			WM_USER+1
#define DMSG_HAS_BREAKPOINT			WM_USER+2
#define DMSG_GOTO_FILELINE			WM_USER+3
#define DMSG_DEBUG_START			WM_USER+4
#define DMSG_DEBUG_BREAK			WM_USER+5
#define DMSG_DEBUG_END				WM_USER+6
#define DMSG_CLEAR_STACKTRACE		WM_USER+7
#define DMSG_ADD_STACKTRACE			WM_USER+8
#define DMSG_GOTO_STACKTRACE_LEVEL	WM_USER+9
#define DMSG_GET_STACKTRACE_LEVEL	WM_USER+10
#define DMSG_CLEAR_LOCALVARIABLES	WM_USER+11
#define DMSG_ADD_LOCALVARIABLE		WM_USER+12
#define DMSG_CLEAR_GLOBALVARIABLES	WM_USER+13
#define DMSG_ADD_GLOBALVARIABLE		WM_USER+14
#define DMSG_REDRAW_WATCHES			WM_USER+15

#define DMSG_CREATE_IDE				WM_USER+16
#define DMSG_DESTROY_IDE			WM_USER+17
#define DMSG_SHOW_IDE				WM_USER+18
#define DMSG_HIDE_IDE				WM_USER+19

#define DMSG_DEBUG_STEP_INTO		WM_USER+20
#define DMSG_DEBUG_STEP_OVER		WM_USER+21
#define DMSG_DEBUG_STEP_OUT			WM_USER+22
#define DMSG_DEBUG_RUN_TO_CURSOR	WM_USER+23

#define _DMSG_LAST_MSG			DMSG_DEBUG_RUN_TO_CURSOR
