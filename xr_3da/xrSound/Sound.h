#ifndef SoundH
#define SoundH
#pragma once

#ifdef XRSOUND_EXPORTS
	#define XRSOUND_API __declspec(dllexport)
#else
	#define XRSOUND_API __declspec(dllimport)
#endif

#ifdef __BORLANDC__
	#define XRSOUND_EDITOR_API XRSOUND_API

	// editor only refs
	class XRSOUND_EDITOR_API SoundEnvironment_LIB;
#else
	#define XRSOUND_EDITOR_API
#endif

#define SNDENV_FILENAME "sEnvironment.xr"
#define OGG_COMMENT_VERSION 		0x0001 

// refs
class	CObject;
class	XRSOUND_API					CSound_params;
class	XRSOUND_API					CSound_source;
class	XRSOUND_API					CSound_interface;
class	XRSOUND_API					CSound_stream_interface;
class	XRSOUND_API					CSound_environment;

//
XRSOUND_API extern u32				psSoundFreq				;
XRSOUND_API extern u32				psSoundModel			;
XRSOUND_API extern float			psSoundVMaster			;
XRSOUND_API extern float			psSoundVEffects			;
XRSOUND_API extern float			psSoundVMusic			;
XRSOUND_API extern float			psSoundRolloff			;
XRSOUND_API extern float			psSoundDoppler			;
XRSOUND_API extern float			psSoundOcclusionScale	;
XRSOUND_API extern float			psSoundCull				;
XRSOUND_API extern Flags32			psSoundFlags			;
XRSOUND_API extern int				psSoundTargets			;
XRSOUND_API extern int				psSoundCacheSizeMB		;

// Flags
enum {
	ssWaveTrace			= (1ul<<0ul),	//!< Flag to control wave-tracing
	ssEAX				= (1ul<<1ul),	//!< Use \a EAX or not
	ssHardware			= (1ul<<2ul),	//!< Use hardware mixing only
	ss_forcedword		= u32(-1)
};
enum {
	sf_22K,
	sf_44K,
	sf_forcedword = u32(-1)
};
enum {
	sq_DEFAULT,
	sq_NOVIRT,
	sq_LIGHT,
	sq_HIGH,
	sq_forcedword = u32(-1)
};

enum {
	st_Undefined		= 0,
	st_SourceType		= u32(-1),
	st_forcedword		= u32(-1),
};
/*! \class ref_sound
	\brief Sound source + control
	
	The main class respresenting source/emitter interface
	This class infact just hides internals and redirect calls to 
	specific sub-systems
*/
struct	ref_sound
{
	CSound_source*					handle;			//!< Pointer to wave-source interface
	CSound_interface*				feedback;		//!< Pointer to emitter, automaticaly clears on emitter-stop
	int								g_type;			//!< Sound type, usually for AI
	CObject*						g_object;		//!< Game object that emitts ref_sound

    //! A constructor
    /*!
		\sa ~ref_sound()
	*/
	ref_sound()							{ handle = 0; feedback=0; g_type=st_SourceType; g_object=0; }
	~ref_sound()						{ destroy(); }

	//! Loader/initializer
	/*!
		\sa clone()
		\sa destroy()
		\param _3D Controls whenewer the source is \a 3D or \a 2D.
		\param name Name of wave-file
		\param type Sound type, usually for \a AI
	*/
	IC void					create					( BOOL _3D,	LPCSTR name,	int		type=st_SourceType);

	//! Clones ref_sound from another
	/*!
		\sa create()
		\sa destroy()
		\param from Source to clone.
		\param leave_type Controls whenewer to leave game/AI type as is
	*/
	IC void					clone					( const ref_sound& from,	int		type=st_SourceType);

	//! Destroys and unload wave
	/*!
		\sa create()
		\sa clone()
	*/
	IC void					destroy					( );

	//@{
	//! Starts playing this source
	/*!
		\sa stop()
	*/
	IC void					play					( CObject* O /*!< Object */,											BOOL bLoop=false  /*!< Looping */, float delay=0.f /*!< Delay */);
	IC void					play_unlimited			( CObject* O /*!< Object */,											BOOL bLoop=false  /*!< Looping */, float delay=0.f /*!< Delay */);
	IC void					play_at_pos				( CObject* O /*!< Object */,	const Fvector &pos /*!< 3D position */,	BOOL bLoop=false  /*!< Looping */, float delay=0.f /*!< Delay */);
	IC void					play_at_pos_unlimited	( CObject* O /*!< Object */,	const Fvector &pos /*!< 3D position */,	BOOL bLoop=false  /*!< Looping */, float delay=0.f /*!< Delay */);
	//@}

	//! Stops playing this source
	/*!
		\sa play(), etc
	*/
	IC void					stop 					( );
	IC void					set_2D_mode				( );
	IC void					set_position			( const Fvector &pos);
	IC void					set_frequency			( float freq);
	IC void					set_range				( float min, float max );
	IC void					set_volume				( float vol );

	IC const CSound_params*	get_params				( );
    IC void					set_params				( CSound_params* p );
};

/// definition (Sound Source)
class XRSOUND_API	CSound_source
{
public:
	virtual	u32				length_ms				( )		= 0;
	virtual u32				game_type				( )		= 0;
	virtual LPCSTR			file_name				( )		= 0;
};

/// definition (Sound Source)
class XRSOUND_API	CSound_environment
{
public:
};

/// definition (Sound Params)
class XRSOUND_API	CSound_params
{
public:
	Fvector			position;
	float			volume;
	float			freq;
	float			min_distance;
	float			max_distance;
};

/// definition (Sound Interface)
class XRSOUND_API	CSound_interface
{
public:
	virtual	void					set_2D_mode				()															= 0;
	virtual void					set_position			(const Fvector &pos)										= 0;
	virtual void					set_frequency			(float freq)												= 0;
	virtual void					set_range				(float min, float max)										= 0;
	virtual void					set_volume				(float vol)													= 0;
	virtual void					stop					( )															= 0;
	virtual	const CSound_params*	get_params				( )															= 0;
};

/// definition (Sound Stream Interface)
class XRSOUND_API	CSound_stream_interface
{
public:
};

/// definition (Sound Stream Interface)
class XRSOUND_API	CSound_stats
{
public:
	u32				_rendered;
	u32				_simulated;
	u32				_cache_hits;
	u32				_cache_misses;
};

/// definition (Sound Callback)
typedef		void __stdcall sound_event						(ref_sound* S, float range);

/// definition (Sound Manager Interface)
class XRSOUND_API	CSound_manager_interface
{
public:
	//@{
	/// General
	virtual void					_initialize				( u64 window )																			= 0;
	virtual void					_destroy				( )																						= 0;
	virtual void					_restart				( )																						= 0;
	//@}

	//@{
	/// Sound interface
	virtual void					create					( ref_sound& S, BOOL _3D,	LPCSTR fName,	int		type=st_SourceType)					= 0;
	virtual void					destroy					( ref_sound& S)																			= 0;
	virtual void					play					( ref_sound& S, CObject* O,								BOOL bLoop=false, float delay=0.f)	= 0;
	virtual void					play_unlimited			( ref_sound& S, CObject* O,								BOOL bLoop=false, float delay=0.f)	= 0;
	virtual void					play_at_pos				( ref_sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false, float delay=0.f)	= 0;
	virtual void					play_at_pos_unlimited	( ref_sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false, float delay=0.f)	= 0;

	virtual void					set_geometry_env		( IReader* I )																			= 0;
	virtual void					set_geometry_occ		( CDB::MODEL* M )																		= 0;
	virtual void					set_handler				( sound_event* E )																		= 0;
	//@}

	virtual void					update					( const Fvector& P, const Fvector& D, const Fvector& N, float dt )						= 0;
	virtual void					update_events			( )																						= 0;
	virtual void					statistic				( CSound_stats&  dest )																	= 0;

#ifdef __BORLANDC__
	virtual SoundEnvironment_LIB*	get_env_library			()																						= 0;
	virtual void					refresh_env_library		()																						= 0;
	virtual void					set_user_env			( CSound_environment* E)																= 0;
	virtual void					refresh_sources			()																						= 0;
#endif
};
extern XRSOUND_API CSound_manager_interface*		Sound;

/// ********* Sound ********* (utils, accessors, helpers)
IC void	ref_sound::create						( BOOL _3D,	LPCSTR name,	int		type)				{	::Sound->create					(*this,_3D,name,type);				}
IC void	ref_sound::destroy						( )														{	::Sound->destroy				(*this);							}
IC void	ref_sound::play							( CObject* O,						BOOL bLoop, float d){	::Sound->play					(*this,O,bLoop,d);					}
IC void ref_sound::play_unlimited				( CObject* O,						BOOL bLoop, float d){	::Sound->play_unlimited			(*this,O,bLoop,d);					}
IC void	ref_sound::play_at_pos					( CObject* O,	const Fvector &pos,	BOOL bLoop, float d){	::Sound->play_at_pos			(*this,O,pos,bLoop,d);				}
IC void	ref_sound::play_at_pos_unlimited		( CObject* O,	const Fvector &pos,	BOOL bLoop, float d){	::Sound->play_at_pos_unlimited	(*this,O,pos,bLoop,d);				}
IC void	ref_sound::set_2D_mode					( )														{	VERIFY(feedback);feedback->set_2D_mode();							}
IC void	ref_sound::set_position					( const Fvector &pos)									{	VERIFY(feedback);feedback->set_position(pos);						}
IC void	ref_sound::set_frequency				( float freq)											{	if (feedback)	feedback->set_frequency(freq);						}
IC void	ref_sound::set_range					( float min, float max )								{	if (feedback)	feedback->set_range(min,max);						}
IC void	ref_sound::set_volume					( float vol )											{	if (feedback)	feedback->set_volume(vol);							}
IC void	ref_sound::stop							( )														{	if (feedback)	feedback->stop();									}
IC const CSound_params*	ref_sound::get_params	( )														{	if (feedback)	return feedback->get_params(); else return NULL;	}
IC void	ref_sound::set_params					( CSound_params* p )									
{	
	if (feedback){
    	feedback->set_position	(p->position);
    	feedback->set_frequency	(p->freq);
        feedback->set_range   	(p->min_distance,p->max_distance);
        feedback->set_volume   	(p->volume);
    }
}
IC void	ref_sound::clone						( const ref_sound& from, int type )		
{
	feedback	= 0;
	g_object	= 0;
	handle		= from.handle;
	g_type		= (type==st_SourceType)?handle->game_type():type;
}
#endif