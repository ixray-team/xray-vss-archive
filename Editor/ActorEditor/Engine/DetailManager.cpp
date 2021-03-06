// DetailManager.cpp: implementation of the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "DetailManager.h"
#include "fstaticrender.h"
#include "xr_creator.h"
#include "collide\cl_intersect.h"

float		psDetailDensity		= 0.15f;

const float slot_size			= DETAIL_SLOT_SIZE;
const float slot_radius			= DETAIL_SLOT_SIZE*0.70710678118654752440084436210485f; // (slot_size/2)*sqrtf(2)

const float dbgOffset			= 0.f;
const int	dbgItems			= 128;

//--------------------------------------------------- Decompression
static int magic4x4[4][4] =  
{
 	 0, 14,  3, 13,
	11,  5,  8,  6,
	12,  2, 15,  1,
	 7,  9,  4, 10
};

void bwdithermap	(int levels, int magic[16][16] )
{
	/* Get size of each step */
    float N = 255.0f / (levels - 1);    
	
	/*
	* Expand 4x4 dither pattern to 16x16.  4x4 leaves obvious patterning,
	* and doesn't give us full intensity range (only 17 sublevels).
	* 
	* magicfact is (N - 1)/16 so that we get numbers in the matrix from 0 to
	* N - 1: mod N gives numbers in 0 to N - 1, don't ever want all
	* pixels incremented to the next level (this is reserved for the
	* pixel value with mod N == 0 at the next level).
	*/
	
    float	magicfact = (N - 1) / 16;
    for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			for ( int k = 0; k < 4; k++ )
				for ( int l = 0; l < 4; l++ )
					magic[4*k+i][4*l+j] =
					(int)(0.5 + magic4x4[i][j] * magicfact +
					(magic4x4[k][l] / 16.) * magicfact);
}
//--------------------------------------------------- Decompression
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDetailManager::CDetailManager	()
{
}

CDetailManager::~CDetailManager	()
{
	
}
/*
*/
void CDetailManager::Load		()
{
	// Open file stream
	FILE_NAME	fn;
	if (!Engine.FS.Exist(fn,Path.Current,"level.details"))
	{
		dtFS	= NULL;
		return;
	}
	dtFS		= new CVirtualFileStream(fn);
	
	// Header
	dtFS->ReadChunkSafe	(0,&dtH,sizeof(dtH));
	R_ASSERT			(dtH.version == DETAIL_VERSION);
	DWORD m_count		= dtH.object_count;
	
	// Models
	CStream* m_fs	= dtFS->OpenChunk(1);
	for (DWORD m_id = 0; m_id < m_count; m_id++)
	{
		CDetail				dt;
		CStream* S			= m_fs->OpenChunk(m_id);
		dt.Load				(S);
		objects.push_back	(dt);
		S->Close			();
	}
	m_fs->Close		();
	
	// Get pointer to database (slots)
	CStream* m_slots= dtFS->OpenChunk(2);
	dtSlots			= (DetailSlot*)m_slots->Pointer();
	m_slots->Close	();
	
	// Initialize 'vis' and 'cache'
	ZeroMemory(&visible,sizeof(visible));	visible.resize	(dm_max_objects);
	Slot*	slt = s_pool;
	for (DWORD i=0; i<dhm_matrix; i++)
		for (DWORD j=0; j<dhm_matrix; j++, slt++)
			s_data	[i][j]	= slt;
	c_x=c_z=0;
	s_task.clear();

	// Make dither matrix
	bwdithermap		(2,dither);

	// Hardware specific optimizations
	if (UseVS())	VS_Load		();
	else			soft_Load	();
	 
}

void CDetailManager::Unload		()
{
	if (UseVS())	VS_Unload	();
	else			soft_Unload	();
	
	for (DWORD it=0; it<objects.size(); it++)
		objects[it].Unload();
	objects.clear	();
	visible.clear	();
	_DELETE			(dtFS);
}

extern float g_fSCREEN;
extern float ssaLIMIT;

void CDetailManager::Render		(Fvector& EYE)
{
	if (0==dtFS)	return;

	int s_x	= iFloor			(EYE.x/slot_size+.5f);
	int s_z	= iFloor			(EYE.z/slot_size+.5f);

	UpdateCache					(s_x,s_z,4);

	float fade_limit			= 14.5f;fade_limit=fade_limit*fade_limit;
	float fade_start			= 1.f;	fade_start=fade_start*fade_start;
	float fade_range			= fade_limit-fade_start;

	// Collect objects for rendering
	Device.Statistic.RenderDUMP_DT_VIS.Begin	();
	for (int _z=s_z-dm_size; _z<=(s_z+dm_size); _z++)
	{
		for (int _x=s_x-dm_size; _x<=(s_x+dm_size); _x++)
		{
			// Query for slot
			Device.Statistic.RenderDUMP_DT_Cache.Begin	();
			Slot&	S		= Query						(_x,_z);
			Device.Statistic.RenderDUMP_DT_Cache.End	();
			if (!S.bReady)	continue;

			// Transfer visibile and partially visible slot contents
			BYTE mask		= 0xff;
			switch (::Render.ViewBase.testAABB(S.BB.min,S.BB.max,mask))
			{
			case fcvNone:		// nothing to do
				break;
			case fcvPartial:	// addition with TEST
				{
					for (int sp_id=0; sp_id<dm_obj_in_slot; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==0xff)	continue;
						CList<SlotItem*>&	vis = visible	[sp.id];
						float				R   = objects	[sp.id].bounds.R;
						
						SlotItem			*siIT=sp.items.begin(), *siEND=sp.items.end();
						for (; siIT!=siEND; siIT++) 
						{
							SlotItem& Item	= *siIT;

							float	dist_sq = EYE.distance_to_sqr(Item.P);
							if (dist_sq>fade_limit)	continue;
							
							if (::Render.ViewBase.testSphereDirty(siIT->P,R*Item.scale))	
							{
								float	alpha	= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
								float	scale	= Item.scale*(1-alpha);
								float	radius	= R*scale;
								
								if (g_fSCREEN*radius*radius/dist_sq < ssaLIMIT) continue;

								Item.scale_calculated = scale; //alpha;
								vis.push_back	(siIT);
							}
						}
					}
				}
				break;
			case fcvFully:		// addition
				{
					for (int sp_id=0; sp_id<3; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==0xff)	continue;
						CList<SlotItem*>&	vis = visible	[sp.id];
						float				R   = objects	[sp.id].bounds.R;
						
						SlotItem			*siIT=sp.items.begin(), *siEND=sp.items.end();
						for (; siIT!=siEND; siIT++) 
						{
							SlotItem& Item	= *siIT;

							float	dist_sq = EYE.distance_to_sqr(Item.P);
							if (dist_sq>fade_limit)	continue;
							
							float	alpha	= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
							float	scale	= Item.scale*(1-alpha);
							float	radius	= R*scale;

							if (g_fSCREEN*radius*radius/dist_sq < ssaLIMIT) continue;

							Item.scale_calculated = scale; //alpha;
							vis.push_back	(siIT);
						}
					}
				}
				break;
			}
		}
	}
	Device.Statistic.RenderDUMP_DT_VIS.End	();

	Device.Statistic.RenderDUMP_DT_Render.Begin	();
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE));
	if (UseVS())	VS_Render	();
	else			soft_Render	();
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW));
	Device.Statistic.RenderDUMP_DT_Render.End	();
}

CDetailManager::Slot&	CDetailManager::Query	(int sx, int sz)
{
	static Slot	empty;
	empty.bReady	= FALSE;

	// Search cache
	int rx	= sx-c_x + dhm_line;
	if (rx<0 || rx>=dhm_matrix)		return empty;
	int rz  = sz-c_z + dhm_line;
	if (rz<0 || rz>=dhm_matrix)		return empty;

	// Check for unpacked state
	return	*s_data[rz][rx];
}

//--------------------------------------------------- Decompression
IC float	Interpolate			(float* base,		DWORD x, DWORD y, DWORD size)
{
	float	f	= float(size);
	float	fx	= float(x)/f; float ifx = 1.f-fx;
	float	fy	= float(y)/f; float ify = 1.f-fy;
	
	float	c01	= base[0]*ifx + base[1]*fx;
	float	c23	= base[2]*ifx + base[3]*fx;
	float	c02	= base[0]*ify + base[2]*fy;
	float	c13	= base[1]*ify + base[3]*fy;
	
	float	cx	= ify*c01 + fy*c23;
	float	cy	= ifx*c02 + fx*c13;
	return	(cx+cy)/2;
}

IC bool		InterpolateAndDither(float* alpha255,	DWORD x, DWORD y, DWORD sx, DWORD sy, DWORD size, int dither[16][16] )
{
	clamp 	(x,0ul,size-1);
	clamp 	(y,0ul,size-1);
	int		c	= iFloor(Interpolate(alpha255,x,y,size)+.5f);
    clamp   (c,0,255);

	DWORD	row	= (y+sy) % 16;
	DWORD	col	= (x+sx) % 16;
 	return	c	> dither[col][row];
}

const	float phase_range = PI/16;

void CDetailManager::UpdateCache	(int v_x, int v_z, int limit)
{
	// *****	SCROLL
	if (v_x!=c_x)	{
		if (v_x>c_x)	{
			// scroll matrix to left
			c_x ++;
			for (int z=0; z<dhm_matrix; z++)
			{
				Slot*	S	= s_data[z][0];
				if (S->bReady)	{	S->bReady = FALSE; s_task.push_back(S); }
				for (int x=1; x<dhm_matrix; x++)	s_data[z][x-1] = s_data[z][x];
				s_data[z][dhm_matrix-1] = S;
				S->set	(c_x-dhm_line+dhm_matrix-1, c_z-dhm_line+z);
			}
		} else {
			// scroll matrix to right
			c_x --;
			for (int z=0; z<dhm_matrix; z++)
			{
				Slot*	S	= s_data[z][dhm_matrix-1];
				if (S->bReady)	{	S->bReady = FALSE; s_task.push_back(S); }
				for (int x=dhm_matrix-1; x>0; x--)	s_data[z][x] = s_data[z][x-1]; 
				s_data[z][0]	= S;
				S->set	(c_x-dhm_line+0,c_z-dhm_line+z);
			}
		}
	}
	if (v_z!=c_z)	{
		if (v_z>c_z)	{
			// scroll matrix down a bit
			c_z ++;
			for (int x=0; x<dhm_matrix; x++)
			{
				Slot*	S	= s_data[dhm_matrix-1][x];
				if (S->bReady)	{	S->bReady = FALSE; s_task.push_back(S); }
				for (int z=dhm_matrix-1; z>0; z--)	s_data[z][x] = s_data[z-1][x];
				s_data[0][x]	= S;
				S->set	(c_x-dhm_line+x,c_z-dhm_line+0);
			}
		} else {
			// scroll matrix up
			c_z --;
			for (int x=0; x<dhm_matrix; x++)
			{
				Slot*	S = s_data[0][x];
				if (S->bReady)	{	S->bReady = FALSE; s_task.push_back(S); }
				for (int z=0; z<dhm_matrix; z++)	s_data[z-1][x] = s_data[z][x];
				s_data[dhm_matrix-1][x]	= S;
				S->set	(c_x-dhm_line+x,c_z-dhm_line+dhm_matrix-1);
			}
		}
	}
	
	// *****	perform TASKs
	for (int taskid=0; (taskid<limit) && (!s_task.empty()); taskid++)
	{
		// Gain access to data
		Slot&	D		= *(s_task.back());	s_task.pop_back();
		DetailSlot&	DS	= QueryDB(D.x,D.z);
		D.BB.min.set	(D.x*slot_size,	DS.y_min, D.z*slot_size);
		D.BB.max.set	(D.BB.min.x+slot_size,	DS.y_max,	D.BB.min.z+slot_size);
		D.BB.grow		(EPS_L);
		D.bReady		= TRUE;

		// Select polygons
		XRC.BBoxMode		(0); // BBOX_TRITEST
		XRC.BBoxCollide		(precalc_identity,pCreator->ObjectSpace.GetStaticModel(),precalc_identity,D.BB);
		DWORD	triCount	= XRC.GetBBoxContactCount();
		if (0==triCount)	continue;
		RAPID::tri* tris	= pCreator->ObjectSpace.GetStaticTris();
		
		// Build shading table
		float		alpha255	[dm_obj_in_slot][4];
		for (int i=0; i<dm_obj_in_slot; i++)
		{
			D.G[i].id			= DS.items[i].id;
			D.G[i].items.clear	();
			alpha255[i][0]		= 255.f*float(DS.items[i].palette.a0)/15.f;
			alpha255[i][1]		= 255.f*float(DS.items[i].palette.a1)/15.f;
			alpha255[i][2]		= 255.f*float(DS.items[i].palette.a2)/15.f;
			alpha255[i][3]		= 255.f*float(DS.items[i].palette.a3)/15.f;
		}
		
		// Prepare to selection
		float		density		= psDetailDensity;
		float		jitter		= density/2.f;
		DWORD		d_size		= iCeil	(slot_size/density);
		svector<int,dm_obj_in_slot>		selected;
		
		CRandom				r_selection	(0x12071980);
		CRandom				r_jitter	(0x12071980);
		CRandom				r_yaw		(0x12071980);
		CRandom				r_scale		(0x12071980);
		
		// Decompressing itself
		for (DWORD z=0; z<=d_size; z++)
		{
			for (DWORD x=0; x<=d_size; x++)
			{
				// shift
                DWORD shift_x =  r_jitter.randI(16);
                DWORD shift_z =  r_jitter.randI(16);
				
				// Iterpolate and dither palette
				selected.clear();
				if ((DS.items[0].id!=0xff)&& InterpolateAndDither(alpha255[0],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(0);
				if ((DS.items[1].id!=0xff)&& InterpolateAndDither(alpha255[1],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(1);
				if ((DS.items[2].id!=0xff)&& InterpolateAndDither(alpha255[2],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(2);
				if ((DS.items[3].id!=0xff)&& InterpolateAndDither(alpha255[3],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(3);
				
				// Select 
				if (selected.empty())	continue;
				DWORD index;
				if (selected.size()==1)	index = selected[0];
				else					index = selected[r_selection.randI(selected.size())]; 
				
				CDetail&	Dobj = objects[DS.items[index].id];
				SlotItem	Item;
				
				// Position (XZ)
				float		rx = (float(x)/float(d_size))*slot_size + D.BB.min.x;
				float		rz = (float(z)/float(d_size))*slot_size + D.BB.min.z;
				Item.P.set	(rx + r_jitter.randFs(jitter), D.BB.max.y, rz + r_jitter.randFs(jitter));
				
				// Position (Y)
				float y		= D.BB.min.y-5;
				Fvector	dir; dir.set(0,-1,0);
				
				float		r_u,r_v,r_range;
				for (DWORD tid=0; tid<triCount; tid++)
				{
					RAPID::tri&	T		= tris[XRC.BBoxContact[tid].id];
					if (RAPID::TestRayTri(Item.P,dir,T.verts,r_u,r_v,r_range,TRUE))
					{
						if (r_range>=0)	{
							float y_test	= Item.P.y - r_range;
							if (y_test>y)	y = y_test;
						}
					}
				}
				if (y<D.BB.min.y)	continue;
				Item.P.y	= y;
				
				// Angles and scale
				Item.scale	= r_scale.randF		(Dobj.s_min,Dobj.s_max);
				Item.phase_x= ::Random.randFs	(phase_range);
				Item.phase_z= ::Random.randF	(phase_range);
				Item.mRotY.rotateY(r_yaw.randF	(0,PI_MUL_2));
				
				// Color
				DetailPalette*	c_pal	= (DetailPalette*)&DS.color;
				float gray255	[4];
				gray255[0]		= 255.f*float(c_pal->a0)/15.f;
				gray255[1]		= 255.f*float(c_pal->a1)/15.f;
				gray255[2]		= 255.f*float(c_pal->a2)/15.f;
				gray255[3]		= 255.f*float(c_pal->a3)/15.f;
				int c_dw		= iFloor(Interpolate(gray255,x,z,d_size)+.5f);
				clamp			(c_dw,0,255);
				Item.C			= D3DCOLOR_RGBA		(c_dw,c_dw,c_dw,255);
				
				// Save it
				D.G[index].items.push_back(Item);
			}
		}
	}		
}

DetailSlot&	CDetailManager::QueryDB(int sx, int sz)
{
	static DetailSlot	DS;

	int db_x = sx+dtH.offs_x;
	int db_z = sz+dtH.offs_z;
	if ((db_x>=0) && (db_x<int(dtH.size_x)) && (db_z>=0) && (db_z<int(dtH.size_z)))
	{
		DWORD linear_id			= db_z*dtH.size_x + db_x;
		return dtSlots			[linear_id];
	} else {
		// Empty slot
		DS.y_min				= 0;
		DS.y_max				= EPS_L;
		
		DS.items[0].id			= 0xff;
		DS.items[1].id			= 0xff;
		DS.items[2].id			= 0xff;
		DS.items[3].id			= 0xff;
		
		DS.color				= 0xffff;
		return DS;
	}
}
