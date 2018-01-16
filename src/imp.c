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
#include "hdrloaderC.h"

#define READMODE O_RDONLY  /* Read file mode. */

TWOBYTE	header[HEADLEN/2] = { 0,0,0,0,0,1,500,500,1,1,0,0,0,1,1 };
TWOBYTE	trailer[TRAILEN/2];
char	comment[COMLEN] = {0};
char	lastname[CHPERLN] = {0};	/* a copy of the last file name specified -- for labeling windows */

char*   macbuf;    	    	/* the macro buffer */
char*   exbuf[EX_NEST_DEPTH];    	    	// the execute buffers 
char*	macstring;		/* strings that can be inserted in macros */

char*	variable_names;		/* variable names in macros */

char	headcopy[512];		/* copy of all header for file stuff */
int     maccount,macflag,macptr,macval;
int	macincrement = 1;	/* the increment for macros */
int     exflag,exptr[EX_NEST_DEPTH],exval[EX_NEST_DEPTH];
int	npts;			/* number of data points */

DATAWORD	min;
DATAWORD	max;		/* for maxx subroutine */
DATAWORD	rmax,gmax,bmax;	// for rgb images

/* Detector Characteristics */
int	dlen = 75000;		/* the max number of channels on the detector */
int	dhi = 75000;		/* the max number of tracks on the detector   */
//int	nbias = 373;	    	/* one pixel bias offset to remove in binning */

char	passflag = 0;		/* flag that, when set, passes unrecognised
				   commands to the controller */

unsigned int     maxint = (1<<(DATABYTES*8-1))-4;		//	32764		// the max count 
DATAWORD	cmin = 0;		/* the color display parameters */
DATAWORD	cmax = 1000;
short	pixsiz = 1;		/* block pixel size */
short	newwindowflag = 1;	/* determines whether new window should be opened or just
				   display in the old one */
int	doffset = 80;		/* the data offset. 0 for SIT data; 80 for CCD data. */
short	detector = 1;		/* the detector type 0 for SIT; 1 for CCD */
int	detectorspecified = 1;	/* flag that tells if the detector type has
					    been explicitly specified with the ccd 
					    or sit command */
int	openflag = 0;		/* flag that controls whether files are closed after */
				/*	a "get" command.  If set, files remain open. */
				/*	Use this for getting data from files containing */
				/*	several different pictures with a single header */
int	fileisopen = 0;		/*	This also needed for above. Not user set. */

int	have_max = 0;		/* flag that indicates whether or not the minimum
				   and maximum value have been found for the data
				   in the current buffer. 0 -> no;  1 -> yes */

char	block_ave = 0;		/* flag that determines if the results of the BLOCK command
				   are to be averaged. 1 -> average, 0 -> simply sum with no
				   overflow checking */

DATAWORD *datpt;		/* the data pointer */
DATAWORD *respdat;		/* the data pointer for responses*/
DATAWORD *backdat;		/* the data pointer for backgrounds */
DATAWORD *meandat;		/* the data pointer for mean */

DATAWORD* temp_dat[NUM_TEMP_IMAGES*4] = {0};
TWOBYTE*  temp_header[NUM_TEMP_IMAGES*4] = {0};


float 	*fdatpt;		/* floating point data pointer */
long data_buffer_size;		/* the number of bytes in the data buffer */

unsigned int meansize,backsize,respsize;

/*	DATAWORD  data[DBUFLEN];*/
DATAWORD  mathbuf[MATHLEN];
char    cmnd[CHPERLN];  		/* the command buffer */

char	saveprefixbuf[PREFIX_CHPERLN];		/* save data file prefix buffer */
char	savesuffixbuf[PREFIX_CHPERLN];		/* save data file suffix buffer */
char	getprefixbuf[PREFIX_CHPERLN];		/* get data file prefix buffer */
char	getsuffixbuf[PREFIX_CHPERLN];		/* get data file suffix buffer */
char	graphicsprefixbuf[PREFIX_CHPERLN];	/* graphics file prefix buffer */
char	graphicssuffixbuf[PREFIX_CHPERLN];	/* graphics file suffix buffer */
char	macroprefixbuf[PREFIX_CHPERLN];	/* macro file prefix buffer */
char	macrosuffixbuf[PREFIX_CHPERLN];	/* macro file suffix buffer */

int have_full_name = 0;				// if this is set, fullname doesn't do anything
									// used in non-MacOS cases
int swap_bytes;

short image_is_color = 0;           // set this if image is color
short image_planes = 1;             // the number of image planes

#ifndef ENDIAN
#define ENDIAN 0
#endif

int is_big_endian = ENDIAN;		// this tells the byte ordering of this machine
								// big endian is PowerPC et al
								// little endian is intel et al


int save_rgb_rectangle = 0;		// flag to determine if rectangle to be saved is part of an rgb image

int start_oma_time;
									
extern ComDef    commands[];

int argc = 0;
char *argv[200];
char dcraw_arg[CHPERLN];

Variable namedTempImages[2*NUM_TEMP_IMAGES];    // a place to store the names of named temp images


int printf(),pprintf(),dcraw_glue(),gets(),float_image(),loadwarp(),minus(),multiply(),divide(),bit8(),read_jpeg(),read_fit(),
	get_float_image(),new_float_image(),arithtmp(),dcrawarg();

int main()
{
        int i = 1;
        extern int pause_flag,macro_line_number;
		
		start_oma_time = TickCount();
		
		datpt = 0;		/* show that there is no vaild pointer */
		fdatpt = 0;		/* show that there is no vaild pointer */
		
		dinit();		/* initialize the display and set default params */
		checkpar();		/* go check allocation of memory */
		respdat = backdat = meandat = 0;	/* pointers not yet allocated */
		
		macbuf =  calloc(MBUFLEN,1);
		//exbuf[which_ex_buffer] =  calloc(MBUFLEN,1);
		macstring = calloc(COMLEN,1);
		variable_names = calloc(VBUFLEN,1);
		strcpy(cmnd,	DCRAW_ARG);
		dcrawarg(0,-1);
		
        while (i) {
                printf("OMA>");
                if(!pause_flag) {
                	comdec();
                }
                else {
                	while(pause_flag != 0) {
                		checkevents();
                		if(pause_flag == -1){
                			if(macro_line_number >=2)
                				macro_line_number -= 2;
                			pause_flag = 0;	
                		}
                		
                	}
                	comdec();
               }
                	
        }
        return 0;
}

/* ************************ Commands ********************************/
/* ********** */

#define LOOKING_FOR_MATCH 0
#define LISTING 1

int help(int n, int index)
{
        extern  ComDef    commands[],my_commands[];
        int i,j,status,length;
	int fd,gethelpfile(),gettextline();
	char string[CHPERLN];
	
		if(index == 0) {
                	printf( "Available Commands are:\n" );
                	i = 0;
               		while ( commands[i].text.name[0] != EOL ) {
                        	printf( "%s\n", commands[i].text.name);
              			i++;
                	}
                	printf( "Available Custom Commands are:\n" );
                	i = 0;
               		while ( my_commands[i].text.name[0] != EOL ) {
                        	printf( "%s\n", my_commands[i].text.name);
              			i++;
                	}
		}
		else {
			fd = gethelpfile();
			if( fd == -1) {
				printf("'OMA HELP' File Not Found.\n");
				return -1;
			}
			
			for( i = index; (cmnd[i] = toupper(cmnd[i])) != EOL; i++ ) ; /* convert to uppercase */
			status = LOOKING_FOR_MATCH;
			while( (length = gettextline(fd,string)) > 0) {
				switch (status) {
				case LOOKING_FOR_MATCH:
					for( j = index; j<i; j++) {
						if( cmnd[j] != string[j-index])
							break;
					}
					if( j != i )
						break;
					printf("%s",string);
					status = LISTING;
					break;
				case LISTING:
					if( length <= 2) {
						status = LOOKING_FOR_MATCH;
						printf("\n");
						break; 
					}
					printf("%s",string);
				}
						
			}
			helpdone(fd);	/* clean-up in the file system, close help file */
		}
		return 0;
}

/* get a line of text from a file specified by fd */
#define BREAK_AFTER 80	
int gettextline(fd,textline)

int fd;
char textline[];
{
	static int i = -2;
	int n = 0;
	static int j = 0;

	
	if( i == -2 ){				// read the first time 
		i = read(fd,headcopy,512);
	}	
	
	while( headcopy[j]  != CarRet && headcopy[j]  != LF ) {
		
		if( j >= i) {
			i = read(fd,headcopy,512);
			j = 0;
			if( i < 1) {
				return -1;	// end of file or error 
			}
                        //goto next_char;
		} else {
            if( n >  BREAK_AFTER && headcopy[j] == ' '){
				// cut things off here
				
				#ifdef Mac	
				textline[n++] = CarRet;		// Mac uses <cr> for new line
				//j++;
				#else
				textline[n++] =  LF;		// Unix uses <lf> for new line
				//j++;
				#endif
				textline[n++] ='\t'	;		// tab over 
				textline[n] = 0;
				return n;					
			}
			textline[n++] = headcopy[j++];
			
			// this for lines that may be too long 
			if(n >=CHPERLN-2) {
				n = CHPERLN-3;
			}
		}
//next_char:                
	}
#ifdef Mac	
	textline[n++] = CarRet;		// Mac uses <cr> for new line
        j++;
#else
	textline[n++] =  LF;		// Unix uses <lf> for new line
	j++;
#endif

	textline[n] = 0;
	return n;		
}

/* ********** */

int logg(int n,int index)
{
	
        int i,j,k;
        int nc = 1;
	char *tempbuf=0;
	
        i = 0;          /* array pointers*/

        if (n == 0)
                n = 1;
        /* to get pointer to the next character, skip past n-1 zeros in the
           comment buffer.  1st line is reserved for date  -- in ccd not old oma */

        j = n;
        while (--j) { 			/* find the start of the specified comment line */
		k = i;
		while (comment[i++]){}; 
		if ( k+1 == i) {
			beep();
			printf("Log Is Not That Long.\n");
			return -1;
		}
	}

	while ( (cmnd[index] != EOL) && (cmnd[index] != ';') ) {
		if(cmnd[index++] == ' ') { 	/* if there is a comment on this line */
			j = strlen(&cmnd[index])+1;	/* the length of what we are going to add */ 
			nc = i;
			k = 0;
			if (comment[nc] != EOL){ /* if there are more lines, they will have to be tacked on at the end */
				while (comment[nc++]){};	 /* nc is now the start of the next line or EOL */
				tempbuf = (char*) mathbuf;
				while(comment[nc] ) {
					while(comment[nc])
						*(tempbuf+k++) = comment[nc++];
					*(tempbuf+k++) = comment[nc++];		/* remember the 0 */
				}
				/* printf("%d characters to be appended\n",k); */
			}
			
			if ( (i + k + j) >= COMLEN) {
				beep();
       	        printf ("Comment Buffer Overflow.\n");
       	        return -1;
       	    }
			while((cmnd[index] != EOL) && (cmnd[index] != ';')) { 
				comment[i++] = cmnd[index++]; 
			}
			comment[i++] = EOL;			

			for(j=0; j<k; j++) {
				comment[i++] = *(tempbuf+j);	
			}
			comment[i++] = EOL;
			return 0;
		}
	}

        while (nc) {
                printf("Line #%d: ",n++);
                nc = getlin(cmnd);
                if ( (i+nc) >= COMLEN ) {
					beep();
                    printf ("Comment Buffer Overflow.\n");
                    break;
                }
                j = 0;
                while (cmnd[j]) { comment[i++] = cmnd[j++]; }
                comment[i++] = EOL;
        }
        comment[i] = EOL;
        return 0;
}

/* ********** */

int list()
{
        int lc,i;
	
        lc = 1;
        i = 0;
        while (comment[i]) {
                printf( "Line #%d: ",lc++);
		pprintf( "%s\n",&comment[i]);
                while (comment[i]) {
                        i++;
                }
                i++;
        }
	pprintf("\n");
	pprintf(" %7d  Data Points\n",npts);
	pprintf(" %7d  Columns (Channels)\n",header[NCHAN]);
	pprintf(" %7d  Rows (Tracks)\n",header[NTRAK]);
	pprintf(" %7d  X0\n",header[NX0]);
	pprintf(" %7d  Y0\n",header[NY0]);
	pprintf(" %7d  Delta X\n",header[NDX]);
	pprintf(" %7d  Delta Y\n",header[NDY]);
	#ifdef FLOAT
	pprintf(" %g  Color Minimum\n %g  Color Maximum\n",cmin,cmax);
	#else
	pprintf(" %7d  Color Minimum\n %7d  Color Maximum\n",cmin,cmax);
	#endif
/*	printf(" File Prefix: '%s'\n",prefixbuf); */
/*	printf(" File Suffix: '%s'\n",suffixbuf); */
/*	printf("\nDisplay Type  : dt  = %d\n",disp_dflag);
	pprintf("Max height of any pixel in a row : dhi = %d\n",disp_height);
	pprintf("3D grid resolution : ddx = %d ddy = %d ddz = %d\n",
		disp_dx,disp_dy,disp_dz);
	pprintf("Display origin: orgx= %d orgy = %d\n",disp_x0,disp_y0);

	if (passflag)
		pprintf("\nUnknown Commands Passed to Camera Controller.\n");    
	else
		pprintf("\nUnknown Commands Flagged.\n");  */	
	return 0;
		
}

/* ********** */

int prefix(int n,int index)
{

	if (index == 0)
		saveprefixbuf[0] = '\0';
	else
		strcpy(saveprefixbuf,&cmnd[index]);
	return 0;
}
	
/* ********** */

int suffix(int n,int index)
{

	if (index == 0)
		savesuffixbuf[0] = '\0';
	else
		strcpy(savesuffixbuf,&cmnd[index]);
	return 0;
}
	
/* ********** */

int defmac(int n,int index)
{

        extern char* macbuf;

        int i,j,k;
        int nc = 1;
	char *tempbuf=0;
	
        i = 0;          /* array pointers*/

        if (n == 0)
                n = 1;
        /* to get pointer no the next character, skip past n-1 zeros in the
           macro buffer */	
	
	j = n;
        while (--j) { 			/* find the start of the specified line */
		k = i;
		while (*(macbuf + i++) ){}; 
		if ( k+1 == i) {
			beep();
			printf("Macro Is Not That Long.\n");
			return -1;
		}
	}




	while ( (cmnd[index] != EOL) && (cmnd[index] != ';') ) {
		if(cmnd[index++] == ' ') { 	/* if there is a macro definition on this line */
			j = strlen(&cmnd[index])+1;	/* the length of what we are going to add */ 
			nc = i;
			k = 0;
			if (*(macbuf+nc) != EOL){ /* if there are more lines, they will have to be tacked on at the end */
				while (*(macbuf+nc++)){};	 /* nc is now the start of the next line or EOL */
				tempbuf = (char*) mathbuf;
				while(*(macbuf+nc) ) {
					while(*(macbuf+nc))
						*(tempbuf+k++) = *(macbuf+nc++);
					*(tempbuf+k++) = *(macbuf+nc++);		/* remember the 0 */
				}
				/* printf("%d characters to be appended\n",k); */
			}
			
			if ( (i + k + j) >= MBUFLEN) {
				beep();
       	                	 printf ("Macro Buffer Overflow.\n");
       	                 	return -1;
       	       	  	}
			while((cmnd[index] != EOL) && (cmnd[index] != ';')) { 
				*(macbuf+i++) = cmnd[index++]; 
			}
			*(macbuf+i++) = EOL;			

			for(j=0; j<k; j++) {
				*(macbuf+i++) = *(tempbuf+j);	
			}
			*(macbuf+i++) = EOL;
			return 0;
		}
	}


        while (nc) {
                printf("Line #%d: ",n++);
                nc = getlin(cmnd);
                if ( (i+nc) >= MBUFLEN ) {
			beep();
                        printf ("Macro Buffer Overflow.\n");
                        break;
                }
                j = 0;
                while (cmnd[j]) {
                        *(macbuf + i++) = cmnd[j++];
                }
                *(macbuf + i++) = EOL;
        }
        *(macbuf+i) = EOL;
	clear_macro_to_end();		/* insert trailing zeros after the macro */
	return 0;
}

int clear_macro_to_end()
{
	int i=0;	
	
	while( *(macbuf+i++) ) {
		while( *(macbuf+i++)){};
	}
	//printf("%d\n",i);
	while( i<MBUFLEN) 
		*(macbuf+i++) = 0;
	return 0;	
	
}
/* ********** */

int listmac()
{
        extern char* macbuf;
        int lc,i;

        lc = 1;
        i = 0;
        while (*(macbuf+i)) {
                printf( "Line #%d: ",lc++);
		pprintf( "%s\n",macbuf+i);
                while (*(macbuf+i)) {
                        i++;
                }
                i++;
        }
        return 0;
}

/* ********** */

int stringmacro(int n,int index)
{
        extern char* macstring;

	if (index == 0) {
		pprintf("%s\n",macstring);
	} else {
	strncpy(macstring,&cmnd[index],COMLEN-index);	
	}
	return 0;
}
/* ********** */

int valmac(int n,int index)
{

     extern int int_value,inc_value;
     int narg;
     narg = sscanf(&cmnd[index],"%d %d",&int_value,&inc_value);
     switch( narg){
     	case 0:
     		int_value = 0;
     		inc_value = 1;
     		break;
     	case 1:
     		inc_value = 1;
     		break;
	}
	printf("Start Value: %d  Increment: %d\n",int_value,inc_value);
	return 0;
}
/* ********** */

int echo(int n,int index)
{

	if (index != 0) {
		pprintf("%s\n",&cmnd[index]);
	}
	return 0;
}

/* ********** */

int echo_color(int n,int index)
{
	int narg;
	int colflag;
	char arg1[512];
	char arg2[512];
	char *remains;
	extern char scratch[];
	int cprintf();
	
	// Lets keep a copy of the arguments for later
	strcpy(scratch, &cmnd[index]);
	
	// Read in the first argument as a string
	narg = sscanf(&cmnd[index],"%s %s", arg1, arg2);
	
	colflag = BLACK;
	if (!strncmp(arg1, "R", 1) ||  !strncmp(arg1, "r", 1) ) colflag = RED;
	if (!strncmp(arg1, "O", 1) || !strncmp(arg1, "o", 1) ) colflag = ORANGE;
	if (!strncmp(arg1, "Y", 1) || !strncmp(arg1, "y", 1) ) colflag = YELLOW;
	if (!strncmp(arg1, "G", 1) || !strncmp(arg1, "g", 1) ) colflag = GREEN;
	if (!strncmp(arg1, "B", 1) || !strncmp(arg1, "b", 1) ) colflag = BLUE;
	if (!strncmp(arg1, "I", 1) || !strncmp(arg1, "i", 1) ) colflag = INDIGO;
	
	// Print the rest of the line after the first argument
	remains = strchr(scratch, ' ');
	if (narg >= 2)  cprintf(colflag, "%s\n", remains);
	return OMA_OK;
}


/* ********** */

int null()
{
	return 0;

}
/* ********** */


/* ********** */

int exitx()
{
    int savsettings(int,int);
#ifdef GigEthernetCamera
    PvUnInitialize();
#endif	
	
//	omaio(BYE);
#ifdef SBIG
	int close_SBIG();
    extern int linked;
	if(linked) close_SBIG();
#endif
#ifdef ANDOR
	ShutDown();
#endif

#ifdef GPHOTO
	int close_gphoto_cam();
	close_gphoto_cam();
#endif
    printf( "Exit\n");
    savsettings(0,0);        // save current settings for next time
	exit(0);
	return 0;
}

/* ********** */

int maxx()
{
	extern	DATAWORD min,max;
	extern 	int have_max;
	extern Variable user_variables[];
	
	
	
	if(!have_max) find_minmax();
	
	max = *(datpt+doffset+header[NMAX]+header[LMAX]*header[NCHAN]);
	min = *(datpt+doffset+header[NMIN]+header[LMIN]*header[NCHAN]);
	printf(MAXMSG,max,header[LMAX],header[NMAX]);
	printf(MINMSG,min,header[LMIN],header[NMIN]);
    if(image_is_color)  printf("R G B max: %g %g %g\n",rmax,gmax,bmax);
	printf(SFACTRMSG,trailer[SFACTR]);
	
	user_variables[5].ivalue = user_variables[5].fvalue = min;
	user_variables[5].is_float = 1;
	user_variables[6].ivalue = user_variables[6].fvalue = max;
	user_variables[6].is_float = 1;
    if(image_is_color){
        user_variables[7].ivalue = user_variables[7].fvalue = rmax;
        user_variables[7].is_float = 1;
        user_variables[8].ivalue = user_variables[8].fvalue = gmax;
        user_variables[8].is_float = 1;
        user_variables[9].ivalue = user_variables[9].fvalue = bmax;
        user_variables[9].is_float = 1;
    } else {
        user_variables[7].ivalue = user_variables[7].fvalue = header[LMAX];
        user_variables[7].is_float = 0;
        user_variables[8].ivalue = user_variables[8].fvalue = header[NMAX];
        user_variables[8].is_float = 0;
   
    }
    
	return 0;
}
/* ********** */

int find_minmax()
{
    extern  DATAWORD *datpt;
	extern	int npts,have_max;
	extern	DATAWORD min,max;
    
    int lmx,lmn,n;
	DATAWORD *locmin,*locmax;
	DATAWORD *mydatpt,*pt_green,*pt_blue;
    
	mydatpt = datpt+doffset;
    locmin = locmax = mydatpt;
    *locmin = *locmax = rmax = *mydatpt;
	pt_green = mydatpt + npts/3;
	pt_blue =  pt_green + npts/3;
    gmax = *pt_green;
    bmax = *pt_blue;
    
    if(image_is_color){    
        while ( mydatpt < datpt+npts+doffset ) {
            if ( *mydatpt > *locmax ) {
                locmax = mydatpt;
            }
            
            if ( *mydatpt < *locmin ) {
                locmin = mydatpt;
            }
            if( (mydatpt < pt_green) && (*mydatpt > rmax)) rmax = *mydatpt;
            if( (mydatpt >= pt_green) && (mydatpt < pt_blue) && (*mydatpt > gmax)) gmax = *mydatpt;
            if( (mydatpt >= pt_blue) && (*mydatpt > bmax)) bmax = *mydatpt;
            
            mydatpt++;
        }
	} else{
        while ( mydatpt < datpt+npts+doffset ) {
            if ( *mydatpt > *locmax ) {
                locmax = mydatpt;
            }
            
            if ( *mydatpt < *locmin ) {
                locmin = mydatpt;
            }
            
            mydatpt++;
        }
        
    }
	min = *locmin;
	max = *locmax;
	lmx = (int) (locmax - datpt - doffset);
	lmn = (int) (locmin - datpt - doffset);
	n = header[NCHAN];
	header[LMAX] = lmx/n;	      		/* This is the row of the max */
	header[NMAX] = lmx - lmx/n*n;   	/* Column of max */
	header[LMIN] = lmn/n;	      		/* This is the row of the min */
	header[NMIN] = lmn - lmn/n*n;   	/* Column of min */
	if ( trailer[SFACTR] == 0 ) trailer[SFACTR] = 1;
	have_max = 1;
	return 0;
}

/* ********** */

int incmac(int n)
{
        extern int macincrement;

        macincrement = n;
        if (macincrement == 0) 
                macincrement = 1;
         return 0;
}

/* ********** */

int my_invert()					/* invert the data array */
{
	int size,i;
	DATAWORD *datp2,*datp,temp;
	
        extern DATAWORD *datpt;
	extern int	doffset;
	
	
	size = header[NCHAN] * header[NTRAK];
	datp = datpt + doffset;
	datp2 = datpt + size -1 + doffset;
	
	for(i=0; i < size/2 ; i++){
		temp = *(datp+i);
		*(datp+i) = *(datp2-i);
		*(datp2-i) = temp;
	}
	
	header[NMAX] = header[NCHAN] - 1 - header[NMAX];
	header[NMIN] = header[NCHAN] - 1 - header[NMIN];
	header[LMAX] = header[NTRAK] - 1 - header[LMAX];
	header[LMIN] = header[NTRAK] - 1 - header[LMIN];

	setarrow();	
	return 0;
}
/* ********** */

int mirror()					/* mirror the data array */
{
	int nt,nc,size;
	DATAWORD *datp,temp;

        extern DATAWORD *datpt;
	extern int	doffset;
	
	
	
	datp = datpt+doffset;
	size = header[NCHAN]-1;
		
	for(nt=0; nt<header[NTRAK];nt++){
		for(nc=0; nc<header[NCHAN]/2; nc++) {
			temp = *(datp+nc);
			*(datp+nc) = *(datp+size-nc);
			*(datp+size-nc) = temp;
		}
		datp += header[NCHAN];
	}
	
	header[NMAX] = header[NCHAN] - 1 - header[NMAX];
	header[NMIN] = header[NCHAN] - 1 - header[NMIN];

	setarrow();	
	return 0;
}

/* ********** */

int setccd(int n,int index)
/* set the detector flag to ccd*/
{

	int x,y,i;

	/* Try to get two arguments */
	
	for ( i = index; (cmnd[i] != EOL) && (cmnd[i] != ';') ; i++) {
		if(cmnd[i] == ' ') { 
			if( sscanf(&cmnd[index],"%d %d",&x,&y) >= 2) {
				dlen = x;
				dhi = y;
			} 
			break;
		}
	}
	detector = CCD;			
	detectorspecified = 1;		
	doffset = 80;
	return 0;			
	
}
/* ********** */

int setsit(int n)
/* set the detectof flag to ccd*/
{
	if( n>= 0) {
		detector = SIT;
		detectorspecified = 1;
		doffset = 0; }
	else {
		detectorspecified = 0;
	}
	return 0;
	
}

/* ********** */


int rotate(int n,int index)
/* rotate the data array */
{
	int size,nt,nc,width=0,height=0,midx=0,midy=0,vrel,hrel;
	DATAWORD *datp,*datp2;
	DATAWORD idat(int,int),interpolate(float,float);
	
	float theta,sintheta=0,costheta=1,ntf,ncf,outsideval,angle;
	
	
	extern DATAWORD *datpt;
	extern int	doffset;
	
	if(n==0) {
		size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET);
	} else {
		sscanf(&cmnd[index],"%f",&angle);
		theta = angle / 180.0 * PI;
		sintheta = sin(theta);
		costheta = cos(theta);
		
		if(angle == 180. || angle == -180.){
			my_invert();
			return 0;
		}
		if(angle == 270. || angle == -90.){
			my_invert();
			n = 0;
		}
		
		printf("%f theta, %f sin, ",theta,sintheta);
		printf("%f cos\n",costheta);
		
		width = header[NCHAN]*fabs(costheta) + header[NTRAK]*fabs(sintheta);
		height = header[NTRAK]*fabs(costheta) + header[NCHAN]*fabs(sintheta);
		midx = width/2;
		midy = height/2;
		size = width * height + MAXDOFFSET;
		printf("%d %d chan track\n",width,height);
	}
	size = (size+511)/512*512;		/* make a bit bigger for file reads */
	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	if(datp == 0) {
		nomemory();
		return -1;
	}
	
	for(nc=0; nc<doffset; nc++) 
		*(datp++) = *(datpt+nc);	/* copy the CCD header */
	if(n==0 || angle == 90.) {
		for(nc=header[NCHAN]-1; nc >= 0; nc--) {
			for(nt=0; nt<header[NTRAK];nt++){
				*(datp++) = idat(nt,nc);
			}
		}
		nt = header[NCHAN];
		header[NCHAN] = header[NTRAK];
		header[NTRAK] = nt;

		nt = header[NX0];
		header[NX0] = header[NY0];
		header[NY0] = nt;

		nt = header[NDX];
		header[NDX] = header[NDY];
		header[NDY] = nt;

		nt = header[NMAX];
		header[NMAX] = header[LMAX];
		header[LMAX] = header[NTRAK] - nt -1;

		nt = header[NMIN];
		header[NMIN] = header[LMIN];
		header[LMIN] = header[NTRAK] - nt -1;

	} else {
		// for points outside the image, calculate the average of the perimeter
		outsideval = 0.0;
		for(nc=0; nc<header[NCHAN]; nc++) {
			outsideval += idat(0,nc);
			outsideval += idat(header[NTRAK]-1,nc);
		}
		for(nt=1; nt<header[NTRAK]-1; nt++) {
			outsideval += idat(nt,0);
			outsideval += idat(nt,header[NCHAN]-1);
		}
		outsideval = outsideval/(header[NCHAN]+header[NTRAK]-2)/2;
		for(nt=0; nt<height;nt++){
			vrel = nt - midy;
			for(nc=0; nc<width; nc++) {
				hrel = nc-midx;
				ncf = hrel*costheta - vrel*sintheta + header[NCHAN]/2.0;
				ntf = vrel*costheta + hrel*sintheta + header[NTRAK]/2.0;
				if( (ntf >= 0.0) && (ntf < (float)header[NTRAK]) &&
				    (ncf >= 0.0) && (ncf < (float)header[NCHAN])   ) {
					*(datp++) = interpolate(ncf,ntf);
				} else {
				
					*(datp++) = outsideval;	
				}
			}
		}
		header[NCHAN] = width;
		header[NTRAK] = height;
		npts = header[NCHAN] * header[NTRAK];
		have_max = 0;
	}

	free(datpt);
	datpt = datp2;
	maxx();
	update_status();
	setarrow();	
	return 0;
}

int rotrgb(int n,int index)
/* rotate an rgb image */
{
	int stemp(int,int),gtemp(int,int),ftemp(int,int);
	int rgb2red(),rgb2green(),rgb2blue(),comtmp();
    
	
	
	// disable printing from commands
	keylimit(-1);
	// store the current image
	have_max = 1;
	stemp(-1,-1);
    header[NMAX] = header[LMAX] = header[NMIN] = header[LMIN] = 0;
	// get green
	rgb2green(-1,-1);	
	// rotate and save
	rotate(n,index);
	stemp (-2,-1);
	// get the big image
	gtemp(-1,-1);
    header[NMAX] = header[LMAX] = header[NMIN] = header[LMIN] = 0;
	// get blue
	rgb2blue(-1,-1);	
	// rotate and save
	rotate(n,index);
	stemp (-3,-1);
	// get the big image
	gtemp(-1,-1);
    header[NMAX] = header[LMAX] = header[NMIN] = header[LMIN] = 0;
	// get red
	rgb2red(-1,-1);
	// rotate
	rotate(n,index);
	// add green 
	comtmp(-2,-1);
	// add blue
	have_max = 0;
	keylimit(-2);	// reset printing to previous mode
    image_is_color = 1;
    trailer[IS_COLOR] = 1;
	comtmp(-3,-1);
    image_is_color = 1;
    trailer[IS_COLOR] = 1;
	
	// free temporaries
	
	ftemp(-1,-1);
	ftemp(-2,-1);
	ftemp(-3,-1);
	
	return 0;
	
}


/* ********** */

int sback()					/* get and save a background */
{
	int nc;

	extern DATAWORD *datpt;
	extern int	doffset;

	
	backsize = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;

	if(backdat != 0) free(backdat);
	backdat = malloc(backsize);
	if(backdat == 0) {
		nomemory();
		backsize = 0;
		return -1;
	}
	
	for(nc=0; nc<doffset + header[NCHAN] * header[NTRAK]; nc++) 
		*(backdat+nc) = *(datpt+nc);								/* copy the header and data*/
		
	setarrow();	
	return 0;
}
/* ********** */

int sresp()
					/* get and save a response */
{
	int nc;

   	extern DATAWORD *datpt;
	extern int	doffset;
	
	respsize = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;

	if(respdat != 0) free(respdat);
	respdat = malloc(respsize);
	if(respdat == 0) {
		nomemory();
		respsize = 0;
		return -1;
	}
	
	
	for(nc=0; nc<doffset + header[NCHAN] * header[NTRAK]; nc++) { 
		*(respdat+nc) = *(datpt+nc); 		/* copy the header and data*/
		if( *(respdat+nc) == 0)
			*(respdat+nc) = 1; /* 0's? We don't need no stinking 0's */
	}
		
		
	setarrow();	
	return 0;
}
/* ********** */

int smean()
					/* get and save a mean */
{
	int nc;
	
   	extern DATAWORD *datpt;
	extern int	doffset;
	
	meansize = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;


	if(meandat != 0) free(meandat);
	meandat = malloc(meansize);
	if(meandat == 0) {
		nomemory();
		meansize = 0;
		return -1;
	}
	
	
	for(nc=0; nc<doffset + header[NCHAN] * header[NTRAK]; nc++) { 
		*(meandat+nc) = *(datpt+nc); 		/* copy the header and data*/
	}
		
		
	setarrow();
	return 0;	
}
/* ********** */

int freem()			/* free the memory from the background, mean and response */ 

{
	
	if(backdat != 0) free(backdat);
	if(respdat != 0) free(respdat);
	if(meandat != 0) free(meandat);
	backdat = respdat = meandat = 0;
	meansize = respsize = backsize = 0;
        return 0;
}
/* ********** */

int corect(int n)
{

	int x,nc;
	DATAWORD *datptcopy,*bcopy,*rcopy;
	
	if(backdat == 0 || respdat==0) {
		beep();
		printf("Must Define Background and Response.\n");
		return -1;
	}
	if( (header[NCHAN]*header[NTRAK]+MAXDOFFSET)*DATABYTES != respsize ||
	    (header[NCHAN]*header[NTRAK]+MAXDOFFSET)*DATABYTES != backsize) {
		beep();
		printf("Background and/or Response Sizes Conflict With Data.\n");
		return -1;
	}
	    
	
	
	if(n==0) n=1;
	
	datptcopy = datpt + doffset;
	bcopy = backdat + doffset;
	rcopy = respdat + doffset;
	
	for(nc=doffset; nc<doffset + header[NCHAN] * header[NTRAK]; nc++) {
		x = ( *(datptcopy) - *(bcopy++)) * n / *(rcopy++);
		*(datptcopy++) = x;
	}
	have_max = 0;	
	setarrow();
	return 0;	

}
/* ********** */

int fluct(int n)	/* get the fluctuation or fluctuation^2 of the data in the buffer */
{

	int x,nc;
	DATAWORD *datptcopy,*mcopy;
	
	if(meandat == 0) {
		beep();
		printf("Must Define Mean.\n");
		return -1;
	}
	if( (header[NCHAN]*header[NTRAK]+MAXDOFFSET)*DATABYTES != respsize) {
		beep();
		printf("Mean Size Conflicts With Data.\n");
		return -1;
	}

	
		
	datptcopy = datpt + doffset;
	mcopy = meandat + doffset;
	
	for(nc=doffset; nc<doffset + header[NCHAN] * header[NTRAK]; nc++) {
		*(datptcopy) = *(datptcopy) - *(mcopy++);
		datptcopy++;
	}
	
	/* if n=0, get just the fluctuation; if n != 0 get fluctuation^2/n */
	
	if( n ) {
		datptcopy = datpt + doffset;
		for(nc=doffset; nc<doffset + header[NCHAN] * header[NTRAK]; nc++) {
			x = (*(datptcopy)) * (*(datptcopy));
			*(datptcopy++) = x/n;
		}
	}
	have_max = 0;
	setarrow();	
	return 0;

}

/* ********** */
int
gsmooth(int n, int index)
				//  Gaussian Smoothing of the Data 
				// GSMOOT NX [NY]
				// sigma_x = (NX-1)/3.5
{
	DATAWORD *datp,*datp2;
	extern int	doffset;
	DATAWORD idat(int,int);
	
	int dx,dy,dxs,dys,i,j,m,size,nt,nc;
	float sigx,sigy,*mask,norm,sum;
	
	mask = 0;

	if(n <= 0) n = 3;
	
	dx = dy = n;	/* the smoothing amounts */
	
	/* Check to see if there was a second argument */
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d",&dx,&dy);
			break;
		}
	}
	if (dx/2 == dx/2.0) {
		beep();
		printf("THE X SIZE OF THE REGION MUST BE ODD!\n");
		
		return -1;
	}
	if (dy/2 == dy/2.0) {
		printf("THE Y SIZE OF THE REGION MUST BE ODD!\n");
		beep();
		return -1;
	}

	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */

	datp2 = datp = malloc(size);
	if(datp == 0) {
		nomemory();
		return -1;
	}
	
	norm = 0;

	dx = (dx-1)/2;
	dxs = -dx;
	sigx = dx/1.75;

	dy = (dy-1)/2;
	dys = -dy;
	sigy = dy/1.75;
	printf("Sigx=%5.2f, Sigy=%5.2f, ",sigx,sigy);
	printf("pixels=%d x %d\n",dx*2+1,dy*2+1);
	
	if ( sigy == 0 ) sigy = 1;
	if ( sigx == 0 ) sigx = 1;
	
	if(dx == 0 && dy == 0) return(0);	// The 1 x 1 smoothing case

	/* Set loop limit so only have to do "<", not "<=" */
	dx=dx+1; 
	dy=dy+1;

	mask = (float*) malloc((dx-dxs) * (dy-dys) * sizeof(float));
	norm = 0;
	for(i=dxs; i<dx; i++) {
		for(j=dys; j<dy; j++) {
			m=(j - dys)*(dx - dxs) + (i - dxs);
			mask[m]=exp(-(i*i/(sigx*sigx)+j*j/(sigy*sigy))/2);
			norm += mask[m];
		}
	}
	
	
	for(nc=0; nc<doffset; nc++) 
		*(datp++) = *(datpt+nc);	/* copy the CCD header */
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN];nc++){
			sum = 0;
			
			for(i=dxs; i<dx; i++) {
				for(j=dys; j<dy; j++) {
					/*
					if( (nt+j) < header[NTRAK] && 
					    (nc+i) < header[NCHAN] &&
					    (nt+j) >= 0 && (nc+i) >= 0) {
						m = (j - dys)*(dx - dxs) + (i - dxs);
						norm += mask[m];
						sum += idat(nt+j,nc+i)*mask[m];
					}
					*/
					m = (j - dys)*(dx - dxs) + (i - dxs);
					
					sum += idat(nt+j,nc+i)*mask[m];
				}
			}
			*(datp++) = sum/norm;
		}
	}
	if(mask!=0) {free(mask); mask = 0;}
	free(datpt);
	datpt = datp2;
	have_max = 0;
	maxx();
	setarrow();	
	return 0;
}

/* ********** */

int
gsmooth2(int n, int index)
				//  Gaussian Smoothing of the Data 
				// GSMOOT NX [NY]
				// sigma_x = (NX-1)/6.0
{
	DATAWORD *datp,*datp2;
	extern int	doffset;
	DATAWORD idat(int,int);
	
	int dx,dy,dxs,dys,i,j,m,size,nt,nc;
	float sigx,sigy,*mask,norm,sum;
	
	mask = 0;

	if(n <= 0) n = 3;
	
	dx = dy = n;	/* the smoothing amounts */
	
	/* Check to see if there was a second argument */
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d",&dx,&dy);
			break;
		}
	}
	if (dx/2 == dx/2.0) {
		beep();
		printf("THE X SIZE OF THE REGION MUST BE ODD!\n");
		
		return -1;
	}
	if (dy/2 == dy/2.0) {
		printf("THE Y SIZE OF THE REGION MUST BE ODD!\n");
		beep();
		return -1;
	}

	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */

	datp2 = datp = malloc(size);
	if(datp == 0) {
		nomemory();
		return -1;
	}
	
	norm = 0;

	dx = (dx-1)/2;
	dxs = -dx;
	sigx = dx/3.0;

	dy = (dy-1)/2;
	dys = -dy;
	sigy = dy/3.0;
	printf("Sigx=%5.2f, Sigy=%5.2f, ",sigx,sigy);
	printf("pixels=%d x %d\n",dx*2+1,dy*2+1);
	
	if ( sigy == 0 ) sigy = 1;
	if ( sigx == 0 ) sigx = 1;
	
	if(dx == 0 && dy == 0) return(0);	// The 1 x 1 smoothing case

	/* Set loop limit so only have to do "<", not "<=" */
	dx=dx+1; 
	dy=dy+1;

	mask = (float*) malloc((dx-dxs) * (dy-dys) * sizeof(float));
	norm = 0;
	for(i=dxs; i<dx; i++) {
		for(j=dys; j<dy; j++) {
			m=(j - dys)*(dx - dxs) + (i - dxs);
			mask[m]=exp(-(i*i/(sigx*sigx)+j*j/(sigy*sigy))/2);
			norm += mask[m];
		}
	}
	
	
	for(nc=0; nc<doffset; nc++) 
		*(datp++) = *(datpt+nc);	/* copy the CCD header */
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN];nc++){
			sum = 0;
			
			for(i=dxs; i<dx; i++) {
				for(j=dys; j<dy; j++) {
					/*
					if( (nt+j) < header[NTRAK] && 
					    (nc+i) < header[NCHAN] &&
					    (nt+j) >= 0 && (nc+i) >= 0) {
						m = (j - dys)*(dx - dxs) + (i - dxs);
						norm += mask[m];
						sum += idat(nt+j,nc+i)*mask[m];
					}
					*/
					m = (j - dys)*(dx - dxs) + (i - dxs);
					
					sum += idat(nt+j,nc+i)*mask[m];
				}
			}
			*(datp++) = sum/norm;
		}
	}
	if(mask!=0) {free(mask); mask = 0;}
	free(datpt);
	datpt = datp2;
	have_max = 0;
	maxx();
	setarrow();	
	return 0;
}

/* ********** */
int smooth(int n,int index)
/* smooth the data */
{
	DATAWORD *datp,*datp2;
	extern DATAWORD *datpt;
	extern int	doffset;
	DATAWORD idat(int,int);
	
	int dx,dy,i,j,size,nt,nc,count,dxs,dys;
	float sum;
	
	if(n <= 0) n = 2;
	
	dx = dy = n;	/* the smoothing amounts */
	
	/* Check to see if there was a second argument */
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d",&dx,&dy);
			break;
		}
	}

	
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */

	datp2 = datp = malloc(size);
	if(datp == 0) {
		nomemory();
		return -1;
	}
	dxs = -dx/2;
	dys = -dy/2;
	if( dx & 0x1)
		dx = dx/2+1;
	else
		dx /= 2;
	if( dy & 0x1)
		dy = dy/2+1;
	else
		dy /= 2;
	
	
	for(nc=0; nc<doffset; nc++) 
		*(datp++) = *(datpt+nc);	/* copy the CCD header */
	for(nt=0; nt<header[NTRAK]; nt++) {
		
		for(nc=0; nc<header[NCHAN];nc++){
			sum = 0;
			count = 0;
			for(i=dxs; i<dx; i++) {
				for(j=dys; j<dy; j++) {
					if( (nt+j) < header[NTRAK] && 
					    (nc+i) < header[NCHAN] &&
					    (nt+j) >= 0 && (nc+i) >= 0) {
						count++;
						sum += idat(nt+j,nc+i);
					}
				}
			}

			*(datp++) = sum/count;
		}
	}
	free(datpt);
	datpt = datp2;
	have_max = 0;
	setarrow();
	return 0;	
}

/* ********** */

int subsam(int n,int index)		/* sub-sample the image */
/* SUBSAM nx [ny]
	Subsample the current image, keeping every nx and ny pixel and discarding
	the rest. This command does not do any interpolation (as MAKNEW does) or
	pixel summing (as BLOCK does). If ny is ommitted, it is taken equal to nx.
*/
{
	DATAWORD *datp,*datp2;
	extern DATAWORD *datpt;
	extern int	doffset;
	DATAWORD idat(int,int);
	
	int dx,dy,i,size,nt,nc;
        
	if( index < 0) {  /* if called from routine other than command decoder */
		dx = n;
		dy = -index;
	} else {
		if(n <= 0) n = 2;
	
		dx = dy = n;	/* the blocking amounts */
	
		/* Check to see if there was a second argument */
	
		for ( i = index; cmnd[i] != EOL; i++) {
			if(cmnd[i] == ' ') { 
				sscanf(&cmnd[index],"%d %d",&dx,&dy);
				break;
			}
		}
	}

	size = (header[NCHAN]/dx * header[NTRAK]/dy + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */
	datp2 = datp = malloc(size);
	if(datp == 0) {
		nomemory();
		return 1;
	}
	
	for(nc=0; nc<doffset; nc++) 
		*(datp++) = *(datpt+nc);	/* copy the CCD header */
	
	for(nt=0; nt<header[NTRAK]/dy*dy; nt+=dy) {
		for(nc=0; nc<header[NCHAN]/dx*dx;nc+=dx){
			*(datp++) = idat(nt,nc);
		}
	}
	header[NCHAN] /= dx;
	header[NTRAK] /= dy;
	header[NDX] *= dx;
	header[NDY] *= dy;
	npts = header[NCHAN] * header[NTRAK];
	free(datpt);
	datpt = datp2;
	have_max = 0;
	update_status();
	setarrow();
	return 0;
}
/* ********** */

int block(int n,int index)
/* combine the data into n x m blocks */
{
	DATAWORD *datp,*datp2;
	extern DATAWORD *datpt;
	extern int	doffset;
	DATAWORD idat(int,int);
	
	#ifdef FLOAT
	DATAWORD sum;
	#else
	int sum;
	#endif
	
	int dx,dy,i,j,size,nt,nc,count;
	float fsum,*fdatp;
	extern float *fdatpt;
        
	if( index < 0) {  /* if called from routine other than command decoder */
		dx = n;
		dy = -index;
	} else {
		if(n <= 0) n = 2;
	
		dx = dy = n;	/* the blocking amounts */
	
		/* Check to see if there was a second argument */
	
		for ( i = index; cmnd[i] != EOL; i++) {
			if(cmnd[i] == ' ') { 
				sscanf(&cmnd[index],"%d %d",&dx,&dy);
				break;
			}
		}
	}

	
	size = (header[NCHAN]/dx * header[NTRAK]/dy + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */
	datp2 = datp = malloc(size);
	if(datp == 0) {
		nomemory();
		return 1;
	}
	
	for(nc=0; nc<doffset; nc++) 
		*(datp++) = *(datpt+nc);	/* copy the CCD header */
	
	count = dx*dy;
	
	if( block_ave) {
		for(nt=0; nt<header[NTRAK]/dy*dy; nt+=dy) {
			for(nc=0; nc<header[NCHAN]/dx*dx;nc+=dx){
				sum = 0;
				for(i=0; i<dx; i++) {
					for(j=0; j<dy; j++) {
						sum += idat(nt+j,nc+i);
					}
				}
				*(datp++) = sum/count;
			}
		}
	} else {
                // get floating point image
                new_float_image(header[NCHAN]/dx,header[NTRAK]/dy);
                fdatp = fdatpt;
		for(nt=0; nt<header[NTRAK]/dy*dy; nt+=dy) {
                    for(nc=0; nc<header[NCHAN]/dx*dx;nc+=dx){
                        fsum = 0.0;
                        for(i=0; i<dx; i++) {
                            for(j=0; j<dy; j++) {
                                    fsum += idat(nt+j,nc+i)*trailer[SFACTR];
                            }
                        }
                        *(fdatp++) = fsum;
                    }
		}
	}

	header[NCHAN] /= dx;
	header[NTRAK] /= dy;
	header[NDX] *= dx;
	header[NDY] *= dy;
	npts = header[NCHAN] * header[NTRAK];
	free(datpt);
	datpt = datp2;
	have_max = 0;
        if(  block_ave == 0 ){
            get_float_image();
        }
	update_status();
	setarrow();
	return 0;
}

/* ********** */

int blockrgb(int n,int index)
/* combine rgb data into n x m blocks */
{
	DATAWORD *datp,*datp2,*datp_green,*datp_blue,*pt,*pt_green,*pt_blue;
	extern DATAWORD *datpt;
	extern int	doffset;
	DATAWORD idat(int,int);
	
	DATAWORD sum,sum_green,sum_blue;
	
	int dx,dy,i,j,size,nt,nc,count,nwidth,nheight;
	
	extern float *fdatpt;
        
	if( index < 0) {  /* if called from routine other than command decoder */
		dx = n;
		dy = -index;
	} else {
		if(n <= 0) n = 2;	
		dx = dy = n;	/* the blocking amounts */	
		/* Check to see if there was a second argument */	
		for ( i = index; cmnd[i] != EOL; i++) {
			if(cmnd[i] == ' ') { 
				sscanf(&cmnd[index],"%d %d",&dx,&dy);
				break;
			}
		}
	}
	
	nwidth = header[NCHAN]/dx;
	nheight = header[NTRAK]/3/dy * 3;
	size = (nwidth * nheight + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */
	datp2 = datp = malloc(size);
	if(datp == 0) {
		nomemory();
		return 1;
	}
	
	pt = datpt+doffset;
	pt_green = pt + header[NCHAN]*header[NTRAK]/3;
	pt_blue =  pt_green + header[NCHAN]*header[NTRAK]/3;

	
	for(nc=0; nc<doffset; nc++) 
		*(datp++) = *(datpt+nc);	/* copy the CCD header */
	
	datp_green = datp + nwidth*nheight/3;
	datp_blue =  datp_green + nwidth*nheight/3;

	
	count = dx*dy;
	
	for(nt=0; nt < nheight*dy/3; nt+=dy) {
		for(nc=0; nc < nwidth*dx;nc+=dx){
			sum = sum_green = sum_blue = 0;
			for(i=0; i<dx; i++) {
				for(j=0; j<dy; j++) {
					sum += *(pt + nc+i + header[NCHAN] * (nt+j));
					sum_green += *(pt_green + nc+i + header[NCHAN] * (nt+j));
					sum_blue += *(pt_blue + nc+i + header[NCHAN] * (nt+j));
				}
			}
			if( block_ave){
				*(datp++) = sum/count;
				*(datp_green++) = sum_green/count;
				*(datp_blue++) = sum_blue/count;
			} else {
				*(datp++) = sum;
				*(datp_green++) = sum_green;
				*(datp_blue++) = sum_blue;
			}
		}
	}

	header[NCHAN] = nwidth;
	header[NTRAK] = nheight;
	header[NDX] *= dx;
	header[NDY] *= dy;
	npts = header[NCHAN] * header[NTRAK];
	free(datpt);
	datpt = datp2;
	have_max = 0;
	maxx();
	update_status();
	setarrow();
	return 0;
}

/* ********** */

int blkave(int n)
{

	block_ave = n;
	return 0;

}

/* ********** */

#define PMODE 0666 /* RW */

int savefile(int n,int index)
{
    extern char     *fullname();
    extern char     txt[];
    extern DATAWORD      *datpt;

    int fd,nr,error = 0;
    //char *ptr2,tc;
	//int i;
	//void swap_bytes_routine(char* co, int num, int nb);
	void set_byte_ordering_value();


	maxx();
	set_byte_ordering_value();		// keep this appropriate for this machine
	
	
	if(index >= 0) {		/* if index <0, assume the file is already open */

		strcpy(lastname,&cmnd[index]);	   /* save a copy of the short name for labeling
						      graphics windows */
#ifdef Mac
		//fd = creat(fullname(&cmnd[index],SAVE_DATA));
		fd = creat(fullname(&cmnd[index],SAVE_DATA),PMODE);
#else
		fd = creat(fullname(&cmnd[index],SAVE_DATA),PMODE);
#endif
		strcpy(txt,&cmnd[index]);
	} else {
		fd = n;
	}
	
	//if(swap_bytes)swap_bytes_routine((char*)header,HEADLEN,2);
	 
    if( write(fd,(char*)&header[0],HEADLEN) != HEADLEN) {
		file_error();
		error = -1;
	}
	//if(swap_bytes)swap_bytes_routine((char*)header,HEADLEN,2);
	
	/*  68000 aranges text differently */
	for(nr=0; nr < COMLEN; nr += 2) {
		headcopy[nr+1] = comment[nr];
		headcopy[nr] = comment[nr+1];
	}

    if( write(fd,headcopy,COMLEN)  != COMLEN) {
		file_error();
		error = -2;
	}
	//if(swap_bytes) swap_bytes_routine((char*)trailer,TRAILEN,2);
    if( write(fd,(char*)&trailer[0],TRAILEN)  != TRAILEN) {
		file_error();
		error = -3;
	}
    //if(swap_bytes) swap_bytes_routine((char*)trailer,TRAILEN,2);
#if DATABYTES == 2
	if (detector == SIT) {
		nbyte = (header[NCHAN]*header[NTRAK]) * DATABYTES;
		for(nr=0; nr < nbyte/DATABYTES; nr++)
			*(datpt+nr) = ~(*(datpt+nr));
	}
#endif
	//if(swap_bytes)swap_bytes_routine((char*)datpt, (npts+ doffset) * DATABYTES, DATABYTES);

    if( write(fd,(char*)datpt,(npts+ doffset) * DATABYTES) != (npts+ doffset) * DATABYTES) { 
		file_error();
		error = -4;
	}
	//if(swap_bytes)swap_bytes_routine((char*)datpt, (npts+ doffset) * DATABYTES, DATABYTES);

#if DATABYTES == 2	
	if (detector == SIT ) {
		for(nr=0; nr < nbyte/DATABYTES; nr++)
		*(datpt+nr) = ~(*(datpt+nr));
	}
#endif
    close(fd);
	setdata(txt);
	if(index >= 0) fileflush(txt);	/* for updating directory */
	setarrow();
	return error;
}
/* ********** */
int file_error()
{
	beep();
	printf("File error.\n");
	setarrow();
    return 0;
}
/* ********** */

int 	bigfile_fd;
int 	bigfile_open = 0;
char 	bigfile_name[COMLEN] = {0};

int createfile(int n,int index)
{
        extern char *fullname();
		void set_byte_ordering_value();
        int nr,error = 0;

        maxx();
	
	
	strcpy(lastname,&cmnd[index]);		/* save a copy of the short name for labeling
						   graphics windows */
#ifdef Mac
	//bigfile_fd = creat(fullname(&cmnd[index],SAVE_DATA));
	bigfile_fd = creat(fullname(&cmnd[index],SAVE_DATA),PMODE);
#else
	bigfile_fd = creat(fullname(&cmnd[index],SAVE_DATA),PMODE);
#endif
	strcpy(bigfile_name,&cmnd[index]);

    if( write(bigfile_fd,(char*)&header[0],HEADLEN) != HEADLEN) {
		file_error();
		error = -1;
	}
	
	/*  68000 aranges text differently */
	for(nr=0; nr < COMLEN; nr += 2) {
		headcopy[nr+1] = comment[nr];
		headcopy[nr] = comment[nr+1];
	}

    if( write(bigfile_fd,headcopy,COMLEN)  != COMLEN) {
		file_error();
		error = -2;
	}
	
	set_byte_ordering_value();		// keep this appropriate for this machine
    if( write(bigfile_fd,(char*)&trailer[0],TRAILEN)  != TRAILEN) {
		file_error();
		error = -3;
	}
	
	bigfile_open = 1;
	concatfile();
	setarrow();
	return error;
}

int concatfile()
{
     
	extern DATAWORD      *datpt;

	int nbyte;
		
	if( bigfile_open == 0) {
		beep();
		printf("Use CREATE to Specify a File First.\n");
		return -1;
	}
	
	
#if DATABYTES == 2	
	if (detector == SIT) {
		for(nr=0; nr < header[NCHAN]*header[NTRAK]; nr++)
		*(datpt+nr) = ~(*(datpt+nr));
	}
#endif	
	nbyte = (npts+ doffset) * DATABYTES; 
	nbyte = (nbyte+511)/512*512;
	
        if( write(bigfile_fd,(char*)datpt, nbyte) != nbyte) { 
		file_error();
	}

#if DATABYTES == 2	
	if (detector == SIT ) {
		for(nr=0; nr < header[NCHAN]*header[NTRAK]; nr++)
		*(datpt+nr) = ~(*(datpt+nr));
	}
#endif
	setarrow();
        return 0;
}

/* ********** */

int closefile()
{
    close(bigfile_fd);
	bigfile_open = 0;
	setdata(bigfile_name);
	fileflush(bigfile_name);	/* for updating directory */
	printf("File %s Closed.\n",bigfile_name);
	return 0;

}

/* ********** */

int openfl(int n)
{
	extern int	openflag,fileisopen;
	extern unsigned int fd;
	openflag = n;
	if(openflag == 0 && fileisopen == 1){ 
		close(fd);				/* close anything left over */
		fileisopen = 0;
	}
	return 0;
}

/* ********** */

int	normal_prefix = 1;

int uprefx(int n,int index)		/* force the use of a particular prefix andsuffix */
{

	switch( cmnd[index] ) {
		case 'G':
		case 'g':
			printf("Using 'Get File' Prefixes.\n");
			normal_prefix = 0;
			break;
		case 'S':
		case 's':
			printf("Using 'Save File' Prefixes.\n");
			normal_prefix = -1;
			break;
		default:
			printf("Using Default Prefixes.\n");
			normal_prefix = 1;
			break;
	}
	return 0;

}

/* ********** */

 int two_to_four(DATAWORD* dpt, int num, TWOBYTE scale)
 {
		TWOBYTE* two_byte_ptr;
		int i;
		
		two_byte_ptr = (TWOBYTE*) dpt;
		
		if(scale <= 0) scale = 1;
		for(i=num-1; i>=0; i--){			
			*(dpt+i) = *(two_byte_ptr+i);
			*(dpt+i) *= scale;
		}
		return 0;
}

unsigned int fd,nbyte;
int	open_file_chans;
int	open_file_tracks;

int getfile(int n,int index)
{
    extern char     *fullname();
    extern DATAWORD      *datpt;
	extern int	openflag,fileisopen;
	extern char 	txt[];		     /* the file name will be stored here temporarily */
	
	extern char unit_text[];
	extern int ruler_scale_defined;
	extern float ruler_scale;
	void swap_bytes_routine(char* co, int num, int nb);
	void set_byte_ordering_value();
	int get_byte_swap_value(short);

	TWOBYTE *scpt,tmp_2byte;
	

    extern	unsigned int fd,nbyte;
	unsigned int nr;

	char ch;
	//char *ptr2,tc;
	//int i;

	if(openflag == 0 && fileisopen == 1){ 
		close(fd);				/* close anything left over */
		fileisopen = 0;
	}
	
	if(index >= 0) {	/* if index negative, assume file name is already set */
		strcpy(txt,&cmnd[index]);
	}
	
	if(openflag == 0 || fileisopen == 0) {		/* open file if one isn't open */
	
		strcpy(lastname,txt);			/* save a copy of the short name for labeling
							   graphics windows */
							   
		if( index >=0 ) {
			fullname(txt,GET_DATA);
		}
		
     	if((fd = open(txt,READMODE)) == -1) {
			beep();
			printf("File %s Not Found.\n",txt);
			return(-1);
		}
		fileisopen = 1;
				
       	read(fd,(char*)header,HEADLEN);
       	
        read(fd,comment,COMLEN);
	
	/*  68000 aranges text differently */
		for(nr=0; nr < COMLEN; nr += 2) {
			ch = comment[nr+1];
			comment[nr+1] = comment[nr];
			comment[nr] = ch;
		}

	
        read(fd,(char*)trailer,TRAILEN);
		swap_bytes = get_byte_swap_value(trailer[IDWRDS]);
		if(swap_bytes) {
			swap_bytes_routine((char*)header,HEADLEN,2);
			swap_bytes_routine((char*)trailer,TRAILEN,2);
		}
		set_byte_ordering_value();		// keep this appropriate for this machine

		nbyte = (header[NCHAN]*header[NTRAK]) * DATABYTES;
        
        if(trailer[IS_COLOR] == 1) 
            image_is_color = 1;
        else 
            image_is_color = 0;
		
		if(trailer[RULER_CODE] == MAGIC_NUMBER) {	/* If there was a ruler defined */
			ruler_scale_defined = 1;
			
			scpt = (TWOBYTE*) &ruler_scale;
			if(swap_bytes) {
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
	
		if( detectorspecified == 0) {
			if(nbyte > 110*110*2) { /* assume that big pics are CCD, small ones from a SIT */
				detector = CCD;
				doffset = 80;}
			else {
				detector = SIT;
				doffset = 0;
			}
		}
		nbyte += doffset*DATABYTES; 
		nbyte = (nbyte+511)/512*512;
	
		if(nbyte <= 0 || checkpar()==1) {
			beep();
			printf(" Cannot Read %d Bytes!\n",nbyte);
			printf(" %d Channels & %d Tracks Reset to 1.\n",header[NCHAN],header[NTRAK]);
			header[NCHAN] = header[NTRAK] = npts = 1;
			close(fd);
			fileisopen = 0;
			return(-1);
		}
		open_file_chans = header[NCHAN];
		open_file_tracks = header[NTRAK];
	}
	else {
		header[NCHAN] = open_file_chans;
		header[NTRAK] = open_file_tracks;
		if(checkpar()==1) {
			beep();
			printf(" Memory Needed.\n");
			return(-1);
		}
	}
	/* Read in the actual data */
	
	if(nbyte > data_buffer_size) {
		beep();
		printf("Data Buffer Too Small.\n");
		return(-1);
	}
	
	nr = read(fd,(char*)datpt, nbyte);
	printf("%d Bytes read.\n",nr);
	
	if(nbyte/nr == 2) {
		// this is a 2-byte data file
		// adjust to 4-byte format
		printf("2-byte input file\n");
		if(swap_bytes)  swap_bytes_routine((char*) datpt, nr,2);
		two_to_four(datpt,nr/2,trailer[SFACTR]);
		trailer[SFACTR]=1;
	} else {
		if(swap_bytes) swap_bytes_routine((char*) datpt, nr, DATABYTES);
	}
	
#if DATABYTES == 2	
	if (detector == SIT ) {
		for(nr=0; nr < nbyte/DATABYTES; nr++)
			*(datpt+nr) = ~(*(datpt+nr));
	}
#endif
	if(openflag == 0) {
		close(fd);
		fileisopen = 0;
	}
	setarrow();
	have_max = 0;
	
    // the question is whether or not to trust the min
  	//   and max supplied by the file.  The setting now 
	//   is NOT to trust it.
		   
	//if(header[NMAX] == 0 &&
	//   header[LMAX] == 0 &&
	//   header[NMIN] == 0 &&
	//   header[LMIN] == 0) {
	//   	have_max = 0;
	//}
	maxx();
	update_status();
	return(0);
}
/* ********** */

// get temporary image index.
// routine that returns an index to a temporary image

int numberNamedTempImages = 0;

int temp_image_index (int n,int index, char* name,int define)
{
    int i,j;
    if (index > 0 && name[0] >= 'a' && name[0] <='z') {
        // this is a named temporary image
        // check to see if it already exists
        for(i=0; i< numberNamedTempImages; i++){
            for(j=0; j< strlen(name); j++){
                if( *(name+j) != namedTempImages[i].vname[j])
                    break;
            }
            if( j == strlen(name) && j == strlen(namedTempImages[i].vname)){
                // this is already defined
                return 2*NUM_TEMP_IMAGES+i;
            }
        }
        if( i == numberNamedTempImages && define == 1){	// add a new named temp image to the list
            if(numberNamedTempImages >= 2*NUM_TEMP_IMAGES){	// TOO MANY named temps
                return -1;
            }
            for(j=0; j<= strlen(name); j++)
                namedTempImages[numberNamedTempImages].vname[j] = *(name+j);
            numberNamedTempImages++;
            return 2*NUM_TEMP_IMAGES+numberNamedTempImages-1;
        }
		beep();
		printf("Temporary image %s not defined.\n",name);
		return(-1);
    }
	// if index is too big -> error
	if(n >= NUM_TEMP_IMAGES){
		beep();
		printf("Illegal temporary image number.\n");
		return(-1);
	}
	if(n >= 0) return n;
	// negative temp image numbers from the command line -> error
	if( n<0 && index != -1){
		beep();
		printf("Illegal temporary image number.\n");
		return(-1);
	}
	// otherwise:
	// if called from within oma (index == -1), then allow internal temporary images
	// valid values range from -1 to -NUM_TEMP_IMAGES
	// store these in the range NUM_TEMP_IMAGES to 2*NUM_TEMP_IMAGES-1
	// no checking for out of bounds negative values (from within oma), so be careful in writing the code
	n = 2*NUM_TEMP_IMAGES+n;
	return n;
}
/* ********** */
/*
GTEMP name
	Get temporary image that has been previously saved using STEMP 
*/
int gtemp(int n,int index)
{
	int i;

	n = temp_image_index (n,index,&cmnd[index],0);
	if (n < 0) return -1;
	
	if(temp_dat[n] == 0){
		beep();
		printf("Temporary image %d not defined.\n",n);
		return(-1);
	}
	
	for(i=0;i<HEADLEN/2;i++){
		header[i] = *(temp_header[n]+i);
	}
	for(i=0;i<TRAILEN/2;i++){
		trailer[i] = *(temp_header[n]+i+(512-TRAILEN)/2);
	}
	if(checkpar()==1){
		beep();
		printf("Error getting temp image %d.\n",n);
		return -1;
	}
	for(i=0;i<header[NCHAN]*header[NTRAK]+doffset;i++){
		*(datpt+i) = *(temp_dat[n]+i);
	}
	npts = header[NCHAN]*header[NTRAK];
    
    image_is_color = trailer[IS_COLOR];
    if(image_is_color) have_max = 0;    // we don't store the rgb max currently
    
	maxx();
	update_status();	
	
	return 0;
}

/* ********** */
/*
STEMP name
	Save current image as temporary image with specified name. The name can be 0-9 or
    a text string beginning with a lower case letter. This can be retrieved with GTEMP.
*/
int stemp(int n,int index)
{

	int size,nc;

	extern DATAWORD *datpt;
	extern int	doffset;

	
	n = temp_image_index (n,index,&cmnd[index],1);
	if (n < 0) return -1;
	
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;

	if(temp_dat[n] != 0) free(temp_dat[n]);
	temp_dat[n] = malloc(size);
	if(temp_dat[n] == 0) {
		nomemory();
		return -1;
	}
	
	if(temp_header[n] != 0) free(temp_header[n]);
	temp_header[n] = malloc(512);
	if(temp_header[n] == 0) {
		nomemory();
		return -1;
	}
	
	
	if(!have_max) find_minmax();
	
	for(nc=0; nc<doffset + header[NCHAN] * header[NTRAK]; nc++) 
		*(temp_dat[n]+nc) = *(datpt+nc);								/* copy the data*/
			
	for(nc=0; nc<HEADLEN/2; nc++) 
		 *(temp_header[n]+nc) = *(header+nc);								/* copy the header */
	for(nc=0; nc<TRAILEN/2; nc++) 
		 *(temp_header[n]+nc+(512-TRAILEN)/2) = *(trailer+nc);				/* copy the trailer*/

	return 0;

}
/* ********** */
/*
FTEMP n
	Free memory associated with temporary image n.
*/
int ftemp(int n,int index)
{
    int i;
	n = temp_image_index (n,index,&cmnd[index],0);
	if (n < 0) return -1;
	
	if(temp_header[n] != 0) free(temp_header[n]);
	if(temp_dat[n] != 0) free(temp_dat[n]);
	
	temp_dat[n] = 0;
	temp_header[n] = 0;
    
    if (n >= 2*NUM_TEMP_IMAGES) {
        // this was a named temp
        // remove that name from the list
        n -= 2*NUM_TEMP_IMAGES; // the index of this name
        numberNamedTempImages--;
        for (i = n; i<numberNamedTempImages; i++) {
            namedTempImages[i] = namedTempImages[i+1];
            temp_header[i+2*NUM_TEMP_IMAGES] = temp_header[i+1+2*NUM_TEMP_IMAGES];
            temp_dat[i+2*NUM_TEMP_IMAGES] = temp_dat[i+1+2*NUM_TEMP_IMAGES];
        }
    }
	
	return 0;
}
/* ********** */
/*
 LTEMP 
    list defined temporary images.
 */
int ltemp(int n,int index)
{
    int i,ncolors;
    for (i=0; i<NUM_TEMP_IMAGES; i++) {
        if(temp_header[i] != 0){
            if(*(temp_header[i]+NCOLOR))
                ncolors=3;
            else
                ncolors=1;
            printf("Temp Image %d: %d x %d x %d\n",i,
                   *(temp_header[i]+NCHAN),*(temp_header[i]+NTRAK),ncolors);
        }
    }
    for (i = 0; i<numberNamedTempImages; i++) {
        n = i+2*NUM_TEMP_IMAGES;
        if(*(temp_header[n]+NCOLOR))
            ncolors=3;
        else
            ncolors=1;
        printf("Temp Image %s: %d x %d x %d\n",namedTempImages[i].vname,
               *(temp_header[n]+NCHAN),*(temp_header[n]+NTRAK),ncolors);
    }
    
	return 0;
}
/* ********** */

int addtmp(int n,int index)
{
	int err;
	err = arithtmp(ADD,n,index,&cmnd[index]);
    return err;
}
/* ********** */

int subtmp(int n,int index)
{
	int err;
	err = arithtmp(SUB,n,index,&cmnd[index]);
    return err;
}

/* ********** */

int multmp(int n,int index)
{
	int err;
	err = arithtmp(MUL,n,index,&cmnd[index]);
    return err;
}

/* ********** */

int divtmp(int n,int index)
{
	int err;
	err = arithtmp(DIV,n,index,&cmnd[index]);
    return err;
}


/* ********** */
/*
SWAPBY flag
    The SWAPBY command sets or clears a flag that determines if the GET and SAVE
    commands swap byte order on data read in or saved. This is included for
    cross-platform compatibility between Mac and Intel worlds. The flag should normally
    be set to 0 unless you are migrating data from one platform to another. This only
    affects GET and SAVE -- all other commands that use files leave the byte order alone.
*/
int swapby(int n, int index)
{
	swap_bytes = n;
	return 0;
}

/* ********** */

int getfit(int n,int index)			/* get a file in simple FITS format */
{
	extern char     *fullname();
	extern char 	txt[];		     /* the file name will be stored here temporarily */
		
 	strcpy(txt,&cmnd[index]);
	strcpy(lastname,txt);
	
	fullname(txt,FTS_DATA);		/* add prefix and suffix  */
	
			
	return(read_fit(txt));
}

int read_fit(char* txt)
{
	extern char     *fullname();
	extern DATAWORD      *datpt;
	extern	unsigned int fd,nbyte;
	unsigned int nr;
	DATAWORD *ptr2;
	short *shortpt;
	char buffer[80];
	int nby,n,i;
	
	if((fd = open(txt,READMODE)) == -1) {
		beep();
		printf("File %s Not Found.\n",txt);
		return -1;
	}
	
	read(fd,buffer,80);	/* see if this is simple */
	
	if( buffer[29] != 'T' ) {
		beep();
		printf("Don't recognize this as simple FITS format.\n");
		
		close(fd);
		return -2;
	}
	
	read(fd,buffer,80);	/* bits per pixel */
	sscanf(&buffer[9],"%d",&nby);
	nby /= 8;
	printf("%d Bytes/Pixel\n",nby);
	if (nby > DATABYTES) {
		beep();
		printf("Currently, can not handle %d byte data.\n",nby);
		
		close(fd);
		return -3;
	}
	
	read(fd,buffer,80);	/* # axes */
	sscanf(&buffer[9],"%d",&n);
	if (n != 2) {
		beep();
		printf("Currently, can only read 2D FITS data.\n");
		
		close(fd);
		return -4;
	}
		
	read(fd,buffer,80);	/* # colums */
	sscanf(&buffer[9],"%d",&n);
	header[NCHAN] = n;
	
	read(fd,buffer,80);	/* # rows */
	sscanf(&buffer[9],"%d",&n);
	header[NTRAK] = n;
	
	for( n=0; n<31; n++ ) {	/* skip over the next part of the header -- just don't care for now */
		read(fd,buffer,80);
	}
	
	header[NDX] = header[NDY] = 1;
	header[NX0] = header[NY0] = 0;
	
	nbyte = (header[NCHAN]*header[NTRAK]) * DATABYTES;

	detector = CCD;
	doffset = 80;
					
	trailer[SFACTR] = 1;
	
	if(nbyte <= 0 || checkpar()==1) {
			beep();
			printf(" Cannot Read %d Bytes!\n",nbyte);
			printf(" %d Channels & %d Tracks Reset to 1.\n",header[NCHAN],header[NTRAK]);
			header[NCHAN] = header[NTRAK] = npts = 1;
			close(fd);
			return -5;
	}
	
	/* Read in the actual data */
	
	if(nbyte > data_buffer_size) {
		beep();
		printf("Data Buffer Too Small.\n");
		return -6;
	}
	
	if( nby == 1) {
		ptr2 = datpt+doffset;
		for(i=0; i < header[NCHAN]*header[NTRAK]; )  {
			read(fd,buffer,80);
			for(n=0; n<80; n++) {
				*(ptr2+i++) = buffer[n];
				if( i >=  header[NCHAN]*header[NTRAK]) goto close;
			}
		}
		goto close;
	}
	
	if( nby == 2) {
		shortpt = (short*)buffer;
		ptr2 = datpt+doffset;
		for(i=0; i < header[NCHAN]*header[NTRAK]; ) {
			read(fd,buffer,80);
			for(n=0; n<40; n++) {
				*(ptr2+i++) = *(shortpt+n);
				if( i >=  header[NCHAN]*header[NTRAK]) goto close;
			}
		}
		goto close;
	}
	nr = read(fd,(char*)(datpt+doffset), nbyte);
	ptr2 = datpt+doffset;		/* a copy of the data pointer */
	printf("%d Bytes read.\n",nr);
	
close:	close(fd);
	setarrow();
	
	have_max = 0;
	
	maxx();
	update_status();
	return 0;
}
/* ********** */

/* 
 COLORFLAG [value]
 If an argument is given, the image_is_color flag is set to 0 (flag = 0) or 1 (anything else). If no argument is given, the current value of the color flag is printed. command_return_1 contains the value of the flag.
 */
int colorflag(int n, int index){
	
    extern Variable user_variables[];
	
	if(index == 0){
		printf("Color flag value: %d\n",image_is_color);
	} else {
        if(n == 0){
            image_is_color = 0;
            trailer[IS_COLOR] = 0;
            have_max = 0;
        } else {
            image_is_color = 1;
            trailer[IS_COLOR] = 1;
            have_max = 0;
        }
    }
	user_variables[0].fvalue = user_variables[0].ivalue = image_is_color;
	user_variables[0].is_float = 0;
    
	return 0;
}

/* ********** */

/* ********** */
int dcrawarg(int n, int index){
	
	int next = 0, i;
	extern char txt[];
	
	if(index == 0){
		i = argc;
		argc = 1;
		dcraw_glue(txt,-1);
		argc = i;
		printf("\nCurrent settings are: ");
		for(i=0; i<argc; i++){
			printf("%s ",argv[i]);
		}
		printf("\n");
		return 0;
	}
	
	argc = 0;
	strcpy(dcraw_arg, cmnd);
	argv[argc++] = &dcraw_arg[next];
	for(i=0; i<strlen(cmnd); i++){
		if(cmnd[i] == ' '){
			dcraw_arg[i] = 0;
			next = i+1;
			argv[argc++] = &dcraw_arg[next];
		}
	}
	if(index != -1){
		printf("%d arguments:\n",argc);
		printf("DCRAW command is: %s\n",cmnd);
		
	}
	
	return 0;
}

/* ********** */
int getrgb(int n,int index)			// get a file in RAW format
									// uses Dave Coffin's dcraw routines, modified as little as possible
									// the modified version is in dcrawx.c
{
	extern char     *fullname();
	extern char 	txt[];		     /* the file name will be stored here temporarily */
	extern float	r_scale,g_scale,b_scale;
	
	int err,num_col;
	
 	strcpy(txt,&cmnd[index]);
	strcpy(lastname,txt);
	
	fullname(txt,RAW_DATA);		// add prefix
	
	if(strcmp("jpg",&txt[strlen(txt)-3]) == 0) {
		err = read_jpeg(txt,-1);

		return(err);
	}
	num_col = dcraw_glue(txt,-1);
	if(num_col < OMA_OK) return -1;
	
	have_max = 0;
	maxx();
	if(num_col == 3)
		r_scale=g_scale=b_scale= 1.0;
	update_status();
	return 0;
}
/* ********** */
/*
 GETFILENAMES NameFile
    Open NameFile (currently uses macro prefix and suffix) which contains the names of files.
    The NEXTFILE command is used to open the next file. NameFile is left open until all files 
    have been accessed.
 */


FILE* nameFilePtr = NULL;

int getFileNames(int n,int index)			// open a file containing file names
{
    extern char     *fullname();
    if( nameFilePtr != NULL) fclose(nameFilePtr);
    
    nameFilePtr = fopen(fullname(&cmnd[index],MACROS_DATA),"r");
	if( nameFilePtr != NULL) {
		return OMA_OK;
	}
	else {
		beep();
		printf("Could not open %s\n",&cmnd[index]);
		return OMA_FILE;
	}
}
/* ********** */

/* 
NEXTFILE [prefix]
   Open the next file specified in the FileNames file that was opened with the GetFileNames command.
   If a prefix is specified, that is added to to the name before trying to open the file.
   command_return_1 is the the filename without any prefix and without the extension (last 4 characters)
 */
int nextFile(int n,int index){
    char 	txt[256];          // read the filename in here initially
    char 	fulltxt[512];
    extern Variable user_variables[];
    
    if( nameFilePtr == NULL){
		beep();
		printf("No Names file is open. Use GetFileNames Command.\n");
		return OMA_FILE;
    }
    if(fscanf(nameFilePtr, "%s",txt) == EOF){
        beep();
		printf("All files have been read.\n");
        fclose(nameFilePtr);
        nameFilePtr = NULL;
		return OMA_FILE;
    }
       
    // return the file name without the extension as the first  return value
    
    user_variables[0].fvalue = user_variables[0].ivalue = 0;
	user_variables[0].is_float = -1;
    strncpy( user_variables[0].estring,txt,strlen(txt)-4);
    user_variables[0].estring[strlen(txt)-4] = 0;   // need to end this explicitly
    
    printf("%s\n",user_variables[0].estring);
    
    if(index != 0)
        strcpy(fulltxt, &cmnd[index]);
    else
        fulltxt[0]=0;
    n = strlen(fulltxt);
    
    strcpy(&fulltxt[n], txt);
    printf("%s\n",fulltxt);

    
 	if(strcmp("jpg",&fulltxt[strlen(fulltxt)-3]) == 0) {
		int err = read_jpeg(fulltxt,-1);
		return(err);
	}
    index = 1;
    strcpy(&cmnd[index],fulltxt);
    if(strcmp("dat",&fulltxt[strlen(fulltxt)-3]) == 0) {
		int err = getfile(0,index);
		return(err);
	}
    
    return getrgb(index,index);
    
}
/* ********** */
int getred(int n,int index)			// get a file in RAW format
									// uses Dave Coffin's dcraw routines, modified as little as possible
									// the modified version is in dcrawx.c
{
	extern char     *fullname();
	char 	txt[256];		     /* the file name will be stored here temporarily */
	int err;
    
 	strcpy(txt,&cmnd[index]);
	strcpy(lastname,txt);
	fullname(txt,RAW_DATA);		// add prefix
	
	if(strcmp("jpg",&txt[strlen(txt)-3]) == 0) {
        err = read_jpeg(txt,0);
		return(err);
	}
	
	dcraw_glue(txt,0);
    
    image_is_color = 0;
    trailer[IS_COLOR] = 0;

	
	have_max = 0;
	maxx();
	update_status();
	return 0;
}
/* ********** */
int getgrn(int n,int index)			// get a file in RAW format
									// uses Dave Coffin's dcraw routines, modified as little as possible
									// the modified version is in dcrawx.c
{
	extern char     *fullname();
	extern char 	txt[];		     /* the file name will be stored here temporarily */
	int err;
    	
 	strcpy(txt,&cmnd[index]);
	strcpy(lastname,txt);
	fullname(txt,RAW_DATA);		// add prefix

	if(strcmp("jpg",&txt[strlen(txt)-3]) == 0) {
        err = read_jpeg(txt,1);
		return(err);
	}

	
	dcraw_glue(txt,1);
    
    image_is_color = 0;
    trailer[IS_COLOR] = 0;

	
	have_max = 0;
	maxx();
	update_status();
	return 0;
}
/* ********** */
int getblu(int n,int index)			// get a file in RAW format
									// uses Dave Coffin's dcraw routines, modified as little as possible
									// the modified version is in dcrawx.c
{
	extern char     *fullname();
	extern char 	txt[];		     /* the file name will be stored here temporarily */
	int err;
    	
 	strcpy(txt,&cmnd[index]);
	strcpy(lastname,txt);
	fullname(txt,RAW_DATA);		// add prefix
	
	if(strcmp("jpg",&txt[strlen(txt)-3]) == 0) {
        err = read_jpeg(txt,2);
		return(err);
	}
	
	dcraw_glue(txt,2);
    
    image_is_color = 0;
    trailer[IS_COLOR] = 0;
	
	have_max = 0;
	maxx();
	update_status();
	return 0;
}
/* ********** */
/* 
Create a composite image by appending the data in the specified file 
to that which exists in the current image buffer. The width of the 
specified file must be the same as that of the current image.
*/
int compos(int n,int index)	
{
	extern char     *fullname();
	extern DATAWORD      *datpt;
	extern int	openflag,fileisopen;

	extern	unsigned int fd,nbyte;
	void swap_bytes_routine(char* co, int num, int nb);
	void set_byte_ordering_value();
	int get_byte_swap_value(short);

	
	TWOBYTE head[HEADLEN/2];
	TWOBYTE trail[TRAILEN/2];
	DATAWORD *newpoint;
	char *bypt;
	int nr,origsize;
	
	static TWOBYTE file_scale_factor;
	float x, rescale;
	
	origsize = npts;
	//origsize = header[NTRAK] * header[NCHAN];
	if(openflag == 0 && fileisopen == 1){ 
		close(fd);				/* close anything left over */
		fileisopen = 0;
	}
	
	if(openflag == 0 || fileisopen == 0) {		/* open file if one isn't open */
	
		strcpy(lastname,&cmnd[index]);		/* save a copy of the short name for labeling
							   graphics windows */

		if((fd = open(fullname(&cmnd[index],GET_DATA),READMODE)) == -1) {
			beep();
			printf("File Not Found.\n");
			return -1;
		}
		fileisopen = 1;
				
		read(fd,(char*)head,HEADLEN);
		read(fd,(char*)headcopy,COMLEN);
		read(fd,(char*)trail,TRAILEN);
		swap_bytes = get_byte_swap_value(trail[IDWRDS]);
		if(swap_bytes) {
			swap_bytes_routine((char*)head,HEADLEN,2);
			swap_bytes_routine((char*)trail,TRAILEN,2);
		}
		set_byte_ordering_value();		// keep this appropriate for this machine

		
		file_scale_factor = trail[SFACTR];	/* the file's scale factor */
		if( file_scale_factor < 1 ) file_scale_factor = 1;

		/* the comment buffer stays unchanged */
		
		nbyte = (head[NCHAN]*head[NTRAK]) * DATABYTES;
		nbyte += doffset*DATABYTES; 
		nbyte = (nbyte+511)/512*512;

		if( detectorspecified == 0) {
			if(nbyte > 110*110*2) { /* assume that big pics are CCD, small ones from a SIT */
				detector = CCD;
				doffset = 80;}
			else {
				detector = SIT;
				doffset = 0;
			}
		}

		if(header[NCHAN] != head[NCHAN]) {
			beep();
			printf("Different Image Width.\n");
			close(fd);
			fileisopen = 0;
			return -2;
		}
		header[NTRAK] += head[NTRAK];
	
		if(nbyte <= 0 || checkpar()==1) {
			beep();
			printf(" Cannot Read %d Bytes!\n",nbyte);
			header[NTRAK] -= head[NTRAK];
			npts = origsize;
			close(fd);
			fileisopen = 0;
			return -3;
		}
		open_file_chans = head[NCHAN];
		open_file_tracks = head[NTRAK];
	}
	else {
		header[NTRAK] += open_file_tracks;
		if(checkpar()==1) {
			beep();
			printf(" Memory Needed.\n");
			return -4;
		}
	}
	/* Read in the actual data */
	
	if(nbyte > data_buffer_size) {
		beep();
		printf("Data Buffer Too Small.\n");
		return -5;
	}
	
	
	newpoint = datpt+doffset+origsize;

	if(doffset != 0) {
		read(fd,(char*)(headcopy),doffset*DATABYTES);  // skip the header info assuming this is the correct size
	}

	nr = read(fd,(char*)(newpoint), nbyte);
	printf("%d Bytes read.\n",nr);
	if((nbyte)/nr == 2) {
		// this is a 2-byte data file
		// adjust to 4-byte format
		printf("2-byte input file\n");
		// we skipped over some of the valid data for this 2-byte case
		// have to move things in the data buffer down
		bypt = (char*) newpoint;
		for(nr = head[NCHAN]*head[NTRAK]*2-1; nr >= 0; nr--){
			*(bypt+nr+doffset*2) = *(bypt+nr);
		}
		// now go back and get those pieces that were read in as if they were part of the data offset
		bypt = (char*) newpoint;
		for(nr = 0; nr < doffset*2; nr++){
			*(bypt+nr) = *(headcopy+nr+doffset*2);
		}
		if(swap_bytes) swap_bytes_routine((char*)newpoint,head[NCHAN]*head[NTRAK]*DATABYTES,2);
		two_to_four(newpoint,head[NCHAN]*head[NTRAK],trail[SFACTR]);
		trail[SFACTR]=file_scale_factor=1;
	} else {
		if(swap_bytes) swap_bytes_routine((char*)newpoint,head[NCHAN]*head[NTRAK]*DATABYTES,DATABYTES);
	}
	
#if DATABYTES == 2	
	if (detector == SIT ) {
		for(nr=0; nr < open_file_chans*open_file_tracks; nr++)
			*(datpt+nr+origsize) = ~(*(datpt+nr+origsize));
	}
#endif
	if( trailer[SFACTR] != file_scale_factor ) {
		if( file_scale_factor > trailer[SFACTR]) {
			rescale = (float)trailer[SFACTR]/(float)file_scale_factor;
			printf("%f rescale\n",rescale);
			for(nr=0; nr < origsize; nr++) {
				x = (float)*(datpt+nr+doffset);
				x *= rescale;
				*(datpt+nr+doffset) = x;
			}
			trailer[SFACTR] = file_scale_factor;
		} else {
			rescale = (float)file_scale_factor/(float)trailer[SFACTR];
			for(nr=0; nr < open_file_chans*open_file_tracks; nr++) {
				x = (float)*(datpt+nr+origsize+doffset);
				x *= rescale;
				*(datpt+nr+origsize+doffset) = x;
			}
		}
	}
		
	
	if(openflag == 0) {
		close(fd);
		fileisopen = 0;
	}
	setarrow();
	have_max = 0;	/* There is a smarter way to do this but skip it for now. */
	update_status();
	maxx();
    return 0;
}
/* ********** */

int comtmp(int n,int index)	
/* 
COMTMP n
	Create a composite image by appending the data in the temporary image 
	to that which exists in the current image buffer. The width of the 
	specified file must be the same as that of the current image.
*/
{
        extern char     *fullname();
        extern DATAWORD      *datpt;
		

        extern	unsigned int nbyte;
	
	
	
	DATAWORD *newpoint;
	unsigned int nr,origsize;
	
	static TWOBYTE file_scale_factor;
	float x, rescale;
	
	origsize = npts;

	n = temp_image_index (n,index,&cmnd[index],0);
	if (n < 0) return -1;
	
	if(temp_dat[n] == 0){
		beep();
		printf("Temporary image %d not defined.\n",n);
		return(-1);
	}

	file_scale_factor = *(temp_header[n] + (HEADLEN + COMLEN)/2 + SFACTR);	// the temp image scale factor 
	if( file_scale_factor < 1 ) file_scale_factor = 1;

	/* the comment buffer stays unchanged */
	
	if( detectorspecified == 0) {
		if(nbyte > 110*110*2) { /* assume that big pics are CCD, small ones from a SIT */
			detector = CCD;
			doffset = 80;}
		else {
			detector = SIT;
			doffset = 0;
		}
	}

		
	if(header[NCHAN] != *(temp_header[n]+NCHAN)) {
		beep();
		printf("Different Image Width.\n");
		return -2;
	}
	header[NTRAK] += *(temp_header[n]+NTRAK);

	if(checkpar()==1) {
		beep();
		printf(" Cannot Read %d Bytes!\n",nbyte);
		header[NTRAK] -= *(temp_header[n]+NTRAK);
		npts = origsize;
		return -3;
	}
	open_file_chans = *(temp_header[n]+NCHAN);
	open_file_tracks = *(temp_header[n]+NTRAK);

	/* Read in the actual data */
	
	newpoint = datpt+doffset+origsize;
	for(nr=0;nr<open_file_chans*open_file_tracks; nr++){
		*(newpoint+nr) = *(temp_dat[n]+doffset+nr);
	}
#if DATABYTES == 2	
	if (detector == SIT) {
		for(nr=0; nr < open_file_chans*open_file_tracks; nr++)
			*(datpt+nr+origsize) = ~(*(datpt+nr+origsize));
	}
#endif
	if( trailer[SFACTR] != file_scale_factor ) {
		if( file_scale_factor > trailer[SFACTR]) {
			rescale = (float)trailer[SFACTR]/(float)file_scale_factor;
			printf("%f rescale\n",rescale);
			for(nr=0; nr < origsize; nr++) {
				x = (float)*(datpt+nr+doffset);
				x *= rescale;
				*(datpt+nr+doffset) = x;
			}
			trailer[SFACTR] = file_scale_factor;
		} else {
			rescale = (float)file_scale_factor/(float)trailer[SFACTR];
			for(nr=0; nr < open_file_chans*open_file_tracks; nr++) {
				x = (float)*(datpt+nr+origsize+doffset);
				x *= rescale;
				*(datpt+nr+origsize+doffset) = x;
			}
		}
	}
		
	
	setarrow();
	have_max = 0;	/* There is a smarter way to do this but skip it for now. */
	update_status();
	maxx();
    return 0;
}

#ifdef DOTIFF
//***************************************************
//*** SATIFF - Convert image to 8bit and save as a
//***          Greyscale TIFF file (uses LibTIFF)
//***          P. Kalt (2003) after J.Fielding
//***************************************************
int satiff(int n, int index)
{
	uint32 rowsperstrip = (uint32) -1;
	double resolution = -1;
	unsigned char *buf = NULL,*buf2;
	uint32 row, col;
	tsize_t linebytes;
	uint16 spp;
	TIFF *out;
	uint32 w, h;

	//extern char cmnd[];
	 
    extern DATAWORD *datpt;
    extern char    *fullname();
    extern char txt[];
    int i;

    DATAWORD *datpk;
    DATAWORD		*datpk2,*pt_green,*pt_blue;	// Pointer to buffer
	w = header[NCHAN];
	h = header[NTRAK];
	spp = 1; // grayscale
    if(image_is_color){
        spp=3;
        h /= 3;
    }
	
	for (i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] != EOL) {
			sscanf(&cmnd[index],"%s",txt);
			break;
		}
	}
	
	fullname(txt,TIF_DATA);  //if TIFF_DATA is not on your system use GET_DATA
	printf("Writing TIFF to file: %s\n",txt);
	
	out = TIFFOpen(txt, "w");
	if (out == NULL)
		return (-4);
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH,  w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, spp);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	if(image_is_color)
        TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    else
        TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    
	TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

	linebytes = spp * w;
	buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(out));
	TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
	    TIFFDefaultStripSize(out, rowsperstrip));
	if (resolution > 0) {
		TIFFSetField(out, TIFFTAG_XRESOLUTION, resolution);
		TIFFSetField(out, TIFFTAG_YRESOLUTION, resolution);
		TIFFSetField(out, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	}

	bit8(0,0); // Convert to 8bit greyscale
	
	datpk = datpt +doffset;
    
	datpk2 = datpk;
	pt_green = datpk2 + h*w;
	pt_blue =  pt_green + h*w;

	if (image_is_color) {
        for (row = 0; row < h; row++) {
            buf2 = buf;
            for (col = 0; col < w; col++){
                *buf2++ = (unsigned char)*(datpk++);
                *buf2++ = (unsigned char)*(pt_green++);
                *buf2++ = (unsigned char)*(pt_blue++);
            }
            if (TIFFWriteScanline(out, buf, row, 0) < 0) break;
        }

    } else{
        for (row = 0; row < h; row++) {
            for (col = 0; col < w; col++)
                *(buf+col) = (unsigned char)*(datpk++);
            if (TIFFWriteScanline(out, buf, row, 0) < 0) break;
        }
    }
	
	(void) TIFFClose(out);
	if (buf)
		_TIFFfree(buf);

	return (0);
}
//***************************************************
//*** SATIFFSCALED - Convert image to 16bit and save as a
//***          Color or Greyscale TIFF file (uses LibTIFF)
//***         Uses  min and max for scaling
//***************************************************
int satiffscaled(int n, int index)
{
	uint32 rowsperstrip = (uint32) -1;
	double resolution = -1;
	unsigned short *buf = NULL,*buf2;
	uint32 row, col;
	tsize_t linebytes;
	uint16 spp;
	TIFF *out;
	uint32 w, h;

	//extern char cmnd[];
	 
    extern DATAWORD *datpt;
    DATAWORD		*datpk2,*pt_green,*pt_blue;	// Pointer to buffer
    extern char    *fullname();
    extern char txt[];
    int i,narg=0;

    DATAWORD *datpk,mymin,mymax;
	w = header[NCHAN];
	h = header[NTRAK];
	spp = 1; // grayscale
    
    int bit16(int,int);
    
    if(image_is_color){
        spp=3;
        h /= 3;
    }
	
	for (i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] != EOL) {
			narg = sscanf(&cmnd[index],"%f %f %s",&mymin,&mymax,txt);
			break;
		}
	}
	if(narg != 3) {
		beep();
		printf("Need 3 arguments: min max filename\n");
		return -1;
	}
	
	fullname(txt,TIF_DATA);  //if TIFF_DATA is not on your system use GET_DATA
	printf("Writing TIFF to file: %s\n",txt);
	
	out = TIFFOpen(txt, "w");
	if (out == NULL)
		return (-4);
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH,  w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, spp);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 16);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    if(image_is_color)
        TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    else
        TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

	TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

	linebytes = spp * w;
	buf = (unsigned short *)_TIFFmalloc(TIFFScanlineSize(out));
	TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
	    TIFFDefaultStripSize(out, rowsperstrip));
	if (resolution > 0) {
		TIFFSetField(out, TIFFTAG_XRESOLUTION, resolution);
		TIFFSetField(out, TIFFTAG_YRESOLUTION, resolution);
		TIFFSetField(out, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	}

	bit16(n,index); // Convert to 16 bit greyscale
	
	datpk = datpt + doffset;
	datpk2 = datpk;
	pt_green = datpk2 + h*w;
	pt_blue =  pt_green + h*w;
    
	if (image_is_color) {
        for (row = 0; row < h; row++) {
            buf2 = buf;
            for (col = 0; col < w; col++){
                *buf2++ = (unsigned short)*(datpk++);
                *buf2++ = (unsigned short)*(pt_green++);
                *buf2++ = (unsigned short)*(pt_blue++);
            }
            if (TIFFWriteScanline(out, buf, row, 0) < 0) break;
        }
        
    } else{
        for (row = 0; row < h; row++) {
            for (col = 0; col < w; col++)
                *(buf+col) = (unsigned short)*(datpk++);
            if (TIFFWriteScanline(out, buf, row, 0) < 0) break;
        }
    }
	
	(void) TIFFClose(out);
	if (buf)
		_TIFFfree(buf);

	return (0);
}


//********************************************************
//*** GTIFF - Import a greyscale TIFF file (8 or 16 bit)
//***         uses LibTIFF
//***         P. Kalt (2003) after J.Fielding
//********************************************************
int gtiff(int n,int index)
{
	uint16 		photometric;
	uint16 		compression;
	uint16 		bitspersample;
	uint16 		bytespersample;
	uint16 		config;
	uint32 		rowsperstrip = (uint32) -1;
	uint16 		fillorder = 1;
	
	uint16 		spp;
	TIFF 		*image;
	uint32 		w, h, countbyte;
	uint32 		*bytecounts;

	//extern char 			cmnd[];
    
    extern DATAWORD 		*datpt;
    extern char    			*fullname();
	extern int is_big_endian;
    //extern TWOBYTE  		header[];
    //extern char     		comment[];
    //extern TWOBYTE  		trailer[];
	extern Variable user_variables[];
    char 					tfil[CHPERLN], tempfil[CHPERLN];
    int 					i;
	
	
	
	//****/  V A R I A B L E S - Added by PKALT 8Apr03
	int				size;	// Size (in DATAWORDS) of the OMA image from the TIFF
	DATAWORD		*datpk, *datpk2,*pt_green,*pt_blue;	// Pointer to buffer
	uint8  			*buffer, tempbyte;			// PKALT lets give it a try
	int				narg=0, count;		// Loop counters...
	unsigned long	imageOffset;
	unsigned long	bufferSize;
	unsigned long	result;
	int				stripMax, stripCount;
	int				IFDPageNo;				// Page number for Multiple Images in 1 TIFF file
	tsize_t			stripSize;
	unsigned short		pkDW;
	
	
	// Process the command line arguments
	for (i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] != EOL) {
			narg = sscanf(&cmnd[index],"%s %d",tfil, &IFDPageNo);
			if (narg == 1) IFDPageNo = 0;
			break;
		}
	}
	printf("%d args %s\n",narg,tfil);
	strcpy(tempfil,tfil);
	
	fullname(tfil,TIFF_DATA);  //if TIFF_DATA is not on your system use GET_DATA
	printf("%d args %s\n",narg,tfil);
	
	image = TIFFOpen(tfil, "r");
	if (image == NULL) {
		strcpy(tfil,tempfil);
		fullname(tfil,TIF_DATA);
		image = TIFFOpen(tfil, "r");
		if (image == NULL) {
			image = TIFFOpen(&cmnd[index], "r");
			if (image == NULL) {
				printf("Error Opening TIFF File %s\n", &cmnd[index]);
				return (-1);
			}
		}
	} // First try .tiff and then .tif extensions, then try with just the given file name
	

	//***
	//***  L i b T I F F   C A L L S   T O   G E T F I E L D
	//***
	//**  Move to the Right Directory (For multifile TIFF images)
	
	/* this seems to exclude tiff files from a movie sequence, so take it out
	 
	for (count=0; count < IFDPageNo; count++){
		if (TIFFReadDirectory(image) == 0){
			beep();
			printf("File only has %d IFD's\n", count+1);
			return(-1);
		}
	}
	 */
	// Read the TIFF fields
	TIFFGetField(image, TIFFTAG_PHOTOMETRIC, &photometric);
	
	if (TIFFGetField(image, TIFFTAG_FILLORDER, &fillorder)) {
		if(fillorder != FILLORDER_MSB2LSB) printf("Fill Order is strange!!\n");
	}	
			
	if (TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &bitspersample)) {
		printf("%d bits per sample.\n",bitspersample);
	}
	bytespersample = (short)bitspersample/8;
	//printf("%d bytes per sample.\n",bytespersample);	
	TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &spp);
	if (spp != 1 && spp != 3) {
		printf("Samples per pixel recorded as: %d\nDisregard...\n",spp);
		spp = 1;
	} else {
		printf("%d Samples per pixel\n",spp);
	}

	TIFFGetField(image, TIFFTAG_IMAGEWIDTH,  &w);
	TIFFGetField(image, TIFFTAG_IMAGELENGTH, &h);
	TIFFGetField(image, TIFFTAG_PLANARCONFIG, &config);
	
	if (config != PLANARCONFIG_CONTIG) {
		printf("Can only handle contiguous data packing\n");
	}
	TIFFGetField(image, TIFFTAG_COMPRESSION, &compression);
	
	if (TIFFGetField(image, TIFFTAG_ROWSPERSTRIP, &rowsperstrip)) {
		//printf("%d rows per strip.\n",rowsperstrip);
	}
	
	TIFFGetField(image, TIFFTAG_STRIPBYTECOUNTS, &bytecounts);
	
	// Find the number of strips in the image and their size
	stripSize = TIFFStripSize (image);
	stripMax = TIFFNumberOfStrips (image);
	imageOffset = 0;

	// Allocate the memory for the buffer used in TIFFReadImage
	bufferSize = TIFFNumberOfStrips (image) * stripSize;
	//printf("%d buff size\n",bufferSize);
	if((buffer = (uint8 *) malloc(bufferSize)) == NULL){
		beep();
		printf("Cannot Allocate Memory for TIFF File Read\n");
		return(-1);
	}
	
	// Sort out the OMA impdef variables for the header
	header[NCHAN] = w;
	header[NTRAK] = h*spp;
	header[NDX] = header[NDY] = 1;
	header[NX0] = header[NY0] = 0;
	npts = header[NCHAN]*header[NTRAK];
	trailer[SFACTR] = 1;
    if(spp == 3){
        image_is_color = 1;
        trailer[IS_COLOR] = 1;
    } else {
        image_is_color = 0;
        trailer[IS_COLOR] = 0;
    }

	
	// Allocate memory for the new OMA image (including Headers)
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	datpk = datpk2 = (DATAWORD *)malloc(size);
	if(datpk == 0){
		nomemory();
		return(-1);
	}
	
	// Before we start writing data we must skip over the OMA Data Offset
	datpk2 = datpk + doffset;
	pt_green = datpk2 + h*w;
	pt_blue =  pt_green + h*w;

	// The number of TOTAL bytes remaining to be read (initially the whole lot)
	countbyte = w * h * bytespersample*spp;
	
	//Transfer the image from the LibTIFF to Buffers
	for (stripCount = 0; stripCount<stripMax; stripCount++){
		if((result = TIFFReadEncodedStrip (image, stripCount,
					buffer + imageOffset,
					stripSize)) == -1){
			beep();
			printf("Read error on input strip number %d\n", stripCount);
			return(-1);
		}
		imageOffset += result;
	}

	// Deal with Photometric Interpretations
	
	/*
	if(photometric != PHOTOMETRIC_MINISBLACK){
		// Flip bits
		printf("Fixing the Photometric Interpretation\n");
		for(count = 0; count< bufferSize; count++)
			buffer[count] = ~buffer[count];
	}
	*/
	
	//if (photometric == PHOTOMETRIC_MINISBLACK) printf("Photometric: Min is BLACK\n");
	//if (photometric == PHOTOMETRIC_MINISWHITE) printf("Photometric: Min is WHITE\n");

	// Deal with the FILL Order
	if(fillorder != FILLORDER_MSB2LSB){
		// We need to swap bits - ABCDEFGH becomes HGFEDCBA
		printf("Fixing the Fill Order to Big Endian\n");
		for(count = 0; count<bufferSize; count++){
			tempbyte = 0;
			if(buffer[count] & 128) tempbyte += 1;
			if(buffer[count] & 64) tempbyte += 2;
			if(buffer[count] & 32) tempbyte += 4;
			if(buffer[count] & 16) tempbyte += 8;
			if(buffer[count] & 8) tempbyte += 16;
			if(buffer[count] & 4) tempbyte += 32;
			if(buffer[count] & 2) tempbyte += 64;
			if(buffer[count] & 1) tempbyte += 128;
			buffer[count] = tempbyte;
		}
	}
	
	
	//  Move the Buffer to the image
	for (count = 0; count < npts; count++) {
			if (bytespersample != 1) {
				if(is_big_endian)
					//pkDW =  ((buffer[count*bytespersample]<<8| buffer[count*bytespersample+1]))/2;
					pkDW =  ((buffer[count*bytespersample]<<8| buffer[count*bytespersample+1]));
				else
					pkDW =  ((buffer[count*bytespersample+1]<<8| buffer[count*bytespersample]));
			} else {
				pkDW = (DATAWORD)(buffer[count]);
			}
			if(spp == 3){
				switch (count%3){
					case 0:
						*(datpk2++) = pkDW;
						break;
					case 1:
						*(pt_green++) = pkDW;
						break;
					case 2:
						*(pt_blue++) = pkDW;
						break;
				}
			} else {
				*(datpk2++) = pkDW;
			}
	}

	
	//Clode the TIFF File using the LibTIFF call
	(void) TIFFClose(image);

	// Pass the OMA image to datpt
	free(datpt);
	datpt = datpk;

	// Free up loose memory
	if (buffer) _TIFFfree(buffer);
	have_max = 0;
	
	if( bitspersample == 16){
		//trailer[SFACTR] = 2;
	}
	user_variables[0].fvalue = user_variables[0].ivalue = spp;
	user_variables[0].is_float = 0;
	
	maxx();
	update_status();

	return (0);
}

#endif

/* ********** */


int ghdr(int n,int index){
    
    int				size;	// Size (in DATAWORDS) of the OMA image from the TIFF
	DATAWORD		*datpk, *datpk2,*pt_green,*pt_blue;	// Pointer to buffer
    HDRLoaderResult result;
    int returnOK,count;
    float *hdrptr;
    extern char  *fullname();
    
    returnOK = loadHDR(fullname(&cmnd[index],RAW_DATA), &result);
    
    if (!returnOK) {
        beep();
        printf("Could not load HDR from %s.\n", &cmnd[index]);
        return OMA_FILE;
    }
    
    printf("%d %d width height\n", result.width,result.height);
    
    // Sort out the OMA impdef variables for the header
	header[NCHAN] = result.width;
	header[NTRAK] = result.height*3;
	header[NDX] = header[NDY] = 1;
	header[NX0] = header[NY0] = 0;
	npts = header[NCHAN]*header[NTRAK];
	trailer[SFACTR] = 1;

    image_is_color = 1;
    trailer[IS_COLOR] = 1;
    
    // Allocate memory for the new OMA image (including Headers)
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	datpk = datpk2 = (DATAWORD *)malloc(size);
	if(datpk == 0){
		nomemory();
		return(-1);
	}
	// Before we start writing data we must skip over the OMA Data Offset
	datpk2 = datpk + doffset;
	pt_green = datpk2 + result.width*result.height;
	pt_blue =  pt_green + result.width*result.height;
    hdrptr = result.cols;

    
    for (count = 0; count < npts; count++) {
            switch (count%3){
                case 0:
                    *(datpk2++) = *(hdrptr++);
                    break;
                case 1:
                    *(pt_green++) = *(hdrptr++);
                    break;
                case 2:
                    *(pt_blue++) = *(hdrptr++);
                    break;
            }
	}


    free(result.cols);
	// Pass the OMA image to datpt
	free(datpt);
	datpt = datpk;
    
	have_max = 0;
	
	//user_variables[0].fvalue = user_variables[0].ivalue = spp;
	//user_variables[0].is_float = 0;
	
	maxx();
	update_status();

    return OMA_OK;
}

/* ********** */


//***************************************************
//*** 8BIT - reduce bit depth to 8 bits per pixel
//***************************************************
int bit8(int n, int index)
{
	extern DATAWORD *datpt;
	extern DATAWORD max,min;
	DATAWORD mymax,mymin;

	DATAWORD *datp,range;
	float fpval;
	int i,narg = 0;
	
	datp = datpt+doffset;
	
	if (index !=0 ) {	// look for 2 arguments
		for ( i = index; cmnd[i] != EOL; i++) {
			if(cmnd[i] == ' ') { 
				#ifdef FLOAT
				narg = sscanf(&cmnd[index],"%f %f",&mymin,&mymax);
				#else
				narg = sscanf(&cmnd[index],"%d %d",&mymin,&mymax);
				#endif
				break;
			}
		}
		if (narg != 2) narg = 0;
	}

	if( narg == 0 ) {
		find_minmax();
		range = max - min;
		for(i=0; i<npts; i++){
			fpval = (*datp-min) * 255.0/range;
			*datp++ = fpval;
		}
		have_max = 0;
		maxx();
		return 0;
	} else {
		//find_minmax();
		//if(min < mymin) mymin = min;
		//if(max > mymax) mymax = max;
		range = mymax - mymin;
		for(i=0; i<npts; i++){
			fpval = (*datp-mymin) * 255.0/range;
			if (fpval > 255.) fpval = 255;
			if(fpval < 0.) fpval = 0;
			*datp++ = fpval;
		}
		have_max = 0;
		maxx();
		return 0;
	
	}
}

//***************************************************
//*** 16BIT - reduce bit depth to 16 bits per pixel
//***************************************************
int bit16(int n, int index)
{
	extern DATAWORD *datpt;
	extern DATAWORD max,min;
	DATAWORD mymax,mymin;
    
	DATAWORD *datp,range;
	float fpval;
	int i,narg = 0;
	
	datp = datpt+doffset;
	
	if (index !=0 ) {	// look for 2 arguments
		for ( i = index; cmnd[i] != EOL; i++) {
			if(cmnd[i] == ' ') {
#ifdef FLOAT
				narg = sscanf(&cmnd[index],"%f %f",&mymin,&mymax);
#else
				narg = sscanf(&cmnd[index],"%d %d",&mymin,&mymax);
#endif
				break;
			}
		}
		if (narg != 2) narg = 0;
	}
    
	if( narg == 0 ) {
		find_minmax();
		range = max - min;
		for(i=0; i<npts; i++){
			fpval = (*datp-min) * 65535.0/range;
			*datp++ = fpval;
		}
		have_max = 0;
		maxx();
		return 0;
	} else {
		//find_minmax();
		//if(min < mymin) mymin = min;
		//if(max > mymax) mymax = max;
		range = mymax - mymin;
		for(i=0; i<npts; i++){
			fpval = (*datp-mymin) * 65535.0/range;
			if (fpval > 65535.) fpval = 65535;
			if(fpval < 0.) fpval = 0;
			*datp++ = fpval;
		}
		have_max = 0;
		maxx();
		return 0;
        
	}
}

/* ********** */


int smacro(int n,int index)
{
        extern char  *fullname();
	extern char*	macbuf;

	FILE 	*fp;
	int i;
	
	fp = fopen(fullname(&cmnd[index],MACROS_DATA),"w");
	if( fp != NULL) {
        	for ( i=0; *(macbuf+i) != EOL; i++) {
                	fprintf(fp,"%s\n",macbuf+i);
			for(;*(macbuf+i) != EOL; i++){};
		}
    		fclose(fp);
	
		settext(&cmnd[index]);		/* text file type */
		fileflush(&cmnd[index]);	/* for updating directory */
		return 0;

	}
	else {
		beep();
		printf("error\n");
		return -1;
	}

}

/* ********** */


int gmacro(int n,int index)
{
	extern char *fullname();
	extern char*	macbuf;

	int fd,nread,i;
	
	if(index == -1)
		fd = open(cmnd,READMODE);
	else
		fd = open(fullname(&cmnd[index],MACROS_DATA),READMODE);
	
	if(fd == -1) {
		beep();
		printf("Macro File '%s' Not Found.\n",&cmnd[index]);
		return -1;
	}
	for(i=0; i<MBUFLEN; i++) *(macbuf+i) = 0;	// clear the buffer
	nread = read(fd,macbuf,MBUFLEN);		/* read the largest buffer  */
	printf("%d Bytes Read.\n",nread);
	
	
	/* the format of macro files has changed -- now they are formatted text files */
	/* previously, they were constant length files containing C strings */
	/* this code should read both formats */
	
	for(i=0; i<nread ; i++) {
		if( *(macbuf+i) == 0x0D || *(macbuf+i) == 0x0A)
			*(macbuf+i) = 0x00;	/* change CR or LF to null */
	}
	*(macbuf+nread) = 0;				/* one extra to signify end of buffer */
	*(macbuf+nread+1) = 0;
	
	close(fd);
	clear_macro_to_end();		/* insert trailing zeros after the macro */
	return 0;
}

/* ********** */

int channel(int n)
{

	TWOBYTE temp;
        
        if ( n < 1 || n > dlen) {
                illval();
                return -1;
        }
	temp = header[NCHAN];
        header[NCHAN] = n;
        npts = n * header[NTRAK];
        if ( checkpar() == 1) {
                header[NCHAN] = temp;
	        npts = temp * header[NTRAK];
	}
	have_max = 0;
	update_status();
	return 0;
}

/* ********** */

int tracks(int n)
{

	TWOBYTE temp;
        
        if ( n < 1 || n > dhi) {
                illval();
                return -1;
        }
	temp = header[NTRAK];
        header[NTRAK] = n;
        npts = n * header[NCHAN];
        if ( checkpar() == 1) {		/* if no memory, change it back */
                header[NTRAK] = temp;
	        npts = temp * header[NCHAN];
	}
	have_max = 0;
	update_status();
	return 0;
}

/* ********** */

int setx0(int n)
{

	TWOBYTE temp;
        
        if ( n < 0 || n > dlen) {
                illval();
                return -1;
        }
	temp = header[NX0];
        header[NX0] = n;
        if ( checkpar() == 1 )
                header[NX0] = temp;
	update_status();
	return 0;
       
}

/* ********** */

int sety0(int n)
{

	TWOBYTE temp;
        
        if ( n < 0 || n > dhi) {
                illval();
                return -1;
        }
	temp = header[NY0];
        header[NY0] = n;
        if ( checkpar() == 1)
                header[NY0] = temp;
	update_status();
	return 0;
       
}
/* ********** */

int dx(int n)
{

	TWOBYTE temp;
        
        if ( n < 1 || n > dlen/2) {
                illval();
                return -1;
        }
	temp = header[NDX];
        header[NDX] = n;
        if ( checkpar() == 1)
                header[NDX] = temp;
	update_status();
	return 0;
}
/* ********** */

int dy(int n)
{

	TWOBYTE temp;
        
        if ( n < 1 || n > dhi/2) {
                illval();
                return -1;
        }
	temp = header[NDY];
        header[NDY] = n;
        if ( checkpar() == 1 )
                header[NDY] = temp;
	update_status();
	return 0;
}

/* ********** */

int addfile(int n,int index)
{
	int err;
	err = arithfile(ADD,index);
    return err;
}

/* ********** */

int subfile(int n,int index)
{
	int err;
	err = arithfile(SUB,index);
    return err;
}

/* ********** */

int mulfile(int n,int index)
{
	int err;
	err = arithfile(MUL,index);
    return err;
}

/* ********** */

int divfile(int n,int index)
{
	int err;
	err = arithfile(DIV,index);
	return err;
}

/* ********** */


int wdata(int n)
{
        extern DATAWORD *datpt;

        int i = 0;

        if ( n == 0 )
                n = npts;       

        while ( i < n ) {
                printf(DATAMSG,*(datpt+doffset+i++));
       }
       return 0;
}
/* ********** */
/*
PIXVALUE x y
	Prints the value of the pixel at location x,y (i.e, at column x and row y). The upper left of an image is at 0,0.
	command_return_1 is the value.
*/
int pixvalue(int n, int index)
{
        extern DATAWORD *datpt;
		extern Variable user_variables[];
		DATAWORD interpolate(float xi, float yi);

		float x,y,value;

        if ( sscanf(&cmnd[index],"%f %f",&x,&y) != 2 ){
			beep();
			printf("Need two arguments.\n");
			return -1;
		}
		value = interpolate(x,y);
		printf("%g\n",value);      

		user_variables[0].ivalue = user_variables[0].fvalue = value;
		user_variables[0].is_float = 1;
       return 0;
}

/**********************************************************************/
float sx,sy;		/* the scale factors needed to change coordinates */ 

int maknew(int n,int index)		/* resize the data array */
{
	int size,i,j,chan2,track2;
	DATAWORD *datp,*datp2;
	DATAWORD interpolate(float,float);
	float xi,yi;
	
	extern DATAWORD *datpt;
	extern int	doffset,npts;
		
	
	/* Check to see if there was a second argument */
	
	track2 = chan2 = 128;
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d",&chan2,&track2);
			break;
		}
	}
	
	size = (chan2 * track2 + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */

	/* get memory for the new array */
	
	datp = malloc(size);
	if(datp == 0) {
		nomemory();
		return -1;
	}
	
	
	datp2 = datp + doffset;
	
	sx = (float)(header[NCHAN]-1)/(float)(chan2-1);
	sy = (float)(header[NTRAK]-1)/(float)(track2-1);
	
	for( j=0; j<track2; j++) {
		for( i=0; i<chan2; i++) {
			xi = i * sx;
			yi = j * sy;
			*(datp2++) = interpolate(xi,yi); 
		}
	}
	
	free(datpt);				/* get rid of the old memory */
	datpt = datp;				/* the new array is the real one */
	header[NCHAN] = chan2;
	header[NTRAK] = track2;
	npts = header[NCHAN] * header[NTRAK];
	have_max = 0;
	maxx();
	update_status();
	setarrow();
	return 0;
}

			
DATAWORD interpolate(float xi, float yi)

{
	float z,xf,yf;
	int ix,iy;
	DATAWORD z1,z2,z3,z4,a00,a11,a10,a01,idat(int,int);
	//DATAWORD value,hy,hx;
	ix = xi;
	iy = yi;
	
	if( (ix+1) == header[NCHAN] || (iy+1) == header[NTRAK]) 
		return(idat(iy,ix));
	
	xf = xi - ix;	/* the fraction part */
	yf = yi - iy;
	//		z3-----------z4
	//		|             |
	//		|             |
	//		|             |
	//		|             |
	//		|             |
	//      z1-----------z2
	
	
	z1 = idat(iy,ix);
	z2 = idat(iy,ix+1);
	z3 = idat(iy+1,ix);
	z4 = idat(iy+1,ix+1);
	/*
	hy = idat(iy+1,ix+1);
	
	if( yf <= xf) {
		hx = idat(iy,ix+1);			// lower right triangle 
		z = xf*(hx-z1) + yf*(hy-hx) + z1;
	}
	else {
		hx = idat(iy+1,ix);			// upper left triangle 
		z = xf*(hy-hx) + yf*(hx-z1) + z1;
	}
	*/
	// Bilinear Interpolation

	a00 = z1;
	a10 = z2 - z1;
	a01 = z3 - z1;
	a11 = z1 - z2 - z3 + z4;
	z = a00 + a10*xf + a01*yf + a11*xf*yf;
	
	return(z);
	
}

/* ********** */

float a00,a01,a10,a11;
float b00,b01,b10,b11;
int warp_parameters_defined = 0;

int warpar(int n, int index)		// get image warping information from a text file 
{
	extern char *fullname();
	int err;

	err = loadwarp(&cmnd[index]);	
	return err;
}

int loadwarp(char* name)
{

	FILE *fp;	
	int notfound = 0;

        extern char     *fullname();

	fp = fopen(fullname(name,GET_DATA),"r");
	
	if( fp != NULL) {
            if( fscanf(fp,"%f %f %f %f\n",&a00,&a01,&a10,&a11) != 4) notfound = 1;
            if( fscanf(fp,"%f %f %f %f\n",&b00,&b01,&b10,&b11) != 4) notfound = 1;
            fclose(fp);

            if(notfound == 1) {
                beep();
                printf(" Data Format Problem.\n");
                return(-1);
        }
        printf(" a parameters: %f   %f   %f",a00,a01,a10);
        printf("   %f\n",a11);
        printf(" b parameters: %f   %f   %f",b00,b01,b10);
        printf("   %f\n",b11);
        warp_parameters_defined = 1;
        return(0);
    
	}
	else {
		beep();
		printf(" File Not Found.\n");
		return(-2);
	}
}


/* ********** */

/**************************************************************************************/ 
        /*
        xÕ = a00 + a01x + a10y + a11xy
        yÕ = b00 + b01x + b10y + b11xy
        */
        // warp the image according to parameters specified  with the WARPAR command
int warp(int n,int index)		
{
	int size,i,j,chan2,track2,ix,iy;
	DATAWORD *datp,*datp2,idat(int,int);
	float xi,yi,x,y;
        float xmax,xmin,ymax,ymin,fx,fy,pixval,subpix=0.2;
        float xwarp(float,float),ywarp(float,float);
        
	
	extern DATAWORD *datpt;
	extern int	doffset,npts;
        
        if(!warp_parameters_defined){
            beep();
            printf("Parameters Not Defined -- Use 'WARPAR filename' first.\n");
            return -2;
        }
        
        xmax = ymax = 0;
        xmin = dlen;
        ymin = dhi; 
        
        // look for a floating point parameter
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%f",&subpix);
			break;
		}
	}
		
	printf("Subpixel resolution: %.2f\n", subpix);
	// get the size of the new image
        // look along top row
        y = 0;
        for(x=0; x<header[NCHAN]; x+= subpix){
            xi = xwarp(x,y);
            yi = ywarp(x,y);
            if(xi > xmax) xmax = xi;
            if(xi < xmin) xmin = xi;
            if(yi > ymax) ymax = yi;
            if(yi < ymin) ymin = yi;
        
        }
        // look along bottom row	
        y = header[NTRAK]-1;
        for(x=0; x<header[NCHAN]; x+= subpix){
            xi = xwarp(x,y);
            yi = ywarp(x,y);
            if(xi > xmax) xmax = xi;
            if(xi < xmin) xmin = xi;
            if(yi > ymax) ymax = yi;
            if(yi < ymin) ymin = yi;
        
        }	
        // look along left column	
        x = 0;
        for(y=0; y<header[NTRAK]; y+= subpix){
            xi = xwarp(x,y);
            yi = ywarp(x,y);
            if(xi > xmax) xmax = xi;
            if(xi < xmin) xmin = xi;
            if(yi > ymax) ymax = yi;
            if(yi < ymin) ymin = yi;
        
        }	
        // look along right column	
        x = header[NCHAN]-1;
        for(y=0; y<header[NTRAK]; y+= subpix){
            xi = xwarp(x,y);
            yi = ywarp(x,y);
            if(xi > xmax) xmax = xi;
            if(xi < xmin) xmin = xi;
            if(yi > ymax) ymax = yi;
            if(yi < ymin) ymin = yi;
        
        }
        printf("xmin: %.2f  xmax: %.2f\n", xmin,xmax);
        printf("ymin: %.2f  ymax: %.2f\n", ymin,ymax);
        
        track2 = ymax - ymin + 1.5;
        chan2 = xmax - xmin + 1.5;
		
	size = (chan2 * track2 + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	// make a bit bigger for file reads 

	
	
	//datp = malloc(size);
        datp = calloc(size,1);
	if(datp == 0) {
		nomemory();
		return -1;
	}
	
	datp2 = datp + doffset;
		
	for( y=0; y<header[NTRAK]; y += subpix) {
		for( x=0; x<header[NCHAN]; x += subpix) {
                    i = x;
                    j = y;
                    pixval = idat(j,i);
                    xi = xwarp(x,y);
                    yi = ywarp(x,y);
                    ix = xi;
                    iy = yi;
                    //coordinates of pixel in the new image
                    
                    //put the intensity from this pixel into the (up to 4) pixels that this pixel covers
                    fx = xi - ix;   // the fractions
                    fy = yi - iy;
                    //
                    // remap origin to 0,0
                    xi -= xmin;
                    yi -= ymin;
                    ix = xi;
                    iy = yi;
                    // this pixel
                    *(datp2+ix+iy*chan2) += pixval * (1.0 - fx) * (1.0 - fy);
                    // pixel to the right
                    *(datp2+ix+1+iy*chan2) += pixval * (fx) * (1.0 - fy);
                    // pixel above
                    *(datp2+ix+(iy+1)*chan2) += pixval * (1.0 - fx) * (fy);
                    // pixel diagonally across
                    *(datp2+ix+1+(iy+1)*chan2) += pixval * (fx) * (fy);
                    /*if(i == 100) {
                        printf("%d %d\n",i,j);
                        printf("%d %d\n",ix,iy);
                        printf("%f %f\n",fx,fy);
                         printf("%f %f\n\n",xi,yi);
                    
                    }*/
		}
	}
	
	free(datpt);				/* get rid of the old memory */
	datpt = datp;				/* the new array is the real one */
	header[NCHAN] = chan2;
	header[NTRAK] = track2;
	npts = header[NCHAN] * header[NTRAK];
	have_max = 0;
	maxx();
	update_status();
	
	return 0;
}

float xwarp(float x, float y)
{
    float xi;
    xi = a00 + a01*x + a10*y + a11 * x * y;
    return xi;
}
float ywarp(float x, float y)
{
    float yi;
    yi = b00 + b01*x + b10*y + b11 * x * y;
    return yi;
}


/**************************************************************************************/


/* ********** */
#ifdef FLOAT
DATAWORD *acpoint;
#else
long int *acpoint;
#endif
unsigned int  acsize = 0;
int	accxsize,accysize;	/* used by the pdf commands */
DATAWORD	startvalue = 0;
DATAWORD	binwidth = 1;		

/* ********** */
int accum(int n)		/* create accumulator n */
{
	extern unsigned int acsize;

	int i;
	if(acsize != 0) {
		beep();
		printf("Accumulator already exists.\n");
		return -1;
	}
	
	acsize = header[NCHAN]*header[NTRAK]*LONGBYTES;
	accxsize = header[NCHAN];
	accysize = header[NTRAK];
#ifdef FLOAT
	acpoint = (DATAWORD*) malloc(acsize);
#else
	acpoint = (long int*) malloc(acsize);
#endif
	if(acpoint == 0) {
		nomemory();
		acsize = 0;
		return -1;
	}
	
	for(i=0; i<acsize/LONGBYTES; i++)
		*(acpoint+i) = 0;
	
	return 0;

}
/* ********** */

int accdelete(int n)		/* get rid of accumulator n */
{

	extern unsigned int acsize;
	
	if(acsize == 0)
		return 0;
	
	acsize = 0;
		
	if(acpoint!=0) {
		free(acpoint);	
		acpoint = 0;
	}
	return 0;

}
/* ********** */

int accadd(int n)		/* add data buffer to accumulator n */
{
	extern DATAWORD *datpt;

	extern int doffset;
	int i;
	
	if(acsize == 0) {
		beep();
		pprintf("No accumulator\n");
		return -1;
	}
	if(header[NCHAN]*header[NTRAK] > acsize/LONGBYTES) {
		beep();
		printf("Accumulator Too Small\n");
		return -1;
	}
	for(i=0; i<header[NCHAN]*header[NTRAK]; i++)
		*(acpoint+i) += *(datpt+i+doffset)*trailer[SFACTR];
	return 0;
}

/* ********** */

int accpdf(int n)		/* accumulate a pdf of 2 variables */
/*
	If n>0, consider channel n 
	If n<0, consider track abs(n)
*/
{
	
	int i,j;
	DATAWORD idat(int,int),v;
	
	if(acsize == 0) {
		beep();
		pprintf("No accumulator\n");
		return -1;
	}
	if(header[NCHAN]*header[NTRAK] > acsize/LONGBYTES) {
		beep();
		printf("Accumulator Too Small\n");
		return -1;
	}
	if( n>0 ) {			/* x axis is data value; y axis is pixel; chan n fixed */
		n--;			/* subtract 1 */
		
		for(i=0; i<header[NTRAK]; i++) {
			v = idat(i,n); 	/* this is the data value */			
			if( v < startvalue) 
				v = startvalue;
			if( v > (startvalue + (accxsize-1)*binwidth) ) 
				v = startvalue + (accxsize-1)*binwidth;
			j = (v-startvalue)/binwidth;
			*(acpoint + i*accxsize + j) += 1;
		}
	}
	return 0;
}

/* ********** */
/* 
	Setup for the ACPDF command.
	Must specify:
		Starting Value
		Bin Width
	Note that the number of bins is specified by the size of the 2D array defined when the
	accumulator is defined with the ACCGET command.
*/
	
int pdfset(int n,int index)
{
	int i;
	
	startvalue = n;		/* the default values */
	binwidth = 1;
	
	/* Check to see if there was a second argument */
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			#ifdef FLOAT
			sscanf(&cmnd[index],"%f %f",&startvalue,&binwidth);
			#else
			sscanf(&cmnd[index],"%d %d",&startvalue,&binwidth);
			#endif
			break;
		}
	}
	if(binwidth == 0) 
		binwidth = 1;
	#ifdef FLOAT
		printf("Start Value: %g\nBin Width: %g\nMax Value: %g\n",startvalue,
		binwidth,startvalue + (accxsize-1)*binwidth);
	#else	
		printf("Start Value: %d\nBin Width: %d\nMax Value: %d\n",startvalue,
		binwidth,startvalue + (accxsize-1)*binwidth);
	#endif
	return 0;
}
/* ********** */
/* 
	Setup for the PDF command.
	This clears the array and gets ready for a new one.
	Add to it with the PDF command
	Must specify:
		Starting Value
		Ending value
		number of bins
		

	Command sequence would be
	pdfsiz 0 1000 100	; 100 bins data values from 0 to 1000
	...load an image.....
	pdf 0			; add all points in the current image to the pdf accumulator
	pdf 1			; get the pdf accumulator
	
*/

DATAWORD pdfstart=0;
DATAWORD pdfend =1000;
int pdfbins=100;
int* pdfptr = 0;
float pdfwidth=10.0;
int pdf_exclude_flag = 0;
	
int pdfsiz(int n,int index)
{
	int i,narg;

	
	
	/* Check to see if there was a second argument */
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			#ifdef FLOAT
			narg = sscanf(&cmnd[index],"%f %f %d %d",&pdfstart,&pdfend,&pdfbins,&pdf_exclude_flag);
			#else
			narg = sscanf(&cmnd[index],"%d %d %d %d",&pdfstart,&pdfend,&pdfbins,&pdf_exclude_flag);
			#endif
			if( narg < 3){
				beep();
				printf("Specify startvalue, end value, number of bins, [pdf exclude flag].\n");
				return -1;
			}
			if( narg == 3) pdf_exclude_flag = 0;
			break;
		}
	}
	if(pdfbins <= 0) 
		pdfbins = 1;
		
	if( pdfptr != 0) free(pdfptr);
	pdfptr = (int*) calloc(pdfbins,sizeof(DATAWORD));
	if(pdfptr == 0) {
		nomemory();
		return -1;
	}

		
	pdfwidth = (float)(pdfend-pdfstart)/(float)pdfbins;	
	#ifdef FLOAT
	printf("Start Value: %g\nMax Value: %g\nBin Width: %g\n",pdfstart,pdfend,pdfwidth);
	#else
	printf("Start Value: %d\nMax Value: %d\nBin Width: %.2f\n",pdfstart,pdfend,pdfwidth);
	#endif
	printf("Number of bins: %d\npdf exclude flag: %d\n",pdfbins,pdf_exclude_flag);
	return 0;	
}
/* ********** */

int pdf(int n)		/* add to the pdf accumulator or get the pdf accumulator */
{
	extern DATAWORD *datpt;
	extern int doffset;
	int i,mn,mx,big,sf,iv;
	DATAWORD v;
	float x;
	
	if(pdfptr == 0) {
		beep();
		printf("No pdf accumulator\n");
		return -1;
	}
	
	if(n == 0){
		for(i=0; i<npts; i++) {
			v = *(datpt+doffset+i)*trailer[SFACTR]; 	/* this is the data value */			
			if( v < pdfstart) {
				v = pdfstart;
				if(pdf_exclude_flag) continue;			// don't count pixels out of range
			}
			if( v > pdfend ) {
				v = pdfend;		// count all pixels
				if(pdf_exclude_flag) continue;			// don't count pixels out of range
			}
			x = (float)(v-pdfstart)/(float)pdfwidth;
			iv = x+0.5;
			if(iv >= pdfbins) iv = pdfbins-1;
			if(iv < 0) iv = 0;
			*(pdfptr + iv) += 1;
		}
	
	} else {
		tracks(1);
		channel(pdfbins);
		mn = mx = *(pdfptr);
		for(i=0; i<pdfbins; i++) {
			if( *(pdfptr+i) > mx) mx = *(pdfptr+i);
			if( *(pdfptr+i) < mn) mn = *(pdfptr+i);
		}
		big = (abs(mx) > abs(mn)) ? abs(mx) : abs(mn);
		sf = 1 + (big-1)/MAXDATAVALUE;
		trailer[SFACTR] = sf;
		for(i=0; i<header[NCHAN]*header[NTRAK]; i++) 
			*(datpt+i+doffset) = (*(pdfptr+i))/sf;
		have_max = 0;
		maxx();
	}
	return 0;
}
	
	

/* ********** */

int accget(int n)		/* move accumulator n data to data buffer*/
{
	extern DATAWORD *datpt;
	extern int doffset;
	int i,sf;
	DATAWORD mn,mx,big;
	
	if(acsize == 0) {
		beep();
		pprintf("No accumulator\n");
		return -1;
	}
	mn = mx = *(acpoint);
	for(i=0; i<header[NCHAN]*header[NTRAK]; i++) {
		if( *(acpoint+i) > mx) mx = *(acpoint+i);
		if( *(acpoint+i) < mn) mn = *(acpoint+i);
	}
	big = (abs(mx) > abs(mn)) ? abs(mx) : abs(mn);
	sf = 1 + (big-1)/MAXDATAVALUE;
	trailer[SFACTR] = sf;
	for(i=0; i<header[NCHAN]*header[NTRAK]; i++) 
		*(datpt+i+doffset) = (*(acpoint+i))/sf;
	have_max = 0;
	maxx();
	return 0;
}

/* ********** */
/*
SHELL shell_command
 send a command to a shell
 */
int sys_command(int n,int index)
{
	FILE*	sf;
	extern char     txt[];
	int i,j;
	//system(&cmnd[index]);
	sf = popen(&cmnd[index],"r");
	i = fread(txt,1,255,sf);
	while( i> 0){
		for(j=0; j< i; j++){
			printf("%c",txt[j]);
		}
		i = fread(txt,1,255,sf);
	}
	
	pclose(sf);
	return(0);
	
}	
/* ************* Service Functions for Other Commands ************* */

int getlin(char* array)
{
        int nc;

        nc = 0;          /* get a command line */
		gets(array);
		nc=strlen(array);
        array[nc] = EOL;
        //fprintf(stderr,"getlin %d chars %c\n",nc,array[0]);
        return(nc);     /* nc is the highest index or number of chars - 1 */
}

int nosuch()
{
	beep();
        printf( "No such command: %s\n",cmnd);
        return 0;
}
 
int checkpar()
{
	int error = 0;
	DATAWORD *saveptr;
	extern long data_buffer_size;
	extern int status_window_inited;
	
	if(header[NDX] <= 0) header[NDX] = 1;
	if(header[NDY] <= 0) header[NDY] = 1;
	data_buffer_size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	
	npts = header[NCHAN] * header[NTRAK];	/* 9/21/88 be sure this is updated */

	data_buffer_size = (data_buffer_size+511)/512*512;	/* make a bit bigger for file reads */
	
	if(datpt == 0) {
		datpt = malloc(data_buffer_size);
		saveptr = 0;
		/* this should only happen the first time */
	}
	else {
		saveptr = datpt;
		datpt = realloc(datpt,data_buffer_size);	/*  */
	}
	
	if(datpt == 0) {
		nomemory();
		error = 1;
		datpt = saveptr;	/* put back the old value */
		return(error);
	}
	
	
    if ((header[NX0] + header[NDX] * header[NCHAN]) > dlen) {
        error = 2;
        beep();
        printf("Possible Parameter Mismatch via dlen.\n");
    }
    if ((header[NY0] + header[NDY] * header[NTRAK]) > dhi ) {
        error = 2;
        beep();
        printf("Possible Parameter Mismatch via dhi %d + %d*%d > %d\n", header[NY0], header[NDY], header[NTRAK], dhi);
    }
/*	Take this out - if controller is off, have to wait for a GPIB timeout!
	else {
		omaio(RUN,1);	 send the scan definition to the CCD 
	}
*/
	if(status_window_inited)
		update_status(); 
	return(error);
}

int illval()
{
	beep();
	printf(" Illegal Value.\n");
        return 0;
}

/* ********** */
/*
*/
/* ********** */

int arithtmp(int code,int n,int index,char* name)
{
	int i;
	int filesf;		// Scale factor for file data
	int ramsf;		// Scale factor for data in RAM
	
	n = temp_image_index (n,index,name,0);
	if (n < 0) return -1;
	
	if(temp_dat[n] == 0){
		beep();
		printf("Temporary image %d not defined.\n",n);
		return(-1);
	}


	filesf = *(temp_header[n] + (HEADLEN + COMLEN)/2 + SFACTR);
	if (filesf == 0) filesf = 1;
	
	ramsf = trailer[SFACTR];
	printf(" Temporary Image Scale Factor: %d %d\n",filesf,ramsf);
	
	printf (" %d points expected. %d chans and %d tracks.\n",
		npts,*(temp_header[n]+NCHAN),*(temp_header[n]+NTRAK));	
	if ( npts != *(temp_header[n] + NCHAN) * *(temp_header[n] + NTRAK) ) {
		beep();
		printf(" Images are not the same length.\n");
		return -1;
	}
	
	// have the file data
#ifdef FLOAT
	// the loop over the data
	for(i = doffset; i < npts+doffset; i++ ) {
		switch(code) {
		case ADD:
			    *(datpt+i) += *(temp_dat[n]+i)*filesf;
			break;
		case SUB:
			    *(datpt+i) -= *(temp_dat[n]+i)*filesf;			
				break;
		case MUL:
			    *(datpt+i) *= *(temp_dat[n]+i)*filesf;			
				break;
		case DIV:
			    *(datpt+i) /= *(temp_dat[n]+i)*filesf;		
		}
	}
#else	
	float_image();			// copy the current image into the floating-point buffer
	// the loop over the data
	for(i = 0; i < npts; i++ ) {
		switch(code) {
		case ADD:
			    *(fdatpt+i) += *(temp_dat[n]+i+doffset)*filesf;
			break;
		case SUB:
			    *(fdatpt+i) -= *(temp_dat[n]+i+doffset)*filesf;			
				break;
		case MUL:
			    *(fdatpt+i) *= *(temp_dat[n]+i+doffset)*filesf;			
				break;
		case DIV:
			    *(fdatpt+i) /= *(temp_dat[n]+i+doffset)*filesf;		
		}
	}
	get_float_image();  // get the result
#endif

	have_max = 0;
    maxx();
    return 0;
}

int arithfile(int code,int index)
{
	extern char*     fullname();
	

	int i;
	TWOBYTE *pointer;	// point to things in the header
	TWOBYTE filesf;		// Scale factor for file data
	int ramsf;		// Scale factor for data in RAM
	
	void swap_bytes_routine(char* co, int num, int nb);
	void set_byte_ordering_value();
	int get_byte_swap_value(short);

	
	unsigned int fild;    
	
	DATAWORD *file_datpt;

	// Open the file and read the header 
	
	if((fild = open(fullname(&cmnd[index],GET_DATA),READMODE)) == -1) {
		beep();
		printf("File Not Found.\n");
		return -1;
	}

    read(fild,(char*)headcopy,256*2);

	pointer = (TWOBYTE*)headcopy;	
	swap_bytes = get_byte_swap_value(*(pointer + (HEADLEN + COMLEN)/2 + IDWRDS));
	if(swap_bytes) {
		swap_bytes_routine((char*)headcopy,256*2,2);
	}
	set_byte_ordering_value();		// keep this appropriate for this machine

	filesf = *(pointer + (HEADLEN + COMLEN)/2 + SFACTR);
	
	if (filesf == 0) filesf = 1;
	ramsf = trailer[SFACTR];
	printf(" File Scale Factor: %d %d\n",filesf,ramsf);
	
	printf (" %d points expected. %d chans and %d tracks.\n",
		npts,*(pointer+NCHAN),*(pointer+NTRAK));	
	if ( npts != *(pointer + NCHAN) * *(pointer + NTRAK) ) {
		beep();
		printf(" Files are not the same length.\n");
	    close(fild);
		return -1;
	}
	
	file_datpt = (DATAWORD *)malloc((npts+MAXDOFFSET)*DATABYTES);
	if(file_datpt == 0){
		nomemory();
		return -1;
	}
	// read in the data
	i = read(fild,(char*)file_datpt,(npts+MAXDOFFSET)*DATABYTES);
	// have the file data
		
	if((npts+MAXDOFFSET)*DATABYTES/i == 2) {
		// this is a 2-byte data file
		// adjust to 4-byte format
		printf("2-byte input file\n");
		if(swap_bytes) swap_bytes_routine((char*)file_datpt,(npts+MAXDOFFSET)*DATABYTES,2);
		two_to_four(file_datpt,i/2,filesf);
		filesf = 1;
	} else {
		if(swap_bytes) swap_bytes_routine((char*)file_datpt,(npts+MAXDOFFSET)*DATABYTES,DATABYTES);
	}

#ifdef FLOAT
	// the loop over the data
	for(i = doffset; i < npts+doffset; i++ ) {
		switch(code) {
		case ADD:
			    *(datpt+i) += *(file_datpt+i)*filesf;
			break;
		case SUB:
			    *(datpt+i) -= *(file_datpt+i)*filesf;			
				break;
		case MUL:
			    *(datpt+i) *= *(file_datpt+i)*filesf;			
				break;
		case DIV:
			    *(datpt+i) /= *(file_datpt+i)*filesf;		
		}
	}
#else	
	float_image();			// copy the current image into the floating-point buffer	
	// the loop over the data
	for(i = 0; i < npts; i++ ) {
		switch(code) {
		case ADD:
			    *(fdatpt+i) += *(file_datpt+i+doffset)*filesf;
			break;
		case SUB:
			    *(fdatpt+i) -= *(file_datpt+i+doffset)*filesf;			
				break;
		case MUL:
			    *(fdatpt+i) *= *(file_datpt+i+doffset)*filesf;			
				break;
		case DIV:
			    *(fdatpt+i) /= *(file_datpt+i+doffset)*filesf;		
		}
	}
	get_float_image();  // get the result
#endif	
	free(file_datpt);
	
    close(fild);
	have_max = 0;
    maxx();
    return 0;
}


char* fullname( fnam,  type)
char* fnam;
short type;
{

	char *prefixbuf;		
	char *suffixbuf;
	
	char long_name[CHPERLN];
	
	
	if( have_full_name ) return(fnam);
	
	if( type == GET_DATA || type == SAVE_DATA ) {
		switch(normal_prefix) {
			case 0:
				type = GET_DATA;
				break;
			case -1:
				type = SAVE_DATA;
				break;
			default:
			case 1:
				break;
		}		
	}
	
	switch (type) {
	case GET_DATA:				
		prefixbuf = getprefixbuf;	
		suffixbuf = getsuffixbuf;		
		break;				
	case SETTINGS_DATA:
		prefixbuf = graphicsprefixbuf;		
		suffixbuf = graphicssuffixbuf;	
		break;
		
	case MACROS_DATA:
		prefixbuf = macroprefixbuf;	
		suffixbuf = macrosuffixbuf;
		break;
	case TIFF_DATA:				
		prefixbuf = getprefixbuf;	
		suffixbuf = ".tiff";		
		break;				
	case TIF_DATA:				
		prefixbuf = getprefixbuf;	
		suffixbuf = ".tif";		
		break;				
	case PDF_DATA:				
		prefixbuf = getprefixbuf;	
		suffixbuf = ".pdf";		
		break;				
	case FTS_DATA:				
		prefixbuf = getprefixbuf;	
		suffixbuf = ".fts";		
		break;				
	case RAW_DATA:				
		prefixbuf = getprefixbuf;	
		suffixbuf = "";		
		break;				
	case CSV_DATA:				
		prefixbuf = saveprefixbuf;	
		suffixbuf = ".csv";		
		break;				
	case SAVE_DATA_NO_SUFFIX:
		prefixbuf = saveprefixbuf;		
		suffixbuf = "";
		break;
			
	default:
	case SAVE_DATA:
		prefixbuf = saveprefixbuf;		
		suffixbuf = savesuffixbuf;

	}
 	
	//strncpy(long_name,prefixbuf,CHPERLN);
	strcpy(long_name,prefixbuf);
	
	
	//n = CHPERLN - strlen(prefixbuf)-1;
	//strncat(long_name,fnam,n);		/* add the middle of the file name */
	strcat(long_name,fnam);
    
	//n = CHPERLN - strlen(long_name)-1;
	//strncat(long_name,suffixbuf,n);	/* prefix buf now has entire name */
	strcat(long_name,suffixbuf);
    
	if( (strlen(long_name) + 8) >= CHPERLN) {
	    beep();
	    printf(" File Name Is Too Long!\n"); }
	else 
	    //strncpy(fnam,long_name,CHPERLN);	/* put the full name back in the command line */
	    strcpy(fnam,long_name);
	    //*(prefixbuf+n) = '\0';	/* reset end of string in the prefix */
	return(fnam);
}

/* ********** */

int get_byte_swap_value(short id)
{
	char* cp;
	cp = (char*) &id;
	if( is_big_endian ) {	// running on a PowerPC
		if( *(cp) ==  LITTLE_ENDIAN_CODE && *(cp+1) ==  LITTLE_ENDIAN_CODE)
			return 1;	// must have been saved on an intel machine so have to swap bytes
		else
			return 0;	// must be same kind, leave it alone
	} else {			// running on intel
		if( *(cp) ==  LITTLE_ENDIAN_CODE && *(cp+1) ==  LITTLE_ENDIAN_CODE)
			return 0;	// must have been saved on an intel machine so leave it as is
		else
			return 1;	// must be from a powerPC, have to change it
	}

}

void set_byte_ordering_value()
{
	char* cp;
	cp = (char*) &trailer[IDWRDS];
	if( is_big_endian ) {	// running on a PowerPC
		*(cp) = BIG_ENDIAN_CODE;
		*(cp+1) = BIG_ENDIAN_CODE;
	} else {			// running on intel
		*(cp) = LITTLE_ENDIAN_CODE;
		*(cp+1) = LITTLE_ENDIAN_CODE;
	}
}

/*____________________________________________________________________________*/
