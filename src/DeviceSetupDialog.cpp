// DeviceSetupDialog.cpp : implementation file

// Windows
#include "stdafx.h"

// Header file for dialog
#include "DeviceSetupDialog.h"

// Application
#include "LabJackDasy.h"

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
	DDX_Control(pDX, IDC_ID_ENTRY, idEntry);
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
	int id;

	// Get the id number as string
	CString * idAddress = new CString("");
	idEntry.GetWindowText(*idAddress);

	// Try to convert the number to string or default to 0
	id = atoi(*idAddress);

	// Determine the device type selected
	switch(DeviceCombo.GetCurSel())
	{
	case U3_COMBOBOX_INDEX:
		OpenNewDevice(LJ_dtU3, id);
		break;
	case U6_COMBOBOX_INDEX:
		OpenNewDevice(LJ_dtU6, id);
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
			OpenNewDevice(LJ_dtUE9, id);
		break;
	}

	delete idAddress;

	CDialog::OnOK();
}

void DeviceSetupDialog::PopulateFields()
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

		// Show the ethernet control and disable local id entry
		ToggleControls(true, IsUsingEthernet());
	}
	else
		ToggleControls(false, false);
	UpdateData(FALSE);
}

/**
 * Name: ToggleControls(bool showCheckbox, bool showIPEntry)
 * Desc: Shows or hides the ethernet controls and toggles 
 *		 the ethernet checkbox and local ID entry, filling
 *		 each appropriately.
**/
void DeviceSetupDialog::ToggleControls(bool showCheckbox, bool showIPEntry)
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
		idEntry.EnableWindow(FALSE);

		// Fill the IP address entry
		ipEntry.SetWindowText(GetIPAddress());
	}
	else
	{
		ethernetCheck.SetCheck(BST_UNCHECKED);
		ipAddressLabel.ShowWindow(SW_HIDE);
		ipEntry.ShowWindow(SW_HIDE);
		idEntry.EnableWindow(TRUE);

		// Fill the ID entry
		idEntry.SetWindowText(ToCString(GetID()));
	}
}
void DeviceSetupDialog::OnCbnSelchangeDeviceTypeCombo()
{
	// Determine the device type selected
	if(DeviceCombo.GetCurSel() == UE9_COMBOBOX_INDEX)
		ToggleControls(true, IsUsingEthernet());
	else
		ToggleControls(false, false);
}

void DeviceSetupDialog::OnBnClickedEthernetCheck()
{
	if(ethernetCheck.GetCheck())
		ToggleControls(true, true);
	else
		ToggleControls(true, false);
}
