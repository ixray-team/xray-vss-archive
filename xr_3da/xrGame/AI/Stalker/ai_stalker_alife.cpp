////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_alife.cpp
//	Created 	: 25.07.2003
//  Modified 	: 25.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife online behaviour
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../ai_alife.h"
#include "../../graph_engine.h"

void CAI_Stalker::vfChooseTask()
{
	if (!bfHealthIsGood()) {
		if (!bfCanTreat()) {
			if (!bfEnoughMoneyToTreat() || !bfEnoughTimeToTreat()) {
				m_tTaskState = eTaskStateSendSOS;
				return;
			}
			else {
				m_tTaskState = eTaskStateGoToCustomer;
				return;
			}
		}
		else {
			vfChooseTask();
			return;
		}
	}
	else {
		if (!bfEnoughEquipmentToGo()) {
			if (bfDistanceToTraderIsDanger() || !bfEnoughMoneyToEquip()) {
				m_tTaskState = eTaskStateSendSOS;
				return;
			}
			else {
				m_tTaskState = eTaskStateGoToCustomer;
				return;
			}
		}
		else {
			m_tDestGraphPointIndex	= _GRAPH_ID(-1);
			vfChooseHumanTask		();
			CSE_ALifeTask			*l_tpTask = m_tpALife->tpfGetTaskByID(m_tTaskID);
			switch (l_tpTask->m_tTaskType) {
				case eTaskTypeSearchForItemCG :
				case eTaskTypeSearchForItemOG : {
					m_tDestGraphPointIndex = l_tpTask->m_tGraphID;
					break;
				}
				case eTaskTypeSearchForItemCL :
				case eTaskTypeSearchForItemOL : {
#pragma todo("Dima to Dima : add graph point type item search")
					//m_baVisitedVertices.assign(tpALife->m_tpTerrain[0][l_tpTask->m_tLocationID].size(),false);
					//m_tDestGraphPointIndex = tpALife->m_tpTerrain[l_tpTask->m_tLocationID][m_dwCurTaskLocation = 0];
					//m_baVisitedVertices[m_dwCurTaskLocation] = true;
					break;
				}
				default				: NODEFAULT;
			};
			m_tTaskState			= eTaskStateAccomplishTask;
		}
	}
}

void CAI_Stalker::vfGoToCustomer()
{
//	if (m_tpGraphPath.empty()) {
//		ai().m_tpAStar->ffFindMinimalPath(m_tGraphID,m_tpALife->tpfGetObjectByID(m_tpALife->tpfGetTaskByID(m_tTaskID)->m_tCustomerID)->m_tGraphID,m_tpGraphPath);
//		m_dwCurGraphPathNode					= 0;
//		m_tNextGraphID				= m_tGraphID;
//	}
//	if ((m_dwCurGraphPathNode >= (m_tpGraphPath.size() - 1)) && (m_tGraphID == m_tNextGraphID)) {
//		m_tpGraphPath.clear();
//		m_dwCurGraphPathNode = 0;
//		if (int(m_tTaskID) > 0) {
////			CSE_ALifeTask			*l_tpTask = m_tpALife->tpfGetTaskByID(m_tTaskID);
////			CSE_ALifeTrader			*l_tpTrader = dynamic_cast<CSE_ALifeTrader*>(m_tpALife->tpfGetObjectByID(l_tpTask->m_tCustomerID));
////			if (l_tpTrader)
////				m_tpALife->vfCommunicateWithCustomer(this,l_tpTrader);
//		}
//		m_tTaskState = eTaskStateChooseTask;
//	}
}

void CAI_Stalker::vfAccomplishTask()
{
//	// build path and wait until we go to the end of it
//	if (m_tpGraphPath.empty() && (m_tGraphID != m_tDestGraphPointIndex)) {
//		ai().m_tpAStar->ffFindMinimalPath(m_tGraphID,m_tDestGraphPointIndex,m_tpGraphPath);
//		m_dwCurGraphPathNode	= 0;
//		m_tNextGraphID			= m_tGraphID;
//	}
//
//	if (m_tNextGraphID == m_tpALife->tpfGetTaskByID(m_tTaskID)->m_tGraphID)
//		m_tTaskState	= eTaskStateSearchItem;
//	else
//		m_tTaskState	= eTaskStateGoingToSearchItem;
}

void CAI_Stalker::vfFinishTask()
{
	if (bfCheckIfTaskCompleted())
		m_tTaskState		= eTaskStateGoToCustomer;
	else {
		CSE_ALifeTask		*l_tpALifeTask = m_tpALife->tpfGetTaskByID(m_tTaskID);
		switch (l_tpALifeTask->m_tTaskType) {
			case eTaskTypeSearchForItemCG :
			case eTaskTypeSearchForItemOG : {
				if (CMovementManager::path_completed() && (game_vertex_id() == l_tpALifeTask->m_tGraphID)) {
					++(l_tpALifeTask->m_dwTryCount);
					m_tTaskState = eTaskStateChooseTask;
				}
				break;
			}
			case eTaskTypeSearchForItemCL :
			case eTaskTypeSearchForItemOL : {
//					++(tpALife->tpfGetTaskByID(m_tTaskID)->m_dwTryCount);
//					m_tTaskState		= eTaskStateChooseTask;
//					for (++m_dwCurTaskLocation; (m_dwCurTaskLocation < tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID].size()) && (m_baVisitedVertices[m_dwCurTaskLocation]); ++m_dwCurTaskLocation);
//					if (m_dwCurTaskLocation < tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID].size()) {
//						m_baVisitedVertices[m_dwCurTaskLocation] = true;
//						tpAlife->ffFindMinimalPath(m_tGraphID,tpAlife->m_tpTerrain[m_tpTasks[m_dwCurTask]->m_tLocationID][m_dwCurTaskLocation],m_tpGraphPath);
//						m_dwCurGraphPathNode = 0;
//					}
//					else
				m_tTaskState	= eTaskStateChooseTask;
				break;
			}
			default :				NODEFAULT;
		}
	}
}

void CAI_Stalker::vfGoToSOS()
{
}

void CAI_Stalker::vfSendSOS()
{
}

void CAI_Stalker::ALifeUpdate()
{
	if (!m_tpALife) {
		vfContinueWithALifeGoals();
		return;
	}
	switch (m_tTaskState) {
		case eTaskStateChooseTask : {
			vfChooseTask();
			break;
		}
		case eTaskStateGoToCustomer : {
			vfGoToCustomer();
			break;
		}
		case eTaskStateGoToSOS : {
			vfGoToSOS();
			break;
		}
		case eTaskStateSendSOS : {
			vfSendSOS();
			break;
		}
		case eTaskStateAccomplishTask : {
			vfAccomplishTask();
			break;
		}
		case eTaskStateSearchItem : {
			vfSearchObject();
			break;
		}
		case eTaskStateGoingToCustomer :
		case eTaskStateGoingToSearchItem : {
			vfContinueWithALifeGoals();
			break;
		}
		default			: NODEFAULT;
	}
}