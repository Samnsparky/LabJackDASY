/**
 * Copyright (c) 2010 LabJack Corp.
 * See License.txt for more information
 *
 * Name: LabJackDasy.h
 * Desc: A layer that provides entry points for
 *		 and simplifies interactions with DASYLab
**/

//	DASYLab driver interface
#include "treiber.h"

// Windows
#include "windef.h"

/** Required DASYLab entry points **/
#ifdef  __cplusplus
extern "C"
{
#endif 

const int NONE_TYPE = -1; // No device type suitable

/** Prototypes **/
int _stdcall DRV_AdvanceAnalogOutputBuf();
int _stdcall DRV_AdvanceCounterOutputBuf();
int _stdcall DRV_AdvanceDigitalOutputBuf();
int _stdcall DRV_AdvanceInputBuf();
char* _stdcall DRV_ExplainGainCode(SCAN_ENTRY chan, UINT gainCode);
LPSAMPLE _stdcall DRV_GetAnalogOutputBuf();
int _stdcall DRV_GetAnalogOutputStatus();
LPSAMPLE _stdcall DRV_GetCounterOutputBuf();
int _stdcall DRV_GetCounterOutputStatus();
LPSAMPLE _stdcall DRV_GetDigitalOutputBuf();
int _stdcall DRV_GetDigitalOutputStatus();
HW_COMPONENT* _stdcall DRV_GetHardwareDescription();
LPSAMPLE _stdcall DRV_GetInputBuf();
int _stdcall DRV_GetInputBufStatus();
DRV_MEASINFO* _stdcall DRV_GetMeasInfoEx();
int _stdcall DRV_InitDevice(DRV_INFOSTRUCT *infoStruct);
int _stdcall DRV_KillDevice();
int _stdcall DRV_LoadWorksheet(const char *name);
int _stdcall DRV_NewWorksheet();
DWORD _stdcall DRV_ReadCounterInput(UINT ch);
DWORD _stdcall DRV_ReadDigitalInput(UINT ch);
int _stdcall DRV_SaveWorksheet(const char *name);
int _stdcall DRV_SetAnalogOutputBufferMode(UINT mode, DWORD numSamples, DWORD startDelay);
int _stdcall DRV_SetCounterOutputBufferMode(UINT mode, DWORD numSamples, DWORD startDelay);
int _stdcall DRV_SetDigitalOutputBufferMode(UINT mode, DWORD numSamples, DWORD startDelay);
int _stdcall DRV_SetInputBufferSize(DWORD Size);
int _stdcall DRV_SetScanList(UINT scanListLength, UINT scanListMaxGroupSize, SCAN_ENTRY *scanList);
int _stdcall DRV_ShowDialog(UINT BoxNr, DWORD ExtraPara);
int _stdcall DRV_ShowError();
int _stdcall DRV_StartMeas();
int _stdcall DRV_StopMeas();
int _stdcall DRV_TestStruct();
int _stdcall DRV_WriteAnalogOutput(UINT chan, DWORD outVal);
int _stdcall DRV_WriteCounterOutput(UINT chan, DWORD outVal);
int _stdcall DRV_WriteDigitalOutput(UINT chan, DWORD outVal);
BOOL _stdcall DlgCardDef (HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Helper functions not exported to DASYLab
void StreamCallbackWrapper(long scansAvailable, double userValue);
void CALLBACK CommandResponseCallbackWrapper(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
void OpenNewDevice(long newDeviceType, int id);
void OpenNewEthernetDevice(long newDeviceType, CString value);
long GetDeviceType();
bool IsUsingEthernet();
int GetID();
CString GetIPAddress();
char * ToCharArray(int x);
CString ToCString(int x);

#ifdef  __cplusplus
} // extern C
#endif

// CLabJackDasyApp . . . Thanks MFC
// See LabJackDasy.cpp for the implementation of this class
//

class CLabJackDasyApp : public CWinApp
{
public:
	CLabJackDasyApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};