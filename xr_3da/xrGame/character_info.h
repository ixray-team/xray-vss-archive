//////////////////////////////////////////////////////////////////////////
// character_info.h			������� ���������� ��� ���������� � ����
// 
//////////////////////////////////////////////////////////////////////////

#pragma		once

#include "character_info_defs.h"
#include "PhraseDialogDefs.h"

#include "alife_registry_container.h"
#include "alife_registry_wrapper.h"


#define DEFAULT_CHARACTER_FILE "npc_profile.xml"

class CInventoryOwner;


class CCharacterInfo
{
private:
	friend CInventoryOwner;
public:
	CCharacterInfo();
	~CCharacterInfo();

	virtual LPCSTR Name() const ;
	virtual LPCSTR Rank() const ;
	virtual LPCSTR Community() const ;

	int		TradeIconX()	const	 {return m_iIconX;}
	int		TradeIconY()	const	 {return m_iIconY;}
	int		MapIconX()		const 	{return m_iMapIconX;}
	int		MapIconY()		const	{return m_iMapIconY;}


	//�������� ������� ��������� �� xml �����,
	//���� ������� �� ������ - ���������� false, ����� true
	bool Load(LPCSTR name_id, LPCSTR xml_file = DEFAULT_CHARACTER_FILE);


	int					 GetGoodwill			(u16 person_id) const ;
	void				 SetGoodwill			(u16 person_id, int goodwill);
	ALife::ERelationType GetRelationType		(u16 person_id) const ;
	void				 SetRelationType		(u16 person_id, ALife::ERelationType new_relation);
	void				 ClearRelations			();

protected:
	//////////////////////////////////////////////////////////////////////////
	// ������� ������������� 

	//������� ��� ���������
	ref_str m_sGameName;

	//��������...(���� ��� RGP ���������)
	ref_str m_sTeamName;
	ref_str m_sRank;

	//��������� ������
	PHRASE_DIALOG_INDEX m_iStartDialog;


	//���� ��������� � ������� �����������
	typedef CALifeRegistryWrapper<CRelationRegistry> RELATION_REGISTRY;
	RELATION_REGISTRY relation_registry;

	//////////////////////////////////////////////////////////////////////////
	// ���������� �������������
	
	//��� ������������ ������
	ref_str m_sVisualName;
	//��������� ������� ������ (��� �������� � �������) � ����� � �������� 
	int	m_iIconX, m_iIconY;
	//��������� ��������� ������ (��� �����)
	int	m_iMapIconX, m_iMapIconY;
};