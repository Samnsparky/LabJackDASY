/**
 * Copyright (c) 2010 LabJack Corp.
 * See License.txt for more information
 *
 * Name: LabJackDasy.cpp
 * Desc: A layer that provides entry points for
 *		 and simplifies interactions with DASYLab
**/

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
#include "AboutDialog.h"
#include "DeviceSetupDialog.h"

//	LabJack
#include "c:\program files\labjack\drivers\LabJackUD.h" // TODO: needs to be flexible

/** Global variables **/
// TODO: Need to get rid of these distasteful global variables
LabJackLayer * deviceLayer;
const char DRIVER_NAME [] = "LabJackDASY";
const char DASY_DRIVER_VERSION [] = "0.1//05.10";	// Version/Date as required by DASYLab
const double DEFAULT_FREQUENCY = 10;		// The default frequency for input (Hz)
const DWORD DEFAULT_BLOCK_SIZE = 4;			// The default size allocated for readings (bytes)
//DeviceSetupDialog * deviceDialog;
DWORD StartTime;							// for start time of measure
HANDLE hInst = NULL;						// handle to previous WINDOWS-instance

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
 * Name: DRV_ExplainGainCode()
 * Desc: Returns a string explanation of the given range for the given channel and code
**/
char* _stdcall DRV_ExplainGainCode(SCAN_ENTRY chan, UINT gainCode)
{
	UNUSED (chan);

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
int _stdcall DRV_InitDevice(DRV_INFOSTRUCT *infoStruct)
{
	// Define the driver identification information not related to the device
	_fstrcpy(infoStruct->DriverName, DRIVER_NAME);
	_fstrcpy(infoStruct->DLL_Version, DASY_DRIVER_VERSION);
	_fstrcpy(infoStruct->VxD_Version, DASY_DRIVER_VERSION);
	infoStruct->DriverIdCode = DRV_ID_FREEWARE;	// or DRV_ID_OEM

	// Set experiment defaults
	infoStruct->AI_Frequency = DEFAULT_FREQUENCY;
	infoStruct->ADI_BlockSize = DEFAULT_BLOCK_SIZE;
	
	// Create the device layer and buffer, allowing the device layer to set the
	// hardware specific information
	deviceLayer = new LabJackLayer(infoStruct, LJ_dtU6); // TODO: Let user choose device type (!)
	
	// Create setup dialog 
	//deviceDialog = new DeviceSetupDialog(NULL);
	//deviceDialog->SetDeviceLayer(deviceLayer);

	// Return value according to errors generated
	if (deviceLayer->GetError())
		return DRV_FUNCTION_FALSE;
	else
		return DRV_FUNCTION_OK;
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
		//delete deviceLayer;
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
	switch(boxNum)
	{
	case DRV_DLG_ABOUT:
	{
		// NOTE: Using block here becuase of the AboutDialog instance
		AboutDialog aboutDialog;
		aboutDialog.ShowWindow(SW_SHOW);
		aboutDialog.UpdateWindow();
		break;
	}
	case DRV_DLG_CARD_DEF:
		//deviceDialog->ShowWindow(SW_SHOW);
		//deviceDialog->UpdateWindow();
		break;
	}

	return DRV_FUNCTION_OK;
}

/**
 * Name: DRV_ShowError()
 * Desc: Displays a basic message box with an error encountered
**/
int _stdcall DRV_ShowError()
{
	char err[255];

	ErrorToString(deviceLayer->GetError(), err);

	MessageBox (GetActiveWindow (), err, "LabJack Error", MB_OK | MB_ICONSTOP);
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
	deviceLayer->BeginExperiment((long)(&StreamCallback));
    
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
	// TODO: Finish this stub (0.1)
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
 * Note: This is a stub that will be implemented for Alpha 0.1
**/
int _stdcall DRV_WriteDigitalOutput(UINT chan, DWORD outVal)
{
	// TODO: Finish this stub (0.1)
	return DRV_FUNCTION_OK;
}

} // extern "C"

/**
 * Name: StreamCallback
 * Desc: Wrapper function that provides an entry point for the UD driver
 *		 to call the StreamCallback function on the device layer
**/
void StreamCallback(long scansAvailable, double userValue)
{
	deviceLayer->StreamCallback(scansAvailable, userValue);
}