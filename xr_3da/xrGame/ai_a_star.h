////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_a_star.h
//	Created 	: 21.03.2002
//  Modified 	: 05.04.2002
//	Author		: Dmitriy Iassenev
//	Description : A* algortihm for finding the x-est path between two nodes
////////////////////////////////////////////////////////////////////////////

#ifndef __AI_A_STAR__
#define __AI_A_STAR__

////////////////////////////////////////////////////////////////////////////
// included headers
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// type declarations
////////////////////////////////////////////////////////////////////////////

typedef unsigned char uchar;
typedef unsigned int  uint;

#pragma pack(4)
typedef struct tagTNode {
	uchar		ucOpenCloseMask:1;
	int			iIndex:31;
	float		f;
	float		g;
	float		h;
	tagTNode	*tpNext;
	tagTNode	*tpForward;
	tagTNode	*tpBack;
	tagTNode	*tpOpenedNext;
	tagTNode	*tpOpenedPrev;
} TNode;

typedef struct tagTIndexNode {
	TNode *tpNode;
	DWORD dwTime;
} TIndexNode;
#pragma pack()

#endif