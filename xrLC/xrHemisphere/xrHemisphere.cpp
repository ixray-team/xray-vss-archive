// xrHemisphere.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "xrHemisphere.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       u32  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


#define HEMI1_LIGHTS	26
#define HEMI1_LIGHTS_F	42
#define HEMI2_LIGHTS	91
#define HEMI2_LIGHTS_F	162

const double hemi_1[HEMI1_LIGHTS][3] = 
{
	{0.00000,	1.00000,	0.00000	},
	{0.52573,	0.85065,	0.00000	},
	{0.16246,	0.85065,	0.50000	},
	{-0.42533,	0.85065,	0.30902	},
	{-0.42533,	0.85065,	-0.30902},
	{0.16246,	0.85065,	-0.50000},
	{0.89443,	0.44721,	0.00000	},
	{0.27639,	0.44721,	0.85065	},
	{-0.72361,	0.44721,	0.52573	},
	{-0.72361,	0.44721,	-0.52573},
	{0.27639,	0.44721,	-0.85065},
	{0.68819,	0.52573,	0.50000	},
	{-0.26287,	0.52573,	0.80902	},
	{-0.85065,	0.52573,	-0.00000},
	{-0.26287,	0.52573,	-0.80902},
	{0.68819,	0.52573,	-0.50000},
	{0.95106,	0.00000,	0.30902	},
	{0.58779,	0.00000,	0.80902	},
	{-0.00000,	0.00000,	1.00000	},
	{-0.58779,	0.00000,	0.80902	},
	{-0.95106,	0.00000,	0.30902	},
	{-0.95106,	0.00000,	-0.30902},
	{-0.58779,	0.00000,	-0.80902},
	{0.00000,	0.00000,	-1.00000},
	{0.58779,	0.00000,	-0.80902},
	{0.95106,	0.00000,	-0.30902}
};

const double hemi_1F[HEMI1_LIGHTS_F][3] = 
{
	{0.00000,	0.00000,	1.00000	},
	{0.89443,	0.00000,	0.44721	},
	{0.27639,	0.85065,	0.44721	},
	{-0.72361,	0.52573,	0.44721 },
	{-0.72361,	-0.52573,	0.44721 },
	{0.27639,	-0.85065,	0.44721	},
	{0.72361,	0.52573,	-0.44721},
	{-0.27639,	0.85065,	-0.44721},
	{-0.89443,	-0.00000,	-0.44721},
	{-0.27639,	-0.85065,	-0.44721},
	{0.72361,	-0.52573,	-0.44721},
	{0.00000,	0.00000,	-1.00000},
	{0.52573,	0.00000,	0.85065	},
	{0.16246,	0.50000,	0.85065	},
	{-0.42533,	0.30902,	0.85065	},
	{-0.42533,	-0.30902,	0.85065	},
	{0.16246,	-0.50000,	0.85065	},
	{0.68819,	0.50000,	0.52573	},
	{-0.26287,	0.80902,	0.52573	},
	{-0.85065,	-0.00000,	0.52573	},
	{-0.26287,	-0.80902,	0.52573	},
	{0.68819,	-0.50000,	0.52573	},
	{0.95106,	0.30902,	0.00000	},
	{-0.00000,	1.00000,	0.00000	},
	{-0.95106,	0.30902,	0.00000	},
	{-0.58779,	-0.80902,	0.00000	},
	{0.58779,	-0.80902,	0.00000	},
	{0.95106,	-0.30902,	0.00000	},
	{0.58779,	0.80902,	0.00000	},
	{-0.58779,	0.80902,	0.00000	},
	{-0.95106,	-0.30902,	0.00000	},
	{0.00000,	-1.00000,	0.00000	},
	{0.26287,	0.80902,	-0.52573},
	{-0.68819,	0.50000,	-0.52573},
	{-0.68819,	-0.50000,	-0.52573},
	{0.26287,	-0.80902,	-0.52573},
	{0.85065,	0.00000,	-0.52573},
	{0.42533,	0.30902,	-0.85065},
	{-0.16246,	0.50000,	-0.85065},
	{-0.52573,	-0.00000,	-0.85065},
	{-0.16246,	-0.50000,	-0.85065},
	{0.42533,	-0.30902,	-0.85065}
};

const double hemi_2[HEMI2_LIGHTS][3] = 
{
	{0.00000,	0.00000,	1.00000},
	{0.52573,	0.00000,	0.85065},
	{0.16246,	0.50000,	0.85065},
	{-0.42533,	0.30902,	0.85065},
	{-0.42533,	-0.30902,	0.85065},
	{0.16246,	-0.50000,	0.85065},
	{0.89443,	0.00000,	0.44721},
	{0.27639,	0.85065,	0.44721},
	{-0.72361,	0.52573,	0.44721},
	{-0.72361,	-0.52573,	0.44721},
	{0.27639,	-0.85065,	0.44721},
	{0.68819,	0.50000	,	0.52573},
	{-0.26287,	0.80902	,	0.52573},
	{-0.85065,	-0.00000,	0.52573},
	{-0.26287,	-0.80902,	0.52573},
	{0.68819,	-0.50000,	0.52573},
	{0.95106,	0.30902	,	0.00000},
	{0.58779,	0.80902	,	0.00000},
	{-0.00000,	1.00000	,	0.00000},
	{-0.58779,	0.80902	,	0.00000},
	{-0.95106,	0.30902	,	0.00000},
	{-0.95106,	-0.30902,	0.00000},
	{-0.58779,	-0.80902,	0.00000},
	{0.00000,	-1.00000,	0.00000},
	{0.58779,	-0.80902,	0.00000},
	{0.95106,	-0.30902,	0.00000},
	{0.27327,	0.00000	,	0.96194},
	{0.08444,	0.25989	,	0.96194},
	{-0.22108,	0.16062	,	0.96194},
	{-0.22108,	-0.16062,	0.96194},
	{0.08444,	-0.25989,	0.96194},
	{0.36180,	0.26287	,	0.89443},
	{-0.13820,	0.42533	,	0.89443},
	{-0.44721,	-0.00000,	0.89443},
	{-0.13820,	-0.42533,	0.89443},
	{0.36180,	-0.26287,	0.89443},
	{0.73818,	0.00000	,	0.67461},
	{0.22811,	0.70205	,	0.67461},
	{-0.59720,	0.43389	,	0.67461},
	{-0.59720,	-0.43389,	0.67461},
	{0.22811,	-0.70205,	0.67461},
	{0.63820,	0.26287	,	0.72361},
	{-0.05279,	0.68819	,	0.72361},
	{-0.67082,	0.16246	,	0.72361},
	{-0.36180,	-0.58779,	0.72361},
	{0.44721,	-0.52573,	0.72361},
	{0.63820,	-0.26287,	0.72361},
	{0.44721,	0.52573	,	0.72361},
	{-0.36180,	0.58779	,	0.72361},
	{-0.67082,	-0.16246,	0.72361},
	{-0.05279,	-0.68819,	0.72361},
	{0.82262,	0.25989	,	0.50572},
	{0.00703,	0.86267	,	0.50572},
	{-0.81827,	0.27327	,	0.50572},
	{-0.51275,	-0.69378,	0.50572},
	{0.50138,	-0.70205,	0.50572},
	{0.82262,	-0.25989,	0.50572},
	{0.50138,	0.70205	,	0.50572},
	{-0.51275,	0.69378	,	0.50572},
	{-0.81827,	-0.27327,	0.50572},
	{0.00703,	-0.86267,	0.50572},
	{0.95925,	0.16062	,	0.23245},
	{0.86180,	0.42533	,	0.27639},
	{0.67082,	0.68819	,	0.27639},
	{0.44919,	0.86267	,	0.23245},
	{0.14366,	0.96194	,	0.23245},
	{-0.13820,	0.95106	,	0.27639},
	{-0.44721,	0.85065	,	0.27639},
	{-0.68164,	0.69378	,	0.23245},
	{-0.87046,	0.43389	,	0.23245},
	{-0.94721,	0.16246	,	0.27639},
	{-0.94721,	-0.16246,	0.27639},
	{-0.87046,	-0.43389,	0.23245},
	{-0.68164,	-0.69378,	0.23245},
	{-0.44721,	-0.85065,	0.27639},
	{-0.13820,	-0.95106,	0.27639},
	{0.14366,	-0.96194,	0.23245},
	{0.44919,	-0.86267,	0.23245},
	{0.67082,	-0.68819,	0.27639},
	{0.86180,	-0.42533,	0.27639},
	{0.95925,	-0.16062,	0.23245},
	{0.80902,	0.58779	,	0.00000},
	{0.30902,	0.95106	,	0.00000},
	{-0.30902,	0.95106	,	0.00000},
	{-0.80902,	0.58779	,	0.00000},
	{-1.00000,	-0.00000,	0.00000},
	{-0.80902,	-0.58779,	0.00000},
	{-0.30902,	-0.95106,	0.00000},
	{0.30902,	-0.95106,	0.00000},
	{0.80902,	-0.58779,	0.00000},
	{1.00000,	0.00000	,	0.00000}
};

const double hemi_2F[HEMI2_LIGHTS_F][3] = 
{
	{0.00000	,0.00000	,1.00000	},
	{0.89443	,0.00000	,0.44721},
	{0.27639	,0.85065	,0.44721},
	{-0.72361	,0.52573	,0.44721},
	{-0.72361	,-0.52573	,0.44721},
	{0.27639	,-0.85065	,0.44721},
	{0.72361	,0.52573	,-0.44721},
	{-0.27639	,0.85065	,-0.44721},
	{-0.89443	,-0.00000	,-0.44721},
	{-0.27639	,-0.85065	,-0.44721},
	{0.72361	,-0.52573	,-0.44721},
	{0.00000	,0.00000	,-1.00000},
	{0.27327	,0.00000	,0.96194},
	{0.52573	,0.00000	,0.85065},
	{0.73818	,0.00000	,0.67461},
	{0.08444	,0.25989	,0.96194},
	{0.16246	,0.50000	,0.85065},
	{0.22811	,0.70205	,0.67461},
	{-0.22108	,0.16062	,0.96194},
	{-0.42533	,0.30902	,0.85065},
	{-0.59720	,0.43389	,0.67461},
	{-0.22108	,-0.16062	,0.96194},
	{-0.42533	,-0.30902	,0.85065},
	{-0.59720	,-0.43389	,0.67461},
	{0.08444	,-0.25989	,0.96194},
	{0.16246	,-0.50000	,0.85065},
	{0.22811	,-0.70205	,0.67461},
	{0.82262	,0.25989	,0.50572},
	{0.68819	,0.50000	,0.52573},
	{0.50138	,0.70205	,0.50572},
	{0.00703	,0.86267	,0.50572},
	{-0.26287	,0.80902	,0.52573},
	{-0.51275	,0.69378	,0.50572},
	{-0.81827	,0.27327	,0.50572},
	{-0.85065	,-0.00000	,0.52573},
	{-0.81827	,-0.27327	,0.50572},
	{-0.51275	,-0.69378	,0.50572},
	{-0.26287	,-0.80902	,0.52573},
	{0.00703	,-0.86267	,0.50572},
	{0.50138	,-0.70205	,0.50572},
	{0.68819	,-0.50000	,0.52573},
	{0.82262	,-0.25989	,0.50572},
	{0.95925	,0.16062	,0.23245	},
	{0.95106	,0.30902	,0.00000},
	{0.87046	,0.43389	,-0.23245},
	{0.14366	,0.96194	,0.23245},
	{-0.00000	,1.00000	,0.00000},
	{-0.14366	,0.96194	,-0.23245},
	{-0.87046	,0.43389	,0.23245},
	{-0.95106	,0.30902	,0.00000},
	{-0.95925	,0.16062	,-0.23245},
	{-0.68164	,-0.69378	,0.23245},
	{-0.58779	,-0.80902	,0.00000},
	{-0.44919	,-0.86267	,-0.23245},
	{0.44919	,-0.86267	,0.23245},
	{0.58779	,-0.80902	,0.00000},
	{0.68164	,-0.69378	,-0.23245},
	{0.95925	,-0.16062	,0.23245},
	{0.95106	,-0.30902	,0.00000},
	{0.87046	,-0.43389	,-0.23245},
	{0.44919	,0.86267	,0.23245},
	{0.58779	,0.80902	,0.00000},
	{0.68164	,0.69378	,-0.23245},
	{-0.68164	,0.69378	,0.23245},
	{-0.58779	,0.80902	,0.00000},
	{-0.44919	,0.86267	,-0.23245},
	{-0.87046	,-0.43389	,0.23245},
	{-0.95106	,-0.30902	,0.00000},
	{-0.95925	,-0.16062	,-0.23245},
	{0.14366	,-0.96194	,0.23245},
	{0.00000	,-1.00000	,0.00000},
	{-0.14366	,-0.96194	,-0.23245},
	{0.51275	,0.69378	,-0.50572},
	{0.26287	,0.80902	,-0.52573},
	{-0.00703	,0.86267	,-0.50572},
	{-0.50138	,0.70205	,-0.50572},
	{-0.68819	,0.50000	,-0.52573},
	{-0.82262	,0.25989	,-0.50572},
	{-0.82262	,-0.25989	,-0.50572},
	{-0.68819	,-0.50000	,-0.52573},
	{-0.50138	,-0.70205	,-0.50572},
	{-0.00703	,-0.86267	,-0.50572},
	{0.26287	,-0.80902	,-0.52573},
	{0.51275	,-0.69378	,-0.50572},
	{0.81827	,-0.27327	,-0.50572	},
	{0.85065	,0.00000	,-0.52573},
	{0.81827	,0.27327	,-0.50572},
	{0.22108	,0.16062	,-0.96194},
	{0.42533	,0.30902	,-0.85065},
	{0.59720	,0.43389	,-0.67461},
	{-0.08444	,0.25989	,-0.96194},
	{-0.16246	,0.50000	,-0.85065},
	{-0.22811	,0.70205	,-0.67461},
	{-0.27327	,-0.00000	,-0.96194},
	{-0.52573	,-0.00000	,-0.85065},
	{-0.73818	,-0.00000	,-0.67461},
	{-0.08444	,-0.25989	,-0.96194},
	{-0.16246	,-0.50000	,-0.85065},
	{-0.22811	,-0.70205	,-0.67461},
	{0.22108	,-0.16062	,-0.96194},
	{0.42533	,-0.30902	,-0.85065},
	{0.59720	,-0.43389	,-0.67461},
	{0.36180	,0.26287	,0.89443},
	{0.61786	,0.26468	,0.74040},
	{0.44265	,0.50583	,0.74040},
	{-0.13820	,0.42533	,0.89443},
	{-0.06079	,0.66941	,0.74040},
	{-0.34429	,0.57730	,0.74040},
	{-0.44721	,-0.00000	,0.89443},
	{-0.65544	,0.14904	,0.74040},
	{-0.65544	,-0.14904	,0.74040},
	{-0.13820	,-0.42533	,0.89443},
	{-0.34429	,-0.57730	,0.74040},
	{-0.06079	,-0.66941	,0.74040},
	{0.36180	,-0.26287	,0.89443},
	{0.44265	,-0.50583	,0.74040},
	{0.61786	,-0.26468	,0.74040},
	{1.00000	,0.00000	,0.00000},
	{0.95535	,-0.14904	,-0.25512},
	{0.95535	,0.14904	,-0.25512},
	{0.30902	,0.95106	,0.00000},
	{0.43697	,0.86254	,-0.25512},
	{0.15347	,0.95465	,-0.25512},
	{-0.80902	,0.58779	,0.00000},
	{-0.68529	,0.68212	,-0.25512},
	{-0.86050	,0.44096	,-0.25512},
	{-0.80902	,-0.58779	,0.00000	},
	{-0.86050	,-0.44096	,-0.25512},
	{-0.68529	,-0.68212	,-0.25512},
	{0.30902	,-0.95106	,0.00000},
	{0.15347	,-0.95465	,-0.25512},
	{0.43697	,-0.86254	,-0.25512},
	{0.80902	,0.58779	,0.00000},
	{0.68529	,0.68212	,0.25512},
	{0.86050	,0.44096	,0.25512},
	{-0.30902	,0.95106	,0.00000},
	{-0.43697	,0.86254	,0.25512},
	{-0.15347	,0.95465	,0.25512},
	{-1.00000	,-0.00000	,0.00000},
	{-0.95535	,-0.14904	,0.25512},
	{-0.95535	,0.14904	,0.25512},
	{-0.30902	,-0.95106	,0.00000},
	{-0.15347	,-0.95465	,0.25512},
	{-0.43697	,-0.86254	,0.25512},
	{0.80902	,-0.58779	,0.00000},
	{0.86050	,-0.44096	,0.25512},
	{0.68529	,-0.68212	,0.25512},
	{0.13820	,0.42533	,-0.89443},
	{0.06079	,0.66941	,-0.74040},
	{0.34429	,0.57730	,-0.74040},
	{-0.36180	,0.26287	,-0.89443},
	{-0.61786	,0.26468	,-0.74040},
	{-0.44265	,0.50583	,-0.74040},
	{-0.36180	,-0.26287	,-0.89443},
	{-0.44265	,-0.50583	,-0.74040},
	{-0.61786	,-0.26468	,-0.74040},
	{0.13820	,-0.42533	,-0.89443},
	{0.34429	,-0.57730	,-0.74040},
	{0.06079	,-0.66941	,-0.74040},
	{0.44721	,0.00000	,-0.89443},
	{0.65544	,0.14904	,-0.74040},
	{0.65544	,-0.14904	,-0.74040}
};

extern "C"
{
	// Returns TRUE only if everything O},K.
	XRHS_API VOID	xrHemisphereBuild
		(
		int						quality,
		BOOL					ground,
		float					ground_scale,
		float					energy,
		xrHemisphereIterator*	iterator,
		LPVOID					param
		)
	{
		// SELECT table
		int		h_count, h_table[3];
		const double (*hemi)[3] = 0;
		switch (quality)
		{
		case 1:	// LOW quality
			if (ground)	
			{
				h_count		= HEMI1_LIGHTS_F;
				h_table[0]	= 0;
				h_table[1]	= 2;
				h_table[2]	= 1;
				hemi		= hemi_1F;
			} else {
				h_count		= HEMI1_LIGHTS;
				h_table[0]	= 0;
				h_table[1]	= 1;
				h_table[2]	= 2;
				hemi		= hemi_1;
			}
			break;
		case 2:	// HIGH quality
			if (ground)
			{
				h_count		= HEMI2_LIGHTS_F;
				h_table[0]	= 0;
				h_table[1]	= 2;
				h_table[2]	= 1;
				hemi		= hemi_2F;
			} else {
				h_count		= HEMI2_LIGHTS;
				h_table[0]	= 0;
				h_table[1]	= 2;
				h_table[2]	= 1;
				hemi		= hemi_2;
			}
			break;
		default:// NO 	
			return;
		}
		
		// Calculate energy
		float total = 0;
		for (int i=0; i<h_count; i++)
		{
			float y		=	-float(hemi[i][h_table[1]]);
			float E		=	(y<0)?ground_scale:1;
			total		+=	E;
		}
		
		// Iterate
		for (i=0; i<h_count; i++)
		{
			float x		=	-float(hemi[i][h_table[0]]);
			float y		=	-float(hemi[i][h_table[1]]);
			float z		=	-float(hemi[i][h_table[2]]);
			float mag	=	sqrtf(x*x + y*y + z*z);
			x /= mag;	y /= mag;	z /= mag;

			float E		=	((y<0)?ground_scale:1)/total;

			iterator	(x,y,z,E*energy,param);
		}
	}
};
