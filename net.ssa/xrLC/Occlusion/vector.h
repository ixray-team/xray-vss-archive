#ifndef _vector_included
#define _vector_included

#define EPS_S		0.0000001f
#define EPS			0.0000100f
#define EPS_L		0.0010000f
#define PI			3.1415926535897932384626433832795f
#define PI_MUL_2	6.2831853071795864769252867665590f
#define PI_DIV_2	1.5707963267948966192313216916398f
#define PI_DIV_4	0.7853981633974483096156608458199f
#define PI_DIV_6	0.5235987755982988730771072305466f
#define PI_DIV_8	0.3926990816987241548078304229099f

struct _vector;		// vector
struct _matrix;		// matrix

struct _fpoint;		// fpoint
struct _lpoint;		// lpoint

struct _lrect;		// lrect
struct _frect;		// frect
struct _fbox;		// fbox

struct _TLpolygon;	// tlpolygon
struct _Lpolygon;	// lpolygon

struct _3dbuffer;	// direct sound 3d buffer
struct _3dlistener;	// direct sound listener

struct _vertex;		// standart vertex (like D3DVERTEX)
struct _Lvertex;	// already lit veretx
struct _TLvertex;	// transformed&lit vertex
struct _LITvertex;	// already lit, but can be litted addititionally

struct _color;		// floating point based color definition
struct _Pcolor;		// Packed color - DWORD

struct _lminmax;	// Min Max
struct _fminmax;	// Min Max

typedef unsigned char	uchar;


#pragma pack(push)
#pragma pack(1)

#include "_pcolor.h"
#include "_color.h"
#include "_vector3d.h"
#include "_quaternion.h"
#include "_matrix.h"
#include "_ipoint.h"
#include "_fpoint.h"
#include "_irect.h"
#include "_frect.h"
#include "_fbox.h"
#include "_vertex.h"
#include "_lit_vertex.h"
#include "_l_vertex.h"
#include "_tl_vertex.h"
#include "_plane.h"
#include "_intersection_quad.h"

typedef FLvertex		D3DLPOLYGON		[ 4	];
typedef FTLvertex		D3DTLPOLYGON	[ 4	];

void Ipoint::set(_fpoint &p){
	x=int(p.x);
	y=int(p.y);
};

void Fpoint::set(_ipoint &p){
	x=float(p.x);
	y=float(p.y);
};


#define TO_REAL(_X_, _S_)	((_X_)+1.f)*float(_S_/2)

typedef struct _TLpolygon {
public:
	union{
		struct{
			_TLvertex 	lb, lt, rb, rt;
		};
		_TLvertex		m[4];
	};

	__forceinline	void*	d3d		(void)									{ return this; }
	__forceinline	void	setpos	(Frect &r)								{ lb.sx=r.x1;	lb.sy=r.y2;	lt.sx=r.x1;	lt.sy=r.y1;	rb.sx=r.x2;	rb.sy=r.y2;	rt.sx=r.x2;	rt.sy=r.y1;}
	__forceinline	void	setpos	(float x1, float y1, float x2, float y2){ lb.sx=x1;		lb.sy=y2;	lt.sx=x1;	lt.sy=y1;	rb.sx=x2;	rb.sy=y2;	rt.sx=x2;	rt.sy=y1;}
	__forceinline	void	setdepth(float z = 0.f, float rhw = 1.f)		{ lb.sz=z;		rb.sz= z;	lt.sz = z;	rt.sz = z;	lb.rhw = rhw; rb.rhw = rhw; lt.rhw = rhw; rt.rhw = rhw;};
	__forceinline	void	setcolor(FPcolor c, FPcolor s)					{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c);   lb.specular.set(c); rb.specular.set(c); lt.specular.set(c); rt.specular.set(c);};
	__forceinline	void	setcolor(DWORD c, DWORD s)						{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c);   lb.specular.set(c); rb.specular.set(c); lt.specular.set(c); rt.specular.set(c);};
	__forceinline	void	setcolor(DWORD c)								{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c); };
	__forceinline	void	setcolor(FPcolor c)								{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c); };
	__forceinline	void	settex	(Frect &r)								{ lb.tu = r.x1; lb.tv = r.y2; lt.tu = r.x1; lt.tv = r.y1; rb.tu = r.x2; rb.tv = r.y2;	rt.tu = r.x2; rt.tv = r.y1; }
	__forceinline	void	settex	(float x1, float y1, float x2, float y2){ lb.tu=x1;		lb.tv=y2;	lt.tu=x1;	lt.tv=y1;	rb.tu=x2;	rb.tv=y2;	rt.tu=x2;	rt.tv=y1;}

	__forceinline	void	to_real	(int w, int h)
	{
		lt.sx=lb.sx=TO_REAL(lb.sx, w);	rb.sx=rt.sx=TO_REAL(rb.sx, w);
		lt.sy=rt.sy=TO_REAL(lt.sy, h);	lb.sy=rb.sy=TO_REAL(lb.sy, h);
	}

	__forceinline	void	to_real	(Frect &r, int w, int h)
	{
		lt.sx=lb.sx=TO_REAL(r.x1, w);	lt.sy=rt.sy=TO_REAL(r.y1, h);
		rb.sx=rt.sx=TO_REAL(r.x2, w);	lb.sy=rb.sy=TO_REAL(r.y2, h);
	}

	__forceinline	void	add	(float dx, float dy)
	{
		lb.sx+=dx;	lb.sy+=dy;	lt.sx+=dx;	lt.sy+=dy;
		rb.sx+=dx;	rb.sy+=dy;	rt.sx+=dx;	rt.sy+=dy;
	}

	__forceinline	void	add	(Frect &r, float dx, float dy)
	{
		lb.sx=r.x1+dx;	lb.sy=r.y2+dy;	lt.sx=r.x1+dx;	lt.sy=r.y1+dy;
		rb.sx=r.x2+dx;	rb.sy=r.y2+dy;	rt.sx=r.x2+dx;	rt.sy=r.y1+dy;
	}

	__forceinline	void	sub	(float dx, float dy)
	{
		lb.sx-=dx;	lb.sy-=dy;	lt.sx-=dx;	lt.sy-=dy;
		rb.sx-=dx;	rb.sy-=dy;	rt.sx-=dx;	rt.sy-=dy;
	}

	__forceinline	void	sub	(Frect &r, float dx, float dy)
	{
		lb.sx=r.x1-dx;	lb.sy=r.y2-dy;	lt.sx=r.x1-dx;	lt.sy=r.y1-dy;
		rb.sx=r.x2-dx;	rb.sy=r.y2-dy;	rt.sx=r.x2-dx;	rt.sy=r.y1-dy;
	}
} FTLpolygon;

typedef struct _Lpolygon {
public:
	union{
		struct{
			_Lvertex 	lb, lt, rb, rt;
		};
		_Lvertex		m[4];
	};

	__forceinline	void*	d3d		(void)									{ return this; }
	__forceinline	void	setcolor(FPcolor c, FPcolor s)					{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c);   lb.specular.set(c); rb.specular.set(c); lt.specular.set(c); rt.specular.set(c);};
	__forceinline	void	setcolor(DWORD c, DWORD s)						{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c);   lb.specular.set(c); rb.specular.set(c); lt.specular.set(c); rt.specular.set(c);};
	__forceinline	void	setcolor(DWORD c)								{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c); };
	__forceinline	void	setcolor(FPcolor c)								{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c); };
	__forceinline	void	settex	(Frect &r)								{ lb.tu = r.x1; lb.tv = r.y2; lt.tu = r.x1; lt.tv = r.y1; rb.tu = r.x2; rb.tv = r.y2;	rt.tu = r.x2; rt.tv = r.y1; }
	__forceinline	void	settex	(float x1, float y1, float x2, float y2){ lb.tu=x1;		lb.tv=y2;	lt.tu=x1;	lt.tv=y1;	rb.tu=x2;	rb.tv=y2;	rt.tu=x2;	rt.tv=y1;}

} FLpolygon;

typedef struct _light {
public:
    DWORD           type;             /* Type of light source */
    Fcolor          diffuse;          /* Diffuse color of light */
    Fcolor          specular;         /* Specular color of light */
    Fcolor          ambient;          /* Ambient color of light */
    Fvector         position;         /* Position in world space */
    Fvector         direction;        /* Direction in world space */
    float		    range;            /* Cutoff range */
    float	        falloff;          /* Falloff */
    float	        attenuation0;     /* Constant attenuation */
    float	        attenuation1;     /* Linear attenuation */
    float	        attenuation2;     /* Quadratic attenuation */
    float	        theta;            /* Inner angle of spotlight cone */
    float	        phi;              /* Outer angle of spotlight cone */

	__forceinline	D3DLIGHT7	*d3d() { return (D3DLIGHT7 *)this; };
	__forceinline	_light() {
		VERIFY(sizeof(_light)==sizeof(D3DLIGHT7));
	}
	__forceinline	void	set(DWORD ltType, float x, float y, float z) {
		VERIFY(sizeof(_light)==sizeof(D3DLIGHT7));
		ZeroMemory( this, sizeof(_light) );
		type=ltType;
		diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
		specular.set(diffuse);
		position.set(x,y,z);
		direction.set(x,y,z);
		direction.normalize();
		range= D3DLIGHT_RANGE_MAX;
	}
} Flight;

typedef struct _material {
public:
    Fcolor			diffuse;        /* Diffuse color RGBA */
    Fcolor			ambient;        /* Ambient color RGB */
    Fcolor		    specular;       /* Specular 'shininess' */
    Fcolor			emissive;       /* Emissive color RGB */
    float			power;          /* Sharpness if specular highlight */

	__forceinline	D3DMATERIAL7	*d3d(void) { return (D3DMATERIAL7 *)this; };
	__forceinline	void	Set(float r, float g, float b)
	{
		VERIFY(sizeof(_material)==sizeof(D3DMATERIAL7));
		ZeroMemory( this, sizeof(_material) );
		diffuse.r = ambient.r = r;
		diffuse.g = ambient.g = g;
		diffuse.b = ambient.b = b;
		diffuse.a = ambient.a = 1.0f;
		power	  = 0;
	}
	__forceinline	void	Set(float r, float g, float b, float a)
	{
		VERIFY(sizeof(_material)==sizeof(D3DMATERIAL7));
		ZeroMemory( this, sizeof(_material) );
		diffuse.r = ambient.r = r;
		diffuse.g = ambient.g = g;
		diffuse.b = ambient.b = b;
		diffuse.a = ambient.a = a;
		power	  = 0;
	}
	__forceinline	void	Set(Fcolor &c)
	{
		VERIFY(sizeof(_material)==sizeof(D3DMATERIAL7));
		ZeroMemory( this, sizeof(_material) );
		diffuse.r = ambient.r = c.r;
		diffuse.g = ambient.g = c.g;
		diffuse.b = ambient.b = c.b;
		diffuse.a = ambient.a = c.a;
		power	  = 0;
	}
} Fmaterial;

typedef struct _lminmax {
public:
	union{
		struct{
			int 	min, max;
		};
		int 		f[2];
	};

	__forceinline	void	set(int  _min, int  _max)	{ min = _min;	max = _max; };
	__forceinline	void	set(_lminmax p)				{ min = p.min;	max = p.max; };
	__forceinline   void	null( )						{ min = 0;		max = 0;  };
	__forceinline   bool	isnull( )					{ return ((!min)&&(!max));  };

	__forceinline   void	clamp( int& x )				{ if(x>max) x=max; else if (x<min) x=min; };

} Lminmax;

typedef struct _fminmax {
public:
	union{
		struct{
			float 	min, max;
		};
		float		f[2];
	};

	__forceinline	void	set(float  _min, float  _max)	{ min = _min;	max = _max; };
	__forceinline	void	set(_fminmax p)					{ min = p.min;	max = p.max; };
	__forceinline   void	null( )							{ min = 0;		max = 0;  };
	__forceinline   bool	isnull( )						{ return ((!min)&&(!max));  };

	__forceinline   void	clamp( float& x )				{ if(x>max) x=max; else if (x<min) x=min; };

} Fminmax;

#pragma pack(pop)

__forceinline	void	Fvector::transform_tiny(const _matrix &mat)
{
	float xx = (x*mat._11 + y*mat._21 + z* mat._31 + mat._41);
	float yy = (x*mat._12 + y*mat._22 + z* mat._32 + mat._42);
	float zz = (x*mat._13 + y*mat._23 + z* mat._33 + mat._43);
	x=xx;
	y=yy;
	z=zz;
}
_E(__forceinline	void	Fvector::transform(const _matrix &mat))
	float w = x*mat._14 + y*mat._24 + z* mat._34 + mat._44;
	VERIFY(fabsf(w)>0.000001f);
	float xx = (x*mat._11 + y*mat._21 + z* mat._31 + mat._41)/w;
	float yy = (x*mat._12 + y*mat._22 + z* mat._32 + mat._42)/w;
	float zz = (x*mat._13 + y*mat._23 + z* mat._33 + mat._43)/w;
	x=xx;
	y=yy;
	z=zz;
_end;
__forceinline	void	Fvector::transform_dir(const _matrix &mat)
{
	float xx = x*mat._11 + y*mat._21 + z* mat._31;
	float yy = x*mat._12 + y*mat._22 + z* mat._32;
	float zz = x*mat._13 + y*mat._23 + z* mat._33;
	x=xx;
	y=yy;
	z=zz;
}
__forceinline	void	Fvector::transform_tiny(const _vector &v, const _matrix &mat)
{
	x = (v.x*mat._11 + v.y*mat._21 + v.z* mat._31 + mat._41);
	y = (v.x*mat._12 + v.y*mat._22 + v.z* mat._32 + mat._42);
	z = (v.x*mat._13 + v.y*mat._23 + v.z* mat._33 + mat._43);
}
_E(__forceinline	void	Fvector::transform(const _vector &v,const _matrix &mat))
	float w = v.x*mat._14 + v.y*mat._24 + v.z*mat._34 + mat._44;
	VERIFY(fabsf(w)>0.000001f);
	x = (v.x*mat._11 + v.y*mat._21 + v.z* mat._31 + mat._41)/w;
	y = (v.x*mat._12 + v.y*mat._22 + v.z* mat._32 + mat._42)/w;
	z = (v.x*mat._13 + v.y*mat._23 + v.z* mat._33 + mat._43)/w;
_end;
__forceinline	void	Fvector::transform_dir(const _vector &v,const _matrix &mat)
{
	x = v.x*mat._11 + v.y*mat._21 + v.z*mat._31;
	y = v.x*mat._12 + v.y*mat._22 + v.z*mat._32;
	z = v.x*mat._13 + v.y*mat._23 + v.z*mat._33;
}
__forceinline	void	Fvector::position_of_camera(const _matrix &m)
{
    z = (-(m._41*(m._11*(m._12*m._23-m._13*m._22)+m._12*m._13*(m._21-1))-m._11*(m._42*(m._11*m._23-m._13)+m._43*(m._12*m._21-m._11*m._22)))/(m._11*m._11*(m._22*m._33-m._23*m._32)-m._11*(m._12*(m._21*m._33-m._23*m._31)+m._13*(m._22*m._31-m._32))+m._12*m._13*m._31*(m._21-1)));
    y = -(z*(m._11*m._32-m._12*m._31)+m._42*m._11-m._41*m._12)/(m._11*m._22-m._12*m._21);
    x = (-m._41 -y*m._21-z*m._31)/m._11;
}

#endif
