///////////////////////////////////////////////////////////////
// encyclopedia_article.h
// ���������, �������� � ����������� ������ � ������������
///////////////////////////////////////////////////////////////

#pragma once

#include "encyclopedia_article_defs.h"
#include "xml_str_id_loader.h"
#include "shared_data.h"

#include "ui\uistatic.h"

//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: ������ ��� InfoProtion
//////////////////////////////////////////////////////////////////////////
struct SArticleData : CSharedResource
{
	ref_str name;
	ref_str group;
	
	//��������
	CUIStatic image;

	//����� ������
	ref_str	text;
	
	//������ ltx, ������ ������ ������
	ref_str ltx;
};

class CEncyclopediaArticle;

class CEncyclopediaArticle : public CSharedClass<SArticleData, ARTICLE_INDEX>,
							 public CXML_IdToIndex<ARTICLE_STR_ID,	ARTICLE_INDEX, CEncyclopediaArticle>
{
private:
	typedef CSharedClass<SArticleData, ARTICLE_INDEX> inherited_shared;
	typedef CXML_IdToIndex<ARTICLE_STR_ID, ARTICLE_INDEX, CEncyclopediaArticle>	id_to_index;

	friend id_to_index;
public:
	CEncyclopediaArticle			();
	virtual ~CEncyclopediaArticle	();

	virtual void Load	(ARTICLE_STR_ID str_id);
	virtual void Load	(ARTICLE_INDEX  index);

protected:
	ARTICLE_INDEX	m_ArticleIndex;

	void load_shared	(LPCSTR);
	SArticleData* data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	static void InitXmlIdToIndex();
};
