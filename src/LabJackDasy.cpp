/**
 * Copyright (c) 2010 LabJack Corp.
 * See License.txt for more information
 *
 * Name: LabJackDasy.cpp
 * Desc: A layer that provides entry points for
 *		 and simplifies interactions with DASYLab
**/

// Allow us to access the MFC hInstance
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

/** Disable warnings in Win32 headers **/
#pragma warning ( disable : 4115 )  // named type definition in parentheses
#pragma warning ( disable : 4201 )  // Nameless struct/union
#pragma warning ( disable : 4214 )  // Bitfield types other than int
#pragma warning ( disable : 4514 )  // unreferenced inline function has been removed
#pragma warning ( disable : 4704 )  // in-line assembler precludes global optimizations
#pragma warning ( disable : 4189 )  // unreferenced constant reference to temporary object
#pragma warning ( disable : 4244 )  // possible loss of data due to cast

/** Includes **/
// TODO: Clean these up, not all are used
// Debug
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//	Windows
#include "stdafx.h" 
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
//#include <afxwin.h>

//	Compiler
#include <float.h>
#include <math.h>
//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

//	DASYLab driver interface
#include "treiber.h"

//	Application
#include "LabJackDasy.h"
#include "LabJackLayer.h"
#include "DeviceSetupDialog.h"

//	LabJack
#include "c:\program files\labjack\drivers\LabJackUD.h" // TODO: needs to be flexible

/** Global variables **/
// TODO: Need to get rid of these distasteful global variables
LabJackLayer * deviceLayer;
const char DRIVER_NAME [] = "LabJackDASY";
const char DASY_DRIVER_VERSION [] = "0.1//05.10";	// Version/Date as required by DASYLab
const double DEFAULT_FREQUENCY = 10;				// The default frequency for input (Hz)
const DWORD DEFAULT_BLOCK_SIZE = 4;					// The default size allocated for readings (bytes)
DWORD StartTime;									// for start time of measure
HINSTANCE hInst = NULL;								// handle to previous WINDOWS-instance
DRV_INFOSTRUCT * infoStruct;						// Pointer to DASYLab's information structure
//DriverSetupWindow setupDialog;

/**
 * Name: DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
 * Args: hinstDLL, handle to the DLL module
 *		 dwReason, the reason for why the DLL is being called
 *		 lpvReserved, NULL for dynamic load, non-NULL for static load
 * Desc: Entry point for DLL
**/
//BOOL WINAPI DllMain ( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
//{
//	switch (dwReason)
//	{
//
//	case DLL_PROCESS_ATTACH:
//		if ( hInst != 0 )
//			return FALSE;
//		hInst = hInstance;
//		break;
//
//	case DLL_THREAD_ATTACH:
//		break;
//
//	case DLL_THREAD_DETACH:
//		break;
//
//	case DLL_PROCESS_DETACH:
//		break;
//
//	default:
//		UNUSED (lpReserved);
//		break;
//	}
//
//	return TRUE;
//}
// NOTE: MFC entry points and object placed below

extern "C" 
{

/**
 * Name: DRV_AdvanceAnalogOutputBuf()
 * Desc: Marks the next block in the analog output buffer to be valid
**/
int _stdcall DRV_AdvanceAnalogOutputBuf()
{
	deviceLayer->AdvanceAnalogOutputBuf();
	return TRUE;
}

/**
 * Name: DRV_AdvanceAnalogOutputBuf()
 * Desc: Marks the next block in the counter output buffer to be valid
 * Note: This is currently a stub and will be implemented in the future
**/
int _stdcall DRV_AdvanceCounterOutputBuf()
{
	// TODO: Finish this counter stub
	return TRUE;
}

/**
 * Name: DRV_AdvanceDigitalOutputBuf()
 * Desc: Marks the next block in the digital output buffer to be valid
**/
int _stdcall DRV_AdvanceDigitalOutputBuf()
{
	deviceLayer->AdvanceDigitalOutputBuf();
	return TRUE;
}

/**
 * Name: DRV_AdvanceInputBuf()
 * Desc: Frees oldest block in intermediate buffer after being processed by DASYLab
**/
int _stdcall DRV_AdvanceInputBuf()
{
	deviceLayer->AdvanceInputBuf(); //AnalogInputBuf();
	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_ExplainGainCode(SCAN_ENTRY chan, UINT gainIndex)
 * Desc: Returns a string explanation of the given range for the given channel and index
**/
char* _stdcall DRV_ExplainGainCode(SCAN_ENTRY chan, UINT gainIndex)
{
	UNUSED (chan);

	long gainCode, dasyLabGainCode;

	dasyLabGainCode = infoStruct->GainInfo[gainIndex];

	gainCode = deviceLayer->ConvertToUDRange(dasyLabGainCode);

	// Explain the gain code according to the range constant it represents
	// TODO: There ought to be a more graceful way to do this?
	switch(gainCode)
	{
		case LJ_rgAUTO:
			return "auto";
		case LJ_rgBIP20V:
			return "+/- 20V";
		case LJ_rgBIP10V:
			return "+/- 10V";
		case LJ_rgBIP5V:
			return "+/- 5V";
		case LJ_rgBIP4V:
			return "+/- 4V";
		case LJ_rgBIP2P5V:
			return "+/- 2V";
		case LJ_rgBIP2V:
			return "+/- 2V";
		case LJ_rgBIP1P25V:
			return "+/- 1.25V";
		case LJ_rgBIP1V:
			return "+/- 1V";
		case LJ_rgBIPP625V:
			return "+/- 0.625V";
		case LJ_rgBIPP1V:
			return "+/- 0.1V";
		case LJ_rgBIPP01V:
			return "+/- 0.01V";
		case LJ_rgUNI20V:
			return "0-20V";
		case LJ_rgUNI10V:
			return "0-10V";
		case LJ_rgUNI5V:
			return "0-5V";
		case LJ_rgUNI4V:
			return "0-4V";
		case LJ_rgUNI2P5V:
			return "0-2.5V";
		case LJ_rgUNI2V:
			return "0-2V";
		case LJ_rgUNI1P25V:
			return "0-1.25V";
		case LJ_rgUNI1V:
			return "0-1V";
		case LJ_rgUNIP625V:
			return "0-0.625V";
		case LJ_rgUNIP5V:
			return "0-5V";
		case LJ_rgUNIP25V:
			return "0-0.25V";
		case LJ_rgUNIP3125V:
			return "0-0.3125V";
		case LJ_rgUNIP025V:
			return "0-0.025V";
		case LJ_rgUNIP0025V:
			return "0-0.0025V";
		default:
			return NULL; // Error - Invalid gain code
	}
}

/**
 * Name: DRV_GetAnalogOutputBuf()
 * Desc: Returns a pointer to a buffer for new AO data
**/
LPSAMPLE _stdcall DRV_GetAnalogOutputBuf()
{
	return deviceLayer->GetAnalogOutputBuf();
}

/**
 * Name: DRV_GetAnalogOutputStatus()
 * Desc: Test if there is space to place a block of output data
**/
int _stdcall DRV_GetAnalogOutputStatus()
{
	return (int)deviceLayer->GetAnalogOutputStatus();
}

/**
 * Name: DRV_GetCounterOutputBuf()
 * Desc: Returns a pointer to a buffer for new counter data
 * Note: This is just a stub
**/
LPSAMPLE _stdcall DRV_GetCounterOutputBuf()
{
	// TODO: Implementation for this counter stub
	return NULL;
}

/**
 * Name: DRV_GetCounterOutputStatus()
 * Desc: Test if there is space to place a block of counter data
 * Note: This is just a stub
**/
int _stdcall DRV_GetCounterOutputStatus()
{
	//TODO: Implementation for this counter stub
	return FALSE;
}

/**
 * Name: DRV_GetDigitalOutputBuf()
 * Desc: Returns a pointer to a buffer for new digital output data
**/
LPSAMPLE _stdcall DRV_GetDigitalOutputBuf()
{
	return deviceLayer->GetDigitalOutputBuf();
}

/**
 * Name: DRV_GetDigitalOutputStatus()
 * Desc: Test if there is space to place a block of output data
**/
int _stdcall DRV_GetDigitalOutputStatus()
{
	return deviceLayer->GetDigitalOutputStatus();
}

/**
 * Name: DRV_GetHardwareDescription()
 * Desc: Give the address of the Hardware Description List
 * Note: This function is a low priority stub
**/
HW_COMPONENT * _stdcall DRV_GetHardwareDescription()
{
	// TODO: Finish the hardware description stub
	return NULL;
}

/**
 * Name: DRV_GetInputBuf()
 * Desc: Return a pointer to a buffer for new input data
**/
LPSAMPLE _stdcall DRV_GetInputBuf()
{
	return deviceLayer->GetInputBuf(); //AnalogInputBuf();
}

/**
 * Name: GetInputBufStatus()
 * Desc: Test if input data is waiting to be processed
 * Retn: DRV_FUNCTION, less than ADI_BlockSize are waiting in the buffer
 *		 DRV_FUNCTION_OK, otherwise
**/
int _stdcall DRV_GetInputBufStatus()
{
	return deviceLayer->GetInputStatus(); //AnalogInputStatus();
}

/**
 * Name: DRV_GetMeasInfoEx()
 * Desc: Get status (running, lost interrupts, etc.) of experiment
**/
DRV_MEASINFO * _stdcall DRV_GetMeasInfoEx()
{
	return deviceLayer->GetMeasInfo();
}

/**
 * Name: DRV_InitDevice()
 * Desc: Creates a new LabJackLayer with the given DASYLab information structure
**/
int _stdcall DRV_InitDevice(DRV_INFOSTRUCT *newInfoStruct)
{
	
	infoStruct = newInfoStruct;
	
	// Create the layer but do not open the device
	deviceLayer = new LabJackLayer(newInfoStruct);

	// Define the driver identification information not related to the device
	_fstrcpy(infoStruct->DriverName, DRIVER_NAME);
	_fstrcpy(infoStruct->DLL_Version, DASY_DRIVER_VERSION);
	_fstrcpy(infoStruct->VxD_Version, DASY_DRIVER_VERSION);
	infoStruct->DriverIdCode = DRV_ID_FREEWARE;	// or DRV_ID_OEM

	// Set experiment defaults
	infoStruct->AI_Frequency = DEFAULT_FREQUENCY;
	infoStruct->ADI_BlockSize = DEFAULT_BLOCK_SIZE;

	// Find first available device
	long numDevices = 0;
	long targetDeviceType = 0;
	long serialNumbers [128];
	long ids [128];
	double addresses [128];

	// TODO: Might be a better way to do this?
	ListAll(LJ_dtU3, LJ_ctUSB, &numDevices, &serialNumbers[0], &ids[0], &addresses[0]);
	targetDeviceType = NONE_TYPE;
	if(numDevices > 0)
		targetDeviceType = LJ_dtU3;
	ListAll(LJ_dtU6, LJ_ctUSB, &numDevices, &serialNumbers[0], &ids[0], &addresses[0]);
	if(numDevices > 0)
		targetDeviceType = LJ_dtU6;
	ListAll(LJ_dtUE9, LJ_ctUSB, &numDevices, &serialNumbers[0], &ids[0], &addresses[0]);
	if(numDevices > 0)
		targetDeviceType = LJ_dtUE9;
	if(targetDeviceType == NONE_TYPE)
	{
		deviceLayer->SetError(LJE_LABJACK_NOT_FOUND);
		DRV_ShowError();
		//return DRV_FUNCTION_FALSE;
	}

	// Open the device
	deviceLayer->OpenDevice(targetDeviceType, 0); // Pass zero for first found

	// Return value according to errors generated
	if (deviceLayer->GetError())
		DRV_ShowError();
		//return DRV_FUNCTION_FALSE;
	//else
	return DRV_FUNCTION_OK;
	// Force to return OK for ethernet purposes
}

/**
 * Name: DRV_KillDevice()
 * Desc: Clean up the device layer
**/
int _stdcall DRV_KillDevice()
{
	if (deviceLayer->IsOpen())
	{
		deviceLayer->CleanUp();
		//_CrtDumpMemoryLeaks();
		delete deviceLayer;
		//_CrtDumpMemoryLeaks();
		//delete deviceDialog;
		return DRV_FUNCTION_OK;
	}
	else
		return DRV_FUNCTION_FALSE;
}

/**
 * Name: DRV_LoadWorksheet(const char *name)
 * Desc: Required DASYLab entry point
 * Note: This is a useless stub and LabJack Corp. has no intention of finishing it :)
**/
int _stdcall DRV_LoadWorksheet(const char *name)
{
	UNUSED(Name);
	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_LoadWorksheet()
 * Desc: Required DASYLab entry point
 * Note: This is a stub that will not be completed in the near future
**/
int _stdcall DRV_NewWorksheet()
{
	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_ReadCounterInput(UINT ch)
 * Desc: Reads the current state of the given counter
 * Note: This is a stub
**/
DWORD _stdcall DRV_ReadCounterInput(UINT ch)
{
	// TODO: Finish this counter stub
	deviceLayer->SetError(DRV_ERR_NOHWSUPPORT);
	UNUSED (ch);
	return DRV_FUNCTION_FALSE;
}

/**
 * Name: DRV_ReadDigitalInput(UINT ch)
 * Desc: Get asynchronous digital input reading
 * Note: This is a stub and is not currrently planned to be completed.
 *		 While the LabJack is capable (designed) to preform this action,
 *		 DASYLab will simply call this function from time to time anyway
 *		 so stream through command response / streaming mode will be favored.
**/
DWORD _stdcall DRV_ReadDigitalInput(UINT ch)
{
	UNUSED ( ch );
	return 0;
}

/**
 * Name: DRV_SaveWorksheet(const char *name)
 * Desc: Required DASYLab entry point called when worksheets are saved
 * Note: This is a stub but is not currently scheduled for completion
**/
int _stdcall DRV_SaveWorksheet(const char *name)
{
	UNUSED(Name);
	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_SetAnalogOutputBufferMode(UINT mode, DWORD numSamples, DWORD startDelay)
 * Desc: Change the size of the analog output buffer
**/
int _stdcall DRV_SetAnalogOutputBufferMode(UINT mode, DWORD numSamples, DWORD startDelay)
{
	UNUSED(mode); // TODO: Take care of multiple modes (not currently supported by other software)
	UNUSED(startDelay); // TODO: Add support for a starting delay on measurement

	deviceLayer->AllocateAOBuffer(numSamples);

	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_SetCounterOutputBufferMode(UINT mode, DWORD numSamples, DWORD startDelay)
 * Desc: Change the size of the counter buffer
 * Note: This is just a stub
**/
int _stdcall DRV_SetCounterOutputBufferMode(UINT mode, DWORD numSamples, DWORD startDelay)
{
	// TODO: Implement this counter stub
	return 0;
}

/**
 * Name: DRV_SetDigitalOutputBufferMode(UINT mode, DWORD numSamples, DWORD startDelay)
 * Desc: Change the mode, size and StartDelay of the DO buffer.
**/
int _stdcall DRV_SetDigitalOutputBufferMode(UINT mode, DWORD numSamples, DWORD startDelay)
{
	UNUSED(mode);
	deviceLayer->SetDigitalOutputBufferMode(numSamples, startDelay);
	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_SetInputBufferSize(DWORD Size)
 * Desc: Ensures that the input buffer for DASYLab is sufficienly large
**/
int _stdcall DRV_SetInputBufferSize(DWORD size)
{
	if ( !deviceLayer->IsMeasuring() )
	{
		deviceLayer->AllocateInputBuffer(size); //AIBuffer(size);
		return DRV_FUNCTION_OK;
	}

	deviceLayer->SetError(DRV_ERR_MEASRUN);
	return DRV_FUNCTION_FALSE;
}

/**
 * Name: DRV_SetScanList(UINT scanListLength, UINT scanListMaxGroupSize, SCAN_ENTRY *scanList)
 * Desc: The functions informs the driver of the scan list to use. However, becuase LabJackDASY
 *		 assumes a standard scan list based on the number of active channels, this
 *		 function will never be called even though DASYLab still requires its presence.
**/
int _stdcall DRV_SetScanList(UINT scanListLength, UINT scanListMaxGroupSize, SCAN_ENTRY *scanList)
{
	return 0;
}

int _stdcall DRV_ShowDialog(UINT boxNum, DWORD extraPara)
{
	DeviceSetupDialog*wnd;
	CWnd * parent;
	FARPROC lpProc;
	HWND hWnd;

	// Ugly code thanks to SP1 :(
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// get actual window
	hWnd = GetActiveWindow();
	parent = CWnd::FromHandle(hWnd);
	wnd = new DeviceSetupDialog();
	wnd->Create(IDD_DEVICE_DIALOG, parent); 
	wnd->PopulateFields();
	wnd->ShowWindow(SW_SHOW);
	//m_pMainWnd = wnd;

	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_ShowError()
 * Desc: Displays a basic message box with an error encountered
**/
int _stdcall DRV_ShowError()
{
	char err[255];

	if(deviceLayer->GetError() > LabJackLayer::LABJACK_ERROR_PREFIX)
	{
		ErrorToString(deviceLayer->GetError()-5000, err);
		MessageBox (GetActiveWindow (), err, "LabJack Error", MB_OK | MB_ICONSTOP);
	}
	else
	{
		sprintf(err, "DASYLab error number %i", deviceLayer->GetError());
		MessageBox (GetActiveWindow (), err, "DASYLab Error", MB_OK | MB_ICONSTOP);
	}
	deviceLayer->SetError(0);

	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_StartMeas()
 * Desc: Prepares info structure and LabJack ready for a DASYLab experiment
**/
int _stdcall DRV_StartMeas()
{
	HCURSOR oldCursor;

	// show WAIT-status with cursor
	oldCursor = SetCursor (LoadCursor (NULL, IDC_WAIT));

	// Tell the LabJackLayer that we are ready
	deviceLayer->BeginExperiment();
    
	// reset cursor
	SetCursor (oldCursor);

	if(deviceLayer->IsMeasuring())
		return DRV_FUNCTION_OK;
	else
		return DRV_FUNCTION_FALSE;
}

/**
 * Name: DRV_StopMeas()
 * Desc: Cleans up after a DASYLab experiment
**/
int _stdcall DRV_StopMeas()
{
	deviceLayer->StopExperiment();
	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_TestStruct()
 * Desc: Verify the validity of the communication structure
**/
int _stdcall DRV_TestStruct()
{
	if(deviceLayer->ConfirmDataStructure())
		return DRV_FUNCTION_OK;
	else
		return DRV_FUNCTION_FALSE;
}

/**
 * Name: DRV_WriteAnalogOutput
 * Desc: Entry point for DASYLab to write the given value to the given channel
 * Note: This is a stub that will be implemented for Alpha 0.1
**/
int _stdcall DRV_WriteAnalogOutput(UINT chan, DWORD outVal)
{
	deviceLayer->WriteDAC(chan, outVal);
	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_WriteCounterOutput
 * Desc: Entry point for DASYLab to write the given value to the given counter channel
 * Note: This is a stub that will be implemented later
**/
int _stdcall DRV_WriteCounterOutput(UINT chan, DWORD outVal)
{
	// TODO: Finish this stub
	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_WriteDigitalOutput
 * Desc: Entry point for DASYLab to write the given value to the given counter channel
**/
int _stdcall DRV_WriteDigitalOutput(UINT chan, DWORD outVal)
{
	deviceLayer->WriteDigitalOutput(chan, outVal);

	return DRV_FUNCTION_OK;
}

} // extern "C"

/**
 * Name: StreamCallback
 * Desc: Wrapper function that provides an entry point for the UD driver
 *		 to call the StreamCallback function on the device layer
**/
void StreamCallbackWrapper(long scansAvailable, double userValue)
{
	deviceLayer->StreamCallback(scansAvailable, userValue);
}

/**
 * Name: CommandResponseCallback()
 * Desc: Wrapper function that provides an entry point for the windows
 *		 to call the PollDevice function on the device layer
**/
void CALLBACK CommandResponseCallbackWrapper(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	deviceLayer->CommandResponseCallback();
}

/**
 * Name: DlgCardDef (HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
 * Desc: administration of dialog
 * Note: This is basically copied from DASYLab's example driver
**/
BOOL CALLBACK DlgCardDef (HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		{			 
			UINT cmd_id  = GET_WM_COMMAND_ID (wParam,lParam);
			UINT cmd_cmd = GET_WM_COMMAND_CMD (wParam,lParam);

			switch (cmd_id)
			{
			case IDOK:
				EndDialog (hWndDlg, cmd_id);
				return TRUE;
			case IDCANCEL:
				EndDialog (hWndDlg, cmd_id);
				return TRUE;
			default:
				break;
			}
		}
		break;

	default:
		break;
	}

	UNUSED(lParam);

	return FALSE;
}

/**
 * Name: OpenNewDevice(long newDeviceType)
 * Desc: Creates a new device layer for the given device type
 *		 and local id
**/
void OpenNewDevice(long newDeviceType, int id)
{
	// TODO: There might be a bug (?) in DASYLab that
	//		 does not let the following occur
	//if(deviceLayer != NULL)
	//	if (deviceLayer->IsOpen())
	//	{
	//		deviceLayer->CleanUp();
	//		delete deviceLayer;
	//	}

	//// Create the device layer and buffer, allowing the device layer to set the
	//// hardware specific information
	//deviceLayer = new LabJackLayer(infoStruct, newDeviceType);
	
	// TODO: This is bad bad form
	deviceLayer->OpenDevice(newDeviceType, id);
}

/**
 * Name: OpenNewEthernetDevice
 * Desc: Instructs the device layer to open a device of deviceType
 *		 via ethernet at the given address
**/
void OpenNewEthernetDevice(long newDeviceType, CString adr)
{
	deviceLayer->OpenEthernetDevice(newDeviceType, adr);
}

/**
 * Name: GetDeviceType()
 * Desc: Returns the device type curently in use by
 *		 the LabJackLayer
**/
long GetDeviceType()
{
	return deviceLayer->GetDeviceType();
}

/**
 * Name: IsUsingEthernet()
 * Desc: Returns true if the device is connected by ethernet
 *		 or false otherwise.
**/
bool IsUsingEthernet()
{
	return deviceLayer->IsUsingEthernet();
}

/**
 * Name: GetID()
 * Desc: Returns the local id of the device in use by DASYLab or
 *		 null if a device is not in use by USB
**/
int GetID()
{
	return deviceLayer->GetDeviceID();
}

/**
 * Name: GetIPAddress
 * Desc: Get the string IP address of the device currently in use
 *		 by DASYLab or null if ethernet is not in use
**/
CString GetIPAddress()
{
	return deviceLayer->GetIPAddress();
}

/**
 * Name: ToCharArray(int x)
 * Desc: Helper function that converts an integer to a string
**/
char * ToCharArray(int x)
{
	char buffer[30];
	sprintf(buffer, "%d", x);
	return buffer;
}

/**
 * Name: ToCString(int x)
 * Desc: Helper function to convert an integer to a string
**/
CString ToCString(int x)
{
	CString returnString;
	returnString.Format("%i", x);
	return returnString;
}

/* MFC Required Portions . . . thanks MFC */
BEGIN_MESSAGE_MAP(CLabJackDasyApp, CWinApp)
END_MESSAGE_MAP()


// CLabJackDasyApp construction

CLabJackDasyApp::CLabJackDasyApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CLabJackDasyApp object

CLabJackDasyApp theApp;


// CLabJackDasyApp initialization

BOOL CLabJackDasyApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
