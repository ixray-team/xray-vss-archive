#pragma once
#include "UIDialogWnd.h"
#include "UIListItem.h"
#include "UIWndCallback.h"

class CGameTask;
class CUIStatic;
class CUIButton;
struct SGameTaskObjective;
class CUIEventsWnd;
class CUIEditBox;

class CUITaskItem :public CUIListItem, public CUIWndCallback
{
	typedef		CUIListItem	inherited;
protected:
	CGameTask*		m_GameTask;
	int				m_TaskObjectiveIdx;
	void			OnClick					();
	void			Init					();
public:
					CUITaskItem				(CUIEventsWnd* w);
	virtual			~CUITaskItem			();
	virtual void	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
					
	virtual void	SetGameTask				(CGameTask* gt, int obj_idx);
	virtual void	Update					();

	CGameTask*		GameTask				()	{return m_GameTask;}
	int				ObjectiveIdx			()	{return m_TaskObjectiveIdx;}
	SGameTaskObjective*	Objective			();

	CUIEventsWnd*	m_EventsWnd;
};

class CUITaskRootItem :public CUITaskItem
{
	typedef		CUITaskItem	inherited;
protected:
	CUIStatic*		m_taskImage;
	CUIStatic*		m_captionStatic;
	CUIStatic*		m_captionTime;
	CUI3tButton*	m_showLocationBtn;
	CUI3tButton*	m_switchDescriptionBtn;
	bool			m_curr_descr_mode;
	void			Init					();
public:	
					CUITaskRootItem			(CUIEventsWnd* w);
	virtual			~CUITaskRootItem		();
	virtual void	Update					();
	virtual void	SetGameTask				(CGameTask* gt, int obj_idx);
			void	OnShowLocationClicked	();
			void	OnSwitchDescriptionClicked();

	virtual void	MarkSelected			(bool b);
	virtual bool	OnDbClick				();
};

class CUITaskSubItem :public CUITaskItem
{
	typedef			CUITaskItem	inherited;
	u32				m_active_color;
	u32				m_failed_color;
	u32				m_accomplished_color;
protected:
	CUI3tButton*	m_showPointerBtn;
	CUI3tButton*	m_showDescriptionBtn;
	CUIStatic*		m_descriptionStatic;
	CUIStatic*		m_stateStatic;

	void			Init					();

public:	
					CUITaskSubItem			(CUIEventsWnd* w);
	virtual			~CUITaskSubItem			();
	virtual void	Update					();
	virtual void	SetGameTask				(CGameTask* gt, int obj_idx);
			void	OnShowPointerClicked	();
			void	OnShowDescriptionClicked();
	virtual void	MarkSelected			(bool b);
	virtual bool	OnDbClick				();
};

class CUIUserTaskEditWnd;
class CUIUserTaskItem :public CUITaskItem
{
	typedef			CUITaskItem	inherited;
protected:
	CUI3tButton*	m_showPointerBtn;
	CUI3tButton*	m_showLocationBtn;
	CUI3tButton*	m_editTextBtn;
	CUI3tButton*	m_removeBtn;
	CUIStatic*		m_captionStatic;
	CUIStatic*		m_descriptionStatic;
	CUIStatic*		m_image;
	CUIUserTaskEditWnd* m_edtWnd;
	void			Init					();

public:	
					CUIUserTaskItem			(CUIEventsWnd* w);
	virtual			~CUIUserTaskItem			();
	virtual void	Update					();
	virtual void	SetGameTask				(CGameTask* gt, int obj_idx);
			void	OnShowLocationClicked	();
			void	OnShowPointerClicked	();
			void	OnDescriptionChanged	();
			void	OnEditTextClicked		();
			void	OnRemoveClicked			();

	virtual bool	OnDbClick				()	{return true;};
	virtual void	MarkSelected			(bool b);
};

class CUIUserTaskEditWnd : public CUIDialogWnd, public CUIWndCallback
{
	CUIUserTaskItem*		m_userTask;
	CUI3tButton*			m_btnOk;
	CUI3tButton*			m_btnCancel;
	CUIFrameWindow*			m_background;
	CUIFrameWindow*			m_background_1;
	CUIFrameWindow*			m_background_2;

	CUIEditBox*				m_editCaption;
	CUIEditBox*				m_editDescription;
protected:
			void			OnOk					();
			void			OnCancel				();
			void			Init					();
public:
							CUIUserTaskEditWnd		();
	virtual void			SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
			void			Start					(CUIUserTaskItem* itm);
};