// Portal.cpp: implementation of the CPortal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "r__sector.h"
#include "..\xrLevel.h"
#include "..\xr_object.h"
#include "..\fbasicvisual.h"
#include "..\IGame_Persistent.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPortal::~CPortal()
{
}

//
void	CPortal::Setup	(Fvector* V, int vcnt, CSector* face, CSector* back)
{
	// calc sphere
	Fbox				BB;
	BB.invalidate		();
	for (int v=0; v<vcnt; v++)
		BB.modify		(V[v]);
	BB.getsphere		(S.P,S.R);

	// 
	poly.assign			(V,vcnt);
	pFace				= face; 
	pBack				= back;
	marker				= 0xffffffff; 

	Fvector				N,T;
	N.set				(0,0,0);

	FPU::m64r();
	for (int i=2; i<vcnt; i++) {
		T.mknormal	(poly[0],poly[i-1],poly[i]);
		N.add		(T);
	}
	float	tcnt = float(vcnt)-2;
	N.div		(tcnt);
	N.normalize	();
	P.build		(poly[0],N);
	FPU::m24r	();

	/*
	if (_abs(1-P.n.magnitude())<EPS)
	Debug.fatal		("Degenerated portal found at {%3.2f,%3.2f,%3.2f}.",VPUSH(poly[0]));
	*/
}

//
CSector::~CSector()
{
}

//
extern float r_ssaDISCARD;

void CSector::traverse			(CFrustum &F, _scissor& R_scissor)
{
	// Register traversal process
	if (r_marker	!=	PortalTraverser.i_marker)	{
		r_marker							=	PortalTraverser.i_marker;
		PortalTraverser.r_sectors.push_back	(this);
		r_frustums.clear					();
		r_scissors.clear					();
	}
	r_frustums.push_back		(F);
	r_scissors.push_back		(R_scissor);

	// Search visible portals and go through them
	sPoly	S,D;
	for	(u32 I=0; I<m_portals.size(); I++)
	{
		if (m_portals[I]->marker == PortalTraverser.i_marker) continue;

		CPortal* PORTAL = m_portals[I];
		CSector* pSector;

		// Select sector (allow intersecting portals to be finely classified)
		if (PORTAL->bDualRender) {
			pSector = PORTAL->getSector				(this);
		} else {
			pSector = PORTAL->getSectorBack			(PortalTraverser.i_vBase);
			if (pSector==this)						continue;
			if (pSector==PortalTraverser.i_start)	continue;
		}

		// SSA	(if required)
		if (PortalTraverser.i_options&CPortalTraverser::VQ_SSA)
		{
			Fvector	dir2portal;
			dir2portal.sub		(PORTAL->S.P,	PortalTraverser.i_vBase);
			float R				=	PORTAL->S.R;
			float distSQ		=	dir2portal.square_magnitude();
			float ssa			=	R*R/distSQ;
			dir2portal.div		(_sqrt(distSQ));
			ssa					*=	_abs(PORTAL->P.n.dotproduct(dir2portal));
			if (ssa<r_ssaDISCARD)	continue;
		}

		// Clip by frustum
		svector<Fvector,8>&	POLY = PORTAL->getPoly();
		S.assign			(&*POLY.begin(),POLY.size()); D.clear();
		sPoly* P			= F.ClipPoly(S,D);
		if (0==P)			continue;

		// Scissor and optimized HOM-testing
		_scissor			scissor;
		if (PortalTraverser.i_options&CPortalTraverser::VQ_SCISSOR && (!PORTAL->bDualRender))
		{
			// Build scissor rectangle in projection-space
			Fbox2	bb;	bb.invalidate(); float depth = flt_max;
			sPoly&	p	= *P;
			for		(u32 vit=0; vit<p.size(); vit++)	{
				Fvector4	t;	
				Fmatrix&	M	= PortalTraverser.i_mXFORM_01;
				Fvector&	v	= p[vit];

				t.x = v.x*M._11 + v.y*M._21 + v.z*M._31 + M._41;
				t.y = v.x*M._12 + v.y*M._22 + v.z*M._32 + M._42;
				t.z = v.x*M._13 + v.y*M._23 + v.z*M._33 + M._43;
				t.w = v.x*M._14 + v.y*M._24 + v.z*M._34 + M._44;
				t.mul	(1.f/t.w);

				if (t.x < bb.min.x)	bb.min.x	= t.x; 
				if (t.x > bb.max.x) bb.max.x	= t.x;
				if (t.y < bb.min.y)	bb.min.y	= t.y; 
				if (t.y > bb.max.y) bb.max.y	= t.y;
				if (t.z < depth)	depth		= t.z;
			}
			// Msg	("bb(%s): (%f,%f)-(%f,%f), d=%f", PORTAL->bDualRender?"true":"false",bb.min.x, bb.min.y, bb.max.x, bb.max.y,depth);
			if (depth<EPS)	{
				scissor	= R_scissor;

				// Cull by HOM (slower algo)
				if  (
					(PortalTraverser.i_options&CPortalTraverser::VQ_HOM) && 
					(!RImplementation.HOM.visible(*P))
					)	continue;
			} else {
				// perform intersection (this is just to be sure, it is probably clipped in 3D already)
				if (bb.min.x > R_scissor.min.x)	scissor.min.x = bb.min.x; else scissor.min.x = R_scissor.min.x;
				if (bb.min.y > R_scissor.min.y)	scissor.min.y = bb.min.y; else scissor.min.y = R_scissor.min.y;
				if (bb.max.x < R_scissor.max.x) scissor.max.x = bb.max.x; else scissor.max.x = R_scissor.max.x;
				if (bb.max.y < R_scissor.max.y) scissor.max.y = bb.max.y; else scissor.max.y = R_scissor.max.y;
				scissor.depth	= depth;

				// Msg	("scissor: (%f,%f)-(%f,%f)", scissor.min.x, scissor.min.y, scissor.max.x, scissor.max.y);
				// Check if box is non-empty
				if (scissor.min.x >= scissor.max.x)	continue;
				if (scissor.min.y >= scissor.max.y)	continue;

				// Cull by HOM (faster algo)
				if  (
					(PortalTraverser.i_options&CPortalTraverser::VQ_HOM) && 
					(!RImplementation.HOM.visible(scissor,depth))
					)	continue;
			}
		} else {
			scissor	= R_scissor;

			// Cull by HOM (slower algo)
			if  (
				(PortalTraverser.i_options&CPortalTraverser::VQ_HOM) && 
				(!RImplementation.HOM.visible(*P))
				)	continue;
		}

		// Create _new_ frustum and recurse
		CFrustum				Clip;
		Clip.CreateFromPortal	(P,PortalTraverser.i_vBase,PortalTraverser.i_mXFORM);
		PORTAL->marker			= PortalTraverser.i_marker;
		PORTAL->bDualRender		= FALSE;
		pSector->traverse		(Clip,scissor);
	}
}

void CSector::load(IReader& fs)
{
	// Assign portal polygons
	u32 size			= fs.find_chunk(fsP_Portals); R_ASSERT(0==(size&1));
	u32 count			= size/2;
	m_portals.reserve	(count);
	while (count) {
		u16 ID		= fs.r_u16();
		CPortal* P	= (CPortal*)RImplementation.getPortal	(ID);
		m_portals.push_back(P);
		count--;
	}

	if	(!g_pGamePersistent->bDedicatedServer)	{
		// Assign visual
		size	= fs.find_chunk(fsP_Root);	R_ASSERT(size==4);
		m_root	= RImplementation.getVisual(fs.r_u32());
	} else {
		m_root	= 0;
	}
}
