// process_killer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cprocess_killerApp:
// See process_killer.cpp for the implementation of this class
//

class Cprocess_killerApp : public CWinApp
{
public:
	Cprocess_killerApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cprocess_killerApp theApp;