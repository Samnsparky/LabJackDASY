// LabJackDasy.h : main header file for the LabJackDasy DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CLabJackDasyApp
// See LabJackDasy.cpp for the implementation of this class
//

class CLabJackDasyApp : public CWinApp
{
public:
	CLabJackDasyApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
