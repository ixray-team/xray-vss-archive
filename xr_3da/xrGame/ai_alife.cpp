////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

#define TOTAL_COUNT			  50
#define MONSTER_FACTOR		.40f
#define HUMAN_FACTOR		.10f
#define WEAPON_FACTOR		.10f
#define WEAPON_CH_FACTOR	.10f
#define EQUIPMENT_FACTOR	.10f
#define ARTEFACT_FACTOR		.20f

FLOAT_VECTOR	fpFactors;

const char *cpMonsterModels[] = {
	// monsters
	"ent_rat",
	"ent_zombie",
	"ent_dog",
	"ent_controller",
	0
};
const char *cpHumanModels[] = {
	// human beings
	"ent_soldier",
	"ent_stalker",
	"ent_trader",
	0
};
const char *cpWeaponModels[] = {
	// weapon
	"wpn_fn2000",
	"wpn_lr300",
	"wpn_ak74",
	"wpn_hpsa",
	"wpn_pm",
	"wpn_fort",
	"wpn_binoc",
	"wpn_toz34",
	0
};
const char *cpWeaponChModels[] = {
	// wepon charges
	"wpn_fn2000_ch",
	"wpn_lr300_ch",
	"wpn_ak74_ch",
	"wpn_hpsa_ch",
	"wpn_pm_ch",
	"wpn_fort_ch",
	"wpn_toz34_ch",
	0
};
const char *cpEquipmentModels[] = {
	// equipment
	"eq_radio",
	"eq_life_saver",
	"eq_capsule",
	"eq_container",
	"eq_psi_probe",
	"eq_u_detector",
	"eq_lf_detector",
	"eq_medikit",
	"eq_u_medikit",
	"eq_p_suit",
	"eq_st_suit",
	"eq_sc_suit",
	"eq_ar_suit",
	0
};
const char *cpArtefactModels[] = {
	// artefacts
	"art_gravi",
	"art_radio",
	"art_magnet",
	"art_mball",
	"art_black_droplets",
	0
};

CAI_ALife::CAI_ALife()
{
	m_bLoaded			= false;
}

CAI_ALife::~CAI_ALife()
{
	shedule_Unregister	();
	//Save				();
}

void CAI_ALife::vfInitTerrain()
{
	m_tpTerrain.resize(LOCATION_COUNT);
	{
		GRAPH_VECTOR_IT		I = m_tpTerrain.begin();
		GRAPH_VECTOR_IT		E = m_tpTerrain.end();
		for ( ; I != E; I++)
			(*I).clear();
	}
	for (_GRAPH_ID i=0; i<(_GRAPH_ID)Level().AI.GraphHeader().dwVertexCount; i++)
		m_tpTerrain[Level().AI.m_tpaGraph[i].tVertexType].push_back(i);
}

// temporary
IC bool	bfSpawnPointPredicate(CALifeSpawnPoint *v1, CALifeSpawnPoint *v2)
{
	return(v1->m_wGroupID <= v2->m_wGroupID);
}

void CAI_ALife::vfGenerateSpawnPoints(const u32 dwTotalCount, FLOAT_VECTOR &fpFactors)
{
	/**
	vector<bool>				tpMarks;
	tpMarks.resize				(Level().AI.GraphHeader().dwVertexCount);
	tpMarks.assign				(tpMarks.size(),false);
	AI::SGraphVertex			*tpaGraph = Level().AI.m_tpaGraph;
	const char					**cppModels[] = { cpMonsterModels, cpHumanModels, cpWeaponModels, cpWeaponChModels, cpEquipmentModels, cpArtefactModels, 0};
	DWORD_VECTOR				dwpModelCounts;
	DWORD_VECTOR				dwpSpawnCounts;
	
	for (u32 dwModelCount = 0; cppModels[dwModelCount]; dwModelCount++);
	dwpModelCounts.resize		(dwModelCount);
	dwpSpawnCounts.resize		(dwModelCount);
	
	DWORD_IT					B = dwpModelCounts.begin();
	DWORD_IT					E = dwpModelCounts.end();
	DWORD_IT					I = B;
	
	for ( ; I != E; I++)
		for ( *I = 0; cppModels[I - B][*I]; ++*I);

	B							= dwpSpawnCounts.begin();
	E							= dwpSpawnCounts.end();
	I							= B;
	{
		FLOAT_IT				i = fpFactors.begin();
		for ( ; I != E; I++, i++)
			*I = u32((*i)*float(dwTotalCount));
	}
	u32 dwSpawnCount = 0;
	for (I = B; I != E; I++)
		dwSpawnCount += *I;
	
	//dwSpawnCount += 3*dwpSpawnCounts[1];
	m_tpSpawnPoints.resize		(dwSpawnCount);
	m_tSpawnHeader.dwCount		= dwSpawnCount;
	m_tSpawnHeader.dwVersion	= SPAWN_POINT_VERSION;
	u16 wGroupID				= 0;
	::Random.seed				(0);
	int							ii = 0;
	for (I = B; I != E; I++)
		for (int i=0; i<(int)*I; i++, ii++) {
			m_tpSpawnPoints[ii].tNearestGraphPointID		= (u16)::Random.randI(Level().AI.GraphHeader().dwVertexCount);
			bool bOk = false;
			for (int j=0; j<ii; j++)
				if (m_tpSpawnPoints[j].tNearestGraphPointID == m_tpSpawnPoints[ii].tNearestGraphPointID) {
					bOk = true;
					m_tpSpawnPoints[ii].wGroupID = m_tpSpawnPoints[j].wGroupID;
					break;
				}
			if (!bOk)
				m_tpSpawnPoints[ii].wGroupID = wGroupID++;
			j = ::Random.randI(dwpModelCounts[I - B]);
			PSGP.memCopy(m_tpSpawnPoints[ii].caModel,cppModels[I - B][j],(1 + strlen(cppModels[I - B][j]))*sizeof(char));
			m_tpSpawnPoints[ii].ucTeam					= (u8)::Random.randI(255);
			m_tpSpawnPoints[ii].ucSquad					= (u8)::Random.randI(255);
			m_tpSpawnPoints[ii].ucGroup					= (u8)::Random.randI(255);
			if (I == B)
				m_tpSpawnPoints[ii].wCount				= (u16)::Random.randI(5,45);
			else
				m_tpSpawnPoints[ii].wCount				= 1;
			m_tpSpawnPoints[ii].fBirthRadius			= 10.f;
			m_tpSpawnPoints[ii].fBirthProbability		= 1.0f;
			m_tpSpawnPoints[ii].fIncreaseCoefficient	= 1.01f;
			m_tpSpawnPoints[ii].fAnomalyDeathProbability= 0.0f;
			m_tpSpawnPoints[ii].ucRoutePointCount		= 0;
			m_tpSpawnPoints[ii].tpRouteGraphPoints.clear();
			if (I == B) {
				m_tpSpawnPoints[ii].tpRouteGraphPoints.push_back(m_tpSpawnPoints[ii].tNearestGraphPointID);
				tpMarks[m_tpSpawnPoints[ii].tNearestGraphPointID] = true;
				u16				wPoint = m_tpSpawnPoints[ii].tNearestGraphPointID;
				int				wCount = tpaGraph[wPoint].dwNeighbourCount;
				AI::SGraphEdge	*tpaEdges = (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[wPoint].dwEdgeOffset);
				for ( j=0; j<(int)wCount; j++) {
					if (!tpMarks[tpaEdges[j].dwVertexNumber]) {
						m_tpSpawnPoints[ii].tpRouteGraphPoints.push_back((u16)tpaEdges[j].dwVertexNumber);
						tpMarks[tpaEdges[j].dwVertexNumber] = true;
					}
					u32				wPoint1 = tpaEdges[j].dwVertexNumber;
					int				wCount1 = tpaGraph[wPoint1].dwNeighbourCount;
					AI::SGraphEdge	*tpaEdges1 = (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[wPoint1].dwEdgeOffset);
					for (int k=0; k<wCount1; k++)
						if (!tpMarks[tpaEdges1[k].dwVertexNumber]) {
							m_tpSpawnPoints[ii].tpRouteGraphPoints.push_back((u16)tpaEdges1[k].dwVertexNumber);
							tpMarks[tpaEdges1[k].dwVertexNumber] = true;
						}
				}
				m_tpSpawnPoints[ii].ucRoutePointCount			= (u8)m_tpSpawnPoints[ii].tpRouteGraphPoints.size();
				for ( j=0; j<(int)m_tpSpawnPoints[ii].ucRoutePointCount; j++)
					tpMarks[m_tpSpawnPoints[ii].tpRouteGraphPoints[j]] = false;
			}
		}
	/**/
	m_tSpawnVersion				= SPAWN_POINT_VERSION;
	u16 wGroupID				= 0;
	m_tpSpawnPoints.resize		(Level().AI.GraphHeader().dwVertexCount + 2);
	int							j;
	SPAWN_P_IT					B = m_tpSpawnPoints.begin();
	SPAWN_P_IT					E = m_tpSpawnPoints.end() - 2;
	SPAWN_P_IT					I = B;
	for ( ; I != E; I++) {
		(*I)							= xr_new<CALifeSpawnPoint> ();
		(*I)->m_tNearestGraphPointID	= _GRAPH_ID(I - B);
		(*I)->m_wGroupID				= wGroupID++;
		j								= ::Random.randI(5);
		PSGP.memCopy					((*I)->m_caModel,cpArtefactModels[j],(1 + strlen(cpArtefactModels[j]))*sizeof(char));
		(*I)->m_ucTeam					= 0;
		(*I)->m_ucSquad					= 0;
		(*I)->m_ucGroup					= 0;
		(*I)->m_wCount					= 1;
		(*I)->m_fBirthRadius			= 10.f;
		(*I)->m_fBirthProbability		= 1.0f;
		(*I)->m_fIncreaseCoefficient	= 1.0f;
		(*I)->m_fAnomalyDeathProbability= 0.0f;
		(*I)->m_tpRouteGraphPoints.clear();
	}
	(*I)							= xr_new<CALifeSpawnPoint> ();
	(*I)->m_tNearestGraphPointID	= _GRAPH_ID(::Random.randI(Level().AI.GraphHeader().dwVertexCount));
	(*I)->m_wGroupID				= wGroupID++;
	j								= 1;
	PSGP.memCopy					((*I)->m_caModel,cpHumanModels[j],(1 + strlen(cpHumanModels[j]))*sizeof(char));
	(*I)->m_ucTeam					= 1;
	(*I)->m_ucSquad					= 0;
	(*I)->m_ucGroup					= 0;
	(*I)->m_wCount					= 1;
	(*I)->m_fBirthRadius			= 10.f;
	(*I)->m_fBirthProbability		= 1.0f;
	(*I)->m_fIncreaseCoefficient	= 1.0f;
	(*I)->m_fAnomalyDeathProbability= 0.0f;
	(*I)->m_tpRouteGraphPoints.clear();

	I++;

	(*I)							= xr_new<CALifeSpawnPoint> ();
	(*I)->m_tNearestGraphPointID	= _GRAPH_ID(::Random.randI(Level().AI.GraphHeader().dwVertexCount));
	(*I)->m_wGroupID				= wGroupID++;
	j								= 2;
	PSGP.memCopy					((*I)->m_caModel,cpHumanModels[j],(1 + strlen(cpHumanModels[j]))*sizeof(char));
	(*I)->m_ucTeam					= 1;
	(*I)->m_ucSquad					= 0;
	(*I)->m_ucGroup					= 0;
	(*I)->m_wCount					= 1;
	(*I)->m_fBirthRadius			= 10.f;
	(*I)->m_fBirthProbability		= 1.0f;
	(*I)->m_fIncreaseCoefficient	= 1.0f;
	(*I)->m_fAnomalyDeathProbability= 0.0f;
	(*I)->m_tpRouteGraphPoints.clear();
	/**/
	sort(m_tpSpawnPoints.begin(),m_tpSpawnPoints.end(),bfSpawnPointPredicate);
}

void CAI_ALife::vfSaveSpawnPoints()
{
	CFS_Memory	tStream;
	tStream.open_chunk	(SPAWN_POINT_CHUNK_VERSION);
	tStream.write		(&m_tSpawnVersion,sizeof(m_tSpawnVersion));
	tStream.close_chunk	();
	tStream.open_chunk	(SPAWN_POINT_CHUNK_DATA);
	tStream.Wdword		(m_tpSpawnPoints.size());
	SPAWN_P_IT			I = m_tpSpawnPoints.begin();
	SPAWN_P_IT			E = m_tpSpawnPoints.end();
	for ( ; I != E; I++) {
		tStream.Wword	((*I)->m_tNearestGraphPointID);
		tStream.Wstring	((*I)->m_caModel);
		tStream.Wbyte	((*I)->m_ucTeam);
		tStream.Wbyte	((*I)->m_ucSquad);
		tStream.Wbyte	((*I)->m_ucGroup);
		tStream.Wword	((*I)->m_wGroupID);
		tStream.Wword	((*I)->m_wCount);
		tStream.Wfloat	((*I)->m_fBirthRadius);
		tStream.Wfloat	((*I)->m_fBirthProbability);
		tStream.Wfloat	((*I)->m_fIncreaseCoefficient);
		tStream.Wfloat	((*I)->m_fAnomalyDeathProbability);
		save_base_vector((*I)->m_tpRouteGraphPoints,tStream);
	}
	tStream.close_chunk	();
	tStream.SaveTo		("game.spawn",0);
}

#ifdef DEBUG
void CAI_ALife::vfRandomizeGraphTerrain()
{
	SGraphVertex *tpaGraph = (SGraphVertex *)xr_malloc(Level().AI.m_tpGraphVFS->Length());
	PSGP.memCopy(tpaGraph,(BYTE *)Level().AI.m_tpaGraph - sizeof(SGraphHeader),Level().AI.m_tpGraphVFS->Length());
	tpaGraph = (SGraphVertex *)((BYTE *)tpaGraph + sizeof(SGraphHeader));
	for (int i=0, m=0; i<(int)Level().AI.GraphHeader().dwVertexCount; i++)
		if (!tpaGraph[i].tVertexType) {
			m = (m + 1) & 0xff;
			tpaGraph[i].tVertexType = (u32)m;
			int			wCount = tpaGraph[i].dwNeighbourCount;
			SGraphEdge	*tpaEdges = (SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[i].dwEdgeOffset);
			for (int j=0; j<(int)wCount; j++)
				if (!tpaGraph[tpaEdges[j].dwVertexNumber].tVertexType) {
					tpaGraph[tpaEdges[j].dwVertexNumber].tVertexType = m;
					int			wCount1 = tpaGraph[tpaEdges[j].dwVertexNumber].dwNeighbourCount;
					SGraphEdge	*tpaEdges1 = (SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[tpaEdges[j].dwVertexNumber].dwEdgeOffset);
					for (int k=0; k<wCount1; k++)
						if (!tpaGraph[tpaEdges1[k].dwVertexNumber].tVertexType)
							tpaGraph[tpaEdges1[k].dwVertexNumber].tVertexType = m;
				}
		}
	tpaGraph = (SGraphVertex *)((BYTE *)tpaGraph - sizeof(SGraphHeader));
	FILE *F = fopen("level.graph","wb");
	fwrite(tpaGraph,1,Level().AI.m_tpGraphVFS->Length(),F);
	fclose(F);
	xr_free(tpaGraph);
}
#endif

// end of temporary
void CAI_ALife::Load()
{
	shedule_Min					=     1;
	shedule_Max					=    20;
	m_dwObjectsBeingProcessed	=     0;
	m_qwMaxProcessTime			=  100*CPU::cycles_per_microsec;
	m_dwStartTime				=	Level().timeServer();
	m_fTimeFactor				=   1.0f;

	// checking if graph is loaded
	if (!Level().AI.m_tpaGraph)
		return;

	// initializing terrain
	vfInitTerrain();

	// loading spawn-points
	CStream						*tpStream;
	FILE_NAME					caFileName;
	if (!Engine.FS.Exist(caFileName, ::Path.GameData, "game.spawn")) {
//		THROW;
#ifdef DEBUG
		fpFactors.push_back(MONSTER_FACTOR);
		fpFactors.push_back(HUMAN_FACTOR);
		fpFactors.push_back(WEAPON_FACTOR);
		fpFactors.push_back(WEAPON_CH_FACTOR);
		fpFactors.push_back(EQUIPMENT_FACTOR);
		fpFactors.push_back(ARTEFACT_FACTOR);
		vfGenerateSpawnPoints(TOTAL_COUNT,fpFactors);
		vfSaveSpawnPoints();
		vfRandomizeGraphTerrain();
#else
		return;
#endif
	}
	else {
		tpStream = Engine.FS.Open(caFileName);
		CALifeSpawnRegistry::Load(*tpStream);
		Engine.FS.Close(tpStream);
	}

	if (!Engine.FS.Exist(caFileName,::Path.GameData,"game.alife")) {
		Generate();
		Save();
	}
	
	if (!Engine.FS.Exist(caFileName,::Path.GameData,"game.alife"))
		THROW;
	
	tpStream = Engine.FS.Open	(caFileName);
	
	CALifeHeader::Load			(*tpStream);
	CALifeGameTime::Load		(*tpStream);
	CALifeObjectRegistry::Load	(*tpStream);
	CALifeEventRegistry::Load	(*tpStream);
	CALifeTaskRegistry::Load	(*tpStream);
	
	vfUpdateDynamicData			();

	Engine.FS.Close				(tpStream);

	m_bLoaded					= true;
}

void CAI_ALife::Save()
{
	CFS_Memory					tStream;
	m_tGameTime					= tfGetGameTime();
	CALifeHeader::Save			(tStream);
	CALifeGameTime::Save		(tStream);
	CALifeObjectRegistry::Save	(tStream);
	CALifeEventRegistry::Save	(tStream);
	CALifeTaskRegistry::Save	(tStream);
	tStream.SaveTo				("game.alife",0);
}

void CAI_ALife::Generate()
{
//	SPAWN_P_IT	B = m_tpSpawnPoints.begin();
//	SPAWN_P_IT	E  = m_tpSpawnPoints.end();
//	for (SPAWN_P_IT I = B ; I != E; ) {
//		u16	wGroupID = I->wGroupID;
//		float fSum = I->fBirthProbability;
//		for (SPAWN_P_IT j= I + 1; (j != E) && (j->wGroupID == wGroupID); j++)
//			fSum += j->fBirthProbability;
//		float fProbability = ::Random.randF(0,fSum);
//		fSum = I->fBirthProbability;
//		SPAWN_P_IT m = j, k = I;
//		for ( j= I + 1; (j != E) && (j->wGroupID == wGroupID); j++) {
//			fSum += j->fBirthProbability;
//			if (fSum > fProbability) {
//				k = j;
//				break;
//			}
//		}
//		vfCreateNewDynamicObject(k);
//		I = m;	
//	}
	vfCreateNewDynamicObject	(m_tpSpawnPoints.end() - 2);
	vfCreateNewDynamicObject	(m_tpSpawnPoints.end() - 1);

	m_tALifeVersion				= ALIFE_VERSION;
	m_tGameTime					= u64(Level().timeServer());
}

void CAI_ALife::vfGenerateArtefacts()
{
}

void CAI_ALife::vfCreateZoneShot()
{
}

void CAI_ALife::vfSellArtefacts(CALifeTrader &tTrader)
{
}

void CAI_ALife::vfUpdateArtefactOrders(CALifeTrader &tTrader)
{
}

void CAI_ALife::vfGiveMilitariesBribe(CALifeTrader &tTrader)
{
}

void CAI_ALife::vfBuySupplies(CALifeTrader &tTrader)
{
}

void CAI_ALife::vfBuyZoneShot(CALifeTrader &tTrader)
{
}

void CAI_ALife::vfAssignPrices(CALifeTrader &tTrader)
{
}

void CAI_ALife::vfBallanceCreatures()
{
}

void CAI_ALife::vfUpdateCreatures()
{
}

