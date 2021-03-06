///////////////////////////////////////////////////////////////
//	Module 		: xr_a_star.h
//	Created 	: 21.03.2002
//  Modified 	: 21.03.2002
//	Author		: Dmitriy Iassenev
//	Description : A* algortihm for finding the shortest path between two nodes
///////////////////////////////////////////////////////////////

#ifndef __XR_A_STAR__
#define __XR_A_STAR__

///////////////////////////////////////////////////////////////
// included headers
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// macros definitions
///////////////////////////////////////////////////////////////

#define DOUBLE_INFINITY (double)1000000.0

///////////////////////////////////////////////////////////////
// type declarations
///////////////////////////////////////////////////////////////

typedef struct tagTNode {
	int		iIndex;
	double	f;
	double	g;
	double	h;
	tagTNode *tpNext;
	tagTNode *tpForward;
	tagTNode *tpBack;
	tagTNode *tpOpenedNext;
	tagTNode *tpClosedNext;
} TNode;

///////////////////////////////////////////////////////////////
// exportes functions
///////////////////////////////////////////////////////////////

extern TNode *tpfFindTheShortestPath(int iStartNode, int iFinishNode);

#endif