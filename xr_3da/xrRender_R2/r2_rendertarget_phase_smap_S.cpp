#include "stdafx.h"

void	CRenderTarget::phase_smap_spot	(light* L)
{
	// Targets + viewport
	if (RImplementation.b_HW_smap)		u_setrt	(rt_smap_d_surf, NULL, NULL, rt_smap_d_depth->pRT);
	else								u_setrt	(rt_smap_d_surf, NULL, NULL, rt_smap_d_ZB);
	D3DVIEWPORT9 VP		=	{L->X.S.posX,L->X.S.posY,L->X.S.size,L->X.S.size,0,1 };
	CHK_DX				(HW.pDevice->SetViewport(&VP));

	// Clear
	CHK_DX				(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER,	0xffffffff,	1.0f, 0L));

	// Stencil	- disable
	RCache.set_Stencil					( FALSE );

	// Misc		- draw only front-faces //back-faces
	RCache.set_CullMode					( CULL_CCW );
	if (RImplementation.b_HW_smap)		RCache.set_ColorWriteEnable	(FALSE);
}
