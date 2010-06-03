/**
 * Copyright (c) 2010 LabJack Corp.
 * See License.txt for more information
 *
 * Name: LabJackLayer.h
 * Desc: Header file for LabJackLayer object class
**/

//	Windows
#include "stdafx.h" 
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

//	Compiler
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

//	DASYLab driver interface
#include "treiber.h"

/**
 * Name: LabJackLayer
 * Desc: Class to abstract and manage control of LabJack and its communication
 *		 with DASYLab through its buffers.
**/
#ifndef LABJACKLAYER_H
#define LABJACKLAYER_H
class LabJackLayer {
	
		DWORD aoStoreIndex;								// The index of the next available position for analog ouput in
														// DASYLab's buffer
		DWORD doStoreIndex;								// The index of the next available position for digital output
														// in DASYLab's buffer
		DWORD aiStoreIndex;								// The index of the next available position for analog input
														// in DASYLab's buffer
		DRV_INFOSTRUCT * infoStruct;					// Pointer to DASYLab's information structure
		DWORD aoBufferSize;								// Analog output buffer size
		DWORD doBufferSize;								// Digital output buffer size
		DWORD aiRetrieveIndex;							// Index of the next index of input to be processed
		bool wrapAround;								// For circular buffer
		LPSAMPLE aoBufferAdr;							// Analog output buffer address
		LPSAMPLE aiBufferAdr;							// Analog input buffer address
		LPSAMPLE doBufferAdr;							// Digital output buffer address
		bool analogBufferValid;							// flag for if the analog input buffer is full
		DRV_MEASINFO measInfo;							// DASYLab structure that keeps track of the status
														// of the experiment
		bool open;										// Flag for whether the device is currently in operation
		long deviceType;								// LabJack device type (LJ_dt)
		long lngHandle;									// LabJack device handle
		//DWORD maxRamSize = DEFAULT_BUFFER_SIZE;
		BOOL measRun;									// Are measuremente being taken
		int aoCounter;									// Number of analog output values sent
		int doCounter;									// Number of digital values sent
		int maxBlocks;
		DWORD doStartDelay;								// How long to wait (ms?) before starting digital output
		DWORD nSamples;									// Number of samples in buffer (taken from exmaple, still needed?)
		DWORD maxRamSize;
		int aiCount;									// Number of analog input readings read
		long retrieveIndex;								// Index of next available buffer element
		WORD aiChannel;									// Current channel being read
		int aoCount;									// Number of analog output values written
		int doCount;									// Number of digital output values written
		DWORD startTime;								// Starting time of DASYLab experiment
		bool isStreaming;								// Indicates if we are streaming (TRUE) or using command-response (FALSE)
		int numAINRequested;							// The number of analog input channels we are polling/streaming
		int numDIRequested;								// The number of digital input channels we are polling/streaming
		int smallestChannel;							// The lowest channel number that we are polling/streaming
		int analogInputScanList[32];					// list of analog channel numbers to acquire
		int digitalInputScanList[32];					// list of digital channel numbers to acquire
		int smallestChannelType;						// ANALOG or DIGITAL
														// TODO: This ought to be an enumerated type :)

		const static DWORD DEFAULT_BUFFER_SIZE = 4096;	// Default buffer size (bytes)
		const static int ANALOG = 1;
		const static int DIGITAL = 2;
		short GAIN_INFO[8];								// TODO: Need config

	public:
		LabJackLayer(DRV_INFOSTRUCT * structAddress, long newDeviceType);
		//~LabJackLayer(void);
		void AdvanceAnalogInputBuf();
		void AdvanceAnalogOutputBuf();
		void AdvanceDigitalOutputBuf();
		void AdvanceInputBuf();
		LPSAMPLE GetAnalogOutputBuf();
		bool GetAnalogOutputStatus();
		LPSAMPLE GetDigitalOutputBuf();
		bool GetDigitalOutputStatus();
		LPSAMPLE GetAnalogInputBuf();
		bool GetAnalogInputStatus();
		DRV_MEASINFO * GetMeasInfo();
		bool IsOpen();
		long GetError();
		void CleanUp();
		void AllocateAOBuffer(UDWORD nSamples);
		void SetDigitalOutputBufferMode(DWORD numSamples, DWORD startDelay);
		bool AllocateAIBuffer(DWORD size);
		bool IsMeasuring();
		void SetDeviceType(int type);
		void BeginExperiment(long streamCallback);
		void StopExperiment();
		bool ConfirmDataStructure();
		void StreamCallback(long scansAvailable, double userValue);
		void SetError(DWORD newError);

	private:
		void FillinfoStructure();
		void FillU3Info();
		void FillU6Info();
		void FillUE9Info();
		void KillBuffer(LPSAMPLE & addr);
		void ErrorHandler(long lngErrorcode);
		bool IsRequestingAIN(int channel);
		bool IsRequestingDI(int channel);
		SAMPLE ConvertAIValue(double value, UINT channel);
		void FreeLockedMem (LPSAMPLE bufferadr);
		LPSAMPLE AllocLockedMem (DWORD nSamples, DRV_INFOSTRUCT * infoStruct);
};
#endif