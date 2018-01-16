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
#include	"impdefs.h"
#include "oma_quartz.h"

#define PMODE 0666 /* RW */
#define READMODE O_RDONLY  /* Read file mode. */

#define CAM_PARMS	18					/* Number of parameters to write * -- was in IODefs.h*/

#define DATA_file 0
#define PREFS_file 1
#define MACRO_file 2
#define PICT_file 3
#define PAL_file 4
#define TEXT_file 5

//#pragma segment MenuCommands 

extern char     cmnd[],txt[];
extern TWOBYTE  header[];
extern char     comment[];
extern TWOBYTE  trailer[];
extern DATAWORD *datpt;
extern int		npts;
extern short	detector;
extern int		doffset;
extern int		detectorspecified;
  
extern char	saveprefixbuf[];		/* save data file prefix buffer */
extern char	savesuffixbuf[];		/* save data file suffix buffer */
extern char	getprefixbuf[];			/* get data file prefix buffer */
extern char	getsuffixbuf[];			/* get data file suffix buffer */
extern char	graphicsprefixbuf[];	/* graphics file prefix buffer */
extern char	graphicssuffixbuf[];	/* graphics file suffix buffer */
extern char	macroprefixbuf[];		/* macro file prefix buffer */
extern char	macrosuffixbuf[];		/* macro file suffix buffer */

/* From Plotone.c */

extern int	pdatminmax;				/* Integrating plot status */
extern int	pstdscrnsize;
extern DATAWORD	ponemax;
extern DATAWORD	ponemin;
extern int 	ponewidth;
extern int 	poneheight;
extern int	poneplotwhite;	
extern int	pintegrate;
extern int	pintx;

/* From Surface.c */

extern	int		sdatminmax;			/* Surface plot status */
extern	int		sstdscrnsize;
extern	int		scolor;
extern	DATAWORD		surfmax;
extern	DATAWORD		surfmin;
extern	int 	surfwidth;
extern	int 	surfheight;
extern	int		plotwhite;	
extern	int		incrementby;
extern	int		persp;	

/* From Plotchans.c */

extern	int		hautoscale;			/* Histogramming status */
extern	int		hstdscrnsize;
extern	DATAWORD		histmax;
extern	DATAWORD		histmin;
extern	int 	histwidth;
extern	int 	histheight;
extern	int		hclear;

/* From Linegraphics.c */

extern	int		lgwidth;			/* Contour plot status */
extern	int		lgheight;
extern	int		nlevls;
extern	DATAWORD		ctrmax;
extern	DATAWORD		ctrmin;
extern	int		datminmax;
extern	int		stdscrnsize;
extern	int		colorctrs;
extern	int		noctrtyping;
extern	int		inwhite;
//extern	int		linegraphicstofile;
extern	int		linedrawing;
extern	float 	clevls[];

/* From imp.c */

extern	DATAWORD	cmin;				/* the color display parameters */
extern	DATAWORD	cmax;
extern	short	pixsiz;				/* block pixel size */
extern	short	newwindowflag;		/* determines whether new window should be opened or just
				  						 display in the old one */
extern	short	detector;			/* the detector type 0 for SIT; 1 for CCD */
extern	int		detectorspecified;	
extern	int	dlen;					/* the max number of channels on the detector */
extern	int	dhi;					/* the max number of tracks on the detector   */
extern	int	c_font;
extern 	int s_font;

/* From Omadialogs.c */

extern	Boolean	autoupdate;
extern	short	cminmaxinc;
extern	short	toolselected;
extern 	int		showselection;
extern 	int		docalcs;
extern Boolean 	autoscale;

extern	int		showruler;
extern	int		plotline;

/* From impio.c */
// used to be extern
short Nu200_par[32];			/* Parameters for the Nu200 */

int star_time;				/* Star 1 Settings */
int star_treg;
int star_auto;
int star_gain;

unsigned long exposure_time;			/* ST-6 Settings */
unsigned short enable_dcs;
unsigned short abg_state;
unsigned short head_offset;
int	temp_control;
int serial_port;
float set_temp;


/* From impdis.c */

//extern RGBColor 	thecolor[];
extern RGBColor 	filecolor[];
extern int			thepalette;

extern OMA_Window	oma_wind[];

//extern WindowPtr 	gwind[];
extern int			gwnum;
extern short		orgx;
extern short		orgy;			/* the starting locations for the first window */
//extern CGrafPort 	screenport;	
extern short		swidth,sheight;

//extern CGrafPtr  	mycgrafptr[];
//extern Rect 		copyRect[];
extern int 			numboxes;
extern Boolean 		loshowbox;
//extern short		windowtype[];
extern int			thereisabox;
extern Point		substart,subend;

//extern GWorldPtr	myGWorldPtrs[]; // offscreen maps
extern Str255  pstring; /* a pascal general purpose text array */
OSErr getfile_dialog( );
OSErr savefile_dialog( );
FSSpec		finalFSSpec;		// file specification returned by new nav routines
FSRef		finalFSRef;			// file reference returned by new nav routines
FSRef		final_parentFSRef;	// directory reference returned by new nav routines
FInfo		final_finfo;		// finder information for file returned by new nav routines

NavReplyRecord		theReply;


#ifdef DO_MACH_O
/*
FSSpec MyFSSpec;
FSRef MyFSRef;
SInt16 new_v_ref_num; 
SInt32 new_dir_ID;
*/

extern char curname[CHPERLN];
		
#endif

int chooseposn(),printf(),read_jpeg(),read_fit(),dcraw_glue(),gtiff();


/* ____________________________ save settings... ____________________________*/

TWOBYTE settings[16];		// store values from various dialogs in this array 

int saveprefs(char* name)
{
	//int i;
	int fd;
	char *comptr;
	extern TWOBYTE headcopy[];
	extern char contents_path[];
  
	short err;
	char oldname[CHPERLN]; 	
	 char txt[CHPERLN];
	//FILE *fp;
	void set_byte_ordering_value();

	set_byte_ordering_value();
#ifdef DO_MACH_O
	getcwd(oldname,CHPERLN);
#endif
	
	if(name == nil) {
			
		err = savefile_dialog();

		if(!err) {
#ifdef DO_MACH_O
            FSRefMakePath(&final_parentFSRef,(unsigned char*)curname,CHPERLN-1);
            chdir(curname);
            //printf("%s\n",curname);
#endif
			fd = creat(txt,PMODE);
			//fp = fopen(txt,"w");
			
		} else {
			SysBeep(1);
			return -1;
		}
	} else {
		if(strcmp(name,SETTINGSFILE) == 0){
			chdir(contents_path);
		}
    	fd = creat(name,PMODE);
		strcpy(txt,name);
		//fp = fopen(name,"w");
	}
	//fprintf(fp,"OMA Settings\nHeader Values\n");
	//for(i=0; i<HEADLEN/2; i++){
	//	fprintf(fp,"%d\n",header[i]);
	//}
		
    write(fd,(char*)&header[0],HEADLEN);
		
	
	//  68000 aranges text differently 
	comptr = (char*) headcopy;
	/*
	for(nt=0; nt < COMLEN; nt += 2) {
		*(comptr+nt+1) = comment[nt];
		*(comptr+nt) = comment[nt+1];
	}
	*/

    if( write(fd,(char*)headcopy,COMLEN)  != COMLEN) {
		file_error();
	}
	//fprintf(fp,"Comment Buffer\n");
	//for(i=0; i<COMLEN; i++){
	//	fprintf(fp,"%s\n",&comment[i]);
	//	i+= strlen(&comment[i]);
	//	if(strlen(&comment[i+1]) == 0) break;		// 2 consecutive EOLs signify the end of the comment buffer
	//}
    write(fd,(char*)&trailer[0],TRAILEN);
	//fprintf(fp,"End of OMA Comments; Start of Trailer\n");
	//for(i=0; i<TRAILEN/2; i++){
	//	fprintf(fp,"%d\n",trailer[i]);
	//}
	
	//fprintf(fp,"Save, Get, Macro, and Settings: Prefix and Suffix\n");
  	//fprintf(fp,"%s\n",saveprefixbuf);		// file prefixes and suffixes 
  	//fprintf(fp,"%s\n",savesuffixbuf);
  	//fprintf(fp,"%s\n",getprefixbuf);		// file prefixes and suffixes 
  	//fprintf(fp,"%s\n",getsuffixbuf);
  	//fprintf(fp,"%s\n",macroprefixbuf);
  	//fprintf(fp,"%s\n",macrosuffixbuf);
  	//fprintf(fp,"%s\n",graphicsprefixbuf);
  	//fprintf(fp,"%s\n",graphicssuffixbuf);
		
	write(fd,(char*)saveprefixbuf,PREFIX_CHPERLN);		// file prefixes and suffixes 
  	write(fd,(char*)savesuffixbuf,PREFIX_CHPERLN);
  	write(fd,(char*)macroprefixbuf,PREFIX_CHPERLN);
  	write(fd,(char*)macrosuffixbuf,PREFIX_CHPERLN);
  	write(fd,(char*)graphicsprefixbuf,PREFIX_CHPERLN);
  	write(fd,(char*)graphicssuffixbuf,PREFIX_CHPERLN);

	
	settings[0] = pdatminmax;				// for integration plots 
	settings[1] = pstdscrnsize;
	settings[2] = ponemax;
	settings[3] = ponemin;
	settings[4] = ponewidth;
	settings[5] = poneheight;
	settings[6] = poneplotwhite;
	settings[7] = pintegrate;
	settings[8] = pintx;

	write(fd,(char*)settings,32);
	//fprintf(fp,"Plot Integrated Settings\n");
	//for(i=0; i < 16; i++){
	//	fprintf(fp,"%d\n",settings[i]);
	//}



	settings[0] = sdatminmax;				// for surface plots 
	settings[1] = sstdscrnsize;
	settings[2] = scolor;
	settings[3] = surfmax;
	settings[4] = surfmin;
	settings[5] = surfwidth;
	settings[6] = surfheight;
	settings[7] = plotwhite;
	settings[8] = incrementby;
	settings[9] = persp;

	write(fd,(char*)settings,32);
	//fprintf(fp,"Surface Plot Settings\n");
	//for(i=0; i < 16; i++){
	//	fprintf(fp,"%d\n",settings[i]);
	//}


	settings[0] = hautoscale;				// for histogram plots 
	settings[1] = hstdscrnsize;
	settings[2] = histmax;
	settings[3] = histmin;
	settings[4] = histwidth;
	settings[5] = histheight;
	settings[6] = hclear;

	write(fd,(char*)settings,32);
	//fprintf(fp,"Historgram Plot Settings\n");
	//for(i=0; i < 16; i++){
	//	fprintf(fp,"%d\n",settings[i]);
	//}
	
	settings[0] = lgwidth;					// for contour plots 
	settings[1] = lgheight;
	settings[2] = nlevls;
	settings[3] = ctrmax;
	settings[4] = ctrmin;
	settings[5] = datminmax;
	settings[6] = stdscrnsize;
	settings[7] = colorctrs;
	settings[8] = noctrtyping;
	settings[9] = inwhite;
//	settings[10] = linegraphicstofile;
//	settings[11] = linedrawing;	 			//don't need to save/restore this 
	
	write(fd,(char*)settings,32);	
	//fprintf(fp,"Contour Plot Settings\n");
	//for(i=0; i < 16; i++){
	//	fprintf(fp,"%d\n",settings[i]);
	//}
	
	write(fd,(char*)clevls,40);
	//for(i=0; i < MAXNOCTRS; i++){
	//	fprintf(fp,"%g\n",clevls[i]);
	//}
	
	settings[0] = pixsiz;					// various things 
	settings[1] = cmin;
	settings[2] = cmax;
	settings[3] = newwindowflag;
	settings[4] = detector;
	settings[5] = cminmaxinc;
	settings[6] = autoupdate;
	settings[7] = toolselected;
	settings[8] = showselection;
	settings[9] = docalcs;
	settings[10] = autoscale;
	settings[11] = dlen;
	settings[12] = dhi;
	settings[13] = c_font;
	settings[14] = s_font;
	settings[15] = showruler;

	
	write(fd,(char*)settings,32);	
	//fprintf(fp,"Various Things\n");
	//for(i=0; i < 16; i++){
	//	fprintf(fp,"%d\n",settings[i]);
	//}


	write(fd,(char*)Nu200_par,64);		// Parameters for the Nu200 -- even though
										//   there may not be one.  Only 18 of these
										//   but leave extra space for future stuff
	//fprintf(fp,"Nu200 Settings\n");
	//for(i=0; i < 32; i++){
	//	fprintf(fp,"%d\n",Nu200_par[i]);
	//}


	settings[0] = star_time;				// Star 1 Settings 
	settings[1] = star_treg;
	settings[2] = star_auto;
	settings[3] = star_gain;

	write(fd,(char*)settings,32);	
	//fprintf(fp,"Star 1 Settings\n");
	//for(i=0; i < 16; i++){
	//	fprintf(fp,"%d\n",settings[i]);
	//}

	settings[0] = plotline;					// more various things 
	settings[1] = enable_dcs;				// ST-6 settings 
	settings[2] = abg_state;
	settings[3] = head_offset;
	settings[4] = temp_control;
	settings[5] = serial_port;
	
	write(fd,(char*)settings,32);	
	//fprintf(fp,"ST-6 Settings\n");
	//for(i=0; i < 16; i++){
	//	fprintf(fp,"%d\n",settings[i]);
	//}

	write(fd,(char*)&exposure_time,4);	
	write(fd,(char*)&set_temp,4);
	//fprintf(fp,"%d\n%f\n",exposure_time,set_temp);
	
	if( write(fd,(char*)getprefixbuf,PREFIX_CHPERLN) != PREFIX_CHPERLN)
		printf("Write error.\n");		// file prefixes and suffixes for get data commands
	if( write(fd,(char*)getsuffixbuf,PREFIX_CHPERLN) != PREFIX_CHPERLN)
		printf("Write error.\n");
	
	//fclose(fp);
	close(fd);
	setpref(txt);					// specify the creator and file type 
#ifdef DO_MACH_O
	chdir(oldname);
#endif	
	return 0;
}
/*
	
    close(fd);
	
	setpref(txt);					// specify the creator and file type 
	
	//err = setvol("", oldvol);
	//HSetVol(NULL,v_ref_num,dir_ID);
#ifdef DO_MACH_O
	chdir(oldname);
#endif	
	return 0;
	
 }
*/
 
 void swap_bytes_routine(char* co, int num,int nb)
 {
	int nr;
	char ch;
	if(nb == 2){
		for(nr=0; nr < num; nr += nb) {
			ch = co[nr+1];
			co[nr+1] = co[nr];
			co[nr] = ch;
		}
	} else if (nb == 4){
		for(nr=0; nr < num; nr += nb) {
			ch = co[nr+3];
			co[nr+3] = co[nr];
			co[nr] = ch;
			ch = co[nr+2];
			co[nr+2] = co[nr+1];
			co[nr+1] = ch;
		}
	} else if (nb == 8){
		for(nr=0; nr < num; nr += nb) {
			ch = co[nr+7];
			co[nr+7] = co[nr];
			co[nr] = ch;
			
			ch = co[nr+6];
			co[nr+6] = co[nr+1];
			co[nr+1] = ch;
			
			ch = co[nr+5];
			co[nr+5] = co[nr+2];
			co[nr+2] = ch;
			
			ch = co[nr+4];
			co[nr+4] = co[nr+3];
			co[nr+3] = ch;
		}
	}
	
}

unsigned long fsize(char* file)
{
    FILE * f = fopen(file, "r");
    if (f == NULL) {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
    fclose(f);
    return len;
}

/* ____________________________ load settings... ____________________________*/
// actual old length is 1872; new oma length is 4944; new oma2 length is 4232, but could change
#define OLD_SETTINGS_LENGTH 2000
// old oma settings were running into trouble with the length of prefixes; new length is 512
#define OLD_PREFIX_CHPERLN	128

int loadprefs(char* name)
{
	
	char oldname[CHPERLN];
	short  err;
	
	int fd,nr,nbyte,oldfont;
	char ch;
	int do_swap;
	//FILE* fp;
	TWOBYTE *scpt,tmp_2byte;
	int prefixLength = OLD_PREFIX_CHPERLN;
	
	extern WindowRef Status_window;
	extern char txt[];
	extern char unit_text[];
	extern int ruler_scale_defined;
	extern float ruler_scale;
	extern char contents_path[];
	
	int get_byte_swap_value(short);
	
#ifdef DO_MACH_O
	getcwd(oldname,CHPERLN);
#endif
	if(name == nil) {
   		
		err = getfile_dialog(  PREFS_file);
		if(err) return -1;
#ifdef DO_MACH_O
		FSRefMakePath(&final_parentFSRef,(unsigned char*)curname,255);
		chdir(curname);
		//printf("%s\n",curname);
#endif
	} else {
		if(strcmp(name,SETTINGSFILE) == 0){
			chdir(contents_path);
		}
		strcpy(txt,name);
	}
	/*
	 fp = fopen(txt,"r");
	 if(fp == NULL){
	 beep();
	 return -1;
	 }
	 
	 */
	unsigned long len = fsize(txt);
    if(len > OLD_SETTINGS_LENGTH) prefixLength = PREFIX_CHPERLN;

	
    fd = open(txt,O_RDONLY);
	
    if(fd == -1) {
		beep();
		return -1;
	}
	
	oldfont = c_font;
	
    read(fd,(char*)header,HEADLEN);
	
    read(fd,(char*)comment,COMLEN);
	
	//  68000 aranges text differently
	for(nr=0; nr < COMLEN; nr += 2) {
		ch = comment[nr+1];
		comment[nr+1] = comment[nr];
		comment[nr] = ch;
	}
	
    read(fd,(char*)trailer,TRAILEN);
	do_swap = get_byte_swap_value(trailer[IDWRDS]);
	if(do_swap) {
		swap_bytes_routine((char*)trailer,TRAILEN,2);
		swap_bytes_routine((char*)header,HEADLEN,2);
	}
	
	if(trailer[RULER_CODE] == MAGIC_NUMBER) {	/* If there was a ruler defined */
		ruler_scale_defined = 1;
		
		scpt = (TWOBYTE*) &ruler_scale;
		if(do_swap) {
			*(scpt+1) = trailer[RULER_SCALE];
			*(scpt) = trailer[RULER_SCALE+1];
			// need to change the order of values in the trailer as well
			tmp_2byte = trailer[RULER_SCALE];
			trailer[RULER_SCALE] = trailer[RULER_SCALE+1];
			trailer[RULER_SCALE+1] = tmp_2byte;
		} else {
			*(scpt) = trailer[RULER_SCALE];
			*(scpt+1) = trailer[RULER_SCALE+1];
		}
		
		strcpy(unit_text,(char*) &trailer[RULER_UNITS]);
		if( unit_text[0] )
			printf("%f Pixels per %s.\n",ruler_scale,unit_text);
		else
			printf("%f Pixels per Unit.\n",ruler_scale);
	} else {
		ruler_scale_defined = 0;
	}
	
	nbyte = (header[NCHAN]*header[NTRAK]) * DATABYTES;
	
	if( detectorspecified == 0) {
		if(nbyte > 110*110*2) { // assume that big pics are CCD, small ones from a SIT
			detector = CCD;
			doffset = 80;}
		else {
			detector = SIT;
			doffset = 0;
		}
	}
	nbyte += doffset*DATABYTES;
	nbyte = (nbyte+511)/512*512;
	
	if(nbyte == 0 || checkpar()==1) {
		beep();
		printf(" Problem in Default Settings!\n");
		header[NCHAN] = header[NTRAK] = 1;
	}
	
	npts = header[NCHAN] * header[NTRAK];
	
	
  	read(fd,(char*)saveprefixbuf,prefixLength);		// file prefixes and suffixes
  	read(fd,(char*)savesuffixbuf,prefixLength);
  	read(fd,(char*)macroprefixbuf,prefixLength);
  	read(fd,(char*)macrosuffixbuf,prefixLength);
  	read(fd,(char*)graphicsprefixbuf,prefixLength);
  	read(fd,(char*)graphicssuffixbuf,prefixLength);
	
	read(fd,(char*)settings,32);
	if(do_swap) swap_bytes_routine((char*)settings,32,2);
	
	pdatminmax = settings[0];					// for integration plots
	pstdscrnsize = settings[1];
	ponemax = settings[2];
	ponemin = settings[3];
	ponewidth = settings[4];
	poneheight = settings[5];
	poneplotwhite = settings[6];
	pintegrate = settings[7];
	pintx = settings[8];
	
	read(fd,(char*)settings,32);
	if(do_swap) swap_bytes_routine((char*)settings,32,2);
	
	sdatminmax = settings[0];				// for surface plots
	sstdscrnsize = settings[1];
	scolor = settings[2];
	surfmax = settings[3];
	surfmin = settings[4];
	surfwidth = settings[5];
	surfheight = settings[6];
	plotwhite = settings[7];
	incrementby = settings[8];
	persp = settings[9];
	
	read(fd,(char*)settings,32);
	if(do_swap) swap_bytes_routine((char*)settings,32,2);
	
	hautoscale = settings[0];				// for histogram plots
	hstdscrnsize = settings[1];
	histmax = settings[2];
	histmin = settings[3];
	histwidth = settings[4];
	histheight = settings[5];
	hclear = settings[6];
	
	read(fd,(char*)settings,32);
	if(do_swap) swap_bytes_routine((char*)settings,32,2);
	
	lgwidth = settings[0];					// for coutour plots
	lgheight = settings[1];
	nlevls = settings[2];
	ctrmax = settings[3];
	ctrmin = settings[4];
	datminmax = settings[5];
	stdscrnsize = settings[6];
	colorctrs = settings[7];
	noctrtyping = settings[8];
	inwhite = settings[9];
	//	linegraphicstofile = settings[10];
	//	linedrawing = settings[11];				// don't need to save/restore this
	
	read(fd,(char*)clevls,40);
	if(do_swap) swap_bytes_routine((char*)clevls,40,4);
	
	read(fd,(char*)settings,32);
	if(do_swap) swap_bytes_routine((char*)settings,32,2);
	
	pixsiz = settings[0];					// various things
	cmin = settings[1];
	cmax = settings[2];
	newwindowflag = settings[3];
	detector = settings[4];
	cminmaxinc = settings[5];
	autoupdate = settings[6];
	toolselected = settings[7];
	showselection = settings[8];
	docalcs = settings[9];
	autoscale = settings[10];
	dlen =	settings[11];
	dhi = settings[12];
	c_font = settings[13];
	s_font = settings[14];
	showruler = settings[15];
	
	
	if( detector != 0) detectorspecified = 1;	// If saved detector type is CCD, no
	//	automatic type switching based on
	//	image size will be done; if saved
	//	type is SIT, type switching will be
	//	enabled.
	
	
	
	read(fd,(char*)Nu200_par,CAM_PARMS*2);		// Parameters for the Nu200
	read(fd,(char*)settings,64-CAM_PARMS*2);	// Get rid of this extra
	if(do_swap) swap_bytes_routine((char*)Nu200_par,CAM_PARMS*2,2);
	
	read(fd,(char*)settings,32);
	if(do_swap) swap_bytes_routine((char*)settings,32,2);
	
	
	star_time = settings[0];					// Star 1 Settings
	star_treg = settings[1];
	star_auto = settings[2];
	star_gain = settings[3];
	
	
	read(fd,(char*)settings,32);
	if(do_swap) swap_bytes_routine((char*)settings,32,2);
	
	plotline = settings[0];					// moe various things
	
	enable_dcs = settings[1];				// ST-6 settings
	abg_state = settings[2];
	head_offset = settings[3];
	temp_control = settings[4];
	serial_port = settings[5];
	
	read(fd,(char*)&exposure_time,4);
	read(fd,(char*)&set_temp,4);
	
	
  	read(fd,(char*)getprefixbuf,prefixLength);		// file prefixes and suffixes for get data commands
  	read(fd,(char*)getsuffixbuf,prefixLength);
	
    close(fd);
	//err = setvol("", oldvol);
	//HSetVol(NULL,v_ref_num,dir_ID);
#ifdef DO_MACH_O
	chdir(oldname);
#endif
	//setfonts(oldfont,-1);		// removed
	
	if(Status_window != 0){
		SetPortWindowPort(Status_window);
		
		//setup_status_fonts();
		//printcmin_cmax();
		update_status();
	}
	return 0;
	
}

/* ____________________________ Import Non-OMA files ... ____________________________*/
int import_file()
{
	short err;
	
 	//char oldname[256]; 	
    //SInt16 v_ref_num; 
    //SInt32 dir_ID;	
	int i=0, have_file = 0,num_col;
	//FSRef MyFSRef;

	extern int have_max;
	extern char	   lastname[],cmnd[];
	extern float r_scale,g_scale,b_scale;
	
	//HGetVol( (unsigned char*) oldname,&v_ref_num,&dir_ID);
	
	err = getfile_dialog(PICT_file);
	if(err != noErr){
		return(err);
	}
	
	//FSpMakeFSRef(&finalFSSpec,&MyFSRef);		// now have FSRef
	FSRefMakePath(&finalFSRef,(unsigned char*)txt,255);

	//printf("file: %s\n",txt);
	
	i = strlen(txt)-4;
	strcpy(lastname,txt);
	if( memcmp(&txt[i],".jpg",4) == 0 || memcmp(&txt[i],".JPG",4) == 0 ){   // open jpeg files
		read_jpeg(txt,-1);	
		have_file = 1;	
	} else if(memcmp(&txt[i],".nef",4) == 0 || memcmp(&txt[i],".NEF",4) == 0 ||
			  memcmp(&txt[i],".raw",4) == 0 || memcmp(&txt[i],".RAW",4) == 0 ||
			  memcmp(&txt[i],".rw2",4) == 0 || memcmp(&txt[i],".RW2",4) == 0 ||
			  memcmp(&txt[i],".dng",4) == 0 || memcmp(&txt[i],".DNG",4) == 0){   // open nikon raw files or dng files
		num_col = dcraw_glue(txt,-1);
		have_max = 0;
		maxx();
		if(num_col == 3)
			r_scale=g_scale=b_scale= 1.0;
		update_status();
		have_file = 1;
	} else if( memcmp(&txt[i],".fts",4) == 0 || memcmp(&txt[i],".FTS",4) == 0 ||
			   memcmp(&txt[i],".fit",4) == 0 || memcmp(&txt[i],".FIT",4) == 0){   // open fits files
		read_fit(txt);
		have_file = 1;
	} else if( memcmp(&txt[i],".tif",4) == 0 || memcmp(&txt[i],".TIF",4) == 0 ||
			   memcmp(&txt[i-1],".tiff",5) == 0 || memcmp(&txt[i-1],".TIFF",5) == 0){   // open tif files
		
		printf("Get tiff...\n");
		strcpy(cmnd,txt);
		gtiff(0,0);
		have_file = 1;
	}

	if(have_file == 0) {
		getpictfile(-1);
		err = getwindowdata(0);
	}
	printf("OMA>");

	//HSetVol(NULL,v_ref_num,dir_ID);
	return err;
}

int getpic(int n, int index)
{

	return getpictfile(&cmnd[index]);
 
}

int getpictfile(char* name) /* Get grayview (Type 'gray') PICT resource files. */				
							/* Also get MacDraw PICT data fork files ('Type 'PICT'). */
							/* Automatically loads pixelpaint 'COLR' resource from */
{							/*   MacDraw PICT files */
/*
  short current, ref,resref;
  Handle thepalhand;
  Handle thepict;
  PicPtr	pictptr;
  short sw,sh;
  //SFReply reply;

  //SFTypeList type;
  Rect lrect;
  long filesize;
  FSCatalogInfo cat_info;

	short err;
 	//char oldname[256]; 	
    //SInt16 v_ref_num; 
    //SInt32 dir_ID;	
	extern char txt[];
	
	//HGetVol( (unsigned char*) oldname,&v_ref_num,&dir_ID);
	
   current = CurResFile();
   
   if(name == nil) {
	  err = getfile_dialog(PICT_file);
	  if(err != noErr){
		return(err);
	  }
	} 
	else if(name !=(Ptr)-1) {
	
		err = FSPathMakeRef((unsigned char*)getprefixbuf,&MyFSRef,NULL);
		err = FSGetCatalogInfo(&MyFSRef,kFSCatInfoNodeID,&cat_info,NULL,&finalFSSpec,NULL);
		//printf("%d %d\n", err,cat_info.nodeID );		
		//printf("%d %d\n",finalFSSpec.vRefNum,finalFSSpec.parID);
		CopyCStringToPascal(name,pstring);		
		FSMakeFSSpec(finalFSSpec.vRefNum,cat_info.nodeID,pstring,&finalFSSpec);
		HSetVol(NULL,finalFSSpec.vRefNum,finalFSSpec.parID);
	}

		err = FSpOpenDF( &finalFSSpec, fsRdPerm, &ref);
		if (err < 0) {
			printf("%d File Error Reading %s!\nOMA>",err,name);
			return -1; }
		err = SetFPos(ref,fsFromStart,PICTHEADERSIZE);	// skip the whole header
		err = GetEOF(ref,&filesize);
		filesize -= PICTHEADERSIZE;		// there is that header there 

		thepict = (Handle)NewHandle(filesize);	// room for the PICT info 

		err = FSRead(ref,&filesize,*thepict);	// read it in 
		err = FSClose(ref);
		
//		Try to load up a 'COLR' resource 

		
		resref = FSpOpenResFile( &finalFSSpec, fsRdPerm);
		if(resref != -1) {
  			UseResFile(resref);
  			thepalhand = GetResource('COLR', 999);	// a pixelpaint resource 
			if(thepalhand != nil) {
				filesize = GetHandleSize(thepalhand);
				// the first 2 bytes tell how many entries there are 
				if( (filesize-2)/6 <= 256) {
					BlockMove( (Ptr) ((*thepalhand)+2+6), (Ptr) filecolor,filesize-2-6);	// skip the first color 
					thepalette = FROMAFILE;
				}
				printf("New Palette Loaded from 'COLR' Resource.\nOMA>");
			}
  			ReleaseResource(thepalhand);
  			CloseResFile(resref);
			
		}
		

//	The end of 'COLR' resource stuff 


	assigncolor1();
	
  	pictptr = (PicPtr) *thepict;
  	lrect =  (pictptr->picFrame);
	
	//printf("%d %d %d %d r l b t\n", lrect.right,lrect.left,lrect.bottom,lrect.top); 

  	sw = lrect.right - lrect.left;
  	sh = lrect.bottom - lrect.top;
	
	lrect.right -= lrect.left;
	lrect.bottom -= lrect.top;
	lrect.top = lrect.left = 0;
	
	chooseposn(sw,sh,1);
	CopyPascalStringToC(finalFSSpec.name,txt);
  	if(!openwindow(sw,sh,txt,documentProc)) {
		DisposeHandle( (Handle) thepict);
  		UseResFile(current);
  		//err = setvol("", oldvol);
  		SetPortWindowPort( oma_wind[gwnum].gwind);
		return -1;
	}
	//SetPort(myGWorldPtrs[gwnum]);
  	DrawPicture((PicHandle)thepict, &lrect);
  	ReleaseResource(thepict);
	DisposeHandle( (Handle) thepict);
  	CloseResFile(ref);
  	UseResFile(current);
  	//err = setvol("", oldvol);
  	//HSetVol(NULL,v_ref_num,dir_ID);
  	SetPortWindowPort( oma_wind[gwnum].gwind);
  	gwnum++;
	orgx += swidth;
	//checkevents();
*/
	return 0;
}

/* ____________________________ load palette... ____________________________*/
  
int getpalettefile(char* name) 
{

  //char oldname[256];
  short  current, ref, err;
  Handle thepalhand;
  FInfo reply;
  
  //SFTypeList type;
  long	size;
  unsigned short *clutptr,i,index,from_cbt=0;
  short defpal=0;
  
  int fd;
 	
    //SInt16 v_ref_num; 
    //SInt32 dir_ID;	
	
	unsigned char colors[256];
	
	//HGetVol( (unsigned char*) oldname,&v_ref_num,&dir_ID);
	


   current = CurResFile();
   
   if(name == nil) {
	  err = getfile_dialog(PAL_file);
	} 
	else {
#ifdef DO_MACH_O
		/*
		// reading this file (see putfile() for writing case)
		err = FSPathMakeRef("oma.app/Contents/Resources/",&MyFSRef,NULL);
		//printf("%d ", err );
		err = FSGetCatalogInfo(&MyFSRef,kFSCatInfoNodeID,&cat_info,NULL,&finalFSSpec,NULL);
		//printf("%d \n", err );
		CopyCStringToPascal(name,pstring);		
		err = FSMakeFSSpec(finalFSSpec.vRefNum,cat_info.nodeID,pstring,&finalFSSpec); //cat_info.nodeID
		//printf("%d %s\n", err,name );
		HSetVol(NULL,finalFSSpec.vRefNum,finalFSSpec.parID);
		*/
		fd = open(name,O_RDONLY);
		if(fd == -1) {
			beep();
			return -1;
		}
		read(fd,colors,256);
		for(i=0; i<256; i++)
			filecolor[i].red = colors[i]<<8;
		read(fd,colors,256);
		for(i=0; i<256; i++)
			filecolor[i].green = colors[i]<<8;
		read(fd,colors,256);
		for(i=0; i<256; i++)
			filecolor[i].blue = colors[i]<<8;
		thepalette = FROMAFILE;
		//printf("%d  handles\n",mycthand );
		assigncolor1();
		return 0;
	
		
		
#else
		
		finalFSSpec.vRefNum =  v_ref_num;
    	finalFSSpec.parID = dir_ID; 
   		CopyCStringToPascal(name,finalFSSpec.name);
#endif		
		
	}

	//err = FSpOpenDF( &finalFSSpec, fsRdPerm, &ref);
	
	ref = FSpOpenResFile( &finalFSSpec, fsRdPerm);
		  FSpGetFInfo ( &finalFSSpec, &reply);
		
 	//err = fsopen(fnam, vol,&ref);		// get a MacDraw PICT Data Fork File 
	if (ref < 0) {
		printf("%d File Error Reading %s!\nOMA>",ref,name);
		return -1; 
	}


   	UseResFile(ref);
   	
	if(reply.fdType == 'drwC' || reply.fdType == 'PA1b' ||defpal == 1) {			// get a Canvas clut Resource 
		thepalhand = GetResource('clut', 999);										// a pixelpaint resource 
		if( thepalhand == nil){
			thepalhand = GetResource('clut', 400);				// CBT clut
			from_cbt = 1;
		}
		if( thepalhand == nil) {
			printf("File Does Not Contain a clut Resource.\nOMA>");
			CloseResFile(ref);
  			UseResFile(current);
  			//err = setvol("", oldvol);
  			//HSetVol(NULL,v_ref_num,dir_ID);
			return -1;
		}
		// handle the clut resource 
		size = GetHandleSize(thepalhand);
		clutptr = (unsigned short*) *thepalhand;
		//printf("%d %d size, ncolors\n",size,clutptr[3]);
		index = -1;
		for(i=4; i/4 <= clutptr[3]; i+=4) {
			// thecolor[clutptr[i]-1].red = clutptr[i+1]; // this was evidently wrong 
			if(from_cbt)
				index++;
			else 
				index = clutptr[i];
			filecolor[index].red = clutptr[i+1];
			filecolor[index].green = clutptr[i+2];
			filecolor[index].blue = clutptr[i+3];
			//printf("%d %d %d %d\n",clutptr[i],clutptr[i+1],clutptr[i+2],clutptr[i+3]);
		}
		thepalette = FROMAFILE;
		//printf("%d  handles\n",mycthand );
		assigncolor1();
	  	ReleaseResource(thepalhand);
	  	CloseResFile(ref);
	  	UseResFile(current);
	  	//err = setvol("", oldvol);
	  	//HSetVol(NULL,v_ref_num,dir_ID);
		return 0;
		
	}
	
	thepalhand = GetResource('COLR', 999);	// a pixelpaint resource 
	if( thepalhand == nil) {
		printf("File Does Not Contain a Recognized Resource.\nOMA>");
		CloseResFile(ref);
  		UseResFile(current);
  		//err = setvol("", oldvol);
  		//HSetVol(NULL,v_ref_num,dir_ID);
		return -1;
	}

	// handle the COLR resource 	
	size = GetHandleSize(thepalhand);
	
	// the first 2 bytes tell how many entries there are 
	// printf("got a COLR resource\n"); 
	
	if(/* **thepalhand > 255 ||*/ (size-2)/6 > 256) 
		printf("  Palette File too Big!\nOMA>");
	else {
		BlockMove( (Ptr) ((*thepalhand)+2+6), (Ptr) filecolor ,size-2-6);	// skip the first color 
		thepalette = FROMAFILE;
		assigncolor1();
	}
  	ReleaseResource(thepalhand);
  	CloseResFile(ref);
  	UseResFile(current);
  	//err = setvol("", oldvol);
  	//HSetVol(NULL,v_ref_num,dir_ID);
	return 0;
}

/* _________________________ save window and screen... __________________________*/

#define MBAR_HEIGHT 20

int savpdf(int n, int index)
{
	WindowPtr theActiveWindow;
	int gwind_no;
	CGRect pageRect;
	char oldname[CHPERLN]; 
	OSErr err;
	char    *fullname();
	OSErr savefile_dialog( );
	extern	WindowPtr	Ctwind;
	extern WindowRef Status_window;
	extern char    cmnd[];
	extern char txt[];
	extern	FSRef finalFSRef;
	void MyCreatePDFFile (CGRect pageRect, const char *filename, int gwnum);


	theActiveWindow = FrontWindow();
	gwind_no = activegwnum(theActiveWindow);
	
	if(theActiveWindow == Ctwind || theActiveWindow == Status_window ){		//  || oma_wind[gwind_no].windowtype == QROWCOL
		SysBeep(1);
		return -1;	
	}
	
	pageRect = CGRectMake(0, 0, oma_wind[gwind_no].width, oma_wind[gwind_no].height);

	if(index == 0) {	
		getcwd(oldname,CHPERLN);
		err = savefile_dialog();
		if(!err) {
			FSRefMakePath(&final_parentFSRef,(unsigned char*)curname,255);
            chdir(curname);
			MyCreatePDFFile ( pageRect, txt, gwind_no);
			chdir(oldname);
			return 0;
		} else {
			return err;
		}
	
	} else {
		strcpy( txt, fullname(&cmnd[index],PDF_DATA));
		MyCreatePDFFile ( pageRect, txt, gwind_no);
	}

	return 0;
}



/* _______________________ save selected rectangle... _______________________*/

int saverectangle(char* name)
{

  int fd,nt,nc,size,error=0,image_part;
  char ch;
  DATAWORD *datp2,*datp;

  extern TWOBYTE  header[],headcopy[];
  extern char     comment[];
  extern TWOBYTE  trailer[];
  extern DATAWORD *datpt;
  extern short		detector;
  extern int		doffset;
  extern int		save_rgb_rectangle;
  
	short err;
 	char oldname[CHPERLN]; 	
	extern char txt[];
	
	Point start,end;
	
	// remove restriction on the way a rectangle is defined
	// previously, the assumption was that all rectangles were defined from the upper left to lower right
	start = substart;
	end = subend;
	if(subend.h < substart.h){
		end.h = substart.h;
		start.h = subend.h;
	}
	if(subend.v < substart.v){
		end.v = substart.v;
		start.v = subend.v;
	}
	
	// check to be sure the rectangle make sense for the current data
	if (end.h >= header[NCHAN] || end.v >= header[NTRAK] 
		|| end.h-start.h >= header[NCHAN] || end.v-start.v >= header[NTRAK] ){
		beep();
		printf("Can't save rectangle -- rectangle size problem.\n");
		return -1;
	}
		
	
	
	for(nt=0; nt<HEADLEN/2 ; nt++){
		headcopy[nt] = header[nt];	
	}
	headcopy[NCHAN] = end.h - start.h +1;
	headcopy[NTRAK] = end.v - start.v +1;
	headcopy[NX0] += start.h*header[NDX];
	headcopy[NY0] += start.v*header[NDY];
	
	headcopy[NMAX] = headcopy[LMAX] = headcopy[NMIN] = headcopy[LMIN] = 0;
	
	if(save_rgb_rectangle){
		if( headcopy[NTRAK]*3 > header[NTRAK] ){
			beep();
			printf("Can't save rectangle as RGB image -- rectangle size problem.\n");
			save_rgb_rectangle = 0;
			return -1;
		} else {
			headcopy[NTRAK] *= 3;
		}
	}

	/* allow menu command to use the currently defined box
	if((!thereisabox) && (name == nil)) {
		SysBeep(1);							// return if there is no box and 
		return -1;							//   the call was not from the keyboard -- 
	}										//  If there is a typed command, use the 
											//   current box defined in the coord window 
	*/
	
	if(thereisabox)
		erasebox(); 
    
	if(name == nil) {
#ifdef DO_MACH_O
        getcwd(oldname,CHPERLN);
#endif
		err = savefile_dialog();

		if(!err) {
#ifdef DO_MACH_O
            FSRefMakePath(&final_parentFSRef,(unsigned char*)curname,255);
            chdir(curname);
            //printf("%s\n",curname);
#endif
			fd = creat(txt,PMODE);
			
		} else {
			SysBeep(1);
			return -1;
		}
		
		
	} else {
	
		fd = creat(name,PMODE);
		strcpy(txt,name);
	}
	
	if(fd == -1) {
		beep();
		return -1;
	}

		
    write(fd,(char*)&headcopy[0],HEADLEN);
	
	//  68000 aranges text differently 
	for(nt=0; nt < COMLEN; nt += 2) {
		ch = comment[nt+1];
		comment[nt+1] = comment[nt];
		comment[nt] = ch;
	}

    write(fd,(char*)comment,COMLEN);
    write(fd,(char*)&trailer[0],TRAILEN);

	size = (headcopy[NCHAN] * headcopy[NTRAK] + doffset) * DATABYTES;

	datp2 = datp = (DATAWORD*) malloc(size);
	if(datp2 == 0) {
		nomemory();
		return -1;
	}
	for(nt=0; nt<doffset; nt++) 
		*(datp2++) = *(datpt+nt);	// copy the CCD header 
	
	for (image_part = 0; image_part < (2 * save_rgb_rectangle)+1 ; image_part++){
		for(nt=start.v+image_part*header[NTRAK]/3; nt<=end.v+image_part*header[NTRAK]/3; nt++) {
			for(nc=start.h; nc<=end.h;nc++){
				if(detector == SIT) {
					#if DATABYTES == 2	
					*(datp2++) = ~(idat(nt,nc));
					#endif
				} else {
					*(datp2++) = idat(nt,nc);	
				}
			}
		}
	}
    //write(fd,(char*)datp, size);		// this writes the data twice -- duh!
	if( write(fd,(char*)datp, size) != size) {
		file_error();
		error = -1;
	}

	free(datp);
    close(fd);
    setdata(txt);					// specify the creator and file type 
	fileflush(txt);
	//err = setvol("", oldvol);
	//HSetVol(NULL,v_ref_num,dir_ID);
#ifdef DO_MACH_O
	chdir(oldname);
#endif
	return error;

 }

 
short scrapbookref,sbresnumber,scrapbookopen;


/* ____________________________ open data... ____________________________*/
  

int getdatafile()
{

	short err;
 	char oldname[CHPERLN]; 	
    //SInt16 v_ref_num; 
    //SInt32 dir_ID;	
	
	

	//HGetVol( (unsigned char*) oldname,&v_ref_num,&dir_ID);
                
#ifdef DO_MACH_O
	getcwd(oldname,CHPERLN);
#endif	
	err = getfile_dialog(DATA_file );
	
#ifdef DO_MACH_O
	//HGetVol( (unsigned char*) curname,&new_v_ref_num,&new_dir_ID);
	//FSMakeFSSpec(new_v_ref_num,new_dir_ID,"",&MyFSSpec);
	////FSpMakeFSRef(&MyFSSpec,&MyFSRef);		// now have FSRef
	//FSRefMakePath(&MyFSRef,(unsigned char*)curname,255);
	//chdir(curname); 
	//printf("%s\n",curname);
#endif
	if(!err){
#ifdef DO_MACH_O
		//HGetVol( (unsigned char*) curname,&new_v_ref_num,&new_dir_ID);
		//FSMakeFSSpec(new_v_ref_num,new_dir_ID,(unsigned char*)"",&MyFSSpec);
		//FSpMakeFSRef(&MyFSSpec,&MyFSRef);		// now have FSRef
		FSRefMakePath(&final_parentFSRef,(unsigned char*)curname,255);
		chdir(curname); 
		//printf("%s\n",curname);
#endif
		getfile(0,-1);
		printf("OMA>");
	}
	
	//HSetVol(NULL,v_ref_num,dir_ID);
#ifdef DO_MACH_O
	chdir(oldname);
#endif
	return 0;
}


/* ____________________________ save data... ____________________________*/
  
int savedatafile()
{

	short err;
 	char oldname[CHPERLN];
	int fd;
	extern char lastname[],txt[];
    //SInt16 v_ref_num; 
    //SInt32 dir_ID;
    
    	
	//HGetVol( (unsigned char*) oldname,&v_ref_num,&dir_ID);
#ifdef DO_MACH_O
	getcwd(oldname,CHPERLN);
#endif
        
	err = savefile_dialog();

	if(!err) {
	    //fsdelete((char*)p2cstr(reply.fName), reply.vRefNum);
#ifdef DO_MACH_O
            //HGetVol( (unsigned char*) curname,&new_v_ref_num,&new_dir_ID);
            //FSMakeFSSpec(new_v_ref_num,new_dir_ID,(unsigned char*)"",&MyFSSpec);
            //FSpMakeFSRef(&MyFSSpec,&MyFSRef);		// now have FSRef
            FSRefMakePath(&final_parentFSRef,(unsigned char*)curname,255);
            chdir(curname);
            //printf("%s\n",curname);
#endif
            
            fd = creat(txt,PMODE);
			
	} else {
		return -1;
	}
	
	strcpy(lastname,txt);
		
	savefile(fd,-1);
	printf("OMA>");
	
	//HSetVol(NULL,v_ref_num,dir_ID);
#ifdef DO_MACH_O
        chdir(oldname);
#endif
        
        return 0;
}

/************************************* Service routines ********************/

Boolean enough_memory()
{

  Ptr	test;		// test pointer -- be sure that there is extra room 


  test = (Ptr)NewPtr(SIZEOFEXTRA);
  if(test == nil) {
  	nomemory();
	return(false);
  }
  DisposePtr(test);
  return(true);
}

/************************************* from navsample menus.c code ********************/

/* file filter for the IMPORT menu option
 file types:
				jpeg
				pict
				tiff
				nef
				fit
				fts
*/
Boolean MyFilterProc (AEDesc *theItem, void *info, 
                    void *callBackUD, 
                    NavFilterModes filterMode )			// this routine copied from the Carbon navigation documentation
{
    Boolean             display = true;
    NavFileOrFolderInfo *theInfo = (NavFileOrFolderInfo*)info;
    FSRef               ref;
    LSItemInfoRecord    outInfo; 
	char curname[CHPERLN];
	int i;
	
    if (theInfo->isFolder == true)										// display folders
        return true;
	if (theInfo->fileAndFolder.fileInfo.finderInfo.fdType == 'PICT')	// display all pict files
			return true;
    AECoerceDesc (theItem, typeFSRef, theItem); 
    if ( AEGetDescData (theItem, &ref, sizeof (FSRef)) == noErr )
    {
        outInfo.extension = NULL;
        if ( LSCopyItemInfoForRef (&ref, 
                    kLSRequestExtension|kLSRequestTypeCreator, 
                    &outInfo) == noErr )
        {
            CFStringRef itemUTI = NULL;
            if ( outInfo.extension != NULL ) 
            {
                itemUTI = UTTypeCreatePreferredIdentifierForTag (
                             kUTTagClassFilenameExtension,
                             outInfo.extension, 
                            NULL );
                CFRelease( outInfo.extension );
            }
            else  
            {
                CFStringRef typeString = UTCreateStringForOSType(
                                            outInfo.filetype );
                itemUTI = UTTypeCreatePreferredIdentifierForTag (
                                kUTTagClassFilenameExtension, 
                                typeString, 
                                NULL );
                CFRelease( typeString );
            }
            if (itemUTI != NULL) 
            {
                display = UTTypeConformsTo(itemUTI,CFSTR("public.jpeg")) || UTTypeConformsTo(itemUTI,CFSTR("public.tiff")) ;
                CFRelease (itemUTI); 
            }
        }
		FSRefMakePath(&ref,(unsigned char*)curname,255);
		i = strlen(curname)-4;
		if( memcmp(&curname[i],".fts",4) == 0 || memcmp(&curname[i],".FTS",4) == 0 ||
			memcmp(&curname[i],".fit",4) == 0 || memcmp(&curname[i],".FIT",4) == 0) display = true;
		if( memcmp(&curname[i],".nef",4) == 0 || memcmp(&curname[i],".NEF",4) == 0 ||
		    memcmp(&curname[i],".dng",4) == 0 || memcmp(&curname[i],".DNG",4) == 0) display = true;
		
    }
    return display;
}

int* 	gDocumentList[2];
NavTypeListHandle my_types_hdl;
OSType thetypes[8];	// space for different resource types
static Handle NewOpenHandle(OSType applicationSignature, short numTypes, OSType typeList[]);

OSErr getfile_dialog(int ftype )
{	
	
	
		
	return noErr;
}

OSErr savefile_dialog()
{

	return noErr;
}


// from NavigationServicesSupport.c

static Handle NewOpenHandle(OSType applicationSignature, short numTypes, OSType typeList[])
{
	Handle hdl = NULL;
	
	if ( numTypes > 0 )
	{
	
		hdl = NewHandle(sizeof(NavTypeList) + numTypes * sizeof(OSType));
	
		if ( hdl != NULL )
		{
			NavTypeListHandle open		= (NavTypeListHandle)hdl;
			
			(*open)->componentSignature = applicationSignature;
			(*open)->osTypeCount		= numTypes;
			BlockMoveData(typeList, (*open)->osType, numTypes * sizeof(OSType));
		}
	}
	
	return hdl;
}
