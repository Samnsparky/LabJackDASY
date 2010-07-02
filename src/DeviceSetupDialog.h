/**
 * Copyright (c) 2010 LabJack Corp.
 * See License.txt for more information
 *
 * Name: DeviceSetupDialog.h
 * Desc: Header file for the MFC DeviceSetupDialog
**/

#pragma once
#include "afxwin.h"
#include "resource.h"
#include <string>
#include "afxcmn.h"
#include <stdio.h>
#include <stdlib.h>

// LabJack
#include "c:\program files\labjack\drivers\LabJackUD.h" // TODO: needs to be flexible

// DeviceSetupDialog dialog

class DeviceSetupDialog : public CDialog
{
	DECLARE_DYNAMIC(DeviceSetupDialog)
	const static int U3_COMBOBOX_INDEX = 0;
	const static int U6_COMBOBOX_INDEX = 1;
	const static int UE9_COMBOBOX_INDEX = 2;

public:
	DeviceSetupDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~DeviceSetupDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	void PopulateFields();

// Dialog Data
	enum { IDD = IDD_DEVICE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CComboBox DeviceCombo;
	void ToggleControls(bool showCheckbox, bool showIPEntry);
public:
	CEdit ipEntry;
	CStatic ethernetLabel;
	CButton ethernetCheck;
	CStatic ipAddressLabel;
	afx_msg void OnCbnSelchangeDeviceTypeCombo();
	afx_msg void OnBnClickedEthernetCheck();
	afx_msg void OnEnChangeIpEntry();
	CEdit idEntry;
};
