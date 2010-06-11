// DeviceSetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceSetupDialog.h"
#include ".\devicesetupdialog.h"

// Application
#include "LabJackDasy.h"

// LabJack
#include "c:\program files\labjack\drivers\LabJackUD.h" // TODO: needs to be flexible

// DeviceSetupDialog dialog

IMPLEMENT_DYNAMIC(DeviceSetupDialog, CDialog)
DeviceSetupDialog::DeviceSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(DeviceSetupDialog::IDD, pParent)
{
	CDialog(DeviceSetupDialog::IDD, pParent);
}

DeviceSetupDialog::~DeviceSetupDialog()
{
}

void DeviceSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVICE_TYPE_COMBO, DeviceCombo);
}


BEGIN_MESSAGE_MAP(DeviceSetupDialog, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// DeviceSetupDialog message handlers

void DeviceSetupDialog::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

void DeviceSetupDialog::OnBnClickedOk()
{
	// Determine the device type selected
	switch(DeviceCombo.GetCurSel())
	{
	case U3_COMBOBOX_INDEX:
		OpenNewDevice(LJ_dtU3);
		break;
	case U6_COMBOBOX_INDEX:
		OpenNewDevice(LJ_dtU6);
		break;
	case UE9_COMBOBOX_INDEX:
		OpenNewDevice(LJ_dtUE9);
		break;
	}

	CDialog::OnOK();
}

void DeviceSetupDialog::PopulateDeviceCombo()
{
	// Get the currently selected device type
	long deviceType = GetDeviceType();

	// TODO: This needs to have constants
	DeviceCombo.ResetContent();
	DeviceCombo.AddString("U3");
	if (deviceType == LJ_dtU3)
		DeviceCombo.SetCurSel(U3_COMBOBOX_INDEX);
	DeviceCombo.AddString("U6");
	if (deviceType == LJ_dtU6)
		DeviceCombo.SetCurSel(U6_COMBOBOX_INDEX);
	DeviceCombo.AddString("UE9");
	if (deviceType == LJ_dtUE9)
		DeviceCombo.SetCurSel(UE9_COMBOBOX_INDEX);
	UpdateData(FALSE);
}