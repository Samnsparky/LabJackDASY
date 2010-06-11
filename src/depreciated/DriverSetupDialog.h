#pragma once


// DriverSetupDialog dialog
#include "resource1.h"

class DriverSetupDialog : public CDialog
{
	DECLARE_DYNAMIC(DriverSetupDialog)

public:
	DriverSetupDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~DriverSetupDialog();
	void FillDeviceCombo();

// Dialog Data
	enum { IDD = IDD_DRIVER_SETUP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
