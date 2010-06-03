/**
 * Name: MemoryManagement.h
 * Desc: Basic memory management functions for LabJackDasy
**/

// Windows includes
#include "stdafx.h" 
#include <windowsx.h>

//	DASYLab driver interface
#include "treiber.h"

void FreeLockedMem (LPSAMPLE bufferadr);
LPSAMPLE AllocLockedMem (DWORD nSamples, DRV_INFOSTRUCT * infoStruct);