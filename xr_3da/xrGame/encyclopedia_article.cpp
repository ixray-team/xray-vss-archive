///////////////////////////////////////////////////////////////
// encyclopedia_article.cpp
// ���������, �������� � ����������� ������ � ������������
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "encyclopedia_article.h"
#include "ui/xrXMLParser.h"
#include "ui/UIXmlInit.h"


CEncyclopediaArticle::CEncyclopediaArticle()
{
}

CEncyclopediaArticle::~CEncyclopediaArticle()
{
}

void CEncyclopediaArticle::Load	(ARTICLE_STR_ID str_id)
{
	Load	(id_to_index::IdToIndex(str_id));
}

void CEncyclopediaArticle::Load	(ARTICLE_INDEX  index)
{
	m_ArticleIndex = index;
	inherited_shared::load_shared(m_ArticleIndex, NULL);
}


void CEncyclopediaArticle::load_shared	(LPCSTR)
{
	const id_to_index::ITEM_DATA& item_data = id_to_index::GetByIndex(m_ArticleIndex);

	CUIXml uiXml;
	CUIXmlInit xml_init;
	string128 xml_file_full;
	strconcat(xml_file_full, *ref_str(item_data.file_name), ".xml");

	bool xml_result = uiXml.Init("$game_data$", xml_file_full);
	R_ASSERT3(xml_result, "xml file not found", xml_file_full);

	//loading from XML
	XML_NODE* pNode = uiXml.NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	R_ASSERT3(pNode, "encyclopedia article id=", *ref_str(item_data.id));

	//�����
	data()->text = uiXml.Read(pNode, "text", 0);
	//���
	data()->name = uiXml.ReadAttrib(pNode, "name", "");
	//������
	data()->group = uiXml.ReadAttrib(pNode, "group", "");
	//������ ltx, ������ ������ ������
	data()->ltx = uiXml.Read(pNode, "ltx", 0);

	if(*data()->ltx)
	{
		data()->image.SetShader(InventoryUtilities::GetEquipmentIconsShader());

		u32 x		= pSettings->r_u32(data()->ltx, "inv_grid_x") * INV_GRID_WIDTH;
		u32 y		= pSettings->r_u32(data()->ltx, "inv_grid_y") * INV_GRID_HEIGHT;
		u32 width	= pSettings->r_u32(data()->ltx, "inv_grid_width") * INV_GRID_WIDTH;
		u32 height	= pSettings->r_u32(data()->ltx, "inv_grid_height") * INV_GRID_HEIGHT;

		data()->image.GetUIStaticItem().SetOriginalRect(x, y, width, height);
		data()->image.ClipperOn();
	}
	else 
		xml_init.InitTexture(uiXml, "", 0, &data()->image);
}

void CEncyclopediaArticle::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "article";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("encyclopedia", "files");
}
