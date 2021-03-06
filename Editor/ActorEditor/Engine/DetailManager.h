// DetailManager.h: interface for the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DETAILMANAGER_H__2C7B9CBD_4751_4D3E_8020_4792B800E4E2__INCLUDED_)
#define AFX_DETAILMANAGER_H__2C7B9CBD_4751_4D3E_8020_4792B800E4E2__INCLUDED_
#pragma once

#include "xr_list.h"
#include "detailformat.h"
#include "detailmodel.h"

const int		dm_max_objects	= 64;
const int		dm_obj_in_slot	= 4;
const int		dm_size			= 7;
const int		dm_cache_line	= 1+dm_size+1+dm_size+1;
const int		dm_cache_size	= dm_cache_line*dm_cache_line;

class ENGINE_API CDetailManager  
{
public:
	struct	SlotItem
	{
		Fvector	P;
		float	scale;
		float	phase_x;
		float	phase_z;
		DWORD	C;
		
		float	scale_calculated;
		Fmatrix	mRotY;
	};
	struct	SlotPart
	{
		DWORD				id;	
		CList<SlotItem>		items;
	};
	enum	SlotType
	{
		stReady		= 0,	// Ready to use
		stInvalid,			// Invalid cache entry
		stPending,			// Pending for decompression

		stFORCEDWORD = 0xffffffff
	};
	struct Slot 
	{
		DWORD				type;
		int					sx,sz;
		DWORD				dwFrameUsed;	// LRU cache
		Fbox				BB;
		SlotPart			G[dm_obj_in_slot];
	};
	struct VS_Model
	{
		IDirect3DVertexBuffer8*	VB;
		IDirect3DIndexBuffer8*	IB;
	};
public:	
	int						dither			[16][16];
public:
	CVirtualFileStream*		dtFS;
	DetailHeader			dtH;
	DetailSlot*				dtSlots;		// note: pointer into VFS
public:
	svector<CDetail,dm_max_objects>			objects;
	svector<Slot,dm_cache_size>				cache;
	svector<CList<SlotItem*>,dm_max_objects> visible;

public:
	IC bool					UseVS			()	{ return HW.Caps.vertex.dwVersion >= CAP_VERSION(1,1); }

	CVertexStream*			soft_VS;
	void					soft_Load		();
	void					soft_Unload		();
	void					soft_Render		();

	DWORD					VS_Code;
	DWORD					VS_BatchSize;
	svector<VS_Model,dm_max_objects>	VS_Models;		
	void					VS_Load			();
	void					VS_Unload		();
	void					VS_Render		();
public:
	void					Decompress		(int sx, int sz, Slot& D);
	Slot&					Query			(int sx, int sz);
	DetailSlot&				QueryDB			(int sx, int sz);
	void					UpdateCache		(int limit);
	
	void					Load			();
	void					Unload			();
	void					Render			(Fvector& EYE);

	CDetailManager			();
	virtual ~CDetailManager	();
};

#endif // !defined(AFX_DETAILMANAGER_H__2C7B9CBD_4751_4D3E_8020_4792B800E4E2__INCLUDED_)
