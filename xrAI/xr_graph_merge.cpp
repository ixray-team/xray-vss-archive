////////////////////////////////////////////////////////////////////////////
//	Module 		: xr_graph_merge.cpp
//	Created 	: 25.01.2003
//  Modified 	: 25.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Merging level graphs for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_ini.h"
#include "xrLevel.h"
#include "xrAI.h"
#include "xrGraph.h"
#include "ai_alife_space.h"
#include "game_base.h"
#include "xrServer_Entities.h"
#include "xrCrossTable.h"
using namespace ALife;

class CLevelGraph;

typedef struct tagSConnectionVertex {
	LPSTR		caConnectName;
	_GRAPH_ID	tGraphID;
	u32			dwLevelID;
} SConnectionVertex;

class CComparePredicate {
public:
	IC bool operator()(LPSTR S1, LPSTR S2) const
	{
		return(strcmp(S1,S2) < 0);
	}
};

DEFINE_MAP		(u32,	CLevelGraph *,		GRAPH_P_MAP,	GRAPH_P_PAIR_IT);
DEFINE_MAP_PRED	(LPSTR,	SConnectionVertex,	VERTEX_MAP,		VERTEX_PAIR_IT,	CComparePredicate());

class CLevelGraph : public CALifeGraph {
public:
	GRAPH_VERTEX_VECTOR			m_tpVertices;
	SLevel						m_tLevel;
	VERTEX_MAP					m_tVertexMap;
	u32							m_dwOffset;

								CLevelGraph(const SLevel &tLevel, LPCSTR S, u32 dwOffset, u32 dwLevelID) : CALifeGraph()
	{
		m_tLevel				= tLevel;
		m_dwOffset				= dwOffset;
		
		FILE_NAME				caFileName;
		// updating cross-table
		{
			strconcat			(caFileName,S,CROSS_TABLE_NAME_RAW);
			CALifeCrossTable	*tpCrossTable = xr_new<CALifeCrossTable>(caFileName);
			vector<CALifeCrossTable::SCrossTableCell> tCrossTableUpdate;
			tCrossTableUpdate.resize(tpCrossTable->m_tCrossTableHeader.dwNodeCount);
			for (int i=0; i<(int)tpCrossTable->m_tCrossTableHeader.dwNodeCount; i++) {
				tCrossTableUpdate[i] = tpCrossTable->m_tpaCrossTable[i];
				tCrossTableUpdate[i].tGraphIndex += dwOffset;
			}

			CMemoryWriter		tMemoryStream;
			CALifeCrossTable	tCrossTable;
			
			tCrossTable.m_tCrossTableHeader.dwVersion = XRAI_CURRENT_VERSION;
			tCrossTable.m_tCrossTableHeader.dwNodeCount = tpCrossTable->m_tCrossTableHeader.dwNodeCount;
			tCrossTable.m_tCrossTableHeader.dwGraphPointCount = tpCrossTable->m_tCrossTableHeader.dwGraphPointCount;
			
			xr_delete			(tpCrossTable);
			
			tMemoryStream.open_chunk(CROSS_TABLE_CHUNK_VERSION);
			tMemoryStream.w(&tCrossTable.m_tCrossTableHeader,sizeof(tCrossTable.m_tCrossTableHeader));
			tMemoryStream.close_chunk();
			
			tMemoryStream.open_chunk(CROSS_TABLE_CHUNK_DATA);
			for (int i=0; i<(int)tCrossTable.m_tCrossTableHeader.dwNodeCount; i++)
				tMemoryStream.w(&(tCrossTableUpdate[i]),sizeof(tCrossTableUpdate[i]));
			tMemoryStream.close_chunk();
			
			strconcat			(caFileName,S,CROSS_TABLE_NAME);
			tMemoryStream.save_to(caFileName);
		}

		// loading graph
		strconcat				(caFileName,S,"level.graph");
		CALifeGraph::Load		(caFileName);
		m_tpVertices.resize		(m_tGraphHeader.dwVertexCount);
		GRAPH_VERTEX_IT			B = m_tpVertices.begin();
		GRAPH_VERTEX_IT			I = B;
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		for ( ; I != E; I++) {
			(*I).tLocalPoint		= m_tpaGraph[I - B].tLocalPoint;
			(*I).tGlobalPoint.add	(m_tpaGraph[I - B].tGlobalPoint,m_tLevel.tOffset);
			(*I).tLevelID			= dwLevelID;
			(*I).tNodeID			= m_tpaGraph[I - B].tNodeID;
			Memory.mem_copy			((*I).tVertexTypes,m_tpaGraph[I - B].tVertexTypes,LOCATION_TYPE_COUNT*sizeof(_LOCATION_ID));
			(*I).tNeighbourCount	= m_tpaGraph[I - B].tNeighbourCount;
			
			(*I).tpaEdges			= (SGraphEdge *)xr_malloc((*I).tNeighbourCount*sizeof(SGraphEdge));
			SGraphEdge				*tpaEdges = (SGraphEdge *)((BYTE *)m_tpaGraph + m_tpaGraph[I - B].dwEdgeOffset);
			for (int i=0; i<(int)(*I).tNeighbourCount; i++) {
				(*I).tpaEdges[i]	= tpaEdges[i];
				(*I).tpaEdges[i].dwVertexNumber += dwOffset;
			}
		}
		// fill vertex map
		{
			//pSettings								= xr_new<CInifile>(SYSTEM_LTX);
			string256								fName;
			//strconcat								(fName,name,tLevel.caLevelName);
			strconcat								(fName,S,"level.spawn");
			IReader									*F = FS.r_open(fName);
			IReader									*O = 0;
			for (int id=0, i=0; 0!=(O = F->open_chunk(id)); id++)	{
				NET_Packet							P;
				P.B.count							= O->length();
				O->r								(P.B.data,P.B.count);
				O->close							();
				u16									ID;
				P.r_begin							(ID);
				R_ASSERT							(M_SPAWN==ID);
				P.r_string							(fName);
				xrServerEntity						*E = F_entity_Create	(fName);
				R_ASSERT2							(E,"Can't create entity.");
				E->Spawn_Read						(P);
				xrGraphPoint						*tpGraphPoint = dynamic_cast<xrGraphPoint*>(E);
				if (tpGraphPoint) {
					Fvector							tVector;
					tVector							= tpGraphPoint->o_Position;
					_GRAPH_ID						tGraphID = _GRAPH_ID(-1);
					float							fMinDistance = 1000000.f;
					GRAPH_VERTEX_IT					B = m_tpVertices.begin();
					GRAPH_VERTEX_IT					I = B;
					GRAPH_VERTEX_IT					E = m_tpVertices.end();
					for ( ; I != E; I++) {
						float fDistance = (*I).tLocalPoint.distance_to(tVector);
						if (fDistance < fMinDistance) {
							fMinDistance	= fDistance;
							tGraphID		= _GRAPH_ID(I - B);
							if (fMinDistance < EPS_L)
								break;
						}
					}
					if (fMinDistance < EPS_L) {
						SConnectionVertex				T;
						LPSTR							S;
						S								= (char *)xr_malloc((strlen(tpGraphPoint->s_name_replace) + 1)*sizeof(char));
						T.caConnectName					= (char *)xr_malloc((strlen(tpGraphPoint->m_caConnectionPointName) + 1)*sizeof(char));
						T.dwLevelID						= tpGraphPoint->m_tLevelID;
						T.tGraphID						= i;
						Memory.mem_copy					(S,tpGraphPoint->s_name_replace,strlen(tpGraphPoint->s_name_replace) + 1);
						Memory.mem_copy					(T.caConnectName,tpGraphPoint->m_caConnectionPointName,strlen(tpGraphPoint->m_caConnectionPointName) + 1);
						m_tVertexMap.insert				(make_pair(S,T));
						i++;
					}
				}
				xr_delete							(E);
			}
			O->close								();
			//xr_delete								(pSettings);
		}
	};

	virtual							~CLevelGraph()
	{
		{
			GRAPH_VERTEX_IT			I = m_tpVertices.begin();
			GRAPH_VERTEX_IT			E = m_tpVertices.end();
			for ( ; I != E; I++)
				xr_free((*I).tpaEdges);
		}
		{
			VERTEX_PAIR_IT			I = m_tVertexMap.begin();
			VERTEX_PAIR_IT			E = m_tVertexMap.end();
			for ( ; I != E; I++) {
				LPSTR	S = (*I).first;
				xr_free(S);
				xr_free((*I).second.caConnectName);
			}
		}
	};

	void						vfAddEdge(u32 dwVertexNumber, SGraphEdge &tGraphEdge)
	{
		R_ASSERT(m_tGraphHeader.dwVertexCount > dwVertexNumber);
		m_tpVertices[dwVertexNumber].tpaEdges = (SGraphEdge *)xr_realloc(m_tpVertices[dwVertexNumber].tpaEdges,sizeof(SGraphEdge)*++m_tpVertices[dwVertexNumber].tNeighbourCount);
		m_tpVertices[dwVertexNumber].tpaEdges[m_tpVertices[dwVertexNumber].tNeighbourCount - 1] = tGraphEdge;
	}

	void						vfSaveVertices(CMemoryWriter &tMemoryStream, u32 &dwOffset)
	{
		GRAPH_VERTEX_IT			I = m_tpVertices.begin();
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		for ( ; I != E; I++) {
			SGraphVertex tVertex;
			tVertex.tLocalPoint		= (*I).tLocalPoint;
			tVertex.tGlobalPoint	= (*I).tGlobalPoint;
			tVertex.tNodeID			= (*I).tNodeID;
			Memory.mem_copy			(tVertex.tVertexTypes,(*I).tVertexTypes,LOCATION_TYPE_COUNT*sizeof(_LOCATION_ID));
			tVertex.tLevelID		= (*I).tLevelID;
			tVertex.dwEdgeOffset	= dwOffset;
			dwOffset				+= (tVertex.tNeighbourCount = (*I).tNeighbourCount)*sizeof(SGraphEdge);
			tMemoryStream.w		(&tVertex,sizeof(tVertex));
		}
	};
	
	void						vfSaveEdges(CMemoryWriter &tMemoryStream)
	{
		GRAPH_VERTEX_IT			I = m_tpVertices.begin();
		GRAPH_VERTEX_IT			E = m_tpVertices.end();
		for ( ; I != E; I++)
			for (int i=0; i<(int)(*I).tNeighbourCount; i++)
				tMemoryStream.w	((*I).tpaEdges + i,sizeof(SGraphEdge));
	};
};

void xrMergeGraphs(LPCSTR name)
{
	// load all the graphs
	Phase("Reading level graphs");
	CInifile *Ini = xr_new<CInifile>(INI_FILE);
	if (!Ini->section_exist("levels"))
		THROW;
	GRAPH_P_MAP						tpGraphs;
	string256						S1, S2;
	CALifeGraph::SLevel				tLevel;
	u32								dwOffset = 0;
	R_ASSERT						(Ini->section_exist("levels"));
    LPCSTR N,V;
    for (u32 k = 0; Ini->r_line("levels",k,&N,&V); k++) {
		R_ASSERT					(Ini->section_exist(N));
		tLevel.tOffset				= Ini->r_fvector3(N,"offset");
		V							= Ini->r_string(N,"name");
		Memory.mem_copy				(tLevel.caLevelName,V,strlen(V) + 1);
		Memory.mem_copy				(S1,V,strlen(V) + 1);
		strconcat					(S2,name,S1);
		strconcat					(S1,S2,"\\");//level.graph");
		tLevel.dwLevelID			= Ini->r_s32(N,"id");
		CLevelGraph					*tpLevelGraph = xr_new<CLevelGraph>(tLevel,S1,dwOffset,tLevel.dwLevelID);
		dwOffset					+= tpLevelGraph->m_tGraphHeader.dwVertexCount;
		tpGraphs.insert				(make_pair(tLevel.dwLevelID,tpLevelGraph));
		tGraphHeader.tpLevels.push_back(tLevel);
    }
	R_ASSERT(tpGraphs.size());
	
	Phase("Adding interconnection points");
	{
		GRAPH_P_PAIR_IT				I = tpGraphs.begin();
		GRAPH_P_PAIR_IT				E = tpGraphs.end();
		for ( ; I != E; I++) {
			VERTEX_PAIR_IT			i = (*I).second->m_tVertexMap.begin();
			VERTEX_PAIR_IT			e = (*I).second->m_tVertexMap.end();
			for ( ; i != e; i++)
				if ((*i).second.caConnectName[0]) {
					GRAPH_P_PAIR_IT				K;
					VERTEX_PAIR_IT				M;
					CALifeGraph::SGraphEdge		tGraphEdge;
					SConnectionVertex			&tConnectionVertex = (*i).second;
					R_ASSERT					((K = tpGraphs.find(tConnectionVertex.dwLevelID)) != tpGraphs.end());
					R_ASSERT					((M = (*K).second->m_tVertexMap.find(tConnectionVertex.caConnectName)) != (*K).second->m_tVertexMap.end());
					tGraphEdge.dwVertexNumber	= (*M).second.tGraphID + (*K).second->m_dwOffset;
					tGraphEdge.fPathDistance	= (*I).second->m_tpVertices[tConnectionVertex.tGraphID].tGlobalPoint.distance_to((*K).second->m_tpVertices[(*M).second.tGraphID].tGlobalPoint);
					(*I).second->vfAddEdge		((*i).second.tGraphID,tGraphEdge);
					tGraphEdge.dwVertexNumber	= (*i).second.tGraphID + (*I).second->m_dwOffset;
					(*K).second->vfAddEdge		((*M).second.tGraphID,tGraphEdge);
				}
		}

	}
	///////////////////////////////////////////////////
	
	// save all the graphs
	Phase("Saving graph being merged");
	CMemoryWriter						F;
	tGraphHeader.dwLevelCount		= tpGraphs.size();
	tGraphHeader.dwVersion			= XRAI_CURRENT_VERSION;
	tGraphHeader.dwVertexCount		= dwOffset;
	F.w_u32					(tGraphHeader.dwVersion);
	F.w_u32					(tGraphHeader.dwVertexCount);
	F.w_u32					(tGraphHeader.dwLevelCount);
	{
		vector<CALifeGraph::SLevel>::iterator	I = tGraphHeader.tpLevels.begin();
		vector<CALifeGraph::SLevel>::iterator	E = tGraphHeader.tpLevels.end();
		for ( ; I != E; I++) {
			F.w_stringZ	((*I).caLevelName);
			F.w_fvector3((*I).tOffset);
			F.w_u32		((*I).dwLevelID);
		}
	}

	dwOffset						*= sizeof(CALifeGraph::SGraphVertex);
	{
//		GRAPH_P_PAIR_IT				I = tpGraphs.begin();
//		GRAPH_P_PAIR_IT				E = tpGraphs.end();
//		for ( ; I != E; I++)
//			(*I).second->vfSaveVertices	(F,dwOffset);
		vector<CALifeGraph::SLevel>::iterator	I = tGraphHeader.tpLevels.begin();
		vector<CALifeGraph::SLevel>::iterator	E = tGraphHeader.tpLevels.end();
		for ( ; I != E; I++) {
			GRAPH_P_PAIR_IT			i = tpGraphs.find((*I).dwLevelID);
			R_ASSERT				(i != tpGraphs.end());
			(*i).second->vfSaveVertices	(F,dwOffset);
		}
	}
	{
		vector<CALifeGraph::SLevel>::iterator	I = tGraphHeader.tpLevels.begin();
		vector<CALifeGraph::SLevel>::iterator	E = tGraphHeader.tpLevels.end();
		for ( ; I != E; I++) {
			GRAPH_P_PAIR_IT			i = tpGraphs.find((*I).dwLevelID);
			R_ASSERT				(i != tpGraphs.end());
			(*i).second->vfSaveEdges(F);
		}
	}
	F.save_to("game.graph");
	
	// _free all the graphs
	Phase("Freeing resources being allocated");
	{
		GRAPH_P_PAIR_IT				I = tpGraphs.begin();
		GRAPH_P_PAIR_IT				E = tpGraphs.end();
		for ( ; I != E; I++)
			xr_free((*I).second);
	}
	xr_delete			(Ini);
}