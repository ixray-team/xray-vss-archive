#include "stdafx.h"
#include "..\fbasicvisual.h"
#include "..\customhud.h"
#include "..\irenderable.h"

IC	bool	pred_sp_sort	(ISpatial* _1, ISpatial* _2)
{
	float	d1		= _1->spatial.center.distance_to_sqr(Device.vCameraPosition);
	float	d2		= _2->spatial.center.distance_to_sqr(Device.vCameraPosition);
	return	d1<d2;
}

void CRender::Render		()
{
	VERIFY					(g_pGameLevel && g_pGameLevel->pHUD);

	//******* Main calc
	Device.Statistic.RenderCALC.Begin		();
	{
		marker									++;
		phase									= PHASE_NORMAL;

		// Frustum & HOM rendering
		ViewBase.CreateFromMatrix				(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
		View									= 0;
		HOM.Enable								();
		HOM.Render								(ViewBase);

		// Calculate sector(s) and their objects
		if (pLastSector)
		{
			// Traverse sector/portal structure
			PortalTraverser.traverse	
				(
				pLastSector,
				ViewBase,
				Device.vCameraPosition,
				Device.mFullTransform,
				CPortalTraverser::VQ_HOM + CPortalTraverser::VQ_SSA
				);

			// Determine visibility for static geometry hierrarhy
			for (u32 s_it=0; s_it<PortalTraverser.r_sectors.size(); s_it++)
			{
				CSector*	sector		= (CSector*)PortalTraverser.r_sectors[s_it];
				IRender_Visual*	root	= sector->root();
				for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)	{
					set_Frustum			(&(sector->r_frustums[v_it]));
					add_Geometry		(root);
				}
			}

			// Traverse object database
			g_SpatialSpace->q_frustum
				(
				ISpatial_DB::O_ORDERED,
				STYPE_RENDERABLE + STYPE_LIGHTSOURCE,
				ViewBase
				);

			// (almost)Exact sorting order (front-to-back)
			lstRenderables.swap	(g_SpatialSpace->q_result);
			std::sort			(lstRenderables.begin(),lstRenderables.end(),pred_sp_sort);

			// Determine visibility for dynamic part of scene
			set_Object							(0);
			//. g_pGameLevel->pHUD->Render_First( );
			for (u32 o_it=0; o_it<lstRenderables.size(); o_it++)
			{
				ISpatial*	spatial		= lstRenderables[o_it];
				CSector*	sector		= (CSector*)spatial->spatial.sector;
				if	(0==sector)										continue;	// disassociated from S/P structure
				if	(PortalTraverser.i_marker != sector->r_marker)	continue;	// inactive (untouched) sector
				for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)
				{
					CFrustum&	view	= sector->r_frustums[v_it];
					if (!view.testSphere_dirty(spatial->spatial.center,spatial->spatial.radius))	continue;

					if (spatial->spatial.type & STYPE_RENDERABLE)
					{
						// renderable
						IRenderable*	renderable		= dynamic_cast<IRenderable*>(spatial);
						VERIFY							(renderable);

						// Occlusion
						vis_data&		v_orig			= renderable->renderable.visual->vis;
						vis_data		v_copy			= v_orig;
						v_copy.box.xform				(renderable->renderable.xform);
						BOOL			bVisible		= HOM.visible(v_copy);
						v_orig.frame					= v_copy.frame;
						v_orig.hom_frame				= v_copy.hom_frame;
						v_orig.hom_tested				= v_copy.hom_tested;
						if (!bVisible)					break;	// exit loop on frustums

						// Rendering
						set_Object						(renderable);
						renderable->renderable_Render	();
						set_Object						(0);
					}
					else 
					{
						VERIFY							(spatial->spatial.type & STYPE_LIGHTSOURCE);
						// lightsource
						light*			L				= dynamic_cast<light*>		(spatial);
						VERIFY							(L);
						Lights.add_light				(L);
					}
					
					break;	// exit loop on frustums
				}
			}
			g_pGameLevel->pHUD->Render_Last						();	
		}
		else
		{
			set_Object											(0);
			g_pGameLevel->pHUD->Render_First					();	
			g_pGameLevel->pHUD->Render_Last						();	
		}
	}
	Device.Statistic.RenderCALC.End				();

	//******* Main render
	{
		// level
		Target.phase_scene						();
		r_dsgraph_render_hud					();
		r_dsgraph_render_graph					(0);
		Details.Render							();
		// mapSorted.traverseRL					(sorted_L1);
		mapSorted.clear							();		// unsupported
	}

	//******* Decompression on some HW :)
	Target.phase_decompress						();

	//******* Directional light
	/*
	if (0 && ps_r2_ls_flags.test(R2FLAG_SUN) )
	{
		for (u32 dls_phase=0; dls_phase<2; dls_phase++)
		{
			//******* Direct lighting+shadow		::: Calculate
			Device.Statistic.RenderCALC.Begin		();
			{
				marker									++;
				phase									= PHASE_SMAP_D;

				HOM.Disable								();
				LR.compute_xfd_1						(dls_phase);
				render_smap_direct						(LR.L_combine);
				LR.compute_xfd_2						(dls_phase);
			}
			Device.Statistic.RenderCALC.End			();

			//******* Direct lighting+shadow		::: Render
			{
				Target.phase_smap_direct				();

				RCache.set_xform_world					(Fidentity);
				RCache.set_xform_view					(LR.L_view);
				RCache.set_xform_project				(LR.L_project);
				r_dsgraph_render_graph					(0);
			}

			//******* Direct lighting+shadow		::: Accumulate
			{
				Target.phase_accumulator				();
				Target.shadow_direct					(dls_phase);
			}
		}

		// Multiply by lighting contribution
		Target.phase_accumulator				();
		Target.accum_direct						();
	}
	*/

	// $$$
	// Target.phase_accumulator			();
	HOM.Disable							();
	// Target.accum_direct				();

	// sort lights by importance???
	// while (has_any_lights_that_cast_shadows) {
	//		if (has_point_shadowed)		->	generate point shadowmap
	//		if (has_spot_shadowed)		->	generate spot shadowmap
	//		switch-to-accumulator
	//		if (has_point_unshadowed)	-> 	accum point unshadowed
	//		if (has_spot_unshadowed)	-> 	accum spot unshadowed
	//		if (was_point_shadowed)		->	accum point shadowed
	//		if (was_spot_shadowed)		->	accum spot shadowed
	//	}
	//	if (left_some_lights_that_doesn't cast shadows)
	//		accumulate them

	/*
	Msg		(
		"total(%d), ps(%d), ss(%d), p(%d), s(%d)",
		Lights.v_point_s.size()+Lights.v_spot_s.size()+Lights.v_point.size()+Lights.v_spot.size(),
		Lights.v_point_s.size(),Lights.v_spot_s.size(),Lights.v_point.size(),Lights.v_spot.size()
	);
	*/

	while	(Lights.v_point_s.size() || Lights.v_spot_s.size() )
	{
		// if (has_point_shadowed)
		light*	L_point_s	= 0;	
		if		(!Lights.v_point_s.empty())	{
			// generate point shadowmap
			light*	L	= L_point_s	= Lights.v_point_s.back();	Lights.v_point_s.pop_back();
			for (u32 pls_phase=0; pls_phase<6; pls_phase++)		{
				phase									= PHASE_SMAP_P;
				if (!LR.compute_xfp_1(pls_phase, L))	continue;	// frustum doesn't touch primary frustum
				r_dsgraph_render_subspace				(L->spatial.sector, LR.P_combine, L->position, TRUE);
				LR.compute_xfp_2						(pls_phase, L);
				if (mapNormal[0].size() || mapMatrix[0].size())	{
					Target.phase_smap_point				(pls_phase);
					RCache.set_xform_world				(Fidentity);
					RCache.set_xform_view				(LR.P_view);
					RCache.set_xform_project			(LR.P_project);
					r_dsgraph_render_graph				(0);
				}
			}
		}

		// if (has_spot_shadowed)
		light*	L_spot_s	= 0;	
		if		(!Lights.v_spot_s.empty())	{
			// generate spot shadowmap
			light*	L	= L_spot_s	= Lights.v_spot_s.back();	Lights.v_spot_s.pop_back();
			phase									= PHASE_SMAP_S;
			LR.compute_xfs_1						(0, L);
			r_dsgraph_render_subspace				(L->spatial.sector, LR.S_combine, L->position, TRUE);
			LR.compute_xfs_2						(0, L);
			if (mapNormal[0].size() || mapMatrix[0].size())	{
				Target.phase_smap_spot				();
				RCache.set_xform_world				(Fidentity);
				RCache.set_xform_view				(LR.S_view);
				RCache.set_xform_project			(LR.S_project);
				r_dsgraph_render_graph				(0);
			}
		}

		//		switch-to-accumulator
		Target.phase_accumulator			();
		HOM.Disable							();

		//		if (has_point_unshadowed)	-> 	accum point unshadowed
		if		(!Lights.v_point.empty())	{
				Target.accum_point			(Lights.v_point.back());
				Lights.v_point.pop_back		();
		}

		//		if (has_spot_unshadowed)	-> 	accum spot unshadowed
		if		(!Lights.v_spot.empty())	{
			Target.accum_point				(Lights.v_spot.back());
			Lights.v_spot.pop_back			();
		}

		//		if (was_point_shadowed)		->	accum point shadowed
		if		(L_point_s)					Target.accum_point			(L_point_s);

		//		if (was_spot_shadowed)		->	accum spot shadowed
		if		(L_spot_s)					Target.accum_spot			(L_spot_s);
	}

	// Point lighting (unshadowed, if left)
	if (!Lights.v_point.empty())		{
		xr_vector<light*>&	Lvec		= Lights.v_point;
		for	(u32 pid=0; pid<Lvec.size(); pid++)	Target.accum_point		(Lvec[pid]);
		Lvec.clear	();
	}

	// Spot lighting (unshadowed, if left)
	if (!Lights.v_spot.empty())		{
		xr_vector<light*>&	Lvec		= Lights.v_spot;
		for	(u32 pid=0; pid<Lvec.size(); pid++)	Target.accum_spot		(Lvec[pid]);
		Lvec.clear	();
	}

	// Postprocess
	// Target.phase_bloom					();
	Target.phase_combine					();
	
	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	g_pGameLevel->pHUD->Render_Direct		();
	Device.Statistic.RenderDUMP_HUD.End		();
}
