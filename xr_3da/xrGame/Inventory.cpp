#include "stdafx.h"
#include "inventory.h"
#include "actor.h"
#include "trade.h"
#include "weapon.h"

#include "ui/UIInventoryUtilities.h"

#include "eatable_item.h"
#include "script_engine.h"
#include "xrmessages.h"
//#include "game_cl_base.h"
#include "xr_level_controller.h"
#include "level.h"
#include "ai_space.h"
#include "entitycondition.h"
#include "game_base_space.h"
#include "clsid_game.h"

using namespace InventoryUtilities;

// what to block
u32	INV_STATE_LADDER		= (1<<RIFLE_SLOT);
u32	INV_STATE_CAR			= INV_STATE_LADDER;
u32	INV_STATE_BLOCK_ALL		= 0xffffffff;
u32	INV_STATE_INV_WND		= INV_STATE_BLOCK_ALL;
u32	INV_STATE_BUY_MENU		= INV_STATE_BLOCK_ALL;

CInventorySlot::CInventorySlot() 
{
	m_pIItem				= NULL;
	m_bVisible				= true;
	m_bPersistent			= false;
	m_blockCounter			= 0;
}

CInventorySlot::~CInventorySlot() 
{
}

bool CInventorySlot::CanBeActivated() const 
{
	return (m_bVisible && !IsBlocked());
};

bool CInventorySlot::IsBlocked() const 
{
	return (m_blockCounter>0);
}

CInventory::CInventory() 
{
	m_fTakeDist									= pSettings->r_float	("inventory","take_dist");
	m_fMaxWeight								= pSettings->r_float	("inventory","max_weight");
	m_iMaxBelt									= pSettings->r_s32		("inventory","max_belt");
	
	m_slots.resize								(SLOTS_TOTAL);
	
	m_iActiveSlot								= NO_ACTIVE_SLOT;
	m_iNextActiveSlot							= NO_ACTIVE_SLOT;
	m_iPrevActiveSlot							= NO_ACTIVE_SLOT;
	m_iLoadActiveSlot							= NO_ACTIVE_SLOT;
	m_pTarget									= NULL;

	string256 temp;
	for(u32 i=0; i<m_slots.size(); ++i ) 
	{
		sprintf(temp, "slot_persistent_%d", i+1);
		if(pSettings->line_exist("inventory",temp)) 
			m_slots[i].m_bPersistent = !!pSettings->r_bool("inventory",temp);
	};

	m_slots[PDA_SLOT].m_bVisible				= false;
	m_slots[OUTFIT_SLOT].m_bVisible				= false;
	m_slots[DETECTOR_SLOT].m_bVisible			= false;
	m_slots[TORCH_SLOT].m_bVisible				= false;

	m_bSlotsUseful								= true;
	m_bBeltUseful								= false;

	m_fTotalWeight								= -1.f;
	m_dwModifyFrame								= 0;
	m_drop_last_frame							= false;
	m_iLoadActiveSlotFrame						= u32(-1);
}


CInventory::~CInventory() 
{
}

void CInventory::Clear()
{
	m_all.clear							();
	m_ruck.clear						();
	m_belt.clear						();
	
	for(u32 i=0; i<m_slots.size(); i++)
	{
		m_slots[i].m_pIItem				= NULL;
	}
	

	m_pOwner							= NULL;

	CalcTotalWeight						();
	InvalidateState						();
}

void CInventory::Take(CGameObject *pObj, bool bNotActivate, bool strict_placement)
{
	CInventoryItem *pIItem				= smart_cast<CInventoryItem*>(pObj);
	VERIFY								(pIItem);
	VERIFY								(pIItem->m_pInventory==NULL);
	VERIFY								(CanTakeItem(pIItem));
	
	pIItem->m_pInventory				= this;
	pIItem->SetDrop						(FALSE);

	m_all.push_back						(pIItem);

	if(!strict_placement)
		pIItem->m_eItemPlace			= eItemPlaceUndefined;

	bool result							= false;
	switch(pIItem->m_eItemPlace)
	{
	case eItemPlaceBelt:
		result							= Belt(pIItem); 
#ifdef DEBUG
		if(!result) 
			Msg("cant put in belt item %s", *pIItem->object().cName());
#endif

		break;
	case eItemPlaceRuck:
		result							= Ruck(pIItem);
#ifdef DEBUG
		if(!result) 
			Msg("cant put in ruck item %s", *pIItem->object().cName());
#endif

		break;
	case eItemPlaceSlot:
		result							= Slot(pIItem, bNotActivate); 
#ifdef DEBUG
		if(!result) 
			Msg("cant slot in ruck item %s", *pIItem->object().cName());
#endif

		break;
	default:
		if(CanPutInSlot(pIItem))
		{
			result						= Slot(pIItem, bNotActivate); VERIFY(result);
		} 
		else if ( !pIItem->RuckDefault() && CanPutInBelt(pIItem))
		{
			result						= Belt(pIItem); VERIFY(result);
		}
		else
		{
			result						= Ruck(pIItem); VERIFY(result);
		}
	}
	
	m_pOwner->OnItemTake				(pIItem);

	CalcTotalWeight						();
	InvalidateState						();

	pIItem->object().processing_deactivate();
	VERIFY								(pIItem->m_eItemPlace != eItemPlaceUndefined);
}

bool CInventory::Drop(CGameObject *pObj, bool call_drop) 
{
	CInventoryItem *pIItem				= smart_cast<CInventoryItem*>(pObj);
	VERIFY								(pIItem);
	if(pIItem->m_pInventory!=this)		return false;

	VERIFY								(pIItem->m_pInventory);
	VERIFY								(pIItem->m_pInventory==this);
	VERIFY								(pIItem->m_eItemPlace!=eItemPlaceUndefined);

	pIItem->object().processing_activate(); 
	
	switch(pIItem->m_eItemPlace)
	{
	case eItemPlaceBelt:{
			VERIFY(InBelt(pIItem));
			m_belt.erase(std::find(m_belt.begin(), m_belt.end(), pIItem));
			pIItem->object().processing_deactivate();
		}break;
	case eItemPlaceRuck:{
			VERIFY(InRuck(pIItem));
			m_ruck.erase(std::find(m_ruck.begin(), m_ruck.end(), pIItem));
		}break;
	case eItemPlaceSlot:{
			VERIFY(InSlot(pIItem));
			if(m_iActiveSlot == pIItem->GetSlot()) Activate(NO_ACTIVE_SLOT);
			m_slots[pIItem->GetSlot()].m_pIItem = NULL;							
			pIItem->object().processing_deactivate();
		}break;
	default:
		NODEFAULT;
	};
	TIItemContainer::iterator it = std::find(m_all.begin(), m_all.end(), pIItem);
	if(it!=m_all.end())
		m_all.erase(std::find(m_all.begin(), m_all.end(), pIItem));
	else
		Msg("! CInventory::Drop item not found in inventory!!!");

	pIItem->m_pInventory = NULL;

	if (call_drop && smart_cast<CInventoryItem*>(pObj))
		m_pOwner->OnItemDrop	(smart_cast<CInventoryItem*>(pObj));

	CalcTotalWeight					();
	InvalidateState					();
	m_drop_last_frame				= true;
	return							true;
}

bool CInventory::DropAll()
{
	TIItemContainer::iterator it;

	for(it = m_all.begin(); m_all.end() != it; ++it)
	{
		PIItem pIItem				= *it;
		Ruck						(pIItem);
		pIItem->Drop				();
	}
	
	CalcTotalWeight					();
	InvalidateState						();

	return							true;
}

//�������� ���� � ����
bool CInventory::Slot(PIItem pIItem, bool bNotActivate) 
{
	VERIFY(pIItem);
//	Msg("To Slot %s[%d]", *pIItem->object().cName(), pIItem->object().ID());
	
	if(!CanPutInSlot(pIItem)) 
	{
#if 0//def _DEBUG
		Msg("there is item %s[%d,%x] in slot %d[%d,%x]", 
				*m_slots[pIItem->GetSlot()].m_pIItem->object().cName(), 
				m_slots[pIItem->GetSlot()].m_pIItem->object().ID(), 
				m_slots[pIItem->GetSlot()].m_pIItem, 
				pIItem->GetSlot(), 
				pIItem->object().ID(),
				pIItem);
#endif
		if(m_slots[pIItem->GetSlot()].m_pIItem == pIItem && !bNotActivate )
			Activate(pIItem->GetSlot());

		return false;
	}


	m_slots[pIItem->GetSlot()].m_pIItem = pIItem;

	//������� �� ������� ��� �����
	TIItemContainer::iterator it = std::find(m_ruck.begin(), m_ruck.end(), pIItem);
	if(m_ruck.end() != it) m_ruck.erase(it);
	it = std::find(m_belt.begin(), m_belt.end(), pIItem);
	if(m_belt.end() != it) m_belt.erase(it);



	if (( (m_iActiveSlot==pIItem->GetSlot())||(m_iActiveSlot==NO_ACTIVE_SLOT) && m_iNextActiveSlot==NO_ACTIVE_SLOT) && (!bNotActivate))
		Activate				(pIItem->GetSlot());

	
	m_pOwner->OnItemSlot		(pIItem, pIItem->m_eItemPlace);
	pIItem->m_eItemPlace		= eItemPlaceSlot;
	pIItem->OnMoveToSlot		();
	
	pIItem->object().processing_activate();

	return						true;
}

bool CInventory::Belt(PIItem pIItem) 
{
	if(!CanPutInBelt(pIItem))	return false;
	
	//���� ���� � �����
	bool in_slot = InSlot(pIItem);
	if(in_slot) 
	{
		if(m_iActiveSlot == pIItem->GetSlot()) Activate(NO_ACTIVE_SLOT);
		m_slots[pIItem->GetSlot()].m_pIItem = NULL;
	}
	
	m_belt.insert(m_belt.end(), pIItem); 

	if(!in_slot)
	{
		TIItemContainer::iterator it = std::find(m_ruck.begin(), m_ruck.end(), pIItem); 
		if(m_ruck.end() != it) m_ruck.erase(it);
	}

	CalcTotalWeight();
	InvalidateState						();

	EItemPlace p = pIItem->m_eItemPlace;
	pIItem->m_eItemPlace = eItemPlaceBelt;
	m_pOwner->OnItemBelt(pIItem, p);
	pIItem->OnMoveToBelt();

	if(in_slot)
		pIItem->object().processing_deactivate();

	pIItem->object().processing_activate();

	return true;
}

bool CInventory::Ruck(PIItem pIItem) 
{
	if(!CanPutInRuck(pIItem)) return false;
	
	bool in_slot = InSlot(pIItem);
	//���� ���� � �����
	if(in_slot) 
	{
		if(m_iActiveSlot == pIItem->GetSlot()) Activate(NO_ACTIVE_SLOT);
		m_slots[pIItem->GetSlot()].m_pIItem = NULL;
	}
	else
	{
		//���� ���� �� ����� ��� ������ ������ ������� � �����
		TIItemContainer::iterator it = std::find(m_belt.begin(), m_belt.end(), pIItem); 
		if(m_belt.end() != it) m_belt.erase(it);
	}
	
	m_ruck.insert									(m_ruck.end(), pIItem); 
	
	CalcTotalWeight									();
	InvalidateState									();

	m_pOwner->OnItemRuck							(pIItem, pIItem->m_eItemPlace);
	pIItem->m_eItemPlace							= eItemPlaceRuck;
	pIItem->OnMoveToRuck							();

	if(in_slot)
		pIItem->object().processing_deactivate();

	return true;
}

void CInventory::Activate_deffered	(u32 slot, u32 _frame)
{
	 m_iLoadActiveSlot			= slot;
	 m_iLoadActiveSlotFrame		= _frame;
}

bool CInventory::Activate(u32 slot, bool force) 
{	
	if(Device.dwFrame == m_iLoadActiveSlotFrame) 
	{
		 if( (m_iLoadActiveSlot == slot) && m_slots[slot].m_pIItem )
			m_iLoadActiveSlotFrame = u32(-1);
		 else
			return false;

	}

	if( (slot!=NO_ACTIVE_SLOT && m_slots[slot].IsBlocked()) && !force)
		return false;

	R_ASSERT2(slot == NO_ACTIVE_SLOT || slot<m_slots.size(), "wrong slot number");

	if(slot != NO_ACTIVE_SLOT && !m_slots[slot].m_bVisible) 
		return false;
	
	if(m_iActiveSlot == slot && m_slots[m_iActiveSlot].m_pIItem){
		m_slots[m_iActiveSlot].m_pIItem->Activate();
	}

	if(m_iActiveSlot == slot || (m_iNextActiveSlot == slot &&
		m_iActiveSlot != NO_ACTIVE_SLOT &&
		m_slots[m_iActiveSlot].m_pIItem &&
		m_slots[m_iActiveSlot].m_pIItem->IsHiding()))
		return false;

	//�������� ���� �� ������
	if(m_iActiveSlot == NO_ACTIVE_SLOT)
	{
		if(m_slots[slot].m_pIItem)
		{
			m_iNextActiveSlot = slot;
			return true;
		}
		else 
			return false;
	}
	//�������� ���� ������������
	else if(slot == NO_ACTIVE_SLOT || m_slots[slot].m_pIItem)
	{
		if(m_slots[m_iActiveSlot].m_pIItem)
			m_slots[m_iActiveSlot].m_pIItem->Deactivate();
		m_iNextActiveSlot = slot;
	
		return true;
	}
	return false;
}


PIItem CInventory::ItemFromSlot(u32 slot) const
{
	VERIFY(NO_ACTIVE_SLOT != slot);
	return m_slots[slot].m_pIItem;
}

void CInventory::SendActionEvent(s32 cmd, u32 flags) 
{
	CActor *pActor = smart_cast<CActor*>(m_pOwner);
	if (!pActor) return;

	NET_Packet		P;
	pActor->u_EventGen		(P,GE_INV_ACTION, pActor->ID());
	P.w_s32					(cmd);
	P.w_u32					(flags);
	P.w_s32					(pActor->GetZoomRndSeed());
	P.w_s32					(pActor->GetShotRndSeed());
	pActor->u_EventSend		(P);
};

bool CInventory::Action(s32 cmd, u32 flags) 
{
	CActor *pActor = smart_cast<CActor*>(m_pOwner);
	
	if (pActor)
	{
		switch(cmd)
		{
			case kWPN_FIRE:
			{
				pActor->SetShotRndSeed();
			}break;
			case kWPN_ZOOM : 
			{
				pActor->SetZoomRndSeed();
			}break;
		};
	};

	if (g_pGameLevel && OnClient() && pActor) {
		switch(cmd)
		{
		case kUSE:
			{
			}break;
		case kWPN_RELOAD:
		case kDROP:
		case kWPN_FIRE:
		case kWPN_NEXT:
			{
				SendActionEvent(cmd, flags);
				return true;
			}break;
		case kWPN_FUNC:
		case kWPN_FIREMODE_NEXT:
		case kWPN_FIREMODE_PREV:
		
/*		case kWPN_1:
		case kWPN_2:
		case kWPN_3:
		case kWPN_4:
		case kWPN_5:
		case kWPN_6:
		case kARTEFACT:
*/
		case kWPN_ZOOM : 
		case kTORCH:
		case kNIGHT_VISION:
			{
				SendActionEvent(cmd, flags);
			}break;
		}
	}


	if (m_iActiveSlot < m_slots.size() && 
			m_slots[m_iActiveSlot].m_pIItem && 
			m_slots[m_iActiveSlot].m_pIItem->Action(cmd, flags)) 
											return true;
	bool b_send_event = false;
	switch(cmd) 
	{
	case kWPN_1:
	case kWPN_2:
	case kWPN_3:
	case kWPN_4:
	case kWPN_5:
	case kWPN_6:
       {
		   if (cmd == kWPN_6 && GameID() != GAME_SINGLE) return false;

			if(flags&CMD_START)
			{
                if((int)m_iActiveSlot == cmd - kWPN_1 &&
					m_slots[m_iActiveSlot].m_pIItem && GameID() == GAME_SINGLE)
				{
					b_send_event = Activate(NO_ACTIVE_SLOT);
				}else{ 					
					if ((int)m_iActiveSlot == cmd - kWPN_1 && GameID() != GAME_SINGLE)
						break;

					b_send_event = Activate(cmd - kWPN_1);
				}

//.				return true;
			}
		}break;
	case kARTEFACT:
		{
			if(flags&CMD_START)
			{
                if((int)m_iActiveSlot == ARTEFACT_SLOT &&
					m_slots[m_iActiveSlot].m_pIItem && GameID() == GAME_SINGLE)
				{
					b_send_event = Activate(NO_ACTIVE_SLOT);
				}else {
					b_send_event = Activate(ARTEFACT_SLOT);
				}
//.				return true;
			}
			else
			{
				int x=0;
				x=x;
			}
		}break;
	}

	if(b_send_event && g_pGameLevel && OnClient() && pActor)
			SendActionEvent(cmd, flags);

	return false;
}


void CInventory::Update() 
{
	bool bActiveSlotVisible;
	
	if(m_iActiveSlot == NO_ACTIVE_SLOT || 
		!m_slots[m_iActiveSlot].m_pIItem ||
        m_slots[m_iActiveSlot].m_pIItem->IsHidden())
	{ 
		bActiveSlotVisible = false;
	}
	else 
	{
		bActiveSlotVisible = true;
	}

	if(m_iNextActiveSlot != m_iActiveSlot && !bActiveSlotVisible)
	{
		if(m_iNextActiveSlot != NO_ACTIVE_SLOT &&
			m_slots[m_iNextActiveSlot].m_pIItem)
			m_slots[m_iNextActiveSlot].m_pIItem->Activate();

		m_iActiveSlot = m_iNextActiveSlot;
	}
	
	//��������� ������ � ����, ���� �� ����, ������� ����� ��������
	u32		drop_count = 0;
	for		(int i = 0; i < 2; ++i)	{
		TIItemContainer &list = i?m_ruck:m_belt;
		TIItemContainer::iterator it = list.begin();
	
		while(list.end() != it)
		{
			PIItem pIItem = *it;
//			Msg("-- %s [%d]", pIItem->Name(), pIItem->object().ID());
			R_ASSERT(pIItem);			
			if( pIItem->GetDrop() )
			{
				pIItem->SetDrop(FALSE);
				if(pIItem->object().H_Parent())	{
					NET_Packet P;
					pIItem->object().u_EventGen(P, GE_OWNERSHIP_REJECT, 
										  pIItem->object().H_Parent()->ID());
					P.w_u16(u16(pIItem->object().ID()));
					if (OnServer()) pIItem->object().u_EventSend(P);
				}
				else 
					m_drop_tasks.push_back(pIItem);
			}
			++it;
		}
	}

	//��������� �����
	for(i=0; i<(int)m_slots.size(); ++i)	{
		PIItem pIItem = m_slots[i].m_pIItem;

		if(pIItem && pIItem->GetDrop())	{
			pIItem->SetDrop		(FALSE);			 
			
			if(pIItem->object().H_Parent())
			{
				NET_Packet P;
				pIItem->object().u_EventGen(P, GE_OWNERSHIP_REJECT, 
									pIItem->object().H_Parent()->ID());
				P.w_u16(u16(pIItem->object().ID()));
				if (OnServer()) pIItem->object().u_EventSend(P);
			}
			else
				m_drop_tasks.push_back	(pIItem);
		}
	}
	while	(m_drop_tasks.size())	{
		drop_count			= Drop(&m_drop_tasks.back()->object()) ? drop_count + 1 : drop_count;
		m_drop_tasks.pop_back	();
	}

	if (m_drop_last_frame)
	{
		m_drop_last_frame = false;
		m_pOwner->OnItemDropUpdate	();
	}
}
//���� �� ����� ������� ������ ����
PIItem CInventory::Same(const PIItem pIItem, bool bSearchRuck) const
{
	const TIItemContainer &list = bSearchRuck ? m_ruck : m_belt;

	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		const PIItem l_pIItem = *it;
		
		if((l_pIItem != pIItem) && 
				!xr_strcmp(l_pIItem->object().cNameSect(), 
				pIItem->object().cNameSect())) 
			return l_pIItem;
	}
	return NULL;
}

//���� �� ����� ���� ��� ����� 
PIItem CInventory::SameSlot(const PIItem pIItem, bool bSearchRuck) const
{
	u32 slot = pIItem->GetSlot();
	if(slot == NO_ACTIVE_SLOT) 	return NULL;

	const TIItemContainer &list = bSearchRuck ? m_ruck : m_belt;
	
	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem _pIItem = *it;
		if(_pIItem != pIItem && _pIItem->GetSlot() == slot) return _pIItem;
	}

	return NULL;
}

//����� � ��������� ���� � ��������� ������
PIItem CInventory::Get(const char *name, bool bSearchRuck) const
{
	const TIItemContainer &list = bSearchRuck ? m_ruck : m_belt;
	
	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(!xr_strcmp(pIItem->object().cNameSect(), name) && 
								pIItem->Useful()) 
				return pIItem;
	}
	return NULL;
}

PIItem CInventory::Get(CLASS_ID cls_id, bool bSearchRuck) const
{
	const TIItemContainer &list = bSearchRuck ? m_ruck : m_belt;
	
	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->object().CLS_ID == cls_id && 
								pIItem->Useful()) 
				return pIItem;
	}
	return NULL;
}

PIItem CInventory::Get(const u16 id, bool bSearchRuck) const
{
	const TIItemContainer &list = bSearchRuck ? m_ruck : m_belt;

	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->object().ID() == id) 
			return pIItem;
	}
	return NULL;
}

//search both (ruck and belt)
PIItem CInventory::GetAny(const char *name) const
{
	PIItem itm = Get(name, false);
	if(!itm)
		itm = Get(name, true);
	return itm;
}

PIItem CInventory::item(CLASS_ID cls_id) const
{
	const TIItemContainer &list = m_all;

	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->object().CLS_ID == cls_id && 
			pIItem->Useful()) 
			return pIItem;
	}
	return NULL;
}

float CInventory::TotalWeight() const
{
	VERIFY(m_fTotalWeight>=0.f);
	return m_fTotalWeight;
}


float CInventory::CalcTotalWeight()
{
	float weight = 0;
	for(TIItemContainer::const_iterator it = m_all.begin(); m_all.end() != it; ++it) 
		weight += (*it)->Weight();

	m_fTotalWeight = weight;
	return m_fTotalWeight;
}


u32 CInventory::dwfGetSameItemCount(LPCSTR caSection, bool SearchAll)
{
	u32			l_dwCount = 0;
	TIItemContainer	&l_list = SearchAll ? m_all : m_ruck;
	for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (!xr_strcmp(l_pIItem->object().cNameSect(), caSection))
            ++l_dwCount;
	}
	
	return		(l_dwCount);
}
u32		CInventory::dwfGetGrenadeCount(LPCSTR caSection, bool SearchAll)
{
	u32			l_dwCount = 0;
	TIItemContainer	&l_list = SearchAll ? m_all : m_ruck;
	for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (l_pIItem->object().CLS_ID == CLSID_GRENADE_F1 || l_pIItem->object().CLS_ID == CLSID_GRENADE_RGD5)
			++l_dwCount;
	}

	return		(l_dwCount);
}

bool CInventory::bfCheckForObject(ALife::_OBJECT_ID tObjectID)
{
	TIItemContainer	&l_list = m_all;
	for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (l_pIItem->object().ID() == tObjectID)
			return(true);
	}
	return		(false);
}

CInventoryItem *CInventory::get_object_by_id(ALife::_OBJECT_ID tObjectID)
{
	TIItemContainer	&l_list = m_all;
	for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (l_pIItem->object().ID() == tObjectID)
			return	(l_pIItem);
	}
	return		(0);
}

//������� ������� 
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
bool CInventory::Eat(PIItem pIItem)
{
	//����������� �������� �� ����
	CEatableItem* pItemToEat = smart_cast<CEatableItem*>(pIItem);
	if(!pItemToEat) return false;

	CEntityAlive *entity_alive = smart_cast<CEntityAlive*>(m_pOwner);
	if(!entity_alive) return false;
	
	pItemToEat->UseBy		(entity_alive);

	if(IsGameTypeSingle() && Actor()->m_inventory == this)
		Actor()->callback(GameObject::eUseObject)((smart_cast<CGameObject*>(pIItem))->lua_game_object());

	if(pItemToEat->Empty() )
	{
		pIItem->Drop();
		return false;
	}
	return true;
}

bool CInventory::InSlot(PIItem pIItem) const
{
	if(pIItem->GetSlot() < m_slots.size() && 
		m_slots[pIItem->GetSlot()].m_pIItem == pIItem)
		return true;
	return false;
}
bool CInventory::InBelt(PIItem pIItem) const
{
	if(Get(pIItem->object().ID(), false)) return true;
	return false;
}
bool CInventory::InRuck(PIItem pIItem) const
{
	if(Get(pIItem->object().ID(), true)) return true;
	return false;
}


bool CInventory::CanPutInSlot(PIItem pIItem) const
{
	if(!m_bSlotsUseful) return false;

	if( !GetOwner()->CanPutInSlot(pIItem, pIItem->GetSlot() ) ) return false;

	if(pIItem->GetSlot() < m_slots.size() && 
		m_slots[pIItem->GetSlot()].m_pIItem == NULL )
		return true;
	
	return false;
}
//��������� ����� �� ��������� ���� �� ����,
//��� ���� ������� ������ �� ��������
bool CInventory::CanPutInBelt(PIItem pIItem)
{
	if(InBelt(pIItem))					return false;
	if(!m_bBeltUseful)					return false;
	if(!pIItem || !pIItem->Belt())		return false;
	if(m_belt.size() == BeltWidth())	return false;

	return FreeRoom_inBelt(m_belt, pIItem, BeltWidth(), 1);
}
//��������� ����� �� ��������� ���� � ������,
//��� ���� ������� ������ �� ��������
bool CInventory::CanPutInRuck(PIItem pIItem) const
{
	if(InRuck(pIItem)) return false;
	return true;
}

u32	CInventory::dwfGetObjectCount()
{
	return		(m_all.size());
}

CInventoryItem	*CInventory::tpfGetObjectByIndex(int iIndex)
{
	if ((iIndex >= 0) && (iIndex < (int)m_all.size())) {
		TIItemContainer	&l_list = m_all;
		int			i = 0;
		for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it, ++i) 
			if (i == iIndex)
                return	(*l_it);
	}
	else {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"invalid inventory index!");
		return	(0);
	}
	R_ASSERT	(false);
	return		(0);
}

CInventoryItem	*CInventory::GetItemFromInventory(LPCSTR caItemName)
{
	TIItemContainer	&l_list = m_all;

	u32 crc = crc32(caItemName, xr_strlen(caItemName));

	for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
		if ((*l_it)->object().cNameSect()._get()->dwCRC == crc){
			VERIFY(	0 == xr_strcmp( (*l_it)->object().cNameSect().c_str(), caItemName)  );
			return	(*l_it);
		}
	return	(0);
}


bool CInventory::CanTakeItem(CInventoryItem *inventory_item) const
{
	VERIFY			(inventory_item);
	VERIFY			(m_pOwner);

	if (inventory_item->object().getDestroy()) return false;

	if(!inventory_item->CanTake()) return false;

	for(TIItemContainer::const_iterator it = m_all.begin(); it != m_all.end(); it++)
		if((*it)->object().ID() == inventory_item->object().ID()) break;
	VERIFY3(it == m_all.end(), "item already exists in inventory",*inventory_item->object().cName());

	CActor* pActor = smart_cast<CActor*>(m_pOwner);
	//����� ������ ����� ����� ����
	if(!pActor && (TotalWeight() + inventory_item->Weight() > m_pOwner->MaxCarryWeight()))
		return	false;

	return	true;
}


u32  CInventory::BeltWidth() const
{
	return m_iMaxBelt;
}

void  CInventory::AddAvailableItems(TIItemContainer& items_container, bool for_trade) const
{
	for(TIItemContainer::const_iterator it = m_ruck.begin(); m_ruck.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(!for_trade || pIItem->CanTrade())
			items_container.push_back(pIItem);
	}

	if(m_bBeltUseful)
	{
		for(TIItemContainer::const_iterator it = m_belt.begin(); m_belt.end() != it; ++it) 
		{
			PIItem pIItem = *it;
			if(!for_trade || pIItem->CanTrade())
				items_container.push_back(pIItem);
		}
	}
	
	if(m_bSlotsUseful)
	{
		TISlotArr::const_iterator slot_it			= m_slots.begin();
		TISlotArr::const_iterator slot_it_e			= m_slots.end();
		for(;slot_it!=slot_it_e;++slot_it)
		{
			const CInventorySlot& S = *slot_it;
			if(S.m_pIItem && (!for_trade || S.m_pIItem->CanTrade()) && !S.m_bPersistent)
				items_container.push_back(S.m_pIItem);
		}
/*
		if(m_slots[KNIFE_SLOT].m_pIItem)
			if(!for_trade || m_slots[KNIFE_SLOT].m_pIItem->CanTrade())
				items_container.push_back(m_slots[KNIFE_SLOT].m_pIItem);

		if(m_slots[PISTOL_SLOT].m_pIItem)
			if(!for_trade || m_slots[PISTOL_SLOT].m_pIItem->CanTrade())
				items_container.push_back(m_slots[PISTOL_SLOT].m_pIItem);

		if(m_slots[RIFLE_SLOT].m_pIItem)
			if(!for_trade || m_slots[RIFLE_SLOT].m_pIItem->CanTrade())
				items_container.push_back(m_slots[RIFLE_SLOT].m_pIItem);

		if(m_slots[GRENADE_SLOT].m_pIItem)
			if(!for_trade || m_slots[GRENADE_SLOT].m_pIItem->CanTrade())
				items_container.push_back(m_slots[GRENADE_SLOT].m_pIItem);

		if(m_slots[APPARATUS_SLOT].m_pIItem)
			if(!for_trade || m_slots[APPARATUS_SLOT].m_pIItem->CanTrade())
				items_container.push_back(m_slots[APPARATUS_SLOT].m_pIItem);

		if(m_slots[OUTFIT_SLOT].m_pIItem)
			if(!for_trade || m_slots[OUTFIT_SLOT].m_pIItem->CanTrade())
				items_container.push_back(m_slots[OUTFIT_SLOT].m_pIItem);
*/
	}		
}

bool CInventory::isBeautifulForActiveSlot	(CInventoryItem *pIItem)
{
	if (!IsGameTypeSingle()) return (true);
	TISlotArr::iterator it =  m_slots.begin();
	for( ; it!=m_slots.end(); ++it) {
		if ((*it).m_pIItem && (*it).m_pIItem->IsNecessaryItem(pIItem))
			return		(true);
	}
	return				(false);
}

#include "WeaponHUD.h"
void CInventory::Items_SetCurrentEntityHud(bool current_entity)
{
	TIItemContainer::iterator it;
	for(it = m_all.begin(); m_all.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		CHudItem* pHudItem = smart_cast<CHudItem*> (pIItem);
		if (pHudItem) 
		{
			pHudItem->GetHUD()->Visible(current_entity);
		};
		CWeapon* pWeapon = smart_cast<CWeapon*>(pIItem);
		if (pWeapon)
		{
			pWeapon->InitAddons();
			pWeapon->UpdateAddonsVisibility();
		}
	}
};

//call this only via Actor()->SetWeaponHideState()
void CInventory::SetSlotsBlocked(u16 mask, bool bBlock)
{
	bool bChanged = false;
	for(int i =0; i<SLOTS_TOTAL; ++i)
	{
		if(mask & (1<<i))
		{
			bool bCanBeActivated = m_slots[i].CanBeActivated();
			if(bBlock){
				++m_slots[i].m_blockCounter;
				VERIFY2(m_slots[i].m_blockCounter< 5,"block slots overflow");
			}else{
				--m_slots[i].m_blockCounter;
				VERIFY2(m_slots[i].m_blockCounter>-5,"block slots underflow");
			}
			if(bCanBeActivated != m_slots[i].CanBeActivated())
				bChanged = true;
		}
	}
	if(bChanged)
	{
		u32 ActiveSlot		= GetActiveSlot();
		u32 PrevActiveSlot	= GetPrevActiveSlot();
		if(ActiveSlot==NO_ACTIVE_SLOT)
		{//try to restore hidden weapon
			if(PrevActiveSlot!=NO_ACTIVE_SLOT && m_slots[PrevActiveSlot].CanBeActivated()) 
				if(Activate(PrevActiveSlot))
					SetPrevActiveSlot(NO_ACTIVE_SLOT);
		}else
		{//try to hide active weapon
			if(!m_slots[ActiveSlot].CanBeActivated() )
				if(Activate(NO_ACTIVE_SLOT))
					SetPrevActiveSlot(ActiveSlot);
		}
	}
}
