
#ifdef PVCAM

#include "impdefs.h"
#define __mac_os_x	// tell the pvcam software what's up
#include "master.h"
#include "pvcam.h"
#include "cam_func.h"


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


/********************************************************************
		G L O B A L   V A R I A B L E   D E C L A R A T I O N S
********************************************************************/


Boolean	pvc_inited = FALSE,				/* FALSE = quit the program */
		camera_status_active = FALSE,/* TRUE = Camera/Status dialog */
		new_camera = FALSE,			/* TRUE = need to update camrera name */
		focus_active = FALSE,		/* TRUE = Exposure/Continuous dialog */
		focus_loop_active = FALSE,	/* TRUE = Focus loop running */
		data_display_failed = FALSE;/* TRUE = Unable to display data */
long	focus_cycles = 100,			/* Number of focus loops */
		focus_cycle_now = 0;		/* Focus loop counter */
		
int		pvc_exptime;				/* the exposure time */
int		pvc_expmode;				/* the exposure mode */
camera_data_type	data = { -1,0, };	/* The camera data */

pvcam(int n, int index){

	if( !pvc_inited ) {
		pl_pvcam_init();
		if (pl_exp_init_seq()) {
			pvc_inited = TRUE;
			init_data_structures(&data);
			printf("PVCam Initialization Successful.\n");
			pvc_exptime = data.exposure_time;
			pvc_expmode	= data.exposure_mode;
			
			printf("Exposure Mode %d\n",pvc_expmode);

		}
	}
	
	
	// check for arguments to pvc command
	
	if(index == 0) return;
	do_pvc_command(&cmnd[index]);
}

// EXP [exposure time]	
//		Take an exposure with optional integration time specified

// STAT
//		Print some information on the status of the camera

// START_SEQ  [exposure time]
//		Start an exposure with optional integration time specified
//		Do not wait for sequence to finish

// WAIT_SEQ
//		Wait for a sequence (started by START_SEQ) to finish
//
// EX_MODE [exposure mode]	
//
//enum {  TIMED_MODE,STROBED_MODE,BULB_MODE,TRIGGER_FIRST_MODE,FLASH_MODE,
//        VARIABLE_TIMED_MODE };


do_pvc_command(string)
char string[];
{
	int16 status;
	uns32 byte_cnt;
	OSErr myerr;
	int i,j,k;


	for( i=0; string[i] != 0; i++) 
		string[i] = toupper(string[i]);

// EXP [exposure time]	
//		Take an exposure with optional integration time specified
	if( strncmp(string,"EXP",strlen("EXP")) == 0){
	
		// check for exposure time specified
		if( sscanf(&string[strlen("EXP")],"%d",&j) == 1) {
			pvc_exptime = j;
		}

		if( pl_cam_check(data.hcam) ) {
		
			data.region.s_offset = header[NX0];
			data.region.s_size = header[NCHAN]*header[NDX];
			data.region.s_bin = header[NDX];

			data.region.p_offset = header[NY0];
			data.region.p_size = header[NTRAK]*header[NDY];
			data.region.p_bin = header[NDY];
			
			data.exposure_time = pvc_exptime;
			data.exposure_mode = pvc_expmode;

						
			if( !setup_one_exposure(&data,&byte_cnt) ) {
				myerr = pl_error_code();
				printf("Setup erorr is %d\n",myerr);
			}

			if( pl_exp_start_seq(data.hcam,datpt+doffset) ) {

				do {
					pl_exp_check_status(data.hcam,&status,&byte_cnt);
					} 
				while( status != READOUT_COMPLETE && status != READOUT_FAILED);

			}
			have_max = 0;
		}
	}
// EX_MODE [exposure mode]	
//		Set the exposure mode
	else if( strncmp(string,"EX_MODE",strlen("EX_MODE")) == 0){
	
		// get the mode
		if( sscanf(&string[strlen("EX_MODE")],"%d",&j) == 1) {
			pvc_expmode = j;
		}
		return;

	}
	
// START_SEQ  [exposure time]
//		Start an exposure with optional integration time specified
//		Do not wait for sequence to finish
	else if( strncmp(string,"START_SEQ",strlen("START_SEQ")) == 0){
	
		// check for exposure time specified
		if( sscanf(&string[strlen("START_SEQ")],"%d",&j) == 1) {
			pvc_exptime = j;
		}

		if( pl_cam_check(data.hcam) ) {
		
			data.region.s_offset = header[NX0];
			data.region.s_size = header[NCHAN]*header[NDX];
			data.region.s_bin = header[NDX];

			data.region.p_offset = header[NY0];
			data.region.p_size = header[NTRAK]*header[NDY];
			data.region.p_bin = header[NDY];
			
			data.exposure_time = pvc_exptime;
			data.exposure_mode = pvc_expmode;
						
			if( !setup_one_exposure(&data,&byte_cnt) ) {
				myerr = pl_error_code();
				printf("Setup erorr is %d\n",myerr);
			}
			if( !pl_exp_start_seq(data.hcam,datpt+doffset) ) {
				beep();
				printf("Error starting sequence.\n");
			}
		}
	}

// WAIT_SEQ
//		Wait for a sequence (started by START_SEQ) to finish
	else if( strncmp(string,"WAIT_SEQ",strlen("WAIT_SEQ")) == 0){
	
		do {
			pl_exp_check_status(data.hcam,&status,&byte_cnt);
			} 
		while( status != READOUT_COMPLETE && status != READOUT_FAILED);
	have_max = 0;
	}

	
// STAT
//		Print some information on the status of the camera
	else if( strncmp(string,"STAT",strlen("STAT")) == 0){

		if( pl_cam_check(data.hcam) ) {

			printf("Prescan %d\nColumns %d\nPostscan %d\n",data.prescan,data.s_max,data.postscan);
			printf("s_offset %d\ns_size %d\ns_bin %d\n",data.region.s_offset,data.region.s_size,data.region.s_bin);		
			printf("Exposure time is %d.\n",pvc_exptime);
		}
	}



}

#endif