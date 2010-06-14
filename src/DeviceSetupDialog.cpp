// DeviceSetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceSetupDialog.h"

// Application
#include "LabJackDasy.h"

// LabJack
#include "c:\program files\labjack\drivers\LabJackUD.h" // TODO: needs to be flexible

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
	DDX_Control(pDX, IDC_ETHERNET_LABEL, ethernetLabel);
	DDX_Control(pDX, IDC_ETHERNET_CHECK, ethernetCheck);
	DDX_Control(pDX, IDC_IP_ADDRESS_LABEL, ipAddressLabel);
	DDX_Control(pDX, IDC_IP_ENTRY, ipEntry);
}


BEGIN_MESSAGE_MAP(DeviceSetupDialog, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_DEVICE_TYPE_COMBO, OnCbnSelchangeDeviceTypeCombo)
	ON_BN_CLICKED(IDC_ETHERNET_CHECK, OnBnClickedEthernetCheck)
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
		
		// Check for ethernet
		if(ethernetCheck.GetCheck())
		{
			CString * ipAddress = new CString("");
			ipEntry.GetWindowText(*ipAddress);
			OpenNewEthernetDevice(LJ_dtUE9, *ipAddress);
			delete ipAddress;
		}
		else
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

	// Take care of the U3 and U6
	DeviceCombo.AddString("U3");
	if (deviceType == LJ_dtU3)
		DeviceCombo.SetCurSel(U3_COMBOBOX_INDEX);
	DeviceCombo.AddString("U6");
	if (deviceType == LJ_dtU6)
		DeviceCombo.SetCurSel(U6_COMBOBOX_INDEX);

	// Handle the more complex UE9 and the ethernet control
	DeviceCombo.AddString("UE9");
	if (deviceType == LJ_dtUE9)
	{
		// Set the selection
		DeviceCombo.SetCurSel(UE9_COMBOBOX_INDEX);

		// Show the ethernet control
		SetEthernetControl(true, IsUsingEthernet());
	}
	else
		SetEthernetControl(false, false);
	UpdateData(FALSE);
}

/**
 * Name: SetEthernetControl(bool showCheckbox, bool showIPEntry)
 * Desc: Shows or hides the ethernet controls and toggles the ethernet checkbox
**/
void DeviceSetupDialog::SetEthernetControl(bool showCheckbox, bool showIPEntry)
{
	if(showCheckbox)
	{
		ethernetLabel.ShowWindow(SW_SHOW);
		ethernetCheck.ShowWindow(SW_SHOW);
	}
	else
	{
		ethernetLabel.ShowWindow(SW_HIDE);
		ethernetCheck.ShowWindow(SW_HIDE);
	}
	if(showIPEntry)
	{
		ethernetCheck.SetCheck(BST_CHECKED);
		ipAddressLabel.ShowWindow(SW_SHOW);
		ipEntry.ShowWindow(SW_SHOW);
	}
	else
	{
		ethernetCheck.SetCheck(BST_UNCHECKED);
		ipAddressLabel.ShowWindow(SW_HIDE);
		ipEntry.ShowWindow(SW_HIDE);
	}

}
void DeviceSetupDialog::OnCbnSelchangeDeviceTypeCombo()
{
	// Determine the device type selected
	if(DeviceCombo.GetCurSel() == UE9_COMBOBOX_INDEX)
		SetEthernetControl(true, IsUsingEthernet());
	else
		SetEthernetControl(false, false);
}

void DeviceSetupDialog::OnBnClickedEthernetCheck()
{
	if(ethernetCheck.GetCheck())
		SetEthernetControl(true, true);
	else
		SetEthernetControl(true, false);
}
