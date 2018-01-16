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


#define DATA_file 0
#define PREFS_file 1
#define MACRO_file 2
#define PICT_file 3
#define PAL_file 4
#define TEXT_file 5
int 	losumx[NUMBER_LINEOUTS];	
extern Rect	loboxes[];
int		numboxes = 1;
int		alignstart = false;
int		alignend = false;
LOLabel	boxlabel[NUMBER_LINEOUTS];
long	startloc[NUMBER_LINEOUTS+1];	/* startloc[i] will be the starting location of the
										ith line out in the big data array that holds all
										line outs.
										*/
long	losize = 0;
Boolean	loshowbox = false;						/* whether or not to show line out boxes */
short	lonormalize = 0;						/* whether or not to normalize line outs */
DATAWORD *lopoint = 0;       					/* line out pointer */


OSErr getfile_dialog( );
OSErr savefile_dialog( );
extern FSSpec		finalFSSpec;		// file specification returned by new nav routines
extern NavReplyRecord		theReply;

#ifdef DO_MACH_O
extern	FSSpec MyFSSpec;
extern	FSRef final_parentFSRef;

extern	char curname[CHPERLN];
extern	SInt16 new_v_ref_num;
extern	SInt32 new_dir_ID;
#endif

int printf();

/* ********** */
int ltsave(int n,int index)
//#pragma unused (n)
{
	extern char cmnd[],*fullname();
	
	savetemplate(&cmnd[index]);	
	return 0;
}


int savetemplate(char* name)
{

	int	i = 0;
	FILE *fp;	
  	
  	char oldname[CHPERLN];
 	short  err=0;

    extern char     *fullname(),txt[];
	    if(name == nil) {
		//HGetVol((unsigned char*)oldname,&v_ref_num,&dir_ID);
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
			fp = fopen(txt,"w");
		} else {
			SysBeep(1);
			return -1;
		}
		
		
	} else {
	
		fp = fopen(fullname(name,SAVE_DATA),"w");
	}
    
	/*
	if(name == nil) {
		where.v = 100;
		where.h = 100;
		sfputfile(&where, "Save Lineout Template:", "Lineout", 0, &reply);
		if (!reply.good) 
  			return;
    	fsdelete((char*)p2cstr(reply.fName), reply.vRefNum);	
    	err = getvol(oldname, &oldvol);
    	err = setvol("", reply.vRefNum);
    	fp = fopen(reply.fName,"w");
	}
	else {
		err = setvol(name,0);
    	err = getvol(oldname, &oldvol);
    	fsdelete(name, oldvol);
    	fp = fopen(fullname(name,SAVE_DATA),"w");
	}
	*/
	if( fp != NULL) {
		fprintf(fp,"OMA Line Out Template\n");
		fprintf(fp,"%d\n",numboxes);

        for ( i=0; i < numboxes; i++ ) {
                fprintf(fp,"%s\n",(char*) &boxlabel[i]);
				fprintf(fp,"%d\t%d\t%d\t%d\t%d\n",loboxes[i].left,
												  loboxes[i].top,
												  loboxes[i].right,
												  loboxes[i].bottom,
												  losumx[i]);
		}
    	fclose(fp);
		if(name == nil) {
			settext(txt);
			//err = flushvol("", reply.vRefNum);
			if(err !=0) {
				printf("Flush Volume Failed: Err = %d.\n",err);
				beep();
			}
		} else {
			settext(name);
			fileflush(name);	// for updating directory 
		}
#ifdef DO_MACH_O
		chdir(oldname);
#endif

	}else {
		beep();
		return -1;
	}
	return 0;
}

/* ********** */

int ltload(int n,int index)
// command to load a line out template 
{
	extern char cmnd[],*fullname();
	int err;
	
	err = loadtemplate(&cmnd[index]);	
	return err;
}

int loadtemplate(char* name)
{

    int	i = 0;
    FILE *fp;	
    
    
    char oldname[CHPERLN];
    short err;
    
    int tmp,l,r,t,b;

    extern char     *fullname(),txt[];

#ifdef DO_MACH_O
	getcwd(oldname,CHPERLN);
#endif
	if(name == nil) {
		err = getfile_dialog(TEXT_file );
		if(err){
			return -1;
		} else {
#ifdef DO_MACH_O
			FSRefMakePath(&final_parentFSRef,(unsigned char*)curname,255);
			chdir(curname);
			//printf("%s\n",curname);
#endif			
			fp = fopen(txt,"r");	
		}
	} else {
		fp = fopen(fullname(name,GET_DATA),"r");
	}

	if( fp != NULL) {
		fread(txt,22,1,fp);		// read the first bit to see if this is a valid template 
		if( memcmp(txt,"OMA Line Out Template\n",22) != 0) {
			printf("Not a Line Out Template.\n");
			beep();
			fclose(fp);
			//err = setvol("", oldvol);
			//HSetVol(NULL,v_ref_num,dir_ID);
#ifdef DO_MACH_O
			chdir(oldname);
#endif
			return(-1);
		}
		fscanf(fp,"%d",&numboxes);
        for ( i=0; i < numboxes; i++ ) {
			fread(txt,1,1,fp);
			for(tmp=0;tmp<LOLAB_LENGTH;tmp++) {
                fread(&(boxlabel[i].name[tmp]),1,1,fp);
				if(boxlabel[i].name[tmp] == '\n')
					break;
			}
			boxlabel[i].name[tmp] = 0x0;
			fscanf(fp,"%d\t%d\t%d\t%d\t%d",&l,&t,&r,&b,&tmp);
			loboxes[i].left = l;
 			loboxes[i].top = t;
			loboxes[i].right = r;
			loboxes[i].bottom = b;
			losumx[i] = tmp;
		}
    	fclose(fp);

#ifdef DO_MACH_O
		chdir(oldname);
#endif
		return(0);

	}
	else {
		beep();
		return(-1);
	}
	
}

/* ********** */

//DATAWORD *lopoint;
unsigned int  loacsize = 0;

int linecalc()
{

	int 		i,j,k,j0;
	
	extern	DATAWORD 	idat(int,int);
	
	startloc[0] = 0;
	
	for(i=1; i<=numboxes; i++) {
		if( losumx[i-1] ) 
			startloc[i] = startloc[i-1] + loboxes[i-1].bottom - loboxes[i-1].top + 1;
		else
			startloc[i] = startloc[i-1] + loboxes[i-1].right - loboxes[i-1].left + 1;
	}
	losize = startloc[numboxes];
	
	loacsize = losize*sizeof(DATAWORD);
	
	if (lopoint != 0) {
		free(lopoint);
	}
	
	lopoint = (DATAWORD*) calloc(losize,sizeof(DATAWORD));
	if(lopoint == 0) {
		nomemory();
		return -1;
	}
	
	for(i=0; i<numboxes; i++) {
		j0 = 0;
		if( losumx[i] ) {
			for(j=loboxes[i].top; j<= loboxes[i].bottom; j++) {
				for(k=loboxes[i].left; k<= loboxes[i].right; k++){
					*(lopoint+startloc[i]+j0) += idat(j,k);
				}
				j0++;
			}
		}
		else {
			for(j=loboxes[i].left; j<= loboxes[i].right; j++) {
				for(k=loboxes[i].top; k<= loboxes[i].bottom; k++){
					*(lopoint+startloc[i]+j0) += idat(k,j);
				}
				j0++;
			}
		}
	}
	return 0;	
}

/* ********** */

int lineget(int n,int index)		/* move line out n data to data buffer*/
{
	extern DATAWORD *lopoint;
	extern DATAWORD *datpt;
	extern TWOBYTE header[],trailer[];
	extern int doffset,have_max;
	extern char cmnd[];
	
	int i,sf,delta;
	DATAWORD mn,mx,multiplier;
	
	/* Try to get two arguments */
	
	multiplier = 1;
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			if( sscanf(&cmnd[index],"%d %f",&sf,&mx) >= 2) {
				n = sf;
				multiplier = mx;
			}
			break;
		}
	}
	
	if(loacsize == 0) {
		beep();
		printf("No Line Out Calculated\nOMA>");
		return -1;
	}
	
	if(n<1 || n > numboxes) {
		printf("Invalid Line Out Number.\nOMA>");
		beep();
		return -1;
	}
	
	mn = mx = *(lopoint+startloc[n-1]);
	for(i=0; i<startloc[n]-startloc[n-1]; i++) {
		if( *(lopoint+i+startloc[n-1]) > mx) mx = *(lopoint+i+startloc[n-1]);
		if( *(lopoint+i+startloc[n-1]) < mn) mn = *(lopoint+i+startloc[n-1]);
	}
	//printf("%f %f min max\n",mn,mx);
	//big = (abs(mx) > abs(mn)) ? abs(mx) : abs(mn);
	
	delta = 1;
	if( lonormalize != 0 ) {
		if(losumx[n-1]) {
			delta = loboxes[n-1].right-loboxes[n-1].left+1;
		} else {
			delta = loboxes[n-1].bottom-loboxes[n-1].top+1;
		}
	}
	
	//big = big * multiplier / delta;
	//sf = 1 + (big-1)/MAXDATAVALUE;
		
	if(losumx[n-1]) {
		header[NCHAN] = 1;
		header[NTRAK] = startloc[n]-startloc[n-1];
	} else {
		header[NTRAK] = 1;
		header[NCHAN] = startloc[n]-startloc[n-1];
	}
	
	printf("LineOut %d; Delta %d; Multiplier %f\n",n,delta,multiplier);
	
	checkpar();
	
	sf = 1;	
	
	trailer[SFACTR] = sf;
	for(i=0; i<startloc[n]-startloc[n-1]; i++) 
		*(datpt+i+doffset) = (*(lopoint+i+startloc[n-1]))*multiplier/delta/sf;

	have_max = 0;
	maxx();
	return 0;
}

/* ********** */

int linestats()		/* print out ave,rms, & centroid of regions specified by lineout template */
{

	int 		i;
			
	for(i=0; i<numboxes; i++) {
		calc(&loboxes[i].top,&loboxes[i].bottom);
	}	
	return 0;
}



/* ********** */

int linenorm(int n)

			/* set the line out normalization flag to specify */
			/* whether or not to normalize line outs by the number of lines summed over */

{
	lonormalize = n;
	return 0;

}


/*_____________________________ Unfinished stuff ________________________*/
/*

linesave(n,index)
int n,index;
{

}
*/

#define PMODE 0666 /* RW */

int savedata(int index,DATAWORD* datpt)
{
    extern char     cmnd[],*fullname(),lastname[];
    extern TWOBYTE  header[];
    extern char     comment[],headcopy[];
    extern TWOBYTE  trailer[];
	extern int		npts,have_max,doffset;
	
       
    int fd,nr;

	have_max = 0;
    maxx();
	
	
	fd = creat(fullname(&cmnd[index],SAVE_DATA),PMODE);
		
	strcpy(lastname,&cmnd[index]);		/* save a copy of the name for labeling
						   					graphics windows */

    if( write(fd,(char*)&header[0],HEADLEN) != HEADLEN) {
		file_error();
	}
	
	/*  68000 aranges text differently */
	for(nr=0; nr < COMLEN; nr += 2) {
		headcopy[nr+1] = comment[nr];
		headcopy[nr] = comment[nr+1];
	}

    if( write(fd,headcopy,COMLEN)  != COMLEN) {
		file_error();
	}

    if( write(fd,(char*)&trailer[0],TRAILEN)  != TRAILEN) {
		file_error();
	}
#if DATABYTES == 2	
	if (detector == SIT) {
		nbyte = (header[NCHAN]*header[NTRAK]) * DATABYTES;
		for(nr=0; nr < nbyte/DATABYTES; nr++)
		*(datpt+nr) = ~(*(datpt+nr));
	}
#endif
    if( write(fd,(char*)datpt,(npts+ doffset) * DATABYTES) != (npts+ doffset) * DATABYTES) { 
		file_error();
	}

#if DATABYTES == 2	
	if (detector == SIT) {
		for(nr=0; nr < nbyte/DATABYTES; nr++)
		*(datpt+nr) = ~(*(datpt+nr));
	}
#endif
        close(fd);
	fileflush(&cmnd[index]);	/* for updating directory */
	setarrow();
	return 0;
}

int linssb(int n,int index)
{
	extern char cmnd[],*fullname(),txt[];
	extern	Point		substart,subend;
	
	Point save_start,save_end;
	int i;
	
	for(i=0; cmnd[index] != ' ' && cmnd[index] != EOL; i++) {
		txt[i] = cmnd[index++];
	}
	txt[i] = EOL;
	index++;
	
	i = sscanf(txt,"%d",&n);
	
	if ( i!= 1 || n <1 || n > numboxes) {
		beep();
		printf("Illegal Arguments.\n");
		return -1;
	}
	
	save_start = substart;
	save_end = subend;
	i = n-1;
	substart.h = loboxes[i].left;
	substart.v = loboxes[i].top;
	subend.h = loboxes[i].right;
	subend.v = loboxes[i].bottom;
	
	saverectangle(fullname(&cmnd[index]),SAVE_DATA);
	setdata(&cmnd[index]);
	fileflush(&cmnd[index]);	/* for updating directory */

	substart = save_start;
	subend = save_end;
	return 0;
}
