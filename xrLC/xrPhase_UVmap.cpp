#include "stdafx.h"
#include "build.h"

extern BOOL	hasImplicitLighting		(Face* F);

int			affected	= 0;
void Face::OA_Unwarp()
{
	if (pDeflector)					return;
	if (!Deflector->OA_Place(this))	return;
	
	// now iterate on all our neigbours
	for (int i=0; i<3; i++) 
		for (vecFaceIt it=v[i]->adjacent.begin(); it!=v[i]->adjacent.end(); it++) 
		{
			affected		+= 1;
			(*it)->OA_Unwarp();
		}
}
void Detach			(vecFace* S)
{
	map_v2v			verts;
	verts.clear		();
	
	// Collect vertices
	for (vecFaceIt F=S->begin(); F!=S->end(); F++)
	{
		for (int i=0; i<3; i++) {
			Vertex*		V=(*F)->v[i];
			Vertex*		VC;
			map_v2v_it	W=verts.find(V);	// iterator
			
			if (W==verts.end()) 
			{	// where is no such-vertex
				VC = V->CreateCopy_NOADJ();	// make copy
				verts.insert(make_pair(V, VC));
			} else {
				// such vertex(key) already exists - update its adjacency
				VC = W->second;
			}
			VC->prep_add		(*F);
			V->prep_remove		(*F);
			(*F)->v[i]=VC;
		}
	}
	// vertices are already registered in container
	// so we doesn't need "vers" for this time
	verts.clear	();
}

void	CBuild::xrPhase_UVmap()
{
	// Main loop
	Status					("Processing...");
	g_deflectors.reserve	(64*1024);
	float p_cost	= 1.f / float(g_XSplit.size());
	float p_total	= 0.f;
	for (int SP = 0; SP<int(g_XSplit.size()); SP++) 
	{
		Progress			(p_total+=p_cost);
		
		// Detect vertex-lighting and avoid this subdivision
		R_ASSERT	(!g_XSplit[SP]->empty());
		Face*		Fvl = g_XSplit[SP]->front();
		if (Fvl->Shader().flags.bLIGHT_Vertex) 	continue;	// do-not touch (skip)
		if (hasImplicitLighting(Fvl))			continue;
		
		//   find first poly that doesn't has mapping and start recursion
		while (TRUE) {
			// Select maximal sized poly
			Face *	msF		= NULL;
			float	msA		= 0;
			for (vecFaceIt it = g_XSplit[SP]->begin(); it!=g_XSplit[SP]->end(); it++)
			{
				if ( (*it)->pDeflector == NULL ) {
					float a = (*it)->CalcArea();
					if (a>msA) {
						msF = (*it);
						msA = a;
					}
				}
			}
			if (msF) {
				g_deflectors.push_back	(new CDeflector);
				
				// Start recursion from this face
				affected				= 1;
				Deflector->OA_SetNormal	(msF->N);
				msF->OA_Unwarp			();
				
				// break the cycle to startup again
				Deflector->OA_Export	();
				
				// Detach affected faces
				vecFace		faces_affected;
				faces_affected.reserve	(_MAX(256,affected));
				faces_affected.clear	();
				for (int i=0; i<int(g_XSplit[SP]->size()); i++) {
					Face *F = (*g_XSplit[SP])[i];
					if ( F->pDeflector==Deflector ) {
						faces_affected.push_back(F);
						g_XSplit[SP]->erase		(g_XSplit[SP]->begin()+i); 
						i--;
					}
				}
				
				// detaching itself
				Detach				(&faces_affected);
				g_XSplit.push_back	(new vecFace(faces_affected));
			} else {
				if (g_XSplit[SP]->empty()) 
				{
					_DELETE			(g_XSplit[SP]);
					g_XSplit.erase	(g_XSplit.begin()+SP);
					SP--;
				}
				// Cancel infine loop (while)
				break;
			}
		}
	}
	Msg("%d subdivisions...",g_XSplit.size());
	DWORD M1	= mem_Usage();
	mem_CompactSubdivs	();
	DWORD M2	= mem_Usage();
	Msg("Compact: %d / %d (%d)",M1,M2,M1-M2);
}

void CBuild::mem_CompactSubdivs()
{
	// Memory compact
	DWORD dwT = timeGetTime	();
	vecFace		temp;
	for (int SP = 0; SP<int(g_XSplit.size()); SP++) 
	{
		temp.clear			();
		temp.assign			(g_XSplit[SP]->begin(),g_XSplit[SP]->end());
		_DELETE				(g_XSplit[SP]);
		mem_Compact			();
		g_XSplit[SP]		= new vecFace;
		g_XSplit[SP]->assign(temp.begin(),temp.end());
	}
	Msg("%d ms for memory compacting...",timeGetTime()-dwT);
}
void CBuild::mem_Compact()
{
	_heapmin			();
	HeapCompact			(GetProcessHeap(),0);
}
DWORD CBuild::mem_Usage()
{
	_HEAPINFO		hinfo;
	int				heapstatus;
	hinfo._pentry	= NULL;
	DWORD	total	= 0;
	while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
	{ 
		if (hinfo._useflag == _USEDENTRY)	total += hinfo._size;
	}
	
	switch( heapstatus )
	{
	case _HEAPEMPTY:
//		Msg( "OK - empty heap\n" );
		break;
	case _HEAPEND:
//		Msg( "OK - end of heap\n" );
		break;
	case _HEAPBADPTR:
		Msg( "ERROR - bad pointer to heap\n" );
		break;
	case _HEAPBADBEGIN:
		Msg( "ERROR - bad start of heap\n" );
		break;
	case _HEAPBADNODE:
		Msg( "ERROR - bad node in heap\n" );
		break;
	}
	return total;
}
