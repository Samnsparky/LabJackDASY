// TODO: We need better documentation in this Visual C++ generated file
#pragma once

// Resource file
#include "Resource.h"

// Windows
#include <afxwin.h>

// LabJack Include
#include "c:\program files\labjack\drivers\LabJackUD.h" // TODO: needs to be flexible

// Application
#include "LabJackLayer.h"

class DeviceSetupDialog : public CDHtmlDialog
{

	DECLARE_DYNCREATE(DeviceSetupDialog);

	LabJackLayer * targetDeviceLayer;

public:
	DeviceSetupDialog(CWnd* pParent = NULL);
	void SetDeviceLayer(LabJackLayer * layer);
	virtual ~DeviceSetupDialog();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_DIALOG2, IDH = IDR_HTML_DEVICESETUPDIALOG };

private:
	const static int LISTBOX_VALUE_1 = LJ_dtU3;
	const static int LISTBOX_VALUE_2 = LJ_dtU6;
	const static int LISTBOX_VALUE_3 = LJ_dtUE9;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
