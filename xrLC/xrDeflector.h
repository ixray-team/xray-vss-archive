#pragma once

#ifndef BORDER
#define BORDER 1
#endif

#include "hash2D.h"
#include "cl_defs.h"

struct UVtri : public _TCF 
{
	Face*	owner;
};

#define LT_DIRECT		0
#define LT_POINT		1
#define LT_SECONDARY	2

struct R_Light
{
    DWORD           type;				// Type of light source		
    Fcolor          diffuse;			// Diffuse color of light	
    Fvector         position;			// Position in world space	
    Fvector         direction;			// Direction in world space	
    float		    range;				// Cutoff range
	float			range2;				// ^2
    float	        attenuation0;		// Constant attenuation		
    float	        attenuation1;		// Linear attenuation		
    float	        attenuation2;		// Quadratic attenuation	
	float			energy;				// For radiosity ONLY
	
	Fvector			tri[3];

	R_Light()		{
		tri[0].set	(0,0,0);
		tri[1].set	(0,0,EPS_S);
		tri[2].set	(EPS_S,0,0);
	}
};

typedef hash2D<UVtri*,128,128>	HASH;

class CDeflector
{
public:
	vector<UVtri>		tris;
	Fvector				N;
	struct Layer
	{
		b_light*		base;
		b_texture		lm;
		
		DWORD			Area ()	{ return (lm.dwWidth+2*BORDER)*(lm.dwHeight+2*BORDER); }

		Layer()			{ ZeroMemory(this,sizeof(*this)); }
	};
	vector<Layer>		layers;

	Fvector				Center;
	float				Radius;

	DWORD				dwWidth;
	DWORD				dwHeight;

	vector<R_Light>		LightsSelected;
	RAPID::XRCollide	DB;
public:
	CDeflector				();
	~CDeflector				();

	VOID	OA_SetNormal	(Fvector &_N )	{ N.set(_N); N.normalize(); }
	BOOL	OA_Place		(Face *owner);
	VOID	OA_Export		();
	VOID	Capture			(CDeflector *D, int b_u, int b_v, int s_u, int s_v, BOOL bRotate);

	VOID	GetRect			(UVpoint &min, UVpoint &max);

	DWORD	GetFaceCount()	{ return tris.size();	};

	VOID	Light			(HASH& H);
	VOID	L_Direct		(HASH& H);
	VOID	L_Direct_Edge	(UVpoint& p1, UVpoint& p2, Fvector& v1, Fvector& v2, Fvector& N, float texel_size);
	VOID	L_Calculate		(HASH& H);
	VOID	Save			();

	WORD	GetBaseMaterial() { return tris.front().owner->dwMaterial;	}

	void	Bounds			(DWORD ID, Fbox2& dest)
	{
		UVtri& TC		= tris[ID];
		dest.min.set	(TC.uv[0].conv());
		dest.max.set	(TC.uv[0].conv());
		dest.modify		(TC.uv[1].conv());
		dest.modify		(TC.uv[2].conv());
	}
	void	Bounds_Summary (Fbox2& bounds)
	{
		bounds.invalidate();
		for (DWORD I=0; I<tris.size(); I++)
		{
			Fbox2	B;
			Bounds	(I,B);
			bounds.merge(B);
		}
	}
	void	RemapUV			(DWORD base_u, DWORD base_v, DWORD size_u, DWORD size_v, DWORD lm_u, DWORD lm_v, BOOL bRotate);
};

typedef vector<UVtri>::iterator UVIt;
IC float Lrnd()
{
	return g_params.m_lm_dither*2.f*float(rand())/float(RAND_MAX) + 
		(1.f - g_params.m_lm_dither);
}

extern void		Jitter_Select	(UVpoint* &Jitter, DWORD& Jcount);
extern void		blit			(LPDWORD dest, DWORD ds_x, DWORD ds_y, LPDWORD src, DWORD ss_x, DWORD ss_y, DWORD px, DWORD py, DWORD aREF);
