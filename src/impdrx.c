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

/* 	 imp routines that have parts that are Mac or display related, 
	 but are not heavily Mac oriented themselves. Also includes 
	 commands that call the camera I/O routines.
*/

#include "impdefs.h"

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

char	   syncflag = 1;	/* flag that, when set, keeps CC200 and
				   				computer in synchronization */
TWOBYTE*	format[NUM_CAMERA_FORMATS] = {0}; 

int display_is_off = 0;

int printf();


/* ********** */

int palette(int n)
			/* set palette number */
{

	extern int thepalette;
	
	thepalette = n;	
	assigncolor1();
	return 0;

}

/* ********** */

int my_erase(int n)
		/* erase all windows (n=0) or nth window */
{

	int i;
	extern int  gwnum;
	
	if(n<0) n=0;
	if( (n !=0) && (n<gwnum)){
		closewindow(n,0);
		return 0;
	}
	i = gwnum;
	for(n=0;n<i;n++)
		closewindow(0,0);	
	setarrow();
	return 0;

}

/* ********** */

int gwindo(int n)
/* get the data from the open window */
{
	int err;
	err = getwindowdata(0);
	return err;
}
/* ********** */

int gimage(int n)
				/* get the data from the off-screen copy of the */
{				/* data in the open window */
	//	getoffscreendata(n);
	return -1;
	
}

/* ********** */

int disoff(int n,int index)
{
	display_is_off = n;
	return 0;
}

/* ********** */

int displa(int n,int index)
#ifdef Mac
//#pragma unused (n)
#endif

{

	//extern short pixsiz;
	//extern DATAWORD *datpt;
	extern TWOBYTE header[];
	short nchan,ntrack;
	
	//int err;
	
	if(display_is_off) return 0;
	
	nchan = header[NCHAN];
	ntrack = header[NTRAK];
	
	if(index) {
		strcpy(lastname,&cmnd[index]);		// save a copy of the name for labeling
						  	   				//		graphics windows 
	}

	
	//err = display(nchan,ntrack,pixsiz,datpt+doffset);
	beep();
	printf("outdated display reference\n");
	return -1;


}
/* ********** */
int setcminmax(int n,int index)		/* get color min and max */
{

	extern DATAWORD cmin,cmax;
	extern char cmnd[];
	extern char autoscale;

	int i;
	DATAWORD j = 1, h;
#ifdef FLOAT
	// Try to get two arguments 
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			i = sscanf(&cmnd[index],"%f %f",&h,&j);
			cmin = h;
			if (i == 2) cmax = j;
			autoscale = 0;
			break;
		} else
			autoscale = 1;
	}

#else
	cmin = n;
	
	// Try to get two arguments 
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d",&h,&j);
			cmin = h;
			cmax = j;
			autoscale = 0;
			break;
		} else
			autoscale = 1;
	}
#endif
	update_status();	// put the new values in the color mapping dialog window, update
				   		// the auto
	return 0;

}
/* ********** */
int setpixsiz(int n)
{
	extern short pixsiz;
	
	pixsiz = n;
	if( pixsiz == 0 ) pixsiz = 1;
	return 0;
}
/* ********** */
/* ********** */

/* save a copy of the current header,trailer, comments, and ccd size in memory */

int save_format(int n)	
{

	TWOBYTE w,h;
	unsigned int size = 516;
	

	if( n>NUM_CAMERA_FORMATS || n<1) {
		beep();
		printf("Allowed format range is 1 - 8.\n");
		return -1;
	}
	n--;
	
	if(format[n] == 0) {
		format[n] = (TWOBYTE*) malloc(size);
		if(format[n] == 0) {
			nomemory();
			format[n] = 0;
			return -1;
		}
	}
	
	printf("%d pointer. %d size\n",format[n],GetPtrSize((Ptr)format[n]));


	BlockMove((Ptr)header,(Ptr)format[n],HEADLEN); 
	BlockMove((Ptr)comment,(Ptr)(format[n]+HEADLEN),COMLEN);
	BlockMove((Ptr)trailer,(Ptr)(format[n]+HEADLEN+COMLEN),TRAILEN);
	w = dlen;
	h = dhi;
	*(format[n]+256) = w;
	*(format[n]+257) = h;
	return 0;

}

/* ********** */

int use_format(int n)
{

	TWOBYTE w,h,wold,hold;
	
	extern char headcopy[];

	if( n>NUM_CAMERA_FORMATS || n<1) {
		beep();
		printf("Allowed format range is 1 - 8.\n");
		return -1;
	}
	n--;
	
	if(format[n] == 0) {
		beep();
		printf("Format %d not currently defined.\n",n+1);
		return -1;
	}
	
	/* Need to save a copy of the current parameters in case the new ones don't work out */
	
	BlockMove((Ptr)header,(Ptr)headcopy,HEADLEN); 
	BlockMove((Ptr)comment,(Ptr)(headcopy+HEADLEN),COMLEN);
	BlockMove((Ptr)trailer,(Ptr)(headcopy+HEADLEN+COMLEN),TRAILEN);
	wold = dlen;
	hold = dhi;

	BlockMove((Ptr)format[n],(Ptr)header,HEADLEN); 
	BlockMove((Ptr)(format[n]+HEADLEN),(Ptr)comment,COMLEN);
	BlockMove((Ptr)(format[n]+HEADLEN+COMLEN),(Ptr)trailer,TRAILEN);
	w = *(format[n]+256);
	h = *(format[n]+257);
	dlen = w;
	dhi = h;

	
	if ( checkpar() == 1) {		/* if no memory, change it back */
		BlockMove((Ptr)headcopy,(Ptr)header,HEADLEN); 
		BlockMove((Ptr)(headcopy+HEADLEN),(Ptr)comment,COMLEN);
		BlockMove((Ptr)(headcopy+HEADLEN+COMLEN),(Ptr)trailer,TRAILEN);
		dlen = wold;
		dhi = hold;
		return -1;
	}


	
	update_status();

	return 0;
}

/* _________________________ routines used by various commands */

int settext(char* name)
{
	short err=0;
	
#ifdef DO_MACH_O
	FSRef MyFSRef;      
	FInfo *finfo_ptr;
	
	//err = HGetVol(NULL,&v_ref_num,&dir_ID);
	
	//CopyCStringToPascal(name,pstring);
	//err = HGetFInfo(v_ref_num,dir_ID,pstring,&finfo);
	//printf("%d %d %d err vol dir\n",err,v_ref_num,dir_ID);            
	//if(err != noErr){
		// must be from a Mach-O path
		err = FSPathMakeRef((unsigned char*)name,&MyFSRef,NULL);
		if(err != noErr){
			beep();
			printf("set text error: %s\n",name);
			return err;
		}
		FSCatalogInfo myInfo;
		err = FSGetCatalogInfo(&MyFSRef,kFSCatInfoFinderInfo,&myInfo,NULL,NULL,NULL);
		finfo_ptr = (FInfo*) &myInfo.finderInfo;
		finfo_ptr->fdType = 'TEXT';
		finfo_ptr->fdCreator = 'OMAm';
		err = FSSetCatalogInfo(&MyFSRef,kFSCatInfoFinderInfo,&myInfo);
		
	//}
	return err;
#endif
}

int setdata(char* name)
{
	
	short err = 0;
#ifdef DO_MACH_O
	FSRef MyFSRef;
	FInfo *finfo_ptr;
	// must be from a Mach-O path
	err = FSPathMakeRef((unsigned char*)name,&MyFSRef,NULL);
	if(err != noErr){
		beep();
		printf("set data error: %s\n",name);
		return err;
	}
	FSCatalogInfo myInfo;
	err = FSGetCatalogInfo(&MyFSRef,kFSCatInfoFinderInfo,&myInfo,NULL,NULL,NULL);
	finfo_ptr = (FInfo*) &myInfo.finderInfo;
	finfo_ptr->fdType = 'ODTA';
	finfo_ptr->fdCreator = 'OMAm';
	err = FSSetCatalogInfo(&MyFSRef,kFSCatInfoFinderInfo,&myInfo);
#endif
	return err;
}


int setpref(char* name)
{
	
	short err = 0;
#ifdef DO_MACH_O
	FInfo *finfo_ptr;
	FSRef MyFSRef;
	// must be from a Mach-O path
	err = FSPathMakeRef((unsigned char*)name,&MyFSRef,NULL);
	if(err != noErr){
		beep();
		printf("set pref error: %s\n",name);
		return err;
	}
	FSCatalogInfo myInfo;
	err = FSGetCatalogInfo(&MyFSRef,kFSCatInfoFinderInfo,&myInfo,NULL,NULL,NULL);
	finfo_ptr = (FInfo*) &myInfo.finderInfo;
	finfo_ptr->fdType = 'OPRF';
	finfo_ptr->fdCreator = 'OMAm';
	err = FSSetCatalogInfo(&MyFSRef,kFSCatInfoFinderInfo,&myInfo);
#endif
	return err;
}
