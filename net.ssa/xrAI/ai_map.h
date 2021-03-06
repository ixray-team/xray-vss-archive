#pragma once

#include "xrLevel.h"
//#include "ai_a_star_search.h"
#include "AIMapExport.h"

#define MAX_VALUE				1000000.0

#define EPS_H				0.5f

#define	LI_NONE				0
#define LI_COLLINEAR        0
#define	LI_INTERSECT		1
#define LI_EQUAL	        2

typedef struct tagSContour {
	Fvector v1,v2,v3,v4;
} SContour;

typedef struct tagSSegment {
	Fvector v1,v2;
} SSegment;

class CAI_Map {
private:
	IReader*					vfs;			// virtual file
	BYTE*						m_nodes;		// virtual nodes DATA array
	NodeCompressed**			m_nodes_ptr;	// pointers to node's data
public:
	hdrNODES					m_header;		// m_header
	xr_vector<bool>				q_mark_bit;		// temporal usage mark for queries
	float						m_fSize2;
	float						m_fYSize2;
	typedef u8 Cover[4];
	Cover						*m_tpCoverPalette;
	u32							m_row_length;

								CAI_Map()
	{
		m_nodes_ptr				= 0;
		vfs						= 0;
		m_tpCoverPalette		= 0;
	}
								CAI_Map(LPCSTR name)
	{
		string256	fName;
		strconcat	(fName,name,"level.ai");
		vfs			= FS.r_open(fName);
		
		// m_header & data
		vfs->r		(&m_header,sizeof(m_header));
		R_ASSERT	(m_header.version == XRAI_CURRENT_VERSION);
		m_row_length= iFloor((m_header.aabb.max.z - m_header.aabb.min.z)/m_header.size + EPS_L + .5f) + 1;

		m_fSize2	= _sqr(m_header.size)/1;
		m_fYSize2	= _sqr((float)(m_header.size_y/32767.0))/1;
		u32			dwPaletteSize = vfs->r_u32();
		m_tpCoverPalette = (Cover*)vfs->pointer();
		vfs->advance(dwPaletteSize*sizeof(Cover));
		m_nodes		= (BYTE*) vfs->pointer();

		// dispatch table
		m_nodes_ptr	= (NodeCompressed**)xr_malloc(m_header.count*sizeof(void*));
		
		for (u32 I=0; I<m_header.count; Progress(float(++I)/m_header.count))
		{
			m_nodes_ptr[I]	= (NodeCompressed*)vfs->pointer();

			NodeCompressed	C;
			vfs->r			(&C,sizeof(C));
//
//			u32				L = C.links;
//			vfs->advance	(L*sizeof(NodeLink));
		}

		// special query tables
		q_mark_bit.assign	(m_header.count,false);
	};

	virtual							~CAI_Map()
	{
		xr_free(m_nodes_ptr);
		xr_delete(vfs);
	};

	IC	NodeCompressed*		Node(u32 ID) const { return vfs?m_nodes_ptr[ID]:NULL; }

	IC	void PackPosition(NodePosition &Pdest, const Fvector& Psrc) const
	{
		float sp = 1/m_header.size;
		int pxz	= iFloor(((Psrc.x - m_header.aabb.min.x)*sp + EPS_L + .5f)*m_row_length) + iFloor((Psrc.z - m_header.aabb.min.z)*sp   + EPS_L + .5f);
		int py	= iFloor(65535.f*(Psrc.y-m_header.aabb.min.y)/(m_header.size_y)+EPS_L);
		clamp	(pxz,0,(1 << 24) - 1);	Pdest.xz = u32(pxz);
		clamp	(py,0,     65535);	Pdest.y = u16	(py);
	}

	IC void unpack_xz(const NodePosition &node_position, int &x, int &z) const
	{
		x = node_position.xz / m_row_length;
		z = node_position.xz % m_row_length;
	}

	IC void unpack_xz(const NodeCompressed &node, int &x, int &z) const
	{
		unpack_xz(node.p,x,z);
	}

	IC void unpack_xz(const NodeCompressed *node, int &x, int &z) const
	{
		unpack_xz(node->p,x,z);
	}

	IC BOOL	u_InsideNode(const NodeCompressed& N, const NodePosition &P) const
	{
		return 	(P.xz == N.p.xz);
	}

	IC	void UnpackPosition(Fvector& Pdest, const NodePosition &Psrc) const
	{
		int	x,z;
		unpack_xz(Psrc,x,z);
		Pdest.x = float(x)*m_header.size;
		Pdest.y = (float(Psrc.y)/65535)*m_header.size_y + m_header.aabb.min.y;
		Pdest.z = float(z)*m_header.size;
	}

	IC	void UnpackPosition(Fvector& Pdest, const NodePosition& Psrc, Fbox& bb, SAIParams& params) const
	{
		int	x,z;
		unpack_xz(Psrc,x,z);
		Pdest.x = float(x)*params.fPatchSize;
		Pdest.y = (float(Psrc.y)/65535)*(bb.max.y-bb.min.y) + bb.min.y;
		Pdest.z = float(z)*params.fPatchSize;
	}

	IC Fvector tfGetNodeCenter(NodeCompressed *tpNode) const
	{
		Fvector tP;
		UnpackPosition(tP, tpNode->p);
		return(tP);
	}

	IC Fvector tfGetNodeCenter(u32 tNodeID) const
	{
		return(tfGetNodeCenter(Node(tNodeID)));
	}

	IC float ffGetDistanceBetweenNodeCenters(NodeCompressed *tpNode0, NodeCompressed *tpNode1) const
	{
		return(tfGetNodeCenter(tpNode0).distance_to(tfGetNodeCenter(tpNode1)));
	}

	IC float ffGetDistanceBetweenNodeCenters(u32 dwNodeID0, u32 dwNodeID1) const
	{
		return(ffGetDistanceBetweenNodeCenters(Node(dwNodeID0),Node(dwNodeID1)));
	}

	IC float ffGetDistanceBetweenNodeCenters(NodeCompressed *tpNode0, u32 dwNodeID1) const
	{
		return(ffGetDistanceBetweenNodeCenters(tpNode0,Node(dwNodeID1)));
	}

	IC float ffGetDistanceBetweenNodeCenters(u32 dwNodeID0, NodeCompressed *tpNode1) const
	{
		return(ffGetDistanceBetweenNodeCenters(Node(dwNodeID0),tpNode1));
	}

	typedef	int	const_iterator;

	IC		void		begin			(const u32 node_index, const_iterator &begin, const_iterator &end) const
	{
		begin					= 0;
		end						= 4;
	}

	IC		float		get_edge_weight	(const u32 node_index1, const u32 node_index2) const
	{
		return					(ffGetDistanceBetweenNodeCenters(node_index1,node_index2));
	}

	IC		u32			get_value		(const u32 node_index, const_iterator &i) const
	{
		return					(Node(node_index)->get_link(i));
	}

	IC		bool		is_accessible	(const u32 node_index) const
	{
		return					(true);
	}

	IC		u32			get_node_count	() const
	{
		return					(m_header.count);
	}

	typedef NodeCompressed InternalNode;

	IC int lines_intersect(	float x1, float y1,	float x2, float y2,	float x3, float y3, float x4, float y4,	float *x, float *y) const
	{
		float a1, a2, b1, b2, c1, c2;	/* Coefficients of line eqns. */
		float r1, r2, r3, r4;			/* 'Sign' values */
		float denom, num;				/* Intermediate values */
		
		a1 = y2 - y1;
		b1 = x1 - x2;
		c1 = x2 * y1 - x1 * y2;
		
		r3 = a1 * x3 + b1 * y3 + c1;
		r4 = a1 * x4 + b1 * y4 + c1;
		
		if ((r3*r4 > EPS) && (_abs(r3) > EPS_H) && (_abs(r4) > EPS_H))
			return(LI_NONE);
		
		a2 = y4 - y3;
		b2 = x3 - x4;
		c2 = x4 * y3 - x3 * y4;
		
		r1 = a2 * x1 + b2 * y1 + c2;
		r2 = a2 * x2 + b2 * y2 + c2;
		
		if ((r1*r2 > EPS) && (_abs(r1) > EPS_H) && (_abs(r2) > EPS_H))
			return(LI_NONE);
		
		if ( _abs(r1*r2)<EPS_S && _abs(r3*r4)<EPS_S ) 
			return (LI_EQUAL);

		denom = a1 * b2 - a2 * b1;
		if ( _abs(denom) < EPS ) return ( LI_COLLINEAR );
		
		num = b1 * c2 - b2 * c1;
		*x = num / denom;
		
		num = a2 * c1 - a1 * c2;
		*y = num / denom;
		
		if ((*x < _min(x3,x4) - EPS_L) || (*x > _max(x3,x4) + EPS_L) || (*y < _min(y3,y4) - EPS_L) || (*y > _max(y3,y4) + EPS_L) || (*x < _min(x1,x2) - EPS_L) || (*x > _max(x1,x2) + EPS_L) || (*y < _min(y1,y2) - EPS_L) || (*y > _max(y1,y2) + EPS_L))
			return(LI_NONE);
		else
			return(LI_INTERSECT);
	}

	IC float ffGetY(NodeCompressed &tNode, float X, float Z) const
	{
		Fvector	DUP, vNorm, v, v1, P;
		DUP.set(0,1,0);
		pvDecompress(vNorm,tNode.plane);
		Fplane PL; 
		UnpackPosition(P,tNode.p);
		PL.build(P,vNorm);
		v.set(X,P.y,Z);	
		PL.intersectRayPoint(v,DUP,v1);	
		return(v1.y);
	}

	IC bool bfInsideNode(NodeCompressed *tpNode, Fvector &tCurrentPosition, bool bUseY = false) const
	{
		Fvector tP;
		float fHalfSubNodeSize = m_header.size*.5f;
		UnpackPosition(tP,tpNode->p);
		return(
			(tCurrentPosition.x >= tP.x - fHalfSubNodeSize - EPS) &&
			(tCurrentPosition.z >= tP.z - fHalfSubNodeSize - EPS) &&
			(tCurrentPosition.x <= tP.x + fHalfSubNodeSize + EPS) &&
			(tCurrentPosition.z <= tP.z + fHalfSubNodeSize + EPS) &&
			((!bUseY) || 
			(_abs(tCurrentPosition.y - ffGetY(*tpNode,tCurrentPosition.x,tCurrentPosition.z)) < 1.f))
		);
	}

	IC void projectPoint(const Fplane& PL, Fvector& P) const
	{
		P.y -= PL.classify(P)/PL.n.y; 
	}

	void UnpackContour(SContour &C, u32 ID) const
	{
		NodeCompressed *tpNode = Node(ID);
		
		// decompress positions
		Fvector P;
		UnpackPosition(P,tpNode->p);
		
		// decompress plane
		Fplane	PL;	
		pvDecompress(PL.n,tpNode->plane);
		PL.d = - PL.n.dotproduct(P);
		
		// create vertices
		float st = m_header.size/2;
		
		C.v1.set(P.x-st,P.y,P.z-st);	projectPoint(PL,C.v1);	// minX,minZ
		C.v2.set(P.x+st,P.y,P.z-st);	projectPoint(PL,C.v2);	// maxX,minZ
		C.v3.set(P.x+st,P.y,P.z+st);	projectPoint(PL,C.v3);	// maxX,maxZ
		C.v4.set(P.x-st,P.y,P.z+st);	projectPoint(PL,C.v4);	// minX,maxZ
	}

	IC bool bfSimilar(Fvector &tPoint0, Fvector &tPoint1) const
	{
		return((_abs(tPoint0.x - tPoint1.x) < EPS_L) && (_abs(tPoint0.z - tPoint1.z) < EPS_L));
	}

	IC bool bfInsideContour(Fvector &tPoint, SContour &tContour) const
	{
		return((tContour.v1.x - EPS_L <= tPoint.x) && (tContour.v1.z - EPS_L <= tPoint.z) && (tContour.v3.x + EPS_L >= tPoint.x) && (tContour.v3.z + EPS_L >= tPoint.z));
	}

	IC void vfIntersectContours(SSegment &tSegment, SContour &tContour0, SContour &tContour1) const
	{
		bool bFound = false;
		
		if (bfInsideContour(tContour0.v1,tContour1)) {
			tSegment.v1 = tContour0.v1;
			bFound = true;
		}

		if (bfInsideContour(tContour0.v2,tContour1)) {
			if (!bFound) {
				tSegment.v1 = tContour0.v2;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour0.v2;
				return;
			}
		}
		if (bfInsideContour(tContour0.v3,tContour1)) {
			if (!bFound) {
				tSegment.v1 = tContour0.v3;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour0.v3;
				return;
			}
		}
		if (bfInsideContour(tContour0.v4,tContour1)) {
			if (!bFound) {
				tSegment.v1 = tContour0.v4;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour0.v4;
				return;
			}
		}
		if (bFound) {
			if (bfInsideContour(tContour1.v1,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v1)))) {
				tSegment.v2 = tContour1.v1;
				return;
			}
			if (bfInsideContour(tContour1.v2,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v2)))) {
				tSegment.v2 = tContour1.v2;
				return;
			}
			if (bfInsideContour(tContour1.v3,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v3)))) {
				tSegment.v2 = tContour1.v3;
				return;
			}
			if (bfInsideContour(tContour1.v4,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v4)))) {
				tSegment.v2 = tContour1.v4;
				return;
			}
		}
		else {
			if (bfInsideContour(tContour1.v1,tContour0)) {
				tSegment.v1 = tContour1.v1;
				bFound = true;
			}
			if (bfInsideContour(tContour1.v2,tContour0)) {
				if (!bFound) {
					tSegment.v1 = tContour1.v2;
					bFound = true;
				}
				else {
					tSegment.v2 = tContour1.v2;
					return;
				}
			}
			if (bfInsideContour(tContour1.v3,tContour0)) {
				if (!bFound) {
					tSegment.v1 = tContour1.v3;
					bFound = true;
				}
				else {
					tSegment.v2 = tContour1.v3;
					return;
				}
			}
			if (bfInsideContour(tContour1.v4,tContour0)) {
				if (!bFound) {
					tSegment.v1 = tContour1.v4;
					bFound = true;
				}
				else {
					tSegment.v2 = tContour1.v4;
					return;
				}
			}
		}

		if (bFound) {
			tSegment.v2 = tSegment.v1;
			Log("! AI_PathNodes: segment has null length");
		}
		else
			Log("! AI_PathNodes: Can't find intersection segment");
	}

	void vfGetIntersectionPoints(NodeCompressed &Node, SContour tCurContour, Fvector tStartPoint, Fvector tFinishPoint, Fvector &tPoint) const
	{
		u32 dwIntersect, dwCount = 0;
		Fvector tTravelNode, tPoints[4];
		dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tCurContour.v1.x,tCurContour.v1.z,tCurContour.v2.x,tCurContour.v2.z,&tTravelNode.x,&tTravelNode.z);
		if (dwIntersect == LI_INTERSECT)
			tPoints[dwCount++] = tTravelNode;
		else
			if (dwIntersect == LI_EQUAL) {
				tPoints[dwCount++] = tCurContour.v1;
				tPoints[dwCount++] = tCurContour.v2;
			}

		dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tCurContour.v3.x,tCurContour.v3.z,tCurContour.v2.x,tCurContour.v2.z,&tTravelNode.x,&tTravelNode.z);
		if (dwIntersect == LI_INTERSECT)
			tPoints[dwCount++] = tTravelNode;
		else
			if (dwIntersect == LI_EQUAL) {
				tPoints[dwCount++] = tCurContour.v3;
				tPoints[dwCount++] = tCurContour.v2;
			}
		
		if (dwCount < 4) {
			dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tCurContour.v3.x,tCurContour.v3.z,tCurContour.v4.x,tCurContour.v4.z,&tTravelNode.x,&tTravelNode.z);
			if (dwIntersect == LI_INTERSECT)
				tPoints[dwCount++] = tTravelNode;
			else
				if (dwIntersect == LI_EQUAL) {
					tPoints[dwCount++] = tCurContour.v3;
					tPoints[dwCount++] = tCurContour.v4;
				}
		}
		
		if (dwCount < 4) {
			dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tCurContour.v1.x,tCurContour.v1.z,tCurContour.v4.x,tCurContour.v4.z,&tTravelNode.x,&tTravelNode.z);
			if (dwIntersect == LI_INTERSECT)
				tPoints[dwCount++] = tTravelNode;
			else
				if (dwIntersect == LI_EQUAL) {
					tPoints[dwCount++] = tCurContour.v1;
					tPoints[dwCount++] = tCurContour.v4;
				}
		}
		for (int i=0; i<(int)dwCount; i++)
			tPoints[i].y = ffGetY(Node,tPoints[i].x,tPoints[i].z);

		switch (dwCount) {
			case 0 : {
				throw("");
				break;
			}
			case 1 : {
				tPoint = tPoints[0];
				break;
			}
			case 2 : {
				if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[1]))
					tPoint = tPoints[0];
				else
					tPoint = tPoints[1];
				break;
			}
			case 3 : {
				if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[1]))
					if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[2]))
						tPoint = tPoints[0];
					else
						tPoint = tPoints[2];
				else
					if (tFinishPoint.distance_to(tPoints[1]) < tFinishPoint.distance_to(tPoints[2]))
						tPoint = tPoints[1];
					else
						tPoint = tPoints[2];
				break;
			}
			case 4 : {
				if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[1]))
					if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[2]))
						if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[3]))
							tPoint = tPoints[0];
						else
							tPoint = tPoints[3];
					else
						if (tFinishPoint.distance_to(tPoints[2]) < tFinishPoint.distance_to(tPoints[3]))
							tPoint = tPoints[2];
						else
							tPoint = tPoints[3];
				else
					if (tFinishPoint.distance_to(tPoints[1]) < tFinishPoint.distance_to(tPoints[2]))
						if (tFinishPoint.distance_to(tPoints[1]) < tFinishPoint.distance_to(tPoints[3]))
							tPoint = tPoints[1];
						else
							tPoint = tPoints[3];
					else
						if (tFinishPoint.distance_to(tPoints[2]) < tFinishPoint.distance_to(tPoints[3]))
							tPoint = tPoints[2];
						else
							tPoint = tPoints[3];
				break;
			}
			default : NODEFAULT;
		}
	}

	IC bool bfBetweenPoints(SSegment tSegment, Fvector tPoint) const
	{
		return(((tSegment.v1.x - EPS_L <= tPoint.x) && (tSegment.v1.z - EPS_L <= tPoint.z) && (tSegment.v2.x + EPS_L >= tPoint.x) && (tSegment.v2.z + EPS_L >= tPoint.z)) || ((tSegment.v2.x - EPS_L <= tPoint.x) && (tSegment.v2.z - EPS_L <= tPoint.z) && (tSegment.v1.x + EPS_L >= tPoint.x) && (tSegment.v1.z + EPS_L >= tPoint.z)));
	}

	void vfChoosePoint(Fvector &tStartPoint, Fvector &tFinishPoint, SContour	&tCurContour, int iNodeIndex, Fvector &tTempPoint, int &iSavedIndex) const
	{
		SContour					tNextContour;
		SSegment					tNextSegment;
		Fvector						tCheckPoint1 = tStartPoint, tCheckPoint2 = tStartPoint, tIntersectPoint;
		UnpackContour				(tNextContour,iNodeIndex);
		vfIntersectContours			(tNextSegment,tNextContour,tCurContour);
		u32							dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tNextSegment.v1.x,tNextSegment.v1.z,tNextSegment.v2.x,tNextSegment.v2.z,&tIntersectPoint.x,&tIntersectPoint.z);
		if (!dwIntersect)
			return;
		for (int i=0; i<4; i++) {
			switch (i) {
				case 0 : {
					tCheckPoint1	= tNextContour.v1;
					tCheckPoint2	= tNextContour.v2;
					break;
				}
				case 1 : {
					tCheckPoint1	= tNextContour.v2;
					tCheckPoint2	= tNextContour.v3;
					break;
				}
				case 2 : {
					tCheckPoint1	= tNextContour.v3;
					tCheckPoint2	= tNextContour.v4;
					break;
				}
				case 3 : {
					tCheckPoint1	= tNextContour.v4;
					tCheckPoint2	= tNextContour.v1;
					break;
				}
				default : NODEFAULT;
			}
			dwIntersect				= lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tCheckPoint1.x,tCheckPoint1.z,tCheckPoint2.x,tCheckPoint2.z,&tIntersectPoint.x,&tIntersectPoint.z);
			if (dwIntersect == LI_INTERSECT) {
				if (tFinishPoint.distance_to_xz(tIntersectPoint) < tFinishPoint.distance_to_xz(tTempPoint) + EPS_L) {
					tTempPoint = tIntersectPoint;
					iSavedIndex = iNodeIndex;
				}
			}
			else
				if (dwIntersect == LI_EQUAL) {
					if (tStartPoint.distance_to_xz(tCheckPoint1) > tStartPoint.distance_to_xz(tTempPoint))
						if (tStartPoint.distance_to_xz(tCheckPoint1) > tStartPoint.distance_to_xz(tCheckPoint2)) {
							tTempPoint = tCheckPoint1;
							iSavedIndex = iNodeIndex;
						}
						else {
							tTempPoint = tCheckPoint2;
							iSavedIndex = iNodeIndex;
						}
					else
						if (tStartPoint.distance_to_xz(tCheckPoint2) > tStartPoint.distance_to_xz(tTempPoint)) {
							tTempPoint = tCheckPoint2;
							iSavedIndex = iNodeIndex;
						}

				}
		}
	}

	float ffCheckPositionInDirection(u32 dwStartNode, Fvector tStartPosition, Fvector tFinishPosition, float fMaxDistance) const
	{
		SContour				tCurContour;
		NodeCompressed			*tpNode;
		int						i, iSavedIndex, iPrevIndex = -1, iNextNode;
		Fvector					tStartPoint = tStartPosition, tTempPoint = tStartPosition, tFinishPoint = tFinishPosition;
		float					fCurDistance = 0.f, fDistance = tStartPosition.distance_to_xz(tFinishPosition);
		u32						dwCurNode = dwStartNode;

		while (!bfInsideNode(Node(dwCurNode),tFinishPosition) && (fCurDistance < (fDistance + EPS_L))) {
			tpNode				= Node(dwCurNode);
			iSavedIndex			= -1;
			UnpackContour		(tCurContour,dwCurNode);
			for ( i=0; i < NODE_NEIGHBOUR_COUNT; i++)
				if ((iNextNode = tpNode->get_link(i)) != iPrevIndex)
					vfChoosePoint	(tStartPoint,tFinishPoint,tCurContour, iNextNode,tTempPoint,iSavedIndex);

			if (iSavedIndex > -1) {
				fCurDistance	= tStartPoint.distance_to_xz(tTempPoint);
				iPrevIndex		= dwCurNode;
				dwCurNode		= iSavedIndex;
			}
			else
				return(MAX_VALUE);
		}

		if (bfInsideNode(Node(dwCurNode),tFinishPosition) && (_abs(ffGetY(*Node(dwCurNode),tFinishPosition.x,tFinishPosition.z) - tFinishPosition.y) < .5f))
			return(tStartPosition.distance_to(tFinishPosition));
		else
			return(MAX_VALUE);
	}

	u32 dwfFindCorrespondingNode(Fvector &tLocalPoint) const
	{
		NodePosition	P;
		PackPosition	(P,tLocalPoint);
		short min_dist	= 32767;
		int selected	= -1;
		for (int i=0; i<(int)m_header.count; i++) {
			NodeCompressed& N = *m_nodes_ptr[i];
			if (u_InsideNode(N,P)) {
				Fvector	DUP, vNorm, v, v1, P;
				DUP.set(0,1,0);
				pvDecompress(vNorm,N.plane);
				Fplane PL; 
				UnpackPosition(P,N.p);
				PL.build(P,vNorm);
				v.set(tLocalPoint.x,P.y,tLocalPoint.z);	
				PL.intersectRayPoint(v,DUP,v1);
				int dist = iFloor((v1.y - tLocalPoint.y)*(v1.y - tLocalPoint.y));
				if (dist < min_dist) {
					min_dist = (short)dist;
					selected = i;
				}
			}
		}
		return(selected);
	}

	void						vfShallowGraphSearch(Fvector tStartPosition, u32 dwStartNode, float fSearchRange, xr_vector<u32> &tpaStack, xr_vector<bool> &tpaMask)
	{
		u32							dwCurNodeID, dwNextNodeID;
		NodeCompressed				*tpStartNode = Node(dwStartNode), *tpCurNode, *tpCurrentNode = tpStartNode;
		float						fRangeSquare = fSearchRange*fSearchRange, fDistance = tStartPosition.distance_to_sqr(tfGetNodeCenter(tpStartNode));
		const_iterator				I, E;

		tpaStack.clear				();
		tpaStack.push_back			(dwStartNode);
		tpaMask[dwStartNode]		= true;

		// Cycle
		for (u32 i=0; i<tpaStack.size(); i++) {
			dwCurNodeID				= tpaStack[i];
			tpCurNode				= Node(dwCurNodeID);
			begin					(dwCurNodeID,I,E);
			for ( ; I != E; I++) {
				if (tpaMask[dwNextNodeID = tpCurNode->get_link(I)])
					continue;
				tpCurrentNode		= Node(dwNextNodeID);
				fDistance			= tStartPosition.distance_to_sqr(tfGetNodeCenter(tpCurrentNode));
				if (fDistance >= fRangeSquare)
					continue;
				tpaMask[dwNextNodeID] = true;
				tpaStack.push_back	(dwNextNodeID);
			}
		}

		{
			xr_vector<u32>::iterator I	= tpaStack.begin();
			xr_vector<u32>::iterator E	= tpaStack.end();
			for ( ; I!=E; I++)	
				tpaMask[*I] = false;
		}
	}
};