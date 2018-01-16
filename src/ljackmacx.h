/*	File:		LabJack.h	Header file for LabJack.c	Written by Dave Fleck (dfleck@ieee.org) for Mac 9, use at your own peril!*/#include <Carbon/Carbon.h>// usefull defines for calls#define	DEMO_OFF		((long)0)#define	DEMO_ON		((long)1)#define	LED_OFF		((long)0)#define	LED_ON		((long)1)#define	ENABLE_CAL	((long)0)#define	DISABLE_CAL	((long)1)#define	READ_ONLY	((long)0)#define	UPDATE_IO	((long)1)#define	LEAVE_COUNT	((long)0)#define	RESET_COUNTER	((long)1)#define	DISABLE_STROBE	((long)0)#define	ENABLE_STROBE	((long)1)#define	LOCKED			((long)0)#define	SINGLE_CHANNEL	((long)1)// errors I added#define	ILLEGAL_PIN_DESCRIPTOR	(long)100#define	NO_GAIN_ON_SINGLE_ENDED	(long)101#define	OUTPUT_PIN_OVERLOAD		(long)102#define	START_BEFORE_SAMPLE		(long)103#define	START_BEFORE_STOP			(long)104#define	ALREADY_STREAMING			(long)105// defined for compatibility#define HANDLE void**#define DWORD unsigned long#define USHORT unsigned short#define TRUE 1#define FALSE 0#define __cdecl #define NO_ERROR_LJ (long)0   //must be 0#define UNKNOWN_ERROR_LJ (long)1#define NO_DEVICES_FOUND_LJ (long)2#define DEVICE_N_NOT_FOUND_LJ (long)3#define SET_BUFFER_ERROR_LJ (long)4#define OPEN_HANDLE_ERROR_LJ (long)5#define CLOSE_HANDLE_ERROR_LJ (long)6#define INVALID_ID_LJ (long)7#define ARRAY_SIZE_OR_VALUE_ERROR_LJ (long)8#define INVALID_POWER_INDEX_LJ (long)9#define FCDD_SIZE_LJ (long)10#define HVC_SIZE_LJ (long)11#define READ_ERROR_LJ (long)12#define READ_TIMEOUT_ERROR_LJ (long)13#define WRITE_ERROR_LJ (long)14#define FEATURE_ERROR_LJ (long)15#define ILLEGAL_CHANNEL_LJ (long)16#define ILLEGAL_GAIN_INDEX_LJ (long)17#define ILLEGAL_AI_COMMAND_LJ (long)18#define ILLEGAL_AO_COMMAND_LJ (long)19#define BITS_OUT_OF_RANGE_LJ (long)20#define ILLEGAL_NUMBER_OF_CHANNELS_LJ (long)21#define ILLEGAL_SCAN_RATE_LJ (long)22#define ILLEGAL_NUM_SAMPLES_LJ (long)23#define AI_RESPONSE_ERROR_LJ (long)24#define RAM_CS_ERROR_LJ (long)25#define AI_SEQUENCE_ERROR_LJ (long)26#define NUM_STREAMS_ERROR_LJ (long)27#define AI_STREAM_START_LJ (long)28#define PC_BUFF_OVERFLOW_LJ (long)29#define LJ_BUFF_OVERFLOW_LJ (long)30#define STREAM_READ_TIMEOUT_LJ (long)31#define ILLEGAL_NUM_SCANS_LJ (long)32#define NO_STREAM_FOUND_LJ (long)33#define ILLEGAL_INPUT_ERROR_LJ (long)40#define ECHO_ERROR_LJ (long)41#define DATA_ECHO_ERROR_LJ (long)42#define RESPONSE_ERROR_LJ (long)43#define ASYNCH_TIMEOUT_ERROR_LJ (long)44#define ASYNCH_START_ERROR_LJ (long)45#define ASYNCH_FRAME_ERROR_LJ (long)46#define ASYNCH_DIO_CONFIG_ERROR_LJ (long)47#define INPUT_CAPS_ERROR_LJ (long)48#define OUTPUT_CAPS_ERROR_LJ (long)49#define FEATURE_CAPS_ERROR_LJ (long)50#define NUM_CAPS_ERROR_LJ (long)51#define GET_ATTRIBUTES_ERROR_LJ (long)52#define WRONG_FIRMWARE_VERSION_LJ (long)57#define DIO_CONFIG_ERROR_LJ (long)58#define CLAIM_ALL_DEVICES_LJ (long)64#define RELEASE_ALL_DEVICES_LJ (long)65#define CLAIM_DEVICE_LJ (long)66#define RELEASE_DEVICE_LJ (long)67#define CLAIMED_ABANDONED_LJ (long)68#define LOCALID_NEG_LJ (long)69#define STOP_THREAD_TIMEOUT_LJ (long)70#define TERMINATE_THREAD_LJ (long)71#define FEATURE_HANDLE_LJ (long)72#define CREATE_MUTEX_LJ (long)73#define SYNCH_CSSTATETRIS_ERROR_LJ (long)80#define SYNCH_SCKTRIS_ERROR_LJ (long)81#define SYNCH_MISOTRIS_ERROR_LJ (long)82#define SYNCH_MOSITRIS_ERROR_LJ (long)83#define SHT1X_CRC_ERROR_LJ (long)89#define SHT1X_MEASREADY_ERROR_LJ (long)90#define SHT1X_ACK_ERROR_LJ (long)91#define SHT1X_SERIAL_RESET_ERROR_LJ (long)92#define STREAMBUFF_ERROR_OFFSET_LJ (long)256  //bit 8 is set for errors in the stream thread#define GETFIRMWAREVERSION_ERROR_OFFSET_LJ (long)512  //bit 9 is set#define WINDOWS_ERROR_OFFSET_LJ (long)1024  //bit 10 is set for Windows API errors#define RAM_BUFF_ROWS 64#define MAX_DEVICES_LJ				100			//driver name convention determines thislong EAnalogIn(long *, long, long, long, long *, float *);long EAnalogOut(long *idnum,								 long demo,								 float analogOut0,								 float analogOut1);long ECount	(long *idnum,							 long demo,							 long resetCounter,							 double *count,							 double *ms);long EDigitalIn(long *idnum,								 long demo,								 long channel,								 long readD,								 long *state);long EDigitalOut(long *idnum,								  long demo,								  long channel,								  long writeD,								  long state);long AsynchConfig(	long *idnum,									 long demo,									 long timeoutMult,									 long configA,									 long,									 long,									 long,									 long,									 long,									 long,									 long,									 long);long Asynch(	long *,							 long,							 long,							 long,							 long,							 long,							 long,							 long,							 long,							 long *);long		AISample(long *, long, long *, long, long, long, long *,					  long *, long, long *, float *);long		AOUpdate(long *, long, long, long, long *, long *, long, long,					  unsigned long *, float, float);float		GetFirmwareVersion (long *);float		GetDriverVersion(void);void		GetErrorString	(long , char *);long		GetWinVersion(unsigned long *, unsigned long *, unsigned long *,						unsigned long *, unsigned long *, unsigned long *);float		GetDriverVersion(void);long		LocalID(long *idnum, long localID);long		ReEnum(long *idnum);long		Reset(long *idnum);long		Watchdog(long *, long, long, long, long, long,					  long, long, long, long, long);long		Counter(long *, long, long *, long *,					 long, long, unsigned long *);long		DigitalIO(long *, long, long *, long,					   long *, long *, long, long *);					   long		AIBurst(long *, long, long, long, long,					 long, long *, long *, float *, long,					 long, long, long, long, float (*)[4],					 long *, long *, long);long		AIStreamStart(long *, long, long, long,						long, long, long *, long *, float *,						long, long, long);long		AIStreamRead(long, long, long, float (*)[4],						long *, long *, long *, long *);long		AIStreamClear(long localID);long		ListAll(long *, long *, long *, long *,					 long (*)[20], long *, long *, long *);