// DeviceSetupDialog.cpp : implementation file

// Windows
#include "stdafx.h"

// Header file for dialog
#include "DeviceSetupDialog.h"

// Application
#include "LabJackDasy.h"
#include ".\devicesetupdialog.h"

IMPLEMENT_DYNAMIC(DeviceSetupDialog, CDialog)
DeviceSetupDialog::DeviceSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(DeviceSetupDialog::IDD, pParent)
{
	CDialog(DeviceSetupDialog::IDD, pParent);
	CreateTimerModesConst();
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
	DDX_Control(pDX, IDC_TIMER0_COMBO, timerCombos[0]);
	DDX_Control(pDX, IDC_TIMER1_COMBO, timerCombos[1]);
	DDX_Control(pDX, IDC_TIMER2_COMBO, timerCombos[2]);
	DDX_Control(pDX, IDC_TIMER3_COMBO, timerCombos[3]);
	DDX_Control(pDX, IDC_TIMER4_COMBO, timerCombos[4]);
	DDX_Control(pDX, IDC_TIMER5_COMBO, timerCombos[5]);
	DDX_Control(pDX, IDC_TIMER6_COMBO, timerCombos[6]);
}


BEGIN_MESSAGE_MAP(DeviceSetupDialog, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_DEVICE_TYPE_COMBO, OnCbnSelchangeDeviceTypeCombo)
	ON_BN_CLICKED(IDC_ETHERNET_CHECK, OnBnClickedEthernetCheck)
	ON_CBN_SELCHANGE(IDC_TIMER0_COMBO, OnCbnSelchangeTimer0Combo)
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
	int i, k;

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

	// Fill timer combo boxes
	for(i=0; i<7; i++)
		for(k=0; k<14; k++)
			timerCombos[i].AddString((LPCTSTR)(LJ_TIMER_MODES[k].GetDescription()));

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

/**
 * Name: LinkedTimerCombo.CreateTimerModesConst()
 * Desc: Create "constant" LJ_TIMER_MODES
**/
void DeviceSetupDialog::CreateTimerModesConst()
{
	LJ_TIMER_MODES[0] = TimerMode(LJ_tmPWM16, "LJ_tmPWM16");
	LJ_TIMER_MODES[1] = TimerMode(LJ_tmPWM8, "LJ_tmPWM8");
	LJ_TIMER_MODES[2] = TimerMode(LJ_tmRISINGEDGES32, "LJ_tmRISINGEDGES32");
	LJ_TIMER_MODES[3] = TimerMode(LJ_tmFALLINGEDGES32, "LJ_tmFALLINGEDGES32");
    LJ_TIMER_MODES[4] = TimerMode(LJ_tmDUTYCYCLE, "LJ_tmDUTYCYCLE");
	LJ_TIMER_MODES[5] = TimerMode(LJ_tmFIRMCOUNTER, "LJ_tmFIRMCOUNTER");
	LJ_TIMER_MODES[6] = TimerMode(LJ_tmFIRMCOUNTERDEBOUNCE, "LJ_tmFIRMCOUNTERDEBOUNCE");
	LJ_TIMER_MODES[7] = TimerMode(LJ_tmFREQOUT, "LJ_tmFREQOUT");
	LJ_TIMER_MODES[8] = TimerMode(LJ_tmQUAD, "LJ_tmQUAD");
	LJ_TIMER_MODES[9] = TimerMode(LJ_tmTIMERSTOP, "LJ_tmTIMERSTOP");
	LJ_TIMER_MODES[10] = TimerMode(LJ_tmSYSTIMERLOW, "LJ_tmSYSTIMERLOW");
	LJ_TIMER_MODES[11] = TimerMode(LJ_tmSYSTIMERHIGH, "LJ_tmSYSTIMERHIGH");
	LJ_TIMER_MODES[12] = TimerMode(LJ_tmRISINGEDGES16, "LJ_tmRISINGEDGES16");
	LJ_TIMER_MODES[13] = TimerMode(LJ_tmFALLINGEDGES16, "LJ_tmFALLINGEDGES16");
}

void DeviceSetupDialog::OnCbnSelchangeTimer0Combo()
{
	// TODO: Add your control notification handler code here
}
