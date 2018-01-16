/*
OMAX -- Photometric Image Processing and Display
Copyright (C) 2006  by the Developers of OMA

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/*
Using the SensiCam

sensi N			connect to board N and take get an image from the camera 
				-- should be the first command issued
				
sensi			get image from currently open camera

sensi stat		print sensicam parameters

sensi mode N	set mode to N, where N= 0 (standard), 1 (double expose 200 ns) or 
				2 (double expose 1 µs)
				
sensi trig N	set external trigger mode to N, where N= 0 (none), 1 (external rising) or 
				2 (external falling)
				
sensi exp Exp_num Delay Exp_time
				sets the Delay and Exposure parameters for a certain Exposure Number.
				Gives a way of setting values corresponding to those shown in the 
				Settings dialog box



*/

// =================================================================================
//	SensiCamImage.cpp	 	© 7/2001, COOKE Corporation	 				
// =================================================================================

#ifdef SensiCam

#include "impdefs.h"
//#include "SensiCamInterface.h"
#include <Sensicam/SensiCamInterface.h> 

extern DATAWORD *datpt;
extern TWOBYTE 	header[],trailer[];
extern char 	comment[];
extern int 		doffset;
extern int     	maxint;		/* the max count CC200 can store */
extern int		npts;		/* number of data points */
extern int		have_max;	/* indicates whether or not the minimum
				   				and maximum value have been found for the data
				   				in the current buffer. 0 -> no;  1 -> yes */
extern char    cmnd[];  	/* the command buffer */
extern int	   dlen,dhi;
extern char    txt[];
extern short	detector;		
extern int	detectorspecified;	


static int sensi_open = 0;
int sensi_focus = 1;
SensiCamRef	theSensiCam = 0;
UInt32		timeCount = 20;
SInt32		timeTable[20] = {0, 1000000,0,1000000,0,1000000,0,1000000,0,1000000,
    							 0,1000000,0,1000000,0,1000000,0,1000000,0,1000000}; 
    							 // init with 0 delay and 10 ms exp.
// These are used in the settings dialog

int SensiCamTrigNone = true;
int SensiCamTrigRising = false;
int SensiCamTrigFalling = false;

int	SensiCamStandard = true;
int	SensiCamDoubleShort = false;
int	SensiCamDoubleLong = false;

int	SensiCamCopyDelay=false;
int	SensiCamCopyExposure=false;


/* *************** */

int sensi(int n, int index)
{

	int i,chip,elect,Exp_num, Delay, Exp_time,GetSensiCamImage();
	unsigned int mode,trig;
	char* string;
	OSErr	error = noErr;

	if(index == 0){			// with no argument, go get an image
		return GetSensiCamImage();
	}
			
	// interpret sensicam commands
	
	string = &cmnd[index];
	
	for( i=0; string[i] != 0; i++) 
		string[i] = toupper(string[i]);
	
	if( strncmp(string,"STAT",4) == 0){		// print camera status
		mode=chip=trig=elect=-1;
		SensiCamGetStatus(&chip, &elect, &mode,&trig);
		printf("Camera Mode: %x\n",mode);
		printf("Trigger Mode: %x\n",trig);
		printf("Chip Temperature: %d\n",chip);
		printf("Electronics Temperature: %d\n",elect);
		return 0;
	} else if( strncmp(string,"MODE",4) == 0){	// set mode to 0 1 or 2
		mode = -1;
		sscanf(&string[4],"%d",&mode);
		if( mode == 0 ){
			SensiCamStandard = true;
			SensiCamDoubleShort = false;
			SensiCamDoubleLong = false;
		} else if(mode == 1){
			SensiCamStandard = false;
			SensiCamDoubleShort = true;
			SensiCamDoubleLong = false;
		} else if(mode == 2){
			SensiCamStandard = false;
			SensiCamDoubleShort = false;
			SensiCamDoubleLong = true;

		} else {
			//beep();
			//printf(" Mode must be 0-2\n");
			SensiCamSettingsDialog();
			return 0;
		}
		if(SensiCamStandard) mode = 1;
		else if(SensiCamDoubleShort) mode = 1+1*65536;
		else if(SensiCamDoubleLong) mode = 1+2*65536;


		error = SensiCam_SetMode(theSensiCam, &mode);	
		if(error != noErr) {
			beep();
			printf("Mode Error %d\n",error);
			
		}
		return(error);
	} else if( strncmp(string,"EXP",3) == 0){	// set delay and exposure for exposure N
		if( sscanf(&string[3],"%d %d %d",&Exp_num,&Delay,&Exp_time) != 3){
			beep();
			printf("Need three arguments: Exp_num Delay Exp_time\n");
			return(-1);
		}
		if(Exp_num <= 0 || Exp_num >10){
			beep();
			printf("Exp_num must be 1-10\n");
			return(-1);
		}
		timeTable[(Exp_num-1)*2] = Delay;
		timeTable[(Exp_num-1)*2+1] = Exp_time;
		
		error = SensiCam_SetTiming(theSensiCam, timeTable, timeCount);	
		if(error != noErr) {
			beep();
			printf("Timing Error %d\n",error);
		}
		return(error);
	} else if( strncmp(string,"TRIG",4) == 0){	// set external trigger mode to 0 1 or 2
		trig = -1;
		sscanf(&string[4],"%d",&trig);
		if( trig == 0 ){
			SensiCamTrigNone = true;
			SensiCamTrigRising = false;
			SensiCamTrigFalling = false;
		} else if(trig == 1){
			SensiCamTrigNone = false;
			SensiCamTrigRising = true;
			SensiCamTrigFalling = false;
		} else if(trig == 2){
			SensiCamTrigNone = false;
			SensiCamTrigRising = false;
			SensiCamTrigFalling = true;
		} else {
			beep();
			printf("External Trigger Mode must be 0-2\n");
			return -1;
		}
		if(SensiCamTrigNone) trig = 0;
		else if(SensiCamTrigFalling) trig = 1;
		else if(SensiCamTrigRising) trig = 2;
	
	
		error = SensiCam_SetTriggerMode(theSensiCam, &trig);	
		if(error != noErr) {
			beep();
			printf("Trigger Error %d\n",error);
		}
		return(error);
	}	
	
	if(!sensi_open){		// open the appropriate camera
		if( n>=1 && n<=4){	
			return OpenSensiCam(n);
		} else {
			beep();
			printf("Board number must be 1-4.\n");
			return -1;
		}
	}
	return 0;
}

// ---------------------------------------------------------------------------
//	¥ OpenSensiCam, static
// ---------------------------------------------------------------------------

int OpenSensiCam (int bd)
{

	OSErr				error = noErr;
	
	UInt32				width, height;

	SInt16 board;

	
	board = bd;

	

	if( !sensi_open) {
		redoMenus(SensiCamMenu);
		error = SensiCam_Open(board, 1, &theSensiCam); // init 'everything'
		if(error != noErr) {
			beep();
			printf("Open Error %d\n",error);
			return(error);
		}
		printf(" Opened camera %d\n",bd);
		sensi_open = 1;
	 	

		error = SensiCam_GetMaxImageSize(theSensiCam, &width, &height);
		if(error != noErr) {
			beep();
			printf("GetMaxImageSize Error %d\n",error);
			return(error);
		}
		// set CCD parameters according to this detector
		dlen = width;
		dhi = height;
		detector = CCD;			
		detectorspecified = 1;		
		doffset = 80;
		checkpar();
		return 0;
	
	}
	return 0;
}

int GetSensiCamImage()
{

	OSErr		error = noErr;
	
	Rect		bounds;
	UInt32		hBin = 2,
                vBin = 2,
                status = 0,
                width, height;

	
	int i,double_rows;
	
		if( !sensi_open) {
			beep();
			printf("Open SensiCam First.\n");
			return -1;
		}

		if(!SensiCamStandard)
			double_rows=2;
		else
			double_rows=1;

		bounds.left = header[NX0]/32*32; //*header[NDX];
		bounds.top = header[NY0]/32*32; //*header[NDY];
		bounds.right = (header[NCHAN])*header[NDX]+bounds.left;
		bounds.bottom = (header[NTRAK]/double_rows)*header[NDY]+bounds.top;
		
		error = SensiCam_SetImageBounds(theSensiCam, &bounds);
		if(error != noErr) {
			beep();
			printf("Image Bounds Error %d\n",error);
			return(error);
		}
		
		hBin = header[NDX];
		vBin = header[NDY];
		
		error = SensiCam_SetBinning(theSensiCam, &hBin, &vBin);
		if(error != noErr) {
			beep();
			printf("Binning Error %d\n",error);
			return(error);
		}

		error = SensiCam_GetImageSize(theSensiCam, &width, &height);	
		if(error != noErr) {
			beep();
			printf("Image Size Error %d\n",error);
			return(error);
		}
		printf(" %d %d : %d %d l r : t b\n%d dubrows\n",bounds.left,bounds.right,bounds.top,bounds.bottom,double_rows);
		if(
			header[NX0] != bounds.left ||
			header[NY0] != bounds.top ||
			header[NCHAN] != width ||
			header[NTRAK] != height) {


				header[NX0] = bounds.left; //header[NDX];
				header[NY0] = bounds.top; //header[NDY];
				header[NCHAN] = width;
				header[NTRAK] = height; //*double_rows;
				checkpar();
				update_status();
				beep();
				printf("Image parameters adjusted.\n");
		}

		error = SensiCam_SetTiming(theSensiCam, timeTable, timeCount);	
		if(error != noErr) {
			beep();
			printf("Timing Error %d\n",error);
			return(error);
		}
		printf("%d %d w h\n",width,height);
/*
		bounds.left = 0;
		bounds.top = 0;
		bounds.right = width;
		bounds.bottom = height;
*/		
		error = SensiCam_RunCOC(theSensiCam, 4); // 4 allows only a single frame to capture !
		if(error != noErr) {
			beep();
			printf("Run Error %d\n",error);
			return(error);
		}
		
		while(1){
			error = SensiCam_GetFrameState(theSensiCam, &status);
        	if (error == noErr && 
            	!(status & status_NoPictureInPCIBuffer) && 
            	!(status & status_ReadOutRunning)) {

		
				error = SensiCam_ReadFrame(theSensiCam, datpt+doffset, 2*width*height, true);
				if(error != noErr) {
					beep();
					printf("Read Error %d\n",error);
					return(error);
				} else
					error = SensiCam_StopCOC(theSensiCam); //
					break;
			}
		}
		
		
		// change byte order
		
		for(i= 0; i< width*height*2; i+=2){
			swap_bytes_fn((char*)datpt+doffset*2+i);
		}
		
		have_max = 0;
		
		maxx();
		return 0;
}

SaveSensiCamSettings()
{

	OSErr							error = noErr;
	UInt32 trig,mode;

	if( !sensi_open){
		beep();
		printf("Can't Save Settings -- No Camera Open.\n");
		return(-1);
	}

	
	if(SensiCamTrigNone) trig = 0;
	else if(SensiCamTrigFalling) trig = 1;
	else if(SensiCamTrigRising) trig = 2;
	
	
	error = SensiCam_SetTriggerMode(theSensiCam, &trig);	
	if(error != noErr) {
		beep();
		printf("Trigger Error %d\n",error);
		return(error);
	}

	if(SensiCamStandard) mode = 1;
	else if(SensiCamDoubleShort) mode = 1+1*65536;
	else if(SensiCamDoubleLong) mode = 1+2*65536;


	error = SensiCam_SetMode(theSensiCam, &mode);	
	if(error != noErr) {
		beep();
		printf("Mode Error %d\n",error);
		return(error);
	}
	
	error = SensiCam_SetTiming(theSensiCam, timeTable, timeCount);	
	if(error != noErr) {
		beep();
		printf("Timing Error %d\n",error);
		return(error);
	}


	return(0);
}

int SensiCamGetStatus(int *chip, int *elect, unsigned int *mode,unsigned int *trig)
{

	if( !sensi_open){
		beep();
		printf("No Camera Open.\n");
		return(-1);
	}


	SensiCam_GetTemp(theSensiCam,chip,elect);
	SensiCam_GetMode(theSensiCam,mode);
	SensiCam_GetTriggerMode(theSensiCam,trig);
	
	return 0;

}

swap_bytes_fn(char* cp)
{
	char c;
	c = *cp;
	*cp = *(cp+1);
	*(cp+1) = c;
	return(0);
}

focus(int n, int index)		// read and display n frames
{
	int i,fc;
	OSErr							error = noErr;
	
	Rect							bounds;
	UInt32							hBin = 2,
                                    vBin = 2,
                                    status = 0,
                                    width, height;
    extern SInt32	timeTable[20]; 	// init with 0 delay and 10 ms exp.
	
	if( !sensi_open) {
		beep();
		printf("Initialize SensiCam first.\n");
		return -1;
	}
	if( !SensiCamStandard) {
		beep();
		printf("For focus, use Standard mode.\n");
		return -1;
	}

	sensi_focus = 1;
	
	bounds.left = header[NX0];
	bounds.top = header[NY0];
	bounds.right = header[NCHAN]*header[NDX]+header[NX0];
	bounds.bottom = header[NTRAK]*header[NDY]+header[NY0];
	error = SensiCam_SetImageBounds(theSensiCam, &bounds);
	if(error != noErr) {
		beep();
		printf("Image Bounds Error %d\n",error);
		return(error);
	}
	if(
		header[NX0] != bounds.left ||
		header[NY0] != bounds.top ||
		header[NCHAN] != (bounds.right-header[NX0])/header[NDX] ||
		header[NTRAK] != (bounds.bottom-header[NY0])/header[NDY]) {


		header[NX0] = bounds.left;
		header[NY0] = bounds.top;
		header[NCHAN] = (bounds.right-header[NX0])/header[NDX];
		header[NTRAK] = (bounds.bottom-header[NY0])/header[NDY];
		checkpar();
		update_status();
		beep();
		printf("Image parameters adjusted.\n");
	}
		
	hBin = header[NDX];
	vBin = header[NDY];
	error = SensiCam_SetBinning(theSensiCam, &hBin, &vBin);
	if(error != noErr) {
		beep();
		printf("Binning Error %d\n",error);
		return(error);
	}

	error = SensiCam_GetImageSize(theSensiCam, &width, &height);	
	if(error != noErr) {
		beep();
		printf("Error %d\n",error);
		return(error);
	}

	error = SensiCam_SetTiming(theSensiCam, timeTable, timeCount);	
	if(error != noErr) {
		beep();
		printf("Error %d\n",error);
		return(error);
	}

		
	error = SensiCam_RunCOC(theSensiCam, 0); // 4 allows only a single frame to capture !
	if(error != noErr) {
		beep();
		printf("Error %d\n",error);
		return(error);
	}
	
	for (fc = 0; fc<n; fc++) {	
		while(1){
			error = SensiCam_GetFrameState(theSensiCam, &status);
        	if (error == noErr && 
            	!(status & status_NoPictureInPCIBuffer) && 
            	!(status & status_ReadOutRunning)) {
            	// start the read
				error = SensiCam_ReadFrame(theSensiCam, datpt+doffset, 2*width*height, true);
				if(error != noErr) {
					beep();
					printf("Read Error %d\n",error);
					return(error);
				} else
					// wait till read us done (status is 0)
					for(SensiCam_GetAsyncReadState(theSensiCam, &status);
						status != 0;
						SensiCam_GetAsyncReadState(theSensiCam, &status)){};
					break;
			}
		}
		
		// change byte order
		
		for(i= 0; i< width*height*2; i+=2){
			swap_bytes_fn((char*)datpt+doffset*2+i);
		}
		
		
		
		if (fc == 0) {
			have_max = 0;
			maxx();
		}
		displa(0,0);
		checkevents();
		if(sensi_focus == 0) {
			error = SensiCam_StopCOC(theSensiCam);
			return fc;
		}

	
	}
	error = SensiCam_StopCOC(theSensiCam); //
	return error;

}

#endif