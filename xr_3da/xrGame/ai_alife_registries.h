////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_static_objects.h"

using namespace ALife;

class CAI_ALife;

class CALifeObjectRegistry : public IPureALifeLSObject {
public:
	_OBJECT_ID						m_tObjectID;			// ������������� ����� �������
	OBJECT_MAP						m_tObjectRegistry;		// ������ ������� ����

	CALifeObjectRegistry()
	{
		m_tObjectID					= 0;
		m_tObjectRegistry.clear		();
	};

	virtual							~CALifeObjectRegistry()
	{
		OBJECT_PAIR_IT it			= m_tObjectRegistry.begin();
		OBJECT_PAIR_IT E			= m_tObjectRegistry.end();
		for ( ; it != E; it++)
			_DELETE((*it).second);
		m_tObjectRegistry.clear();
	};
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.open_chunk	(OBJECT_CHUNK_DATA);
		tMemoryStream.write			(&m_tObjectID,sizeof(m_tObjectID));
		tMemoryStream.Wdword		(m_tObjectRegistry.size());
		OBJECT_PAIR_IT it			= m_tObjectRegistry.begin();
		OBJECT_PAIR_IT E			= m_tObjectRegistry.end();
		for ( ; it != E; it++) {
			CALifeTrader *tpALifeTrader = dynamic_cast<CALifeTrader *>((*it).second);
			if (tpALifeTrader)
				tMemoryStream.Wbyte(ALIFE_TRADER_ID);
			else {
				CALifeHuman	*tpALifeHuman = dynamic_cast<CALifeHuman *>((*it).second);
				if (tpALifeHuman)
					tMemoryStream.Wbyte(ALIFE_HUMAN_ID);
				else {
					CALifeHumanGroup	*tpALifeHumanGroup = dynamic_cast<CALifeHumanGroup *>((*it).second);
					if (tpALifeHumanGroup)
						tMemoryStream.Wbyte(ALIFE_HUMAN_GROUP_ID);
					else {
						CALifeMonster	*tpALifeMonster = dynamic_cast<CALifeMonster *>((*it).second);
						if (tpALifeMonster)
							tMemoryStream.Wbyte(ALIFE_MONSTER_ID);
						else {
							CALifeMonsterGroup	*tpALifeHumanGroup = dynamic_cast<CALifeMonsterGroup *>((*it).second);
							if (tpALifeHumanGroup)
								tMemoryStream.Wbyte(ALIFE_MONSTER_GROUP_ID);
							else {
								CALifeItem *tpALifeItem = dynamic_cast<CALifeItem *>((*it).second);
								if (tpALifeItem)
									tMemoryStream.Wbyte(ALIFE_ITEM_ID);
								else {
									CALifeAnomalousZone *tpALifeAnomalousZone = dynamic_cast<CALifeAnomalousZone *>((*it).second);
									if (tpALifeAnomalousZone)
										tMemoryStream.Wbyte(ALIFE_ANOMALOUS_ZONE_ID);
									else {
										Msg("!Unsupported ALife monster type!");
										R_ASSERT(false);
									}
								}
							}

						}
					}
				}
			}
			(*it).second->Save(tMemoryStream);
		}
		tMemoryStream.close_chunk	();
	};
	
	void CALifeObjectRegistry::Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(OBJECT_CHUNK_DATA));
		tFileStream.Read(&m_tObjectID,sizeof(m_tObjectID));
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			CALifeDynamicObject *tpALifeDynamicObject = 0;
			switch (tFileStream.Rbyte()) {
				case ALIFE_ITEM_ID : {
					tpALifeDynamicObject = new CALifeItem;
					break;
				}
				case ALIFE_MONSTER_ID : {
					tpALifeDynamicObject = new CALifeMonster;
					break;
				}
				case ALIFE_MONSTER_GROUP_ID : {
					tpALifeDynamicObject = new CALifeMonsterGroup;
					break;
				}
				case ALIFE_HUMAN_ID : {
					tpALifeDynamicObject = new CALifeHuman;
					break;
				}
				case ALIFE_HUMAN_GROUP_ID : {
					tpALifeDynamicObject = new CALifeHumanGroup;
					break;
				}
				case ALIFE_TRADER_ID : {
					tpALifeDynamicObject = new CALifeTrader;
					break;
				}
				case ALIFE_ANOMALOUS_ZONE_ID : {
					tpALifeDynamicObject = new CALifeDynamicAnomalousZone;
					break;
				}
				default : NODEFAULT;
			};
			tpALifeDynamicObject->Load	(tFileStream);
			m_tObjectRegistry.insert			(make_pair(tpALifeDynamicObject->m_tObjectID,tpALifeDynamicObject));
		}
	};

	virtual	void					Add	(CALifeDynamicObject *tpALifeDynamicObject)
	{
		m_tObjectRegistry.insert				(make_pair(tpALifeDynamicObject->m_tObjectID = m_tObjectID++,tpALifeDynamicObject));
	};

};

class CALifeEventRegistry : public IPureALifeLSObject {
public:
	_EVENT_ID						m_tEventID;				// ������������� ����� �������
	EVENT_MAP						m_tEventRegistry;		// ������ ������� ����

	CALifeEventRegistry()
	{
		m_tEventID					= 0;
		m_tEventRegistry.clear		();
	};

	virtual							~CALifeEventRegistry()
	{
		EVENT_PAIR_IT it			= m_tEventRegistry.begin();
		EVENT_PAIR_IT E				= m_tEventRegistry.end();
		for ( ; it != E; it++) {
			_DELETE((*it).second->m_tpMonsterGroup1);
			_DELETE((*it).second->m_tpMonsterGroup2);
		}
		m_tEventRegistry.clear();
	};
	
	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.open_chunk	(EVENT_CHUNK_DATA);
		tMemoryStream.write			(&m_tEventID,sizeof(m_tEventID));
		tMemoryStream.Wdword		(m_tEventRegistry.size());
		EVENT_PAIR_IT it			= m_tEventRegistry.begin();
		EVENT_PAIR_IT E				= m_tEventRegistry.end();
		for ( ; it != E; it++) {
			CALifeEvent				&tEvent = *((*it).second);
			tMemoryStream.write		(&tEvent.m_tEventID,		sizeof(tEvent.m_tEventID		));
			tMemoryStream.write		(&tEvent.m_tTimeID,			sizeof(tEvent.m_tTimeID		));
			tMemoryStream.write		(&tEvent.m_tGraphID,		sizeof(tEvent.m_tGraphID		));
			tMemoryStream.write		(&tEvent.m_tBattleResult,	sizeof(tEvent.m_tBattleResult	));
			tEvent.m_tpMonsterGroup1->Save(tMemoryStream);
			tEvent.m_tpMonsterGroup2->Save(tMemoryStream);
		}
		tMemoryStream.close_chunk	();
	};

	virtual	void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(EVENT_CHUNK_DATA));
		tFileStream.Read(&m_tEventID,sizeof(m_tEventID));
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			CALifeEvent				tEvent;
			tFileStream.Read		(&tEvent.m_tEventID,		sizeof(tEvent.m_tEventID		));
			tFileStream.Read		(&tEvent.m_tTimeID,		sizeof(tEvent.m_tTimeID		));
			tFileStream.Read		(&tEvent.m_tGraphID,		sizeof(tEvent.m_tGraphID		));
			tFileStream.Read		(&tEvent.m_tBattleResult,	sizeof(tEvent.m_tBattleResult	));
			tEvent.m_tpMonsterGroup1	= new CALifeEventGroup;
			tEvent.m_tpMonsterGroup2	= new CALifeEventGroup;
			tEvent.m_tpMonsterGroup1->Load(tFileStream);
			tEvent.m_tpMonsterGroup2->Load(tFileStream);
			m_tEventRegistry.insert	(make_pair(tEvent.m_tEventID,&tEvent));
		}
	};
	
	virtual	void					Add	(CALifeEvent	*tpEvent)
	{
		m_tEventRegistry.insert		(make_pair(tpEvent->m_tEventID = m_tEventID++,tpEvent));
	};
};

class CALifeTaskRegistry : public IPureALifeLSObject {
public:
	_TASK_ID						m_tTaskID;				// ������������� ����� �������
	TASK_MAP						m_tTaskRegistry;		// ������ ������� ����

	CALifeTaskRegistry()
	{
		m_tTaskID					= 0;
		m_tTaskRegistry.clear				();
	};

	virtual	void					Save(CFS_Memory &tMemoryStream)
	{
		tMemoryStream.open_chunk	(TASK_CHUNK_DATA);
		tMemoryStream.write			(&m_tTaskID,sizeof(m_tTaskID));
		tMemoryStream.Wdword		(m_tTaskRegistry.size());
		TASK_PAIR_IT it				= m_tTaskRegistry.begin();
		TASK_PAIR_IT E				= m_tTaskRegistry.end();
		for ( ; it != E; it++)
			tMemoryStream.write		(&((*it).second),		sizeof((*it).second));
		tMemoryStream.close_chunk	();
	};
	
	virtual	void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(TASK_CHUNK_DATA));
		tFileStream.Read(&m_tTaskID,sizeof(m_tTaskID));
		u32 dwCount = tFileStream.Rdword();
		for (u32 i=0; i<dwCount; i++) {
			CALifeTask				tTask;
			tFileStream.Read		(&tTask,		sizeof(tTask));
			m_tTaskRegistry.insert	(make_pair(tTask.m_tTaskID,&tTask));
		}
	};
	
	virtual	void					Add	(CALifeTask	*tpTask)
	{
		m_tTaskRegistry.insert		(make_pair(tpTask->m_tTaskID = m_tTaskID++,tpTask));
	};
};

class CALifeSpawnRegistry : public CALifeSpawnHeader {
public:
	typedef CALifeSpawnHeader inherited;
	
	SPAWN_P_VECTOR					m_tpSpawnPoints;
	
	virtual void					Load(CStream	&tFileStream)
	{
		inherited::Load(tFileStream);
		R_ASSERT(tFileStream.FindChunk(SPAWN_POINT_CHUNK_DATA));
		load_vector(m_tpSpawnPoints,tFileStream);
	};
};