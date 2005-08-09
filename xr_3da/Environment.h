#ifndef EnvironmentH
#define EnvironmentH

// refs
class ENGINE_API	IRender_Visual;
class ENGINE_API	CInifile;
class ENGINE_API 	CEnvironment;

// refs - effects
class ENGINE_API	CEnvironment;
class ENGINE_API	CLensFlare;	
class ENGINE_API	CEffect_Rain;
class ENGINE_API	CEffect_Thunderbolt;

class ENGINE_API	CPerlinNoise1D;

// t-defs
class ENGINE_API	CEnvModifier
{
public:
	Fvector3			position;
	float				radius;
	float				power;

	float				far_plane;
	Fvector3			fog_color;
	float				fog_density;
	Fvector3			ambient;
	Fvector3			lmap_color;

	void				load		(IReader*		fs);
	float				sum			(CEnvModifier&	_another, Fvector3& view);
};

class ENGINE_API	CEnvAmbient{
public:
	struct SEffect{
		u32 			life_time;
		ref_sound		sound;		
		shared_str		particles;
		Fvector			offset;
		float			wind_gust_factor;
	};
	DEFINE_VECTOR(SEffect,EffectVec,EffectVecIt);
protected:
	shared_str			section;
	EffectVec			effects;
	xr_vector<ref_sound>sounds;
	Ivector2			sound_period;
	Ivector2			effect_period;
public:
	void				load				(const shared_str& section);
	IC SEffect*			get_rnd_effect		(){return effects.empty()?0:&effects[Random.randI(effects.size())];}
	IC ref_sound*		get_rnd_sound		(){return sounds.empty()?0:&sounds[Random.randI(sounds.size())];}
	IC const shared_str&name				(){return section;}
	IC u32				get_rnd_sound_time	(){return Random.randI(sound_period.x,sound_period.y);}
	IC u32				get_rnd_effect_time (){return Random.randI(effect_period.x,effect_period.y);}
};

class ENGINE_API	CEnvDescriptor
{
public:
    float				exec_time;
	
	STextureList		sky_r_textures;		// C
	STextureList		sky_r_textures_env;	// C
	STextureList		clouds_r_textures;	// C
	float				weight;				// C

	ref_texture			sky_texture		;
	ref_texture			sky_texture_env	;
	ref_texture			clouds_texture	;

	Fvector4			clouds_color	;
	Fvector3			sky_color		;
	float				sky_rotation	;

	float				far_plane;

	Fvector3			fog_color;
	float				fog_density;
	float				fog_near;		// C
	float				fog_far;		// C

	float				rain_density;
	Fvector3			rain_color;

	float				bolt_period;
	float				bolt_duration;

    float				wind_velocity;
    float				wind_direction;  
    
	Fvector3			ambient		;
	Fvector3			lmap_color	;
	Fvector4			hemi_color	;	// w = R2 correction
	Fvector3			sun_color	;
	Fvector3			sun_dir		;

    int					lens_flare_id;
	int					tb_id;
    
	CEnvAmbient*		env_ambient;

						CEnvDescriptor	();

	void				load			(LPCSTR exec_tm, LPCSTR sect, CEnvironment* parent);
    void				unload			();
    
	void				lerp			(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power);
};

class ENGINE_API	CEnvironment
{
	FvectorVec				CloudsVerts;
	U16Vec					CloudsIndices;

	struct str_pred : public std::binary_function<shared_str, shared_str, bool>	{	
		IC bool operator()(const shared_str& x, const shared_str& y) const
		{	return xr_strcmp(x,y)<0;	}
	};

protected:
	CPerlinNoise1D*			PerlinNoise1D;
public:
	float					wind_strength_factor;	
	float					wind_gust_factor;
public:

	DEFINE_VECTOR			(CEnvAmbient*,EnvAmbVec,EnvAmbVecIt);
	DEFINE_VECTOR			(CEnvDescriptor*,EnvVec,EnvIt);
	DEFINE_MAP_PRED			(shared_str,EnvVec,WeatherMap,WeatherPairIt,str_pred);

	// Environments
	CEnvDescriptor			CurrentEnv;
	CEnvDescriptor*			Current[2];
    bool					bTerminator;
    
    EnvVec*					CurrentWeather;
    shared_str				CurrentWeatherName;
	WeatherMap				Weathers;
	xr_vector<CEnvModifier>	Modifiers;
	EnvAmbVec				Ambients;

	ref_shader				sh_2sky;
	ref_geom				sh_2geom;

	ref_shader				clouds_sh;
	ref_geom				clouds_geom;

	CEffect_Rain*			eff_Rain;
	CLensFlare*				eff_LensFlare;
	CEffect_Thunderbolt*	eff_Thunderbolt;

	float					fGameTime;
	float					fTimeFactor;
	ref_texture				tonemap;

    void					SelectEnvs			(float gt);

	void					UpdateAmbient		();
	CEnvAmbient*			AppendEnvAmb		(const shared_str& sect);

	void					Invalidate			();
public:
							CEnvironment		();
							~CEnvironment		();

	void					load				();
    void					unload				();

	void					mods_load			();
	void					mods_unload			();

	void					OnFrame				();

	void					RenderSky			();
	void					RenderClouds		();
	void					RenderFlares		();
	void					RenderLast			();

    void					SetWeather			(shared_str name, bool forced=false);
    shared_str				GetWeather			()					{ return CurrentWeatherName;}
	void					SetGameTime			(float game_time, float time_factor)	{ fGameTime = game_time;  fTimeFactor=time_factor;	}

	void					OnDeviceCreate		();
	void					OnDeviceDestroy		();

	// editor-related
#ifdef _EDITOR
public:
	float					ed_from_time		;
	float					ed_to_time			;
public:
    void					ED_Reload			();
#endif
};

ENGINE_API extern Flags32	psEnvFlags;
ENGINE_API extern float		psVisDistance;

#endif //EnvironmentH
