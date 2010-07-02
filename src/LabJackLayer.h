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

// File
#include <fstream>

//	Compiler
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <map>

//	DASYLab driver interface
#include "treiber.h"

/**
 * Name: LabJackLayer
 * Desc: Class to abstract and manage control of LabJack and its communication
 *		 with DASYLab through its buffers.
**/
#ifndef LABJACKLAYER_H
#define LABJACKLAYER_H

using namespace std;

typedef std::map <long, int> calMapType;

class LabJackLayer {
		
		// Constants
		const static int START_STREAM_FREQUENCY = 100;	// Start streaming at 100 Hz
		const static int HV_CHANNELS = 4;
		const static int MAX_SCANS_PER_SECOND = 50000;
		const static DWORD DEFAULT_BUFFER_SIZE = 4096;	// Default buffer size (bytes)
		const static int ANALOG = 1;
		const static int DIGITAL = 2;
		const static int MAX_BIT_VALUE = 65534;
		const static int MIN_BIT_VALUE = 0;
		const static int CHANNEL_RESOLUTION = 32768;

		// Instance variables
		DWORD aoStoreIndex;								// The index of the next available position for analog ouput in
														// DASYLab's buffer
		DWORD doStoreIndex;								// The index of the next available position for digital output
														// in DASYLab's buffer
		DWORD inputStoreIndex;								// The index of the next available position for analog input
														// in DASYLab's buffer
		DRV_INFOSTRUCT * infoStruct;					// Pointer to DASYLab's information structure
		DWORD aoBufferSize;								// Analog output buffer size
		DWORD doBufferSize;								// Digital output buffer size
		DWORD aiRetrieveIndex;							// Index of the next index of input to be processed
		bool wrapAround;								// For circular buffer
		LPSAMPLE aoBufferAdr;							// Analog output buffer address
		LPSAMPLE inputBufferAdr;						// Analog/digital input buffer address
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
		double calConstants[64];
		short GAIN_INFO[8];								// TODO: Need config
		UINT hTimerID;									// TODO: This might need to be static?
		calMapType posSlopeConstLocations;				// Maps LabJack range values to their locations in the cal constants
		calMapType negSlopeConstLocations;	
		calMapType centerConstLocations;
		calMapType dasyLJGainCodes;						// Maps a DASYLab "real gain" to the gain code used by the UD driver
		//double debugValue;
		//ofstream debugFile;
		int localID;									// The local id of the device that this LabJackLayer wraps
		CString ipAddress;								// The IP address of a UE device opened, if applicable. null otherise
		bool isUsingEthernet;							// Indicates if the device is connected by ethernet

	public:
		LabJackLayer(DRV_INFOSTRUCT * structAddress);
		//~LabJackLayer(void);
		void AdvanceInputBuf();
		void AdvanceAnalogOutputBuf();
		void AdvanceDigitalOutputBuf();
		LPSAMPLE GetAnalogOutputBuf();
		bool GetAnalogOutputStatus();
		LPSAMPLE GetDigitalOutputBuf();
		bool GetDigitalOutputStatus();
		LPSAMPLE GetInputBuf();
		bool GetInputStatus();
		DRV_MEASINFO * GetMeasInfo();
		bool IsOpen();
		long GetError();
		void CleanUp();
		void AllocateAOBuffer(UDWORD nSamples);
		void SetDigitalOutputBufferMode(DWORD numSamples, DWORD startDelay);
		bool AllocateInputBuffer(DWORD size);
		bool IsMeasuring();
		void SetDeviceType(int type);
		void BeginExperiment();
		void StopExperiment();
		bool ConfirmDataStructure();
		void StreamCallback(long scansAvailable, double userValue);
		void SetError(DWORD newError);
		bool IsFrequencyValid();
		bool RequiresStreaming();
		void CommandResponseCallback();
		void WriteDigitalOutput(UINT chan, DWORD outVal);
		void WriteDAC(UINT chan, DWORD outVal);
		void OpenDevice(long deviceType, int id); // TODO: This is bad form
		long GetDeviceType();
		bool IsUsingEthernet();
		void OpenEthernetDevice(long newDeviceType, CString value);
		long ConvertToUDRange(long dasyLabRangeCode);
		CString GetIPAddress();
		int GetDeviceID();

		// Public constants
		const static int LABJACK_ERROR_PREFIX = 5000;	// Starting error number so that DASYLab does
														// not confuse LabJack errors with its internal
														// errors

	private:
		void FillInfoStructure();
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
		bool CheckBitHigh(double value, int position);
		void StartStreaming();
		void StartCommandResponse();
		bool InstallTimerInterruptHandler();
		void RemoveTimerInterruptHandler();
		void AddToInputBuffer(SAMPLE newValue);
		void AddToInputBuffer(SAMPLE * newValue);
		double ConvertAOValue(DWORD value, UINT channel);
		void ConfigureRange();
		//void MapCalConstants();
		double CalMaxAIValue(int channel);
		double CalMinAIValue(int channel);
};
#endif