// LightProjector.cpp: implementation of the CLightProjector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightProjector.h"
#include "xr_object.h"
#include "render.h"
#include "flightscontroller.h"
#include "xr_creator.h"
#include "fbasicvisual.h"
#include "lighttrack.h"

const	int		P_rt_size	= 512;
const	int		P_size		= 32;
const	int		P_o_line	= P_rt_size/P_size;
const	int		P_o_count	= P_o_line*P_o_line;
const	float	P_distance	= 48;
const	float	P_distance2	= P_distance*P_distance;
const	float	P_cam_dist	= 20;

/*
int		psSH_Blur			= 1;

const	float	S_distance	= 48;
const	float	S_distance2	= S_distance*S_distance;

const	float	S_fade		= 3;
const	float	S_fade2		= S_fade*S_fade;

const	float	S_level		= .1f;
const	int		S_size		= 102;
const	int		S_rt_size	= 512;
const	int		batch_size	= 128;
const	float	S_tess		= .5f;
const	int 	S_ambient	= 64;
const	int 	S_clip		= 256-24;
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightProjector::CLightProjector()
{
	current	= 0;
	RT		= 0;
}

CLightProjector::~CLightProjector()
{
}

void CLightProjector::OnDeviceCreate	()
{
	LPCSTR	RTname		= "$user$projector";
	LPCSTR	RTtemp		= "$user$temp";
	string128 RTname2;	strconcat(RTname2,RTname,",",RTname);
	string128 RTtemp2;	strconcat(RTtemp2,RTtemp,",",RTtemp);
	
	// 
	RT			= Device.Shader._CreateRT	(RTname,P_rt_size,P_rt_size);
	RT_temp		= Device.Shader._CreateRT	(RTtemp,P_rt_size,P_rt_size);
	sh_BlurTR	= Device.Shader.Create		("effects\\blur",			RTtemp2);
	sh_BlurRT	= Device.Shader.Create		("effects\\blur",			RTname2);
	vs_Blur		= Device.Streams.Create		(FVF::F_TL2uv, 8);

	// Debug
	sh_Screen	= Device.Shader.Create		("effects\\screen_set",RTname);
	vs_Screen	= Device.Streams.Create		(FVF::F_TL,4);
}

void CLightProjector::OnDeviceDestroy	()
{
	// Debug
	Device.Shader.Delete					(sh_Screen	);
	
	// 
	Device.Shader.Delete					(sh_BlurRT	);
	Device.Shader.Delete					(sh_BlurTR	);
	Device.Shader._DeleteRT					(RT_temp	);
	Device.Shader._DeleteRT					(RT			);
}

void CLightProjector::set_object	(CObject* O)
{
	if ((0==O) || (id.size()>=P_o_line))	current		= 0;
	else 
	{
		Fvector		C;
		O->clCenter	(C);
		float		D = C.distance_to(Device.vCameraPosition)+O->Radius();
		if (D < P_distance)		current	= O;
		else					current = 0;
		
		if (current)
		{
			id.push_back		(receivers.size());
			receivers.push_back	(recv());
			receivers.back().O	= current;
			receivers.back().C	= C;
			receivers.back().D	= D;
		}
	}
}

void CLightProjector::add_element	(NODE* N)
{
	if (0==current)	return;
	receivers.back().nodes.push_back(*N);
}

//
class	pred_sorter
{
	CLightProjector*		S;
public:
	pred_sorter(CLightProjector* _S) : S(_S) {};
	IC bool operator () (int c1, int c2)
	{ return S->receivers[c1].D<S->receivers[c2].D; }
};
//
void CLightProjector::calculate	()
{
	if (id.empty())	return;
	
	Device.Statistic.RenderDUMP_Pcalc.Begin	();
	Device.Shader.set_RT		(RT_temp->pRT,HW.pTempZB);
	
	// sort by distance
	std::sort	(id.begin(),id.end(),pred_sorter(this));
	
	// iterate on objects
	int	slot_id		= 0;
	for (int o_it=0; o_it<id.size(); o_it++)
	{
		recv&	C	= receivers	[id[o_it]];
		if (C.nodes.empty())	continue;

		// calculate projection-matrix
		Fmatrix		mProject;
		float		p_dist	=	P_cam_dist;
		float		p_R		=	C.O->Radius();
		float		p_hat	=	p_R/p_dist;
		float		p_asp	=	1.f;
		float		p_near	=	p_dist-EPS_L;									
		float		p_far	=	_min(Lrange,_max(p_dist+S_fade,p_dist+p_R));	
		if (p_near<EPS_L)			p_near	= eps;
		if (p_far<(p_near+EPS_L))	p_far	= p_near+eps;
		
		mProject.build_projection_HAT	(p_hat,p_asp,p_near,p_far);
		Device.set_xform_project		(mProject);
		
		// calculate view-matrix
		Fmatrix		mView;
		Fvector		v_D,v_N,v_R;
		v_D.sub					(C.C,Lpos);;
		if(1-fabsf(v_D.y)<EPS)	v_N.set(1,0,0);
		else            		v_N.set(0,1,0);
		v_R.crossproduct		(v_N,v_D);
		v_N.crossproduct		(v_D,v_R);
		mView.build_camera		(Lpos,C.C,v_N);
		Device.set_xform_view	(mView);
		
		// combine and build frustum
		Fmatrix		mCombine;
		mCombine.mul			(mProject,mView);
		
		// Select slot and set viewport
		int		s_x			=	slot_id%slot_line;
		int		s_y			=	slot_id/slot_line;
		D3DVIEWPORT8 VP		=	{s_x*S_size,s_y*S_size,S_size,S_size,0,1 };
		CHK_DX					(HW.pDevice->SetViewport(&VP));
		
		// Build bbox out of sphere
		float		p_R		=	C.O->Radius();
		
		// Render object-parts
		for (int n_it=0; n_it<C.nodes.size(); n_it++)
		{
			NODE& N			=	C.nodes[n_it];
			CVisual *V		=	N.val.pVisual;
			Device.set_xform_world	(N.val.Matrix);
			V->Render				(-1.0f);
		}
		
		// register shadow and increment slot
		shadows.push_back	(shadow());
		shadows.back().slot	=	slot_id;
		shadows.back().C	=	C.C;
		shadows.back().M	=	mCombine;
		shadows.back().L	=	&L.L;
		slot_id	++;
	}
	casters.clear	();
	id.clear		();
	
	// Blur
	{
		float						dim				= S_rt_size;
		Fvector2					shift,p0,p1,a0,a1,b0,b1,c0,c1,d0,d1;
		p0.set						(.5f/dim, .5f/dim);
		p1.set						((dim+.5f)/dim, (dim+.5f)/dim);
		shift.set(.5f/dim, .5f/dim); a0.add(p0,shift); a1.add(p1,shift); b0.sub(p0,shift); b1.sub(p1,shift);
		shift.set(.5f/dim,-.5f/dim); c0.add(p0,shift); c1.add(p1,shift); d0.sub(p0,shift); d1.sub(p1,shift);
		
		// Fill VB
		DWORD C						=	0xffffffff, Offset;
		FVF::TL2uv* pv				=	(FVF::TL2uv*) vs_Blur->Lock(8,Offset);
		pv->set						(0.f,	dim,	C, a0.x, a1.y, b0.x, b1.y);	pv++;
		pv->set						(0.f,	0.f,	C, a0.x, a0.y, b0.x, b0.y);	pv++;
		pv->set						(dim,	dim,	C, a1.x, a1.y, b1.x, b1.y);	pv++;
		pv->set						(dim,	0.f,	C, a1.x, a0.y, b1.x, b0.y);	pv++;
		
		pv->set						(0.f,	dim,	C, c0.x, c1.y, d0.x, d1.y);	pv++;
		pv->set						(0.f,	0.f,	C, c0.x, c0.y, d0.x, d0.y);	pv++;
		pv->set						(dim,	dim,	C, c1.x, c1.y, d1.x, d1.y);	pv++;
		pv->set						(dim,	0.f,	C, c1.x, c0.y, d1.x, d0.y);	pv++;
		vs_Blur->Unlock				(8);
		
		// Actual rendering (pass0, temp2real)
		Device.Shader.set_RT		(RT->pRT,	0);
		Device.Shader.set_Shader	(sh_BlurTR	);
		Device.Primitive.Draw		(vs_Blur,	4, 2, Offset,	Device.Streams_QuadIB);
		
		for (int it=0; it<psSH_Blur; it++)	
		{
			// Actual rendering (pass1, real2temp)
			Device.Shader.set_RT		(RT_temp->pRT,	0);
			Device.Shader.set_Shader	(sh_BlurRT	);
			Device.Primitive.Draw		(vs_Blur,	4, 2, Offset+4,	Device.Streams_QuadIB);
			
			// Actual rendering (pass2, temp2real)
			Device.Shader.set_RT		(RT->pRT,	0);
			Device.Shader.set_Shader	(sh_BlurTR	);
			Device.Primitive.Draw		(vs_Blur,	4, 2, Offset,	Device.Streams_QuadIB);
		}
	}
	
	// Finita la comedia
	Device.Statistic.RenderDUMP_Pcalc.End	();
	
	Device.set_xform_project	(Device.mProject);
	Device.set_xform_view		(Device.mView);
}

#define CLS(a) D3DCOLOR_RGBA(a,a,a,a)
void CLightProjector::render	()
{
	Device.Statistic.RenderDUMP_Srender.Begin	();
	// Gain access to collision-DB
	CDB::MODEL*		DB		= pCreator->ObjectSpace.GetStaticModel();
	CDB::TRI*		TRIS	= DB->get_tris();

	int slot_line	= S_rt_size/S_size;
	int slot_max	= slot_line*slot_line;
	
	// Projection and xform
	float _43			= Device.mProject._43;
	Device.mProject._43 -= 0.001f; 
	Device.set_xform_world	(Fidentity);
	Device.set_xform_project(Device.mProject);
	Fvector	View		= Device.vCameraPosition;
	
	// Render shadows
	Device.Shader.set_Shader	(sh_World);
	int batch					= 0;
	DWORD Offset				= 0;
	DWORD C						= 0x7f7f7f7f;
	FVF::LIT* pv				= (FVF::LIT*) vs_World->Lock(batch_size*3,Offset);
	for (int s_it=0; s_it<shadows.size(); s_it++)
	{
		shadow&		S			=	shadows[s_it];
		float		Le			=	S.L->diffuse.magnitude_rgb();
		int			s_x			=	S.slot%slot_line;
		int			s_y			=	S.slot/slot_line;
		Fvector2	t_scale, t_offset;
		t_scale.set	(float(S_size)/float(S_rt_size),float(S_size)/float(S_rt_size));
		t_scale.mul (.5f);
		t_offset.set(float(s_x)/float(slot_line),float(s_y)/float(slot_line));
		t_offset.x	+= .5f/S_rt_size;
		t_offset.y	+= .5f/S_rt_size;
		
		// Frustum
		CFrustum	F;
		F.CreateFromMatrix		(S.M,FRUSTUM_P_ALL);

		// Query
		XRC.frustum_options		(0);
		XRC.frustum_query		(DB,F);
		if (0==XRC.r_count())	continue;
		
		// Clip polys by frustum
		tess.clear				();
		for (CDB::RESULT* p = XRC.r_begin(); p!=XRC.r_end(); p++)
		{
			// 
			CDB::TRI&	t		= TRIS[p->id];
			sPoly		A,B;
			A.push_back			(*t.verts[0]);
			A.push_back			(*t.verts[1]);
			A.push_back			(*t.verts[2]);

			// Calc plane
			Fplane		P;
			P.build				(A[0],A[1],A[2]);
			if (P.classify(View)<0)				continue;
			if (P.classify(S.L->position)<0)	continue;
			
			// Clip polygon
			sPoly*		clip	= F.ClipPoly(A,B);
			if (0==clip)		continue;
			
			// Triangulate poly 
			for (int v=2; v<clip->size(); v++)
			{
				tess.push_back	(tess_tri());
				tess_tri& T		= tess.back();
				T.v[0]			= (*clip)[0];
				T.v[1]			= (*clip)[v-1];
				T.v[2]			= (*clip)[v];
				T.N				= P.n;
			}
		}

		// Tesselate
		
		
		// Fill VB
		for (int tid=0; tid<tess.size(); tid++)
		{
			tess_tri&	TT		= tess[tid];
			Fvector* 	v		= TT.v;
			Fvector		T;

			int	c0				= PLC_calc(v[0],TT.N,S.L,Le,S.C);	clamp(c0,S_ambient,255);
			int	c1				= PLC_calc(v[1],TT.N,S.L,Le,S.C);	clamp(c1,S_ambient,255);
			int	c2				= PLC_calc(v[2],TT.N,S.L,Le,S.C);	clamp(c2,S_ambient,255);
			if ((c0>S_clip) && (c1>S_clip) && (c2>S_clip))			continue;

			S.M.transform(T,v[0]); pv->set(v[0],CLS(c0),(T.x+1)*t_scale.x+t_offset.x,(1-T.y)*t_scale.y+t_offset.y); pv++;
			S.M.transform(T,v[1]); pv->set(v[1],CLS(c1),(T.x+1)*t_scale.x+t_offset.x,(1-T.y)*t_scale.y+t_offset.y); pv++;
			S.M.transform(T,v[2]); pv->set(v[2],CLS(c2),(T.x+1)*t_scale.x+t_offset.x,(1-T.y)*t_scale.y+t_offset.y); pv++;

			batch++;
			if (batch==batch_size)	{
				// Flush
				vs_World->Unlock		(batch*3);
				Device.Primitive.Draw	(vs_World,batch,Offset);
				pv						= (FVF::LIT*) vs_World->Lock(batch_size*3,Offset);
				batch					= 0;
			}
		}
	}

	// Flush if nessesary
	vs_World->Unlock		(batch*3);
	if (batch)				Device.Primitive.Draw	(vs_World,batch,Offset);
	
	// Clear all shadows
	shadows.clear			();

	// Debug
/*
	{
		// UV
		Fvector2				p0,p1;
		p0.set					(.5f/S_rt_size, .5f/S_rt_size);
		p1.set					((S_rt_size+.5f)/S_rt_size, (S_rt_size+.5f)/S_rt_size);
		
		// Fill vertex buffer
		C			=	0xffffffff;
		DWORD _w	=	S_rt_size/2, _h = S_rt_size/2;
		FVF::TL* pv =	(FVF::TL*) vs_Screen->Lock(4,Offset);
		pv->set(0,			float(_h),	.0001f,.9999f, C, p0.x, p1.y);	pv++;
		pv->set(0,			0,			.0001f,.9999f, C, p0.x, p0.y);	pv++;
		pv->set(float(_w),	float(_h),	.0001f,.9999f, C, p1.x, p1.y);	pv++;
		pv->set(float(_w),	0,			.0001f,.9999f, C, p1.x, p0.y);	pv++;
		vs_Screen->Unlock			(4);
		
		// Actual rendering
		Device.Shader.set_Shader(sh_Screen);
		Device.Primitive.Draw	(vs_Screen,4,2,Offset,Device.Streams_QuadIB);
	}
//*/

	// Projection
	Device.mProject._43 = _43;
	Device.set_xform_project	(Device.mProject);
	Device.Statistic.RenderDUMP_Srender.End	();
}
