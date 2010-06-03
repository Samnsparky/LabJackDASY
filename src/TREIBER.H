/* ======================================================================== */
/*                                                                          */
/*               DASYLab - Data Acquisition Laboratory                      */
/*               =====================================                      */
/*                                                                          */
/* Interface definition between DASYLab and the hardware driver             */
/*                                                                          */
/* Copyright (c) 1992-98 by DASYTEC GmbH, Moenchengladbach, Germany         */
/* All rights reserved.                                                     */
/*                                                                          */
/* Created     : R. Borghoff  02-APR-1992                                   */
/* Modified    : C. Strunk    03-JUN-1993  partly translated into english   */
/*               C. Strunk    21-APR-1994  finished translation             */
/*               C. Strunk    09-MAY-1994  Adapted to Interface Ver 0x10    */
/*               C. Strunk    26-JUN-1995  Prepared for 32-Bit              */
/*               C. Strunk    24-SEP-1996  Adapted to Interface Ver 0x11    */
/*               C. Strunk    05-MAR-1998  Added Dlab 3.52 extensions       */
/*                                                                          */
/* ======================================================================== */

#ifndef _TREIBER_H
#define _TREIBER_H

#include <WINDOWS.H>      /* Import Typedefs for DWORD etc.                 */

/* We specify a definite packing for the following data structures */
#ifndef RC_INVOKED
#pragma pack(1)
#endif  /* RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif   /* __cplusplus */

#ifndef _TIME_T_DEFINED
typedef long    time_t;
#define _TIME_T_DEFINED
#endif 

/************** CONSTANTS    ****************/

/* Driver interface version number in DRV_INFOSTRUCT.Version */
#define DRV_VERSION           0x0011         /* Driver interface version (hardware driver)     */
#define DRV_VERSION_DEMO      0x8011         /* Driver interface version (demo driver)         */
#define SUPPORT_LEVEL         0              /* DASYLab's current support level                */

/* Return codes for most of the functions */
#define DRV_FUNCTION_OK       1              /* Ok. Everything worked well.                    */
#define DRV_FUNCTION_FALSE    0              /* An error occured. DRV_INFOSTRUCT.Error         */
                                             /* contains an error code now                     */
/* Dialog box codes (first parameter to DRV_ShowDialog) */
#define DRV_DLG_CARD_DEF      1              /* Show the 'Hardware Setup' dialog               */
#define DRV_DLG_CHANNEL_DEF   2              /* Show the 'Channel Setup' dialog                */
#define DRV_AO_CHAN_DLG       3              /* Show the 'AO Channel Setup' dialog             */
#define DRV_CT_CHAN_DLG       4              /* Show the 'CT Channel Setup' dialog             */
#define DRV_CO_CHAN_DLG       5              /* Show the 'CO Channel Setup' dialog             */
#define DRV_DLG_ABOUT         99             /* Show the 'Driver Info' dialog                  */

/* Hardware Manufacturer ID codes for DRV_INFOSTRUCT.DriverIdCode */
#define DRV_ID_ILLEGAL        0              /* Illegal or old driver                          */
#define DRV_ID_DEMO           1              /* This is a demo driver                          */
#define DRV_ID_FREEWARE       2              /* This is a freeware driver                      */
#define DRV_ID_OEM            3              /* This is an OEM driver                          */

/* Acquisition modes for SupportedAcqModes and AcquisitionMode */
#define DRV_AQM_CONTINUOUS    0x00000001L    /* Continuous gap-free acquisition                */
#define DRV_AQM_BLOCK         0x00000002L    /* Gap after every ContinouosBlocks blocks        */
#define DRV_AQM_STOP          0x00000004L    /* Stop after capturing ContinouosBlocks blocks   */

/* Status bits for DRV_MEASINFO.MeasStatus */
#define DRV_MEASRUN           0x00010000L    /* Experiment is (still) running                  */
#define DRV_AI_OVRRN          0x00020000L    /* Input buffer overrun                           */
#define DRV_AO_BLOCKEND       0x00040000L    /* Ouput buffer overrun                           */
#define DRV_DATA_LOST         0x00080000L    /* Lost data (e.g. because of lost interrupts)    */
#define DRV_ERROR_STOP        0x00100000L    /* Stopped due to error                           */

/* Input/Output range in AI/O_CHDEF.BaseUnit */
#define DRV_BASE_UNIT_U             0        /* Samples are unsigned short (zero for Min)      */
#define DRV_BASE_UNIT_2COMP         1        /* Samples are signed short (zero for 0.0)        */
#define DRV_BASE_UNIT_INV_U         2        /* Bit-Complement of DRV_BASE_U                   */
#define DRV_BASE_UNIT_INV_2COMP     3        /* Bit-Complement of DRV_BASE_2COMP               */
#define DRV_BASE_UNIT_FLOAT         4        /* Samples are IEEE 32bit float                   */
#define DRV_BASE_UNIT_U_LONG        6        /* Samples are unsigned long (zero for Min)       */

/* Flag values in AI_CHDEF.Flags */
#define AI_FLG_UNAVAILABLE           0x0001  /* This channel is not available                  */
                                             
/* Flag values in DI_CHDEF.Flags */
#define DI_FLG_UNAVAILABLE           0x0001  /* This channel is not available                  */
#define DI_FLG_PROG_DIRECTION        0x0002  /* This digital port supports software prg dir    */

/* Flag values in CT_CHDEF.Flags */
#define CT_FLG_UNAVAILABLE           0x0001  /* This channel is not available                  */
#define CT_FLG_PROG_DIRECTION        0x0002  /* This counter port supports software prg dir    */
#define CT_FLG_NON_DESTRUCTIVE_READ  0x0010  /* Reading does not reset the counter value       */

/* Flag values in AO_CHDEF.Flags */
#define AO_FLG_UNAVAILABLE           0x0001  /* This channel is not available                  */

/* Flag values in DO_CHDEF.Flags */
#define DO_FLG_UNAVAILABLE           0x0001  /* This channel is not available                  */
#define DO_FLG_PROG_DIRECTION        0x0002  /* This digital port supports software prg dir    */

/* Flag values in CO_CHDEF.Flags */
#define CO_FLG_UNAVAILABLE           0x0001  /* This channel is not available                  */
#define CO_FLG_PROG_DIRECTION        0x0002  /* This counter port supports software prg dir    */

/* Modes for CO_CHDEF.Modes */
#define CO_MODE_RATE_MODULATION      0x0001  /* Modulation of the output rate (Square wave)    */
#define CO_MODE_PULSE_MODULATION     0x0002  /* Modulation of the output rate (Pulses)         */
#define CO_MODE_PWM                  0x0004  /* Modulation of the Pulse width                  */

/* Flag values in HW_SECTION.Flags */
#define HW_FLG_SLOW                  0x0001  /* This component is slow                         */
                                             
/* Error codes and warnings in DRV_INFOSTRUCT.Error */
#define DRV_ERR_OK            0x00           /* No error -- everything is fine.                */
#define DRV_ERR_NO386MODE     0x01           /* This driver can only be run in 386enh. mode    */
#define DRV_ERR_NOVDALOGD     0x02           /* The virtual device driver is missing           */
#define DRV_ERR_BUFSIZETOBIG  0x03           /* The driver buffer is too large                 */
#define DRV_ERR_NOCHANNEL     0x04           /* No channel has been specified                  */
#define DRV_ERR_DEVICENOTINIT 0x05           /* The driver is not initialized yet.             */
#define DRV_ERR_NOTENOUGHMEM  0x06           /* Out of memory                                  */
#define DRV_ERR_DLL2DEVICE    0x07           /* DLL + VxD driver don't fit together            */
#define DRV_ERR_OPENCOMX      0x08           /* Error opening the selected COM port            */
#define DRV_ERR_NOHWSUPPORT   0x09           /* No support for this hardware                   */
#define DRV_ERR_MEASRUN       0x0A           /* This function is not allowed during experiment */
#define DRV_ERR_CARDNOTFOUND  0x0B           /* No Hardware was found                          */
#define DRV_ERR_CHANGEGAIN    0x0C           /* Gain list had to be changed                    */
#define DRV_ERR_CHECKHARDWARE 0x0D           /* Installed hardware did not match the setup     */
#define DRV_ERR_WRONGVXD      0x0E           /* Wrong VxD driver                               */
#define DRV_ERR_HARD_CONFLICT 0x0F           /* Experiment could not start due to hardw. confl.*/
#define DRV_WARN_CHANGEFREQ   0x10           /* AI_Frequency had to be changed                 */
#define DRV_WARN_CHANGECHLIST 0x11           /* Channel list had to be changed                 */

/* Output mode in DRV_INFOSTRUCT.AO_Mode and DRV_INFOSTRUCT.DO_Mode */
#define DRV_OUT_CONTIMODE     0x00           /* Do continuous output                           */
#define DRV_OUT_BLOCKMODE     0x01           /* Do repeated block output of one block only     */

/* Flags for supported features in DRV_INFOSTRUCT.Features */
#define SUPPORT_DEFAULT       0x00000000L    /* Support default features */
/* define SUPPORT_AI_COUNTER    0x00000001L    / * Support 'Stop after n samples' mode         */
#define SUPPORT_THERMOMEAS    0x00000002L    /* Support Thermo element + CJC measurement       */
#define SUPPORT_NO_SYNC_AO    0x00000004L    /* Don't support synchronous AO                   */
#define SUPPORT_NO_SYNC_DI    0x00000008L    /* Don't support synchronous DI                   */
#define SUPPORT_NO_SYNC_DO    0x00000010L    /* Don't support synchronous DO                   */
#define SUPPORT_NO_SYNC_CT    0x00000020L    /* Don't support synchronous CT                   */
#define SUPPORT_NO_ASYNC_AO   0x00000040L    /* Don't support asynchronous AO                  */
#define SUPPORT_AO_CONTIMODE  0x00000080L    /* Support continuous output                      */
#define SUPPORT_AO_BLOCKMODE  0x00000100L    /* Support repeated block output                  */
#define SUPPORT_ALT_COUNT     0x00000200L    /* Count channels starting from 1 instead of 0    */
#define SUPPORT_ENHANCED_DLG  0x00000400L    /* Support the 'Channel Setup' dialog             */
#define SUPPORT_NO_ADI_BUFFER 0x00001000L    /* Don't support a driver buffer                  */
#define SUPPORT_OUT_ALL       0x00002000L    /* Output all AO or DO channels simultaneously    */
#define SUPPORT_SLOW_HW       0x00004000L    /* Support slow HW components                     */
#define SUPPORT_AO_CHAN_DLG   0x00008000L    /* Support the 'AO Channel Setup' dialog          */
#define SUPPORT_CT_CHAN_DLG   0x00010000L    /* Support the 'CT Channel Setup' dialog          */
#define SUPPORT_CO_CHAN_DLG   0x00020000L    /* Support the 'CO Channel Setup' dialog          */
#define SUPPORT_DLG_ON_RUN    0x00040000L    /* Allow 'Hardware Setup' dialog while running    */

/* Setup flags in DRV_INFOSTRUCT.HWSetup */
#define SETUP_THERMOMEAS      0x00000001L    /* Setup for CJC temperature measurement          */

/* *** Scan list layout *** */

/* The SCAN_ENTRY type is used for items inside the scan list and for   */
/* the first parameter of the DRV_ExplainGainCode() function.           */

/* The scan list consists of unsigned short values */
typedef unsigned short SCAN_ENTRY;

/* Each scan list entry is an ORed combination of one type and one channel number   */
#define SCAN_TYPE(ent) ( ent & 0xF000 )
#define SCAN_CHAN(ent) ( ent & 0x0FFF )

/* The scan list may contain entries of the following types */
#define SCAN_TYPE_SPECIAL     0x0000         /* Special entry like dummy channel    */
#define SCAN_TYPE_AI          0x1000         /* Analog input channel                */
#define SCAN_TYPE_DI          0x2000         /* Digital input channel               */
#define SCAN_TYPE_CT          0x3000         /* Counter input channel               */

/* The following scan types are not used inside a scan list */
#define SCAN_TYPE_AO          0x4000         /* Analog input channel                */
#define SCAN_TYPE_DO          0x5000         /* Digital input channel               */

/* Max number of Entries in HW_SECTION.GainInfo array */
#define  MAX_GAINS  16

/************** TYPES ***********************/

#ifdef WIN32
typedef short          INT16;
typedef unsigned short UINT16;
#define _far   /* nothing */
#define __far  /* nothing */
#define huge   /* nothing */
#define _huge  /* nothing */
#define __huge /* nothing */
#else
typedef int  INT16;
typedef UINT UINT16;
#endif

/* All data (analog, digital and counter) is transferred in the SAMPLE data type   */
/* which is a synonym to 'short' at the moment. Please do not use 'short' or 'int' */
/* types directly as we may eventually change this type in a future release of the */
/* interface. Write 'sizeof(SAMPLE)' instead of 'sizeof(int)' and so on.           */

typedef short          SAMPLE;
typedef unsigned short USAMPLE;
typedef SAMPLE _huge * LPSAMPLE;

/* Analog input channel specifications */
typedef struct
{
   double InputRange_Max;
   double InputRange_Min;
   DWORD  Resolution;
   UINT16 Flags;
   UINT16 BaseUnit;
   char Unit[8];                 /* SI unit; currently unsupported */
   UINT16 __rsv[4];                     /* reserved for future use */
} AI_CHDEF;

/* Analog input channel setup */
typedef struct
{
   UINT16 GainCode;
   DWORD DriverParam;
   UINT16 __rsv[4];                     /* reserved for future use */
} AI_CHSET;

/* Digital input channel specifications */
typedef struct
{
   UINT16 Flags;
   UINT16 AvailBits;
   /* Parameters for software programmable direction */
   UINT16 DO_Chan;                                  /* Corrosponding DO channel */
   UINT16 GroupWidth;             /* Port width for ProgDir, i,e, 1, 4, 8, etc. */
   UINT16 __rsv[2];                     /* reserved for future use */
} DI_CHDEF;

/* Digital input channel setup */
typedef struct
{
   UINT16 OutputBits;  /* Used with software programmable direction only */
   DWORD DriverParam;
   UINT16 __rsv[2];                     /* reserved for future use */
} DI_CHSET;

/* Counter input channel specifications */
typedef struct
{
   UINT16 Flags;
   UINT16 CO_Chan;                      /* Corrosponding CO channel */
   UINT16 BaseUnit;
   UINT16 __rsv[1];                     /* reserved for future use */
} CT_CHDEF;

/* Counter input channel setup */
typedef struct
{
   DWORD DriverParam;
   UINT16 __rsv[2];                     /* reserved for future use */
} CT_CHSET;


/* Analog output channel specifications */
typedef struct
{
   double OutputRange_Max;
   double OutputRange_Min;
   DWORD  Resolution;
   UINT16 Flags;
   UINT16 BaseUnit;         /* Currently unsupported; must be zero */
   char Unit[8];                 /* SI unit; currently unsupported */
   UINT16 __rsv[4];                     /* reserved for future use */
} AO_CHDEF;

/* Analog output channel setup */
typedef struct
{
   UINT16 GainCode;           /* Currently unsupported; must be zero */
   DWORD DriverParam;
   UINT16 __rsv[4];                     /* reserved for future use */
} AO_CHSET;

/* Digital output channel specifications */
typedef struct
{
   UINT16 Flags;
   UINT16 AvailBits;
   /* Parameters for software programmable direction */
   UINT16 DI_Chan;                     /* Corrosponding DI channel */
   UINT16 __rsv[2];                     /* reserved for future use */
} DO_CHDEF;

/* Digital output channel setup */
typedef struct
{
   DWORD DriverParam;
   UINT16 __rsv[2];                     /* reserved for future use */
} DO_CHSET;

/* Counter output channel specifications */
typedef struct
{
   double BaseFrequency;                  /* Basic Frequency in Hz */
   UINT16 Flags;
   UINT16 Modes;
   DWORD  MinRate;                      /* Min output value e.g. 2 */
   DWORD  MaxRate;                  /* Max output value e.g. 65534 */
   UINT16 BaseUnit;         /* Currently unsupported; must be zero */
   UINT16 __rsv[7];                     /* reserved for future use */
} CO_CHDEF;

/* Counter output channel setup */
typedef struct
{
   DWORD  DriverParam;
   UINT16 Mode;                        /* Selected CO mode */
   DWORD  Rate;                        /* Output Rate factor */
   DWORD __rsv[4];                     /* reserved for future use */
} CO_CHSET;

/* Hardware component section */
typedef struct
{
   double MaxSumFrequency;
   double MaxSingleFrequency;
   UINT16 FirstChan;
   UINT16 FirstChanLabel;
   UINT16 NumChannels;
   UINT16 Grouping;
   INT16  GainInfo[MAX_GAINS];
   UINT16 Flags;                       /* HW_FLG_SLOW or zero */
   UINT16 __rsv[7];                    /* reserved for future use */
} HW_SECTION;

/* Hardware component */
typedef struct
{
   char ShortName[20];
   char LongName[64];
   HW_SECTION AI;
   HW_SECTION DI;
   HW_SECTION CT;
   HW_SECTION AO;
   HW_SECTION DO;
   HW_SECTION CO;
   UINT16 Flags;                      /* Currently unused; must be zero */
   UINT16 __rsv[7];                   /* reserved for future use */
} HW_COMPONENT;


/* What follows is the main interface structure */
/* Data flow: '>' == to TREIBER.DLL; '<' == from TREIBER.DLL */

typedef struct
{
   /* Part 1: Identification of hardware and driver */
   UINT16 Version;                /* >  Driver interface version number                        */
   UINT16 SupportLevel;           /* >  Level of supported features                            */
   DWORD  Error;                  /*  < Last error code                                        */
   char   DriverName[60];         /*  < Internal driver name                                   */
   char   DeviceName[20];         /*  < Name of device as displayed in the experiment setup dlg*/
   DWORD  Features;               /*  < Features supported by the driver                       */
   UINT16 DlabVersionCode;        /* >  DASYLab version code i.e. 352 for 3.52.xx              */
   UINT16 DriverIdCode;           /*  < Hardware Manufacturer identification code DRV_ID_xxx   */
   char   DLL_Version[40];        /*  < String in format "Version/Date" for the DLL driver     */
   char   VxD_Version[40];        /*  < String in format "Version/Date" for the VxD driver     */
   char   INIFileName[20];        /*  < Name of INI file for this driver                       */
   char   HelpFileName[20];       /*  < Hardware help file name                                */
   UINT16 HelpIndex;              /*  < Main entry point in help file                          */

   /* Part 2: General hardware parameters */
   double MinFreq;                /*  < Lowest possible AI sum frequency                       */
   double MaxFreq;                /*  < Highest possible AI sum frequency                      */
   double MinFreqPerChan;         /*  < Lowest possible frequency per channel                  */
   double MaxFreqPerChan;         /*  < Highest possible frequency per channel                 */
   double AiFreqList[8];          /*  < List for frequency selection display                   */

   UINT16 Max_AI_Channel;         /*  < Number of supported analog input channels              */
   UINT16 Max_DI_Channel;         /*  < Number of supported digital input channels             */
   UINT16 Max_CT_Channel;         /*  < Number of supported digital output channels            */
   UINT16 Max_AO_Channel;         /*  < Number of supported counter input channels             */
   UINT16 Max_DO_Channel;         /*  < Number of supported analog output channels             */
   UINT16 Max_CO_Channel;         /*  < Number of supported counter output channels            */

   UINT16 DIO_Width;              /*  < Max number of bits per DI/DO channel; typically 16     */
   INT16  GainInfo[32];           /*  < Gain factors for up to 32 gain codes                   */

   AI_CHDEF AI_ChInfo[512];       /*  < Parameters of the analog input channels                */
   DI_CHDEF DI_ChInfo[32];        /*  < Parameters of the digital input channels               */
   CT_CHDEF CT_ChInfo[32];        /*  < Parameters of the counter input channels               */
   AO_CHDEF AO_ChInfo[32];        /*  < Parameters of the analog output channels               */
   DO_CHDEF DO_ChInfo[32];        /*  < Parameters of the digital output channels              */
   CO_CHDEF CO_ChInfo[32];        /*  < Parameters of the counter output channels              */

   DWORD ScanListFeatures;        /*  < Scan list features; 0 == no scan list support          */
   UINT16 ScanListMinLen;         /*  < Scan list minimum length (typically 0 or 1)            */
   UINT16 ScanListMaxLen;         /*  < Scan list maximum length                               */

   DWORD MaxContinuousSamples;    /*  < Max. samples that can be captured gap-free or 0        */

   UINT16 NumHardwareComponents;  /*  < Number of installed hardware components                */

   DWORD SupportedAcqModes;       /*  < ORed combination of all supported acquisition modes    */

   DWORD MinSlowDownCount;        /*  < Minimum slowdown rate for slow channels                */

   /* Part 3: Experiment setup */
   double AI_Frequency;           /* >< Acquisition frequency == AI sum frequency              */
   DWORD  DI_FreqRate;            /* >  DI Rate  ==  AI_Sum_Frequency / DI_Sum_Frequency       */
   DWORD  CT_FreqRate;            /* >  CT Rate  ==  AI_Sum_Frequency / CT_Sum_Frequency       */
   DWORD  AO_FreqRate;            /* >  AO Rate  ==  AI_Sum_Frequency / AO_Sum_Frequency       */
   DWORD  DO_FreqRate;            /* >  DO Rate  ==  AI_Sum_Frequency / DO_Sum_Frequency       */
   DWORD  CO_FreqRate;            /* >  CO Rate  ==  AI_Sum_Frequency / CO_Sum_Frequency       */

   WORD   AI_Channel[32];         /* >< AI channels to acquire synchonously                    */
   DWORD  DI_Channel;             /* >  DI channels to acquire synchonously                    */
   DWORD  CT_Channel;             /* >  CT channels to acquire synchonously                    */
   DWORD  AO_Channel;             /* >  AO channels to output synchonously                     */
   DWORD  DO_Channel;             /* >  DO channels to output synchonously                     */
   DWORD  CO_Channel;             /* >  CO channels to output synchonously                     */

   DWORD  AI_AsyncChannel[32];    /* >  AI channels to acquire asynchonously                   */
   DWORD  DI_AsyncChannel;        /* >  DO channels to acquire asynchonously                   */
   DWORD  CT_AsyncChannel;        /* >  CT channels to acquire asynchonously                   */
   DWORD  AO_AsyncChannel;        /* >  AO channels to output asynchonously                    */
   DWORD  DO_AsyncChannel;        /* >  DO channels to output asynchonously                    */
   DWORD  CO_AsyncChannel;        /* >  CO channels to output asynchonously                    */
   
   DWORD  AcquisitionMode;        /* >  Mode of data acquisition                               */

   DWORD  DriverBufferSize;       /* >< WORD count: Total size of the driver buffer            */
   DWORD  ADI_BlockSize;          /* >  WORD count: Size of a DRV_GetInputBuf() transfer block */
   DWORD  AO_BlockSize;           /* >  WORD count: Size of a DRV_GetAnalogOutputBuf() block   */
   DWORD  DO_BlockSize;           /* >  WORD count: Size of a DRV_GetDigitalOutputBuf() block  */
   DWORD  CO_BlockSize;           /* >  WORD count: Size of a DRV_GetCounterOutputBuf() block  */

   DWORD  MaxBlocks;              /* >  Number of ADI_BlockSize blocks to acquire before stop  */
                                  /*    0 = infinite                                           */

   DWORD  SlowDownCount;          /* >  Slowdown rate for slow channels                        */

   AI_CHSET AI_ChSetup[512];      /* >< Setups for the analog input channels                   */
   DI_CHSET DI_ChSetup[32];       /* >< Setups for the digital input channels                  */
   CT_CHSET CT_ChSetup[32];       /* >< Setups for the counter input channels                  */
   AO_CHSET AO_ChSetup[32];       /* >< Setups for the analog output channels                  */
   DO_CHSET DO_ChSetup[32];       /* >< Setups for the digital output channels                 */
   CO_CHSET CO_ChSetup[32];       /* >< Setups for the digital output channels                 */
   DWORD  HWSetup;                /* >  Hardware setup, like CJC measurement enable            */

   time_t StartOfExperiment;      /*  < Start date/time of experiment (if specified)           */

   /* Part 4: Space for the driver to save parameters with the flow chart */
   char   DriverParam[140];       /* >  Space for the driver to store flow chart specific data */

   /* Part 5: Room for future extensions */
   DWORD  NoFunction[50];         /*    Reserved for future use */
} DRV_INFOSTRUCT;


typedef struct
{
   DWORD  MeasStatus;             /*  < Status Running/Stopped/Overrun etc.                    */
   long   CJValue;                /*  < Cold Junction temperature in 1/10 DegC                 */
   char   Message[12];            /*  < Driver message to be displayed inside the status bar   */
   DWORD  ADI_PercentFull;        /*  < Info on how full the buffer is                         */
   DWORD  __rsv[40];              /*    Reserved for future use */
} DRV_MEASINFO;


/************** External callback functions or function pointers **********/


/* ------------------------------------------------------------------------ */
/* Function     : DRV_AdvanceAnalogOutputBuf                                */
/* Description  : Tell Driver that the next data block was stored           */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_OK always                                    */
/* Calling      : DRV_GetAnalogOutputStatus will be continuously called     */
/*              : while the experiment is running. Every time it returned   */
/*              : TRUE, the DRV_GetAnalogOutputBuf function will then be    */
/*              : called, the data be stored, and after that a call to      */
/*              : DRV_AdvanceAnalogOutputBuf is made.                       */
/*              : At the time the DRV_AdvanceAnalogOutputBuf function is    */
/*              : called, the data block is stored completely, so the       */
/*              : driver can now start outputting the data.                 */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_AdvanceAnalogOutputBuf(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_AdvanceCounterOutputBuf                               */
/* Description  : Tell Driver that the next data block was stored           */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_OK always                                    */
/* Calling      : DRV_GetCounterOutputStatus will be continuously called    */
/*              : while the experiment is running. Every time it returned   */
/*              : TRUE, the DRV_GetCounterOutputBuf function will then be   */
/*              : called, the data be stored, and after that a call to      */
/*              : DRV_AdvanceCounterOutputBuf is made.                      */
/*              : At the time the DRV_AdvanceCounterOutputBuf function is   */
/*              : called, the data block is stored completely, so the       */
/*              : driver can now start outputting the data.                 */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_AdvanceCounterOutputBuf(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_AdvanceDigitalOutputBuf                               */
/* Description  : Tell Driver that the next data block was stored           */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_OK always                                    */
/* Calling      : DRV_GetDigitalOutputStatus will be continuously called    */
/*              : while the experiment is running. Every time it returned   */
/*              : TRUE, the DRV_GetDigitalOutputBuf function will then be   */
/*              : called, the data be stored, and after that a call to      */
/*              : DRV_AdvanceDigitalOutputBuf is made.                      */
/*              : At the time the DRV_AdvanceDigitalOutputBuf function is   */
/*              : called, the data block is stored completely, so the       */
/*              : driver can now start outputting the data.                 */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_AdvanceDigitalOutputBuf(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_AdvanceInputBuf                                       */
/* Description  : Tell Driver that the last data block is no longer needed  */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_OK always                                    */
/* Calling      : DRV_GetBufStatus will be continuously called while the    */
/*              : experiment is running. Every time it returned TRUE, the   */
/*              : DRV_GetBuf function will then be called, the data be      */
/*              : processed, and after that a call to DRV_SetBufFree is     */
/*              : made. At the time the driver DRV:SetBufFree function is   */
/*              : called, the data block processing is complete, so the     */
/*              : space can now be re-used for new data.                    */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_AdvanceInputBuf(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_ExplainGainCode                                       */
/* Description  : Convert gain code into a range string                     */
/* Parameters   : Chan: Channel number in SCAN_ENTRY coding                 */
/*              : GainCode: The code to convert                             */
/* Return Value : string explaining the input range or                      */
/*              : NULL in case of error                                     */
/* Calling      : This may be called at any time, but only if the flag      */
/*              : SUPPORT_EXPLAIN_GAIN is set. DASYLab will show these      */
/*              : strings inside the 'input range' list of the ADC box.     */
/* ------------------------------------------------------------------------ */

char FAR *FAR PASCAL DRV_ExplainGainCode(SCAN_ENTRY Chan, UINT GainCode);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_GetAnalogOutputBuf                                    */
/* Description  : Return a pointer to a buffer to new AO data               */
/* Parameters   : None.                                                     */
/* Return Value : Pointer to buffer (always)                                */
/* Calling      : After DRV_GetAnalogOutputBufStatus did return TRUE, the   */
/*              : DRV_GetAnalogOutputBuf function will be called to get a   */
/*              : location where to store the data. DRV_GetAnalogOutputBuf  */
/*              : must always return a valid pointer after                  */
/*              : DRV_GetAnalogOutputBufStatus returned TRUE, so this       */
/*              : function can never return NULL.                           */
/* ------------------------------------------------------------------------ */

LPSAMPLE FAR PASCAL DRV_GetAnalogOutputBuf(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_GetAnalogOutputStatus                                 */
/* Description  : Test if there is space to place a block of output data    */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_FALSE = less than InfoStruct->AO_BlockSize   */
/*              :                      SAMPLEs are free in the buffer       */
/*              : DRV_FUNCTION_OK otherwise                                 */
/* Calling      : DRV_GetAnalogOutputStatus will be continuously called     */
/*              : while the experiment is running. Every time it returned   */
/*              : TRUE, the DRV_GetAnalogOutputBuf function will then be    */
/*              : called, the data be stored, and after that a call to      */
/*              : DRV_AdvanceAnalogOutputBuf is made.                       */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_GetAnalogOutputStatus(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_GetCounterOutputBuf                                   */
/* Description  : Return a pointer to a buffer to new AO data               */
/* Parameters   : None.                                                     */
/* Return Value : Pointer to buffer (always)                                */
/* Calling      : After DRV_GetCounterOutputBufStatus did return TRUE, the  */
/*              : DRV_GetCounterOutputBuf function will be called to get a  */
/*              : location where to store the data. DRV_GetCounterOutputBuf */
/*              : must always return a valid pointer after                  */
/*              : DRV_GetCounterOutputBufStatus returned TRUE, so this      */
/*              : function can never return NULL.                           */
/* ------------------------------------------------------------------------ */

LPSAMPLE FAR PASCAL DRV_GetCounterOutputBuf(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_GetCounterOutputStatus                                */
/* Description  : Test if there is space to place a block of output data    */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_FALSE = less than InfoStruct->CO_BlockSize   */
/*              :                      SAMPLEs are free in the buffer       */
/*              : DRV_FUNCTION_OK otherwise                                 */
/* Calling      : DRV_GetCounterOutputStatus will be continuously called    */
/*              : while the experiment is running. Every time it returned   */
/*              : TRUE, the DRV_GetCounterOutputBuf function will then be   */
/*              : called, the data be stored, and after that a call to      */
/*              : DRV_AdvanceCounterOutputBuf is made.                      */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_GetCounterOutputStatus(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_GetDigitalOutputBuf                                   */
/* Description  : Return a pointer to a buffer to new AO data               */
/* Parameters   : None.                                                     */
/* Return Value : Pointer to buffer (always)                                */
/* Calling      : After DRV_GetDigitalOutputBufStatus did return TRUE, the  */
/*              : DRV_GetDigitalOutputBuf function will be called to get a  */
/*              : location where to store the data. DRV_GetDigitalOutputBuf */
/*              : must always return a valid pointer after                  */
/*              : DRV_GetDigitalOutputBufStatus returned TRUE, so this      */
/*              : function can never return NULL.                           */
/* ------------------------------------------------------------------------ */

LPSAMPLE FAR PASCAL DRV_GetDigitalOutputBuf(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_GetDigitalOutputStatus                                */
/* Description  : Test if there is space to place a block of output data    */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_FALSE = less than InfoStruct->DO_BlockSize   */
/*              :                      SAMPLEs are free in the buffer       */
/*              : DRV_FUNCTION_OK otherwise                                 */
/* Calling      : DRV_GetDigitalOutputStatus will be continuously called    */
/*              : while the experiment is running. Every time it returned   */
/*              : TRUE, the DRV_GetDigitalOutputBuf function will then be   */
/*              : called, the data be stored, and after that a call to      */
/*              : DRV_AdvanceDigitalOutputBuf is made.                      */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_GetDigitalOutputStatus(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_GetHardwareDescription                                */
/* Description  : Get List of Hardware Components                           */
/* Return Value : List Pointer or NULL                                      */
/* ------------------------------------------------------------------------ */

HW_COMPONENT FAR * FAR PASCAL DRV_GetHardwareDescription(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_GetInputBuf                                           */
/* Description  : Return a pointer to a buffer of new input data            */
/* Parameters   : None.                                                     */
/* Return Value : Pointer to buffer (always)                                */
/* Calling      : After DRV_GetInputBufStatus did return TRUE, the          */
/*              : DRV_GetInputBuf function will be called to retrieve the   */
/*              : data. DRV_GetInputBuf must always return a valid pointer  */
/*              : after DRV_GetInputBufStatus returned TRUE, so this        */
/*              : function can never return NULL.                           */
/* ------------------------------------------------------------------------ */

LPSAMPLE FAR PASCAL DRV_GetInputBuf(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_GetInputBufStatus                                     */
/* Description  : Test if input data are waiting to be processed            */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_FALSE = less than InfoStruct->ADI_BlockSize  */
/*              :                      SAMPLEs are waiting in the buffer    */
/*              : DRV_FUNCTION_OK otherwise                                 */
/* Calling      : DRV_GetInputBufStatus will be continuously called while   */
/*              : the experiment is running. Every time it returned TRUE,   */
/*              : the DRV_GetInputBuf function will then be called, the     */
/*              : data be processed, and after that a call to               */
/*              : DRV_AdvanceInputBuf is made.                              */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_GetInputBufStatus(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_GetMeasInfoEx                                         */
/* Description  : Get status (running, lost interrupts, etc.) of experiment */
/* Parameters   : None.                                                     */
/* Return Value : Pointer to status description structure.                  */
/* Calling      : DRV_GetMeasInfoEx will be continuously called while the   */
/*              : experiment is running.                                    */
/* ------------------------------------------------------------------------ */

DRV_MEASINFO FAR * FAR PASCAL DRV_GetMeasInfoEx(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_InitDevice                                            */
/* Description  : General setup for the hardware driver                     */
/* Parameters   : lpDRV_InfoStruct pointer to a DRV_InfoStruct variable in  */
/*              : the calling program's data space. The calling program     */
/*              : only sets the 'Version' member before the call.           */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong                 */
/* Calling      : DRV_InitDevice is the very first function called.         */
/*              : It checks the version number for correctness and then it  */
/*              : fills up the DRV_InfoStruct structure for the calling     */
/*              : program. If the driver uses ADI buffering, a default size */
/*              : buffer is also allocated here.                            */
/* Hints        : The driver saves the adress of the DRV_InfoStruct in a    */
/*              : local variable for later communication with the calling   */
/*              : program.                                                  */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_InitDevice(DRV_INFOSTRUCT FAR *lpDRV_InfoStruct);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_KillDevice                                            */
/* Description  : Prepare for system exit                                   */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong                 */
/* Calling      : This will be called at the very end, just before DASYLab  */
/*              : exits.                                                    */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_KillDevice(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_LoadWorksheet                                         */
/* Description  : Notification: A new worksheet has been loaded             */
/* Parameters   : Name of Worksheet.                                        */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_LoadWorksheet(const char FAR *Name);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_NewWorksheet                                          */
/* Description  : Notification: A new worksheet will be created             */
/* Parameters   : None.                                                     */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_NewWorksheet(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_ReadCounterInput                                      */
/* Description  : Asynchronous Counter/Timer input                          */
/* Parameters   : Chan: Channel to read                                     */
/*              : lpResult: Pointer to a DWORD where to put the result      */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong                 */
/* Calling      : This may be called only if counter channels are present,  */
/*              : but the SUPPORT_NO_SYNC_CT flag is set in the features.   */
/*              : In that case the function will be called asynchronously   */
/*              : from time to time.                                        */
/* ------------------------------------------------------------------------ */

DWORD FAR PASCAL DRV_ReadCounterInput(UINT chan);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_ReadDigitalInput                                      */
/* Description  : Asynchronous digital input                                */
/* Parameters   : Chan: Channel to read                                     */
/*              : lpResult: Pointer to a DWORD where to put the result      */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong                 */
/* Calling      : This may be called only if digital channels are present,  */
/*              : but the SUPPORT_NO_SYNC_DI flag is set in the features.   */
/*              : In that case the function will be called asynchronously   */
/*              : from time to time.                                        */
/* ------------------------------------------------------------------------ */

DWORD FAR PASCAL DRV_ReadDigitalInput(UINT chan);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_SaveWorksheet                                         */
/* Description  : A worksheet will be saved soon                            */
/* Parameters   : Name of Worksheet.                                        */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_SaveWorksheet(const char FAR *Name);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_SetAnalogOutputBufferMode                             */
/* Description  : Change the size of the AO buffer                          */
/* Parameters   : nSamples: New size in SAMPLEs                             */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong (out of mem)    */
/* Calling      : This may be called at any time. If the request cannot be  */
/*              : satisfied (i.e. memory shortage) the driver is free to    */
/*              : modify the buffer size.                                   */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_SetAnalogOutputBufferMode(UINT Mode, DWORD nSamples,
                                               DWORD StartDelay);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_SetCounterOutputBufferMode                            */
/* Description  : Change the size of the DO buffer                          */
/* Parameters   : nSamples: New size in SAMPLEs                             */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong (out of mem)    */
/* Calling      : This may be called at any time. If the request cannot be  */
/*              : satisfied (i.e. memory shortage) the driver is free to    */
/*              : modify the buffer size.                                   */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_SetCounterOutputBufferMode(UINT Mode, DWORD nSamples,
                                                DWORD StartDelay);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_SetDigitalOutputBufferMode                            */
/* Description  : Change the size of the DO buffer                          */
/* Parameters   : nSamples: New size in SAMPLEs                             */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong (out of mem)    */
/* Calling      : This may be called at any time. If the request cannot be  */
/*              : satisfied (i.e. memory shortage) the driver is free to    */
/*              : modify the buffer size.                                   */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_SetDigitalOutputBufferMode(UINT Mode, DWORD nSamples,
                                                DWORD StartDelay);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_SetInputBufferSize                                    */
/* Description  : Change the size of the input buffer                       */
/* Parameters   : nSamples: New size in SAMPLEs                             */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong (out of mem)    */
/* Calling      : This may be called at any time. If the request cannot be  */
/*              : satisfied (i.e. memory shortage) the driver is free to    */
/*              : modify the buffer size.                                   */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_SetInputBufferSize(DWORD nSamples);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_SetScanList                                           */
/* Description  : Get List of Hardware Components                           */
/* Return Value : List Pointer or NULL                                      */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_SetScanList(UINT ScanListLength,
                                 UINT ScanListMaxGroupSize,
                                 SCAN_ENTRY FAR *ScanList);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_ShowDialog                                            */
/* Description  : Show a driver specific dialog box                         */
/* Parameters   : DlgNumber: Number of the dialog; e.g. DRV_DLG_CARD_DEF    */
/*              : lParam: Dialog-specific data (if needed)                  */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong                 */
/* Calling      : This may be called from time to time. Currently DASYLab   */
/*              : will not call this function while the experiment is       */
/*              : running, but we may eventuelly change this in the future  */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_ShowDialog(UINT DlgNumber, DWORD lParam);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_ShowError                                             */
/* Description  : Show an error dialog box                                  */
/* Parameters   : None. The error code in DRV_InfoStruct.Error is used to   */
/*              : show the corrosponding error box.                         */
/* Return Value : always DRV_FUNCTION_OK                                    */
/* Calling      : After a driver function returned DRV_FUNCTION_FALSE the   */
/*              : calling program may call DRV_ShowError to display an      */
/*              : error box on the screen. DRV_ShowError also resets the    */
/*              : DRV_InfoStruct.Error component to zero.                   */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_ShowError(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_StartMeas                                             */
/* Description  : Start the experiment                                      */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong                 */
/* Calling      : DASYLab will always call DRV_TestStruct before calling    */
/*              : DRV_StartMeas.                                            */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_StartMeas(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_StopMeas                                              */
/* Description  : Stop the experiment                                       */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong                 */
/* Calling      : DASYLab will call this function on the stop or pause case */
/*              : The driver should *not* flush it's buffers here, but only */
/*              : stop data acquisition. The remaining dada in the buffer   */
/*              : may still be processed by DASYLab even after calling      */
/*              : DRV_StopMeas.                                             */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_StopMeas(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_TestStruct                                            */
/* Description  : Test and correct integrity of InfoStruct                  */
/* Parameters   : None.                                                     */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong or some values  */
/*              :     inside the InfoStruct had to be changed.              */
/* Calling      : DASYLab will call this function at start of experiment    */
/*              : and in other cases, such as after loading a flow chart,   */
/*              : after changes were made in the analog input setup etc.    */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_TestStruct(void);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_WriteAO                                               */
/* Description  : Asynchronous analog output                                */
/* Parameters   : Chan: Channel to write                                    */
/*              : OutVal: Value to output                                   */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong                 */
/* Calling      : This will be called if the user activates ascnchronous    */
/*              : analog output.                                            */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_WriteAnalogOutput(UINT Chan, DWORD OutVal);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_WriteCounterOutput                                    */
/* Description  : Asynchronous counter output                               */
/* Parameters   : Chan: Channel to write                                    */
/*              : OutVal: Value to output                                   */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong                 */
/* Calling      : This will be called if the user activates ascnchronous    */
/*              : digital output.                                           */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_WriteCounterOutput(UINT Chan, DWORD OutVal);

/* ------------------------------------------------------------------------ */
/* Function     : DRV_WriteDigitalOutput                                    */
/* Description  : Asynchronous digital output                               */
/* Parameters   : Chan: Channel to write                                    */
/*              : OutVal: Value to output                                   */
/* Return Value : DRV_FUNCTION_OK    = everything is fine                   */
/*              : DRV_FUNCTION_FALSE = something went wrong                 */
/* Calling      : This will be called if the user activates ascnchronous    */
/*              : digital output.                                           */
/* ------------------------------------------------------------------------ */

int FAR PASCAL DRV_WriteDigitalOutput(UINT Chan, DWORD OutVal);

/*********************************/

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif   /* __cplusplus */

/* Switch back to default packing */
#ifndef RC_INVOKED
#pragma pack()
#endif  /* RC_INVOKED */

#endif /* ! defined(_TREIBER_H) */

/* -eof- */
