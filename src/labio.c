#include "impdefs.h"

#ifdef LJU3
#include "u3.h"
#include <unistd.h>
#endif

#include "ljackmacx.h"


#include <termios.h> // POSIX terminal control definitions


#define SER_BUF_LEN 800
unsigned char 	inbuf[SER_BUF_LEN];
unsigned char 	sendbuf[SER_BUF_LEN];


int printf();

void testMain(void)
{	long	idnum = -1;
	long	errorCode;
	char	errorString[255];

		long	stateIO = 0;
		long	channels[] = {0,0,0,0};
		long	gains[] = {0,0,0,0};
		long	overVoltage[4];
		float	voltages[] = {0.0, 0.0, 0.0, 0.0};
		
		float		rate = 500.0;
		long		numChannels = 2;
		//long		idnum = -1;
		
	// turn LED off
	{


		printf("Turn LED off and sample A0\n");

		idnum = -1;
		errorCode = AISample(&idnum, 0, &stateIO, (long)0, LED_OFF, (long)1,
				&channels[0], &gains[0], DISABLE_CAL, &overVoltage[0], &voltages[0]);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
		}
		else
		{
			printf("A0 in = %f\n", voltages[0]);
		}

		printf("Hit return\n");
		fflush(nil);
//		getchar();
	}

	// turn LED on
	{
		long	stateIO = 0;
		long	channels[] = {0,0,0,0};
		long	gains[] = {0,0,0,0};
		long	overVoltage[4];
		float	voltages[] = {0.0, 0.0, 0.0, 0.0};

		printf("Turn LED on and sample A0\n");

		idnum = -1;
		errorCode = AISample(&idnum, 0, &stateIO, (long)0, LED_ON, (long)1,
				&channels[0], &gains[0], DISABLE_CAL, &overVoltage[0], &voltages[0]);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
		}
		else
		{
			printf("A0 in = %f\n", voltages[0]);
		}

		printf("Hit return\n");
		fflush(nil);
//		getchar();
	}

	// Print version
	{
		float	version;

		printf("Print version\n");

		idnum = -1;
		version = GetFirmwareVersion (&idnum);

		printf("version %f\n\n",version);
	}

	// test local ID
	// attempt to turn led off on wrong local ID

	// had trouble with this - so I never got it working
	// (it didn't work on 9 either)
/*
	{
		long	stateIO = 0;
		long	channels[] = {0,0,0,0};
		long	gains[] = {0,0,0,0};
		long	overVoltage[4];
		float	voltages[] = {0.0, 0.0, 0.0, 0.0};
		float	voltages2[] = {0.0, 0.0, 0.0, 0.0};

		printf("Attempt LocalID 7 - this should fail\n");

		idnum = 7;
		errorCode = AISample(&idnum, 0, &stateIO, (long)0, LED_OFF, (long)1,
				&channels[0], &gains[0], DISABLE_CAL, &overVoltage[0], &voltages[0]);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
		}

		// change localID
		printf("Set local ID 7\n");
		idnum = -1;
		errorCode = LocalID(&idnum, 7);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
		}

		printf("Reenumerate to get new local ID\n");
		idnum = -1;
		errorCode = ReEnum(&idnum);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
		}

		// test new local ID
		// turn LED off
		printf("Turn LED off for new Local ID\n");

		idnum = 7;
		errorCode = AISample(&idnum, 0, &stateIO, (long)0, LED_OFF, (long)1,
				&channels[0], &gains[0], DISABLE_CAL, &overVoltage[0], &voltages2[0]);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
		}

		// change localID
		printf("Set back to local ID 0\n");

		idnum = 7;
		errorCode = LocalID(&idnum, 0);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
		}

		printf("Reenumerate to get new local ID\n");
		idnum = -1;
		errorCode = ReEnum(&idnum);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
		}

		printf("Hit return\n");
		fflush(nil);
//		getchar();
	}
*/
	{
		long	productIDList[MAX_DEVICES_LJ];
		long	serialnumList[MAX_DEVICES_LJ];
		long	localIDList[MAX_DEVICES_LJ];
		long	powerList[MAX_DEVICES_LJ];
		long	calMatrix[MAX_DEVICES_LJ][20];
		long	numberFound;
		long	fcddMaxSize;
		long	hvcMaxSize;
		short	loop1;
		short	loop2;

		printf("List all units\n");

		for (loop1 = 0; loop1 < MAX_DEVICES_LJ; loop1++)
		{
			productIDList[loop1] = 0;
			serialnumList[loop1] = 0;
			localIDList[loop1] = 0;
			powerList[loop1] = 0;
			for (loop2 = 0; loop2 < 20; loop2++)
			{
				calMatrix[loop1][loop2] = 0;
			}
		}

		errorCode = ListAll((long *)&productIDList[0],
					 (long *)&serialnumList[0],
					 (long *)&localIDList[0],
					 (long *)&powerList[0],
					 (long (*)[20])&calMatrix[0][0],
					 &numberFound,
					 &fcddMaxSize,
					 &hvcMaxSize);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
		}
		else
		{
			for (loop1 = 0; loop1 < numberFound; loop1++)
			{
				printf("found serial number %ld\n",serialnumList[loop1]);
			}
		}
		printf("\n"); 
	}

	// Burst data
	{
//		float	voltages[4096][4];
		float	(*voltages)[4];
		long	channels[] = {7,0,0,0};
		long	values[4096];
		long	gains[] = {0,0,0,0};
		long	overVoltage[4];
		short	loop;
		short	loop2;
		float	rate = 1000.0;

		printf("Burst data from A7\n");

		voltages = (float (*)[4])malloc(sizeof(float) * 4096 * 4);
		if (!voltages)
		{
			printf("\n MEMORY ALLOCATION ERROR\n");
		}
		else
		{
			idnum = -1;
			for (loop = 0; loop < 4096; loop++)
			{
				values[loop] = 0;
				for (loop2 = 0; loop2 < 4; loop2++)
				{
					voltages[loop][loop2] = 0;
				}
			}
			errorCode = AIBurst(&idnum, (long)0, (long)0, (long)0, LED_ON,
									(long)1, &channels[0], &gains[0], &rate,
									DISABLE_CAL, (long)0, (long)0, (long)555, (long)20,
									 voltages, &values[0],  &overVoltage[0], (long)1);
			if (errorCode != NO_ERROR_LJ)
			{
				GetErrorString	(errorCode, (char *)&errorString);
				printf("Error: %s\n", (char *)&errorString);
			}
			else
			{
				for (loop = 0; loop < 20; loop++)
				{
					printf("Value %d is %f\n",loop, voltages[loop][0]);
				}
			}
			free (voltages);
		}

		printf("\n");
	}

	// set an analogue output and read it back
	{

		long				stateD = 0;
		long				stateIO = 0;
		unsigned long	count;

		long	channels[] = {2,0,0,0};
		long	gains[] = {0,0,0,0};
		long	overVoltage[4];
		float	voltages[] = {0.0, 0.0, 0.0, 0.0};

		printf("Set AO0 to 2.5V, AO1 to 3.75V\n");

		idnum = -1;
		errorCode = AOUpdate(&idnum, (long)0,
					  (long)0, (long)0,
					  &stateD,
					  &stateIO,
					  (long)0,
					  (long)0,
					  &count,
					  2.5,
					  3.75);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
		}

		idnum = -1;
		stateIO = 0;
		errorCode = AISample(&idnum, 0, &stateIO, (long)0, LED_OFF, (long)1,
				&channels[0], &gains[0], DISABLE_CAL, &overVoltage[0], &voltages[0]);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
		}
		else
		{
			printf("A2 in = %f\n", voltages[0]);
		}

		printf("Hit return\n");
		fflush(nil);
//		getchar();
	}


	// turn LED on
	{
		long	stateIO = 0;
		long	channels[] = {0,0,0,0};
		long	gains[] = {0,0,0,0};
		long	overVoltage[4];
		float	voltages[] = {0.0, 0.0, 0.0, 0.0};

		printf("Turn LED on and sample A0\n");

		idnum = -1;
		errorCode = AISample(&idnum, 0, &stateIO, (long)0, LED_ON, (long)1,
				&channels[0], &gains[0], DISABLE_CAL, &overVoltage[0], &voltages[0]);
		if (errorCode != NO_ERROR_LJ)
		{
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n",(char *) &errorString);
		}
		else
		{
			printf("A0 in = %f\n", voltages[0]);
		}
	}
	
	{
		printf("\nStream tests\n");

		
		{ 
			long		channels[4] = {0, 1, 2, 3};
			long		gain[4] = {0, 0, 0, 0};
			long		xferMode = 2; 		// turbo mode

			printf("Start Stream\n");

			errorCode = AIStreamStart(&idnum, DEMO_OFF, (long)0, READ_ONLY, LED_ON,
							numChannels, &channels[0], &gain[0], &rate, ENABLE_CAL,
							xferMode, (long)0);
			if (errorCode != NO_ERROR_LJ)
			{
				GetErrorString	(errorCode, (char *)&errorString);
				printf("Error: %s\n",(char *) &errorString);
			}
		}

		{
			short loop3;
			
			for (loop3 = 0; loop3 < 3; loop3++)
			{
				float		(*voltages)[4];
				long		overVoltage;
				long		*values=0;
				long		pcScanBacklog;
				long		ljScanBacklog;
				long		numSamples = 500;
				long		timeOut = 2; 		// 2 seconds
				short		loop;
				short		loop2;

				printf("Read Stream pass %d\n", loop3 + 1);

				voltages = (float (*)[4])malloc(sizeof(float) * 4096 * 4);
				if (!voltages)
				{
					printf("mem full error\n");
				}
				else
				{
					values = (long *)malloc(sizeof(long) * 4096);
					if (!values)
					{
						free (voltages);
						printf("mem full error\n");
					}
					else
					{
						for (loop = 0; loop < 4096; loop++)
						{
							values[loop] = 0;
							for (loop2 = 0; loop2 < 4; loop2++)
							{
								voltages[loop][loop2] = 0;
							}
						}

						errorCode = AIStreamRead(idnum, numSamples, timeOut, voltages, &values[0],
										  &pcScanBacklog, &ljScanBacklog ,&overVoltage);
						if (errorCode != NO_ERROR_LJ)
						{
							GetErrorString	(errorCode, (char *)&errorString);
							printf("Error: %s\n", (char *)&errorString);
						}
						else
						{
							for (loop = 0; loop < 20; loop++)
							{
								printf("Value %d is %f\n",loop, voltages[loop][0]);
							}
						}
					}
				}
				free (values);
				free (voltages);
			}
		}

		{
			printf("Stop Stream\n");

			errorCode = AIStreamClear(idnum);
			if (errorCode != NO_ERROR_LJ)
			{
				GetErrorString	(errorCode, (char *)&errorString);
				printf("Error: %s\n", (char *)&errorString);
			}
		}
	}
	
	printf("End of testing\n");
}

/* ********** */
// WAITHI bit_number
//	Waits until the specified digital I/O bit is aserted high on a LabJack U12. Don't use bit_number = 2, as that is reserved for outbut using the DOUT command


int waithi(int n, int index)
{

	long	idnum = -1;
	long	errorCode;
	long 	channel;
	long 	demo = 0;
	long 	readD = 0;
	char	errorString[255];

	long	state;

	channel = n;	// 

	errorCode = EDigitalIn(&idnum,demo,channel,readD,&state);
	if (errorCode != NO_ERROR_LJ) {
		GetErrorString	(errorCode, (char *)&errorString);
		printf("Error: %s\n", (char *)&errorString);
	}
	while(state == 0 ){
		errorCode = EDigitalIn(&idnum,demo,channel,readD,&state);
		if (errorCode != NO_ERROR_LJ) {
			GetErrorString	(errorCode, (char *)&errorString);
			printf("Error: %s\n", (char *)&errorString);
			return -1;
		}
	}
	//printf("%d %d \n",idnum,state);
	return 0;
}

/* ********** */

// DOUT state
//	Sets digital I/O bit 2 to low (state = 0) or high (state = 1) on a LabJack U12

int dout(int n, int index)
{
	long	idnum = -1;
	long	errorCode;
	long 	channel;
	long 	demo = 0;
	long 	writeD = 0;
	char	errorString[255];

	long	state;

	channel = 2;	//
	state = n;

	errorCode = EDigitalOut(&idnum,demo,channel,writeD,state);
	if (errorCode != NO_ERROR_LJ) {
		GetErrorString	(errorCode, (char *)&errorString);
		printf("Error: %s\n", (char *)&errorString);
		return -1;
	}
	return 0;
}
/* ********** */

// AIN channel
//	Read Analog input on a LabJack U12

int ain(int n, int index)
{

	long idnum = -1;
	long demo=0;
	long channel;
	long gain=0;
	long overVoltage;
	float voltage;
	long	errorCode;
	char	errorString[255];
    static int no_u3 = 1;

	extern Variable user_variables[];
	
	channel = n;
    
#ifdef LJU3
    
	int localID;
	static u3CalibrationInfo caliInfo;
	static HANDLE hDevice; 
    static long DAC1Enable;
    long error;
    static int u3_connected = 0;
    double dblVoltage;
    
	//Open first found U3 over USB
    if(u3_connected == 0){
        localID = -1;
        hDevice = openUSBConnection(localID);
        
        if(hDevice == NULL) {
            no_u3=1;
            //closeUSBConnection(hDevice);
            printf("No Labjack U3.\n");
        } else {
            no_u3=0;
            u3_connected = 1;
            if(getCalibrationInfo(hDevice, &caliInfo) < 0)
                goto close;
            if( (error = eAIN(hDevice, &caliInfo, 1, &DAC1Enable, 3, 31, &dblVoltage, 0, 0, 0, 0, 0, 0)) != 0 )goto close;
            printf("AIN3 value = %.3f\n", dblVoltage);

        }
    } else {
        
        if(n <0){   // unhook from this
            closeUSBConnection(hDevice);
            u3_connected=0;
            return 0;
        }
    
        no_u3=0;
        //printf("\nCalling eAIN to read voltage from AIN3\n");

    if( (error = eAIN(hDevice, &caliInfo, 0, &DAC1Enable, 3, 31, &dblVoltage, 0, 0, 0, 0, 0, 0)) != 0 )
        goto close;
    printf("AIN3 value = %.3f\n", dblVoltage);
    }
	
    
    //printf("Calling eDAC to set DAC0 to 2.1 V\n");
	
close:

    if(error != 0 && u3_connected == 1){
        printf("Received an error code of %ld\n", error);
        closeUSBConnection(hDevice);
        u3_connected=0;
    }
    user_variables[0].fvalue = dblVoltage;
	user_variables[0].is_float = 1;

    
    /* Note: The eAIN, eDAC, eDI, and eDO "easy" functions have the ConfigIO
     parameter.  If calling, for example, eAIN to read AIN3 in a loop, set the
     ConfigIO parameter to 1 (True) on the first iteration so that the
     ConfigIO low-level function is called to ensure that channel 3 is set to
     an analog input.  For the rest of the iterations in the loop, set the
     ConfigIO parameter to 0 (False) since the channel is already set as
     analog. */
    
    
    /* Note: The eAIN "easy" function has the DAC1Enable parameter that needs to
     be set to calculate the correct voltage.  In addition to the earlier
     note, if running eAIN in a loop, set ConfigIO to 1 (True) on the first
     iteration to also set the output of the DAC1Enable parameter with the
     current setting on the U3.  For the rest of the iterations, set ConfigIO
     to 0 (False) and use the outputted DAC1Enable parameter from the first
     interation from then on.  If DAC1 is enabled/disabled from a later eDAC
     or ConfigIO low-level call, change the DAC1Enable parameter accordingly
     or make another eAIN call with the ConfigIO parameter set to 1. */
    
    if(!no_u3) return error;

    
#endif    
    
    
	errorCode = EAnalogIn(&idnum, demo, channel, gain, &overVoltage, &voltage);

	if (errorCode != NO_ERROR_LJ)
	{
		GetErrorString	(errorCode, (char *)&errorString);
		printf("Error: %s\n", (char *)&errorString);
	}
	else
	{
		printf("AI%d in = %f\n", channel,voltage);
	}
	user_variables[0].fvalue = voltage;
	user_variables[0].is_float = 1;

	return errorCode;
   
}


// AINBURST num_channels, num_points, rate
//	Read Analog input on a LabJack U12 in burst mode
// num_channels: must be 1, 2 or 4
// num_points: acquired for each channel
// rate: scan rate 

long AIBurst(long *idnum,
					 long demo,
					 long stateIOin,
					 long updateIO,
					 long ledOn,
					 long numChannels,
					 long *channels,
					 long *gains,
					 float *scanRate,
					 long disableCal,
					 long triggerIO,
					 long triggerState,
					 long numScans,
					 long timeout,
					 float (*voltages)[4],
					 long *stateIOout,
					 long *overVoltage,
					 long transferMode);
					 
int ainburst(int n, int index)
{

	 float rate = 1000.;			// samples per second
	 char	errorString[255];

	 long	errorCode;
	 long idnum =  -1;					 
	 long demo=0;
	 long stateIOin = 0;
	 long updateIO = 0;
	 long ledOn = 1;
	 long numChannels = 1;
	 long channels[4] = {0,1,2,3};	// single ended
	 long gains[4] ={ 0};     // meaningless for single-ended input
	// float *scanRate;
	//	scanRate = &rate;
	 long disableCal = 1;	// raw readings
	 long triggerIO = 0;	// not triggered
	 long triggerState = 0;
	 long numScans = 1024;	// number of points collected
	 long timeout = 4;	// function timeout in seconds
	 //float (*voltages)[4];
	 long stateIOout[4096] = {0};
	 long overVoltage=0;
	 long transferMode = 1;	// normal mode
	 float voltages[4096][4] ={ {0.}};
	int i,j=0,nc,ns,nt;
	
	extern Variable user_variables[];
	extern char cmnd[];
	extern DATAWORD *datpt;
	extern int	doffset;
	extern int have_max;
	extern int npts;
	extern TWOBYTE header[];
	DATAWORD *datpt2;
 
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			j = sscanf(&cmnd[index],"%d %d %f",&nc,&ns,&rate);
			break;
		}
	}
	
	if( j != 3){
		beep();
		printf(" 3 arguments needed: num_channels, num_points, rate\n");
		return(-1);
	}
	numChannels = nc ;
	numScans = ns;
	
	if( numChannels != 1 && numChannels != 2 && numChannels != 4) {
		beep();
		printf("num_channels must be 1, 2 or 4.\n");
		return(-1);
	}
	
	if( rate*numChannels < 400. || rate*numChannels > 8192.) {
		beep();
		printf("num_channels*rate must be 400-8192.\n");
		return(-1);
	}

	if( numChannels*numScans < 1 || numChannels*numScans > 4096) {
		beep();
		printf("num_channels*num_points must be 1-4096.\n");
	}

	errorCode = AIBurst(&idnum,demo,stateIOin,updateIO,ledOn,numChannels,channels,gains,&rate,disableCal,triggerIO,triggerState,numScans,
		timeout,voltages,stateIOout,&overVoltage,transferMode);

	if (errorCode != NO_ERROR_LJ)
	{
		GetErrorString	(errorCode, (char *)&errorString);
		printf("Error: %s\n", (char *)&errorString);
		return -1;
	}

//	data goes to oma data buffer

	nt = header[NTRAK];
	nc = header[NCHAN];
	
	header[NCHAN] = numScans;	/* the number of x pixels in new image */
	header[NTRAK] = numChannels;	/* the number of y pixels in new image */
	npts = header[NTRAK]*header[NCHAN];

	if ( checkpar() == 1) {
			beep();
			printf("Not enough memory for %d x %d image.\n", header[NCHAN],header[NTRAK]);
            header[NCHAN] = nc;
	        header[NTRAK] = nt;
			npts = nc*nt;
	}
	header[NX0] = header[NY0] = 0;
	header[NDX] = header[NDY] = 1;
	
	datpt2 = datpt+doffset;
	
	for (j = 0; j< header[NTRAK]; j++){
		for(i = 0; i< header[NCHAN]; i++){
			*datpt2++ = voltages[i][j];
		}
	}
	have_max = 0;
	maxx();
	


	return 0;
}

/* ********** */
#ifdef NI-DAQ
#define _NI_int8_DEFINED_
#define _NI_int16_DEFINED_
#define _NI_int32_DEFINED_

#include "NIDAQmxBase.h"
#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }

int send_NIDAQ(float volts,int aochan, int dev){
    
    // Task parameters
    int32       error = 0;
    TaskHandle  taskHandle = 0;
    char        errBuff[2048]={'\0'};
    
    // Channel parameters
    char        chan[256];  // = "Dev1/ao0";
    float64     min = 0.0;
    float64     max = 5.0;
    
    // Timing parameters
    uInt64      samplesPerChan = 1;
    
    // Data write parameters
    float64     data = volts;
    int32       pointsWritten;
    float64     timeout = 10.0;
    
    sprintf(chan,"Dev%d/ao%d",dev,aochan);
    
    
    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateAOVoltageChan(taskHandle,chan,"",min,max,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));
    
    DAQmxErrChk (DAQmxBaseWriteAnalogF64(taskHandle,samplesPerChan,0,timeout,DAQmx_Val_GroupByChannel,&data,&pointsWritten,NULL));
    
Error:
    if( DAQmxFailed(error) )
	DAQmxBaseGetExtendedErrorInfo(errBuff,2048);
    if( taskHandle!=0 ) {
	DAQmxBaseStopTask(taskHandle);
	DAQmxBaseClearTask(taskHandle);
    }
    if( DAQmxFailed(error) ){
	printf ("DAQmxBase Error %d: %s\n", error, errBuff);
	return DAQmxFailed(error);
    }
    return 0;
    
    
}
#endif
/*
AOUTPUT v1 v2 [v3 v4]
	Sends voltages to D/A converters 0 and 1 on a Labjack USB analog/digital I/O device.
 	If a Labjack U3 is present, v1 and v2 are sent to it.
	If there is no U3 but there is a Labjack U12, v1 and v2 are sent to the U12.
	If both U3 and U12 Labjacks are present and all 4 voltages are given, v1 and v2 are sent to the U3 and v3 and v4 are sent to the U12.
*/

int aout(int n, int index)
{

	long idnum = -1;
	long demo=0;
	float v1=0.,v2=0.,v3=0.,v4=0.;
	long	errorCode=0;
	char	errorString[255];


	int no_u3=1;
    
	#ifdef LJU3
	int localID;
	u3CalibrationInfo caliInfo;
	HANDLE hDevice;
	#endif	

	extern Variable user_variables[];
	extern char cmnd[];

	int i, j=0;
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			j = sscanf(&cmnd[index],"%f %f %f %f",&v1,&v2,&v3,&v4);
			break;
		}
	}

#ifdef LJU3	
	//Open first found U3 over USB
	localID = -1;
	hDevice = openUSBConnection(localID);
	
	if(hDevice == NULL) {
		no_u3=1;
		closeUSBConnection(hDevice);
	} else {
	  no_u3=0;
	  if(getCalibrationInfo(hDevice, &caliInfo) < 0)
			goto close;
	  if((errorCode = eDAC(hDevice, &caliInfo, 0, 0,v1, 0, 0, 0)) != 0)
		goto close;
	  errorCode = eDAC(hDevice, &caliInfo, 1, 1,v2, 0, 0, 0);
	}

  //printf("Calling eDAC to set DAC0 to 2.1 V\n");
	
close:
  if(errorCode > 0)
    printf("Received an error code of %ld\n", errorCode);
  closeUSBConnection(hDevice);
 #endif
  
	// go on to see if there is a u12
	if(no_u3){
		errorCode = EAnalogOut(&idnum, demo, v1,v2);
	}
	if(no_u3==0 && j>2){
		errorCode = EAnalogOut(&idnum, demo, v3,v4);
	}

	if (errorCode != NO_ERROR_LJ)
	{
#ifdef NI-DAQ
	    // check if there is a national instruments DAQ device
	    // assume we don't mix NI-DAQs and Labjacks
	    NIDAQ_error = send_NIDAQ(v1,0,1);	    // try to send the first voltage to ao0
	    if(NIDAQ_error == 0) {
		NIDAQ_error = send_NIDAQ(v2,1,1);    // send the second voltage to ao1
		return NIDAQ_error;
	    }
#endif
		GetErrorString	(errorCode, (char *)&errorString);
		printf("Error: %s\n", (char *)&errorString);
	}
   
	return errorCode;

}


#ifdef SERIAL_PORT
// use unix terminal control routies
#include <termios.h> /* POSIX terminal control definitions */

int serial_fd; /* File descriptor for the port */
struct termios serial_options;
int serial_inited = 0;

int open_serial_port(char* dev_name){
	  serial_fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
      if (serial_fd == -1) {
		return -1;
      }else{
		fcntl(serial_fd, F_SETFL, 0);	
		fcntl(serial_fd, F_SETFL, FNDELAY);		// return at once from a read, even if there are no characters ready
	  }

     //Get the current options for the port...
    tcgetattr(serial_fd, &serial_options);

    // Set the baud 
    cfsetispeed(&serial_options, B9600);
    cfsetospeed(&serial_options, B9600);
    //cfsetispeed(&serial_options, B19200);
    //cfsetospeed(&serial_options, B19200);

    // Enable the receiver and set local mode...

    serial_options.c_cflag |= (CLOCAL | CREAD);

	serial_options.c_cflag &= ~CSIZE; /* Mask the character size bits */
    serial_options.c_cflag |= CS8;    /* Select 8 data bits */
	serial_options.c_cflag &= ~PARENB;
	serial_options.c_cflag &= ~CSTOPB;
	serial_options.c_cflag &= ~CSIZE;
	serial_options.c_cflag |= CS8;
    //options.c_cflag &= ~CNEW_RTSCTS;
	
    // Set the new options for the port...
    tcsetattr(serial_fd, TCSANOW, &serial_options);
	return 0;
}

int serclo(int n, int index)
{
	if(serial_inited == 0) {
		beep();
		printf("Serial port not initialized.\n");
		return -1;
	} else {
		close(serial_fd);
		serial_inited = 0;
		return 0;
	}
		
		
}

int serial(int n, int index)
{
	int length,i;
	extern char cmnd[];
	char buffer[ESTRING_LENGTH];
	extern Variable user_variables[];
	
	if(serial_inited == 0) {
		if(open_serial_port(&cmnd[index]) != 0){
			beep();
			printf("Can't open serial port %s.\n",&cmnd[index]);
			return -1;
		} else {
			printf("Opened serial port %s.\n",&cmnd[index]);
			serial_inited = 1;
			return 0;
		}
	}
	length = strlen(&cmnd[index]);
	if( length == 0 ) {
		beep();
		printf("Nothing sent\n");
		return -1;
	}
	cmnd[index+length] = 13;	// add a <CR> (=13)
	length++;
	write(serial_fd, &cmnd[index], length);
	//echo any reply
	length = read(serial_fd, buffer, ESTRING_LENGTH-1);
	for(i=0;i<length;i++)buffer[i] &= 127;
	while (length > 0){
		buffer[length] = 0;
		printf("%d characters returned",length);
		pprintf("%s\n",buffer);
		strcpy(&user_variables[0].estring[0],buffer);
		user_variables[0].is_float = -1;
		length = read(serial_fd, buffer, ESTRING_LENGTH-1);

	}
	//printf("sent %d characters -%s-\n",length,&cmnd[index]);
	return 0;
}



#endif



#ifdef ST6
/*----------------------------------------------------------------*/
// this code is copied from the pre-OSX version
// it doesn't work
// it could probably be made to work by updating the serial communications stuff

#include <Serial.h>

/*	For ST-6 */

#define TAKE_IMAGE			0x1
#define SHUTTER_CONTROL		0x4
#define ACK					0x6
#define GET_ACTIVITY_STATUS	0x5
#define START_BYTE			0xa5
#define GET_UNCOMP_LINE		0x1f
#define END_EXPOSURE		0x2
#define GET_LINE			0x7
#define SET_COM_BAUD		0x1a
#define GET_ROM_VERSION		0x19
#define REGULATE_TEMP		0xe
#define READ_THERMISTER		0x1d

#define RE_TRIES		2000
#define ERR_COUNT_REC	30

short in_ref,out_ref;
int st6_inited = 0;
unsigned char	sendbuf[800];
unsigned char 	inbuf[800];

unsigned long exposure_time = 100;	/* settings */
unsigned long baud = 9600;
unsigned short enable_dcs = 1;		/* settings */
unsigned short dc_restore = 0;		/* derived */
unsigned short abg_state = 1;		/* settings */
unsigned short abg_period = 300;	/* const */
unsigned short dest_buffer = 1;		/* const */
unsigned short auto_dark = 0;		/* const */
unsigned short x2_horizontal = 1;	/* const */
unsigned short open_shutter = 1;	/* specified */
unsigned short head_offset = 167;	/* settings */
unsigned short p_gain = 1000;		/* const */
unsigned short i_gain = 200;		/* const */
unsigned short samp_rate = 10;		/* const */
unsigned short reset_brownout = 0;	/* const */
int	temp_control = 1;				/* settings */
int serial_port = 1;				/* settings */
									/* 0 for printer port, 1 for modem port */
float set_temp = -10.0;				/* settings */
float current_temp;

int re_tries = RE_TRIES*5;

/* these are the temperature set points for values from -50 to +25 in 5 degree increments */

unsigned short set_pts[] = {49354,46516,43404,40060,36550,32949,29343,25819,22457,
							19318,16451,13882,11619,9656,7975,6553,0};

/* ************ */
/*
Commands for the ST-6 detector.
Use these via the ST6 OMA command.
Format is:

ST6 command_string [parameters]
	command_string specifies a 3 character ST-6 command.
	The values for parameters depend on the particular command.
	
	
Commands:

TIM	END	STA	REC	RC2 SHT COM QCM BOD TMP

*/
st6(n,index)
int n,index;
//#pragma unused (n)

{
	extern void redoMenus();
	extern char txt[],cmnd[];
	
	if( !st6_inited ) {
		if( load_drivers() == 0 ) return;
		st6_inited = 1;
		if( enable_dcs ) 
			dc_restore = 0;
		else
			dc_restore = 1;
		redoMenus(st6Menu);
		re_tries = RE_TRIES*10;	/* a good safe value for starters */
		if( send_packet(GET_ROM_VERSION,0,txt) > 0 ) {
			printf("ST-6 Camera Found.\n");
			sprintf(txt,"TMP %d %.2f",temp_control,set_temp);
			do_st6_command(txt);
			sprintf(txt,"QCM 0f %d",head_offset);
			do_st6_command(txt);

		} else {
			printf("No ST-6 Camera Found.\n");
		}
	}
	if(index == 0) return;
	do_st6_command(&cmnd[index]);
}

do_st6_command(string)
char string[];

{

	int i,j,k,arg[16],packet_reply,error_count;
	OSErr inerr,outerr;
	int shutter = 0;
	static unsigned short status;
	static unsigned char command;
	DATAWORD *ptr2;
	unsigned short delta,base;
	unsigned short set_point,get_set_point(float);
	float exposure,get_temp();
	char txt[128];
	
	
	extern DATAWORD *datpt;
	extern int	doffset;
	extern int have_max;
	extern int npts;
	extern TWOBYTE trailer[],header[];
	
	for( i=0; string[i] != 0; i++) 
		string[i] = toupper(string[i]);
	
	if( strncmp(string,"SHT",3) == 0){

		sscanf(string,"%s %d",txt, &shutter);
		if( shutter != 0) shutter = 1;
		printf("Shutter State: %d.\n", shutter);
		txt[0] = shutter;
		txt[1] = 0;
		command = (unsigned char) SHUTTER_CONTROL;
		if( send_packet(SHUTTER_CONTROL,2,txt) == 0 ) {
			beep();
			printf("Error.\n");
			return;
		} else
			return;		

	} else if( strncmp(string,"TIM",3) == 0){
		i = sscanf(string,"%s %f %d",txt,&exposure,&j);
		if( i > 1 )	
			exposure_time = exposure * 100.0;
		if( i > 2 ) {
			if( j)
				open_shutter = 1;
			else
				open_shutter = 0;
		}
		printf("Exposure: %.2f Shutter: %d\n",exposure, open_shutter);	
		insert_value(txt,i,0);	/* initialize routine */
		
		insert_value(txt,exposure_time,4);	/* put parameters in */
		insert_value(txt,header[NY0],2);
		insert_value(txt,header[NTRAK],2);
		insert_value(txt,header[NX0],2);
		insert_value(txt,header[NCHAN],2);
		
		insert_value(txt,enable_dcs,2);
		insert_value(txt,dc_restore,2);
		insert_value(txt,abg_state,2);
		insert_value(txt,abg_period,2);
		insert_value(txt,dest_buffer,2);
		insert_value(txt,auto_dark,2);
		insert_value(txt,x2_horizontal,2);
		insert_value(txt,open_shutter,2);
		
		command = (unsigned char) TAKE_IMAGE;

		if( send_packet(TAKE_IMAGE,28,txt) ) {
			get_st6_status( TAKE_IMAGE,&status);
			while( status > 0 ) {
				printf("Status: %d.\n",status);
				if( status == 5 )
					return;
				delay(45);
				get_st6_status( TAKE_IMAGE,&status);
			}
			return;
		} else {
			beep();
			printf("Error.\n");
			return;
		}
	} else if( strncmp(string,"END",3) == 0){
		printf("End Image command.\n");
		
		insert_value(txt,i,0);	/* initialize routine */
		
		status = 0;
		insert_value(txt,status,2);	/* put parameters in */
		
		command = (unsigned char) END_EXPOSURE;

		if( send_packet(END_EXPOSURE,2,txt) ) {
			delay(10);
			get_st6_status( END_EXPOSURE,&status);
			while( status > 0 ) {
				printf("Status: %d.\n",status);
				delay(45);
				get_st6_status( END_EXPOSURE,&status);
			}
			return;
		} else {
			beep();
			printf("Error.\n");
			return;
		}
	} else if( strncmp(string,"COM",3) == 0){

		i = sscanf(string,"%s %x %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
			 txt, &j,&arg[0],&arg[1],&arg[2],&arg[3],&arg[4],&arg[5],&arg[6],&arg[7],
			 &arg[8],&arg[9],&arg[10],&arg[11],&arg[12],&arg[13],&arg[14],&arg[15]);
			 
		command = j;
		
		if( i<2 ) {
			beep();
			printf("Need more args.\n");
			return;
		}
		i -= 2;		/* now i is the number of 2 byte arguments */
		
		printf("Cmnd: %x; %d args.\n",command,i);
		
		insert_value(txt,i,0);	/* initialize routine */
		
		for (j=0; j< i; j++) {
			insert_value(txt,arg[j],2);
		}
		packet_reply = send_packet(command,2*i,txt);
		if( packet_reply == -1 ) {
			printf("ACK\n");
			return;
		} else if( packet_reply > 0) {
			for(j=0; j< packet_reply; j++) {
				printf("%x ",sendbuf[j]);
			}
			printf("\n");
			i = sendbuf[2] + 256 * sendbuf[3];	/* data length */
			for(j=1; j<=i; j++) {
				k = sendbuf[2*j+2] + 256 * sendbuf[2*j+3];
				printf("%d\t",k);
			}
			printf("\n");
			return;
		} else {
			beep();
			printf("Error.\n");
			return;
		}
	} else if( strncmp(string,"QCM",3) == 0){

		i = sscanf(string,"%s %x %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
			 txt, &j,&arg[0],&arg[1],&arg[2],&arg[3],&arg[4],&arg[5],&arg[6],&arg[7],
			 &arg[8],&arg[9],&arg[10],&arg[11],&arg[12],&arg[13],&arg[14],&arg[15]);
			 
		command = j;
		
		if( i<2 ) {
			beep();
			printf("Need more args.\n");
			return;
		}
		i -= 2;		/* now i is the number of 2 byte arguments */
				
		insert_value(txt,i,0);	/* initialize routine */
		
		for (j=0; j< i; j++) {
			insert_value(txt,arg[j],2);
		}
		packet_reply = send_packet(command,2*i,txt);
		if( packet_reply ) {
			return;
		} else {
			beep();
			printf("Error.\n");
			return;
		}
	} else if( strncmp(string,"STA",3) == 0){
		printf("Status command.\n");
		
		get_st6_status( command,&status);
		printf("Command: %x Status: %d.\n",command,status);
		return;
	} else if( strncmp(string,"BOD",3) == 0){
		i = sscanf(string,"%s %d", txt, &baud);	 
		command = SET_COM_BAUD;
		if( i<2 ) {
			beep();
			printf("Need more args.\n");
			return;
		}
		if( baud != 9600 && baud != 19200 && baud != 57600) {
			printf("Illegal baud rate.\n");
			beep();
			return;
		}
		insert_value(txt,i,0);	/* initialize routine */
		insert_value(txt,baud,4);	/* put parameters in */
		packet_reply = send_packet(command,4,txt);
		/* printf("%d reply.\n",packet_reply);*/
		if( packet_reply == -1 ) {
			switch (baud) {
				case 9600:
					inerr = SerReset(in_ref,baud9600+data8+stop20+noParity);
					outerr = SerReset(out_ref,baud9600+data8+stop20+noParity);
					re_tries = RE_TRIES*5;
					break;
				case 19200:
					inerr = SerReset(in_ref,baud19200+data8+stop20+noParity);
					outerr = SerReset(out_ref,baud19200+data8+stop20+noParity);
					re_tries = RE_TRIES*2;
					break;
				case 57600:
					inerr = SerReset(in_ref,baud57600+data8+stop20+noParity);
					outerr = SerReset(out_ref,baud57600+data8+stop20+noParity);
					re_tries = RE_TRIES;
					break;
			}
			if(inerr == noErr && outerr == noErr ){
				delay(10);		/* need a little pause here for some reason */
				if( send_packet(GET_ROM_VERSION,0,txt) ){ 
					printf("Baud Rate set to %d.\n",baud);
					return;
				}
			} else {
				printf("SerReset Error\n");
				return;
			}
		} else {
			beep();
			printf("Error. Computer set to 9600 baud.\n");
			inerr = SerReset(in_ref,baud9600+data8+stop20+noParity);
			outerr = SerReset(out_ref,baud9600+data8+stop20+noParity);
			return;
		}
	} else if( strncmp(string,"REC",3) == 0){
		printf("Receive Compressed Image command.\n");

		command = (unsigned char) GET_LINE;
		
		ptr2 = datpt+doffset;		/* a copy of the data pointer */
		trailer[SFACTR] = 1;
		have_max = 0;	
		error_count = 0;
		for( i = header[NY0]; i < (header[NY0] + header[NTRAK]); i++) {
		
			insert_value(txt,i,0);	/* initialize routine */
			insert_value(txt,dest_buffer,2);	/* put parameters in */
			insert_value(txt,i,2);
			insert_value(txt,header[NX0],2);
			insert_value(txt,header[NCHAN],2);
			packet_reply = send_packet(GET_LINE,8,txt);
			if( packet_reply ) {
				/* now have a line of data */
				status = sendbuf[4] + 256*sendbuf[5];
				printf(" %d",status);
				
				base = sendbuf[6]*256 + sendbuf[7];
				*(ptr2++) = base;
				for( j = 8; j<packet_reply - 2;) {
					if( sendbuf[j] & 0x80 ) {
						if( sendbuf[j] & 0x40 ) {
							base = ((sendbuf[j] & 0x3f)<<8) | sendbuf[j+1];
							base = base << 2;
							*(ptr2++) = base;
							j+=2;
						} else {
							delta =  ((sendbuf[j] & 0x3f)<<8) | sendbuf[j+1];
							j+=2;
							if( delta & 0x2000 ) {
								delta = ~(delta | 0xc000) + 1; /* delta negative ; >= - 8192 */
								base -= delta;	
								*(ptr2++) = base;
							} else {
								base += delta;	/* delta positive; <= 8191 */
								*(ptr2++) = base;
							}
						}
					} else {
						if( sendbuf[j] & 0x40 ) {
							delta = sendbuf[j++];
							delta = ~(delta | 0xff80) + 1; /* delta negative ; >= - 64 */
							base -= delta;	
							*(ptr2++) = base;
						} else {
							base += sendbuf[j++];	/* delta positive; <= 63 */
							*(ptr2++) = base;
						}
					}
				}				
			} else {
				/* beep();
				printf("Error on line %d.\n",i); */
				error_count++;
				i--;
				if(error_count > ERR_COUNT_REC) {
					beep();
					pprintf( "Too many errors.\n");
					return;
				}
			}
		}
		printf("\n");
		ptr2 = datpt+doffset;		/* a copy of the data pointer */
		for( i = 0; i < npts; i++) {
			if( *(ptr2++) & 0x8000 ) {
				ptr2 = datpt+doffset;		/* a copy of the data pointer */
				for( i = 0; i < npts; i++) {
					*ptr2 = (*ptr2 >> 1) & 0x7fff;
					ptr2++;
				}
				trailer[SFACTR] = 2;
				if( error_count != 0) {
					pprintf("%d Errors on Receive.\n",error_count);
				}
				return;
			}
		}
		if( error_count != 0) {
			pprintf("%d Errors on Receive.\n",error_count);
		}
		return;
	} else if( strncmp(string,"RC2",3) == 0){
		printf("Receive Image command.\n");

		command = (unsigned char) GET_UNCOMP_LINE;
		
		ptr2 = datpt+doffset;		/* a copy of the data pointer */
		trailer[SFACTR] = 1;
		have_max = 0;	
		error_count = 0;
		for( i = header[NY0]; i < (header[NY0] + header[NTRAK]); i++) {
		
			insert_value(txt,i,0);	/* initialize routine */
			insert_value(txt,dest_buffer,2);	/* put parameters in */
			insert_value(txt,i,2);
			insert_value(txt,header[NX0],2);
			insert_value(txt,header[NCHAN],2);

			if( send_packet(GET_UNCOMP_LINE,8,txt) ) {
				/* now have a line of data */
				status = sendbuf[4] + 256*sendbuf[5];
				printf("Line: %d.\n",status);
				for( j = 3; j < header[NCHAN] + 3; j++) {
					*(ptr2++) = sendbuf[j*2] + 256*sendbuf[j*2+1] ;
				}
				
			} else {
				/* beep();
				printf("Error on line %d.\n",i); */
				error_count++;
				i--;
				if(error_count > ERR_COUNT_REC) {
					beep();
					pprintf( "Too many errors.\n");
					return;
				}
			}
		}
		printf("\n");
		ptr2 = datpt+doffset;		/* a copy of the data pointer */
		for( i = 0; i < npts; i++) {
			if( *(ptr2++) & 0x8000 ) {
				ptr2 = datpt+doffset;		/* a copy of the data pointer */
				for( i = 0; i < npts; i++) {
					*ptr2 = (*ptr2 >> 1) & 0x7fff;
					ptr2++;
				}
				trailer[SFACTR] = 2;
				if( error_count != 0) {
					pprintf("%d Errors on Receive.\n",error_count);
				}
				return;
			}
		}
		if( error_count != 0) {
			pprintf("%d Errors on Receive.\n",error_count);
		}
		return;
	} else if( strncmp(string,"TMP",3) == 0){
		i = sscanf(string,"%s %d %f ",txt,&j,&set_temp);
		if( i == 1 ) { 	
			/* just report temp. */
			if(get_current_temp()){
				printf(" Current Temperature: %.1f\n",current_temp );
			} else {
				printf("Error.\n");
			}
			return;
		}
		if( i >= 2 ) {
			if( j )
				temp_control = 1;
			else
				temp_control = 0;
		}
		/* printf("%d temp_control.\n",temp_control); */
		if( set_temp < -50.0 || set_temp > 25.0 ) {
			beep();
			printf("Set Temperature Must be within [-50,25].\n");
			return;
		}
		
		set_point = get_set_point(set_temp);
		
		insert_value(txt,i,0);	/* initialize routine */
		
		insert_value(txt,temp_control,2);	/* put parameters in */
		insert_value(txt,set_point,2);
		insert_value(txt,samp_rate,2);
		insert_value(txt,p_gain,2);
		insert_value(txt,i_gain,2);
		insert_value(txt,reset_brownout,2);
		
		command = (unsigned char) REGULATE_TEMP;

		if( send_packet(REGULATE_TEMP,12,txt)  == -1) {

			return;
		} else {
			beep();
			printf("Error.\n");
			return;
		}
	} else {
		printf("Illegal ST-6 command.\n");
	}
}
/* ************ */

int load_drivers()
{
	OSErr result;
	SerShk shake;

	if( serial_port == 0 ) {
		result = opendriver(".BIn",&in_ref);
		if(result != noErr) {
			printf("Error opening driver.\n");
			beep();
			return(0);
		}
		result = opendriver(".BOut",&out_ref);
		if(result != noErr) {
			printf("Error opening driver.\n");
			beep();
			return(0);
		}
	} else {
		result = opendriver(".AIn",&in_ref);
		if(result != noErr) {
			printf("Error opening driver (#%d) - Continuing anyway\n",result);
/*			beep();
			return(0);*/
		}
		result = opendriver(".AOut",&out_ref);
		if(result != noErr) {
			printf("Error opening driver (#%d) - Continuing anyway\n",result);
/*			beep();
			return(0);*/
		}
	
	}
	
	shake.fCTS = 0;		/* Clear to Send handshaking off */
	shake.fXOn = 0;		/* Xon Xoff handshaking off */
	shake.fInX = 0;		/* Xon Xoff handshaking off */
	shake.errs = 0;
	shake.evts = 0;
	

	SerHShake(out_ref,&shake);
	SerHShake(in_ref,&shake);

/*	LockMemory(inbuf,800); */

	SerSetBuf(in_ref,inbuf,800);
	
	return(1);
}

/* ************ */

insert_value(mytxt,var,count)		/* put values in an array in proper order */
unsigned char mytxt[];
int count,var;
{
	static int index;
	int i;
	
	if( count == 0 ) {
		index = 0;
		return;
	}
	for(i=0; i<count; i++) {
		*(mytxt + index + i) = (var >> (i*8)) & 255;
	}
	index += count;
	
}

/* ************ */

get_st6_status( command,status)
unsigned short *status;
unsigned char command;
{

	extern char txt[];
	
	txt[0] = command;
	txt[1] = 0;
	if( send_packet(GET_ACTIVITY_STATUS,2,txt) ) {
		*status = sendbuf[6] + 256 * sendbuf[7];
	} else {
		beep();
		printf("Error getting status.\n");
		*status = -1;
	}
}

/* ************ */

int check_data()
{
	int i,length;
	unsigned int checksum = 0;
	unsigned int datasum;

	length = sendbuf[2] + 256 * sendbuf[3];		/* the length of the data */
	for(i=0; i < length+4; i++) {
		checksum += sendbuf[i];
	}
	checksum = checksum & 0xffff;
	datasum = sendbuf[length+4] + 256 * sendbuf[length+5];
	if ( checksum == datasum )
		return(1);
	else
		printf("%x %x\n",checksum,datasum);
		return(0);
}

/* ************ */

int send_packet(command,length,buffer)
char command, buffer[];
long length;
{
	unsigned int checksum = 0;
	int i;
	long reply_length = 0;
	
	sendbuf[0] = 0xA5;
	sendbuf[1] = command;
	sendbuf[2] = length & 0xFF;
	sendbuf[3] = (length>>8) & 0xFF;
	for(i=0; i < length+4; i++) {
		if( i>= 4) {
			sendbuf[i] = buffer[i-4];
		}
		checksum += sendbuf[i];
	}
	sendbuf[length+4] = checksum & 0xFF;
	sendbuf[length+5] = (checksum>>8) & 0xFF;
	length += 6;

	FSWrite(out_ref,&length,sendbuf);
	/*
	printf("%d written\n",length);
	for(i=0; i<length; i++) {
		printf("%x\n",sendbuf[i]);
	} 
	delay(15);
	*/
	for( i=0; reply_length == 0 && i < re_tries; i++) 
		//SerGetBuf(in_ref,&reply_length);

	if( reply_length > 0 ) {
		FSRead(in_ref,&reply_length,sendbuf);
		if( sendbuf[0] == ACK ) return(-1);
		if( sendbuf[0] == START_BYTE ) {
			/* there is a packet returned */
			return( get_packet(reply_length));
		} else {
			printf("Returned: %x\n",sendbuf[0]);
			return(0);
		}
	} else {
		printf("No Reply.\n");
		return(0);
	}
}

/* ************ */

int get_packet(length)		
long length;
						/* get a packet of info -- the first "length" characters have been read
						   but there may be more. The returned value is the length of the packet or
						   0 for an error. */
{

	long data_length,new_bytes;
	OSErr err;
	int i;
	
	/* there is a packet returned */
	
	data_length = 0;
	
	for( i=0; (length < 4) && (i < re_tries); i++ ) { 
		// have to wait for more 
		//SerGetBuf(in_ref,&new_bytes);
		if( new_bytes > 0 ) {
			err = FSRead(in_ref,&new_bytes,&sendbuf[length]);
			if( err != noErr) goto CommErr;
			length += new_bytes;
			i = 0;
		}			
	}
	if( i >= re_tries ) goto ReplyErr;
	
	/* check to see how long this is supposed to be */
		
	data_length = sendbuf[2] + 256 * sendbuf[3];		/* the length of the data */
	/* printf("%d data bytes in packet.\n",data_length); */
	for( i = 0; ((data_length + 6) > length) && (i < re_tries); i++) {
		
		// need to read the rest of the packet still 
		//SerGetBuf(in_ref,&new_bytes);
		if( new_bytes > 0 ) {
			err = FSRead(in_ref,&new_bytes,&sendbuf[length]);
			if( err != noErr) goto CommErr;
			length += new_bytes;
			i = 0;
		}
				
	}
	if( i >= re_tries ) goto ReplyErr;

	/* entire packet has been read in */
	if( check_data() ) 
		return(length);		/* if checksum is OK, return the length of entire packet */
	else {
		printf("Checksum error in reply.\n");
		return(0);			/* otherwise, return error indicator */
	}
CommErr:
		printf("Communications Error.\n");
		return(0);			/*  return error indicator */

ReplyErr:
		/* printf("Reply Error %d vs %d.\n",length,data_length); */
		return(0);			/*  return error indicator */
	
}

/* ************ */

unsigned short get_set_point(float temp)

{
	int i;
	
	i = (temp + 50.0) / 5.0 + .5;	/* the index of the set_pts array */
	return(set_pts[i]);				/* should interpolate, but for now just set to closest 5 deg */
}

/* ************ */

float get_temp(reading)
unsigned short reading;
{
	float temp;
	int i;
	
	for(i=0; reading <= set_pts[i]; i++ );
	if( i != 0 )
		temp = i * 5 - 50.0 + (reading - set_pts[i]) * 5.0 / (set_pts[i] - set_pts[i-1]);
	else
		temp = i * 5 - 50.0 - (reading - set_pts[i]) * 5.0 / (set_pts[i] - set_pts[i+1]);
	return(temp);
}

/* ************ */

int get_current_temp()
{
	unsigned short set_point;
	extern char txt[];

	if( send_packet(READ_THERMISTER,0,txt) ) {
		/* just report temp. */
		set_point = sendbuf[4] + 256 * sendbuf[5];
		current_temp = get_temp(set_point);
		return(1);
	} else {
		beep();
		return(0);
	}
}

#endif

#ifdef SPEX

/*----------------------------------------------------------------*/

/*	For SPEX */

#define GET_ACK				' '			
#define GET_MOTOR_STATUS	'E'
#define GET_BOOT_VERSION	'y'
#define GET_MAIN_VERSION	'z'
#define SET_GRATING			'F'
#define SET_SLIT			'k'
#define READ_GRATING		'H'
#define READ_SLIT			'j'
#define INIT_MOTOR			'A'

int spex_inited = 0;

long mv_grating_steps = 0;				/* move relative */
long mv_slit_steps = 0;					/* move relative */
int sltsteps_per_micron = 8;			/* steps/um x 100 */
int grtsteps_per_nm = 8;				/* steps/nm */
long current_grt_steps;
long current_slt_steps;
int get_current_grt(),get_current_slt();
int grating_pos,slit_pos,sign;			/* wanted positions */
long str_len = 0;
char move_s[30];						/* for convert input int to string */

short			out_ref;
short 			in_ref;
int				re_tries;


extern char  txt[];

extern int serial_port;				/* settings */
									/* 0 for printer port, 1 for modem port */								

int send_command();
int check_motor();
void jump_to_main();
void do_spex_command();

/* ************ */
/*
Commands for the SPEX Spectrometer.
Use these via the SPEX OMA command.
Format is:

SPEX command_string [parameters]
	command_string specifies a SPEX command.
	The values for parameters depend on the particular command.
	
	
Commands:

INIT	GRATING		SLIT	VERSION

*/

int spex(n,index)
int n,index;

{
	static unsigned char command;
	static unsigned char status;
	int length;
	
	extern char cmnd[];
	
	if( !spex_inited ) {
		//printf("Looking for Spex at modem port\n");
		serial_port = 1;
		if( serial_inited == 0 ){	
			beep();
			printf("Open serial port first (use SERIAL command).\n");
			return -1;
		}
		spex_inited = 1;
		in_ref = out_ref = serial_fd;
		
		//SerReset(in_ref,baud19200+data8+stop10+noParity);
		//SerReset(out_ref,baud19200+data8+stop10+noParity);
		re_tries = 2000;
					
		command = GET_ACK;			/* SP, begin auto baud sequence */
		
		txt[0] = command;
		length = 0;
		status = 0x0;
		while( length == 0 ) {
			printf("Looking for Spex at serial port\n");
			length = send_command(1,txt);
		} 
		
		if( length <= 0) {
			beep();
			printf("Error.1\n");
			return -1;
		} else {
			status = sendbuf[0];
			switch (status) {
				case '*':
					printf("Spex Autobaud ACK OK.\n");
					
					command = 0xF7;						/* send Hi IQ ID */
					txt[0] = command;
					send_command(1,txt);
					status = sendbuf[0];
					if( status != '=' ) {
						printf("bad ACK. Try again.\n");
						return -1;
					}
					jump_to_main();
					status = sendbuf[0];
					while( status != '*' ) {
						jump_to_main();
						status = sendbuf[0];
					}
					printf("First time communicatiing since turned on Spex - Must force initialization\n");
					do_spex_command("INIT");
					break;
				case 'B':
					printf("Spex Init Program ACK OK.\n");
					jump_to_main();
					status = sendbuf[0];
					while( status != '*' ) {
						jump_to_main();
						status = sendbuf[0];
					}
					break;
				case 'F':
					printf("Spex Main Program ACK OK.\n");
					break;
				default:
					printf("Error: SPEX Communication NOT Established.\n");
					break;
				}			
		}
	}
	if(index != 0) { 
		do_spex_command(&cmnd[index]);
	}
					/* get Grating setting */
	if( get_current_grt() ) {	
		grating_pos = current_grt_steps / grtsteps_per_nm;
		printf("Grating Position (Abs. Step #%d): %d nm\n",
				current_grt_steps,grating_pos);
	}else {
		printf("Error in getting grating position.\n");
	}

	if( get_current_slt() ) {
		slit_pos = (current_slt_steps * 100) / sltsteps_per_micron;
		printf("    Slit Opening (Abs. Step #%d): %d µm\n",
				current_slt_steps,slit_pos);
	}else {
		printf("Error in getting slit position.\n");
	}
	return 1;
}

void do_spex_command(string)
char string[];
{

	int i;
	static unsigned short status;
	static unsigned char command;
	static long echo_length;
		

	switch (toupper(string[0])) {
	
	
		case 'G': 			 /* GRATING MOVEMENT/MONITORING */
		
			i = sscanf(string,"%s %d",txt,&grating_pos);	/* wavelngth in nm */
			if( i > 1 ) {
			
				get_current_grt();
				mv_grating_steps = grating_pos * grtsteps_per_nm - current_grt_steps;
				printf("Moving Grating\n",mv_grating_steps,grating_pos); 
				
				if ((sign = mv_grating_steps) < 0)
					mv_grating_steps = -mv_grating_steps;
				str_len = 0;
				do {										/* in reverse order */
					move_s[str_len++] = (mv_grating_steps % 10) + 48;
				} while ((mv_grating_steps /= 10) > 0);
					if (sign < 0) {
					move_s[str_len++] = '-';
				}
			
				command = (unsigned char) SET_GRATING;
			
				txt[0] = 'F';
				txt[1] = '0';
				txt[2] = ',';
			
				if( move_s[str_len-1] == '-' ) {
					for( i=1; i< str_len; i++ ){
						txt[i+3] = move_s[str_len-i-1];
					}
					txt[3] ='-';
				} else {
					for( i=0; i< str_len; i++ ){
						txt[i+3] = move_s[str_len-i-1];
					}
				}
				txt[str_len+3] = 0x0D;
			
				if( send_command(str_len+4,txt) > 0 ) {
					status = sendbuf[0];
					if( status == 'o' ){
						if( check_motor() ) return;
						else {
							printf("Error in getting motor status.\n");
							return;
						}		
					}
					if( status == 'b' ){
						printf("SPEX Receive is bad.");
						return;
					}
					else {
						printf("Status 1 was %c.\n",status);
						return;
					}
				}
				return;
			} else {
				printf("No wavelength entered... (Format: spex g 532)\n");
				return;
			}


		case 'S':				/* SLIT MOVEMENT/MONITORING */

			i = sscanf(string,"%s %d",txt,&slit_pos);
			if( i > 1 ) {
			
				get_current_slt();
				mv_slit_steps = (slit_pos * sltsteps_per_micron) / 100 - current_slt_steps;
				printf("Opening slits\n");
	
				if ((sign = mv_slit_steps) < 0)
					mv_slit_steps = -mv_slit_steps;
				str_len = 0;
				do {										/* in reverse order */
					move_s[str_len++] = (mv_slit_steps % 10) + 48;
				} while ((mv_slit_steps /= 10) > 0);
				if (sign < 0) {
					move_s[str_len++] = '-';
				}

				command = (unsigned char) SET_SLIT;

				txt[0] = command;
				txt[1] = '0';
				txt[2] = ',';
				txt[3] = '0';				/* to front entrance slit */
				txt[4] = ',';

				if( move_s[str_len-1] == '-' ) {
					for( i=1; i< str_len; i++ ){
						txt[i+5] = move_s[str_len-i-1];
					}
					txt[5] ='-';
				} else {
					for( i=0; i< str_len; i++ ){
						txt[i+5] = move_s[str_len-i-1];
					}
				}
				txt[str_len+5] = 0x0D;
/*			
				for( i=0; i< str_len+6; i++ ){
					printf("txt[%d] = %c\n",i,txt[i]);
				}	*/

				if( send_command(str_len+6,txt) > 0 ) {
					status = sendbuf[0];
					if( status == 'o' ) {
						if( check_motor() ) return;
						else {
							printf("Error in getting motor status.\n");
							return;
						}		
					}
					if( status == 'b' ){
						printf("SPEX Receive is bad.");
						return;
					}
					else {
						printf("Status 2 was %c.\n",status);
						return;
					}
				}
				return;
			} else {
				printf("No slit width entered... (Format: spex s 200)\n");
				return;
			}

		
		case 'I':		 /* INITIALIZE SPEX */

			command = (unsigned char) INIT_MOTOR;

			txt[0] = command;
			send_command(1,txt);
			printf("Initializing Grating and Slit - wait a minute\n");
			echo_length = 0;
			while( echo_length <= 0 ){
				//SerGetBuf(in_ref,&echo_length);
				 echo_length = read(in_ref,sendbuf,SER_BUF_LEN);
			}	
			//FSRead(in_ref,&echo_length,sendbuf);
			
			status = sendbuf[0];
			if( status == 'o' ) {
				printf("Motor initialization OK.\n");
			} else {
				beep();
				printf("Error initializing motors\n");
			}
			return;


		case 'V':	 /* GET SPEX FIRMWARE VERSIONS */

			command = (unsigned char) GET_BOOT_VERSION;
			txt[0] = command;
			txt[1] = '0';
			i=send_command(2,txt);
			if(i > 0){
				status = sendbuf[0];
				if( status == 'o' ) {
					sendbuf[0] =' ';
					printf(" Boot Version: %s\n",sendbuf);
				} else {
					beep();
					printf("Error getting Boot firmware version\n");
				}
			}			
			command = (unsigned char) GET_MAIN_VERSION;
			txt[0] = command;
			txt[1] = '0';
			i=send_command(2,txt);
			if(i > 0){
				status = sendbuf[0];
				if( status == 'o' ) {
					sendbuf[0] =' ';
					printf("Main Version: %s\n",sendbuf);
					return;
				} else {
					beep();
					printf("Error getting main firmware version\n");
					return;
				}
			}


		default:

			printf("SPEX COMMANDS:\n");
			printf("(I)nitialize   : initializes SPEX to default grating & slit positions\n");
			printf("(S)lit nnn     : Sets slit width in µm\n");
			printf("(G)rating nnn  : Sets grating wavelenth in nm\n");
			printf("(V)ersion      : Returns Spex Firmware versions\n");
			printf("NO COMMAND     : Returns current slit width and Grating wavelength\n\n");
	}
}


/* ************ */

int send_command(send_len,buffer)
char buffer[];
long send_len;
{
	int i;
	long reply_length = 0;
	int delay();

	//FSWrite(out_ref,&send_len,buffer);
	send_len = write(out_ref, buffer, send_len);
	
	delay(15);
	 
	for( i=0; reply_length == 0 && i < re_tries; i++) {
		//SerGetBuf(in_ref,&reply_length);
		 reply_length = read(in_ref,sendbuf,SER_BUF_LEN);
	}	
	if( reply_length > 0 ) {
		//FSRead(in_ref,&reply_length,sendbuf);
/*		printf("Returned: %c\n",sendbuf[0]);
		printf("Returned: %s\n",sendbuf);
		printf("Returned length: %d\n",reply_length);	
*/		return( reply_length );
	} else {
/*		printf("No Reply.\n"); */
		return(0);
	}
}


/* ************ */

void jump_to_main()
{
	static int length = 0;
	
	txt[0] = 'O';
	txt[1] = '2';
	txt[2] = '0';
	txt[3] = '0';
	txt[4] = '0';
	txt[5] = 0x0;
		
	length = send_command(6,txt);

	if( length == 0 ) {
		beep();
		printf("Error getting initialization status.\n");
	}
}


/* ************ */

int check_motor()
{
		static unsigned short status;
		static unsigned char command;
	
		command = (unsigned char) GET_MOTOR_STATUS;

		txt[0] = command;
		if( send_command(1,txt) > 0){
			status = sendbuf[0];
			if( status == 'o' ) {
				if( sendbuf[1] == 'q' ){
					//printf("Motor is busy.\n");
					while( sendbuf[1] != 'z' ) {
						send_command(1,txt);
					}
				}
//				if( sendbuf[1] == 'z' ){
//					printf("job completed\n");
//				}
				return(1);
			}
			if( status == 'b' ){
				printf("SPEX Receive is bad.");
				return(0);
			}
		}
		else {
			printf("Status 3 was %c.\n",status);
			
		}
		return(0);
}

/* ************ */

int get_current_grt()
{
		static unsigned short status;
		static unsigned char command;
		static int echo_length,i,total_no;

		command = (unsigned char) READ_GRATING;

		txt[0] = 'H';
		txt[1] = '0';
		txt[2] = 0x0D;			/* <CR> */
		echo_length = send_command(3,txt);
		if( !echo_length ) return(-1);
		status = sendbuf[0];
		total_no = 0;
		if( status == 'o' ) {
			for( i=1; i< echo_length && sendbuf[i] != 13; i++ ){
/*				printf("sendbuf[%d] = %c\n",i,sendbuf[i]);	*/
				total_no = total_no * 10 + sendbuf[i] - 48;
			}
			current_grt_steps = total_no;
			return(1);
		}
		if( status == 'b' ){
			printf("SPEX Receive is bad.");
			return(0);
		}
		else {
			printf("Status 4 was %c.\n",status);
			return(0);
		}
}

/* ************ */

int get_current_slt()
{
		static unsigned short status;
		static unsigned char command;
		static int echo_length,i,total_no;
		
		command = (unsigned char) READ_SLIT;

		txt[0] = command;
		txt[1] = '0';
		txt[2] = ',';
		txt[3] = '0';
		txt[4] = 0x0D;
		echo_length = send_command(5,txt);
		if( !echo_length ) return(-1);
		status = sendbuf[0];
		total_no = 0;
		if( status == 'o' ) {
			for( i=1; i< echo_length && sendbuf[i] != 13; i++ ){
/*				printf("sendbuf[%d] = %d\n",i,sendbuf[i]);	*/
				total_no = total_no * 10 + sendbuf[i] - 48;
				}
			current_slt_steps = total_no;
			return(1);
		}
		if( status == 'b' ){
			printf("SPEX Receive is bad.");
			return(0);
		}
		else {
			printf("Status 5 was %c.\n",status);
			return(0);
		}
}

#endif	// end of SPEX commands