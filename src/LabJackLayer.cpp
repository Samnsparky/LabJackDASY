/**
 * Copyright (c) 2010 LabJack Corp.
 * See License.txt for more information
 *
 * Name: LabJackLayer.cpp
 * Desc: An object that acts as a thin software layer between
 *		 LabJackDASY and the UD driver
**/

/** Includes **/

// Debug
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// Windows
#include "stdafx.h"
#include <windows.h>

// Timer
#include<mmsystem.h>
#pragma comment(lib, "winmm.lib")

// Bitset
#include <bitset>

//	LabJack
#include "c:\program files\labjack\drivers\LabJackUD.h" // TODO: needs to be flexible

// Class header file
#include "LabJackLayer.h"

// Application
#include "LabJackDasy.h"

using namespace std;

/**
 * Name: LabJackLayer(DRV_infoStruct *StructAddress, long newDeviceType)
 * Desc: Constructor for LabJackLayer that saves the given DASYLab structure to infoStruct
 * Args: StructAddress, the address of the DASYLab infoStruct
**/
LabJackLayer::LabJackLayer(DRV_INFOSTRUCT * structAddress)
{
	debugValue = (MAX_BIT_VALUE/2.0);

	// Put in some default values
	aiRetrieveIndex = 0;
	wrapAround = FALSE;
	aoStoreIndex = 0;
	inputStoreIndex = 0;
	analogBufferValid = FALSE;
	open = FALSE;
	measRun = FALSE;
	isStreaming = FALSE;
	open = FALSE;

	// Save the structure address and device type
	infoStruct = structAddress;

	// Issue 4: Command-response does not work until the experiment is started for a second time.
	// TODO: This is a somewhat dirty fix that, if possible, should be addressed with timer
	//		 configuration changes. (0.2)
    InstallTimerInterruptHandler();
	RemoveTimerInterruptHandler();

	SetError(0);
}

/**
 * Name: AdvanceAnalogOutputBuf()
 * Desc: Changes aoStoreIndex to the next block for AO output
**/
void LabJackLayer::AdvanceAnalogOutputBuf()
{
	aoCounter += infoStruct->AO_BlockSize;
	aoStoreIndex += infoStruct->AO_BlockSize;

	if ( aoStoreIndex + infoStruct->AO_BlockSize > aoBufferSize )
		aoStoreIndex = 0;
}

/**
 * Name: AdvanceDigitalOutputBuf()
 * Desc: Changes doStoreIndex to the next block for DO output
**/
void LabJackLayer::AdvanceDigitalOutputBuf()
{
	doStoreIndex += infoStruct->DO_BlockSize;

	if ( doStoreIndex + infoStruct->DO_BlockSize > doBufferSize )
		doStoreIndex = 0;
}

/**
 * Name: AdvanceDigitalOutputBuf()
 * Desc: Moves retrieve index one block forward for intermediate buffer
**/
void LabJackLayer::AdvanceInputBuf()
{
	// mark processed data - one block processed
	aiRetrieveIndex += infoStruct->ADI_BlockSize;

	if (aiRetrieveIndex == infoStruct->DriverBufferSize)
	{
		aiRetrieveIndex = 0;
		wrapAround = TRUE;
	}

	if (maxBlocks && !--maxBlocks)
	{
		StopExperiment();
		maxBlocks = (DWORD) -1L;
	}
}

/**
 * Name: GetAnalogOutputBuf()
 * Desc: Returns a pointer to a buffer to new AO data
**/
LPSAMPLE LabJackLayer::GetAnalogOutputBuf()
{
	return aoBufferAdr + aoStoreIndex;
}

/**
 * Name: DRV_GetAnalogOutputStatus()
 * Desc: Test if there is space to place a block of output data
**/
bool LabJackLayer::GetAnalogOutputStatus()
{
	return !(( aoBufferSize == 0 || infoStruct->AO_BlockSize == 0 || aoBufferAdr == NULL )
		&& ( aoCounter + infoStruct->AO_BlockSize > aoBufferSize ));
}

/**
 * Name: GetDigitalOutputBuf()
 * Desc: Returns a pointer to a buffer for new digital output data
**/
LPSAMPLE LabJackLayer::GetDigitalOutputBuf()
{
	return doBufferAdr + doStoreIndex;
}

/**
 * Name: DRV_GetDigitalOutputStatus()
 * Desc: Test if there is space to place a block of output data
**/
bool LabJackLayer::GetDigitalOutputStatus()
{
	if ( doBufferSize == 0 || infoStruct->DO_BlockSize == 0 || doBufferAdr == NULL )
		return FALSE;

	if ( doCounter + infoStruct->DO_BlockSize > doBufferSize )
		return FALSE;

	return TRUE;
}

/**
 * Name: DRV_GetInputBuf()
 * Desc: Return a pointer to a buffer for new input data
**/
LPSAMPLE LabJackLayer::GetInputBuf()
{
	LPSAMPLE addr;

	if (analogBufferValid)
	{
		/* get adress of data */
		addr = ((LPSAMPLE) inputBufferAdr) + aiRetrieveIndex;
	}
	else
	{
		addr = 0l;
	}

	return addr;
}

/**
 * Name: GetInputStatus()
 * Desc: Test if input data are waiting to be processed
**/
bool LabJackLayer::GetInputStatus()
{
	long delta;

	if (maxBlocks == -1L)
		return FALSE;

	delta = inputStoreIndex - aiRetrieveIndex;

	if ( wrapAround )
	{
		delta += infoStruct->DriverBufferSize;
	}

	measInfo.ADI_PercentFull = ( 100L * delta ) / infoStruct->DriverBufferSize;

	analogBufferValid = ( delta >= (long) infoStruct->ADI_BlockSize );

	return analogBufferValid;
}

/**
 * Name: GetMeasInfo()
 * Desc: Return measInfo, the DASYLab structure that holds the status of the experiment
**/
DRV_MEASINFO * LabJackLayer::GetMeasInfo()
{
	return &measInfo;
}

/**
 * Name: IsOpen()
 * Desc: Return TRUE if the device is open and false otherwise
**/
bool LabJackLayer::IsOpen()
{
	return open;
}

/**
 * Name: GetError()
 * Desc: Return the LabJack errorcode of the most recent error encountered or 0
 *		 if no errors have occured
**/
long LabJackLayer::GetError()
{
	return infoStruct->Error;
}

/**
 * Name: FillInfoStructure()
 * Desc: (private) fill the information structure with hardware specific information
**/
void LabJackLayer::FillInfoStructure()
{
	int n;
	calMapType::iterator calIt;

	// Map the calibration constants
	//MapCalConstants();

	// Frequency and features
	infoStruct->Features = SUPPORT_DEFAULT | SUPPORT_OUT_ALL;
	infoStruct->SupportedAcqModes = DRV_AQM_CONTINUOUS | DRV_AQM_STOP;
	infoStruct->MaxFreq = 50000.0;
	infoStruct->MinFreq = 0.0001;
	infoStruct->MaxFreqPerChan = 50000.0;
	infoStruct->MinFreqPerChan = 0.00001;

	// Device general channel specific settings
	infoStruct->Max_AO_Channel = 2; // DAC0 and DAC1
	infoStruct->Max_CT_Channel = 0; // TODO: Counters have not yet been implemented
	infoStruct->DIO_Width = 1; // 1 bit per channel

	// Set block size to 1 for most responsiveness
	infoStruct->ADI_BlockSize = 1;

	for (n = 0; n < infoStruct->Max_AO_Channel; n++)
	{
		infoStruct->AO_ChInfo[n].OutputRange_Min = 0;
		infoStruct->AO_ChInfo[n].OutputRange_Max = 5;
		infoStruct->AO_ChInfo[n].Resolution = CHANNEL_RESOLUTION;		/* == 12 Bit */
	}

	// Channel info?
	//_fmemset (infoStruct->AI_ChInfo, 0, sizeof (infoStruct->AI_ChInfo));
	//_fmemset (infoStruct->DI_ChInfo, 0, sizeof (infoStruct->DI_ChInfo));
	//_fmemset (infoStruct->CT_ChInfo, 0, sizeof (infoStruct->CT_ChInfo));
	//_fmemset (infoStruct->AO_ChInfo, 0, sizeof (infoStruct->AO_ChInfo));
	//_fmemset (infoStruct->DO_ChInfo, 0, sizeof (infoStruct->DO_ChInfo));

	//infoStruct->HelpFileName; // I don't think Vista/7 even supports this!
	//infoStruct->HelpIndex = 0;

	// Fill device model specific information
	switch(deviceType)
	{
		case LJ_dtU3:
			FillU3Info();
			break;
		case LJ_dtUE9:
			FillUE9Info();
			break;
		case LJ_dtU6:
			FillU6Info();
			break;
	}

	// Put gains into the information structure
	// TODO: Might be a nicer way to do this?
	n = 0;
	for ( calIt=dasyLJGainCodes.begin() ; calIt != dasyLJGainCodes.end(); calIt++ )
	{
		infoStruct->GainInfo[n] = calIt->first;
		n++;
	}
}

/**
 * Name: FillU3Info()
 * Desc: (private) Fills the DASYLab info structure wtih U3 specific information
**/
void LabJackLayer::FillU3Info()
{
	double dblValue;
	int n=0;

	// TODO: These ought to be constants
	infoStruct->Max_AI_Channel = 19; // LabJackLayer will map 16,17,18 to 30,31,32 respectively
	infoStruct->Max_DI_Channel = 20; // User will need to manage which line is out/in
	infoStruct->Max_DO_Channel = 20;

	// Determine if a HV or LV is present
	dblValue = 0;
	eGet(lngHandle, LJ_ioGET_CONFIG, LJ_chU3HV, &dblValue, 0);
	if(dblValue) // If HV
		for (n = 0; n < HV_CHANNELS; n++) 
		{
			infoStruct->AI_ChInfo[n].InputRange_Max = 10;
			infoStruct->AI_ChInfo[n].InputRange_Min = -10;
			infoStruct->AI_ChInfo[n].Resolution = CHANNEL_RESOLUTION;	  /* == 16 Bit */
			infoStruct->AI_ChInfo[n].BaseUnit = DRV_BASE_UNIT_2COMP;
		}
	for (; n < infoStruct->Max_AI_Channel; n++)
	{
		infoStruct->AI_ChInfo[n].InputRange_Max = 2.5;
		infoStruct->AI_ChInfo[n].InputRange_Min = -2.5;
		infoStruct->AI_ChInfo[n].Resolution = CHANNEL_RESOLUTION;	  /* == 16 Bit */
		infoStruct->AI_ChInfo[n].BaseUnit = DRV_BASE_UNIT_2COMP;
	}

	// TODO: Fill gains
}

/**
 * Name: FillU6Info()
 * Desc: (private) Fills the DASYLab info structure wtih U6 specific information
**/
void LabJackLayer::FillU6Info()
{
	//double dblValue;
	int n;

	// TODO: These ought to be constants
	infoStruct->Max_AI_Channel = 16;
	infoStruct->Max_DI_Channel = 23; // User will need to manage which line is out/in
	infoStruct->Max_DO_Channel = 23;

	for (n=0; n < infoStruct->Max_AI_Channel; n++) 
	{
		infoStruct->AI_ChInfo[n].InputRange_Max = 10;//CalMaxAIValue(n);
		infoStruct->AI_ChInfo[n].InputRange_Min = -10;//CalMinAIValue(n);
		infoStruct->AI_ChInfo[n].Resolution = CHANNEL_RESOLUTION;	  /* == 16 Bit */
		infoStruct->AI_ChInfo[n].BaseUnit = DRV_BASE_UNIT_2COMP;
	}

	// Insert gains
	dasyLJGainCodes.clear();
	dasyLJGainCodes.insert(pair<long, long>(1, LJ_rgBIP10V));
	dasyLJGainCodes.insert(pair<long, long>(10, LJ_rgBIP1V));
	dasyLJGainCodes.insert(pair<long, long>(100, LJ_rgBIPP1V));
	dasyLJGainCodes.insert(pair<long, long>(1000,LJ_rgBIPP01V));
}

/**
 * Name: FillUE9Info()
 * Desc: (private) Fills the DASYLab info structure wtih U6 specific information
**/
void LabJackLayer::FillUE9Info()
{
	int n;

	// TODO: These ought to be constants
	infoStruct->Max_AI_Channel = 22; // LabJackLayer will map 16, 17, 18, 19, 20, 21 to
									 // 128, 132, 133, 136, 140, 141 respectively
	infoStruct->Max_DI_Channel = 23; // User will need to manage which line is out/in
	infoStruct->Max_DO_Channel = 23;

	// Put in the analog input ranges
	for (n=0; n < infoStruct->Max_AI_Channel; n++) 
	{
		infoStruct->AI_ChInfo[n].InputRange_Max = 5;
		infoStruct->AI_ChInfo[n].InputRange_Min = -5;
		infoStruct->AI_ChInfo[n].Resolution = CHANNEL_RESOLUTION;	  /* == 16 Bit */
		infoStruct->AI_ChInfo[n].BaseUnit = DRV_BASE_UNIT_2COMP;
	}

	// Fill gain codes
	// TODO: 0-5V range
	dasyLJGainCodes.clear();
	dasyLJGainCodes.insert(pair<long, long>(1, LJ_rgBIP5V));
	dasyLJGainCodes.insert(pair<long, long>(2, LJ_rgUNI2P5V));
	dasyLJGainCodes.insert(pair<long, long>(4, LJ_rgUNI1P25V));
	dasyLJGainCodes.insert(pair<long, long>(8, LJ_rgUNIP625V));
}

/**
 * Name: CleanUp()
 * Desc: Frees up the device and buffers used for DASYLab
**/
void LabJackLayer::CleanUp()
{
	// Clean up the buffers
	//maxRamSize = 0;
	KillBuffer(inputBufferAdr);
	delete inputBufferAdr;
	KillBuffer(aoBufferAdr);
	delete aoBufferAdr;
	KillBuffer(doBufferAdr);
	delete doBufferAdr;

	// Mark the device closed
	open = FALSE;

	// Close through UD driver
	Close();
}

/**
 * Name: KillBuffer(LPSAMPLE & addr)
 * Desc: (private) Cleans up the buffer at the given address
**/
void LabJackLayer::KillBuffer(LPSAMPLE & addr)
{
	if ( addr != NULL && ! measRun )
	{
		FreeLockedMem ( addr );
		addr = NULL;
	}
}

/**
 * Name: AllocateAOBuffer(DWORD nSamples)
 * Desc: Ensures that the analog output buffer for DASYLab is sufficiently large
**/
void LabJackLayer::AllocateAOBuffer(DWORD nSamples)
{
	if ( infoStruct->AO_BlockSize > 1 )
	{
		/* Round to next multiple of AO_BlockSize */
		nSamples += infoStruct->AO_BlockSize / 2;
		nSamples /= infoStruct->AO_BlockSize;
		nSamples *= infoStruct->AO_BlockSize;
	}

	if ( aoBufferSize != nSamples )
	{
		FreeLockedMem ( aoBufferAdr );
		aoBufferAdr = AllocLockedMem ( nSamples, infoStruct );
		aoBufferSize = nSamples;
	}
}

/**
 * Name: SetDigitalOutputBufferMode()
 * Desc: Sets the mode, size, and starting delay for DO and its buffer
**/
void LabJackLayer::SetDigitalOutputBufferMode(DWORD numSamples, DWORD startDelay)
{
	doStartDelay = startDelay;

	if ( infoStruct->DO_BlockSize > 1 )
	{
		// Round to next multiple of DO_BlockSize
		nSamples += infoStruct->DO_BlockSize / 2;
		nSamples /= infoStruct->DO_BlockSize;
		nSamples *= infoStruct->DO_BlockSize;
	}

	if ( doBufferSize != nSamples )
	{
		FreeLockedMem ( doBufferAdr );
		doBufferAdr = AllocLockedMem ( nSamples, infoStruct );
		doBufferSize = nSamples;
	}

}

/**
 * Name: AllocateInputBuffer(DWORD nSamples)
 * Desc: Ensures that the analog/digital input buffer for DASYLab is sufficiently large
 * Note: DASYLab's example checked to see if inputBufferAdr is null after allocating
 *		 memory to see if there is enough and, thus, this method returns bool. 
 *		 It was not present in the others and might not be needed though.
**/
bool LabJackLayer::AllocateInputBuffer(DWORD size)
{
	KillBuffer (inputBufferAdr);

	inputBufferAdr = AllocLockedMem ( size*2, infoStruct );

	// TODO: Didn't need it for the others...
	if ( inputBufferAdr == NULL )
	{
		infoStruct->DriverBufferSize = infoStruct->ADI_BlockSize;
		infoStruct->Error = DRV_ERR_NOTENOUGHMEM;
		return FALSE;
	}

	maxRamSize = size * sizeof (SAMPLE);

	infoStruct->DriverBufferSize = size;
	
	return TRUE;
}

/**
 * Name: IsMeasuring()
 * Desc: Returns true if the device is currently being used in a DASYLab experiment
 *		 or false otherwise
**/
bool LabJackLayer::IsMeasuring()
{
	return measRun;
}

/**
 * Name: SetDeviceType(int type)
 * Desc: Sets the LabJackLayer to looks for a given device type as
 *		 defined in the LabJack header file
**/
void LabJackLayer::SetDeviceType(int type)
{
	deviceType = type;
}

/**
 * Name: BeginExperiment()
 * Desc: Sets up DASYLab information structure and deteremines scan list
 * Args: callbackFunction: the pointer (cast as a long) of the function to have the UD
 *						   driver call. Should be a wrapper to this object's callback
 *						   function.
 * Retn: True is successful and false otherwise
 **/
void LabJackLayer::BeginExperiment()
{
	debugFile.open("C:\\Documents and Settings\\Owner\\Desktop\\DasyLabTest\\sample1.txt");

	// Check that the device is capable of the desired frequency
	if(!IsFrequencyValid())
	{
		MessageBox (GetActiveWindow (), "Whoops! Your LabJack is not capable of your desired frequency. Please see section 3.2 of the User's Guide for more information.", "LabJack Error", MB_OK | MB_ICONSTOP);
		measRun = FALSE;
		return;
	}
	
	if (infoStruct->AcquisitionMode == DRV_AQM_STOP)
		maxBlocks = infoStruct->MaxBlocks;
	else
		maxBlocks = 0;

	// (re-)set vars for buffer handling
	wrapAround = FALSE;
	aiRetrieveIndex = 0;
	inputStoreIndex = 0;

	aiChannel = 0;
	aoCount = 0;
	doCount = 0;
	aiCount = 0;

	// store start time
	startTime = GetCurrentTime();

	// Check to see if any channels are being used
	if (numAINRequested == 0 && numDIRequested == 0)
		return;

	// Configure the range
	ConfigureRange();

	// TODO: A more empirical approach to determining which mode would
	//       be more efficient
	// Start streaming / command response loop
	if (infoStruct->AI_Frequency < START_STREAM_FREQUENCY)
		StartCommandResponse();
	else
		StartStreaming();
}

/**
 * Name: StopExperiment()
 * Desc: Updates the information structure for DASYLab and stops
 *		 LabJack streaming if applicable
**/
void LabJackLayer::StopExperiment()
{
	long lngErrorcode;

	debugFile.close();

	if(isStreaming)
	{
		//Stop the stream
		lngErrorcode = eGet(lngHandle, LJ_ioSTOP_STREAM, 0, 0, 0);
		ErrorHandler(lngErrorcode);
		isStreaming = FALSE;
	}
	else
		RemoveTimerInterruptHandler();

	if (measRun)
		measRun = FALSE;

	maxBlocks = 0;
}

/**
 * Name: ConfirmDataStructure()
 * Desc: Verifies the validity of the information strcture for DASYLab
 * Note: mostly copied from TestStruct() in Demo.c from DASYLab
**/
// TODO: This needs some clean up
bool LabJackLayer::ConfirmDataStructure()
{
	DWORD timeBase;
	DWORD minBuffer;
	DWORD blockSize;
	int c, i;						  /* for-loop variables */

	/* initialization complete */
	if (!open)
	{
		infoStruct->Error = DRV_ERR_DEVICENOTINIT;
		return FALSE;
	}
	/* TestStruct nevver called during acquisition */
	if (measRun)
	{
		infoStruct->Error = DRV_ERR_MEASRUN;
		return FALSE;
	}
	/* When TestStruct called, all buffers must be allocated */
	if (inputBufferAdr == NULL)
	{
		infoStruct->Error = DRV_ERR_NOTENOUGHMEM;
		return FALSE;
	}
	/* check if buffersize not to big */
	if (infoStruct->DriverBufferSize * sizeof (SAMPLE) != maxRamSize)
	{
		infoStruct->Error = DRV_ERR_BUFSIZETOBIG;
		return FALSE;
	}
	/* check if any channel is active */
	/*if (numAINRequested == 0 && numDIRequested == 0)
	{
		infoStruct->Error = DRV_ERR_NOCHANNEL;
		return FALSE;
	}*/
	/* check rate-parameters */
	if (infoStruct->AI_Frequency > infoStruct->MaxFreq)
	{
		infoStruct->AI_Frequency = infoStruct->MaxFreq;
		infoStruct->Error = DRV_WARN_CHANGEFREQ;
		return FALSE;
	}
	if (infoStruct->AI_Frequency < infoStruct->MinFreq)
	{
		infoStruct->AI_Frequency = infoStruct->MinFreq;
		infoStruct->Error = DRV_WARN_CHANGEFREQ;
		return FALSE;
	}

	/* Round Frequency to nearest possible value */
	//timeBase = 100;	 /* Use 100 Hz MultiMedia interrupt as time base */

	//PacerRate = (DWORD) ( 0.5 + ( TimeBase / infoStruct->AI_Frequency ) );
	//if ( PacerRate < 1 )
	//	PacerRate = 1;

	//infoStruct->AI_Frequency = TimeBase / (double) PacerRate;

	/* Check for correct rates */
	if (infoStruct->DI_FreqRate <= 0)
		infoStruct->DI_FreqRate = 1;
	if (infoStruct->AO_FreqRate <= 0)
		infoStruct->AO_FreqRate = 1;
	if (infoStruct->DO_FreqRate <= 0)
		infoStruct->DO_FreqRate = 1;


	/* be sure, that the whole bufferlength is */
	/* a multiple of the blocksize								 */

	if (infoStruct->DriverBufferSize * sizeof (SAMPLE) != maxRamSize)
		AllocateInputBuffer (infoStruct->DriverBufferSize);
	blockSize = infoStruct->ADI_BlockSize * sizeof (SAMPLE);	  /* Recalculate from SAMPLES to BYTES */
	minBuffer = blockSize * (maxRamSize / blockSize);
	if (minBuffer != maxRamSize)
	{
		AllocateInputBuffer (minBuffer / sizeof (SAMPLE));
	}

	inputStoreIndex = 0l;
	//maxBufferIndex = maxRamSize / sizeof (SAMPLE);

	/* calculate count of active channels */
	// TODO: Variable 
	//AnzahlChannel = (WORD) (Num_AI_Channels ());
	//OutputChannel = (BOOL) (infoStruct->AO_Channel != (DWORD) 0);
	//DigitalInput = (BOOL) (infoStruct->DI_Channel != (DWORD) 0);
	//OutputDigital = (BOOL) (infoStruct->DO_Channel != (DWORD) 0);
	//Counter = (BOOL) (infoStruct->CT_Channel != (DWORD) 0);

	/* check and store which channels to poll in local array (access-time) */
	// TODO: Need constants!
	for (c = 0, i = 0; i < 32; i++)
	{
		if (IsRequestingAIN(i))
		{
			analogInputScanList[c] = i;
			c++;
		}
	}
	numAINRequested = c;

	for (c = 0, i = 0; i < 32; i++)
	{
		if (IsRequestingDI(i))
		{
			digitalInputScanList[c] = i;
			c++;
		}
	}
	numDIRequested = c;

	/* calculate sizes in sample */
	//blockSizeInSamples = infoStruct->ADI_BlockSize;
	//bufferSizeInSamples = infoStruct->DriverBufferSize;

	//freqInMS = (DWORD) (1000.0 / (infoStruct->AI_Frequency / BlockSize));

	return TRUE;
}

/**
 * Name: StreamCallback(long scansAvailable, double userValue)
 * Desc: Stream callback function that places values read by LabJack 
 *		 into DASYLab buffer
**/
void LabJackLayer::StreamCallback(long scansAvailable, double userValue)
{
	
	long lngErrorcode;
	int i;
	double dblScansAvailable = scansAvailable;
	double adblData[4000]; // TODO: Dynamic allocation
	long padblData = (long)&adblData[0];

	UNUSED(userValue);

	lngErrorcode = eGet(lngHandle, LJ_ioGET_STREAM_DATA, LJ_chALL_CHANNELS, &dblScansAvailable, padblData);
	ErrorHandler(lngErrorcode);
	
	// Convert analog input values and place into buffer
	for(i=0; i<numAINRequested; i++)
	{
		AddToInputBuffer(ConvertAIValue(adblData[i], i));
	}

	// Determine states of digital input on channel 193
	double diData = adblData[i+1];
	for(i=0; i<numDIRequested; i++)
	{
		// Feed channel value in FIFO buffer
		inputBufferAdr[inputStoreIndex] = CheckBitHigh(diData, digitalInputScanList[i]);

		// increment FIFO index and wrap around
		inputStoreIndex++;
		if (inputStoreIndex == infoStruct->DriverBufferSize)
		{
			inputStoreIndex = 0;
			wrapAround = TRUE;
		}
	}
}

/**
 * Name: ErrorHandler(LJ_ERROR lngErrorcode)
 * Desc: Checks for errors after every LabJack UD call
**/
void LabJackLayer::ErrorHandler (LJ_ERROR lngErrorcode)
{
	if (lngErrorcode != LJE_NOERROR)
	{
		SetError(lngErrorcode);
		DRV_ShowError();
	}
}

/**
 * Name: IsRequestingAIN(int channel)
 * Desc: Returns True if the user is polling/streaming the given AIN
		 channel or False otherwise.
**/
bool LabJackLayer::IsRequestingAIN(int channel)
{
	return (infoStruct->AI_Channel[0] & (1 << channel)) > 0;
}

/**
 * Name: IsRequestingDI(int channel)
 * Desc: Returns True if the user is polling/streaming the given digital
 *		 channel as input or False otherwise.
**/
bool LabJackLayer::IsRequestingDI(int channel)
{
	return (infoStruct->DI_Channel & (1 << channel)) > 0;
}

/**
 * Name: ConvertAIValue
 * Desc: Converts a normal double into a value
 *		 suitable for DASYLab AIN use.
**/
SAMPLE LabJackLayer::ConvertAIValue(double value, UINT channel)
{
	//SAMPLE returnArray[2];
	double maxValue;

	// Apply range
	if(infoStruct->AI_ChSetup[channel].GainCode != 0)
		value = value * infoStruct->GainInfo[infoStruct->AI_ChSetup[channel].GainCode];

	// TODO: This is a really unacceptable excuse for shuffling data around
	// Calculate range
	debugFile << value << "\n";
	double inputRange = infoStruct->AI_ChInfo[channel].InputRange_Max - infoStruct->AI_ChInfo[channel].InputRange_Min;
	double bitsAvailable = sizeof(SAMPLE) * 8;

	// Find the maximum value
	if (infoStruct->AI_ChInfo[channel].InputRange_Min < 0)
		maxValue = pow(2.0, bitsAvailable-1);
	else
		maxValue = pow(2.0, bitsAvailable);

	// Return the converted value
	return (SAMPLE)(maxValue / inputRange * value);
}

/**
 * Name: FreeLockedMem
 * Desc: Unlocks memory taken by DASYLab
 * Note: This is from the DASYLab example and left mostly unchanged
**/
void LabJackLayer::FreeLockedMem (LPSAMPLE bufferadr)
{
	HGLOBAL hMem;

	if ( bufferadr == NULL )
		return;

	hMem = GlobalPtrHandle(bufferadr);

	if ( hMem == 0 )
		return;

#ifdef WIN32
	VirtualUnlock(hMem,GlobalSize(hMem));
	GlobalUnlock (hMem);
#else
	GlobalUnlock (hMem);
	GlobalPageUnlock (hMem);
	GlobalUnfix (hMem);
#endif

	GlobalFree (hMem);
}

/**
 * Name: AllocLockedMem (DWORD nSamples)
 * Desc: Allocatese reserved memory for DASYLab use
 * Note: This is simply taken from the DASYLab driver example
**/
LPSAMPLE LabJackLayer::AllocLockedMem (DWORD nSamples, DRV_INFOSTRUCT * infoStruct)
{
	LPSAMPLE bufferadr;
	HGLOBAL hMem;

	if ( nSamples == 0 )
		return NULL;

	GlobalCompact (nSamples * 8);// TODO: infoStruct->ADI_BlockSize);
	hMem = GlobalAlloc (GMEM_FIXED | GMEM_ZEROINIT, nSamples * sizeof(SAMPLE));
	if (!hMem)
	{
		infoStruct->Error = DRV_ERR_NOTENOUGHMEM;
		return NULL;
	}
#ifdef WIN32
	bufferadr = (LPSAMPLE) GlobalLock (hMem);
	VirtualLock(bufferadr,nSamples * sizeof(SAMPLE));
#else
	GlobalFix (hMem);
	GlobalPageLock (hMem);
	bufferadr = (LPSAMPLE) GlobalLock (hMem);
#endif

	return bufferadr;
	/*SAMPLE * bufferAdr = NULL;
	bufferAdr = new SAMPLE[nSamples];
	return bufferAdr;*/
}

/**
 * Name: SetError(DWORD newError)
 * Desc: Sets the information structure to a LabJack error
 * Note: This borders on a bad practice and should be avoided
**/
void LabJackLayer::SetError(DWORD newError)
{
	if(newError > 0)
		infoStruct->Error = newError + LABJACK_ERROR_PREFIX;
	else
		infoStruct->Error = 0;
}

/**
 * Name: CheckBitHigh(float value, int position)
 * Desc: Returns true if bit at position is 1 or false otherwise
**/
bool LabJackLayer::CheckBitHigh(double value, int position)
{
	return std::bitset<sizeof(double)>(value).test(position);
}

/**
 * Name: IsFrequencyValid()
 * Desc: Returns true if the device is capable of streaming at the
 *		 user's desired frequency and false otherwise
**/
bool LabJackLayer::IsFrequencyValid()
{
	return infoStruct->AI_Frequency <= MAX_SCANS_PER_SECOND;
}

/**
 * Name: StartStreaming()
 * Desc: Configures and starts analog/digital input streaming
**/
void LabJackLayer::StartStreaming()
{
	long lngErrorcode, lngIOType, lngChannel;
	double dblValue;
	int i;

    // Configure all analog inputs for 12-bit resolution
    lngErrorcode = AddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chAIN_RESOLUTION, 12, 0, 0);
    ErrorHandler(lngErrorcode);

    // Set the scan rate.
    lngErrorcode = AddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_SCAN_FREQUENCY, infoStruct->AI_Frequency/(numAINRequested + numDIRequested), 0, 0);
    ErrorHandler(lngErrorcode);

    // Give the driver a 5 second buffer (scanRate * channels * 5 seconds).
    lngErrorcode = AddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_BUFFER_SIZE, infoStruct->AI_Frequency*5, 0, 0);
    ErrorHandler(lngErrorcode);

    // Configure reads to retrieve whatever data is available without waiting
    lngErrorcode = AddRequest(lngHandle, LJ_ioPUT_CONFIG, LJ_chSTREAM_WAIT_MODE, LJ_swNONE, 0, 0);
    ErrorHandler(lngErrorcode);

	// Clear stream channels
	lngErrorcode = AddRequest(lngHandle, LJ_ioCLEAR_STREAM_CHANNELS, 0, 0, 0, 0);
    ErrorHandler(lngErrorcode);

    // Define the analog input scan list
	for(i=0; i<numAINRequested; i++)
	{
		lngErrorcode = AddRequest(lngHandle, LJ_ioADD_STREAM_CHANNEL, analogInputScanList[i], 0, 0, 0);
		ErrorHandler(lngErrorcode);
	}

	// Define the digital input scan list
	if (numDIRequested > 0)
	{
		lngErrorcode = AddRequest(lngHandle, LJ_ioADD_STREAM_CHANNEL, 193, 0, 0, 0); // Channel 193 provides FIO/EIO data (sec. 3.2.1 of user's gude)
		ErrorHandler(lngErrorcode);
	}

	//Execute the list of requests.
    lngErrorcode = GoOne(lngHandle);
    ErrorHandler(lngErrorcode);
    
	//Get all the results just to check for errors.
	lngErrorcode = GetFirstResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
	ErrorHandler(lngErrorcode);
	while(lngErrorcode < LJE_MIN_GROUP_ERROR)
	{
		lngErrorcode = GetNextResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
		if(lngErrorcode != LJE_NO_MORE_DATA_AVAILABLE)
			ErrorHandler(lngErrorcode);
	}
    
	// Put in the callback. If the X1 parameter is set to something other than 0
	// the driver will call the specified function after that number of scans
	// have been reached.
	//long pCallback = void (*StreamCallback)(long ScansAvailable, double UserData);
	//pCallback = &StreamCallback;
    lngErrorcode = ePut(lngHandle, LJ_ioSET_STREAM_CALLBACK, (long)StreamCallbackWrapper, 0, 0);
	ErrorHandler(lngErrorcode);

	//Start the stream.
    lngErrorcode = eGet(lngHandle, LJ_ioSTART_STREAM, 0, &dblValue, 0);
    ErrorHandler(lngErrorcode);

	isStreaming = TRUE;

	// Indicate that we have started measuring
	measRun = TRUE;
}

/**
 * Name: StartCommandResponse()
 * Desc: Begin reading analog/digital input through command response
**/
void LabJackLayer::StartCommandResponse()
{
	// Find smallest channel
	if (numAINRequested > 0)
	{
		smallestChannel = analogInputScanList[0];
		smallestChannelType = ANALOG;
	}
	else
	{
		smallestChannel = digitalInputScanList[0];
		smallestChannelType = DIGITAL;
	}

	// Install MultiMedia interrupt handler
	if ( ! InstallTimerInterruptHandler() )
	{
		infoStruct->Error = DRV_ERR_HARD_CONFLICT;
		MessageBeep((UINT)-1);
		measRun = FALSE;
	}
	else
		measRun = TRUE; // Indicate that we have started measuring
}

/**
 * Name: InstallTimerInterruptHandler()
 * Desc: Starts software timer to poll device
 * Note: This is mostly copied from the DASYLab example driver code
**/
bool LabJackLayer::InstallTimerInterruptHandler()
{
	// TODO: DASYLab used a value of 10 for the resolution but I don't see why a minimum could not be found (0.1)
	if ( hTimerID == 0 )
	{
		TIMECAPS capabilities;

		if ( timeGetDevCaps ( &capabilities, sizeof(capabilities)) == TIMERR_NOCANDO )
			return FALSE;
		if ( capabilities.wPeriodMin > 10 || capabilities.wPeriodMax < 10 )
			return FALSE;

		if ( timeBeginPeriod (10) == TIMERR_NOCANDO )
		{
			timeEndPeriod (10);
			return FALSE;
		}

		hTimerID = timeSetEvent ( (1.0/infoStruct->AI_Frequency*1000), 10, CommandResponseCallbackWrapper, (DWORD)this, TIME_PERIODIC);
		if ( hTimerID == 0 )
		{
			timeEndPeriod (10);
			return FALSE;
		}
	}		 

	return TRUE;
}

/**
 * Name: RemoveTimerInterruptHandler()
 * Desc: Stops software timer to poll device
 * Note: This is mostly copied from the DASYLab exmaple driver code
**/
void LabJackLayer::RemoveTimerInterruptHandler ()
{
	if ( hTimerID != 0 )
	{
		timeKillEvent( hTimerID );
		timeEndPeriod (10);

		hTimerID = 0;
	}	
}

/**
 * Name: CommandResponseCallback()
 * Desc: Polls device for analog/digital input data and places
 *		 it in the FIFO data structure shared with DASYLab
**/
void LabJackLayer::CommandResponseCallback()
{
	LJ_ERROR lngErrorcode;
	int i;
	double dblValue;

	// TODO: Try to use PostMessage
	
	// Make requests for the channels that the experiment is reading
	for(i=0; i<numAINRequested; i++)
	{
		lngErrorcode = AddRequest(lngHandle, LJ_ioGET_AIN, analogInputScanList[i], 0, 0, 0);
		ErrorHandler(lngErrorcode);
	}

	for(i=0; i<numDIRequested; i++)
	{
		lngErrorcode = AddRequest(lngHandle, LJ_ioGET_DIGITAL_BIT, digitalInputScanList[i], 0, 0, 0);
		ErrorHandler(lngErrorcode);
	}

	//Execute the requests.
	lngErrorcode = GoOne(lngHandle);
	ErrorHandler(lngErrorcode);

	// Read back the results
	for(i=0; i<numAINRequested; i++)
	{
		lngErrorcode = GetResult(lngHandle, LJ_ioGET_AIN, analogInputScanList[i], &dblValue);
		ErrorHandler(lngErrorcode);
		AddToInputBuffer(ConvertAIValue(dblValue, analogInputScanList[i])); // TODO: We ought to have a channel variable in here
	}

	for(i=0; i<numDIRequested; i++)
	{
		lngErrorcode = GetResult(lngHandle, LJ_ioGET_DIGITAL_BIT, digitalInputScanList[i], &dblValue);
		ErrorHandler(lngErrorcode);
		AddToInputBuffer((SAMPLE)dblValue);
	}

}

/**
 * Name: AddToInputBuffer(SAMPLE newValue)
 * Desc: Adds a new reading to DASYLab's input buffer
**/
void LabJackLayer::AddToInputBuffer(SAMPLE newValue)
{
	// Feed channel value in FIFO buffer
	inputBufferAdr[inputStoreIndex] = newValue;

	// increment FIFO index and wrap around
	inputStoreIndex++;
	if (inputStoreIndex == infoStruct->DriverBufferSize)
	{
		inputStoreIndex = 0;
		wrapAround = TRUE;
	}
}

/**
 * Name: WriteDigitalOutput(UINT chan, DWORD outVal)
 * Desc: Sets a FIO line high or low
**/
void LabJackLayer::WriteDigitalOutput(UINT chan, DWORD outVal)
{
	long lngErrorcode;

	// TODO: AddRequest did not work here but this should be changed in a future release
	lngErrorcode = ePut(lngHandle, LJ_ioPUT_DIGITAL_BIT, chan, outVal, 0);
	ErrorHandler(lngErrorcode);
}

/**
 * Name: WriteDAC(UINT chan, DWORD outVal)
 * Desc: Writes an analog output voltage to the given DAC channel
**/
void LabJackLayer::WriteDAC(UINT chan, DWORD outVal)
{
	long lngErrorcode = 0;
	double convertedVoltage;

	// Convert Voltage
	convertedVoltage = ConvertAOValue(outVal, chan);

	// Write the value for the given channel
	lngErrorcode = AddRequest(lngHandle, LJ_ioPUT_DAC, chan, convertedVoltage, 0, 0);
	GoOne(lngHandle); // TODO: What is up here? Performance issue!
	ErrorHandler(lngErrorcode);
}

/**
 * Name: ConvertAOValue(DWORD value, UINT channel)
 * Desc: Converts a DASYLab sample into a value suitable for LabJack DAC use	
**/
double LabJackLayer::ConvertAOValue(DWORD value, UINT channel)
{
	double inputRange = infoStruct->AI_ChInfo[channel].InputRange_Max - infoStruct->AI_ChInfo[channel].InputRange_Min;
	double bitsAvailable = sizeof(SAMPLE) * 8;
	double maxValue = pow(2.0, bitsAvailable-1);
	double conversionFactor = (maxValue * 2.0 / inputRange); // Multiply by 2 to get both negative and positive values
	return value / conversionFactor;
}

/**
 * Name: OpenDevice(long deviceType)
 * Desc: Closes the current device (if any) and opens the  
 *		 device of the given type.
**/
void LabJackLayer::OpenDevice(long newDeviceType)
{
	long lngErrorcode;

	if(open)
		Close();

	// Open the LabJack and create buffer
	SetError(OpenLabJack (newDeviceType, LJ_ctUSB, "1", 1, &lngHandle));
	if (!GetError() && AllocateInputBuffer (DEFAULT_BUFFER_SIZE)) 
		open = TRUE;

	// Save the device type
	deviceType = newDeviceType;

	// Get the cal constants
	long pCalMem = (long)&calConstants[0];
	lngErrorcode = eGet(lngHandle, LJ_ioGET_CONFIG, LJ_chCAL_CONSTANTS, 0, pCalMem);
    ErrorHandler(lngErrorcode);

	// Reset the InfoStructure
	// Fill the information structure
	FillInfoStructure();
}

/**
 * Name: OpenDeviceByEthernet(long deviceType, char * )
 * Desc: Closes the current device (if any) and opens the  
 *		 device of the given type via ethernet at the given address
**/
void LabJackLayer::OpenEthernetDevice(long newDeviceType, CString address)
{
	if(open)
		Close();

	// Open the LabJack and create buffer
	SetError(OpenLabJack (newDeviceType, LJ_ctETHERNET, address.GetBuffer(0), 1, &lngHandle));
	if (!GetError() && AllocateInputBuffer (DEFAULT_BUFFER_SIZE)) 
		open = TRUE;

	// Save the device type
	deviceType = newDeviceType;

	// Reset the InfoStructure
	// Fill the information structure
	FillInfoStructure();
}

/**
 * Name: GetDeviceType()
 * Desc: Returns the device type of the device currently
 *		 in operation
**/
long LabJackLayer::GetDeviceType()
{
	return deviceType;
}

/**
 * Name: IsUsingEthernet()
 * Desc: Returns true if the device is in use through ethernet
 *		 or false otherwise
**/
bool LabJackLayer::IsUsingEthernet()
{
	return false;
}

/**
 * Name: ConfigureRange()
 * Desc: Changes the information structure's range
**/
void LabJackLayer::ConfigureRange()
{
	int n;
	long lngErrorcode;

	for (n=0; n < numAINRequested; n++) 
	{
		//infoStruct->AI_ChInfo[n].InputRange_Max = CalMaxAIValue(n);
		//infoStruct->AI_ChInfo[n].InputRange_Min = CalMinAIValue(n);
		lngErrorcode = AddRequest(lngHandle, LJ_ioPUT_AIN_RANGE, n, infoStruct->GainInfo[infoStruct->AI_ChSetup[n].GainCode], 0, 0);
		ErrorHandler(lngErrorcode);
	}
}

/**
 * Name: CalMaxAIValue(int channel)
 * Desc: Gets the calibrated max value for the given channel
**/
double LabJackLayer::CalMaxAIValue(int channel)
{
	double posSlope, center;
	long dasyLabRangeCode, ljRangeCode;
	dasyLabRangeCode = infoStruct->GainInfo[infoStruct->AI_ChSetup[channel].GainCode];
	ljRangeCode = ConvertToUDRange(dasyLabRangeCode);
	posSlope = calConstants[posSlopeConstLocations[ljRangeCode]];
	center = calConstants[centerConstLocations[ljRangeCode]];

	return (MAX_BIT_VALUE - center) * posSlope;
}

/**
 * Name: CalMinAIValue(int channel)
 * Desc: Returns the calibrated min value for the given range
**/
double LabJackLayer::CalMinAIValue(int channel)
{
	double negSlope, center;
	long dasyLabRangeCode, ljRangeCode;

	dasyLabRangeCode = infoStruct->GainInfo[infoStruct->AI_ChSetup[channel].GainCode];
	ljRangeCode = ConvertToUDRange(dasyLabRangeCode);
	negSlope = calConstants[negSlopeConstLocations[ljRangeCode]];
	center = calConstants[centerConstLocations[ljRangeCode]];

	return (center - MIN_BIT_VALUE) * negSlope;
}

/**
 * Name: MapCalConstants()
 * Desc: Loads the locations for various calibration constants
 *		 for the current device type.
**/
//void LabJackLayer::MapCalConstants()
//{
//	// The following values map the ranges defined in the UD driver header
//	// to the number of bytes from the starting byte (0) as returned from the
//	// UD driver
//	switch(deviceType)
//	{
//	case LJ_dtU3:
//		break; // TODO: Support special range
//
//	case LJ_dtU6:
//
//		// Clear maps
//		posSlopeConstLocations.clear();
//		negSlopeConstLocations.clear();
//		centerConstLocations.clear();
//
//		// Bipolar 10 V
//		posSlopeConstLocations.insert(pair<long, long>(LJ_rgBIP10V, 0));
//		negSlopeConstLocations.insert(pair<long, long>(LJ_rgBIP10V, 8));
//		centerConstLocations.insert(pair<long, long>(LJ_rgBIP10V, 9));
//		
//		// Bipolar 1 V
//		posSlopeConstLocations.insert(pair<long, long>(LJ_rgBIP1V, 3));
//		negSlopeConstLocations.insert(pair<long, long>(LJ_rgBIP1V, 11));
//		centerConstLocations.insert(pair<long, long>(LJ_rgBIP1V, 12));
//		
//		// Bipolar 100 mV
//		posSlopeConstLocations.insert(pair<long, long>(LJ_rgBIPP1V, 5));
//		negSlopeConstLocations.insert(pair<long, long>(LJ_rgBIPP1V, 13));
//		centerConstLocations.insert(pair<long, long>(LJ_rgBIPP1V, 14));
//		
//		// Bipolar 10 mV
//		posSlopeConstLocations.insert(pair<long, long>(LJ_rgBIPP01V, 7));
//		negSlopeConstLocations.insert(pair<long, long>(LJ_rgBIPP01V, 15));
//		centerConstLocations.insert(pair<long, long>(LJ_rgBIPP01V, 16));
//		
//		break;
//	}
//}

/**
 * Name: ConvertToUDRange(long dasyRange)
 * Desc: Converts a DASYLab range to a range code needed for the UD driver
**/
long LabJackLayer::ConvertToUDRange(long dasyRange)
{
	return dasyLJGainCodes[dasyRange];
}