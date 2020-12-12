// file:		CCustomSpin.h
// description:	base class for CSpinNum & CSpinText
// created:		15.06.2005
// author:		Serge Vynnychenko
// mail:		narrator@gsc-game.kiev.ua
//
// copyright 2005 GSC Game World

#pragma once
#include "UIWindow.h"
#include "UIOptionsItem.h"

class CUI3tButton;
class CUIFrameLineWnd;
class CUILines;


class CUICustomSpin : public CUIWindow, public CUIOptionsItem {
public:
	CUICustomSpin();
	virtual ~CUICustomSpin();

	// CUIWindow
	virtual void	Init(float x, float y, float width, float height);
	virtual void	SendMessage(CUIWindow* pWnd, s16 msg, void* pData  = NULL);
	virtual void	Draw();
	virtual void	Update();
	virtual void	Enable(bool status);

	// own
	virtual void	OnBtnUpClick();
	virtual void	OnBtnDownClick();
			LPCSTR	GetText();

			void	SetTextColor(u32 color);
			void	SetTextColorD(u32 color);

protected:
	virtual bool	CanPressUp()	= 0;
	virtual bool	CanPressDown()	= 0;

	CUIFrameLineWnd*	m_pFrameLine;
	CUI3tButton*		m_pBtnUp;
	CUI3tButton*		m_pBtnDown;
	CUILines*			m_pLines;

	DWORD				m_time_begin;
	DWORD				m_delay;

	u32 m_textColor[2];
};
