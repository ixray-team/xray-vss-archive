// Stalker_net.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CStalker_netApp:
// See Stalker_net.cpp for the implementation of this class
//

class CStalker_netApp : public CWinApp
{
public:
	CStalker_netApp();
	virtual	~CStalker_netApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CStalker_netApp theApp;