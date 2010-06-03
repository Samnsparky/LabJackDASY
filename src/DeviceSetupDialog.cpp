// DeviceSetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "LabJackDasy.h"
#include "DeviceSetupDialog.h"

//	LabJack
#include "c:\program files\labjack\drivers\LabJackUD.h" // TODO: needs to be flexible

// DeviceSetupDialog dialog

IMPLEMENT_DYNCREATE(DeviceSetupDialog, CDHtmlDialog)

// TODO: This could require some clean up
DeviceSetupDialog::DeviceSetupDialog(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(DeviceSetupDialog::IDD, DeviceSetupDialog::IDH, pParent)
{
	targetDeviceLayer = NULL;
}

DeviceSetupDialog::~DeviceSetupDialog()
{
	delete targetDeviceLayer;
}

void DeviceSetupDialog::SetDeviceLayer(LabJackLayer* layer)
{
	targetDeviceLayer = layer;
}

void DeviceSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL DeviceSetupDialog::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(DeviceSetupDialog, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(DeviceSetupDialog)
DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// DeviceSetupDialog message handlers

HRESULT DeviceSetupDialog::OnButtonOK(IHTMLElement* /*pElement*/)
{
	// TODO: Do I need to delete this?
	switch(((CComboBox *)GetDlgItem(IDC_DEVICECOMBO))->GetCurSel())
	{
		case 0:
			targetDeviceLayer->SetDeviceType(LISTBOX_VALUE_1);
			break;
		case 1:
			targetDeviceLayer->SetDeviceType(LISTBOX_VALUE_2);
			break;
		case 2:
			targetDeviceLayer->SetDeviceType(LISTBOX_VALUE_3);
			break;
	}
	CloseWindow();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

HRESULT DeviceSetupDialog::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	CloseWindow();
	return S_OK;  // return TRUE  unless you set the focus to a control
}
