// CUIMapWnd.cpp:  ������ ������������ �����
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMapWnd.h"

#include "../HUDManager.h"

#include "../actor.h"

#include "../pda.h"
#include "../infoportion.h"
#include "UIPdaWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../level.h"

#include "UIInventoryUtilities.h"
#include "../string_table.h"

using namespace InventoryUtilities;

#define MAP_LEFT		89
#define MAP_TOP			93
#define MAP_RADIUS		80

//�������� ���������� ����� �� LevelEditor
//x1 - ����, x2 - �����, 
//z1 - ��� , z2 - ����

const char * const	PDA_MAP_XML					= "map.xml";
const				SCROLLBARS_SHIFT			= 5;
const				VSCROLLBAR_STEP				= 20; // � ��������
const				HSCROLLBAR_STEP				= 20; // � ��������

// Global map stuff
const char * const	GLOBAL_MAP_LTX				= "global_map";
const char * const	GLOBAL_MAP_TEX_LTX			= "texture";
const char * const	GLOBAL_MAP_W_LTX			= "width";
const char * const	GLOBAL_MAP_H_LTX			= "height";
const char * const	GLOBAL_MAP_LOCATIONS_LTX	= "global_map_locations";

const u32			activeLocalMapColor			= 0xff00ff00;
const u32			inactiveLocalMapColor		= 0xff0000ff;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMapWnd::CUIMapWnd()
	:	m_eCurrentMode	(emmLocal),
		m_pCurrentMap	(&UILocalMapBackground)
{
	Show(false);
	
	SetFont(HUD().pFontSmall);

//	Init();
}

CUIMapWnd::~CUIMapWnd()
{
	UILocalMapBackground.RemoveAllSpots();
	UIGlobalMapBackground.RemoveAllSpots();
}

void CUIMapWnd::Show(bool status)
{
	if (status)
		InitMaps();
	inherited::Show(status);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::Init()
{
	CUIXmlInit xml_init;
	CUIXml uiXml;
	uiXml.Init("$game_data$", PDA_MAP_XML);

	inherited::Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	AttachChild(&UIMainMapFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIMainMapFrame);
	UIMainMapFrame.AttachChild(&UIPDAHeader);
	xml_init.InitFrameLine(uiXml, "map_header_frame_line", 0, &UIPDAHeader);

	string128 buf;
	strconcat(buf, ALL_PDA_HEADER_PREFIX, "/Map");

	UIPDAHeader.UITitleText.SetText(buf);

	//���������� � ����� �� ����� �� ������� ������� �����
	AttachChild(&UIStaticInfo);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticInfo);

	UIStaticInfo.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0, UIStaticInfo.GetWidth(), UIStaticInfo.GetHeight(), "map_character.xml");
	UIStaticInfo.Enable(false);
	
	UIMainMapFrame.AttachChild(&UILocalMapBackground);
	xml_init.InitWindow(uiXml, "window", 0, &UILocalMapBackground);
	UILocalMapBackground.SetMessageTarget(this);
	UILocalMapBackground.Enable(true);

	UIMainMapFrame.AttachChild(&UIGlobalMapBackground);
	xml_init.InitWindow(uiXml, "window", 0, &UIGlobalMapBackground);
	UIGlobalMapBackground.SetMessageTarget(this);
	UIGlobalMapBackground.Enable(true);
	UIMainMapFrame.DetachChild(&UIGlobalMapBackground);

	// Descriptions
	UIGlobalMapBackground.AttachChild(&UIMapName);
	xml_init.InitStatic(uiXml, "map_name_static", 0, &UIMapName);
	UIMapName.Enable(false);
	UIMapName.Show(false);

	UIGlobalMapBackground.AttachChild(&UIMapGoals);
	xml_init.InitListWnd(uiXml, "map_goals_list", 0, &UIMapGoals);
	UIMapGoals.Enable(false);
	UIMapGoals.Show(false);

	RECT r = UILocalMapBackground.GetWndRect();

	UIMainMapFrame.AttachChild(&UIMapBgndV);
	UIMapBgndV.Init(r.right + SCROLLBARS_SHIFT, r.top, r.bottom - r.top, false);
	UIMapBgndV.SetMessageTarget(this);

	UIMainMapFrame.AttachChild(&UIMapBgndH);
	UIMapBgndH.Init(r.left, r.bottom + SCROLLBARS_SHIFT, r.right - r.left, true);
	UIMapBgndH.SetMessageTarget(this);

	// ������ ������������ ����������/��������� �����
	UIMainMapFrame.AttachChild(&UIMapTypeSwitch);
	xml_init.InitButton(uiXml, "gl_button", 0, &UIMapTypeSwitch);
	UIMapTypeSwitch.SetMessageTarget(this);

	//�������� ��������������� ����������
	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	m_sGlobalMapLocFrameName = uiXml.Read("map_location_frame", 0, "");

	InitMaps();
}

void CUIMapWnd::InitLocalMap()
{
	UILocalMapBackground.m_pActiveMapSpot = NULL;

	if (Level().pLevel->section_exist("level_map"))	
	{
		UILocalMapBackground.m_LevelBox.x1 = Level().pLevel->r_float("level_map","x1");
		UILocalMapBackground.m_LevelBox.z1 = Level().pLevel->r_float("level_map","z1");
		UILocalMapBackground.m_LevelBox.x2 = Level().pLevel->r_float("level_map","x2");
		UILocalMapBackground.m_LevelBox.z2 = Level().pLevel->r_float("level_map","z2");
		UILocalMapBackground.m_MapTextureName = Level().pLevel->r_string("level_map","texture");
	}
	else
	{
		//� ���������� ����� ������ ���� �����-�� ������ ������
		UILocalMapBackground.m_LevelBox.x1 = -443.66000f;
		UILocalMapBackground.m_LevelBox.z1 = -131.66000f;
		UILocalMapBackground.m_LevelBox.x2 = 196.33000f;
		UILocalMapBackground.m_LevelBox.z2 = 508.32999f;
	}

	UILocalMapBackground.InitMapBackground();

	///������ ��� ������������� ������� �� ����� 
	UILocalMapBackground.RemoveAllSpots();

	Ivector2 P;
	Fvector src;
	
	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
		
	if (!pActor) return;

	int left = UILocalMapBackground.GetWndRect().left;
	int top = UILocalMapBackground.GetWndRect().top;

	////////////////////////////////////////////
	//�������� �������, ��������� � ������ PDA
	////////////////////////////////////////////
	
	for(LOCATIONS_PTR_VECTOR_IT it = Level().MapLocations().begin();
		Level().MapLocations().end() != it; 
		it++)
	{
		CUIMapSpot * map_spot;
		map_spot = xr_new<CUIMapSpot>();
		SMapLocation& map_location = *(*it);

		if(map_location.attached_to_object)
		{
			map_spot->SetObjectID(map_location.object_id);
			src = map_spot->MapPos();
		}
		else
		{
			src.x = map_location.x;
			src.y = 0;
			src.z = map_location.y;

			map_spot->SetObjectID(0xffff);
			map_spot->m_vWorldPos.set(map_location.x,0.f,map_location.y);
		}

		map_spot->arrow_color = map_location.icon_color;
		map_spot->m_bArrowEnabled = map_location.marker_show;

		UILocalMapBackground.ConvertToLocal(src,P);

		map_spot->SetShader(GetMapIconsShader());
		map_spot->MoveWindow(P.x + left, P.y + top);
		map_spot->SetWidth(MAP_ICON_WIDTH*map_location.icon_width);
		map_spot->SetHeight(MAP_ICON_HEIGHT*map_location.icon_height);
		map_spot->GetUIStaticItem().SetOriginalRect(
			map_location.icon_x*MAP_ICON_GRID_WIDTH,
			map_location.icon_y*MAP_ICON_GRID_HEIGHT,
			map_location.icon_width*MAP_ICON_GRID_WIDTH + MAP_ICON_WIDTH,
			map_location.icon_height*MAP_ICON_GRID_HEIGHT + MAP_ICON_HEIGHT);

		map_spot->m_eAlign = CUIMapSpot::eCenter;
		map_spot->m_sDescText.SetText("");
		map_spot->m_sNameText.SetText("");
		map_spot->m_bArrowEnabled = true;

		if(xr_strlen(map_location.text)>1)
		{
			map_spot->m_sDescText.SetText(*map_location.text);
			map_spot->m_sNameText.SetText(*map_location.name);
		}

		UILocalMapBackground.m_vMapSpots.push_back(map_spot);
		UILocalMapBackground.AttachChild(map_spot);
	}

	////////////////////////////////
	//�������� �� ����� ������
	////////////////////////////////

	UILocalMapBackground.ConvertToLocal(pActor->Position(),P);
	CUIMapSpot* map_spot;

	map_spot = xr_new<CUIMapSpot>();
	map_spot->SetObjectID(pActor->ID());

	// viewport
	float h,p;
	Device.vCameraDirection.getHP	(h,p);
	map_spot->m_fHeading = h;
	map_spot->m_bHeading = true;

	map_spot->InitTexture(pSettings->r_string("game_map", "actor_arrow"));
	map_spot->SetWidth(MAP_ICON_WIDTH);
	map_spot->SetHeight(MAP_ICON_HEIGHT);

	map_spot->MoveWindow(P.x + left, P.y + top);
	map_spot->m_eAlign = CUIMapSpot::eCenter;
	map_spot->m_sDescText.SetText("It's you.");
	map_spot->m_sNameText.SetText("You");
//	map_spot->Show(true);

	UILocalMapBackground.m_vMapSpots.push_back(map_spot);
	UILocalMapBackground.AttachChild(map_spot);
	m_pActorSpot = map_spot;

	//!!! ��������������� ������ ���������� �����
	m_pActorSpot->GetStaticItem()->Render();

	//���������� � ��������� �������
	UIStaticInfo.Show(false);
	UICharacterInfo.Show(false);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::InitGlobalMap()
{
	// �������������� ��������� ����� �� ltx �����
	UIGlobalMapBackground.m_LevelBox.x1		= 0.0f;
	UIGlobalMapBackground.m_LevelBox.z1		= 0.0f;
	UIGlobalMapBackground.m_LevelBox.x2		= pSettings->r_float(GLOBAL_MAP_LTX, GLOBAL_MAP_W_LTX);
	UIGlobalMapBackground.m_LevelBox.z2		= pSettings->r_float(GLOBAL_MAP_LTX, GLOBAL_MAP_H_LTX);
	UIGlobalMapBackground.m_MapTextureName	= pSettings->r_string(GLOBAL_MAP_LTX, GLOBAL_MAP_TEX_LTX);

	UIGlobalMapBackground.InitMapBackground();

	// �������������� ���������
	InitGlobalMapLocationObjectives();

	// �������������� ��������� ��������� ����
	int globalMapLocations = pSettings->line_count(GLOBAL_MAP_LOCATIONS_LTX);
	for (int i = 0; i < globalMapLocations; ++i)
	{
		LPCSTR name, value;
		pSettings->r_line(GLOBAL_MAP_LOCATIONS_LTX, i, &name, &value);

		//. ???
		/*
#ifndef DEBUG
		//�������� �� ������������� ������ � ������� ������
		if(ai().get_alife() && ai().get_game_graph())
			ai().game_graph().header().level(name);
#endif
		*/

		// ���� ��� ����� ��� ���� �������, �� �������������� ��.
		// ���� ���, �� ������� �� ������ � ���������
		MapLocations_it it = m_MapLocations.find(name);
		if(it != m_MapLocations.end())
		{
			Ivector4 v = pSettings->r_ivector4(GLOBAL_MAP_LOCATIONS_LTX, name);
			AddGlobalMapLocation(name, v);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::InitGlobalMapLocationObjectives()
{
	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

	if (!pActor) return;

	for (MapLocations_it it = m_MapLocations.begin(); it != m_MapLocations.end(); ++it)
	{
		it->second.obj.clear();
	}

	//--------------------------------------------------------------
	//�������� ������� �� �����, ������� ����� ������ � info_portions
	if(pActor->known_info_registry.objects_ptr())
	{
		const KNOWN_INFO_VECTOR& know_info = *pActor->known_info_registry.objects_ptr();
		for(KNOWN_INFO_VECTOR::const_iterator it = know_info.begin();
			know_info.end() != it; it++)
		{
			CInfoPortion info_portion;
			info_portion.Load((*it).id);
			//�������� ������� �� �����
			for(u32 i=0; i<info_portion.MapLocations().size(); i++)
			{
				const SMapLocation& map_location = info_portion.MapLocations()[i];
				if(xr_strlen(*map_location.level_name) > 0)
				{
					m_MapLocations[map_location.level_name].obj.push_back(map_location.text);
				}
			}

		}
	}}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::InitMaps()
{
	InitGlobalMap();
	InitLocalMap();
	
	m_eCurrentMode = emmGlobal;
	SwitchMapMode(emmLocal);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::AddObjectSpot(CGameObject* pGameObject)
{	
	//!!! ���� ������ ��� CEntity (���� ���� ������ ����� ������)
	CEntity* pEntity = smart_cast<CEntity*>(pGameObject);
	if(!pEntity) return;


	int left = UILocalMapBackground.GetWndRect().left;
	int top = UILocalMapBackground.GetWndRect().top;

	Ivector2 P;
    UILocalMapBackground.ConvertToLocal(pGameObject->Position(),P);
	
	CUIMapSpot* map_spot;
	map_spot = xr_new<CUIMapSpot>();
	map_spot->SetObjectID(pEntity->ID());
	
	map_spot->SetShader(GetCharIconsShader());
	map_spot->SetWidth(MAP_ICON_WIDTH);
	map_spot->SetHeight(MAP_ICON_HEIGHT);
	map_spot->MoveWindow(P.x + left, P.y + top);
	map_spot->m_eAlign = CUIMapSpot::eCenter;
	map_spot->m_sDescText.SetText("");
	map_spot->m_sNameText.SetText("");

	map_spot->GetUIStaticItem().SetOriginalRect(
						pEntity->GetMapIconX()*ICON_GRID_WIDTH + MAP_ICON_WIDTH / 2,
						pEntity->GetMapIconY()*ICON_GRID_HEIGHT + MAP_ICON_HEIGHT,
						(pEntity->GetMapIconX()+1)*ICON_GRID_WIDTH,
						(pEntity->GetMapIconY()+1)*ICON_GRID_HEIGHT);

	UILocalMapBackground.m_vMapSpots.push_back(map_spot);
	UILocalMapBackground.AttachChild(map_spot);
}

void CUIMapWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	static u16 prevScrollPosV = 0;
	static u16 prevScrollPosH = 0;

	if(pWnd == &UILocalMapBackground)
	{
		if(MAPSPOT_FOCUS_RECEIVED == msg)
		{
			if(m_pCurrentMap->m_pActiveMapSpot)
			{
				UIStaticInfo.Show(true);
				
				//CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(m_pCurrentMap->m_pActiveMapSpot->m_pObject);

				if(xr_strlen(m_pCurrentMap->m_pActiveMapSpot->m_sDescText.GetBuf())>1)
				{
					UICharacterInfo.Show(true);
					UICharacterInfo.ResetAllStrings();

					UICharacterInfo.UIName.SetText(m_pCurrentMap->m_pActiveMapSpot->m_sNameText);
					UICharacterInfo.UIRank.SetText(m_pCurrentMap->m_pActiveMapSpot->m_sDescText);
					
				}
				else
				{
					UICharacterInfo.ResetAllStrings();
/*					if(pInvOwner)
					{
						UICharacterInfo.InitCharacter(pInvOwner);
						UICharacterInfo.Show(true);
					}
					else*/
					UICharacterInfo.Show(false);
				}
            }
		}
		else if(MAPSPOT_FOCUS_LOST == msg)
		{
			UIStaticInfo.Show(false);
		}
	}

	if (&UIGlobalMapBackground == pWnd)
	{
		static CUIMapSpot *prevSpot = NULL;
		if (MAPSPOT_FOCUS_RECEIVED == msg)
		{
			if (UIGlobalMapBackground.m_pActiveMapSpot && prevSpot != UIGlobalMapBackground.m_pActiveMapSpot)
			{
				CUIGlobalMapLocation *pGML = smart_cast<CUIGlobalMapLocation*>(UIGlobalMapBackground.m_pActiveMapSpot);
				R_ASSERT(pGML);
				pGML->SetColor(activeLocalMapColor);
				UIMapName.SetText(*pGML->m_strMapName);
				// Show map goals
				for (Objectives_it it = m_MapLocations[pGML->m_strMapName].obj.begin(); it != m_MapLocations[pGML->m_strMapName].obj.end(); ++it)
				{
					UIMapGoals.AddItem<CUIListItem>(static_cast<const char *>(*(*it)));
				}
				prevSpot = UIGlobalMapBackground.m_pActiveMapSpot;
			}
			UIMapGoals.Show(true);
			UIMapName.Show(true);
		}

		if (MAPSPOT_FOCUS_LOST == msg)
		{
			UIMapGoals.Show(false);
			UIMapGoals.RemoveAll();
			prevSpot = NULL;
			if (UIGlobalMapBackground.m_pActiveMapSpot)
			{
				CUIGlobalMapLocation *pGML = smart_cast<CUIGlobalMapLocation*>(UIGlobalMapBackground.m_pActiveMapSpot);
				R_ASSERT(pGML);
				pGML->SetColor(inactiveLocalMapColor);
			}
			UIMapName.Show(false);
		}
	}

	if (&UIMapBgndV == pWnd)
	{
		if (SCROLLBAR_VSCROLL == msg)
		{
			m_pCurrentMap->MoveMap(0, -VSCROLLBAR_STEP * (UIMapBgndV.GetScrollPos() - prevScrollPosV));
			prevScrollPosV = UIMapBgndV.GetScrollPos();
			m_pCurrentMap->Update();
		}
	}

	if (&UIMapBgndH == pWnd)
	{
		if (SCROLLBAR_HSCROLL == msg)
		{
			m_pCurrentMap->MoveMap(-HSCROLLBAR_STEP * (UIMapBgndH.GetScrollPos() - prevScrollPosH), 0);
			prevScrollPosH = UIMapBgndH.GetScrollPos();
			m_pCurrentMap->Update();
		}
	}

	if (m_pCurrentMap == pWnd && MAP_MOVED == msg)
	{
		Fvector2	dest;
		m_pCurrentMap->ConvertFromLocalToMap(HSCROLLBAR_STEP, VSCROLLBAR_STEP, dest);
		if (UIMapBgndV.IsShown())
			UIMapBgndV.SetScrollPos(static_cast<s16>(UIMapBgndV.GetMaxRange() - iCeil((m_pCurrentMap->m_fMapHeightMeters - m_pCurrentMap->m_fMapViewHeightMeters - m_pCurrentMap->m_fMapY) / dest.y)));
		if (UIMapBgndH.IsShown())
			UIMapBgndH.SetScrollPos(static_cast<s16>(UIMapBgndH.GetMaxRange() - iCeil((m_pCurrentMap->m_fMapWidthMeters - m_pCurrentMap->m_fMapViewWidthMeters - m_pCurrentMap->m_fMapX) / dest.x)));

		prevScrollPosH = UIMapBgndH.GetScrollPos();
		prevScrollPosV = UIMapBgndV.GetScrollPos();
		m_pCurrentMap->Update();
	}
	
	if (&UIMapTypeSwitch == pWnd && BUTTON_CLICKED == msg)
	{
		if (emmGlobal == m_eCurrentMode)
			SwitchMapMode(emmLocal);
		else
			SwitchMapMode(emmGlobal);
	}

	inherited::SendMessage(pWnd, msg, pData);
}


void CUIMapWnd::Update()
{
	float h,p;
	Device.vCameraDirection.getHP	(h,p);
	m_pActorSpot->m_fHeading = h;

	inherited::Update();
}

void CUIMapWnd::Draw()
{
	inherited::Draw();
}

//������� �� ������� ������� ��������� � ���������� �����
//� ������� ��������� � ����� ������� ����
void CUIMapWnd::ConvertToLocal(const Fvector& src, Ivector2& dest)
{
	dest.x = iFloor(0.5f + m_iMapWidth*(src.x - m_fWorldMapLeft)/m_fWorldMapWidth);
	dest.y = m_iMapHeight - iFloor(0.5f + m_iMapHeight*(src.z - m_fWorldMapTop)/m_fWorldMapHeight);
}

void CUIMapWnd::SetActivePoint(const Fvector &vNewPoint)
{
	UILocalMapBackground.SetActivePos(vNewPoint);
	UILocalMapBackground.m_bNoActorFocus = true;
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::SwitchMapMode(const EMapModes mode)
{
	// ��������� �� ������������ � ������� �����
	if (mode == m_eCurrentMode) return;

	m_eCurrentMode = mode;

	CStringTable st;

	UIMainMapFrame.DetachChild(m_pCurrentMap);

	switch (m_eCurrentMode)
	{
	case emmLocal:
		UIMapTypeSwitch.SetText(*st("Global Map"));
		m_pCurrentMap = &UILocalMapBackground;
		UILocalMapBackground.Show(true);
		break;
	case emmGlobal:
		UIMapTypeSwitch.SetText(*st("Local Map"));
		m_pCurrentMap = &UIGlobalMapBackground;
		UIGlobalMapBackground.Show(true);
		break;
	default:
		R_ASSERT(!"Unknown map mode");
	}    

	UIMainMapFrame.AttachChild(m_pCurrentMap);

	m_fWorldMapWidth = m_pCurrentMap->m_LevelBox.x2 - m_pCurrentMap->m_LevelBox.x1;
	m_fWorldMapHeight = m_pCurrentMap->m_LevelBox.z2 - m_pCurrentMap->m_LevelBox.z1;

	m_fWorldMapLeft = m_pCurrentMap->m_LevelBox.x1;
	m_fWorldMapTop = m_pCurrentMap->m_LevelBox.z2;

	Fvector2	dest;
	m_pCurrentMap->ConvertFromLocalToMap(HSCROLLBAR_STEP, VSCROLLBAR_STEP, dest);

	// ����������� �����������
	if (m_pCurrentMap->m_fMapHeightMeters > m_pCurrentMap->m_fMapViewHeightMeters)
	{
		UIMapBgndV.Show(true);
		UIMapBgndV.SetRange(0, static_cast<s16>(iCeil((m_pCurrentMap->m_fMapHeightMeters - m_pCurrentMap->m_fMapViewHeightMeters) / dest.y)));
	}
	else
		UIMapBgndV.Show(false);

	if (m_pCurrentMap->m_fMapWidthMeters > m_pCurrentMap->m_fMapViewWidthMeters)
	{
		UIMapBgndH.Show(true);
		UIMapBgndH.SetRange(0, static_cast<s16>(iCeil((m_pCurrentMap->m_fMapWidthMeters - m_pCurrentMap->m_fMapViewWidthMeters) / dest.x)));
	}
	else
		UIMapBgndH.Show(false);

	SendMessage(m_pCurrentMap, MAP_MOVED, NULL);

	m_iMapWidth = m_pCurrentMap->GetWidth();
	m_iMapHeight = m_pCurrentMap->GetHeight();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapWnd::AddGlobalMapLocation(const ref_str mapName, const Ivector4 &v)
{
	R_ASSERT(v[2] > 0);
	R_ASSERT(v[3] > 0);

	CUIGlobalMapLocation *pGML = xr_new<CUIGlobalMapLocation>();
	pGML->Init(v[2], v[3], UIGlobalMapBackground.GetAbsoluteRect(), *m_sGlobalMapLocFrameName);
	pGML->m_vWorldPos.x = static_cast<float>(v[0]);
	pGML->m_vWorldPos.z = static_cast<float>(v[1]);
	pGML->SetColor(inactiveLocalMapColor);
	pGML->m_strMapName = mapName;
	m_MapLocations[mapName].l = pGML;
	UIGlobalMapBackground.m_vMapSpots.push_back(pGML);
	UIGlobalMapBackground.AttachChild(pGML);
}