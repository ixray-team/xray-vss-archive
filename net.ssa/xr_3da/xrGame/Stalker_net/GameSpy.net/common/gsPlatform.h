///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef __GSPLATFORM_H__
#define __GSPLATFORM_H__

// GameSpy platform definition and headers

// Windows:          _WIN32
// Xbox:             _WIN32 + _XBOX
// MacOSX:           _MACOSX + _UNIX
// Linux:            _LINUX + _UNIX
// Nintendo DS:      _NITRO
// PSP:              _PSP
// PS3:              _PS3

// PlayStation2:     _PS2
//    w/ EENET:      EENET       (set by developer project)
//    w/ INSOCK:     INSOCK      (set by developer project)
//    w/ SNSYSTEMS:  SN_SYSTEMS  (set by developer project)
//    Codewarrior:   __MWERKS__

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Set the platform define
#ifdef __mips64
	#ifndef _PS2
		#define _PS2
	#endif

	// Make sure a network stack was defined
	#if !defined(SN_SYSTEMS) && !defined(EENET) && !defined(INSOCK)
		#error "PlayStation2 network stack was not defined!"
	#endif
#endif

#if defined(_LINUX) || defined(_MACOSX)
	#define _UNIX
#endif
// WIN32, set by OS headers
// _XBOX, set by OS headers
// __MWERKS__, set by compiler
// _NITRO, set by OS headers


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Include common OS headers
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// XBOX
#if defined(_XBOX)
	#include <Xtl.h>

// WIN32
#elif defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <limits.h>

	#if defined(GSI_WINSOCK2)
		#include <winsock2.h>
	#else
		#include <winsock.h>
	#endif

// PS2
#elif defined(_PS2)
	// EENet headers must be included before common PS2 headers
	#ifdef EENET 
		#include <libeenet.h>
		#include <eenetctl.h>
		#include <ifaddrs.h>
		#include <sys/socket.h>
		#include <sys/errno.h>
		#include <netinet/in.h>
		#include <arpa/inet.h>
		#include <net/if.h>
		#include <sys/select.h>
		#include <malloc.h>
	#endif // EENET

	// Common PS2 headers
	#include <eekernel.h>
	#include <stdio.h>
	#include <malloc.h>
	#include <sifdev.h>
	#include <sifrpc.h>
	#include <sifcmd.h>
	#include <ilink.h>
	#include <ilsock.h>
	#include <ilsocksf.h>

	#ifdef GSI_UNICODE
	#include <wchar.h>
	#endif

	#ifdef SN_SYSTEMS
		// undefine socket defines from sys/types.h
		// This is to workaround sony now automatically including sys/types.h
		// and SNSystems having not produce a patch yet (they'll likely do the same since
		// the SNSystems fd_set is a slightly different size than the sys/types.h.
		#undef FD_CLR	
		#undef FD_ZERO
		#undef FD_SET	
		#undef FD_ISSET
		#undef FD_SETSIZE
		#undef fd_set
		#include "snskdefs.h"
		#include "sntypes.h"
		#include "snsocket.h"
		#include "sneeutil.h"
		#include "sntcutil.h"
	#endif // SN_SYSTEMS

	#ifdef INSOCK
		#include "libinsck.h"
		#include "libnet.h"
		#include "sys/errno.h"
		//#include "libmrpc.h"
	#endif // INSOCK

// LINUX and MACOSX
#elif defined(_UNIX)
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <stdio.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>

	// MACOSX Warning!! netdb.h has it's own NOFILE define.
	// GameSpy uses NOFILE to determine if an HD is available
	#ifndef NOFILE
		// Since GameSpy NOFILE is not defined, include netdb.h then undef NOFILE
		#include <netdb.h>
		#undef NOFILE
	#else
		// Otherwise leave NOFILE defined
		#include <netdb.h>
	#endif

	#include <arpa/inet.h>
	#include <errno.h>
	#include <sys/time.h>
	#include <limits.h>
	//#include <sys/syslimits.h>
	#include <netinet/tcp.h>

// Nintendo DS
#elif defined(_NITRO)
	#include <nitro.h>
	#define NINET_NWBASE_MD5_H_  // resolves md5 conflicts
	#include <nitroWiFi.h>
	#include <extras.h>  // mwerks

	// DS doesn't have native support for these
	struct tm *gmtime(const time_t *timer);
	char *ctime(const time_t *timer);


// Sony PSP
#elif defined(_PSP)
	#include <kernel.h>
	#include <pspnet.h>
	#include <pspnet_error.h>
	#include <pspnet_inet.h>
	#include <pspnet/sys/select.h>
	#include <pspnet_resolver.h>
	#include <pspnet_apctl.h>
	#include <pspnet_ap_dialog_dummy.h>
	#include <rtcsvc.h>
	#include <errno.h>
	#include <wlan.h>

	#include <pspnet/sys/socket.h>
	#include <pspnet/netinet/in.h>
	#include <utility\utility_common.h>
	#include <utility\utility_netconf.h>
	#include <utility\utility_netmodule.h>
// PS3
#elif defined(_PS3)
#include <netex/errno.h>
	#include <sys/process.h>
	#include <sys/time.h>
	#include <sys/types.h>	
	#include <sys/select.h>
	#include <sys/socket.h>
	#include <sys/sys_time.h>
	#include <sys/timer.h>
	#include <errno.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netex/net.h>
	#include <netex/ifctl.h>
//	#include <netex/netset.h>
	#include <limits.h>
	#include "time.h"

// Nintendo Wii
#elif defined(_REVOLUTION)
	#include <revolution.h>
	
// Unsupported platform or no platform defined!
#else
	#error "The GameSpy SDKs do not support this operating system"

#endif //(platform switch)

//
//Added 05-19-2006 by Scott Horwath
//---------- __cdecl fix for __fastcall conventions ----------
//
#if defined(_WIN32)
	#define GS_STATIC_CALLBACK __cdecl
#else
	#define GS_STATIC_CALLBACK
#endif


//---------- Handle Endianess ----------------------
#if defined _MACOSX || defined _PS3 || defined(_REVOLUTION)
	#define GSI_BIG_ENDIAN
#endif
#ifndef GSI_BIG_ENDIAN
	#define GSI_LITTLE_ENDIAN
#endif



#if defined(GSI_UNICODE)
	#include <wchar.h>
#endif

#ifndef GS_UNDER_CE
	#include <ctype.h>
	#if defined(_MACOSX)
		#undef _T
	#endif
#endif

#ifdef GS_UNDER_CE
	#include <platutil.h>
#endif

#if defined(GS_UNDER_CE)
	#define assert(a)
#else
	#include <assert.h>
#endif

#if defined(GS_NO_FILE) || defined(GS_UNDER_CE) || defined(_PS2) || defined(_PS3) || defined(_NITRO) || defined(_PSP)
	#define NOFILE
#endif

#if defined(_PSP) || defined(_NITRO)
	#define GS_WIRELESS_DEVICE
#endif

#if !defined(GSI_DOMAIN_NAME)
	#define GSI_DOMAIN_NAME "gamespy.com"
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Define GameSpy types

// common base type defines, please refer to ranges below when porting
typedef char              gsi_i8;
typedef unsigned char     gsi_u8;
typedef short             gsi_i16;
typedef unsigned short    gsi_u16;
typedef int               gsi_i32;
typedef unsigned int      gsi_u32;
typedef unsigned int      gsi_time;  // must be 32 bits

// decprecated
typedef gsi_i32           goa_int32;  // 2003.Oct.04.JED - typename deprecated
typedef gsi_u32           goa_uint32; //  these types will be removed once all SDK's are updated

typedef int               gsi_bool;
#define gsi_false         ((gsi_bool)0)
#define gsi_true          ((gsi_bool)1)
#define gsi_is_false(x)   ((x) == gsi_false)
#define gsi_is_true(x)    ((x) != gsi_false)

// Max integer size
#if defined(_INTEGRAL_MAX_BITS) && !defined(GSI_MAX_INTEGRAL_BITS)
	#define GSI_MAX_INTEGRAL_BITS   _INTEGRAL_MAX_BITS
#else
	#define GSI_MAX_INTEGRAL_BITS   32
#endif

// Platform dependent types
#ifdef _PS2
	typedef signed long           gsi_i64;
	typedef unsigned long         gsi_u64;
#elif defined(_WIN32)
	#if (!defined(_M_IX86) || (defined(_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64))
	typedef __int64               gsi_i64;
	typedef unsigned __int64      gsi_u64;
	#endif
#elif defined(_NITRO)
	typedef s64                   gsi_i64;
	typedef u64                   gsi_u64;
#elif defined (_PSP)
	typedef long long             gsi_i64;
	typedef unsigned long long    gsi_u64;
#elif defined (_PS3)
	typedef signed long long      gsi_i64;
	typedef unsigned long long    gsi_u64;
#elif defined (_REVOLUTION)
	typedef signed long long      gsi_i64;
	typedef unsigned long long    gsi_u64;
#else /* _UNIX */
	typedef long long             gsi_i64;
	typedef unsigned long long    gsi_u64;
#endif // 64 bit types

#ifndef GSI_UNICODE
	#define gsi_char  char
#else
	#define gsi_char  unsigned short
#endif // goa_char


// expected ranges for integer types
#define GSI_MIN_I8        CHAR_MIN
#define GSI_MAX_I8        CHAR_MAX
#define GSI_MAX_U8        UCHAR_MAX

#define GSI_MIN_I16       SHRT_MIN
#define GSI_MAX_I16       SHRT_MAX
#define GSI_MAX_U16       USHRT_MAX

#define GSI_MIN_I32       INT_MIN
#define GSI_MAX_I32       INT_MAX
#define GSI_MAX_U32       UINT_MAX

#if (GSI_MAX_INTEGRAL_BITS >= 64)
#define GSI_MIN_I64     (-9223372036854775807 - 1)
#define GSI_MAX_I64       9223372036854775807
#define GSI_MAX_U64       0xffffffffffffffffui64
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Common platform string functions
#undef _vftprintf
#undef _ftprintf
#undef _tprintf
#undef _tsnprintf
#undef _tcscpy
#undef _tfopen
#undef _tcscat
#undef _tcslen
#undef _T

#ifdef GSI_UNICODE
	#define _vftprintf  vfwprintf
	#define _ftprintf   fwprintf
	#define _stprintf   swprintf
	#define _tprintf    wprintf
	#define _tcscpy     wcscpy
	#define _tcsncpy	wcsncpy
	#define _tcscat     wcscat
	#define _tcslen     wcslen
	#define _tcschr     wcschr
	#define _tfopen     _wfopen
	#define _T(a)       L##a

	#if defined(_WIN32) || defined(_PS2)
		#define _tsnprintf _snwprintf
	#else
		#define _tsnprintf swprintf
	#endif
#else
	#define _vftprintf  vfprintf
	#define _ftprintf   fprintf
	#define _stprintf   sprintf
	#define _tprintf    printf
	#define _tcscpy     strcpy
	#define _tcsncpy	strncpy
	#define _tcscat     strcat
	#define _tcslen     strlen
	#define _tcschr	    strchr
	#define _tfopen     fopen
	#define _T(a)       a

	#if defined(_WIN32)
		#define _tsnprintf _snprintf
	#else
		#define _tsnprintf snprintf
	#endif
#endif // GSI_UNICODE

#if defined(_WIN32)
	#define snprintf _snprintf
#endif // _WIN32

#if defined(_WIN32) && !defined(GS_UNDER_CE)
	#define strcasecmp _stricmp
	#define strncasecmp _strnicmp
#else	
	char *_strlwr(char *string);
	char *_strupr(char *string);
#endif

#undef strdup
#define strdup goastrdup
#undef _strdup
#define _strdup goastrdup
#if defined(__cplusplus)
extern "C" {
#endif
char * goastrdup(const char *src);
#if defined(__cplusplus)
}
#endif

#if defined(GS_UNDER_CE)
	int strcasecmp(const char *string1, const char *string2);
	int strncasecmp(const char *string1, const char *string2, size_t count);
#endif


// ------ Cross Plat Alignment macros ------------
/* ex use
PRE_ALIGN(16)	struct VECTOR			
{
	float	x,y,z,_unused;	
}	POST_ALIGN(16);

// another example when defining a variable:
PRE_ALIGN(16);
static char _mempool[MEMPOOL_SIZE]	POST_ALIGN(16);

*/
#if defined (_XBOX) 
	#define PRE_ALIGN(x)	__declspec(align(x))	// ignore Win32 directive
	#define POST_ALIGN(x)	// ignore

#elif defined _WIN32
	#define PRE_ALIGN(x)	__declspec(align(x))	// ignore Win32 directive
	#define POST_ALIGN(x)	// ignore

#elif defined  (_PS2) || defined (_PSP) || defined (_PS3) 
	#define PRE_ALIGN(x)	// ignored this on psp/ps2
	#define POST_ALIGN(x)	__attribute__((aligned (x)))		// 
#else
	// #warning "Platform not supported"
	#define PRE_ALIGN(x)	// ignore
	#define POST_ALIGN(x)	// ignore
#endif

#define DIM( x )				( sizeof( x ) / sizeof((x)[ 0 ]))

#ifdef __cplusplus
extern "C" {
#endif

extern gsi_u16 gsiByteOrderSwap16(gsi_u16);
extern gsi_u32 gsiByteOrderSwap32(gsi_u32);
extern gsi_u64 gsiByteOrderSwap64(gsi_u64);

#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // __GSPLATFORM_H__

