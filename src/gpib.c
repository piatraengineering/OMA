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
#include "impdefs.h"

/* Definitions for National Instruments Macintosh GPIB subroutines */

/* gpib device names are assumed to be:

	gpib address		device name
		1					dev1
		2					dev2
		3					dev3
		.					 .
		.					 .
		.					 .
	These are the default names in the National Instruments software.
	They can be set with the IBCONF utility.
*/

#define DMAMODE		1			/* enable dma mode */
#define HEADERLENGTH 80			/* the ccd header length */
#define CCDSRQ	64				/* CC200 service request code */
#define TIMEOUT 15				/* # of seconds to wait */



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
extern char	   lastname[];


extern Str255  pstring; /* a pascal general purpose text array */

#ifdef TEXACO
#ifdef GPIB

#ifdef __MACH__
#include <NI488/ni488.h>
#else
#include "ni488.h"
#endif
#include <stdio.h>


//-----------------------------------------------------------------------------------

extern char	   syncflag;
int io_err = 0;


/* ********** */

int synch(n)	/* Set flag for CC200/uVAX synchronization */
int n;
{
	syncflag = n;
	return io_err;
}

/* ********** */

int conect(n)
{
	if( n < 0 ) {
		n = abs(n);
		if( n > 8 ) n = 8;
		omaio(FORCE_INIT,n);
	} else {
		if( n > 8 ) n = 8;
		if( n == 0 ) n=1;
		omaio(INIT,n);
	}
	return io_err;
}

/* ********** */

int run()
{
	omaio(RUN,0);
	return io_err;
}
/* ********** */


int discon()
{
	omaio(BYE);
	return io_err;
}

/* ********** */

int inform()
{
	omaio(INFO);
	return io_err;
}
/* ********** */

int flush()
{
	omaio(FLUSH);
	return io_err;
}

/* ********** */

int send(n,index)
int n,index;
#ifdef Mac
//#pragma unused (n)
#endif

{
	extern char txt[];
	strcpy(txt,&cmnd[index]);
	omaio(SEND,index,txt);
	return io_err;
}

/* ********** */

int ask(n,index)
int n,index;
#ifdef Mac
//#pragma unused (n)
#endif

{
	omaio(ASK,index);
	return io_err;
}

/* ********** */

int transfer()
{
	omaio(TRANS);
	return io_err;
}

/* ********** */

int receiv()
{
    //	omaio(TTIME);		// Put time in the log block first 
    //	header[NCHAN] = 390;	// allocate a bigish space for this 
    //	header[NTRAK] = 590;	// this may be too big but just need to be sure 
    /*	if(checkpar() == 1) return; */	
	
	omaio(RECEIVE);
    //	checkpar();		// go make the buffer the right size
	have_max = 0;
    /*	maxx();		Speed up. Put correct min/max in log block */
	return io_err;
}

/* ********** */
int gpibdv(n)
int n;
{
	extern short dev;
	
	dev = n-1;
	if( dev < 0 ) dev = 0;
	return 0;
}

/* ********** */


//-----------------------------------------------------------------------------------






short	dev = 0;							/* the current gpib device addressed */
short	numdev = 0;							/* the number of active gpib devices */
short 	devlist[16];						/* a list of gpib devices */
short	detlist[16];						/* identifies the detector type for each
                                             device */ 
char	poll;								/* the status of gpib stuff */
short	bd,cam;								/* the board and camera numbers */
short	forceinit = 0;						/* forces "successful" INIT even if error */

short gpib_time_flag = 1;					/* flag that determines if gpib time out is set
                                             GPIBTO command */

/* for the Star 1 */

extern int star_time;
extern int star_treg;
extern int star_auto;
extern int star_gain;

/* For Princeton Instruments */
int pi_chan_max[16],pi_track_max[16];
int j5;

int omaio(code,index,string)
int code,index;
char string[];

{
	extern int npts;
	extern DATAWORD *datpt;
	extern TWOBYTE header[];
	extern TWOBYTE trailer[];
	extern char cmnd[];
	extern char txt[];
	extern char syncflag;
	extern int  exflag,macflag;
	
	extern int	doffset;
	extern short pixsiz;
	extern char	lastname[];
	extern void redoMenus();
	
	extern short	detector;
	extern Variable user_variables[];
    /*	extern int scope_rec;
     extern FILE *fp_scope;*/
	
	/* Definitions for National Instruments VAX GPIB subroutines */
    
	static int bsize = 18000; /* Block size for data transfer */ 
    /* changed from 32000 (6-2) */
	static char trq = 192;	  /* Device request to talk    */
	static char ccdok = 32;	  	/* CC200 successful command code*/
	static int recimageno;		/* incremented each time a new image is received */
    
	
	
	unsigned char wfm[1028];            /* for waveform received from scope   */
	int checksum;						/* for checking whether data read from scope is garbage */
	
	
	int i,j,k,l;
	unsigned char spr;
	char *pointer,ch;
	short ccdheader[HEADERLENGTH] = {0};		/* the 80 word header from the ccd */
	char name[8];						/* for various names */
	
	int two_to_four(DATAWORD* dpt, int num, TWOBYTE scale);
    
	if(code != INIT && code != FORCE_INIT) {
		if( (numdev == 0) ){
			SysBeep(1);
			printf("Use CONECT First.\n");
			return -1; 
		}
		if( dev < 0 || dev > numdev-1 ) {
			SysBeep(1);
			printf("Reference to Non-Responding Device\n");
			return -1;
		}
		sprintf(name,"dev%d",devlist[dev]);
		cam = ibfind(name);
		ibonl(cam,1);	
	}
	
	
    
	switch (code) {
        case FORCE_INIT:
            forceinit = 1;	
        case INIT:		/* init gpib  */
            
            
            
            /*		This was for V1.0 of NI software but will not work for V 1.1 */
            /* 		Find the first gpib address 		*/
            /*
             *		for (i=1; i<=6; i++) {
             *			sprintf(name,"gpib%d",i);
             *			bd = ibfind(name);
             *			if( bd > 0) break;	
             *		}
             */
            sprintf(name,"gpib0");
            bd = ibfind(name);			/* this will find any board in Version 1.1 */
            if( gpib_time_flag) 
                ibtmo(bd,T100ms);			/* set  board timeout */
            else
                ibtmo(bd,TNONE);
            
            if(bd < 0) {
                SysBeep(1);
                printf("No GPIB board found.\n");
                return -1;
            }
            printf("GPIB Board Found.\n");	
            
            //ibdma(bd,DMAMODE); 			// set dma appropriately -- previously commented out; why?	
            
            numdev = 0;
            dev = 0;
            for(i=1; i<= index; i++) {				/* allow multiple devices */
                sprintf(name,"dev%d",i);
                printf("Looking for %s.\n",name);
                cam=ibfind(name);				/* the name of the CCD device -- use IBCONF */
                if(cam >= 0) {
                    printf("GPIB Interface Configured for dev%d\n",i);
                    if( gpib_time_flag)
                        ibtmo(cam,T100ms);			/* set camera timeout */
                    
#ifdef OLDROMS
                    ibwrt(cam,"2 xspd",6L);		/* "balls to the wall" speed wise OLD ROMS */
                    waitreply();
#endif
                    /*	ibrsp(cam,&poll);	*/
                    ibwrt(cam,"cisc",4L);
                    
                    delay(20);	/* wait a bit here, but don't do count on 
                                 PHOTOMETRICS-type waits to work */
                    
                    ibrsp(cam,&poll);
                    
                    if(poll < CCDSRQ){
                        printf("%d Returned Status.\n",poll);
                        if( forceinit ) {
                            devlist[numdev] = i;
                            printf("CONECT Forced: dev%d\n",i);
                        }
                    }
                    else {
                        devlist[numdev] = i;
                        printf("dev%d Responded.\n",i);
                    }
                    
                    if (! (poll & ccdok)) { 
                        /* Whoever is out there didn't like the CC-200 command */
                        /* Maybe it's a STAR_1 -- yeah, try that */
                        ibwrt(cam,"!A2",3);
                        delay(20);	/* wait a bit here, but don't do count on 
                                     PHOTOMETRICS-type waits to work */
                        ibwrt(cam,"@A",2);
                        delay(20);	/* wait a bit here, but don't do count on 
                                     PHOTOMETRICS-type waits to work */
                        ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
                        cmnd[ibcnt] = 0;
                        l=0;
                        sscanf(cmnd,"%d",&l);
                        if (l!=2) /*(! (poll & ccdok)) */{ 
                            /* Is it an oscilloscope? */
                            ibwrt(cam,"id?",3); /* Send query for scope id */
                            ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
                            if ((strspn(cmnd,"TEK") > 0) || (strspn(cmnd,"ID TEK") > 0)) {
                                detlist[numdev++] = SCOPE;
                                printf("TEK SCOPE\n"); 
                            }else{
                                /* NOT the SCOPE either -- must be PRINCETON INSTRUMENTS */
                                ibrsp(cam,&poll);
                                printf("poll:%x\n",poll);
                                ibwrt(cam,"er",2);	/* clear error light */
                                ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
                                ibwrt(cam,"RTA",3); 	/* give info using ASCII */
                                ibwrt(cam,"CL0",3);		/* set extra cleans to 0 */
                                ibwrt(cam,"AD15",4);	/* set AD bits to 15 */
                                printf("AD bits set to 15\n",cmnd);
                                ibwrt(cam,"as?",3);		/* get array size */
                                ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
                                cmnd[ibcnt] = 0;	/* put in end of message flag */
                                sscanf(cmnd,"%d,%d",&pi_track_max[numdev],&pi_chan_max[numdev]);
                                printf("Array Size %d x %d\n",pi_chan_max[numdev],
                                       pi_track_max[numdev]);
                                
                                /* If Tek 512x512 "D" chip, have to reconfigure Array Size Def
                                 (if it is any other Tek 512x512 chip, all bets are off...*/
                                ibwrt(cam,"cdG",3);		/* get array size */
                                ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
                                cmnd[ibcnt] = 0;	/* put in end of message flag */
                                sscanf(cmnd,"%d",&j);
                                if ((j-128)==5) {
                                    ibwrt(cam,"as512,512,1,0,15,0",18);		/* get array size */
                                }
                                
                                
                                detlist[numdev++] = PRINCETON_INSTRUMENTS_1;
                                printf("Princeton Instruments Detector.\n");
                            }
                        } else { 
                            printf("Star 1.\n");
                            detlist[numdev++] = STAR_1;
                            redoMenus(starMenu);
                            if(star_treg){ 
                                /*omaio(SEND,dummy,"!C1");*/
                                ibwrt(cam,"!C1",3);
                                printf("Temperature Regulated\n");
                                delay(10);
                            }else{
                                /*omaio(SEND,dummy,"!C0");*/
                                ibwrt(cam,"!C0",3);
                                printf("Not Tempurature Regulated\n");
                                delay(10);
                            }
                            if(star_gain){ 
                                /*omaio(SEND,dummy,"!B1");*/
                                ibwrt(cam,"!B1",3);
                                delay(10);
                                printf("High Gain (x4)\n");
                            }else{
                                /*omaio(SEND,dummy,"!B0");*/
                                ibwrt(cam,"!B0",3);
                                printf("Low Gain (x1)\n");
                                delay(10);
                            }
                            if(star_auto) {
                                /*omaio(SEND,dummy,"!I1");*/
                                ibwrt(cam,"!I1",3);
                                printf("Auto on\n");
                                delay(10);
                            }else{
                                /*omaio(SEND,dummy,"!I0");*/
                                ibwrt(cam,"!I0",3);
                                printf("Auto off\n");
                                delay(10);
                            }
                            sprintf(txt,"!A%d",star_time);	
                            /*omaio(SEND,dummy,txt);*/
                            for (l = 0; txt[l++] != EOL;){};
                            l--;
                            ibwrt(cam,txt,l);
                            printf("Integration Time: %3.1f sec\n",star_time/10.,l);
                            delay(10);
                        }
                    } else {
                        /* Is it really a cc200 ? */
                        
                        ibwrt(cam,"opt",3);	 /* Send query  */
                        
                        delay(60);
                        
                        ibrsp(cam,&spr);
                        ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */									 
                        
                        if ((strspn(cmnd,"3") > 0) || (strspn(cmnd,"2") > 0)) {
                            detlist[numdev++] = PHOTOMETRICS_CC200;
                            printf("CC 200.\n");
                            
                            /* Type out answer */
                            printf("    ");
                            while (ibcnt > 0) {
                                cmnd[ibcnt] = 0;	/* put in end of message flag */
                                printf("%s ",cmnd);
                                ibrd(cam,(char*)cmnd,CHPERLN);
                            }
                            printf("\n");  
                        }else{				
                            /* Is it an oscilloscope? */
                            
                            ibwrt(cam,"id?",3); /* Send query for scope id */
                            ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
                            if ((strspn(cmnd,"TEK") > 0) || (strspn(cmnd,"ID TEK") > 0)) {
                                detlist[numdev++] = SCOPE;
                                printf("TEK SCOPE\n"); 
                            }
                        }			
                        
                    }
                    
                }
            }
            forceinit = 0;
            if( numdev >= 1) {
                //setccd(1);					/* set detector to ccd type */
                detector = CCD;	
                pixsiz = 1;					/* want small pixels for CCDs */
                printf("%d Devices Responded\n",numdev);
                printf("Synch Status: %d.\n",syncflag);}
            else {
                beep();
                printf("No Devices Responded\n");
                return -1;
            }
            
            break;
            
            
        case BYE:		/* Put the controller back in local */
            ibloc(cam);	
            break;
            
        case RECEIVE:		/* Transfer data to the Mac from the CC200 */
            
            switch(detlist[dev]) {
                case STAR_1:
                    /* Transfer data to the Mac from the CC300 */
                    
                    bsize = 18000;
                    /*printf("this is star1\n");*/
                    omaio(RUN,0);				/* first, reset the parameters */
                    /*printf("reset params\n");*/
                    k = 0;
                    
                    if( (header[NDX] != 1) || (header[NDY] != 1) ) {
                        /* the Star 1 can't bin pixels; this is done internally */
                        i = header[NCHAN];	/* save the old values */
                        j = header[NTRAK];
                        k = header[NDX];
                        l = header[NDY];
                        
                        header[NCHAN] *= header[NDX];
                        header[NTRAK] *= header[NDY];
                        header[NDX] = header[NDY] = 1;
                        
                        if(checkpar()==1) {					/* allocate memory */
                            /* Not enough memory for this */
                            beep();
                            printf(" Memory Needed.\n");
                            header[NCHAN] = i;	/* restore the old values */
                            header[NTRAK] = j;
                            header[NDX] = k;
                            header[NDY] = l;
                            checkpar();			/* allocate as before */
                            return -1;
                        }
                    }
                    
                    
                    ibwrt(cam,":J",2L);			/* Send transmit command */
                    /*printf("sent transmit\n");	*/					
                    while(!(ibrsp(cam,&poll) & ERR)) {
                        if( poll == 0x60) 		/* OK */
                            break;
                        else if(poll == 0x40)	/* Error */
                            printf("error\n");
                    }
                    
                    if( gpib_time_flag)
                        ibtmo(cam,T1s);
                    
                    
                    for(j=0; j< HEADERLENGTH; j++)
                        *(datpt+j) = ccdheader[j];
                    
                    j = 0;					
                    pointer = (char*)(datpt + HEADERLENGTH);
                    
                    ibrd(cam,(char*)pointer,bsize);	/* read data; after read, ibcnt has # of bytes read */
                    /*printf("begin reading\n");*/
                    while ( ibcnt == bsize){
                        pointer += bsize; 
                        j += ibcnt;
                        ibrd(cam,(char*)pointer,bsize);
                        
                    }
                    j += ibcnt;
                    i = j;						
                    
                    printf("    %d Bytes Received.\n",j);
                    user_variables[0].ivalue = j;
                    user_variables[0].is_float = 0;
                    
                    two_to_four(datpt,j/2,1);
                    
                    pointer = (char*)(datpt + HEADERLENGTH);	
                    for(i=0; i<j; i+=2) {
                        ch = *(pointer+i);
                        *(pointer+i) = *(pointer+i+1);
                        *(pointer+i+1) = ch;
                    }
                    
                    if( gpib_time_flag)
                        ibtmo(cam,T100ms);
                    
                    sprintf(lastname,"Image %d", recimageno++);
                    
                    
                    if( k != 0 ) {
                        if(block(k,-l) == 1){
                            beep();
                            printf("Could Not Block Pixels. Number of Rows and Columns Changed.\n");
                            return -1;
                        }
                    }
                    
                    break;
                case PHOTOMETRICS_CC200:
                    
                    bsize = 18000;
                    
                    ibrsp(cam,&poll);			/* Clear service reqests  6-2 */
                    ibwrt(cam,"xmt",3L);		/* Send transmit command */
                    /* Read data		 */
                    /* printf(" xmt sent.\n"); */
                    /*  on an FX, we need some time between these commands */
                    delay(1);
                    
                    ibrd(cam,(char*)ccdheader,HEADERLENGTH*2);	/* read header */		
                    
                    npts  = ccdheader[60] * ccdheader[61];	/* OMA log block */
                    header[NCHAN] = ccdheader[60];
                    header[NTRAK] = ccdheader[61];
                    header[NDX] = ccdheader[62];
                    header[NDY] = ccdheader[63];
                    header[NX0] = ccdheader[58];
                    header[NY0] = ccdheader[59];
                    trailer[SFACTR] = 1;
                    
                    if(checkpar() == 1) {
                        printf(" %d Channels & %d Tracks Reset to 1.\n",header[NCHAN],header[NTRAK]);
                        header[NCHAN] = header[NTRAK] = npts = 1;
                        return -1;					/* not enough memory  -- this will leave things unread */
                    }			
                    for(j=0; j< HEADERLENGTH; j++)
                        *(datpt+j) = ccdheader[j];
                    
                    j = 0;					
                    pointer = (char*)(datpt + HEADERLENGTH);
                    
                    ibrd(cam,(char*)pointer,bsize);	/* read data; after read, ibcnt has # of bytes read */
                    
                    while ( ibcnt == bsize){
                        pointer += bsize; 
                        j += ibcnt;
                        ibrd(cam,(char*)pointer,bsize);
                        
                    }
                    j += ibcnt;
                    i = j;						
                    
                    printf("    %d Bytes Received.",j);
                    user_variables[0].ivalue = j;
                    user_variables[0].is_float = 0;
                    
                    
                    /* Ensure it wasn't garbage */
                    
                    if ( j<bsize && (npts*2)>bsize ) {
                        SysBeep(1);
                        printf("\n    That was garbage.");
                        
                        /* The garbage could be some little message that the CCD controller wanted to
                         get out. It's possible that there could be several of these. Try to flush
                         short messages and get to the (presumably long) data transfer. */
                        
                        while ( ibcnt<bsize && ibsta>0){	/* if short count and no errors, continue */
                            pointer = (char*)datpt; 
                            j = 0;
                            ibrd(cam,(char*)pointer,bsize);		/* read more */
                            j += ibcnt;
                            if(ibsta>0) 
                                pointer += ibcnt;
                            if ( ibcnt<bsize ) 
                                printf("\n    %d bytes flushed.",ibcnt); /* another short count */
                        }
                        
                        if (ibsta<0) { 
                            SysBeep(1);
                            printf("    ERROR, ABORT !\n");
                            exflag = macflag = 0;
                            break;  
                        }
                        ibrd(cam,(char*)pointer,bsize);
                        while (ibcnt == bsize) {
                            pointer += bsize;
                            j += ibcnt;
                            ibrd(cam,(char*)pointer,bsize);
                        }
                        j += ibcnt;
                        i = j;	
                        printf("\n    %d Bytes Received.",j);
                    }
                    ibrsp(cam,&poll);		 /* Clear service reqest  6-2 */
                    
                    j = *(datpt+60) * *(datpt+61) * 2;
                    
                    if (i != j ) 
                    { SysBeep(1);
                        printf("\n    Warning : %d Bytes were expected !",j);
                        printf("\n    Will FLUSH trash and abort.\n");
                        
                        exflag = macflag = 0; 
                        omaio(FLUSH);	/* Flush garbage on queue after data */
                    }
                    
                    printf("\n");
                    sprintf(lastname,"Image %d", recimageno++);
                    break;
                case PRINCETON_INSTRUMENTS_1:
                    
                    /* stuff moved to run command */
                    
                    ibwrt(cam,"SROFF",5); /* Disable service requests...prevents  FIFO overflow */
                    ibrsp(cam,&poll);
                    if( gpib_time_flag)
                        ibtmo(cam,T30s);
                    
                    /*________________*/
                    
                    pointer = (char*)(datpt + HEADERLENGTH);
                    bsize = (header[NCHAN]*header[NTRAK]*2)+1;
                    
                    
                    ibwrt(cam,"st",2);
                    
                    ibrd(cam,(char*)pointer,bsize);	/* read data; after read, ibcnt has # of bytes read */
                    
                    printf("    %d Bytes Received.\n",ibcnt);
                    user_variables[0].ivalue = ibcnt;
                    user_variables[0].is_float = 0;
                    
                    two_to_four(pointer,ibcnt/2,1);
                    
                    printf("poll:%x status: %x\n",poll,*(pointer+ibcnt-1));
                    if( gpib_time_flag)
                        ibtmo(cam,T100ms);
                    
                    ibwrt(cam,"SRON",4);
                    ibrsp(cam,&poll);
                    
                    break;
                    
                case SCOPE:
                    
                    bsize = 1028;             /* time of full scope scan is digitized into 
                                               1024 one byte segments       */	
                    header[NCHAN] = 1024;
                    header[NTRAK] = 1;
                    header[NDX] = 1;
                    header[NDY] = 1;
                    header[NX0] = 0;
                    header[NY0] = 0;
                    trailer[SFACTR] = 1;
                    
                    if(checkpar() == 1) {
                        printf(" %d Channels & %d Tracks Reset to 1.\n",header[NCHAN],header[NTRAK]);
                        header[NCHAN] = header[NTRAK] = npts = 1;
                        return -1;					/* not enough memory  -- this will leave things unread */
                    }	
                    
                    //ibonl(cam,1);		
                    
                    ibwrt(cam,"path off;data encdg:rpbinary",28L);   /* gets rid of path string
                                                                      which would precede data and specifies data format as pos. integers
                                                                      from 0-255      */
                    
                    ibwrt(cam,"curve?",6L);		  /* tells scope that you want to read waveform */
                    
                    ibrsp(cam,&poll);		 /* Clear service reqest  6-2 */
                    
                    ibrd(cam,wfm,bsize);          /* read waveform from scope into wfm[] */
                    
                    
                    
                    int nread = ibcnt;
                    printf("%d bytes received\n",nread);
                    
                    ibrsp(cam,&poll);		 /* Clear service reqest  6-2 */
                    
                    user_variables[0].ivalue = nread;
                    user_variables[0].is_float = 0;
                    
                    
                    checksum = 0;
                    for (i = 0; i<nread; i++){
                        if((i>0) && (i<nread)){
                            checksum += wfm[i];
                            checksum = checksum % 256;
                        }
                        /*if((i<10)||(i>1020)){
                         printf("%d    %d\n",i,wfm[i]); 
                         }*/
                        if((i>2) && (i<nread)){
                            *(datpt+i-3+doffset) = wfm[i];
                        }
                    }
                    checksum = 256 - checksum;
                    
                    if(checksum != wfm[nread-1]){
                        printf("Checksum?\n");
                    }
                    //ibonl(cam,0);	
                    break;				
                    
                    
                    
                    
                default:
                    break;
            } /* end of detector type switch for RECEIVE case */
            break; /* end of RECEIVE */
            
        case FLUSH:		/* Flush any data from the controller */
            pointer = (char*)datpt;
            for(ibrd(cam,(char*)pointer,bsize); ibcnt > 0; ibrd(cam,(char*)pointer,bsize)){
                printf("    %d Bytes Received.\n",ibcnt);
                pointer += ibcnt; 
            }
            break;
            
            
        case RUN:	/* fill the command buffer with a ccdformat command */
            
            switch(detlist[dev]) {
                case STAR_1:
                    set_star_param(cam,"!D",header[NX0]);
                    set_star_param(cam,"!E",header[NY0]);
                    set_star_param(cam,"!F",header[NDX]*header[NCHAN]);
                    set_star_param(cam,"!G",header[NDY]*header[NTRAK]);
                    omaio(SEND,index,":S");
                    break;
                    
                case PHOTOMETRICS_CC200:
                    
                    sprintf(cmnd,"%d %d %d %d %d %d ccdfmt",
                            header[NX0],header[NY0],header[NCHAN],header[NTRAK],
                            header[NDX],header[NDY]);
                    
                    for (i = 0; cmnd[i] != '\0'; i++){};
                    
                    ibwrt(cam,cmnd,i);				/* send format command */
                    waitreply();
                    break;
                    
                case PRINCETON_INSTRUMENTS_1:
                    
                    if ((header[NX0]+header[NDX]*header[NCHAN]>pi_chan_max[dev]) ||
                        (header[NY0]+header[NDY]*header[NTRAK]>pi_track_max[dev])){
                        printf("Invalid CCD specification (%d x %d max)\n",pi_chan_max[dev],
                               pi_track_max[dev]);
                        beep();
                        break;
                    }
                    /* Wait for bit 0 on jumper J5 to go high (default case) */
                    //			cmnd[2] =  " "; 
                    //			j5=0;			
                    //			while(j5!=5){	
                    //				ibwrt(cam,"RP",2);	 	// Send query  
                    //				ibrd(cam,(char*)cmnd,CHPERLN);	 // Read reply  
                    //				cmnd[ibcnt] = 0;	// put in end of message flag 
                    //				sscanf(&cmnd[2],"%d",&j5);							
                    //				printf("%d\n",j5);  								
                    //			}	 													
                    
                    
                    sprintf(cmnd,"sdb,1,%d,u,0,%d,%d,b,1,%d",
                            header[NX0],header[NDX]*header[NCHAN],header[NCHAN],
                            pi_chan_max[dev] - header[NX0] - header[NDX]*header[NCHAN]);
                    
                    for (i = 0; cmnd[i] != '\0'; i++){};
                    
                    /* send sdb command */
                    
                    ibwrt(cam,cmnd,i);				
                    ibrsp(cam,&poll);
                    /*			printf("%s\n",cmnd);*/
                    
                    if ((pi_track_max[dev] - header[NY0] - header[NDY]*header[NTRAK])!=0) {
                        sprintf(cmnd,"LDb,1,%d,u,0,%d,%d,b,1,%d",
                                header[NY0],header[NDY]*header[NTRAK],header[NTRAK],
                                pi_track_max[dev] - header[NY0] - header[NDY]*header[NTRAK]);
                    }else{
                        sprintf(cmnd,"LDb,1,%d,u,0,%d,%d",
                                header[NY0],header[NDY]*header[NTRAK],header[NTRAK]);
                    }
                    
                    for (i = 0; cmnd[i] != '\0'; i++){};
                    
                    /* send ldb command */
                    
                    ibwrt(cam,cmnd,i);				
                    ibrsp(cam,&poll);	
                    /*			printf("%s...\npoll %x\n",cmnd,poll);*/
                    
                    break;
                    
                default:
                    break;
            }
            break;
            
        case TRANS:		/* Transmit data To the camera controller */
            
            switch(detlist[dev]) {
                case PHOTOMETRICS_CC200:	
                    
                    j = i = ( npts + doffset ) * 2 ;  /* the number of bytes */
                    
                    /* Now initiate the transfer */
                    
                    pointer = (char*)datpt;
                    
                    ibwrt(cam,"rcv",3L);  			/* send receive command */
                    ibeot(cam,0);					/* disable EOT messages */
                    for (ibwrt(cam,pointer,bsize); i >= bsize; ibwrt(cam,pointer,bsize)) {
                        pointer += bsize;
                        i -= bsize; 
                    }
                    
                    ibeot(cam,1);					/* For this last one, send an end command */
                    ibwrt(cam,pointer,i); 
                    //			ibrsp(cam,&poll);		 		// Clear service reqest 
                    printf("poll: %d\n",poll);
                    
                    ibwrt(cam," ",1L);				/* dummy write */
                    //		ibrsp(cam,&poll); 
                    
                    break;
                case STAR_1:
                case PRINCETON_INSTRUMENTS_1:
                    printf("Not Used for this Detector.\n");
                    beep();
                    break;
                    
                default:
                    break;
            }
            break;
            
        case SEND:		/* Send a command to camera controller */
            switch(detlist[dev]) {
                case STAR_1:
                case PHOTOMETRICS_CC200:	
                    
                    for (i = 0; string[i++] != EOL;){};
                    i--;
                    //			ibrsp(cam,&poll);		 		// Clear service reqests 
                    //			printf("    poll:%d;\n",poll);
                    
                    ibwrt(cam,string,i);		// Send commmand 
                    
                    if (!syncflag) break;			// If no sync, don't wait for CC200 
                    
                    waitreply();
                    
                    if (! (poll & ccdok)) { 
                        SysBeep(1);
                        printf("    Command Error: %d\n",poll);
                        break;
                    }
                    else 
                        if ( poll != trq) 
                            break;
                        else { 
                            printf("    Controller replied!  Use ASK.\n"); 
                            omaio(FLUSH);
                        }
                    break;
                    
                case PRINCETON_INSTRUMENTS_1:
                    
                    for (i = 0; string[i++] != EOL;){};
                    i--;
                    
                    ibrsp(cam,&poll);
                    ibwrt(cam,string,i);		/* Send commmand */
                    ibrsp(cam,&poll);
                    printf("    poll:%x\n",poll);
                    break;
                    
                case SCOPE:
                    
                    for (i = 0; string[i++] != EOL;){};
                    i--;
                    //			printf("cam = %d comm length = %d  command = %s\n",cam,i,string);
                    ibwrt(cam,string,i);	 // Send command 
                    break;	
                    
                    
                default:
                    break;
            }
            break;
            
        case ASK:	// Send command to CC200, wait for reply and type it 
            switch(detlist[dev]) {
                case STAR_1:
                case PHOTOMETRICS_CC200:	
                    i = 0;
                    for (j = index; cmnd[j++] != EOL; i++){};
                    //			ibrsp(cam,&poll);		 	// Clear service reqests 
                    
                    ibwrt(cam,&cmnd[index],i);	 // Send query  
                    
                    waitreply();
                    
                    if ( !(poll&ccdok)) { 
                        printf("    Command Error.\n");
                        beep();
                        break;
                    }
                    
                    ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
                    
                    if (ibcnt<=0) { 
                        printf("    No reply - none expected.\n");
                        break;
                    }
                    
                    /* Type out answer */
                    printf("    ");
                    while (ibcnt > 0) {
                        cmnd[ibcnt] = 0;	/* put in end of message flag */
                        printf("%s ",cmnd);
                        ibrd(cam,(char*)cmnd,CHPERLN);
                    }
                    printf("\n");                                           
                    break;
                    
                case PRINCETON_INSTRUMENTS_1:
                    
                    i = 0;
                    for (j = index; cmnd[j++] != EOL; i++){};
                    
                    ibrsp(cam,&poll);
                    ibwrt(cam,&cmnd[index],i);	 /* Send query  */
                    ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
                    ibrsp(cam,&poll);
                    
                    if (ibcnt<=0) { 
                        printf("    No reply - none expected.\n");
                        break;
                    }
                    
                    /* Type out answer */
                    printf("    ");
                    cmnd[ibcnt] = 0;	/* put in end of message flag */
                    printf("%s \n",cmnd);
                    break;
                    
                case SCOPE:
                    
                    i = 0;
                    for (j = index; cmnd[j++] != EOL; i++){};
                    
                    ibwrt(cam,&cmnd[index],i);	 /* Send query  */
                    
                    ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
                    
                    if (ibcnt<=0) { 
                        printf("    No reply - none expected.\n");
                        break;
                    }
                    
                    /* Type out answer */
                    printf("    ");
                    cmnd[ibcnt] = 0;	/* put in end of message flag */
                    printf("%s \n",cmnd);
                    /*			if (scope_rec == 1) {
                     fprintf(fp_scope,"%s\n",cmnd);
                     }
                     */			break;
                    
                default:
                    break;
            }	
            
            break;
            
            //	case TTIME:		// Print out time and put it in log 
            //
            //		lib$date_time(&desc);			// Get time 
            //		printf("    ");				// Print it 
            //		for (j=0; j!=22; putchar(tstring[j++]));
            //		printf("\n");
            //					// Time in 1st line of log  
            //		for (j=0; j!=22; comment[j]=tstring[j++]);
            //		comment[j] = EOL;	// Terminate log entry	    
            //		break; 
	}
	ibonl(cam,0);
	setarrow();
	return 0;
}

waitreply()	 /* Await reply */

{
	long 	ticks,t2;								/* for changing watches */
	unsigned char spr;
	int i;
	
	ticks = t2 = TickCount();
	poll = 0;
	
	for(i=0;i<10000;i++){};
	
	while( ticks+60*TIMEOUT > TickCount() ) {
		if( !(ibwait(bd,SRQI | TIMO) & TIMO) ) {
			while( !(ibrsp(cam,&spr) & ERR) ) {
				if(spr == 0x60)
					poll = spr;
				else if(spr == 0x40) {
					poll = spr;
					return 0; }
				
				else if (poll == 0x60)
					return 0;
				if( TickCount() > (t2+10)) {		
					t2 = TickCount();
					
				}
				
			}
			return 0;
		}
		if( TickCount() > (t2+10)) {		
			t2 = TickCount();
			
		}
	}
	return 0;
}

set_star_param(cam,cmd,value)
char cmd[];
short cam,value;

{
	extern char txt[];
	char poll;
	
	sprintf(txt,"%s%d",cmd,value);
	if(ibwrt(cam,txt,strlen(txt)) & ERR)
		return false;
	while( !(ibrsp(cam,&poll) & ERR)) {
		if( poll == 0x60)
			break;
		else if (poll == 0x40)
			return false;
	}
	return true;
}

// end of GPIB conditional
#endif

#else
// this is the TEXACO not defined code
#ifdef VISA

#include <stdio.h>
#include <stdlib.h>

#include <VISA/visa.h>

static char instrDescriptor[VI_FIND_BUFLEN];
static ViUInt32 numInstrs;
static ViFindList findList;
static ViSession defaultRM, instr;
static ViStatus status;
static ViUInt32 retCount;
static ViUInt32 writeCount;
static ViEvent ehandle;
static ViEventType etype;


//-----------------------------------------------------------------------------------

extern char	   syncflag;
int io_err = 0;


/* ********** */

int synch(n)	// Set flag for CC200/uVAX synchronization 
int n;
{
	syncflag = n;
	return io_err;
}

/* ********** */

int conect(n)
{
	if( n < 0 ) {
		n = abs(n);
		if( n > 8 ) n = 8;
		omaio(FORCE_INIT,n);
	} else {
		if( n > 8 ) n = 8;
		if( n == 0 ) n=1;
		omaio(INIT,n);
	}
	return io_err;
}

/* ********** */

int run()
{
	omaio(RUN,0);
	return io_err;
}
/* ********** */


int discon()
{
	omaio(BYE);
	return io_err;
}

/* ********** */

int inform()
{
	omaio(INFO);
	return io_err;
}
/* ********** */

int flush()
{
	omaio(FLUSH);
	return io_err;
}

/* ********** */

int send(int n,int index)
{
	extern char txt[];
	strcpy(txt,&cmnd[index]);
	omaio(SEND,index,txt);
	return io_err;
}

/* ********** */

int ask(int n,int index)
{
	omaio(ASK,index);
	return io_err;
}

/* ********** */

int transfer()
{
	omaio(TRANS);
	return io_err;
}

/* ********** */

int receiv()
{
//	omaio(TTIME);		// Put time in the log block first 
//	header[NCHAN] = 390;	// allocate a bigish space for this 
//	header[NTRAK] = 590;	// this may be too big but just need to be sure 
//	if(checkpar() == 1) return; 	
	
	omaio(RECEIVE);
//	checkpar();		// go make the buffer the right size
	have_max = 0;
//	maxx();		Speed up. Put correct min/max in log block 
	return io_err;
}

/* ********** */
int gpibdv(int n)
{
	extern short dev;
	
	dev = n-1;
	if( dev < 0 ) dev = 0;
	return 0;
}

/* ********** */


//-----------------------------------------------------------------------------------


/* Global variable meanings:

dev -- index that tells what instrument (device) is currently being accessed 
		dev = n-1, where n comes from the "GPIB n" command
dev indexes arrays:

detlist
*/




short	dev = 0;							/* the current gpib device addressed */
short	numdev = 0;							/* the number of active gpib devices */
//short 	devlist[16];						/* a list of gpib devices */
short	detlist[16];						// identifies the detector type for each device 

char instrDescriptorList[16][VI_FIND_BUFLEN];
char	poll;								/* the status of gpib stuff */
short	bd,cam;								/* the board and camera numbers */
short	forceinit = 0;						/* forces "successful" INIT even if error */

short gpib_time_flag = 1;					/* flag that determines if gpib time out is set
											   GPIBTO command */

/* for the Star 1 */

extern int star_time;
extern int star_treg;
extern int star_auto;
extern int star_gain;

/* For Princeton Instruments */
int pi_chan_max[16],pi_track_max[16];
int j5;


int star_send(char* string){

	
	status = viSetAttribute (instr, VI_ATTR_TMO_VALUE, 3000);		// 3 second timeout
	status = viWrite (instr, (ViBuf)string, strlen(string), &writeCount);
	if (status < VI_SUCCESS) {
		printf("    Error writing to the device\n");
		status = viClose(instr);
		return -1;
	}


	if (!syncflag) return 0;			// If no sync, don't wait for CC200 
	
	status = viWaitOnEvent (instr, VI_EVENT_SERVICE_REQ, 3000, &etype, &ehandle);	// wait 3 seconds
	ViUInt16 statusByte;
	viReadSTB (instr, &statusByte);

	return 0;
}

int set_star_param(char* cmd, int value)
{
	extern char txt[];

	sprintf(txt,"%s%d",cmd,value);
	//printf("%s%d",cmd,value);
	status = viWrite (instr, (ViBuf)txt, strlen(txt), &writeCount);
	if (status < VI_SUCCESS) {
		printf("    Error writing to the device\n");
		status = viClose(instr);
		return -1;
	}
	status = viWaitOnEvent (instr, VI_EVENT_SERVICE_REQ, 1000, &etype, &ehandle);	// wait 1 second
	ViUInt16 statusByte;
	viReadSTB (instr, &statusByte);
	//printf("  Status: %x\n",statusByte); 
	if( statusByte == 0x60)
		return 0;
	else if (statusByte == 0x40)
		return 1;
	return 2;
}


int omaio(int code,int index, char* string)
//char string[];
{
	extern int npts;
	extern DATAWORD *datpt;
	extern TWOBYTE header[];
	extern TWOBYTE trailer[];
	extern char cmnd[];
	extern char txt[];
	extern char syncflag;
	extern int  exflag,macflag;
	extern int is_big_endian;
	
	extern int	doffset;
	extern short pixsiz;
	extern char	lastname[];
	extern void redoMenus();
	
	extern short	detector;
	extern Variable user_variables[];
/*	extern int scope_rec;
	extern FILE *fp_scope;*/
	
	// Definitions for National Instruments VAX GPIB subroutines 

	static int bsize = 18000; /* Block size for data transfer */ 
							  /* changed from 32000 (6-2) */
	static char trq = 192;	  /* Device request to talk    */
	static char ccdok = 32;	  	/* CC200 successful command code*/
	static int recimageno;		/* incremented each time a new image is received */

	
	unsigned char wfm[1028];            /* for waveform received from scope   */
	int checksum;						/* for checking whether data read from scope is garbage */
	
	
	int i,j,k,l;
	unsigned char spr;
	char *pointer,ch;
	short ccdheader[HEADERLENGTH] = {0};		/* the 80 word header from the ccd */
	char name[8];						/* for various names */
	
	int two_to_four(DATAWORD* dpt, int num, TWOBYTE scale);
		
	if(code != INIT && code != FORCE_INIT) {
		if( (numdev == 0) ){
			SysBeep(1);
			printf("Use CONECT First.\n");
			return -1; 
		}
		if( dev < 0 || dev > numdev-1 ) {
			SysBeep(1);
			printf("Reference to Non-Responding Device\n");
			return -1;
		}
		// get the name of the instrument already opened
		strcpy(instrDescriptor,&instrDescriptorList[dev][0]);
		// open the instrument
		status = viOpen (defaultRM, instrDescriptor, VI_NULL, VI_NULL, &instr);
		if (status < VI_SUCCESS) {
			  printf ("An error occurred opening a session to %s\n",instrDescriptor);
			  return status;
		}
		//  Now we must enable the service request event so that VISA will receive the events.
		status = viEnableEvent (instr, VI_EVENT_SERVICE_REQ, VI_QUEUE, VI_NULL);
	    if (status < VI_SUCCESS){
			printf("The SRQ event could not be enabled");
			status = viClose(instr);
			return -1;
	   }

	}

	switch (code) {
	case FORCE_INIT:
		forceinit = 1;	
	case INIT:		// init gpib  
			numdev = 0;
		   // First we will need to open the default resource manager. This stays open
		   status = viOpenDefaultRM (&defaultRM);
		   if (status < VI_SUCCESS)
		   {
			  printf("Could not open a session to the VISA Resource Manager!\n");
			  return -1;
		   }  

			/*
			 * Find all the VISA resources in our system and store the number of resources
			 * in the system in numInstrs.  Notice the different query descriptions a
			 * that are available.

				Interface         Expression
			--------------------------------------
				GPIB              "GPIB[0-9]*::?*INSTR"
				VXI               "VXI?*INSTR"
				GPIB-VXI          "GPIB-VXI?*INSTR"
				Any VXI           "?*VXI[0-9]*::?*INSTR"
				Serial            "ASRL[0-9]*::?*INSTR"
				PXI               "PXI?*INSTR"
				All instruments   "?*INSTR"
				All resources     "?*"
				visa://cld6.eng.yale.edu/PXI0::6::INSTR
				visa://cld6.eng.yale.edu/GPIB[0-9]*::?*INSTR
			*/
			
			// find GPIB resources
		   status = viFindRsrc (defaultRM,"GPIB[0-9]*::?*INSTR", &findList, &numInstrs, instrDescriptor);
		   if (status < VI_SUCCESS)
		   {
			  printf ("An error occurred while finding resources.\n");
			  viClose (defaultRM);
			  return status;
		   }

		   printf("%d GPIB devices found.",numInstrs);
		   numdev = numInstrs;
		   
		   for(i=0; i< numInstrs; i++){
			   if(i != 0){
				  status = viFindNext (findList, instrDescriptor);  /* find next desriptor */
				  if (status < VI_SUCCESS) 
				  {   /* did we find the next resource? */
					 printf ("An error occurred finding the next resource.");
					 //viClose (defaultRM);
					 continue; 
				  } 
				}

			   detlist[i] = STAR_1;
			   printf("\nGPIB %d:\t%s \n",i+1,instrDescriptor);

			   // Now we will open a session to the instrument we just found.
			   status = viOpen (defaultRM, instrDescriptor, VI_NULL, VI_NULL, &instr);
			   if (status < VI_SUCCESS) {
				  printf ("An error occurred opening a session to %s\n",instrDescriptor);
				  return status;
			   } else {
					strcpy(&instrDescriptorList[i][0],instrDescriptor);
					status = viSetAttribute (instr, VI_ATTR_TMO_VALUE, 1000);		// 1 second timeout
					strcpy(name,"ID?");
					status = viWrite (instr, (ViBuf)name, strlen(name), &writeCount);
					if (status < VI_SUCCESS) {
						printf("    Error writing to the device\n");
						status = viClose(instr);
						continue;
					}
					status = viRead (instr, wfm, 100, &retCount);
					if (status < VI_SUCCESS) {
						// no response from the ID querry, look for specific cameras that we have
						// first, the STAR 1
						star_send("!A2");	// try to set and read the exposure time
						star_send("@A");
						status = viRead (instr, wfm, 100, &retCount);
						if (status < VI_SUCCESS) { // it's not a STAR 1
								printf("    Error reading a response from the device\n");
								status = viClose(instr);
								continue;
						}
						// just check to be sure this is right
						sscanf((char*)wfm,"%d",&l);
						if( l == 2){
							  printf("\tSTAR 1\n");
							  status = viClose(instr);
							  continue;
						}
						printf("    Error reading a response from the device\n");
						status = viClose(instr);
						continue;
				   } else {
					if ((strspn((char*)wfm,"TEK") >= 3) || (strspn((char*)wfm,"ID TEK") >= 5)) {
						detlist[i] = SCOPE;
						printf("\tTEK SCOPE\n"); 
					}
					printf("\tData read: %*s\n",retCount,wfm);
				   }
					viClose (instr);
			   }
			}

		   status = viClose(findList);
		   //status = viClose (defaultRM);
			
	
		break;


	case BYE:		/* Put the controller back in local */
		//ibloc(cam);	
		break;

	case RECEIVE:		
	
		switch(detlist[dev]) {
		case STAR_1:
		
			//omaio(RUN,0,0);				// first, reset the parameters
			set_star_param("!D",header[NX0]);
			set_star_param("!E",header[NY0]);
			set_star_param("!F",header[NDX]*header[NCHAN]);
			set_star_param("!G",header[NDY]*header[NTRAK]);
			star_send(":S");

			for(j=0; j< HEADERLENGTH; j++)
				*(datpt+j) = ccdheader[j];
 
			j = 0;					
			pointer = (char*)(datpt+HEADERLENGTH/2);

			strcpy(name,":J");
			status = viWrite (instr, (ViBuf)name, strlen(name), &writeCount);
			if (status < VI_SUCCESS) {
				printf("    Error writing to the device\n");
				status = viClose(instr);
				//status = viClose(defaultRM);
				return -1;
			}
			status = viWaitOnEvent (instr, VI_EVENT_SERVICE_REQ, 5000, &etype, &ehandle);	// wait 5 seconds
			ViUInt16 statusByte;
			viReadSTB (instr, &statusByte);
			status = viRead (instr, pointer, header[NCHAN]*2*header[NTRAK], &retCount);
			if (status < VI_SUCCESS) {
				printf("    Error reading a response from the device\n");
				status = viClose(instr);
				//status = viClose(defaultRM);
				return -1;
		   } else {
			  printf("%d Bytes read\n",retCount);
		   }
		   
		   
		   if(is_big_endian){
				pointer = (char*)(datpt + HEADERLENGTH/2);	
				for(i=0; i<header[NCHAN]*2*header[NTRAK]; i+=2) {
					ch = *(pointer+i);
					*(pointer+i) = *(pointer+i+1);
					*(pointer+i+1) = ch;
				}
		   }
			user_variables[0].ivalue = retCount;
			user_variables[0].is_float = 0;

		   two_to_four(datpt,retCount/2,1);
		   

			viClose (instr);

			/*						

			two_to_four(datpt,j/2,1);

		*/


			
			break;
		case PHOTOMETRICS_CC200:

			break;
		case PRINCETON_INSTRUMENTS_1:

			break;
		
		case SCOPE:
			
			bsize = 1028;             /* time of full scope scan is digitized into 
										1024 one byte segments       */	
			header[NCHAN] = 1024;
			header[NTRAK] = 1;
			header[NDX] = 1;
			header[NDY] = 1;
			header[NX0] = 0;
			header[NY0] = 0;
	   		trailer[SFACTR] = 1;

			if(checkpar() == 1) {
				printf(" %d Channels & %d Tracks Reset to 1.\n",header[NCHAN],header[NTRAK]);
				header[NCHAN] = header[NTRAK] = npts = 1;
				return -1;					/* not enough memory  -- this will leave things unread */
			}	
			
			//ibonl(cam,1);		
			
			//ibwrt(cam,"path off;data encdg:rpbinary",28L);   // gets rid of path string
				//which would precede data and specifies data format as pos. integers
				//from 0-255      */
			
			//ibwrt(cam,"curve?",6L);		  /* tells scope that you want to read waveform */
			star_send("curve?");
			status = viRead (instr, wfm, bsize, &retCount);
			if (status < VI_SUCCESS) {
				printf("    Error writing to the device\n");
				status = viClose(instr);
				return -1;
			}
			status = viClose(instr);
			//ibrsp(cam,&poll);		 /* Clear service reqest  6-2 */
			
			//ibrd(cam,wfm,bsize);          /* read waveform from scope into wfm[] */
			
			
			
			//int nread = ibcnt;
			int nread = retCount;
			printf("%d bytes received\n",nread);
			
			//ibrsp(cam,&poll);		 /* Clear service reqest  6-2 */
			
			user_variables[0].ivalue = nread;
			user_variables[0].is_float = 0;


			checksum = 0;
			for (i = 0; i<nread; i++){
				if((i>0) && (i<nread)){
					checksum += wfm[i];
					checksum = checksum % 256;
				}
				/*if((i<10)||(i>1020)){
					printf("%d    %d\n",i,wfm[i]); 
				}*/
				if((i>2) && (i<nread)){
					*(datpt+i-3+doffset) = wfm[i];
				}
			}
			checksum = 256 - checksum;

			if(checksum != wfm[nread-1]){
				printf("Checksum?\n");
			}
			//ibonl(cam,0);	
			break;				
				
				
		default:
			break;
		} /* end of detector type switch for RECEIVE case */
		break; /* end of RECEIVE */
		
	case FLUSH:		/* Flush any data from the controller */
		pointer = (char*)datpt;


		break;

		
	case RUN:	/* fill the command buffer with a ccdformat command */
		
		switch(detlist[dev]) {
		case STAR_1:
			set_star_param("!D",header[NX0]);
			set_star_param("!E",header[NY0]);
			set_star_param("!F",header[NDX]*header[NCHAN]);
			set_star_param("!G",header[NDY]*header[NTRAK]);
			star_send(":S");
			status = viClose(instr);
			break;
		
		case PHOTOMETRICS_CC200:
	
			sprintf(cmnd,"%d %d %d %d %d %d ccdfmt",
		  		header[NX0],header[NY0],header[NCHAN],header[NTRAK],
		  		header[NDX],header[NDY]);
		
			for (i = 0; cmnd[i] != '\0'; i++){};
		
			//ibwrt(cam,cmnd,i);				/* send format command */
			waitreply();
			break;
			
		case PRINCETON_INSTRUMENTS_1:

			if ((header[NX0]+header[NDX]*header[NCHAN]>pi_chan_max[dev]) ||
			   (header[NY0]+header[NDY]*header[NTRAK]>pi_track_max[dev])){
				printf("Invalid CCD specification (%d x %d max)\n",pi_chan_max[dev],
						pi_track_max[dev]);
				beep();
				break;
			}
			/* Wait for bit 0 on jumper J5 to go high (default case) */
//			cmnd[2] =  " "; 
//			j5=0;			
//			while(j5!=5){	
//				ibwrt(cam,"RP",2);	 	// Send query  
//				ibrd(cam,(char*)cmnd,CHPERLN);	 // Read reply  
//				cmnd[ibcnt] = 0;	// put in end of message flag 
//				sscanf(&cmnd[2],"%d",&j5);							
//				printf("%d\n",j5);  								
//			}	 													

			
			sprintf(cmnd,"sdb,1,%d,u,0,%d,%d,b,1,%d",
		  			header[NX0],header[NDX]*header[NCHAN],header[NCHAN],
		 			pi_chan_max[dev] - header[NX0] - header[NDX]*header[NCHAN]);

			for (i = 0; cmnd[i] != '\0'; i++){};

			/* send sdb command */

			//ibwrt(cam,cmnd,i);				
			//ibrsp(cam,&poll);
/*			printf("%s\n",cmnd);*/

			if ((pi_track_max[dev] - header[NY0] - header[NDY]*header[NTRAK])!=0) {
				sprintf(cmnd,"LDb,1,%d,u,0,%d,%d,b,1,%d",
		  				header[NY0],header[NDY]*header[NTRAK],header[NTRAK],
		 	 			pi_track_max[dev] - header[NY0] - header[NDY]*header[NTRAK]);
			}else{
				sprintf(cmnd,"LDb,1,%d,u,0,%d,%d",
		  				header[NY0],header[NDY]*header[NTRAK],header[NTRAK]);
			}
		
			for (i = 0; cmnd[i] != '\0'; i++){};

			/* send ldb command */

			//ibwrt(cam,cmnd,i);				
 			//ibrsp(cam,&poll);	
/*			printf("%s...\npoll %x\n",cmnd,poll);*/

			break;

		default:
			break;
		}
		break;

	case TRANS:		/* Transmit data To the camera controller */

		switch(detlist[dev]) {
		case PHOTOMETRICS_CC200:	
		
			j = i = ( npts + doffset ) * 2 ;  /* the number of bytes */

			/* Now initiate the transfer */
		
			pointer = (char*)datpt;

			//ibwrt(cam,"rcv",3L);  			/* send receive command */
			//ibeot(cam,0);					/* disable EOT messages */
			/*for (ibwrt(cam,pointer,bsize); i >= bsize; ibwrt(cam,pointer,bsize)) {
				pointer += bsize;
				i -= bsize; 
			}
			*/
			//ibeot(cam,1);					/* For this last one, send an end command */
			//ibwrt(cam,pointer,i); 
//			ibrsp(cam,&poll);		 		// Clear service reqest 
			printf("poll: %d\n",poll);
		
			//ibwrt(cam," ",1L);				/* dummy write */
	//		ibrsp(cam,&poll); 

			break;
		case STAR_1:
		case PRINCETON_INSTRUMENTS_1:
			beep();
			printf("Not Used for this Detector.\n");
			break;

		default:
			break;
		}
		break;

	case SEND:		/* Send a command to camera controller */
		switch(detlist[dev]) {
		case SCOPE:
		case STAR_1:
		case PHOTOMETRICS_CC200:	
			star_send( string);
			status = viClose(instr);
			break;
			
		case PRINCETON_INSTRUMENTS_1:

			for (i = 0; string[i++] != EOL;){};
			i--;
			
			//ibrsp(cam,&poll);
			//ibwrt(cam,string,i);		/* Send commmand */
			//ibrsp(cam,&poll);
			printf("    poll:%x\n",poll);
			break;
			
			
		default:
			break;
		}
		break;

	case ASK:	// Send command to CC200, wait for reply and type it 
		switch(detlist[dev]) {
		case STAR_1:
			

			status = viSetAttribute (instr, VI_ATTR_TMO_VALUE, 3000);		// 3 second timeout
			//strcpy(&cmnd[index],"@F");
			if( star_send(&cmnd[index]) == -1) return -1;
			status = viRead (instr, wfm, 100, &retCount);
			if (status < VI_SUCCESS) {
				printf("    Error reading a response from the device\n");
				status = viClose(instr);
				return -1;
		   } else {
			  printf("Data read: %*s\n",retCount,wfm);
		   }
			viClose (instr);

			break;

		
		case PHOTOMETRICS_CC200:	
			i = 0;
			break;

	case SCOPE:
		
			i = 0;
			for (j = index; cmnd[j++] != EOL; i++){};

			//ibwrt(cam,&cmnd[index],i);	 /* Send query  */

			//ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
		/*
			if (ibcnt<=0) { 
				printf("    No reply - none expected.\n");
				break;
			}
		*/					 
			/* Type out answer */
			printf("    ");
			//cmnd[ibcnt] = 0;	/* put in end of message flag */
			printf("%s \n",cmnd);
/*			if (scope_rec == 1) {
				fprintf(fp_scope,"%s\n",cmnd);
			}
*/			break;

		default:
			break;
		}	
		
		break;

//	case TTIME:		// Print out time and put it in log 
//
//		lib$date_time(&desc);			// Get time 
//		printf("    ");				// Print it 
//		for (j=0; j!=22; putchar(tstring[j++]));
//		printf("\n");
//					// Time in 1st line of log  
//		for (j=0; j!=22; comment[j]=tstring[j++]);
//		comment[j] = EOL;	// Terminate log entry	    
//		break; 
	}
	//ibonl(cam,0);
	
	return 0;
}

waitreply()	 /* Await reply */

{


	/*
	long 	ticks,t2;								// for changing watches 
	unsigned char spr;
	int i;
	
	ticks = t2 = TickCount();
	poll = 0;
	
	for(i=0;i<10000;i++){};
	
	while( ticks+60*TIMEOUT > TickCount() ) {
		if( !(ibwait(bd,SRQI | TIMO) & TIMO) ) {
			while( !(ibrsp(cam,&spr) & ERR) ) {
				if(spr == 0x60)
					poll = spr;
				else if(spr == 0x40) {
					poll = spr;
					return 0; }
				
				else if (poll == 0x60)
					return 0;
				if( TickCount() > (t2+10)) {		
					t2 = TickCount();
					
				}
				
			}
			return 0;
		}
		if( TickCount() > (t2+10)) {		
			t2 = TickCount();
			
		}
	}
	*/
	return 0;
}

// end of VISA conditional
// if VISA is defined, the following GPIB stuff is ignored

#else

// If GPIB is not defined in impMach-O-cam.h, this will all be skipped
#ifdef GPIB

#ifdef __MACH__
#include <NI488/ni488.h>
#else
#include "ni488.h"
#endif
#include <stdio.h>

//-----------------------------------------------------------------------------------

extern char	   syncflag;
int io_err = 0;


/* ********** */

int synch(n)	/* Set flag for CC200/uVAX synchronization */
int n;
{
	syncflag = n;
	return io_err;
}

/* ********** */

int conect(n)
{
	if( n < 0 ) {
		n = abs(n);
		if( n > 8 ) n = 8;
		omaio(FORCE_INIT,n);
	} else {
		if( n > 8 ) n = 8;
		if( n == 0 ) n=1;
		omaio(INIT,n);
	}
	return io_err;
}

/* ********** */

int run()
{
	omaio(RUN,0);
	return io_err;
}
/* ********** */


int discon()
{
	omaio(BYE);
	return io_err;
}

/* ********** */

int inform()
{
	omaio(INFO);
	return io_err;
}
/* ********** */

int flush()
{
	omaio(FLUSH);
	return io_err;
}

/* ********** */

int send(n,index)
int n,index;
#ifdef Mac
//#pragma unused (n)
#endif

{
	extern char txt[];
	strcpy(txt,&cmnd[index]);
	omaio(SEND,index,txt);
	return io_err;
}

/* ********** */

int ask(n,index)
int n,index;
#ifdef Mac
//#pragma unused (n)
#endif

{
	omaio(ASK,index);
	return io_err;
}

/* ********** */

int transfer()
{
	omaio(TRANS);
	return io_err;
}

/* ********** */

int receiv()
{
//	omaio(TTIME);		// Put time in the log block first 
//	header[NCHAN] = 390;	// allocate a bigish space for this 
//	header[NTRAK] = 590;	// this may be too big but just need to be sure 
/*	if(checkpar() == 1) return; */	
	
	omaio(RECEIVE);
//	checkpar();		// go make the buffer the right size
	have_max = 0;
/*	maxx();		Speed up. Put correct min/max in log block */
	return io_err;
}

/* ********** */
int gpibdv(n)
int n;
{
	extern short dev;
	
	dev = n-1;
	if( dev < 0 ) dev = 0;
	return 0;
}

/* ********** */


//-----------------------------------------------------------------------------------






short	dev = 0;							/* the current gpib device addressed */
short	numdev = 0;							/* the number of active gpib devices */
short 	devlist[16];						/* a list of gpib devices */
short	detlist[16];						/* identifies the detector type for each
											   device */ 
char	poll;								/* the status of gpib stuff */
short	bd,cam;								/* the board and camera numbers */
short	forceinit = 0;						/* forces "successful" INIT even if error */

short gpib_time_flag = 1;					/* flag that determines if gpib time out is set
											   GPIBTO command */

/* for the Star 1 */

extern int star_time;
extern int star_treg;
extern int star_auto;
extern int star_gain;

/* For Princeton Instruments */
int pi_chan_max[16],pi_track_max[16];
int j5;

int omaio(code,index,string)
int code,index;
char string[];

{
	extern int npts;
	extern DATAWORD *datpt;
	extern TWOBYTE header[];
	extern TWOBYTE trailer[];
	extern char cmnd[];
	extern char txt[];
	extern char syncflag;
	extern int  exflag,macflag;
	
	extern int	doffset;
	extern short pixsiz;
	extern char	lastname[];
	extern void redoMenus();
	
	extern short	detector;
	extern Variable user_variables[];
/*	extern int scope_rec;
	extern FILE *fp_scope;*/
	
	/* Definitions for National Instruments VAX GPIB subroutines */

	static int bsize = 18000; /* Block size for data transfer */ 
							  /* changed from 32000 (6-2) */
	static char trq = 192;	  /* Device request to talk    */
	static char ccdok = 32;	  	/* CC200 successful command code*/
	static int recimageno;		/* incremented each time a new image is received */

	
	
	unsigned char wfm[1028];            /* for waveform received from scope   */
	int checksum;						/* for checking whether data read from scope is garbage */
	
	
	int i,j,k,l;
	unsigned char spr;
	char *pointer,ch;
	short ccdheader[HEADERLENGTH] = {0};		/* the 80 word header from the ccd */
	char name[8];						/* for various names */
	
	int two_to_four(DATAWORD* dpt, int num, TWOBYTE scale);
		
	if(code != INIT && code != FORCE_INIT) {
		if( (numdev == 0) ){
			SysBeep(1);
			printf("Use CONECT First.\n");
			return -1; 
		}
		if( dev < 0 || dev > numdev-1 ) {
			SysBeep(1);
			printf("Reference to Non-Responding Device\n");
			return -1;
		}
		sprintf(name,"dev%d",devlist[dev]);
		cam = ibfind(name);
		ibonl(cam,1);	
	}
	
	

	switch (code) {
	case FORCE_INIT:
		forceinit = 1;	
	case INIT:		/* init gpib  */
	
	

/*		This was for V1.0 of NI software but will not work for V 1.1 */
/* 		Find the first gpib address 		*/
/*
 *		for (i=1; i<=6; i++) {
 *			sprintf(name,"gpib%d",i);
 *			bd = ibfind(name);
 *			if( bd > 0) break;	
 *		}
 */
		sprintf(name,"gpib0");
		bd = ibfind(name);			/* this will find any board in Version 1.1 */
		if( gpib_time_flag) 
			ibtmo(bd,T100ms);			/* set  board timeout */
		else
			ibtmo(bd,TNONE);
		
		if(bd < 0) {
			SysBeep(1);
			printf("No GPIB board found.\n");
			return -1;
		}
		printf("GPIB Board Found.\n");	
		
		//ibdma(bd,DMAMODE); 			// set dma appropriately -- previously commented out; why?	
		
		numdev = 0;
		dev = 0;
		for(i=1; i<= index; i++) {				/* allow multiple devices */
			sprintf(name,"dev%d",i);
			printf("Looking for %s.\n",name);
			cam=ibfind(name);				/* the name of the CCD device -- use IBCONF */
			if(cam >= 0) {
				printf("GPIB Interface Configured for dev%d\n",i);
				if( gpib_time_flag)
					ibtmo(cam,T100ms);			/* set camera timeout */
			
#ifdef OLDROMS
				ibwrt(cam,"2 xspd",6L);		/* "balls to the wall" speed wise OLD ROMS */
				waitreply();
#endif
			/*	ibrsp(cam,&poll);	*/
				ibwrt(cam,"cisc",4L);
				
				delay(20);	/* wait a bit here, but don't do count on 
							   PHOTOMETRICS-type waits to work */
							   
				ibrsp(cam,&poll);
				
				if(poll < CCDSRQ){
					printf("%d Returned Status.\n",poll);
					if( forceinit ) {
						devlist[numdev] = i;
						printf("CONECT Forced: dev%d\n",i);
					}
				}
				else {
					devlist[numdev] = i;
					printf("dev%d Responded.\n",i);
				}
								
				if (! (poll & ccdok)) { 
					/* Whoever is out there didn't like the CC-200 command */
					/* Maybe it's a STAR_1 -- yeah, try that */
					ibwrt(cam,"!A2",3);
					delay(20);	/* wait a bit here, but don't do count on 
							   PHOTOMETRICS-type waits to work */
					ibwrt(cam,"@A",2);
					delay(20);	/* wait a bit here, but don't do count on 
							   PHOTOMETRICS-type waits to work */
					ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
					cmnd[ibcnt] = 0;
					l=0;
					sscanf(cmnd,"%d",&l);
					if (l!=2) /*(! (poll & ccdok)) */{ 
						/* Is it an oscilloscope? */
						ibwrt(cam,"id?",3); /* Send query for scope id */
						ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
			    		if ((strspn(cmnd,"TEK") > 0) || (strspn(cmnd,"ID TEK") > 0)) {
							detlist[numdev++] = SCOPE;
							printf("TEK SCOPE\n"); 
						}else{
							/* NOT the SCOPE either -- must be PRINCETON INSTRUMENTS */
							ibrsp(cam,&poll);
							printf("poll:%x\n",poll);
							ibwrt(cam,"er",2);	/* clear error light */
							ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
							ibwrt(cam,"RTA",3); 	/* give info using ASCII */
							ibwrt(cam,"CL0",3);		/* set extra cleans to 0 */
							ibwrt(cam,"AD15",4);	/* set AD bits to 15 */
							printf("AD bits set to 15\n",cmnd);
							ibwrt(cam,"as?",3);		/* get array size */
							ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
							cmnd[ibcnt] = 0;	/* put in end of message flag */
							sscanf(cmnd,"%d,%d",&pi_track_max[numdev],&pi_chan_max[numdev]);
							printf("Array Size %d x %d\n",pi_chan_max[numdev],
									pi_track_max[numdev]);
							
							/* If Tek 512x512 "D" chip, have to reconfigure Array Size Def
							    (if it is any other Tek 512x512 chip, all bets are off...*/
							ibwrt(cam,"cdG",3);		/* get array size */
							ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
							cmnd[ibcnt] = 0;	/* put in end of message flag */
							sscanf(cmnd,"%d",&j);
							if ((j-128)==5) {
								ibwrt(cam,"as512,512,1,0,15,0",18);		/* get array size */
							}

							
							detlist[numdev++] = PRINCETON_INSTRUMENTS_1;
							printf("Princeton Instruments Detector.\n");
						}
					} else { 
						printf("Star 1.\n");
						detlist[numdev++] = STAR_1;
						redoMenus(starMenu);
						if(star_treg){ 
							/*omaio(SEND,dummy,"!C1");*/
							ibwrt(cam,"!C1",3);
							printf("Temperature Regulated\n");
							delay(10);
						}else{
							/*omaio(SEND,dummy,"!C0");*/
							ibwrt(cam,"!C0",3);
							printf("Not Tempurature Regulated\n");
							delay(10);
						}
						if(star_gain){ 
							/*omaio(SEND,dummy,"!B1");*/
							ibwrt(cam,"!B1",3);
							delay(10);
							printf("High Gain (x4)\n");
						}else{
							/*omaio(SEND,dummy,"!B0");*/
							ibwrt(cam,"!B0",3);
							printf("Low Gain (x1)\n");
							delay(10);
						}
						if(star_auto) {
							/*omaio(SEND,dummy,"!I1");*/
							ibwrt(cam,"!I1",3);
							printf("Auto on\n");
							delay(10);
						}else{
							/*omaio(SEND,dummy,"!I0");*/
							ibwrt(cam,"!I0",3);
							printf("Auto off\n");
							delay(10);
						}
						sprintf(txt,"!A%d",star_time);	
						/*omaio(SEND,dummy,txt);*/
						for (l = 0; txt[l++] != EOL;){};
						l--;
						ibwrt(cam,txt,l);
						printf("Integration Time: %3.1f sec\n",star_time/10.,l);
						delay(10);
					}
				} else {
					/* Is it really a cc200 ? */
					
					ibwrt(cam,"opt",3);	 /* Send query  */

					delay(60);

					ibrsp(cam,&spr);
					ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */									 
			
					if ((strspn(cmnd,"3") > 0) || (strspn(cmnd,"2") > 0)) {
						detlist[numdev++] = PHOTOMETRICS_CC200;
						printf("CC 200.\n");

						/* Type out answer */
						printf("    ");
						while (ibcnt > 0) {
							cmnd[ibcnt] = 0;	/* put in end of message flag */
							printf("%s ",cmnd);
		   	 				ibrd(cam,(char*)cmnd,CHPERLN);
						}
						printf("\n");  
					}else{				
						/* Is it an oscilloscope? */

						ibwrt(cam,"id?",3); /* Send query for scope id */
						ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
			    		if ((strspn(cmnd,"TEK") > 0) || (strspn(cmnd,"ID TEK") > 0)) {
							detlist[numdev++] = SCOPE;
							printf("TEK SCOPE\n"); 
						}
					}			
				
				}
				
			}
		}
		forceinit = 0;
		if( numdev >= 1) {
			//setccd(1);					/* set detector to ccd type */
			detector = CCD;	
			pixsiz = 1;					/* want small pixels for CCDs */
			printf("%d Devices Responded\n",numdev);
			printf("Synch Status: %d.\n",syncflag);}
		else {
			beep();
			printf("No Devices Responded\n");
			return -1;
		}

		break;


	case BYE:		/* Put the controller back in local */
		ibloc(cam);	
		break;

	case RECEIVE:		/* Transfer data to the Mac from the CC200 */
	
		switch(detlist[dev]) {
		case STAR_1:
			/* Transfer data to the Mac from the CC300 */

			bsize = 18000;
			/*printf("this is star1\n");*/
			omaio(RUN,0);				/* first, reset the parameters */
			/*printf("reset params\n");*/
			k = 0;
			
			if( (header[NDX] != 1) || (header[NDY] != 1) ) {
				/* the Star 1 can't bin pixels; this is done internally */
				i = header[NCHAN];	/* save the old values */
				j = header[NTRAK];
				k = header[NDX];
				l = header[NDY];
				
				header[NCHAN] *= header[NDX];
				header[NTRAK] *= header[NDY];
				header[NDX] = header[NDY] = 1;
				
				if(checkpar()==1) {					/* allocate memory */
					/* Not enough memory for this */
					beep();
					printf(" Memory Needed.\n");
					header[NCHAN] = i;	/* restore the old values */
					header[NTRAK] = j;
					header[NDX] = k;
					header[NDY] = l;
					checkpar();			/* allocate as before */
					return -1;
				}
			}


			ibwrt(cam,":J",2L);			/* Send transmit command */
			/*printf("sent transmit\n");	*/					
			while(!(ibrsp(cam,&poll) & ERR)) {
				if( poll == 0x60) 		/* OK */
					break;
				else if(poll == 0x40)	/* Error */
					printf("error\n");
			}
		
			if( gpib_time_flag)
				ibtmo(cam,T1s);
		
		
			for(j=0; j< HEADERLENGTH; j++)
				*(datpt+j) = ccdheader[j];
 
			j = 0;					
			pointer = (char*)(datpt + HEADERLENGTH);
		
			ibrd(cam,(char*)pointer,bsize);	/* read data; after read, ibcnt has # of bytes read */
			/*printf("begin reading\n");*/
			while ( ibcnt == bsize){
				pointer += bsize; 
				j += ibcnt;
				ibrd(cam,(char*)pointer,bsize);
				
			}
			j += ibcnt;
			i = j;						

			printf("    %d Bytes Received.\n",j);
			user_variables[0].ivalue = j;
			user_variables[0].is_float = 0;

			two_to_four(datpt,j/2,1);
		
			pointer = (char*)(datpt + HEADERLENGTH);	
			for(i=0; i<j; i+=2) {
				ch = *(pointer+i);
				*(pointer+i) = *(pointer+i+1);
				*(pointer+i+1) = ch;
			}
		
			if( gpib_time_flag)
				ibtmo(cam,T100ms);
		
			sprintf(lastname,"Image %d", recimageno++);
			
			
			if( k != 0 ) {
				if(block(k,-l) == 1){
					beep();
					printf("Could Not Block Pixels. Number of Rows and Columns Changed.\n");
					return -1;
				}
			}
			
			break;
		case PHOTOMETRICS_CC200:

			bsize = 18000;

			ibrsp(cam,&poll);			/* Clear service reqests  6-2 */
			ibwrt(cam,"xmt",3L);		/* Send transmit command */
										/* Read data		 */
			/* printf(" xmt sent.\n"); */
			/*  on an FX, we need some time between these commands */
			delay(1);
			
			ibrd(cam,(char*)ccdheader,HEADERLENGTH*2);	/* read header */		
		
			npts  = ccdheader[60] * ccdheader[61];	/* OMA log block */
			header[NCHAN] = ccdheader[60];
			header[NTRAK] = ccdheader[61];
			header[NDX] = ccdheader[62];
			header[NDY] = ccdheader[63];
			header[NX0] = ccdheader[58];
			header[NY0] = ccdheader[59];
	   		trailer[SFACTR] = 1;
		
			if(checkpar() == 1) {
				printf(" %d Channels & %d Tracks Reset to 1.\n",header[NCHAN],header[NTRAK]);
				header[NCHAN] = header[NTRAK] = npts = 1;
				return -1;					/* not enough memory  -- this will leave things unread */
			}			
			for(j=0; j< HEADERLENGTH; j++)
				*(datpt+j) = ccdheader[j];
 
			j = 0;					
			pointer = (char*)(datpt + HEADERLENGTH);
		
			ibrd(cam,(char*)pointer,bsize);	/* read data; after read, ibcnt has # of bytes read */

			while ( ibcnt == bsize){
				pointer += bsize; 
				j += ibcnt;
				ibrd(cam,(char*)pointer,bsize);
				
			}
			j += ibcnt;
			i = j;						

			printf("    %d Bytes Received.",j);
			user_variables[0].ivalue = j;
			user_variables[0].is_float = 0;

		
			/* Ensure it wasn't garbage */
		
			if ( j<bsize && (npts*2)>bsize ) {
				SysBeep(1);
		   		printf("\n    That was garbage.");
			
			/* The garbage could be some little message that the CCD controller wanted to
			   get out. It's possible that there could be several of these. Try to flush
			   short messages and get to the (presumably long) data transfer. */
			
		    	while ( ibcnt<bsize && ibsta>0){	/* if short count and no errors, continue */
		    		pointer = (char*)datpt; 
					j = 0;
					ibrd(cam,(char*)pointer,bsize);		/* read more */
		       		j += ibcnt;
					if(ibsta>0) 
						pointer += ibcnt;
		        	if ( ibcnt<bsize ) 
						printf("\n    %d bytes flushed.",ibcnt); /* another short count */
				}

		    	if (ibsta<0) { 
					SysBeep(1);
					printf("    ERROR, ABORT !\n");
					exflag = macflag = 0;
					break;  
				}
				ibrd(cam,(char*)pointer,bsize);
		    	while (ibcnt == bsize) {
		      	   pointer += bsize;
				   j += ibcnt;
				   ibrd(cam,(char*)pointer,bsize);
				}
		    	j += ibcnt;
		    	i = j;	
		  		printf("\n    %d Bytes Received.",j);
			}
			ibrsp(cam,&poll);		 /* Clear service reqest  6-2 */

			j = *(datpt+60) * *(datpt+61) * 2;

			if (i != j ) 
		  		{ SysBeep(1);
		    	printf("\n    Warning : %d Bytes were expected !",j);
		    	printf("\n    Will FLUSH trash and abort.\n");

		    	exflag = macflag = 0; 
		    	omaio(FLUSH);	/* Flush garbage on queue after data */
		  	}

			printf("\n");
			sprintf(lastname,"Image %d", recimageno++);
			break;
		case PRINCETON_INSTRUMENTS_1:
		
			/* stuff moved to run command */
						
			ibwrt(cam,"SROFF",5); /* Disable service requests...prevents  FIFO overflow */
			ibrsp(cam,&poll);
			if( gpib_time_flag)
				ibtmo(cam,T30s);

			/*________________*/
			
			pointer = (char*)(datpt + HEADERLENGTH);
			bsize = (header[NCHAN]*header[NTRAK]*2)+1;
			
			
			ibwrt(cam,"st",2);
		
			ibrd(cam,(char*)pointer,bsize);	/* read data; after read, ibcnt has # of bytes read */

			printf("    %d Bytes Received.\n",ibcnt);
			user_variables[0].ivalue = ibcnt;
			user_variables[0].is_float = 0;

			two_to_four(pointer,ibcnt/2,1);

			printf("poll:%x status: %x\n",poll,*(pointer+ibcnt-1));
			if( gpib_time_flag)
				ibtmo(cam,T100ms);

			ibwrt(cam,"SRON",4);
			ibrsp(cam,&poll);

			break;
		
		case SCOPE:
			
			bsize = 1028;             /* time of full scope scan is digitized into 
										1024 one byte segments       */	
			header[NCHAN] = 1024;
			header[NTRAK] = 1;
			header[NDX] = 1;
			header[NDY] = 1;
			header[NX0] = 0;
			header[NY0] = 0;
	   		trailer[SFACTR] = 1;

			if(checkpar() == 1) {
				printf(" %d Channels & %d Tracks Reset to 1.\n",header[NCHAN],header[NTRAK]);
				header[NCHAN] = header[NTRAK] = npts = 1;
				return -1;					/* not enough memory  -- this will leave things unread */
			}	
			
			//ibonl(cam,1);		
			
			ibwrt(cam,"path off;data encdg:rpbinary",28L);   /* gets rid of path string
				which would precede data and specifies data format as pos. integers
				from 0-255      */
			
			ibwrt(cam,"curve?",6L);		  /* tells scope that you want to read waveform */
			
			ibrsp(cam,&poll);		 /* Clear service reqest  6-2 */
			
			ibrd(cam,wfm,bsize);          /* read waveform from scope into wfm[] */
			
			
			
			int nread = ibcnt;
			printf("%d bytes received\n",nread);
			
			ibrsp(cam,&poll);		 /* Clear service reqest  6-2 */
			
			user_variables[0].ivalue = nread;
			user_variables[0].is_float = 0;


			checksum = 0;
			for (i = 0; i<nread; i++){
				if((i>0) && (i<nread)){
					checksum += wfm[i];
					checksum = checksum % 256;
				}
				/*if((i<10)||(i>1020)){
					printf("%d    %d\n",i,wfm[i]); 
				}*/
				if((i>2) && (i<nread)){
					*(datpt+i-3+doffset) = wfm[i];
				}
			}
			checksum = 256 - checksum;

			if(checksum != wfm[nread-1]){
				printf("Checksum?\n");
			}
			//ibonl(cam,0);	
			break;				
				
				
				
				
		default:
			break;
		} /* end of detector type switch for RECEIVE case */
		break; /* end of RECEIVE */
		
	case FLUSH:		/* Flush any data from the controller */
		pointer = (char*)datpt;
		for(ibrd(cam,(char*)pointer,bsize); ibcnt > 0; ibrd(cam,(char*)pointer,bsize)){
		    printf("    %d Bytes Received.\n",ibcnt);
			pointer += ibcnt; 
		}
		break;

		
	case RUN:	/* fill the command buffer with a ccdformat command */
		
		switch(detlist[dev]) {
		case STAR_1:
			set_star_param(cam,"!D",header[NX0]);
			set_star_param(cam,"!E",header[NY0]);
			set_star_param(cam,"!F",header[NDX]*header[NCHAN]);
			set_star_param(cam,"!G",header[NDY]*header[NTRAK]);
			omaio(SEND,index,":S");
			break;
		
		case PHOTOMETRICS_CC200:
	
			sprintf(cmnd,"%d %d %d %d %d %d ccdfmt",
		  		header[NX0],header[NY0],header[NCHAN],header[NTRAK],
		  		header[NDX],header[NDY]);
		
			for (i = 0; cmnd[i] != '\0'; i++){};
		
			ibwrt(cam,cmnd,i);				/* send format command */
			waitreply();
			break;
			
		case PRINCETON_INSTRUMENTS_1:

			if ((header[NX0]+header[NDX]*header[NCHAN]>pi_chan_max[dev]) ||
			   (header[NY0]+header[NDY]*header[NTRAK]>pi_track_max[dev])){
				printf("Invalid CCD specification (%d x %d max)\n",pi_chan_max[dev],
						pi_track_max[dev]);
				beep();
				break;
			}
			/* Wait for bit 0 on jumper J5 to go high (default case) */
//			cmnd[2] =  " "; 
//			j5=0;			
//			while(j5!=5){	
//				ibwrt(cam,"RP",2);	 	// Send query  
//				ibrd(cam,(char*)cmnd,CHPERLN);	 // Read reply  
//				cmnd[ibcnt] = 0;	// put in end of message flag 
//				sscanf(&cmnd[2],"%d",&j5);							
//				printf("%d\n",j5);  								
//			}	 													

			
			sprintf(cmnd,"sdb,1,%d,u,0,%d,%d,b,1,%d",
		  			header[NX0],header[NDX]*header[NCHAN],header[NCHAN],
		 			pi_chan_max[dev] - header[NX0] - header[NDX]*header[NCHAN]);

			for (i = 0; cmnd[i] != '\0'; i++){};

			/* send sdb command */

			ibwrt(cam,cmnd,i);				
			ibrsp(cam,&poll);
/*			printf("%s\n",cmnd);*/

			if ((pi_track_max[dev] - header[NY0] - header[NDY]*header[NTRAK])!=0) {
				sprintf(cmnd,"LDb,1,%d,u,0,%d,%d,b,1,%d",
		  				header[NY0],header[NDY]*header[NTRAK],header[NTRAK],
		 	 			pi_track_max[dev] - header[NY0] - header[NDY]*header[NTRAK]);
			}else{
				sprintf(cmnd,"LDb,1,%d,u,0,%d,%d",
		  				header[NY0],header[NDY]*header[NTRAK],header[NTRAK]);
			}
		
			for (i = 0; cmnd[i] != '\0'; i++){};

			/* send ldb command */

			ibwrt(cam,cmnd,i);				
 			ibrsp(cam,&poll);	
/*			printf("%s...\npoll %x\n",cmnd,poll);*/

			break;

		default:
			break;
		}
		break;

	case TRANS:		/* Transmit data To the camera controller */

		switch(detlist[dev]) {
		case PHOTOMETRICS_CC200:	
		
			j = i = ( npts + doffset ) * 2 ;  /* the number of bytes */

			/* Now initiate the transfer */
		
			pointer = (char*)datpt;

			ibwrt(cam,"rcv",3L);  			/* send receive command */
			ibeot(cam,0);					/* disable EOT messages */
			for (ibwrt(cam,pointer,bsize); i >= bsize; ibwrt(cam,pointer,bsize)) {
				pointer += bsize;
				i -= bsize; 
			}

			ibeot(cam,1);					/* For this last one, send an end command */
			ibwrt(cam,pointer,i); 
//			ibrsp(cam,&poll);		 		// Clear service reqest 
			printf("poll: %d\n",poll);
		
			ibwrt(cam," ",1L);				/* dummy write */
	//		ibrsp(cam,&poll); 

			break;
		case STAR_1:
		case PRINCETON_INSTRUMENTS_1:
			printf("Not Used for this Detector.\n");
			beep();
			break;

		default:
			break;
		}
		break;

	case SEND:		/* Send a command to camera controller */
		switch(detlist[dev]) {
		case STAR_1:
		case PHOTOMETRICS_CC200:	
		
			for (i = 0; string[i++] != EOL;){};
			i--;
//			ibrsp(cam,&poll);		 		// Clear service reqests 
//			printf("    poll:%d;\n",poll);

			ibwrt(cam,string,i);		// Send commmand 
		
			if (!syncflag) break;			// If no sync, don't wait for CC200 
		
			waitreply();
		
			if (! (poll & ccdok)) { 
				SysBeep(1);
				printf("    Command Error: %d\n",poll);
		  		break;
			}
			else 
				if ( poll != trq) 
					break;
		    	else { 
					printf("    Controller replied!  Use ASK.\n"); 
			    	omaio(FLUSH);
				}
			break;
			
		case PRINCETON_INSTRUMENTS_1:

			for (i = 0; string[i++] != EOL;){};
			i--;
			
			ibrsp(cam,&poll);
			ibwrt(cam,string,i);		/* Send commmand */
			ibrsp(cam,&poll);
			printf("    poll:%x\n",poll);
			break;

		case SCOPE:
		
			for (i = 0; string[i++] != EOL;){};
			i--;
//			printf("cam = %d comm length = %d  command = %s\n",cam,i,string);
			ibwrt(cam,string,i);	 // Send command 
			break;	
			
			
		default:
			break;
		}
		break;

	case ASK:	// Send command to CC200, wait for reply and type it 
		switch(detlist[dev]) {
		case STAR_1:
		case PHOTOMETRICS_CC200:	
			i = 0;
			for (j = index; cmnd[j++] != EOL; i++){};
//			ibrsp(cam,&poll);		 	// Clear service reqests 

			ibwrt(cam,&cmnd[index],i);	 // Send query  

			waitreply();
		
			if ( !(poll&ccdok)) { 
					printf("    Command Error.\n");
					beep();
			    	break;
			}

			ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
		
			if (ibcnt<=0) { 
				printf("    No reply - none expected.\n");
				break;
			}
							 
			/* Type out answer */
			printf("    ");
			while (ibcnt > 0) {
				cmnd[ibcnt] = 0;	/* put in end of message flag */
				printf("%s ",cmnd);
		   	 ibrd(cam,(char*)cmnd,CHPERLN);
			}
			printf("\n");                                           
			break;
			
		case PRINCETON_INSTRUMENTS_1:

			i = 0;
			for (j = index; cmnd[j++] != EOL; i++){};

			ibrsp(cam,&poll);
			ibwrt(cam,&cmnd[index],i);	 /* Send query  */
			ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
			ibrsp(cam,&poll);

			if (ibcnt<=0) { 
				printf("    No reply - none expected.\n");
				break;
			}
							 
			/* Type out answer */
			printf("    ");
			cmnd[ibcnt] = 0;	/* put in end of message flag */
			printf("%s \n",cmnd);
			break;

	case SCOPE:
		
			i = 0;
			for (j = index; cmnd[j++] != EOL; i++){};

			ibwrt(cam,&cmnd[index],i);	 /* Send query  */

			ibrd(cam,(char*)cmnd,CHPERLN);	 /* Read reply  */
		
			if (ibcnt<=0) { 
				printf("    No reply - none expected.\n");
				break;
			}
							 
			/* Type out answer */
			printf("    ");
			cmnd[ibcnt] = 0;	/* put in end of message flag */
			printf("%s \n",cmnd);
/*			if (scope_rec == 1) {
				fprintf(fp_scope,"%s\n",cmnd);
			}
*/			break;

		default:
			break;
		}	
		
		break;

//	case TTIME:		// Print out time and put it in log 
//
//		lib$date_time(&desc);			// Get time 
//		printf("    ");				// Print it 
//		for (j=0; j!=22; putchar(tstring[j++]));
//		printf("\n");
//					// Time in 1st line of log  
//		for (j=0; j!=22; comment[j]=tstring[j++]);
//		comment[j] = EOL;	// Terminate log entry	    
//		break; 
	}
	ibonl(cam,0);
	setarrow();
	return 0;
}

waitreply()	 /* Await reply */

{
	long 	ticks,t2;								/* for changing watches */
	unsigned char spr;
	int i;
	
	ticks = t2 = TickCount();
	poll = 0;
	
	for(i=0;i<10000;i++){};
	
	while( ticks+60*TIMEOUT > TickCount() ) {
		if( !(ibwait(bd,SRQI | TIMO) & TIMO) ) {
			while( !(ibrsp(cam,&spr) & ERR) ) {
				if(spr == 0x60)
					poll = spr;
				else if(spr == 0x40) {
					poll = spr;
					return 0; }
				
				else if (poll == 0x60)
					return 0;
				if( TickCount() > (t2+10)) {		
					t2 = TickCount();
					
				}
				
			}
			return 0;
		}
		if( TickCount() > (t2+10)) {		
			t2 = TickCount();
			
		}
	}
	return 0;
}

set_star_param(cam,cmd,value)
char cmd[];
short cam,value;

{
	extern char txt[];
	char poll;
	
	sprintf(txt,"%s%d",cmd,value);
	if(ibwrt(cam,txt,strlen(txt)) & ERR)
		return false;
	while( !(ibrsp(cam,&poll) & ERR)) {
		if( poll == 0x60)
			break;
		else if (poll == 0x40)
			return false;
	}
	return true;
}

// end of GPIB conditional
#endif
// end of VISA conditional
#endif

#endif 
// end of TEXACO conditional

	
