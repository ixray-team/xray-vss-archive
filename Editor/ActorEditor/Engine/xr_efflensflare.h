#ifndef __CLENSFLARE_HPP
#define __CLENSFLARE_HPP

class CInifile;

class CLensFlare
#ifndef _EDITOR
	:public pureDeviceCreate,
	public pureDeviceDestroy
#endif
{
protected:
	BOOL			bInit;
	float			fBlend;

	Fvector			vSunDir;
	Fvector			vecLight;
	Fvector			vecX, vecY, vecDir, vecAxis, vecCenter;
	BOOL			bRender;
	// variable
    Fcolor			LightColor;
	float			fGradientDensity;

	CPrimitive		P;

    void			SetSource	(float fRadius, const char* tex_name);
    void			AddFlare	(float fRadius, float fOpacity, float fPosition, const char* tex_name);
public:
	struct SFlare{
    	float		fOpacity;
	    float		fRadius;
    	float		fPosition;
		Shader*		hShader;
        string128	texture;
    	SFlare(){ZeroMemory(this,sizeof(SFlare));}
	};
    DEFINE_VECTOR	(SFlare,FlareVec,FlareIt);
    FlareVec		m_Flares;

	enum {
    	flFlare 	= (1<<0),
    	flSource	= (1<<1),
    	flGradient 	= (1<<2)
    };
	struct{
        DWORD		bFlare			: 1;
        DWORD		bSource			: 1;
        DWORD		bGradient		: 1;
	}				m_Flags;
    // source
    SFlare			m_Source;
    // gradient
    float			m_fGradientDensity;
    Shader*			CreateSourceShader(const char* tex_name);
    Shader*			CreateFlareShader(const char* tex_name);
public:
					CLensFlare	();
	virtual			~CLensFlare	();

	virtual void	OnDeviceDestroy();
	virtual void	OnDeviceCreate();

	void			OnMove		( );
	void			Load		( CInifile* pIni, LPSTR section );
    void __fastcall	Render		( BOOL bSun, BOOL bFlares );
	void			Update		( Fvector& sun_dir, Fcolor& color );
};

#endif // __CLENSFLARE_HPP
