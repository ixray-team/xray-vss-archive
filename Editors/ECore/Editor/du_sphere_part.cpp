//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "du_sphere_part.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
Fvector du_cone_vertices[DU_CONE_NUMVERTEX]=
{
    0.0000f,  0.0000f,	0.0000f, 
    0.5000f,  0.0000f,  1.0000f, 
    0.4619f,  0.1913f,  1.0000f, 
    0.3536f,  0.3536f,  1.0000f, 
    0.1913f,  0.4619f,  1.0000f, 
    -0.0000f, 0.5000f,  1.0000f, 
    -0.1913f, 0.4619f,  1.0000f, 
    -0.3536f, 0.3536f,  1.0000f, 
    -0.4619f, 0.1913f,  1.0000f, 
    -0.5000f, -0.0000f, 1.0000f, 
    -0.4619f, -0.1913f, 1.0000f, 
    -0.3536f, -0.3536f, 1.0000f, 
    -0.1913f, -0.4619f, 1.0000f, 
    0.0000f,  -0.5000f, 1.0000f, 
    0.1913f,  -0.4619f, 1.0000f, 
    0.3536f,  -0.3536f, 1.0000f, 
    0.4619f,  -0.1913f, 1.0000f, 
    0.0000f,  0.0000f,  1.0000f
};
WORD du_cone_faces[DU_CONE_NUMFACES*3]=
{
    0,  2,  1,
    0,  3,  2,
    0,  4,  3,
    0,  5,  4,
    0,  6,  5,
    0,  7,  6,
    0,  8,  7,
    0,  9,  8,
    0, 10,  9,
    0, 11, 10,
    0, 12, 11,
    0, 13, 12,
    0, 14, 13,
    0, 15, 14,
    0, 16, 15,
    0,  1, 16,
    17, 1, 2,
    17, 2, 3,
    17, 3, 4,
    17, 4, 5,
    17, 5, 6,
    17, 6, 7,
    17, 7, 8,
    17, 8, 9,
    17, 9, 10,
    17,10, 11,
    17,11, 12,
    17,12, 13,
    17,13, 14,
    17,14, 15,
    17,15, 16,
    17,16, 1
};

WORD du_cone_lines[DU_CONE_NUMLINES*2]=
{
	// tile
    0, 	1,
//    0,	2,
    0,	3,
//    0,	4,
    0,	5,
//    0,	6,
    0,	7,
//    0,	8,
    0,	9,
//    0,	10,
    0,	11,
//    0,	12,
    0,	13,
//    0,	14,
    0,	15,
//    0,	16,
    
	// cape
    1,	2,
    2,	3,
    3, 	4,
    4, 	5,
    5, 	6,
    6, 	7,
    7, 	8,
    8, 	9,
    9, 	10,
    10,	11,
    11,	12,
    12,	13,
    13,	14,
    14,	15,
    15,	16,
    16,	1,
};
