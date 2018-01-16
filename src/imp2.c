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
#include "SDK.h"

extern DATAWORD *datpt;
extern TWOBYTE 	header[],trailer[];
extern float *fdatpt;
extern char 	comment[];
extern int 		doffset;
extern int     	maxint;		/* the max count CC200 can store */
extern int		npts;		/* number of data points */
extern int		have_max;	/* indicates whether or not the minimum
				   				and maximum value have been found for the data
				   				in the current buffer. 0 -> no;  1 -> yes */
extern char    cmnd[];  	/* the command buffer */
extern int	   dlen,dhi;
extern short     image_is_color;

int printf(), pprintf(),get_float_image(),float_image();
DATAWORD idat(int,int);

/* ********** */
/*
FINDBAD Counts

Searches the current image buffer for pixels whose value is more than "Counts" above that of it's
nearest eight neighbors. Those pixels are tagged as hot pixels.
 
*/

int hot_pix[NUMHOT];	// store info on hot pixels
int num_hot = 0;
int ccd_width = 0;
int ccd_height = 0;

int findbad(int n, int index){
	int i,j;
	DATAWORD ave_val;
	num_hot = 0;
	ccd_width = header[NCHAN];
	ccd_height = header[NTRAK];
	if( n == 0) n = 200;		// a reasonable default value
	for(i=0; i< header[NTRAK]; i++){
		for(j = 0; j< header[NCHAN]; j++) {
			ave_val = ( idat(i-1,j-1) + idat(i-1,j) + idat(i-1,j+1) +
			            idat(i,j-1) 				  + idat(i,j+1) +
					    idat(i+1,j-1) + idat(i+1,j) + idat(i+1,j+1) ) / 8;
			if( idat(i,j) - ave_val > n ){
				if(num_hot < NUMHOT){
					hot_pix[num_hot++] = i*header[NCHAN] + j;
					if(num_hot <= 10) printf(" %d\t%d\n",j,i);
				}
			}
		}
	}
	printf("%d hot pixels found\n",num_hot);
	if(num_hot > 10) printf("First 10 printed\n");
	if(header[NX0] != 0 || header[NY0] != 0){
		beep();
		printf("Warning! FINDBAD is designed to operate on a full frame.\n");
	}

	return 0;
	
}
/*
 READBAD filename
 Read in bad pixel data from a text file.
*/
 
 int readbad(int n, int index)			/* read bad pixel data */
{
	int	i,j,k;
	FILE *fp;
	
    extern char     cmnd[],*fullname();	
	
	fp = fopen(fullname(&cmnd[index],GET_DATA),"r");
	if( fp != NULL) {
		
		fscanf(fp,"%d",&num_hot);
		fscanf(fp,"%d %d",&ccd_width, &ccd_height);
		for(k = 0; k < num_hot; k++){
			fscanf(fp,"%d %d",&j,&i);
			hot_pix[k] = i*ccd_width + j;
		}	
	} else {
		beep();
		printf("Could not open file %s\n",&cmnd[index]);
		return -1;
	}
	fclose(fp);
	return 0;
}

/*
 WRITEBAD filename
Write bad pixel data to a text file.
 */

int writebad(int n, int index)			/* read bad pixel data */
{
    int	k;
    FILE *fp;
    
    extern char     cmnd[],*fullname();	
    
    if(num_hot == 0){
	beep();
	printf("No bad pixels found. Use FINDBAD Counts\n");
	return -1;
    }
	
    fp = fopen(fullname(&cmnd[index],SAVE_DATA),"w");
    if( fp != NULL) {
	
	fprintf(fp,"%d\n",num_hot);
	fprintf(fp,"%d\t%d\n",ccd_width, ccd_height);
	for(k = 0; k < num_hot; k++){
	    fprintf(fp,"%d\t%d\n",hot_pix[k]%ccd_width,hot_pix[k]/ccd_width);
	}	
    } else {
	beep();
	printf("Could not open file %s\n",&cmnd[index]);
	return -1;
    }
    fclose(fp);
    return 0;
}

/*
CLEARBAD
 
 Sets pixels tagged as bad to the value of their 8 nearest neighbors. This will not work well if
 there are contiguous bad pixels. For that, consider using the FILBOX or FILMSK commands.
 
 readbad /volumes/in/impx-s/sbig/sbig_bad_pix
 
 */

int clearbad(int n, int index)
{
	int i,j,k;
	DATAWORD new_val;
	
	for(k=0; k<num_hot; k++){
		i = hot_pix[k]/ccd_width;
		j = hot_pix[k] - i*ccd_width - header[NX0];
		i -= header[NY0];
		//printf(" %d %d\n",j,i);
		if(i < header[NTRAK] && j < header[NCHAN] && i >= 0 && j >= 0) {
			new_val = ( idat(i-1,j-1) + idat(i-1,j) + idat(i-1,j+1) +
					   idat(i,j-1) 				  + idat(i,j+1) +
					   idat(i+1,j-1) + idat(i+1,j) + idat(i+1,j+1) ) / 8;
			*(datpt+doffset+j+header[NCHAN]*i) = new_val;
		}
	}
	have_max = 0;
	maxx();
	return 0;
}

/* ********** */

DATAWORD *tDacpoint;
unsigned int  tDacsize = 0;
int	tDaccxsize,tDaccysize,tDslices,tDslicesize;	

char scratch[CHPERLN];		/* a gerneal purpose scratch text buffer */



/* ********** */
int tDstore(int n)		/* store image as one slice of a 3d array */
{
	extern unsigned int tDacsize;

	int i;
	DATAWORD  *tmppt;
	
	if(tDacsize == 0) {		/* create a new 3D array if one does not exist */
		tDslicesize = tDacsize = header[NCHAN]*header[NTRAK];
		tDaccxsize = header[NCHAN];
		tDaccysize = header[NTRAK];
		tDslices = 1;
		
		tDacpoint = (DATAWORD*) malloc(tDacsize*DATABYTES);

		if(tDacpoint == 0) {
			nomemory();
			tDacsize = 0;
			return -3;
		}
	} else {
		if( (tDaccxsize != header[NCHAN]) || (tDaccysize != header[NTRAK]) ) {
			beep();
			printf("Image sizes are different. Current 3D slice is %d by %d.\n",tDaccxsize,tDaccysize);
			return -1;
		}
		tDacsize += tDslicesize;
		tmppt = (DATAWORD*) realloc(tDacpoint,tDacsize*DATABYTES);
		if(tmppt == 0) {
			nomemory();
			return -2;		
		}
		tDacpoint = tmppt;
		tDslices++;		
	}
	
	for(i=0; i<tDslicesize; i++)
		*(tDacpoint + i + (tDslices-1)*tDslicesize) = *(datpt+i+doffset);
	printf("Current 3D array is %d by %d by %d.\n",tDaccxsize,tDaccysize,tDslices );
	return 0;
}

/* ********** */

int tDdelete(int n)		/* get rid of 3D array */
#ifdef Mac
//#pragma unused (n)
#endif
{
	extern unsigned int tDacsize;
	
	if(tDacsize == 0)
		return 0;
	
	tDacsize = 0;
		
	if(tDacpoint!=0) {
		free(tDacpoint);	
		tDacpoint = 0;
	}
	return 0;
}

/* ********** */

int tDyz(int n)		/* get a y-z slice of the 3D array */
{
	int temp1,temp2,nc,nt;
	DATAWORD *datp,i3dat();
	
	if(tDacsize == 0) {	
		beep();
		printf("No 3D array exists.\n");
		return -1;
	}
	if( --n < 0) n = 0;
	if( n >= tDaccxsize) {
		beep();
		printf("Illegal Argument.\n");
		return -2;
	}
	
	temp1 = header[NCHAN];
	temp2 = header[NTRAK];

	header[NCHAN] = tDslices;
	header[NTRAK] = tDaccysize;
	
	if(checkpar() == 1) {
		header[NCHAN] = temp1;
		header[NTRAK] = temp2;
		return -3;
	}
	datp = datpt+doffset;
	
	for(nt=0; nt<tDaccysize; nt++){
		for(nc = 0; nc<tDslices; nc++) {
			*(datp++) = i3dat(nc,nt,n);
		}
	}
	have_max = 0;
	return 0;
}
/* ********** */

int tDxz(int n)		/* get an x-z slice of the 3D array */
{
	int temp1,temp2,nc,nt;
	DATAWORD *datp,i3dat();
	
	if(tDacsize == 0) {	
		beep();
		printf("No 3D array exists.\n");
		return -1;
	}
	if( --n < 0) n = 0;
	if( n >= tDaccysize) {
		beep();
		printf("Illegal Argument.\n");
		return -2;
	}
	
	temp1 = header[NCHAN];
	temp2 = header[NTRAK];

	header[NCHAN] = tDaccxsize;
	header[NTRAK] = tDslices;
	
	if(checkpar() == 1) {
		header[NCHAN] = temp1;
		header[NTRAK] = temp2;
		return -3;
	}
	datp = datpt+doffset;
	
	for(nt=0; nt<tDslices; nt++){
		for(nc = 0; nc<tDaccxsize; nc++) {
			*(datp++) = i3dat(nt,n,nc);
		}
	}
	have_max = 0;
	return 0;
}


/* ********** */

#define PMODE 0644 /* RW */

int tDsavefile(int n, int index)
{
    extern char     cmnd[],*fullname();
    extern char     headcopy[];

    int fd,x,y,z,err = 0;
	DATAWORD *datp;
		
	if(tDacsize == 0) {	
		printf("No 3D array exists.\n");
		beep();
		return -1;
	}

	printf("Current 3D array is %d by %d by %d.\n",tDaccxsize,tDaccysize,tDslices );
	if( tDaccxsize>256 || tDaccysize>256 || tDslices>256 ) {
		printf("Maximum size is 256 x 256 x 256.\n");
		beep();
		return -2;
	}

	
	
	fd = creat(fullname(&cmnd[index],SAVE_DATA),PMODE);
	strcpy(scratch,&cmnd[index]);


	headcopy[0] = tDaccxsize;
	headcopy[1] = tDaccysize;
	headcopy[2] = tDslices;

    if( write(fd,headcopy,3) != 3) {
		file_error();
		err = -4;
	}

	datp = tDacpoint;

	for(z=0; z<tDslices; z++) {
		for(y=0; y<tDaccysize; y++) {
			for(x=0; x<tDaccxsize; x++) {
				headcopy[x] = *(datp++);
			}
			if( write(fd,headcopy,tDaccxsize) != tDaccxsize) { 
				file_error();
				err = -3;
			}
		}
	}
    close(fd);
	setdata(scratch);
	if(index >= 0) fileflush(scratch);	/* for updating directory */
	setarrow();
	return err;
}



/* ********** */

DATAWORD i3dat(ns,nt,nc)

int ns,nt,nc;

{
	DATAWORD *pt;

	pt = tDacpoint + nc + nt*tDaccxsize + ns*tDslicesize;
	return(*pt);

}

/* ********** */

int plus(int n,int index)
{
	extern DATAWORD *datpt;
	
#ifdef FLOAT
	float x;
	int i;
	if( sscanf(&cmnd[index],"%f",&x) != 1)
		x = n;

	i = doffset;
	while ( i < npts+doffset) {
              *(datpt+i) += x;
              i++;
	}
#else
	extern TWOBYTE trailer[];
	extern DATAWORD max,min;
	int oldsf,newsf,i;

	/* Work out (positive) scale factors */
	oldsf = trailer[SFACTR];
	newsf = (max*oldsf+n)/maxint;
	i = (min*oldsf+n)/maxint;
	if (newsf < 0 ) newsf = -newsf;
	if (i < 0 ) i = -i;
	if (i > newsf) newsf = i;
	newsf++;

        i = 0;
        while ( i < npts) {
              *(datpt+i+doffset) = (*(datpt+i+doffset)*oldsf+n)/newsf;
              i++;
        }
	trailer[SFACTR]=newsf;
#endif
    maxx();			/* the location of the scale factor will be the
				   same, so if we already have it, we shouldn't have
				   to recalculate. */
				   
	return 0;
}


/* ********** */

int setpositive(int n)
{
	extern DATAWORD *datpt;
	int i;

        i = 0;
        while ( i < npts) {
              if(*(datpt+i+doffset) < 0)
	      	  *(datpt+i+doffset) = 0;
	      i++;
        }
	have_max = 0;
    maxx();
    return 0;
}


/* ********** */

int clip(int n, int index)
{
	extern DATAWORD *datpt;
	int i;
	#ifdef FLOAT
	float x;
	if( sscanf(&cmnd[index],"%f",&x) != 1)
		x = n;
	i = doffset;
	while ( i < npts+doffset) {
		  if(*(datpt+i) > x) *(datpt+i) = x;
	  i++;
	}
	#else
	i = doffset;
	while ( i < npts+doffset) {
		  if(*(datpt+i) > n) *(datpt+i) = n;
	  i++;
	}
	#endif
	have_max = 0;
    maxx();
    return 0;
}
/* ********** */

int clipfbottom(int n, int index)
{
	extern DATAWORD *datpt,min,max;
	int i;
#ifdef FLOAT
	float x;
	if( sscanf(&cmnd[index],"%f",&x) != 1)
		x = 0.1;
	i = doffset;
	x = min+(max - min)*x;
	while ( i < npts+doffset) {
		if(*(datpt+i) < x) *(datpt+i) = x;
		i++;
	}
#else
	i = doffset;
	while ( i < npts+doffset) {
		if(*(datpt+i) > n) *(datpt+i) = n;
		i++;
	}
#endif
	have_max = 0;
    maxx();
    return 0;
}
/* ********** */

int clipbottom(int n, int index)
{
	extern DATAWORD *datpt;
	int i;
#ifdef FLOAT
	float x;
	if( sscanf(&cmnd[index],"%f",&x) != 1)
		x = n;
	i = doffset;
	while ( i < npts+doffset) {
		if(*(datpt+i) < x) *(datpt+i) = x;
		i++;
	}
#else
	i = doffset;
	while ( i < npts+doffset) {
		if(*(datpt+i) > n) *(datpt+i) = n;
		i++;
	}
#endif
	have_max = 0;
    maxx();
    return 0;
}
/* ********** */

int clipfr(int n, int index)		/* CLIPFR fract
					   Clip data at a fraction of the maximum value. 
					   "fract" is a real arguement */
{
	extern DATAWORD *datpt,max;
	int i;
	float x;
	DATAWORD val;

	if( sscanf(&cmnd[index],"%f",&x) != 1)
		x = n;

	val = x * max;
	printf("Clip at %d.\n",val);
    i = 0;
    while ( i < npts) {
      if(*(datpt+i+doffset) > val)
	      *(datpt+i+doffset) = val;
	  i++;
    }
	have_max = 0;
    maxx();
    return 0;
}
/* ********** */

int absol()				/* take the absolute value of the data */

{
	extern DATAWORD *datpt;
	int i;

        i = 0;
        while ( i < npts) {
		#ifdef FLOAT
			*(datpt+i+doffset) = fabs(*(datpt+i+doffset));
		#else
			*(datpt+i+doffset) = abs(*(datpt+i+doffset));
		#endif
	      	i++;
        }
	have_max = 0;
    maxx();
    return 0;
}

/* ********** */

int power(int n, int index)				/* raise the data to a power */
{
#ifdef FLOAT
	extern DATAWORD *datpt;
	int i;
	float x,y;
	
	if( sscanf(&cmnd[index],"%f",&y) != 1)
		y = n;

	if(	have_max == 0) maxx();
    i = doffset;
    while ( i < npts+doffset) {
		x = *(datpt+i);
		*(datpt+i) = pow(x,y);
	      i++;
    }
#else
	extern DATAWORD *datpt;
	DATAWORD max,min;
	int i,newsf=1;
	float x,y,fmin,fmax;
	
	if( sscanf(&cmnd[index],"%f",&y) != 1)
		y = n;

	if(	have_max == 0) maxx();
    
	fmin = min*trailer[SFACTR];
	fmax = max*trailer[SFACTR];
	if(y>0) {
		fmin = pow(fmin,y);
		fmax = pow(fmax,y);		/* the new maximum */
		newsf = fmax/maxint;
		i = fmin/maxint;
		if (newsf < 0 ) newsf = -newsf;
		if (i < 0 ) i = -i;
		if (i > newsf) newsf = i;
		newsf++;
	} else {	/* handle negative exponent case */
	
	}
    i = doffset;
    while ( i < npts+doffset) {
		x = *(datpt+i)*trailer[SFACTR];
		*(datpt+i) = pow(x,y)/(float)newsf;
	      i++;
    }
	trailer[SFACTR] = newsf;

#endif
	have_max = 0;
    maxx();
	return 0;
}


/* ********** */
int exponent(int n, int index)				// EXP scale_factor denom 
											// result is sf1*exp(data/sf2) 
{
	#ifdef FLOAT
	extern DATAWORD *datpt;
	int i;
	float sf1,sf2,x;
	
	if( sscanf(&cmnd[index],"%f %f",&sf1,&sf2) != 2) {
		beep();
		printf("Need 2 arguments:  EXP scale_factor denom\n");
		return -1;
	}

	if(	have_max == 0) maxx();
	i = doffset;
    while ( i < npts+doffset) {
		x = *(datpt+i);
		*(datpt+i) = sf1*exp(x/sf2);
	      i++;
    }
	#else
	extern DATAWORD *datpt,max,min;
	int i,newsf;
	float sf1,sf2,fmin,fmax,x;
	
	if( sscanf(&cmnd[index],"%f %f",&sf1,&sf2) != 2) {
		beep();
		printf("Need 2 arguments:  EXP scale_factor denom\n");
		return -1;
	}

	if(	have_max == 0) maxx();
    
	fmin = min*trailer[SFACTR];
	fmax = max*trailer[SFACTR];
	
	fmin = sf1*exp(fmin/sf2);
	fmax = sf1*exp(fmax/sf2);		/* the new maximum */
	newsf = fmax/maxint;
	i = fmin/maxint;
	if (newsf < 0 ) newsf = -newsf;
	if (i < 0 ) i = -i;
	if (i > newsf) newsf = i;
	newsf++;
	
    i = 0;
    while ( i < npts) {
		x = *(datpt+i+doffset)*trailer[SFACTR];
		*(datpt+i+doffset) = sf1*exp(x/sf2)/(float)newsf;
	      i++;
    }
	trailer[SFACTR] = newsf;
	#endif
	have_max = 0;
    maxx();
	return 0;
}


/* ********** */

int ln(int n, int index)				/* take the log of the data. Scale by scale factor */
{

	int i;
	float x,scale;
	
	if( sscanf(&cmnd[index],"%f",&scale) != 1)
		scale = n;
	if(scale == 0.0) scale = 1.0;

	#ifdef FLOAT
    for ( i=doffset; i < npts+doffset; i++) {
			x = *(datpt+i);
			x = log(x)*scale;
			*(datpt+i) = x;
    }
	#else
    i = 0;
	float_image();
    while ( i < npts) {
			x = *(fdatpt+i);
			x = log(x)*scale;
			*(fdatpt+i) = x;
	      	i++;
    }
	get_float_image();
	#endif
	have_max = 0;
    maxx();
	setarrow();
	return 0;
}


/* ********** */

int square()				/* square the data */
{
	extern DATAWORD *datpt;
	int i;

        i = doffset;
        while ( i < npts+doffset) {
			*(datpt+i) = (*(datpt+i))*(*(datpt+i));
	      	i++;
        }
	have_max = 0;
    maxx();
    return 0;
}
/* ********** */

int square_root()				/* square root of the data */
{
	extern DATAWORD *datpt;
	int i;
	float x;

        i = 0;
        while ( i < npts) {
			x = *(datpt+i+doffset);
			*(datpt+i+doffset) = sqrt(x);
	      	i++;
        }
	have_max = 0;
    maxx();
    return 0;
}
/* ********** */

//diffy()				/* differentiate the data in the y direction */

/*

{
	extern DATAWORD *datpt;
	int nc,nt;
	DATAWORD idat(int,int),*datp;
	
	datp = datpt + doffset;
	
	for(nt=0; nt<header[NTRAK]-1;nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			*(datp++) = idat(nt,nc) - idat(nt+1,nc);
		}
	}
	for(nc=0;nc < header[NCHAN]; nc++){
		*(datp++) = 0;
	}
	have_max = 0;
        maxx();
}
*/

/* ********** */

//diffx()				/* differentiate the data in the x direction */
/*
{
	extern DATAWORD *datpt;
	int nc,nt;
	DATAWORD idat(int,int),*datp;
	
	datp = datpt+doffset;

	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]-1; nc++){
			*(datp++) = idat(nt,nc) - idat(nt,nc+1);
		}
		*(datp++) = 0;
	}
	have_max = 0;
        maxx();
}
*/
/* ********** */

int diffy()				/* differentiate the data in the y direction  -- central difference */



{
	extern DATAWORD *datpt;
	int nc,nt,size;
	DATAWORD idat(int,int),*datp,*datp2;
	
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET);
	size = (size+511)/512*512;		/* make a bit bigger for file reads */
	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	
	if(datp == 0) {
		nomemory();
		return -1;
	}
	
	datp2 = datp+doffset;

	for(nc=0;nc < header[NCHAN]; nc++){
		*(datp2++) = idat(0,nc) - idat(1,nc);
	}

	
	for(nt=1; nt<header[NTRAK]-1;nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			*(datp2++) = (idat(nt-1,nc) - idat(nt+1,nc))/2.0;
		}
	}
	
	for(nc=0;nc < header[NCHAN]; nc++){
		*(datp2++) = idat(header[NTRAK]-2,nc) - idat(header[NTRAK]-1,nc);
	}
	free(datpt);
	datpt = datp;

	have_max = 0;
    maxx();
    return 0;

}
/* ********** */

int diffx()				/* differentiate the data in the x direction  -- central difference */

{
	extern DATAWORD *datpt;
	int nc,nt,size;
	DATAWORD idat(int,int),*datp,*datp2;
	
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET);
	size = (size+511)/512*512;		/* make a bit bigger for file reads */
	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	
	if(datp == 0) {
		nomemory();
		return -1;
	}
	
	datp2 = datp+doffset;

	for(nt=0; nt<header[NTRAK];nt++) {
		*(datp2++) = idat(nt,1) - idat(nt,0);
		for(nc=1;nc < header[NCHAN]-1; nc++){
			*(datp2++) = (idat(nt,nc+1) - idat(nt,nc-1))/2.0;
		}
		*(datp2++) = idat(nt, header[NCHAN]-1) - idat(nt, header[NCHAN]-2);
	}
	free(datpt);
	datpt = datp;

	have_max = 0;
    maxx();
    return 0;
}

/* ********** */

int gradient()				/* get the 2D (x & y) gradient magnitude */
						/* GRAD command -- uses points on either side of the
										   current point for gradient */
{
	extern DATAWORD *datpt;
	int nc,nt,size;
	DATAWORD idat(int,int),*datp,*datp2;
	
	float dx,dy;
	
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET);
	size = (size+511)/512*512;		/* make a bit bigger for file reads */
	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	
	if(datp == 0) {
		nomemory();
		return -1;
	}
	
	datp2 = datp+doffset;
	
	for(nc=0;nc < header[NCHAN]; nc++){
		*(datp2++) = 0;
	}

	for(nt=1; nt<header[NTRAK]-1;nt++) {
		*(datp2++) = 0;
		for(nc=1;nc < header[NCHAN]-1; nc++){
			dx = (idat(nt,nc+1) - idat(nt,nc-1))/2.0;
			dy = (idat(nt+1,nc) - idat(nt-1,nc))/2.0;
			*(datp2++) = sqrt(dx*dx +dy*dy);
		}
		*(datp2++) = 0;
	}
	for(nc=0;nc < header[NCHAN]; nc++){
		*(datp2++) = 0;
	}
	free(datpt);
	datpt = datp;

	have_max = 0;
    maxx();
    return 0;
}
/* ********** */
/*
CYLIMAGE [COPX COPY COPZ]
	Treat the data in the current image as a half cross section of a cylindrically symmetric intensity distribution.
	The centerline is assumed to be along the LHS of the image. An integrated immage is calculated by rotating the image 
	about the centerline. A pinhole camera imaging configuration is assumed, with the center of projection specified by the
	COPX COPY and COPZ coordinates. Default values are COPX = 0, COPY = imageheight/2, and COPZ = 1.0E10.
	
*/

// A few Globals for the add_intensity function

Vector up_left,low_right;	// upper left and lower right points in the projected rectangle
Vector CoP;		// the center of projection
float z0;		// the location of the projection plane
float project_scalex, project_scaley;

int cylimage(int n,int index)					// cylindrical image
{
	DATAWORD *fdatp;
	Vector p,v,prot;	
	float theta=0.,dtheta;
	
	int ncol,nrow;
	
	Vector project(Vector),rotate_y(Vector,float);
	void add_intensity(Vector p, DATAWORD* image);

	if( sscanf(&cmnd[index],"%f %f %f",&CoP.x,&CoP.y,&CoP.z) != 3) {
		CoP.y = header[NTRAK]/2;
		CoP.x = 0;
		CoP.z = 1e10;
	}
	printf("Center of Projection is %g %g %g\n",CoP.x,CoP.y,CoP.z);
	
	// space for the new image
	fdatp = (DATAWORD*) calloc(npts+doffset,sizeof(DATAWORD));
	
	if(fdatp == 0) {
		nomemory();
		return -2;
	}
	z0 = -2.*header[NCHAN];	// put the projection plane behind the object
	dtheta = PI/360.;		// half degree increments
	
	// find the rectangle for the projected image
	p.x = 0; p.y = 0; p.z = 0; p.peakval = 0.;
	up_left = project(p);
	
	p.x = header[NCHAN]; p.y = header[NTRAK];
	//printf("p is %g %g \n",p.x,p.y);
	low_right = project(p);
	
	project_scalex = (header[NCHAN])/(low_right.x-up_left.x);
	project_scaley = (header[NTRAK])/(low_right.y-up_left.y);
	
	printf("scale x an y by: %g %g\n",project_scalex,project_scaley);
	
	printf("Projected rectangle is %g %g and ",up_left.x,up_left.y);
	printf("%g %g\n",low_right.x,low_right.y);
	for(nrow = 0; nrow < header[NTRAK]; nrow++){
		for(ncol = 1; ncol < header[NCHAN]; ncol++){
			p.peakval = idat(nrow,ncol)*(ncol)/(header[NCHAN]-1);
			p.x = ncol; p.y = nrow; p.z = 0;
			for(theta = -PI/2; theta <= PI/2; theta += dtheta){
				// rotate by theta
				prot = rotate_y(p,theta);
				// project onto plane
				v = project(prot);
				// add the intensity to the projected point in the new image
				add_intensity(v, fdatp);
			}
		}
	}
	// this is cludge --
	// make the first column equal to the second
	for(nrow = 0; nrow < header[NTRAK]; nrow++){
		*(fdatp+0+nrow*header[NCHAN]+doffset)=*(fdatp+1+nrow*header[NCHAN]+doffset);
	}
	
	// use the new image
	free(datpt);
	datpt = fdatp;
	have_max = 0;
    maxx();

	return 0;
}

Vector project(Vector p)
{
	Vector v;
	float t0;
	
	t0 = (z0-p.z)/(p.z-CoP.z);
	v.x = p.x + t0*(p.x-CoP.x);
	v.y = p.y + t0*(p.y-CoP.y);
	v.z = z0;
	v.peakval = p.peakval;
	return v;
}

Vector rotate_y(Vector p, float theta)
{
	float x;
	x = p.x;
	p.x = p.x * cos(theta) + p.z * sin(theta);
	p.z = - x * sin(theta) + p.z * cos(theta);
	return p;
}

void add_intensity(Vector p, DATAWORD* image)
{
	int nrow,ncol;
	float x,y,fx,fy;
	image += doffset;
	
	x = (p.x - up_left.x)*project_scalex;
	y = (p.y - up_left.y)*project_scaley;
	ncol = x;
	nrow = y+.5;
	fx = x - ncol;
	fy = y - nrow;
	if(nrow < 0) return;
	if (nrow >= header[NTRAK]-1)return;
	if(ncol < 0) return;
	if (ncol >= header[NCHAN]-1)return;

	//*(image+ncol+nrow*header[NCHAN]) += p.peakval*fx;
	//if(ncol>0) *(image+ncol-1+nrow*header[NCHAN]) += p.peakval*(1.-fx);
	*(image+ncol+nrow*header[NCHAN]) += p.peakval*(1.-fx);
	if(ncol < header[NCHAN]-1) *(image+ncol+1+nrow*header[NCHAN]) += p.peakval*(fx);
}

/*
PARIMAGE 
 Treat the data in the current image as a half cross section of a cylindrically symmetric intensity distribution.
 The centerline is assumed to be along the LHS of the image. An integrated image is calculated by rotating the image 
 about the centerline. A parallel image is calculated.
 
 */

int parimage(int n,int index)					// parallel integrated image
{
	DATAWORD *fdatp,*norm_plane,v1,v2;
	int depth,ncol,nrow,indx;
	float dist,dr;
	
	// space for the new image
	fdatp = (DATAWORD*) calloc(npts+doffset,sizeof(DATAWORD));
	
	if(fdatp == 0) {
		nomemory();
		return -2;
	}
	// space for the intensity distribution normal to each row
	// this is 1/4 of the whole distribution
	norm_plane = (DATAWORD*) calloc( header[NCHAN]* header[NCHAN],sizeof(DATAWORD));
	if(norm_plane == 0) {
		nomemory();
		return -2;
	}
	
	for(nrow = 0; nrow < header[NTRAK]; nrow++){
		// put the intensity at each point in the normal plane
		for(ncol = 0; ncol < header[NCHAN]; ncol++){
			for(depth = 0; depth < header[NCHAN]; depth++){
				dist = sqrt(depth*depth+ncol*ncol);
				indx = dist;
				if(indx >= header[NCHAN]-1) {
					*(norm_plane+ncol+depth*header[NCHAN]) = idat(nrow,header[NCHAN]-1);
				}else{
					v1 = idat(nrow,indx);
					v2 = idat(nrow,indx+1);
					dr = dist - indx;
					*(norm_plane+ncol+depth*header[NCHAN]) = v1+(v2-v1)*dr;	// simple linear interpolation
				}
			}
			// now integrate the column
			*(fdatp+ncol+nrow*header[NCHAN]+doffset) = *(norm_plane+ncol+0*header[NCHAN]); //the first row is only added once
			for(depth = 1; depth < header[NCHAN]; depth++){
				*(fdatp+ncol+nrow*header[NCHAN]+doffset) += *(norm_plane+ncol+depth*header[NCHAN])*2.; //other rows are added twice
			}
		
		}
		
	}
	// use the new image
	free(datpt);
	free(norm_plane);
	datpt = fdatp;
	have_max = 0;
    maxx();
	
	return 0;
	
}	
/* ********** */

int dfilt(int n,int index)					/* diffusion filter */
{
	extern float *fdatpt;
	float fdat();
	float d2x,d2y,dx,dy,s;
	int nc,nt;
	float *fdatp2,*fdatp;
	
	float t, dt, tmax,lambda;
	float g(float);
	
	if( sscanf(&cmnd[index],"%f %f %f",&dt,&tmax,&lambda) != 3) {
		beep();
		printf("Need 3 arguments:  dt,tmax, and lambda\n");
		return -1;
	}

	
	// space for a difference image
	fdatp = (float*) calloc(npts,sizeof(float));
	
	if(fdatp == 0) {
		nomemory();
		return -2;
	}
	
	float_image();

	for(t = 0.0; t< tmax; t += dt){
	
		fdatp2 = fdatp;
		for(nc=0;nc < header[NCHAN]*2; nc++){
			*(fdatp2++) = 0.0;
		}

		for(nt=2; nt<header[NTRAK]-2;nt++) {
			*(fdatp2++) = 0;
			*(fdatp2++) = 0;
			for(nc=2;nc < header[NCHAN]-2; nc++){
				d2x = (fdat(nt,nc+2) + fdat(nt,nc-2) - 2.0*fdat(nt,nc))/4.0;	// second derivative
				d2y = (fdat(nt+2,nc) + fdat(nt-2,nc) - 2.0*fdat(nt,nc))/4.0;
				dx = (idat(nt,nc+1) - idat(nt,nc-1))/2.0;
				dy = (idat(nt+1,nc) - idat(nt-1,nc))/2.0;
				s = sqrt(dx*dx +dy*dy);
				*(fdatp2++) = g(s/lambda) * (d2x + d2y) * dt;
			}
			*(fdatp2++) = 0;
			*(fdatp2++) = 0;
		}
		for(nc=0;nc < header[NCHAN]*2; nc++){
			*(fdatp2++) = 0;
		}
		
		for(nt = 0; nt<npts; nt++){		
			*(fdatpt+nt) = *(fdatpt+nt) + *(fdatp+nt);
		}

	
	}
	
	free(fdatp);
		
	get_float_image();
    have_max = 0;
    maxx();
    return 0;
        
}

// The constants here are from the hult, Kaminski, whoever, paper --
// Probably need tweaking for any particular image 

float g(float s)
{
	float y;
	float  Cm = 3.3;
	
	y = 1.0 - exp(-Cm/pow(s,8.0));
	
	return(y);

}


/* ********** */

int gradient2()				/* get the 2D (x & y) gradient magnitude */
{
	extern DATAWORD *datpt;
	int nc,nt;
	DATAWORD idat(int,int),*datp;
	
	float dx,dy;
	
	
	
	datp = datpt+doffset;

	for(nt=0; nt<header[NTRAK]-1;nt++) {
		for(nc=0;nc < header[NCHAN]-1; nc++){
			dx = idat(nt,nc) - idat(nt,nc+1);
			dy = idat(nt,nc) - idat(nt+1,nc);
			*(datp++) = sqrt(dx*dx +dy*dy);
		}
		*(datp++) = 0;
	}
	for(nc=0;nc < header[NCHAN]; nc++){
		*(datp++) = 0;
	}

	have_max = 0;
    maxx();
    return 0;
}


/* ********** */

int minus(int n, int index)
{
    
    extern DATAWORD *datpt;
#ifdef FLOAT
	float x;
	int i;
	if( sscanf(&cmnd[index],"%f",&x) != 1)
		x = n;

	i = doffset;
	while ( i < npts+doffset) {
              *(datpt+i) -= x;
              i++;
	}
#else
    extern DATAWORD max,min;
    extern TWOBYTE trailer[];
	int oldsf,newsf,i;
	/* Work out (positive) scale factors */
	oldsf = trailer[SFACTR];
	newsf = (max*oldsf-n)/maxint;
	i = (min*oldsf-n)/maxint;
	if (newsf < 0 ) newsf = -newsf;
	if (i < 0 ) i = -i;
	if (i > newsf) newsf = i;
	newsf++;

        i = 0;
        while ( i < npts) {
              *(datpt+i+doffset) = (*(datpt+i+doffset)*oldsf-n)/newsf;
              i++;
        }
	trailer[SFACTR]=newsf;
#endif
    maxx();			/* the location of the scale factor will be the
				   same, so if we already have it, we shouldn't have
				   to recalculate. */
				   
	return 0;

}

/* ********** */

int rgb2grey(int n, int index)
{
    extern DATAWORD *datpt;
	
	DATAWORD *pt,*pt_green,*pt_blue,*datp,*datp2;
	int ntrack = header[NTRAK];
	int i,nchan = header[NCHAN];
	int size;
	float x,y,z;
	
	pt = datpt+doffset;
	pt_green = pt + nchan*ntrack/3;
	pt_blue =  pt_green + nchan*ntrack/3;

	if( sscanf(&cmnd[index],"%f %f %f",&x,&y,&z) != 3){
		x = y = z = 1.;
	}
	
	size = (nchan*ntrack/3+MAXDOFFSET+511)/512*512;		/* make a bit bigger for file reads */

	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	if(datp == 0) {
			nomemory();
			return(-3);
	}
	datp += doffset;

	
	for (i=0; i < nchan*ntrack/3; i++) {
		*(pt) *= x;
		*(pt_green) *= y;
		*(pt_blue) *= z;
		*(datp++) = *(pt++) + *(pt_green++) + *(pt_blue++);
	}
   
	header[NTRAK] /= 3;

	npts = header[NCHAN] * header[NTRAK];
    
    trailer[IS_COLOR] = 0;
    image_is_color = 0;
    
	have_max = 0;
	free(datpt);
	datpt = datp2;
	maxx();
	update_status();

	return 0;
}

/* ********** */

int rgb2red(int n, int index)
{
    extern DATAWORD *datpt;
	
	DATAWORD *pt,*pt_green,*pt_blue,*datp,*datp2;
	int ntrack = header[NTRAK];
	int i,nchan = header[NCHAN];
	int size;
	
	pt = datpt+doffset;
	pt_green = pt + nchan*ntrack/3;
	pt_blue =  pt_green + nchan*ntrack/3;
	
	size = (nchan*ntrack/3+MAXDOFFSET+511)/512*512;		/* make a bit bigger for file reads */

	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	if(datp == 0) {
		nomemory();
		return(-3);
	}
	datp += doffset;
	
	for (i=0; i < nchan*ntrack/3; i++) {
		//*(datp++) = *(pt++) + *(pt_green++) + *(pt_blue++);
		*(datp++) = *(pt++);
	}
   
	header[NTRAK] /= 3;
	npts = header[NCHAN] * header[NTRAK];   
    
    trailer[IS_COLOR] = 0;
    image_is_color = 0;
    
	if(index != -1) have_max = 0;
	free(datpt);
	datpt = datp2;
	maxx();
	update_status();

	return 0;
}

/* ********** */

int rgb2green(int n, int index)
{
    extern DATAWORD *datpt;
	
	DATAWORD *pt,*pt_green,*pt_blue,*datp,*datp2;
	int ntrack = header[NTRAK];
	int i,nchan = header[NCHAN];
	int size;
	
	pt = datpt+doffset;
	pt_green = pt + nchan*ntrack/3;
	pt_blue =  pt_green + nchan*ntrack/3;
	
	size = (nchan*ntrack/3+MAXDOFFSET+511)/512*512;		/* make a bit bigger for file reads */

	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	if(datp == 0) {
		nomemory();
		return(-3);
	}
	datp += doffset;
	
	for (i=0; i < nchan*ntrack/3; i++) {
		*(datp++) = *(pt_green++);
	}
   
	header[NTRAK] /= 3;
	npts = header[NCHAN] * header[NTRAK];
    
    trailer[IS_COLOR] = 0;
    image_is_color = 0;
    
	if(index != -1) have_max = 0;
	free(datpt);
	datpt = datp2;
	maxx();
	update_status();

	return 0;
}

/* ********** */

int rgb2blue(int n, int index)
{
    extern DATAWORD *datpt;
	
	DATAWORD *pt,*pt_green,*pt_blue,*datp,*datp2;
	int ntrack = header[NTRAK];
	int i,nchan = header[NCHAN];
	int size;
	
	pt = datpt+doffset;
	pt_green = pt + nchan*ntrack/3;
	pt_blue =  pt_green + nchan*ntrack/3;
	
	size = (nchan*ntrack/3+MAXDOFFSET+511)/512*512;

	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	if(datp == 0) {
		nomemory();
		return(-3);
	}
	datp += doffset;
	
	for (i=0; i < nchan*ntrack/3; i++) {
		*(datp++) = *(pt_blue++);
	}
   
	header[NTRAK] /= 3;
	npts = header[NCHAN] * header[NTRAK];
    
    trailer[IS_COLOR] = 0;
    image_is_color = 0;
    
	if(index != -1) have_max = 0;
	free(datpt);
	datpt = datp2;
	maxx();
	update_status();

	return 0;
}

/* ********** */
/*
 DOC2RGB c1 c2 c3 c4
 Treat the image in the current image buffer as a raw document 
 (output from the dcraw routine with options -d or -D selected) 
 and convert it to an RGB image. This is assumed to have a 2 x 2 color matrix
 of R G B values in a Bayer pattern.
 c1 - c4 have values 0, 1 or 2, corresponding to red, green, and blue. For example if Bayer Matrix is
 G B
 R G
 c1 - c4 should be 1 2 0 1
 Appropriate values depend on the specific camera. (See the output from the GETRGB command.)
 */

int doc2rgb(int n, int index){
    extern DATAWORD *datpt;
	
	DATAWORD *pt,*pt_green,*pt_blue,*datp,*datp2;
	
	int bayer[2][2] = {{ 0 }};
	
	int ntrack = header[NTRAK]/2*3;
	int nchan = header[NCHAN]/2;
	int size,narg,row,col;
	
	narg = sscanf(&cmnd[index],"%d %d %d %d",&bayer[0][0],&bayer[0][1],&bayer[1][0],&bayer[1][1]);
	if(narg != 4){
		beep();
		printf("4 arguments needed. E.g., 1 2 0 1 for GBRG\n");
		return -1;
	}
	
	size = (nchan*ntrack+MAXDOFFSET+511)/512*512;
	
	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	if(datp == 0) {
		nomemory();
		return(-3);
	}
	pt = datp+doffset;
	pt_green = pt + nchan*ntrack/3;
	pt_blue =  pt_green + nchan*ntrack/3;
	
	datp = datpt + doffset;

	for (row=0; row < ntrack/3*2; row++) {		
		if( row&1)
			pt_green -= nchan;
		for (col=0; col < nchan*2; col++){
			switch (bayer[row&1][col&1]){
				case 0:		// red
					*pt++ = *datp++;
					break;
				case 1:		// green
					if(row&1){
						*pt_green = (*pt_green + *datp++)/2;
						pt_green++;
					} else {
						*pt_green++ =  *datp++;
					}
					break;
				case 2:		// blue
					*pt_blue++ =  *datp++;
					break;
			}
		}
	    if(header[NCHAN]&1) datp++;	// there may be an odd number of columns
	}
	
	header[NTRAK] =ntrack;
	header[NCHAN] =nchan;
	npts = header[NCHAN] * header[NTRAK];
    
    trailer[IS_COLOR] = 1;
    image_is_color = 1;
    

	have_max = 0;
	free(datpt);
	datpt = datp2;
	maxx();
	update_status();
	
	return 0;
}

/* ********** */
void do_rgb_mult(float x, float y, float z)
{
	extern DATAWORD *datpt;
	
	DATAWORD *pt,*pt_green,*pt_blue;
	int ntrack = header[NTRAK];
	int i,nchan = header[NCHAN];
	
	pt = datpt+doffset;
	pt_green = pt + nchan*ntrack/3;
	pt_blue =  pt_green + nchan*ntrack/3;
	
	for (i=0; i < nchan*ntrack/3; i++) {
		*(pt++) *= x;
		*(pt_green++) *= y;
		*(pt_blue++) *= z;
	}
	have_max=0;
    maxx();	
	
}

int mult_rgb(int n, int index)
{
	float x,y,z;
	
	if( sscanf(&cmnd[index],"%f %f %f",&x,&y,&z) != 3){
		beep();
		printf("3 Arguments needed\n");
		return -1;
	}
	do_rgb_mult(x,y,z);
	return 0;
}


/* ********** */

int minus_rgb(int n, int index)
{
    extern DATAWORD *datpt;
	
	DATAWORD *pt,*pt_green,*pt_blue;
	int ntrack = header[NTRAK];
	int i,nchan = header[NCHAN];
	float x,y,z;
	
	pt = datpt+doffset;
	pt_green = pt + nchan*ntrack/3;
	pt_blue =  pt_green + nchan*ntrack/3;

	if( sscanf(&cmnd[index],"%f %f %f",&x,&y,&z) != 3){
		beep();
		printf("3 Arguments needed\n");
		return -1;
	}
	for (i=0; i < nchan*ntrack/3; i++) {
		*(pt++) -= x;
		*(pt_green++) -= y;
		*(pt_blue++) -= z;
	}
	have_max=0;
    maxx();	
	return 0;
}
/* ********** */

void gamma_set(float rgam, float ggam, float bgam){
    extern DATAWORD *datpt;
    extern DATAWORD rmax,gmax,bmax;
    
    DATAWORD *pt,*pt_green,*pt_blue;
    int ntrack = header[NTRAK];
    int i,nchan = header[NCHAN];
    float r,g,b;
    
    pt = datpt+doffset;
    pt_green = pt + nchan*ntrack/3;
    pt_blue =  pt_green + nchan*ntrack/3;
    
    
    for (i=0; i < nchan*ntrack/3; i++) {
        r = *pt/rmax;
        r = powf(r,1./rgam);
        //r = r <= 0.00304 ? r*12.92 : pow(r,2.5/6)*1.055-0.055 ;
        //r = r <= 0.018 ? r*4.5 : pow(r,0.45)*1.099-0.099;
        *(pt++) = r*rmax;
        
        g = *pt_green/gmax;
        g = powf(g,1./ggam);
        //g = g <= 0.00304 ? g*12.92 : pow(g,2.5/6)*1.055-0.055 ;
        //g = g <= 0.018 ? g*4.5 : pow(g,0.45)*1.099-0.099;
        *(pt_green++) = g*gmax;
        
        b = *pt_blue/bmax;
        b = powf(b,1./bgam);
        //b = b <= 0.00304 ? b*12.92 : pow(b,2.5/6)*1.055-0.055 ;
        //b = b <= 0.018 ? b*4.5 : pow(b,0.45)*1.099-0.099;
        *(pt_blue++) = b*bmax;
        
    }
    have_max=0;
    maxx();	
}
/*
 GAMMA r_gamma g_gamma b_gamma
    Gamma correction. Raise the RGB components of the image to the power of 1/gamma.
 
 */
int gamma_fix(int n, int index)
{
    
    float rg,gg,bg;
    if( sscanf(&cmnd[index],"%f %f %f",&rg,&gg,&bg) != 3){
        beep();
        printf("3 Arguments needed\n");
        return OMA_MISC;
    }
    gamma_set(rg,gg,bg);
    return OMA_OK;
}

/* ********** */

int plus_rgb(int n, int index)
{
    extern DATAWORD *datpt;
	
	DATAWORD *pt,*pt_green,*pt_blue;
	int ntrack = header[NTRAK];
	int i,nchan = header[NCHAN];
	float x,y,z;
	
	pt = datpt+doffset;
	pt_green = pt + nchan*ntrack/3;
	pt_blue =  pt_green + nchan*ntrack/3;

	if( sscanf(&cmnd[index],"%f %f %f",&x,&y,&z) != 3){
		beep();
		printf("3 Arguments needed\n");
		return -1;
	}
	for (i=0; i < nchan*ntrack/3; i++) {
		*(pt++) += x;
		*(pt_green++) += y;
		*(pt_blue++) += z;
	}
	have_max=0;
    maxx();	
	return 0;
}
/* ********** */

int div_rgb(int n, int index)
{
    extern DATAWORD *datpt;
	
	DATAWORD *pt,*pt_green,*pt_blue;
	int ntrack = header[NTRAK];
	int i,nchan = header[NCHAN];
	float x,y,z;
	
	pt = datpt+doffset;
	pt_green = pt + nchan*ntrack/3;
	pt_blue =  pt_green + nchan*ntrack/3;

	if( sscanf(&cmnd[index],"%f %f %f",&x,&y,&z) != 3){
		beep();
		printf("3 Arguments needed\n");
		return -1;
	}
	for (i=0; i < nchan*ntrack/3; i++) {
		*(pt++) /= x;
		*(pt_green++) /= y;
		*(pt_blue++) /= z;
	}
	have_max=0;
    maxx();	
	return 0;
}
/* ********** */

int multiply(int n, int index)
{
#ifdef FLOAT
	extern DATAWORD *datpt;
	int i;
	float x;
	if( sscanf(&cmnd[index],"%f",&x) != 1)
		x = n;
	i = doffset;
	while ( i < npts+doffset) {
              *(datpt+i) *= x;
              i++;
	}
#else
	
	extern TWOBYTE trailer[];
	extern DATAWORD *datpt,min,max;
	int oldsf,newsf,i;
	float x,y;
	if( sscanf(&cmnd[index],"%f",&x) != 1)
		x = n;
	// printf("maxint %d\n",maxint);
	/* Work out (positive) scale factors */
	oldsf = trailer[SFACTR];
	newsf = ((float)(max*oldsf)*x)/maxint;
	i = ((float)(min*oldsf)*x)/maxint;
	if (newsf < 0 ) newsf = -newsf;
	if (i < 0 ) i = -i;
	if (i > newsf) newsf = i;
	newsf++;

	i = 0;
	while ( i < npts) {
	  y = *(datpt+i+doffset)*oldsf*x;
		  *(datpt+i+doffset) = y/newsf;
		  i++;
	}
	trailer[SFACTR]=newsf;
#endif
	have_max = 0;
    maxx();
    return 0;
}
/* ********** */

int divide(int n, int index)
{
#ifdef FLOAT
	extern DATAWORD *datpt;
	int i;
	float x;
	if( sscanf(&cmnd[index],"%f",&x) != 1)
		x = n;
	if( x == 0.0 ) {
		beep();
		printf("Division by 0.\n");
		return -1;
	}
	i = doffset;
	while ( i < npts+doffset) {
              *(datpt+i) /= x;
              i++;
	}
#else
	extern TWOBYTE trailer[];
	extern DATAWORD *datpt,min,max;
	int oldsf,newsf,i;
	
	float x,y;
	
	if( sscanf(&cmnd[index],"%f",&x) != 1)
		x = n;
	if( x == 0.0 ) {
		beep();
		printf("Division by 0.\n");
		return -1;
	}	
	/* Work out (positive) scale factors */
	oldsf = trailer[SFACTR];
	newsf = ((float)(max*oldsf)/x)/maxint;
	i = ((float)(min*oldsf)/x)/maxint;
	if (newsf < 0 ) newsf = -newsf;
	if (i < 0 ) i = -i;
	if (i > newsf) newsf = i;
	newsf++;

	i = 0;
	while ( i < npts) {
	  y = *(datpt+i+doffset)*oldsf/x;
		  *(datpt+i+doffset) = y/newsf;
		  i++;
	}
	trailer[SFACTR]=newsf;
#endif
	have_max = 0;
    maxx();
    return 0;
}

/* ********** */

int ruler(int n, int index)
{
	extern char unit_text[];
	extern int ruler_scale_defined;
	extern float ruler_scale;
	extern TWOBYTE trailer[];
	
	int i=0;
	float pix,unit;
	TWOBYTE *scpt;
	
	if (index == 0) {
		if( ruler_scale_defined ) {
			if( unit_text[0] ) 
				printf("%f Pixels per %s.\n",ruler_scale,unit_text);
			else
				printf("%f Pixels per Unit.\n",ruler_scale);
		} else {
			printf("No Ruler Defined.\n");
		}
		return 0;
	}
	
	if( sscanf(&cmnd[index],"%f %f",&pix,&unit) != 2) {
		printf("Two Arguments Needed.\n");
		ruler_scale = 1.0;
		ruler_scale_defined = 0;
		trailer[RULER_CODE] = 0;
		beep();
		return -1;
	}
	ruler_scale = pix/unit;	
	ruler_scale_defined = 1;
	
	unit_text[0] = 0;
	
	/* Now see if a unit was specified */
	 while (cmnd[index] != EOL && cmnd[index] != ';') {
	 	if(cmnd[index] == ' ') {
			i++;
			if(i==2) {		/* if this is the second space, assume we have a comment */
				index++;
				for(n=0; (n<UNIT_NAME_LENGTH-1) && (cmnd[index] != EOL) && (cmnd[index] != ';') ; n++) {
					unit_text[n] = cmnd[index];
					index++;
				}
				unit_text[n] = EOL;
				break;
			}
		}
	 	index++;
	}
	 	
	if( unit_text[0] ) 
		printf("%f Pixels per %s.\n",ruler_scale,unit_text);
	else
		printf("%f Pixels per Unit.\n",ruler_scale);
		
	trailer[RULER_CODE] = MAGIC_NUMBER;
	
	scpt = (TWOBYTE*) &ruler_scale;
	trailer[RULER_SCALE] = *(scpt);
	trailer[RULER_SCALE+1] = *(scpt+1);

	strcpy((char*) &trailer[RULER_UNITS],unit_text);
	return 0;
	
}

/* ********** */

int keylimit(int n)
/* limit printing command */
{
    // make a buffer so nested klimit commands work in macro/execute when klimit is built in
    // to commands
	extern unsigned char printall;
	static unsigned char saveprintall[100];
    static int printall_depth = 0;
	
	switch(n) {
        case 0:
            printall = 1;
            break;
        case -1:				/* turn off printing now but save old status */
            saveprintall[printall_depth++] = printall;
            if(printall_depth >= 100){
                printall_depth = 99;
                beep();
                printf("KLIMIT depth overflow.\n");
            }
            printall = 0;
            break;
        case -2:
            if(printall_depth <= 0){
                printall_depth = 1;
                beep();
                printf("KLIMIT depth underflow.\n");
                break;
            }
            printall = saveprintall[--printall_depth];	/* restore old status */
            break;
        default:
            printall = 1;
	}
	return 0;
}
/* ********** */

int keyrec(int n, int index)
/* save a rectangle command */
{
	extern char cmnd[],*fullname();
	int error;
	
	
	error = saverectangle(fullname(&cmnd[index],SAVE_DATA));
	if(error == 0) {
		setdata(&cmnd[index]);
		fileflush(&cmnd[index]);	// for updating directory
	}
	return error;
	
}
/* ********** */

int krrgb(int n, int index)
/* save a rectangle command */
{
	extern char cmnd[],*fullname();
	extern int save_rgb_rectangle;
	
	save_rgb_rectangle = 1;
	saverectangle(fullname(&cmnd[index],SAVE_DATA));
	setdata(&cmnd[index]);
	fileflush(&cmnd[index]);	/* for updating directory */
	save_rgb_rectangle = 0;
	
	return 0;
	
}
/* ************************************************************************ */

int macstatus = 0;
extern	char*   macbuf;    	        /* the macro buffer */
extern	int     maccount,macflag,macptr,macval;
extern	int		macincrement;


/*
INTEGRATE direction_flag selection_box do_average

direction_flag = 1 	The result is an array in x; sum in the y direction (add tracks) 
direction_flag = 0 	The result is an array in y; sum in the x direction (add channels) 

selection_box = 1	A selection box specifies how many channels or tracks to sum 
selection_box = 0	Sum all channels or tracks -- direction specified by direction_flag 

do_average = 1		Average along the integrate direction (divide by box size in integration direction)
do_average = 0		just sum, don't average
 
*/

int	integratefill = false;

int integfill(int n, int index)
{
	integratefill = true;
	integ(n,index);
	return 0;
}

int integ(int n, int index)
{

	DATAWORD 	idat(int,int);
	float	sum;
	int			pxstart,pxend,pystart,pyend;
	int 		i,j,xdirection,sbox=0,sf,do_average=0;
	float *acpoint,mx=0,mn=0;
	unsigned int  acsize = 0;

	extern TWOBYTE header[],trailer[];
	extern	DATAWORD 	idat(int,int),*datpt;	
	extern	Point		substart,subend;
	extern char			cmnd[];
	extern	int			doffset,have_max;

	xdirection = n;
	have_max = 0;
	
	/* Try to get two arguments */
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d %d",&i,&j,&do_average);
			xdirection = i;
			sbox = j;
			break;
		}
	}
	
	if(xdirection) {
	
		if( sbox && (substart.h == subend.h) ) {
			beep();		
			sbox = false;
		}
		if(sbox) {
			pxstart = substart.h;
			pxend = subend.h+1;
			pystart = substart.v;
			pyend = subend.v+1;

			if( pxstart < 0 ) 
				pxstart = 0;			
			if( pxend > header[NCHAN]) 
				pxend = header[NCHAN];
			if( pystart < 0 ) 
				pystart = 0;
			if( pyend > header[NTRAK]) 
				pyend = header[NTRAK];

		}
		else {
			pxstart = 0;
			pxend = header[NCHAN];
			pystart = 0;
			pyend = header[NTRAK];
		}
		
		acsize = header[NCHAN]*LONGBYTES;
		acpoint = (float*) malloc(acsize);
		if(acpoint == 0) {
			nomemory();
			return -1;
		}
		

		for(j=pxstart; j < pxend; j++) {
			sum = 0;
			for(i=pystart; i < pyend; i++) {
				sum += idat(i,j) * trailer[SFACTR];
			}
			if(j==pxstart)
				mn = mx = sum;
			*(acpoint + j) = sum;
			if( *(acpoint+j) > mx) mx = *(acpoint+j);
			if( *(acpoint+j) < mn) mn = *(acpoint+j);
		}

		header[NCHAN] = abs(pxend - pxstart);
		
		if(!integratefill) {
			header[NTRAK] = 1;
		}

		checkpar();
#ifdef FLOAT
		trailer[SFACTR] = sf = 1;
#else
		big = (abs(mx) > abs(mn)) ? abs(mx) : abs(mn);
		sf = 1 + (big-1)/MAXDATAVALUE;
		trailer[SFACTR] = sf;
#endif			
		for (i=0; i<header[NTRAK]; i++) {
			for(j=0; j < header[NCHAN]; j++) {
				if(do_average)
				    *(datpt + doffset + j + i*header[NCHAN]) = (*(acpoint+j+pxstart))/sf/(pyend-pystart);
				else
				    *(datpt + doffset + j + i*header[NCHAN]) = (*(acpoint+j+pxstart))/sf;
			}
		}
				
		free(acpoint);	
		integratefill = false;
		maxx();
		return 0;

	}
	else {
		if( sbox && (substart.v == subend.v) ) {
			beep();		
			sbox = false;
		}
		if(sbox) {
			pxstart = substart.v;
			pxend = subend.v+1;
			pystart = substart.h;
			pyend = subend.h+1;

			if( pxstart < 0 ) 
				pxstart = 0;			
			if( pxend > header[NTRAK]) 
				pxend = header[NTRAK];
			if( pystart < 0 ) 
				pystart = 0;
			if( pyend > header[NCHAN]) 
				pyend = header[NCHAN];

		}
		else {
			pxstart = 0;
			pxend = header[NTRAK];
			pystart = 0;
			pyend = header[NCHAN];
		}
		
		acsize = header[NTRAK]*LONGBYTES;
		acpoint = (float*) malloc(acsize);
		if(acpoint == 0) {
			nomemory();
			return -1;
		}

		for(j=pxstart; j < pxend; j++) {
			sum = 0;
			for(i=pystart; i < pyend; i++) {
				sum += idat(j,i) * trailer[SFACTR];
			}
			if(j==pxstart)
				mn = mx = sum;
			*(acpoint + j) = sum;
			if( *(acpoint+j) > mx) mx = *(acpoint+j);
			if( *(acpoint+j) < mn) mn = *(acpoint+j);
		}
		
		header[NTRAK] = abs(pxend - pxstart);
		
		if(!integratefill) {
			header[NCHAN] = 1;
		}
		
		checkpar();
#ifdef FLOAT
		trailer[SFACTR] = sf = 1;
#else
		big = (abs(mx) > abs(mn)) ? abs(mx) : abs(mn);
		sf = 1 + (big-1)/MAXDATAVALUE;
		trailer[SFACTR] = sf;
#endif			
	
		for (i=0; i<header[NTRAK]; i++) {
			for(j=0; j < header[NCHAN]; j++) {
			    if(do_average)
				*(datpt + doffset + j + i*header[NCHAN]) = (*(acpoint+i+pxstart))/sf/(pyend-pystart);
			    else
				*(datpt + doffset + j + i*header[NCHAN]) = (*(acpoint+i+pxstart))/sf;
			}
		}
							
		free(acpoint);
		integratefill = false;
		maxx();
		return 0;
	
	}
}

/* ********** */

/*	Data format is:

	nchan 	ntrak
	0	data value 0
	1	data value 1
	.
	.
*/	

int fwdata(int n, int index)
{

	int	i;
	FILE *fp;
	extern char	comment[];
    extern char cmnd[],*fullname();
	
	n = 0;
	
	//fp = fopen(fullname(&cmnd[index],SAVE_DATA),"w");
	fp = fopen(&cmnd[index],"w");

	if( fp != NULL) {
	
		fprintf(fp,"%d\t%d\n",header[NCHAN],header[NTRAK]);
		i=0;
        while ( i < npts ) {
				#ifdef FLOAT
				fprintf(fp,"%d\t%g\n",i+1,(*(datpt+doffset+i)) * trailer[SFACTR]);
				#else
				fprintf(fp,"%d\t%d\n",i+1,(*(datpt+doffset+i)) * trailer[SFACTR]);
				#endif
				i++;
		}
		i=0;	
        while (comment[i]) {
                fprintf( fp,"%s\n",&comment[i]);
                while (comment[i]) {
                        i++;
                }
                i++;
        }
    	fclose(fp);
	
		settext(&cmnd[index]);
		fileflush(&cmnd[index]);	/* for updating directory */
	}
	else {
		beep();
		return -1;
	}
	return 0;

}
/* ********** */

/*	Data format is:

	first row -- tab separated integers
	second row -- tab separated integers
	.
	.
*/	

int fwdatm(int n,int index)
{

	int	i,nt,nc;
	FILE *fp;
	
    extern char     cmnd[],*fullname();
	
	n = 0;
	
	fp = fopen(fullname(&cmnd[index],SAVE_DATA),"w");
	if( fp != NULL) {
		// fprintf(fp,"%d\t%d\n",header[NCHAN],header[NTRAK]);	omit this for matlab compatibility
		i=0;
		for(nt=0; nt<header[NTRAK]; nt++){
			for(nc=0; nc<header[NCHAN]; nc++){
				#ifdef FLOAT
				fprintf(fp,"%g\t",(*(datpt+doffset+i++)) * trailer[SFACTR]);
				#else
				fprintf(fp,"%d\t",(*(datpt+doffset+i++)) * trailer[SFACTR]);
				#endif
			}
			fprintf(fp,"\n");
		}
		/* omit this for matlab compatibility
		i=0;			
        while (comment[i]) {
                fprintf( fp,"%s\n",&comment[i]);
                while (comment[i]) {
                        i++;
                }
                i++;
        }
		*/
    	fclose(fp);
	
		settext(&cmnd[index]);
		fileflush(&cmnd[index]);	// for updating directory 
	}
	else {
		beep();
		return -1;
	}
	return 0;

}

/* ********** */

/*	Data format is:

	first row -- tab separated integers
	second row -- tab separated integers
	.
	.
	.
	.
*/	


int frdatm(int n, int index)			// read formatted data 
{
	int	i,j,k,nbyte,nr,nc=0;
	FILE *fp;
	char c;
	char text_buf[128];
	float row[4096],fmin = 1e30, fmax = -1e30;
	
    extern char     cmnd[],*fullname();	
    extern short	detector;
	n = 0;
	i = j = 0;
	
	fp = fopen(fullname(&cmnd[index],GET_DATA),"r");
	if( fp != NULL) {
		 c = fgetc(fp);
		while(c != EOF){
			
			if( c == ' ' || c == '\t' || c == ',') {
				if( n != 0)	{	//found a delimiter, get the number
					text_buf[n] = 0;
					sscanf(text_buf,"%f",&row[i]);
					if( row[i] > fmax) fmax = row[i];
					if( row[i] < fmin) fmin = row[i];
					i++;
					n = 0;
				}
			} else if (c == LF || c == CarRet) {
				if( n != 0)	{	//found a delimiter, get the number
					text_buf[n] = 0;
					sscanf(text_buf,"%f",&row[i]);
					if( row[i] > fmax) fmax = row[i];
					if( row[i] < fmin) fmin = row[i];
					i++;
					n = 0;
				}
				nc = i;
				//printf(" %d elements per row\n",i);
				// have a row now
				i = 0;
				j++;
				//printf("\n");
			
			} else {
				text_buf[n++] = c;
			}
			c = fgetc(fp);
		
		}
		if( n != 0)	{	//found a delimiter, get the number
					text_buf[n] = 0;
					sscanf(text_buf,"%f",&row[i]);
					if( row[i] > fmax) fmax = row[i];
					if( row[i] < fmin) fmin = row[i];
					n = 0;
					j++;
		}

		nr = j;
		
		printf("image is %d pixels wide x %d pixels high. \n",nc,nr);
		printf("Min: %g\t\tMax: %g\n",fmin , fmax);
		
		fclose(fp);

		header[NCHAN] = nc;
		header[NTRAK] = nr;
		header[NDX] = header[NDY] = 1;
		header[NX0] = header[NY0] = 0;
		nbyte = (header[NCHAN]*header[NTRAK]) * DATABYTES;
		detector = CCD;
		doffset = 80;
		trailer[SFACTR] = 1;
		npts = (header[NCHAN]*header[NTRAK]);
	
		if(nbyte <= 0 || checkpar()==1) {
			beep();
			printf(" Cannot Read %d Bytes!\n",nbyte);
			printf(" %d Channels & %d Tracks Reset to 1.\n",header[NCHAN],header[NTRAK]);
			header[NCHAN] = header[NTRAK] = npts = 1;
			
			return -1;
		}

		fp = fopen(&cmnd[index],"r");
		k = 0;
		c = fgetc(fp);
		while(c != EOF){
			
			if( c == ' ' || c == '\t' || c == ',') {
				if( n != 0)	{	//found a delimiter, get the number
					text_buf[n] = 0;
					sscanf(text_buf,"%f",&row[i]);
					*(datpt+doffset+k++) = row[i];
					i++;
					n = 0;
				}
			} else if (c == LF || c == CarRet) {
				if( n != 0)	{	//found a delimiter, get the number
					text_buf[n] = 0;
					sscanf(text_buf,"%f",&row[i]);
					*(datpt+doffset+k++) = row[i];
					i++;
					n = 0;
				}
				nc = i;
				//printf(" %d elements per row\n",i);
				// have a row now
				i = 0;
				j++;
				//printf("\n");
			
			} else {
				text_buf[n++] = c;
			}
			c = fgetc(fp);
		
		}
		if( n != 0)	{	//found a delimiter, get the number
					text_buf[n] = 0;
					sscanf(text_buf,"%f",&row[i]);
					*(datpt+doffset+k++) = row[i];
					n = 0;
					j++;
		}

		nr = j;
				
    	fclose(fp);
		setarrow();
		have_max = 0;
		maxx();
		update_status();

		settext(&cmnd[index]);

	}
	else {
		beep();
		return -2;
	}
		return 0;
}

/* ********** */

/*	Data format is:

	nchan 	ntrak
	0	data value 0
	1	data value 1
	.
	.
*/	


int frdata(int n, int index)			/* read formatted data */
{
	int	i,j,nbyte;
	DATAWORD dat;
	FILE *fp;
	
    extern char     cmnd[],*fullname();	
    extern short	detector;
	n = 0;
	
	fp = fopen(fullname(&cmnd[index],GET_DATA),"r");
	if( fp != NULL) {
	
		fscanf(fp,"%d %d",&i,&j);
		
		printf("Will read data as %d by %d array.\n",i,j);
		header[NCHAN] = i;
		header[NTRAK] = j;
		header[NDX] = header[NDY] = 1;
		header[NX0] = header[NY0] = 0;
		nbyte = (header[NCHAN]*header[NTRAK]) * DATABYTES;
		detector = CCD;
		doffset = 80;
		trailer[SFACTR] = 1;
		npts = (header[NCHAN]*header[NTRAK]);
	
		if(nbyte <= 0 || checkpar()==1) {
			beep();
			printf(" Cannot Read %d Bytes!\n",nbyte);
			printf(" %d Channels & %d Tracks Reset to 1.\n",header[NCHAN],header[NTRAK]);
			header[NCHAN] = header[NTRAK] = npts = 1;
			setarrow();
			fclose(fp);
			return -1;
		}

		i=0;
        while ( i < npts ) {
			#ifdef FLOAT
                fscanf(fp,"%d %f",&j,&dat);
			#else
				 fscanf(fp,"%d %d",&j,&dat);
			#endif
                *(datpt+doffset+i++) = dat;
		}
		
    	fclose(fp);
		setarrow();
		have_max = 0;
		maxx();
		update_status();

		settext(&cmnd[index]);

	}
	else {
		beep();
		return -2;
	}
	return 0;
}

/* ********** */

int rectan(int n, int index)
{
    int narg,ulx,uly,lrx,lry;
    Point start,end;
    
    extern char cmnd[];
    extern Point substart,subend;	/* these two points define a data subarray */
    extern int printxyzstuff();
    extern Variable user_variables[];
    /* For this need 4 arguments argument */
    
    narg = sscanf(&cmnd[index],"%d %d %d %d",&ulx,&uly,&lrx,&lry);
    
    if(narg == 0){
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
	
	printf("Current Rectangle is %d %d %d %d.\n",
	       start.h,start.v,end.h,end.v);
	user_variables[0].ivalue = substart.h;
	user_variables[0].is_float = 0;
	user_variables[1].ivalue = substart.v;
	user_variables[1].is_float = 0;
	user_variables[2].ivalue = subend.h;
	user_variables[2].is_float = 0;
	user_variables[3].ivalue = subend.v;
	user_variables[3].is_float = 0;
	
	return 0;
    }
    
    if(narg != 4) {
	beep();
	printf("Need 4 Arguments.\n"); 
	return -1;
    }
    
    if( (ulx > lrx) || (uly > lry) ) {
	beep();
	printf("Invalid Rectangle.\n");
	return -2;
    }
    substart.h = ulx;
    substart.v = uly;
    subend.h = lrx;
    subend.v = lry;
    user_variables[0].ivalue = substart.h;
    user_variables[0].is_float = 0;
    user_variables[1].ivalue = substart.v;
    user_variables[1].is_float = 0;
    user_variables[2].ivalue = subend.h;
    user_variables[2].is_float = 0;
    user_variables[3].ivalue = subend.v;
    user_variables[3].is_float = 0;
    
    printxyzstuff(ulx,uly,0);
    printxyzstuff(lrx,lry,1);
    return 0;
    
    
    // printf("%d %d %d %d\n",substart.h,substart.v,subend.h,subend.v); 
}

/* ********** */

int fill_box()		/* replace the data within the rectangle by interpolating
				   between values on left and right of rectangle */

{
	int i,j,width;
	DATAWORD *mypointer;
	float x,dx;

	extern Point substart,subend;	/* these two points define a data subarray */
	extern DATAWORD *datpt,idat(int,int);
	extern int doffset;
	
	width = subend.h - substart.h;
	if (width <= 0) width = 1;
	for(i=substart.v; i<= subend.v; i++) {
		mypointer = datpt + doffset + substart.h + i*header[NCHAN];
		dx = (idat(i,subend.h) - idat(i,substart.h));
		dx /= width;
		x = idat(i,substart.h);
		/* printf("%f %f %d %d %d x dx width start end.\n",x,dx,width,substart,subend); */
		for(j=substart.h; j<= subend.h; j++) {
			*mypointer = x;
			mypointer = mypointer+1;
			x += dx;
		}
	}
	return 0;
}
/* ********** */

#define READMODE O_RDONLY  /* Read file mode. */

int fill_mask(int n, int index)		
	/* FILMSK filename
		Use the specified file as a mask to define regions of the image
		where data should be interpolated between values on the left and right.
	*/
{
	int i,j,k,width,start=0,endx,inbox;
	DATAWORD *mypointer;
	float x,dx;
	TWOBYTE *pointer;	// point to things in the header
	unsigned int fild;    
	DATAWORD *file_datpt,maskval;

	extern int swap_bytes;
	extern DATAWORD *datpt,idat(int,int);
	extern int doffset;
	extern char   *fullname(), headcopy[];
	void swap_bytes_routine(char* co, int num, int nb);
	void set_byte_ordering_value();
	int get_byte_swap_value(short);
	int two_to_four(DATAWORD* dpt, int num, TWOBYTE scale);


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
		beep();
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
		two_to_four(file_datpt,i/2,1);
	} else {
		if(swap_bytes) swap_bytes_routine((char*)file_datpt,(npts+MAXDOFFSET)*DATABYTES,DATABYTES);
	}
	
	// the loop over the data

	for(i = 0; i < header[NTRAK]; i++ ) {
		inbox = 0;
		for(j=0; j<header[NCHAN]; j++ ){
			maskval = *(file_datpt + doffset + j + i*header[NCHAN]);
			if(maskval > 0 && inbox == 0) {	// enter the box
				printf("%d\n",maskval);
				start = j;
				inbox = 1;
			}
			if(inbox == 1 && maskval <= 0) {	// leave the box and do the fill
				inbox = 0;
				endx = j;
				// do the fill
				width = endx - start;
				if (width <= 0) width = 1;
				mypointer = datpt + doffset + start + i*header[NCHAN];
				dx = idat(i,endx) - idat(i,start);
				dx /= width;
				x = idat(i,start);
				//printf("%f %f %d %d x dx width start end i j.",x,dx,width,start); 
				//printf("%d %d %d\n",endx,i,j);
				for(k=start; k<= endx; k++) {
					*mypointer = x;
					//printf("%d\n",*mypointer);
					mypointer = mypointer+1;
					x += dx;
				}
			}
		}
	}
	
	free(file_datpt);
	
    close(fild);
	have_max = 0;
    maxx();
    return 0;
}


/* ********** */

Point substart,subend;	/* these two points define a data subarray that can be saved */

int calc_cmd()
{
	extern Point substart,subend;
	
	calc(&substart,&subend);
	return 0;
}
/* ********** */

int calcall()
{
	Point substart,subend;
	
	substart.h = substart.v = 0;
	subend.h = header[NCHAN]-1;
	subend.v = header[NTRAK]-1;
	
	calc(&substart,&subend);
	return 0;
}

int calc(Point *start,Point *end)
{
	double xcom,ycom,ave,rms,ftemp;		/* centroid coordinates,average, and rms */
	int icount,nt,nc;
	DATAWORD idat(int,int),datval;
	
	extern int showruler,plotline;
	extern int ruler_scale_defined;
	extern float ruler_scale;
	extern char unit_text[];
	extern Variable user_variables[];	
	extern DATAWORD min;
	
	if(start->v > end->v) {
		nt = end->v;
		end->v = start->v;
		start->v = nt;
	}
	if(start->h > end->h) {
		nt = end->h;
		end->h = start->h;
		start->h = nt;
	}
	
	if( showruler ) {
		if( plotline ) {
			do_line_plot(start,end);
			return 0;
		}	
		nt = start->v - end->v;
		nc = start->h - end->h;
		ftemp = nc;
		ycom = nt;
		xcom = nt*nt+nc*nc;
		xcom = sqrt(xcom);
		if( ruler_scale_defined ) {
			ftemp /= ruler_scale;
			xcom /= ruler_scale;
			ycom /= ruler_scale;
		}
		pprintf("¶x:\t%.2f\t¶y:\t%.2f",ftemp,ycom);		// For some goddamn reason, only can put 2 things on a line 
		if( ruler_scale_defined && unit_text[0] )
			pprintf("\tL:\t%.2f\t%s\n",xcom,unit_text);
		else
			pprintf("\tL:\t%.2f\n",xcom);
		return 0;
	}
	icount = 0;
	xcom = ycom = ave = rms = 0.0;
	
	//printf("%d %d %d %d \n", start->v,start->h,end->v,end->h);
	for(nt=start->v; nt<=end->v; nt++) {
		for(nc=start->h; nc<=end->h; nc++) {
			datval = idat(nt,nc);		
			ave += datval;					// average 
			xcom += nc * (datval-min);			// x center of mass -- subtract min
			ycom += nt * (datval-min);			// y center of mass -- subtract min 
			rms += datval*datval;			// rms 
			icount++;						// number of points 
		}
	}
	xcom /= icount;
	ycom /= icount;
	ave = ave/(float)icount;
	xcom /= (ave-min);
	ycom /= (ave-min);
	
	rms = rms/icount - ave*ave;	
	rms = sqrt(rms);
	
	pprintf("Ave:\t%g\trms:\t%g\t# Pts:\t",ave,rms);
	pprintf("%d",icount);
	if( ruler_scale_defined ) {
		xcom /= ruler_scale;
		ycom /= ruler_scale;
	}
	pprintf("\tx:\t%g\ty:\t%g",xcom,ycom);
	if( ruler_scale_defined && unit_text[0] )
		pprintf("\t%s\n",unit_text);
	else
		pprintf("\n");
	// return values available as variables
	user_variables[0].fvalue = ave;
	user_variables[0].is_float = 1;
	user_variables[1].fvalue = rms;
	user_variables[1].is_float = 1;
	user_variables[2].fvalue = xcom;
	user_variables[2].is_float = 1;
	user_variables[3].fvalue = ycom;
	user_variables[3].is_float = 1;
	return 0;
}

/* ********** */

float x1_ref,y1_ref,x2_ref,y2_ref,x1_i1,x2_i1,x1_i2,x2_i2,y1_i1,y2_i1,y1_i2,y2_i2;
int	  have_match_stuff = 0;
int   image1_width,image1_height,image2_width,image2_height;

int getmatch(int n, int index)		/* get file matching information from a text file */
{
	extern char cmnd[],*fullname();
	int err;
	
	err = loadmatch(&cmnd[index]);	
	return err;
}

int loadmatch(char* name)
{

	FILE *fp;	
	int notfound = 0;


    extern char     *fullname();

/* 	fp = fopen(name,"r"); */
	fp = fopen(fullname(name,GET_DATA),"r");
	
	if( fp != NULL) {
		if( fscanf(fp,"%f %f %f %f %f %f",&x1_ref,&y1_ref,&x1_i1,&y1_i1,
					&x1_i2,&y1_i2) != 6) notfound = 1;
		if( fscanf(fp,"%f %f %f %f %f %f",&x2_ref,&y2_ref,&x2_i1,&y2_i1,
					&x2_i2,&y2_i2) != 6) notfound = 1;
		if( fscanf(fp,"%d %d",&image1_width,&image1_height) != 2) notfound = 1;
		if( fscanf(fp,"%d %d",&image2_width,&image2_height) != 2) notfound = 1;
    	fclose(fp);

		if(notfound == 1) {
			beep();
			printf(" Data Format Problem.\n");
			return(-1);
		}
		printf(" Reference point 1 is %.2f %.2f\n",x1_ref,y1_ref);
		printf(" Reference point 2 is %.2f %.2f\n",x2_ref,y2_ref);
		printf(" Image 1 is %d by %d.\n",image1_width,image1_height);
		printf(" Image 1 point 1 is %.2f %.2f\n",x1_i1,y1_i1);
		printf(" Image 1 point 2 is %.2f %.2f\n",x2_i1,y2_i1);
		printf(" Image 2 is %d by %d.\n",image2_width,image2_height);
		printf(" Image 2 point 1 is %.2f %.2f\n",x1_i2,y1_i2);
		printf(" Image 2 point 2 is %.2f %.2f\n",x2_i2,y2_i2);
		have_match_stuff = 1;
		return(0);

	}
	else {
		beep();
		printf(" File Not Found.\n");
		return(-2);
	}
}


/* ********** */

int match_old(int n, int index)			/* Using the data in the current buffer as one input and
						   the specified file as the other input, create two new
						   files that have the same resolution and cover the same 
						   region. The new files will be called Match_1 and Match_2.
						   Use the file matching parameters previously read in */
{

	
    extern char cmnd[],*fullname(),txt[];
	extern Point substart,subend;	/* these two points define a data subarray */


	float ax,ay,bx,by,cos_th1,th1,cos_th2,th2,x1o,x2o,y1o,y2o;
	float x1,x2,scale1,scale2,x1_ref_rot,y1_ref_rot;
	float x1r_i1,y1r_i1,x2r_i1,y2r_i1,x1r_i2,y1r_i2,x2r_i2,y2r_i2;
	int i,j;

	if( !have_match_stuff ) {
		beep();
		printf("Load Matching Parameters First (use 'GETMAT filename').\n");
		return -1;
	}
	/* Next check the size of the current image (image 1) */
	
	if( (header[NCHAN] != image1_width) || (header[NTRAK] != image1_height)) {
		beep();
		printf("Current Image is not the right size for Image 1.\n");
		return -2;
	}
	
	/* Save the name of image2 -- we will get to that later */
	
	for(i=0; cmnd[index] != EOL; scratch[i++] = cmnd[index++] ) ;
	scratch[i] = 0;
	
	
	/* get the rotation angle for image 1 */
	ax = x2_i1 - x1_i1;
	ay = y2_i1 - y1_i1;
	bx = x2_ref - x1_ref;
	by = y2_ref - y1_ref;
	
	cos_th1 = (ax*bx + ay*by )/ (sqrt(ax*ax + ay*ay) * sqrt(bx*bx + by*by));
	th1 = acos(cos_th1);
		
	/* get the scale factor -- use the direction with greatest length */
	
	if( fabs(bx) >= fabs(by)) {
		/* now get the rotated x coordinates */
	
		x1 = x1_i1 * cos_th1 - y1_i1*sin(th1);
		x2 = x2_i1 * cos_th1 - y2_i1*sin(th1);
	
		scale1 = (x2 - x1) / bx;
	} else {
		/* now get the rotated y coordinates */
	
		x1 = y1_i1 * cos_th1 + x1_i1*sin(th1);
		x2 = y2_i1 * cos_th1 + x2_i1*sin(th1);
	
		scale1 = (x2 - x1) / by;
	}

	/* get the rotation angle for image 2 */
	ax = x2_i2 - x1_i2;
	ay = y2_i2 - y1_i2;
	
	cos_th2 = (ax*bx + ay*by )/ (sqrt(ax*ax + ay*ay) * sqrt(bx*bx + by*by));
	th2 = acos(cos_th2);
	
	if( fabs(bx) >= fabs(by)) {
		/* now get the rotated x coordinates */
	
		x1 = x1_i2 * cos_th2 - y1_i2*sin(th2);
		x2 = x2_i2 * cos_th2 - y2_i2*sin(th2);
	
		scale2 = (x2 - x1) / bx;
	} else {
		/* now get the rotated y coordinates */
	
		x1 = y1_i2 * cos_th2 + x1_i2*sin(th2);
		x2 = y2_i2 * cos_th2 + x2_i2*sin(th2);
	
		scale2 = (x2 - x1) / by;
	}
	printf("Image 1: Scale Factor %.3f; Rotation Angle %.3f\n", scale1, th1);
	printf("Image 2: Scale Factor %.3f; Rotation Angle %.3f\n", scale2, th2);
	
	if( fabs(th1 - th2) < (float)( 0.1/header[NTRAK]) ) {
		printf("No Rotation Needed.\n");
		x1_ref_rot = x1_ref;
		y1_ref_rot = y1_ref;
	}
	else {
		/* rotate the image in memory by the difference */
		
		th2 = (th2 - th1);			/* th2 is now the difference angle in radians */
		th1 = 180.0 / PI * th2;		/* th1 is now the difference angle in degrees */
		printf( "Rotate by %f\n", th1);

		keylimit(-1);			/* disable printing */
		sprintf(cmnd,"%f",th1);
		rotate(1,0);			/* go rotate image 1 */
		keylimit(-2);			/* reset printing  to previous mode*/
	
		/* In image 1, the old x1_i1,y1_i1 and x2_i1,y2_i1 no longer correspond
	   	   to the reference points because of the rotation -- find the new points */
		   
		x1_i1 = x1_i1 - image1_width/2.0;
		y1_i1 = y1_i1 - image1_height/2.0;
		/* printf(" moved point 1 is %.2f %.2f\n",x1_i1,y1_i1); */

		x1 = x1_i1 * cos(-th2) - y1_i1*sin(-th2);
		x2 = x1_i1 * sin(-th2) + y1_i1*cos(-th2);
		/* printf(" rotated 1 point 1 is %.2f %.2f\n",x1,x2); */

		x1_i1 = x1 + header[NCHAN]/2.0;
		y1_i1 = x2 + header[NTRAK]/2.0;
		/* printf(" Final point 1 is %.2f %.2f\n",x1_i1,y1_i1); */
		
		x2_i1 -= image1_width/2.0;
		y2_i1 -= image1_height/2.0;
		x1 = x2_i1 * cos(-th2) - y2_i1*sin(-th2);
		x2 = x2_i1 * sin(-th2) + y2_i1*cos(-th2);
		x2_i1 = x1 + header[NCHAN]/2.0;
		y2_i1 = x2 + header[NTRAK]/2.0;
		
		image1_width = header[NCHAN];
		image1_height = header[NTRAK];
		
		/* rotate the first reference point too */

		x1_ref_rot = -bx/2.0;
		y1_ref_rot = -by/2.0; 
		x1 = x1_ref_rot * cos(-th2) - y1_ref_rot*sin(-th2);
		x2 = x1_ref_rot * sin(-th2) + y1_ref_rot*cos(-th2);
		x1_ref_rot = x1 + x1_ref + bx/2.0;
		y1_ref_rot = x2 + y1_ref + by/2.0;
		
		/* printf("Rotated ref point 1 is %.2f %.2f\n",x1_ref,y1_ref); */
		
		/* printf(" Image 1 point 1 is %.2f %.2f\n",x1_i1,y1_i1);
		printf(" Image 1 point 2 is %.2f %.2f\n",x2_i1,y2_i1); */
	}
	
			
	/* For Image 1, find the size of the image in target coords */
	
	x1r_i1 = -x1_i1 / scale1 + x1_ref_rot;
	y1r_i1 = -y1_i1 / scale1 + y1_ref_rot;

	x2r_i1 = (image1_width-1 - x1_i1) / scale1 + x1_ref_rot;
	y2r_i1 = (image1_height-1 - y1_i1) / scale1 + y1_ref_rot;
	
	/* printf("Bounds of Image 1 are %.2f %.2f",x1r_i1,y1r_i1);
	printf(" and %.2f %.2f\n",x2r_i1,y2r_i1); */

	/* For Image 2, find the size of the image in target coords */
	
	x1r_i2 = x1_ref_rot - x1_i2 / scale2;
	y1r_i2 = y1_ref_rot - y1_i2 / scale2;

	x2r_i2 = x1_ref_rot + (image2_width-1 - x1_i2) / scale2;
	y2r_i2 = y1_ref_rot + (image2_height-1 - y1_i2) / scale2;
	
	/* printf("Bounds of Image 2 are %.2f %.2f",x1r_i2,y1r_i2);
	printf(" and %.2f %.2f\n",x2r_i2,y2r_i2); */
	
	/* determine the area of overlap  */
	
	if( x1r_i1 < x1r_i2 ) x1o = x1r_i2;
	else  x1o = x1r_i1;
	if( x2r_i1 > x2r_i2 ) x2o = x2r_i2;
	else x2o = x2r_i1;
	if( y1r_i1 < y1r_i2 ) y1o = y1r_i2;
	else y1o = y1r_i1;
	if( y2r_i1 > y2r_i2 ) y2o = y2r_i2;
	else y2o = y2r_i1;
	
	printf("Common rectangle has coordinates %.2f %.2f",x1o,y1o);
	printf(" and %.2f %.2f\n",x2o,y2o);

	/* Next, determine which image has greater magnification  ->
		that image will be cropped and saved.  The other image will be
		cropped, expanded, and saved */
		
	if( scale2 > scale1 ) {
		/* Image 1 is smaller, crop it then make it bigger */
		
		/* Get the bounds in pixel coordinates and make the rectangle */

		substart.h = (x1o - x1r_i1) * scale1 + 0.5;
		subend.h =  image1_width - (x2r_i1 - x2o) * scale1 - 0.5;
		
		substart.v = (y1o - y1r_i1)  * scale1 + 0.5;
		subend.v =  image1_height - (y2r_i1 - y2o)* scale1 - 0.5;
		
		printf("Image 1 cropped to %d %d %d %d.\n",substart.h,substart.v,subend.h,subend.v); 
		
		/* save the cropped image in a temp file */
		
		strcpy(txt,"Match_1");

		saverectangle(fullname(txt,SAVE_DATA));
		setdata(txt);
		fileflush(txt);	/* for updating directory */
		
		/* get the temp file and make it bigger */
		
		keylimit(-1);			/* disable printing */
		getfile (0,-1);				/* get the file */
		keylimit(-2);			/* reset printing  to previous mode*/

		/* now make a new bigger image -- the same size as the cropped Image 2*/
		
		/* The size of the image2 */
		
		substart.h = (x1o - x1r_i2) * scale2 + 0.5;
		subend.h =  image2_width - (x2r_i2 - x2o) * scale2 - 0.5;
		
		substart.v = (y1o - y1r_i2) * scale2 + 0.5;
		subend.v =  image2_height - (y2r_i2 - y2o) * scale2 - 0.5;
		
		printf("Image 2 cropped to  %d %d by %d %d.\n",substart.h,substart.v,subend.h,subend.v); 
		
		i = subend.h - substart.h + 1;
		j = subend.v - substart.v + 1;

		keylimit(-1);			/* disable printing */
		sprintf(cmnd,"%d %d",i,j);
		maknew(1,0);				/* make larger image 1 */
		
		/* Save the image */
		
		strcpy(cmnd,"Match_1");
		savefile(0,0);
		
		/* Now work with image 2 */
		
		/* Open Image 2 */
		
		strcpy(txt,scratch);
		fullname(txt,GET_DATA);
		getfile (0,-1);			/* get the file */
		keylimit(-2);			/* reset printing  to previous mode*/

		strcpy(txt,"Match_2");

		saverectangle(fullname(txt,SAVE_DATA));
		setdata(txt);
		fileflush(txt);	/* for updating directory */

		
	} else {
		beep();
		printf(" NOT implemented yet. Must reverse image 1 and 2.\n");
		return -2;
	
	}
	/* must read it in before each time since we have changed things */
	have_match_stuff = 0;	
	return 0;
}

/* ********** */

int match(int n, int index)			/* Using the data in the current buffer as one input and
						   the specified file as the other input, create two new
						   files that have the same resolution and cover the same 
						   region. The new files will be called Match_1 and Match_2.
						   Use the file matching parameters previously read in */
{

	
    extern char cmnd[],*fullname(),txt[];
	extern Point substart,subend;	/* these two points define a data subarray */


	float ax,ay,tan_th1,th1,tan_th2,th2;
	float x1,x2,scale1,scale2;
	
	int i,j,di1,di2;
	Point i2start,i2end;
        // 
        // this version ignores the reference image stuff
        // 
	if( !have_match_stuff ) {
		beep();
		printf("Load Matching Parameters First (use «GETMAT filename«).\n");
		return -1;
	}
	/* Next check the size of the current image (image 1) */
	
	if( (header[NCHAN] != image1_width) || (header[NTRAK] != image1_height)) {
		beep();
		printf("Current Image is not the right size for Image 1.\n");
		return -2;
	}
	
	/* Save the name of image2 -- we will get to that later */
	
	for(i=0; cmnd[index] != EOL; scratch[i++] = cmnd[index++] ) ;
	scratch[i] = 0;
	
	/* get the rotation angle for image 1 */
	ax = x2_i1 - x1_i1;
	ay = y2_i1 - y1_i1;
	
        // the distance between the points will give the scale factor
        scale1 = sqrt(ax*ax + ay*ay);
        // this is the angle made by the points in image 1
	tan_th1 = ay/ax;
	th1 = atan(tan_th1);
		
	/* get the rotation angle for image 2 */
	ax = x2_i2 - x1_i2;
	ay = y2_i2 - y1_i2;
	
        // the distance between the points will give the scale factor
        scale2 = sqrt(ax*ax + ay*ay);
        // this is the angle made by the points in image 2
	tan_th2 = ay/ax;
	th2 = atan(tan_th2);
        
        if(scale1 > scale2){
                beep();
		printf(" Reverse Image 1 and 2 -- Image 1 must be lower rosolution.\n");
		return -2;
        }

	printf("Image 1: Scale Factor %.3f; Rotation Angle %.3f\n", scale1, th1);
	printf("Image 2: Scale Factor %.3f; Rotation Angle %.3f\n", scale2, th2);
	
	if( fabs(th1 - th2) < (float)( 0.1/header[NTRAK]) ) {
		printf("No Rotation Needed.\n");
	}
	else {
		/* rotate the image in memory by the difference */
		
		th2 = (th1 - th2);              /* th2 is now the difference angle in radians */
		th1 = 180.0 / PI * th2;		/* th1 is now the difference angle in degrees */
		printf( "Rotate by %f\n", th1);

		keylimit(-1);			/* disable printing */
		sprintf(cmnd,"%f",th1);
		rotate(1,0);			/* go rotate image 1 */
		keylimit(-2);			/* reset printing  to previous mode*/
	
		/* In image 1, the old x1_i1,y1_i1 and x2_i1,y2_i1 no longer correspond
	   	   to the reference points because of the rotation -- find the new points */
		   
		x1_i1 = x1_i1 - (image1_width)/2.0;
		y1_i1 = y1_i1 - (image1_height)/2.0;
		/* printf(" moved point 1 is %.2f %.2f\n",x1_i1,y1_i1); */

		x1 = x1_i1 * cos(-th2) - y1_i1*sin(-th2);
		x2 = x1_i1 * sin(-th2) + y1_i1*cos(-th2);
		/* printf(" rotated 1 point 1 is %.2f %.2f\n",x1,x2); */

		x1_i1 = x1 + (header[NCHAN]-1)/2.0;         // different
		y1_i1 = x2 + (header[NTRAK]-1)/2.0;
		 printf(" Final point 1 is %.2f %.2f\n",x1_i1,y1_i1); 
		
		x2_i1 -= (image1_width)/2.0;
		y2_i1 -= (image1_height)/2.0;
		x1 = x2_i1 * cos(-th2) - y2_i1*sin(-th2);
		x2 = x2_i1 * sin(-th2) + y2_i1*cos(-th2);
		x2_i1 = x1 + (header[NCHAN]-1)/2.0;
		y2_i1 = x2 + (header[NTRAK]-1)/2.0;
		
                printf(" Final point 2 is %.2f %.2f\n",x2_i1,y2_i1);
                
		image1_width = header[NCHAN];
		image1_height = header[NTRAK];
		
	}
	
        // Scale up Image 1 -- the whole thing
        
        i = image1_width*scale2/scale1+0.5;
        j = image1_height*scale2/scale1+0.5;
        
        printf("New Image 1 size: %d by %d\n",i,j);

        keylimit(-1);			/* disable printing */
        sprintf(cmnd,"%d %d",i,j);
        maknew(1,0);                    /* make larger image 1 */
        keylimit(-2);			/* reset printing  to previous mode*/
        
        
        printf("New Image 1 size: %d by %d\n",image1_width,image1_height);
        
        x1_i1 -= (image1_width-1)/2.0;
        y1_i1 -= (image1_height-1)/2.0;

        x1 = x1_i1 * scale2/scale1;
        x2 = y1_i1 * scale2/scale1;

        x1_i1 = x1 + (header[NCHAN]-1)/2.0;
        y1_i1 = x2 + (header[NTRAK]-1)/2.0;
        printf(" Final point 1 is %.2f %.2f\n",x1_i1,y1_i1); 
        
        x2_i1 -= (image1_width-1)/2.0;
        y2_i1 -= (image1_height-1)/2.0;
        x1 = x2_i1 * scale2/scale1;
        x2 = y2_i1 * scale2/scale1;
        x2_i1 = x1 + (header[NCHAN]-1)/2.0;
        y2_i1 = x2 + (header[NTRAK]-1)/2.0;
        
        printf(" Final point 2 is %.2f %.2f\n",x2_i1,y2_i1);
        
        image1_width = header[NCHAN];
        image1_height = header[NTRAK];
        
	/* For Image 1, find the cropping coords */
		// use the average of the two points as a position reference
		
		x1_i1 = (x1_i1 + x2_i1)/ 2.0;
		y1_i1 = (y1_i1 + y2_i1)/ 2.0;
		x1_i2 = (x1_i2 + x2_i2)/ 2.0;
		y1_i2 = (y1_i2 + y2_i2)/ 2.0;
        
        substart.h = x1_i1 - x1_i2 + 0.5;
        if(substart.h <= 0) {
            i2start.h = -substart.h;    // crop left of image 2
            substart.h = 0;
        } else {
            i2start.h = 0;              // crop left of image 1; not image2
        }
        substart.v = y1_i1 - y1_i2  + 0.5;
	if(substart.v <= 0) {
            i2start.v = -substart.v;    // crop top of image 2
            substart.v = 0; 
        } else {
            i2start.v = 0;              // crop top of image 1; not image2
        }

        di1 = image1_width - 1 - x1_i1 + 0.5;  // the distance to the end of image 1
        di2 = image2_width - 1 - x1_i2 + 0.5;  // the distance to the end of image 2
        
        printf(" x1 x2 %.2f %.2f\n",x1,x2);
        
        if(di2 >= di1) {
            subend.h = image1_width - 1;  // keep all of image 1
            i2end.h =  i2start.h + subend.h - substart.h;
        } else {
            i2end.h =  image2_width - 1;  // keep all of image 2
			subend.h = substart.h + i2end.h - i2start.h;
        }

        di1 = image1_height - 1 - y1_i1 + 0.5;  // the distance to the end of image 1
        di2 = image2_height - 1 - y1_i2 + 0.5;  // the distance to the end of image 2
        
        if(di2 >= di1) {
            subend.v = image1_height-1;  // keep all of image 1
            i2end.v =  i2start.v + subend.v - substart.v;
        } else {
            i2end.v =  image2_height - 1;  // keep all of image 2
			subend.v = substart.v + i2end.v - i2start.v;
        }
        
        /* save the cropped image in a temp file */
        
        strcpy(txt,"Match_1");

        saverectangle(fullname(txt,SAVE_DATA));
        setdata(txt);
        fileflush(txt);	/* for updating directory */

        printf("Image 1 cropped to %d %d %d %d.\n",substart.h,substart.v,subend.h,subend.v); 

       
        /* Now work with image 2 */
        
        /* Open Image 2 */
        
        strcpy(txt,scratch);
        fullname(txt,GET_DATA);
        getfile (0,-1);			/* get the file */
        keylimit(-2);			/* reset printing  to previous mode*/

	/* For Image 2, find the cropping coords */
        
        substart = i2start;
        subend = i2end;
        
        /* save the cropped image in a temp file */
        
        strcpy(txt,"Match_2");

        saverectangle(fullname(txt,SAVE_DATA));
        setdata(txt);
        fileflush(txt);	/* for updating directory */

        printf("Image 2 cropped to %d %d %d %d.\n",substart.h,substart.v,subend.h,subend.v); 
            
	/* must read it in before each time since we have changed things */
	have_match_stuff = 0;	
	return 0;
}

/* ********** */
#define GET_F	1
#define GET_T	2
#define GET_ITERATION	3
#define GET_ERROR 4
#define GET_WMIX 5
#define GET_YF 6
#define GET_AT 7

int fcalc_case = GET_F;
float Fcal,W1,W2,Rc1,Rc2,T1,T2,CpQ,Yf1,Wfu,Xfu,Err,Tad,FlRich,Tcal;
int have_constants = 0;
int MaxIt,start_over;


int fcalc(int n, int index)			/* Using the data in the current buffer as rayleigh input and
						   the specified file as the fluorescence input, create the conserved
						   scalar image */
#ifdef Mac
//#pragma unused (n)
#endif
{
	int i,j,nt,nc;
	DATAWORD *raypoint,*flpoint=0;
	unsigned int  raysize;
	TWOBYTE	rayxsize,rayysize;	
	float f,aT,Wmix,x,aT1,aT2,fold=0,D2,denom,fluor,ray,T;
	int notgreat = 0;
	
    extern char    cmnd[],*fullname(),txt[];
	
	if(have_constants == 0) {
		beep();
		printf(" Load Constants First (use FCONST filename).\n");
		return -1;
	}
	
	/* copy current data into the rayleigh array */
	
	raysize = header[NCHAN]*header[NTRAK];
	rayxsize = header[NCHAN];
	rayysize = header[NTRAK];
		
	raypoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(raypoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++)
		*(raypoint + i) = *(datpt+i+doffset);
	
	/* Get the fluorescence image */
	
	for(i=0; cmnd[index] != EOL; txt[i++] = cmnd[index++] ) ;
	txt[i] = 0;
		
	fullname(txt,GET_DATA);
	getfile (0,-1);			/* get the file */
	/* keylimit(-2);			// reset printing  to previous mode*/

	if( (rayxsize != header[NCHAN]) || (rayysize != header[NTRAK]) ) {
			beep();
			printf("Image sizes are different. \n");
			goto cleanup;
	}
	
	/* copy current data into the fluorescence array */
	
		
	flpoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(flpoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++)
		*(flpoint + i) = *(datpt+i+doffset);
	
	/* now do the calcs for each point */
	
	i = 0;
	aT2 = T2*Rc2;
	aT1 = T1*Rc1;
	Wmix = W2;
	aT = aT2;
	T = 295.0;
	
	denom = Yf1 + CpQ * ( T1 - T2);
	
	D2 = (Tcal * Fcal) / (Wfu * Xfu);	/* fluorescence calibration */
	
	for( nt = 0; nt<header[NTRAK]; nt++) {
		for( nc = 0; nc<header[NCHAN]; nc++) {
		
			fluor = *(flpoint+i);
			ray = *(raypoint+i);
			
			if( start_over ) { /* start from the initial condition each time */
				Wmix = W2;
				T = 295.0;
				aT = aT2;
			}	

			f = fluor / D2 / Wmix * aT / ray + CpQ * (aT / ray - T2);
			f /= denom;
				
			for(j=0; j<MaxIt; j++) {
			
				Wmix = 1./ (f/W1 + (1.0-f)/W2);
				x = f * Wmix/W1;

				/* ----------------------------------
				if( fluor > FlRich ) {
					x = fluor *.01;
				}
				   ----------------------------------  */

				aT = (x) * aT1 + (1-x) * aT2;
				T = aT/ray;
				
				/* if on the lean side */ 							/* Methane Case */
				if ( fluor < FlRich ) { 
					aT = aT * (1.0 - 0.0156*(T - T2)/(Tad - T2));
				}
				
/*				Wmix = Wmix * (1.0 - (T - T2)/(Tad - T2) * (1.0 - 0.963));
*/				fold = f;
				
				f = fluor / D2 / Wmix * aT / ray + CpQ * (aT / ray - T2);
				f /= denom;
				
				if( fabs((f - fold)/f) < Err ) break;
			}
			
			if( fabs((f - fold)/f) > Err ) notgreat++;
			
			switch(fcalc_case) {
			case GET_T:	
				*(datpt+(i++)+doffset) = aT / ray;
				break;
			case GET_ERROR:
				*(datpt+(i++)+doffset) = (f - fold)/f *10000;
				break;
			case GET_ITERATION:
				*(datpt+(i++)+doffset) = j;
				break;
			case GET_WMIX:
				*(datpt+(i++)+doffset) = Wmix * 10;
				break;
			case GET_YF:
				*(datpt+(i++)+doffset) = fluor / D2 / Wmix * aT / ray * 1000.0;
				break;
			case GET_AT:
				*(datpt+(i++)+doffset) = aT /295.0;
				break;
			default:
			case GET_F:
				*(datpt+(i++)+doffset) = f*1000.0 ;
				break;
			}

		}
	}


cleanup:

	if(raypoint!=0) {
		free(raypoint);	
		raypoint = 0;
	}
	if(flpoint!=0) {
		free(flpoint);	
		flpoint = 0;
	}
	have_max = 0;
	pprintf(" %d questionable points.\n",notgreat);
	return 0;
}

/* ********** */

int fcalc2(int n, int index)			/* Using the data in the current buffer as Rayleigh input and
						   the specified file as a fuel rich template, create the 
						   conserved scalar image, based on table read in with FCONST 
						   
						   XXXXXXXXXXXXXXX This not implemented fully XXXXXXXXXXXXX */
#ifdef Mac
//#pragma unused (n)
#endif
{
	int i,nt,nc;
	DATAWORD *raypoint,*flpoint=0;
	unsigned int  raysize;
	TWOBYTE	rayxsize,rayysize;	
	float f,aT,Wmix,aT1,aT2,D2,fluor,ray,T;
	
	
    extern char    cmnd[],*fullname(),txt[];
	
	if(have_constants == 0) {
		beep();
		printf(" Load Constants First (use FCONST filename).\n");
		return -1;
	}
	
	/* copy current data into the rayleigh array */
	
	raysize = header[NCHAN]*header[NTRAK];
	rayxsize = header[NCHAN];
	rayysize = header[NTRAK];
		
	raypoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(raypoint == 0) {
		nomemory();
		goto cleanup2;
	}

	for(i=0; i<raysize; i++)
		*(raypoint + i) = *(datpt+i+doffset);
	
	/* Get the template image */
	
	for(i=0; cmnd[index] != EOL; txt[i++] = cmnd[index++] ) ;
	txt[i] = 0;
		
	fullname(txt,GET_DATA);
	getfile (0,-1);			/* get the file */
	/* keylimit(-2);			// reset printing  to previous mode*/

	if( (rayxsize != header[NCHAN]) || (rayysize != header[NTRAK]) ) {
			beep();
			printf("Image sizes are different. \n");
			goto cleanup2;
	}
	
	/* copy current data into the template array */
	
		
	flpoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(flpoint == 0) {
		nomemory();
		goto cleanup2;
	}

	for(i=0; i<raysize; i++)
		*(flpoint + i) = *(datpt+i+doffset);
	
	/* now do the lookup for each point */
	
	i = 0;
	aT2 = T2*Rc2;
	aT1 = T1*Rc1;
	Wmix = W2;
	aT = aT2;
	T = 295.0;
		
	D2 = (Tcal * Fcal) / (Wfu * Xfu);	/* fluorescence calibration */
	
	for( nt = 0; nt<header[NTRAK]; nt++) {
		for( nc = 0; nc<header[NCHAN]; nc++) {
		
			fluor = *(flpoint+i);
			ray = *(raypoint+i);
			
			f = fluor / D2 / Wmix * aT / ray + CpQ * (aT / ray - T2);
			//  f /= denom;
			
			*(datpt+(i++)+doffset) = f*1000.0 ;

		}
	}


cleanup2:

	if(raypoint!=0) {
		free(raypoint);	
		raypoint = 0;
	}
	if(flpoint!=0) {
		free(flpoint);	
		flpoint = 0;
	}
	have_max = 0;
	return 0;
}


/* ********** */


int fccase(int n)			/* Set the information that the fcalc command returns */
{

	fcalc_case = n;
	
	switch(n) {
	case GET_F:
		printf("FCALC will return F (* 1000).\n");
		break;
	case GET_T:				/* turn off printing now but save old status */
		printf("FCALC will return Temperature (K).\n");
		break;
	case GET_ERROR:
		printf("FCALC will return last change in F (* 10,000).\n");
		break;
	case GET_ITERATION:
		printf("FCALC will return # of iterations.\n");
		break;
	case GET_WMIX:
		printf("FCALC will return Wmix (*10).\n");
		break;
	case GET_YF:
		printf("FCALC will return Yf (*1000).\n");
		break;
	case GET_AT:
		printf("FCALC will return aT (/295).\n");
		break;
	default:
		beep();
		printf("FCALC will return F (* 1000).\n");
		fcalc_case = GET_F;
	}
	return 0;
}
/* ********** */


int fconst(int n, int index)			/* Read in constants to be used for FCALC */	
{
	extern char cmnd[],*fullname(),txt[];

	int notfound = 0;
	FILE *fp;
	int getfloat();
	float tmp;

	fp = fopen(fullname(&cmnd[index],GET_DATA),"r");

	if( fp != NULL) {
		
		if( fscanf(fp,"%f",&Fcal) != 1) notfound = 1;
		printf("%f %d\n", Fcal,notfound);
		return -1;
		if( getfloat(fp,&Tcal) != 1) notfound = 1;
		if( getfloat(fp,&Rc1) != 1) notfound = 1;
		if( getfloat(fp,&Rc2) != 1) notfound = 1;
		if( getfloat(fp,&T1) != 1) notfound = 1;
		if( getfloat(fp,&T2,txt) != 1) notfound = 1;
		if( getfloat(fp,&W1) != 1) notfound = 1;
		if( getfloat(fp,&W2) != 1) notfound = 1;
		if( getfloat(fp,&CpQ) != 1) notfound = 1;
		if( getfloat(fp,&Yf1) != 1) notfound = 1;
		if( getfloat(fp,&Wfu) != 1) notfound = 1;
		if( getfloat(fp,&Xfu) != 1) notfound = 1;
		if( getfloat(fp,&Tad) != 1) notfound = 1;
		if( getfloat(fp,&FlRich) != 1) notfound = 1;
		if( getfloat(fp,&Err) != 1) notfound = 1;
		if( getfloat(fp,&tmp) != 1) notfound = 1;
		MaxIt = tmp;
		if( getfloat(fp,&tmp) != 1) notfound = 1;
		start_over = tmp;
		
    	fclose(fp);

		if(notfound == 1) {
			beep();
			printf(" Data Format Problem.\n");
			return -2; 
		}
		
		
		printf(" Fluorescence Calibration: %.2f\n",Fcal);
		printf(" Calibration Temp: %.2f\n",Tcal);
		printf(" Rayleigh counts for fuel at T2: %.2f\n",Rc1);
		printf(" Rayleigh counts for air at T1: %.2f\n",Rc2);
		printf(" Temperature of Fuel: %.2f\n",T1);
		printf(" Temperature of Air: %.2f\n",T2);
		printf(" Adiabatic Flame Temperature: %.2f\n",Tad);
		
		printf(" mole weight of fuel stream: %.2f\n",W1);
		printf(" mole weight of air stream: %.2f\n",W2);
		
		printf(" Cp/Q: %.2f\n",CpQ);
		printf(" Y(fu,1): %.2f\n",Yf1);
		printf(" Mole weight of pure fuel: %.2f\n",Wfu);
		
		printf(" Mole fraction of fuel in fluor calib: %.2f\n",Xfu);
		printf(" Rich if fluor is greater than: %.2f\n",FlRich);

		printf(" Max acceptable deviation in f: %.2f\n",Err);

		printf(" Max number of iterations: %d\n",MaxIt);

		have_constants = 1;
		return 0;

	}
	else {
		beep();
		printf(" File Not Found.\n");
		return -3;
	}
	
}

int getfloat(FILE *fp,float *x)

{
	int i = 0;
	char j;
	extern char txt[];
	
//	while( (txt[i++] = getc(fp)) != '\n');

	for( j = fgetc(fp); j == '\t' || j > 31; j = fgetc(fp)){   // use any nonprinting character as an end of line
																// tab (ascii code is 9) is not an end of line
																// the idea here is to allow either mac or unix end of line
																// this is for maximum compatibility with old files
		//printf("%c",j);
		txt[i++] = j;	
	}

	txt[i] = 0;
	i = sscanf(txt,"%f",x);
	//printf("%d %f\n",i,*x);
	return (i);
	
}
		 
/* ***************** */

/* Scatter Plot from first and second half of current image */
/* Size of scatter plot is specified */
/* bin size is determined by the min and max values of each half */


int scat(int n, int index)				
{
	int sizx,sizy,binx,biny,size,i;
	DATAWORD xmin,xmax,ymin,ymax;
	DATAWORD *datp,*datp2;
	float binsizex,binsizey,x,y;

	sizx = 100;
	sizy = 100;

#ifdef FLOAT
	if( sscanf(&cmnd[index],"%d %d %f %f %f %f",&sizx,&sizy,&xmin,&xmax,&ymin,&ymax) != 6) {
#else		
	if( sscanf(&cmnd[index],"%d %d %d %d %d %d",&sizx,&sizy,&xmin,&xmax,&ymin,&ymax) != 6) {
#endif
		beep();
		printf("Need 6 arguments: xsize, ysize, xmin, xmax, ymin, ymax.\n");
		return(-2);
	}

	printf("%d x %d Scatter Plot.\n",sizx,sizy);
	
	size = sizx * sizy + MAXDOFFSET; 
	size = (size+511)/512*512;		/* make a bit bigger for file reads */
	
	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	if(datp == 0) {
		nomemory();
		return(-1);
	}
	
	
	binsizex = (xmax - xmin);
	binsizex /= sizx;
	binsizey = (ymax - ymin);
	binsizey /= sizy;	
#ifdef FLOAT	
		if( binsizex <= 0.) {
			binsizex = 1.0;
			beep();
			printf("Possible bin size error.\nBin size set to 1.\n");
		}
			
		if( binsizey <= 0.) {
			binsizey = 1.0;
			beep();
			printf("Possible bin size error.\nBin size set to 1.\n");
		}
#else
		if( binsizex < 1.) {
			binsizex = 1.0;
			beep();
			printf("Possible bin size error.\nBin size set to 1.\n");
		}
		
		if( binsizey < 1.) {
			binsizey = 1.0;
			beep();
			printf("Possible bin size error.\nBin size set to 1.\n");
		}
#endif
	printf("%.2f x bin %.2f  y bins\n %d points",binsizex,binsizey,npts/2);
	datp += doffset;
	
	for(i=0; i <npts/2; i++){
		x = (*(datpt+i+doffset) - xmin)/binsizex;
		binx = x+.5;
		if(binx < 0) 
			binx = 0;
		if(binx > sizx-1)
			binx = sizx-1;

		y = (*(datpt+npts/2+i+doffset) - ymin)/binsizey;
		biny = y+.5;
		if(biny < 0) 
			biny = 0;
		if(biny > sizy-1)
			biny = sizy-1;

		*( datp + binx + sizx*biny) += 1;
	}
	
	header[NCHAN] = sizx;
	header[NTRAK] = sizy;
	npts = header[NCHAN] * header[NTRAK];
	have_max = 0;

	free(datpt);
	datpt = datp2;
	update_status();
	setarrow();	
	return 0;
}

/* ***************** */

/* Put a "frame" around the current image -- making it a new size

	FRAME NewWidth NewHeight [Value] [X0] [Y0] 
	
	default value is 0
	default of x0 and y0 center the old image in the frame
	
*/


int frame(int n, int index)				
{
	int sizx,sizy,x0,y0,value,size,k,j,i;
	DATAWORD *datp,*datp2;
	
	value = 0;
	i = sscanf(&cmnd[index],"%d %d %d %d %d",&sizx,&sizy,&value,&x0,&y0);
	if( i < 2) {
		beep();
		printf("Arguments are: NewWidth NewHeight [Value] [X0] [Y0]\n");
		return(-2);
	}
	
	switch(i) {
	case 2:
	case 3:
		x0 = (sizx - header[NCHAN])/2;
	case 4:				
		y0 = (sizy - header[NTRAK])/2;		
	}
	if( sizx <  header[NCHAN] && sizy < header[NTRAK]){     //need more safety checks here
            // the cropping case
            if( i < 4)
                x0 = (header[NCHAN] - sizx)/2 ;
            if( i < 5)
                y0 = (header[NTRAK] - sizy)/2;
                
            size = sizx * sizy + MAXDOFFSET; 
            size = (size+511)/512*512;		/* make a bit bigger for file reads */
	
            datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
            if(datp == 0) {
                    nomemory();
                    return(-3);
            }
            datp += doffset;
            k=0;
            for(i=0; i<header[NTRAK]; i++){
		for(j=0; j<header[NCHAN]; j++) {
			if( j >= x0  && i >= y0 &&  j < x0+sizx && i < y0+sizy ){
				*(datp++) = *(datpt+k+doffset);
			}
                        k++;
		}
            }

            //printf("%d\n",k);

            header[NCHAN] = sizx;
            header[NTRAK] = sizy;
            
            npts = header[NCHAN] * header[NTRAK];
            have_max = 0;

            free(datpt);
            datpt = datp2;
            update_status();
            return 0;
        }


	if(x0 < 0 ||
	   y0 < 0 ||
	   (x0 + header[NCHAN]) > sizx ||
	   (y0 + header[NTRAK]) > sizy) {
		
		beep();
		printf("Inconsistent arguments.\n");
		printf("%d x %d Image.\n",sizx,sizy);
		printf("Current image starts at: %d\t%d\n",x0,y0);
		printf("Frame Value: %d\n",value);

		return(-1);
	   
	}

	printf("%d x %d Image.\n",sizx,sizy);
	printf("Current image starts at: %d\t%d\n",x0,y0);
	printf("Frame Value: %d\n",value);
	
	
	size = sizx * sizy + MAXDOFFSET; 
	size = (size+511)/512*512;		/* make a bit bigger for file reads */
	
	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	if(datp == 0) {
		nomemory();
		return(-3);
	}
	datp += doffset;
	k=0;
	for(i=0; i<sizy; i++){
		for(j=0; j<sizx; j++) {
			if( j < x0 || i< y0 ||  j >= x0+header[NCHAN] || i >= y0+header[NTRAK] )
				*(datp++) = value;
			else{
				*(datp++) = *(datpt+k+doffset);
				k++;
			}
		
		}
	}

	//printf("%d\n",k);

	header[NCHAN] = sizx;
	header[NTRAK] = sizy;
	
	npts = header[NCHAN] * header[NTRAK];
	have_max = 0;

	free(datpt);
	datpt = datp2;
	update_status();
	return 0;

}
/* ***************** */

/* Put a "frame" around the current image -- making it a new size

	FRAME NewWidth NewHeight [Value] [X0] [Y0] 
	
	default value is 0
	default of x0 and y0 center the old image in the frame
	
*/

int frame2(int n, int index)				
{
	int sizx,sizy,value,size,j,i,fraction=0;
	float x0,y0;
	DATAWORD *datp,*datp2;
	DATAWORD interpolate(float,float);

	value = 0;
	i = sscanf(&cmnd[index],"%d %d %d %f %f",&sizx,&sizy,&value,&x0,&y0);
	if( i < 2) {
		beep();
		printf("Arguments are: NewWidth NewHeight [Value] [X0] [Y0]\n");
		return(-2);
	}
	
	switch(i) {
	case 2:
	case 3:
		x0 = -(sizx - header[NCHAN])/2;
	case 4:				
		y0 = -(sizy - header[NTRAK])/2;		
	}
	
	size = sizx * sizy + MAXDOFFSET; 
	size = (size+511)/512*512;		/* make a bit bigger for file reads */

	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	if(datp == 0) {
			nomemory();
			return(-3);
	}
	datp += doffset;
	
	i = x0;
	j = y0;
	if( x0-i != 0.0 || y0-j != 0.0)fraction=1; 

	printf("%d x %d Image.\n",sizx,sizy);
	if(fraction)
		printf("Interpolation from current image starts at: %.2f\t%.2f\n",x0,y0);
	else
		printf("Current image starts at: %.0f\t%.0f\n",x0,y0);
	printf("Frame Value: %d\n",value);
	
	
	for(i=0; i<sizy; i++){
		for(j=0; j<sizx; j++) {
			if(i+y0<0 || i+y0 >=header[NTRAK] ||
			   j+x0<0 || j+x0 >=header[NCHAN]) {
					 *(datp++) = value;
			}else {
					if(fraction)
						*(datp++) = interpolate(j+x0,i+y0);
					else
						*(datp++) = idat(i+y0,j+x0);
			}
		}
	}


	header[NCHAN] = sizx;
	header[NTRAK] = sizy;
	
	npts = header[NCHAN] * header[NTRAK];
	have_max = 0;

	free(datpt);
	datpt = datp2;
	maxx();
	update_status();
	return 0;

}
/* ***************** */

/* 
Use the current rectangle to crop the image in the buffer.	
*/

int croprectangle(int n, int index)				
{
	int sizx,sizy,size,nt,nc;
	int x0,y0,image_part;
	DATAWORD *datp,*datp2;
	
	Point start,end;
	
	extern int save_rgb_rectangle;
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

	x0 = start.h;
	y0 = start.v;
	sizx = end.h - start.h +1;
	sizy = end.v - start.v +1;
	
	if(x0 + sizx > header[NCHAN] || y0 +sizy > header[NTRAK]){
		beep();
		printf("Rectangle is not contained within the current image.\n");
		return -1;
	}
	
	if(save_rgb_rectangle){
		if( y0 + sizy*3 >= header[NTRAK] ){
			beep();
			printf("Can't save rectangle as RGB image -- rectangle size problem.\n");
			save_rgb_rectangle = 0;
			return -1;
		} else {
			sizy *= 3;
		}
	}


	size = sizx * sizy + MAXDOFFSET; 
	size = (size+511)/512*512;		/* make a bit bigger for file reads */

	datp2 = datp = (DATAWORD*) calloc(size,DATABYTES);
	if(datp == 0) {
			nomemory();
			return(-3);
	}
	datp += doffset;
	
	printf("%d x %d Image.\n",sizx,sizy);
	printf("Current image starts at: %d\t%d\n",x0,y0);
	
	for (image_part = 0; image_part < (2 * save_rgb_rectangle)+1 ; image_part++){
		for(nt=start.v+image_part*header[NTRAK]/3; nt<=end.v+image_part*header[NTRAK]/3; nt++) {
			for(nc=start.h; nc<=end.h;nc++){
				*(datp++) = idat(nt,nc);	
			}
		}
	}

	header[NCHAN] = sizx;
	header[NTRAK] = sizy;
	header[NX0] += start.h*header[NDX];
	header[NY0] += start.v*header[NDY];

	
	npts = header[NCHAN] * header[NTRAK];
	have_max = 0;

	free(datpt);
	datpt = datp2;
	maxx();
	update_status();
	return 0;

}
/* ********** */
int croprgb(int n, int index)
/* crop a rectangle in an RGB image */
{
	extern int save_rgb_rectangle;
	int err = 0;
	
	save_rgb_rectangle = 1;
	err = croprectangle(0,0);
	save_rgb_rectangle = 0;
	
	return err;
	
}
/* ************************************************************************ */


int newwin(int n)

{
	extern short newwindowflag;	
	
	newwindowflag = n;
	return 0;
}

int delay(int n)
{
	// n is in ticks (1/60 seconds)
	
	clock_t start;
	start = clock();
	while ( (clock()- start)*60/CLOCKS_PER_SEC < n);
	return 0;
}

//********************************************************
//*** FECHO, FOPEN , FCLOSE 
//***         P. Kalt (2005)
//********************************************************
FILE 	*fptr_local;
char    *fptr_name;
int
fopen_pk (int n, int index)
{
	char * fullname();
	fptr_name = fullname(&cmnd[index],CSV_DATA);
	fptr_local = fopen(fptr_name,"w");
	if( fptr_local != NULL) {
		settext(&cmnd[index]);      /* text file type */
		fileflush(&cmnd[index]);	/* for updating directory */
		return OMA_OK;
	} else {
		beep();
		printf("Error: Could not open file: %s\n",fptr_name);
		return OMA_FILE;
	}
}

int
fclose_pk (int n, int index)
{
	if( fptr_local != NULL) {
		fclose(fptr_local);
	} else {
		beep();
		printf("Error: No file open. File pointer is NULL\n");
		return OMA_FILE;
	}
	
	return OMA_OK;
}

int
fecho_pk (int n, int index)
{
	if (index != 0) {
		if( fptr_local != NULL) {
			fprintf(fptr_local, "%s\n",&cmnd[index]);
		} else {
			beep();
			printf("Error: No file open. File pointer is NULL\n");
			return OMA_FILE;
		}
	}
	return 0;
}
/*___________________________________________________________________*/
//	moved here from custom.c
/*___________________________________________________________________*/

/***********************/

#define READMODE O_RDONLY  /* Read file mode. */

int image_to_power(int n,int index)
//IMPOWER scale_factor filename
//    Raise the current image to a power specified by a second image in a file.
//	The power is the value in the second image divided by the scale factor.
{
	char filename[128];
	float scale;

	unsigned int fild;
    int i;
	TWOBYTE *pointer;	/* point to things in the header */	
	int filesf;		/* Scale factor for file data */
	int ramsf;		/* Scale factor for data in RAM */
	float x,y;
	DATAWORD *file_datpt;
	
	extern int swap_bytes;
	extern char *fullname(), headcopy[];
	
	void swap_bytes_routine(char* co, int num, int nb);
	void set_byte_ordering_value();
	int get_byte_swap_value(short);
	 int two_to_four(DATAWORD* dpt, int num, TWOBYTE scale);

	/* Check to see if there was a second argument */
	y=n; /* get rid of annoying compiler warnings...*/
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			i = sscanf(&cmnd[index],"%f %s",&scale,filename);
			break;
		}
	}
	if (i != 2) {
		beep();
		printf("Argument error.\nFormat is IMPOWR 'scale_factor' 'filename'\n");
		return -1;
	}
	printf("file is: %s\n",filename);
	/* Open the file and read the header */

	if((fild = open(fullname(filename,GET_DATA),READMODE)) == -1) {
		beep();
		printf("File %s Not Found.\n",filename);
		return -1;
	}

    read(fild,(char*)headcopy,256*2);

	pointer = (TWOBYTE*)headcopy;	
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
	// the loop over the data
	for(i = doffset; i < npts+doffset; i++ ) {
		x = *(datpt+i)*ramsf;
		y = (*(file_datpt+i)*filesf)/scale;
		*(datpt+i) = pow(x,y);
	}

	trailer[SFACTR] = 1;	/* New scale factor */
	free(file_datpt);

    close(fild);
	have_max = 0;
    maxx();
    return 0;
}


/***********************/
//***************************************************************//
//**    FLIPH - Flip the image horizontally                                  *//
//***************************************************************//
int
fliph ()	
{	
	int nc,nt;
	Image Im_p;
		
	if(SDK_Image_Create(&Im_p,header[NCHAN],header[NTRAK])!=OMA_OK){
		beep();
		printf("SDK_Image_Create returned an error\n");
		return OMA_MEMORY;
	}
	
	for(nt=0; nt<header[NTRAK]; nt++) {
		for (nc =0; nc<header[NCHAN]; nc++){
			SDK_Image_SetPixel(&Im_p,header[NCHAN]-1-nc, nt, PKGetPixel(nc,nt));
		}
	}
	SDK_Image_2Buffer(&Im_p);
	SDK_Image_Free(&Im_p);
	SDK_Image_Refresh();
	return OMA_OK;
}

//***************************************************************//
//**    FLIPV - Flip the image horizontally                                  *//
//***************************************************************//
int
flipv ()	
{	
	int nc,nt;
	Image myimage;
	
	if(SDK_Image_Create (&myimage, header[NCHAN],header[NTRAK])!=OMA_OK){ 
		beep();
		nomemory();
		return OMA_MEMORY;
	}
	
	for(nt=0; nt<header[NTRAK]; nt++) {
		for (nc =0; nc<header[NCHAN]; nc++){
			SDK_Image_SetPixel(&myimage, nc,header[NTRAK]-1- nt, PKGetPixel(nc,nt));
		}
	}
	SDK_Image_2Buffer(&myimage);
	SDK_Image_Free(&myimage);
	SDK_Image_Refresh();
	return OMA_OK;
}

// *****************
int rotcrp(n,index)		// rotate and crop image
int n,index;	

/* Command:

	ROTCRP rotation_angle
	
	This command does the following operations using other internal routines --
	
		1) rotate the existing image by "rotation_angle"
		2) crop the rotated image to be so that no "filled" pixels are present (will be smaller than original)
		
*/ 

{
	extern Point substart,subend;


	DATAWORD idat(int,int);
	
	int i,j,width,height;
	float angle;
	extern char *fullname();

	/* get floating pt argument */
	j=0;
	

	j = sscanf(&cmnd[index],"%f",&angle);
	
	if (j != 1) {
		beep();
		printf(" Command format: \n");
		printf("ROTCRP rotation_angle\n");
		return -1;
	}
	
	printf("Rotate by %.2f degrees\n",angle);
	
	sprintf(cmnd,"%f",angle);
	index = 0;
	
	width = header[NCHAN];
	height = header[NTRAK];
	
	rotate(1,index);
	
	i = header[NCHAN] - width;
	j = header[NTRAK] - height;
	
	substart.h = i;
	substart.v = j;
	subend.h = header[NCHAN]-1-i;
	subend.v = header[NTRAK]-1-j;

	return croprectangle(0,0);
	
/*
	index = 0;
	
	strcpy(cmnd,"OMAtemp");

	saverectangle(fullname(cmnd,GET_DATA));
	setdata(cmnd);
	fileflush(cmnd);	// for updating directory
	
	index = 0;
	strcpy(cmnd,"OMAtemp");
	
	getfile(0,index);
	have_max = 0;
	maxx();
	return 0;
*/

}
/*----------------------------------------------------------------*/

int bin_cols = 0, bin_rows = 0, bin_header = 0, swap_bytes_flag = 0, unsigned_flag = 0;

int binsiz(n,index)		// specify the size of files to be read with GETBIN
int n,index;	
{
	long int i;
	int tmp,narg=0;
	
	
	if (n <= 0) {
		if(bin_cols == 0){
			printf("BINSIZ #columns #rows #header_bytes #swap_bytes_flag, [unsigned_flag]\n");
			beep();
			return -1;
		} else {
			printf("columns: %d rows: %d header: %d swap_bytes_flag: %d unsigned_flag: %d\n",
				bin_cols,bin_rows,bin_header,swap_bytes_flag,unsigned_flag);
			return 0;
		}
	}
	
	/* Check to see if there were arguments */
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			narg = sscanf(&cmnd[index],"%d %d %d %d %d",&bin_cols,&bin_rows,&bin_header,&swap_bytes_flag,&tmp);
			break;
		}
	}
	if(narg == 5) 	
		unsigned_flag = tmp;
	else 
		unsigned_flag = 0;
	printf("columns: %d rows: %d header: %d swap_bytes_flag: %d unsigned_flag: %d\n",bin_cols,bin_rows,
				bin_header,swap_bytes_flag,unsigned_flag);
	return 0;

}

int binary_file_bytes_per_data_point = 2;
int binary_file_is_float = 0;

int binbyt(int n,int index)		// specify the the number of bytes in the data read in by GETBIN
								// if the value is -sizeof(float) (usually -4), the data will be interpreted as float
{
	binary_file_is_float = 0;
	
	if(n == -sizeof(float)){
		n = sizeof(float);
		binary_file_is_float = 1;
	}
	if (n <= 0) {
		n = 2;
	}
		
	binary_file_bytes_per_data_point = n;
		
	printf("%d bytes per data point\n",binary_file_bytes_per_data_point);
	if(binary_file_is_float)
		printf("Binary will be interpreted as float\n");
	else
		printf("Binary will be interpreted as integers\n");
	
	return 0;

}
    
// Note that the get file suffix is added here
int getbin(int n,int index)		
{
	
	extern char txt[];		     /* the file name will be stored here temporarily */
    extern int fd,nbyte;
	extern char *fullname();
	unsigned int nr,i;
	unsigned short *usptr;
	short *sptr;
	char *ptr2,tc;
	float *fptr;
	int *iptr;

	
 	strcpy(txt,&cmnd[index]);
	
	fullname(txt,GET_DATA);		// add prefix and suffix  
		
    if((fd = open(txt,READMODE)) == -1) {
		beep();
		printf("File %s Not Found.\n",txt);
		return -1;
	}
	if(bin_header > 0) {
		ptr2 = malloc(bin_header);
		if(ptr2 == 0) {
			nomemory();
			return -1;
		}

    	read(fd,ptr2,bin_header);	// skip over the header 
		free(ptr2);
	}
	
	header[NDX] = header[NDY] = 1;
	header[NX0] = header[NY0] = 0;
	header[NCHAN] = bin_cols;
	header[NTRAK] = bin_rows;
	
	
	nbyte = (header[NCHAN]*header[NTRAK]) * binary_file_bytes_per_data_point;

	//detector = CCD;
	doffset = 80;
					
	trailer[SFACTR] = 1;
	
	if(nbyte <= 0 || checkpar()==1) {
			beep();
			printf(" Cannot Read %d Bytes!\n",nbyte);
			printf(" %d Channels & %d Tracks Reset to 1.\n",header[NCHAN],header[NTRAK]);
			header[NCHAN] = header[NTRAK] = npts = 1;
			close(fd);
			return -1;
	}
	
	if( binary_file_bytes_per_data_point == 1) {
		// allocate memory
		ptr2 = malloc(nbyte);
		if(ptr2 == 0) {
			nomemory();
			close(fd);
			return -1;
		}
		// Read in the actual data 
		nr = read(fd,ptr2, nbyte);
		printf("%d Bytes read.\n",nr);
		close(fd);
		for(i=doffset; i<npts+doffset; i++){
			*(datpt+i) = *(ptr2+i-doffset);
		}
		free(ptr2);
	} else if( binary_file_bytes_per_data_point == sizeof(short)) {
		// allocate memory
		sptr = malloc(nbyte);
		if(sptr == 0) {
			nomemory();
			close(fd);
			return -1;
		}
		// Read in the actual data 
		nr = read(fd,sptr, nbyte);
		printf("%d Bytes read.\n",nr);
		close(fd);
		
		if(swap_bytes_flag){
			// fiddle the byte order
			ptr2 = (char *)sptr;		// a copy of the data pointer 
			for(i=0; i< nr; i+=2){
				tc = *(ptr2);
				*(ptr2) = *(ptr2+1);
				*(++ptr2) = tc;
				ptr2++;
			}
		}
		usptr = (unsigned short*) sptr;		// point to the same data
		if(unsigned_flag && (DATABYTES==2)){
			for(i=0; i< nr/2; i++){
				*(usptr+i) /= 2;
			}
			trailer[SFACTR] = 2;
		}
		for(i=doffset; i<npts+doffset; i++){
			if(unsigned_flag)
				*(datpt+i) = *(usptr+i-doffset);
			else
				*(datpt+i) = *(sptr+i-doffset);
		}
		free(sptr);
	} else if( binary_file_bytes_per_data_point == sizeof(float) && binary_file_is_float) {
		// allocate memory
		fptr = malloc(nbyte);
		if(fptr == 0) {
			nomemory();
			close(fd);
			return -1;
		}
		// Read in the actual data 
		nr = read(fd,fptr, nbyte);
		printf("%d Bytes read.\n",nr);
		close(fd);
		for(i=doffset; i<npts+doffset; i++){
			*(datpt+i) = *(fptr+i-doffset);
		}
		free(fptr);	
	}  else if( binary_file_bytes_per_data_point == sizeof(int)){
		// allocate memory
		iptr = malloc(nbyte);
		if(iptr == 0) {
			nomemory();
			close(fd);
			return -1;
		}
		// Read in the actual data 
		nr = read(fd,iptr, nbyte);
		printf("%d Bytes read.\n",nr);
		close(fd);
		for(i=doffset; i<npts+doffset; i++){
			*(datpt+i) = *(iptr+i-doffset);
		}
		free(iptr);	
	}
	
	have_max = 0;
	
	maxx();
	printparams();
	return 0;
}
/*----------------------------------------------------------------*/

float aspect_ratio = 0.0;		// if this is 0, the aspect ratio of rectangles is as drawn with the cursor
								// if this is set to a nonzero value with the ASPECTRATIO command, 
								// the rectangle will be constrained to the specified aspect ratio
								// aspect ratio is width/height
/*
ASPECTRATIO aspect_ratio

If aspect_ratio is 0 (or the argument is omitted), the aspect ratio of rectangles is as drawn with the cursor.
If aspect_ratio is set to a nonzero value with the ASPECTRATIO command, 
rectangles drawn with the cursor will be constrained to the specified aspect ratio.
The aspect ratio is width/height.
*/

int aspectratio(int n, int index)
{
	if( sscanf(&cmnd[index],"%f",&aspect_ratio) != 1){
		aspect_ratio = 0.0;
		printf("Aspect ratio of rectangles will be as drawn.\n");
	} else {
		aspect_ratio = fabs(aspect_ratio);
		printf("Aspect ratio of rectangles will be constrained to %f\n",aspect_ratio);
	}
	return 0;
}

/*___________________________________________________________________*/
/*______________________________Service bits_________________________*/
/*___________________________________________________________________*/

/* ***************** */

DATAWORD idat(int nt,int nc)
{
	extern DATAWORD *datpt;
	extern int	 doffset;
	DATAWORD *pt;
	
	int index;
	
	if (datpt == 0) return(0);
	if(nc < 0) nc = 0;
	if(nt < 0) nt = 0;
	if(nc > header[NCHAN]-1) nc = header[NCHAN]-1;
	if(nt > header[NTRAK]-1) nt = header[NTRAK]-1;
	index = nc + nt*header[NCHAN];
	//if (index >= header[NCHAN]*header[NTRAK]) return(0);	// check for illegal value passed
	pt = datpt + index + doffset;
	return(*pt);
}

float fdat(int nt,int nc)		// get the floating point data value
{
	extern float *fdatpt;
	extern int	 nbyte;
	float *pt;
	int index;
	
	if (fdatpt == 0) return(0);	
	index = nc + nt*header[NCHAN];
	if (index >= nbyte) return(0);	// check for illegal value passed	
	pt = fdatpt + index;
	return(*pt);
}

int float_image()			// copy the current image into the floating-point buffer
{
	extern float *fdatpt;
	int n;

	if(fdatpt != 0) free(fdatpt);
	
	fdatpt = (float*) malloc(npts*sizeof(float)); 

	if(fdatpt == 0) {
		nomemory();
		return(0);
	}
	
	for(n=0; n < npts; n++) {
              *(fdatpt+n) = *(datpt+n+doffset) * trailer[SFACTR];
    }
    return(1);
}

int new_float_image(int nx, int ny)			// initialize a floating-point buffer of a given size
{
	extern float *fdatpt;
	int n;

	if(fdatpt != 0) free(fdatpt);
	
	fdatpt = (float*) malloc(nx*ny*sizeof(float)); 
         

	if(fdatpt == 0) {
		nomemory();
		return(0);
	}
	
	for(n=0; n <nx*ny; n++) {
              *(fdatpt+n) = 0.0;
        }
    return(1);
}

int get_float_image()			// copy the current floating image into the image buffer
{
    extern float *fdatpt;
    int n,i,newsf;
    float fmn,fmx;
   
    


    if(fdatpt == 0) return(0);
    
    fmn = *(fdatpt);
    fmx = *(fdatpt);

    for(n=1; n < npts; n++) {
            if( *(fdatpt+n) > fmx) fmx = *(fdatpt+n);
            if( *(fdatpt+n) < fmn) fmn = *(fdatpt+n);
    }
    newsf = fmx/maxint;
    i = fmn/maxint;
    if (newsf < 0 ) newsf = -newsf;
    if (i < 0 ) i = -i;
    if (i > newsf) newsf = i;
    newsf++;
                                    
    for(n=0; n < npts; n++) {
            *(datpt+n+doffset) = *(fdatpt+n)/newsf;
    }
    trailer[SFACTR] = newsf;
    
    
    free(fdatpt);
    fdatpt = 0;
    
    return(1);
}

/* ***************** */



/* calculate the histogram of a data array */
/* the histogram will have HISTOSIZE values */
/* bin size is determined by the min and max values */

int histo_array[HISTOSIZE] = {0};

int data_histogram(array,min,max,points)
DATAWORD *array,min,max;
int points;
{

	int i,binsize,bin;
	
	for(i=0; i < HISTOSIZE; i++) 
		histo_array[i] = 0;
	
	binsize = (max - min)/HISTOSIZE;
	if( binsize < 1) binsize = 1;
	
	for(i=0; i < points; i++){
		bin = (array[i]-min)/binsize;
		if(bin < 0) 
			bin = 0;
		if(bin > HISTOSIZE-1)
			bin = HISTOSIZE-1;
		histo_array[bin]++;
	}
	return(binsize);
}

/* ***************** */

/* set the bounds of a data array to be within certain limits */

int setbounds(DATAWORD *array,DATAWORD lower, DATAWORD upper,int points)
{
	int i;
	
	for(i=0; i<points; i++){
		if( array[i] > upper) array[i] = upper;
		if( array[i] < lower) array[i] = lower;
		array[i] -= lower;
	}
	return 0;
}

/****************************************************************************/

/* 
	Check the "macro status" flag 
	 If the flag == n, continue a macro as usual -- 
		otherwise, skip all the rest of the macro commands 
	
*/
/*
chkmac(n)
int n;
{
	if(macflag) {
		if( macstatus == n)	return 0;		// continue as usual 
		
		if(n!=2) {
			maccount++;
			macval -= macincrement; 
		}
		
		for(;;) {
	
			while ( *(macbuf + (++macptr) ) ) ;	// skip remaining commands 
			
			if( *(macbuf+(++macptr))  == EOL) {
				macptr--;
				beep();
				return -1;
			}
		}
	}
	return 0;
}
*/
/****************************************************************************/
/*
int stair,endair,stmix,endmix;
float lowlim,uplim,result;
int chan;


//	setup constants for use in other commands 



setup(n)

{

	static int type = 1;
	
	if( n == 0 ) {
		printf("SETUP Types:\n\n  0) List Values.\n");
		printf("  1) For Diff-Diff Reference.\n     Specify:\n");
		printf("     Channel #\n"); 
		printf("     2 segments to be averaged\n");
		printf("     Allowed bounds for ratio.\n");
		printf("  2) For Detecting Large Excursions in Data:\n     Specify:\n");
		printf("	 Channel #\n");
		printf("     Segment to be checked\n");
		printf("     Max gradient\n");
		printf("     Max value.\n");
		printf("  3) For Calculating Scalar from a Segment:\n     Specify:\n");
		printf("     Channel #\n");
		printf("     Segment to be checked.\n");
		printf("Current Type is %d.\n\n",type); }
	else {
		type = n;
	}	
	switch( type ) {
		case 1:
			if( n!=0) {
				printf("Type Channel Number.\n");
				getlin(scratch);
				sscanf(scratch,"%d",&chan);
				printf("Type START_AIR, END_AIR, START_MIX, END_MIX.\n");
				getlin(scratch);
				sscanf(scratch,"%d %d %d %d",&stair,&endair,&stmix,&endmix);
				printf("Type LOWER_LIMIT and UPPER_LIMIT for ratio. (Reals)\n");
				getlin(scratch);
				sscanf(scratch,"%f%f",&lowlim,&uplim);
			}
			printf("Channel: %d\nStart Air: %d\nEnd Air: %d\nStart Mix: %d\nEnd Mix: %d\n",
				chan,stair,endair,stmix,endmix);	
			printf("Lower Limit: %f\nUpper Limit: %f\n",lowlim,uplim);
			if (n != 0)
				chan--;
			break;
		case 2:
			if( n!=0) {
				printf("Type Channel Number.\n");
				getlin(scratch);
				sscanf(scratch,"%d",&chan);
				printf("Type START_SEG, END_SEG, MAX_GRAD, MAX VAL.\n");
				getlin(scratch);
				sscanf(scratch,"%d %d %d %d",&stair,&endair,&stmix,&endmix);
			}
			printf("Channel: %d\nStart Segment: %d\nEnd Segment: %d\nMax Gradient: %d\nMax Value: %d\n",
				chan,stair,endair,stmix,endmix);	
			if (n != 0)
				chan--;
			break;
		case 3:
			if( n!=0) {
				printf("Type Channel Number.\n");
				getlin(scratch);
				sscanf(scratch,"%d",&chan);
				printf("Type START_SEG, END_SEG.\n");
				getlin(scratch);
				sscanf(scratch,"%d %d",&stair,&endair);
			}
			printf("Channel: %d\nStart Segment: %d\nEnd Segment: %d\n",
				chan,stair,endair);	
			if (n != 0)
				chan--;
			break;
		default:
			printf("Unknown Setup Type.\n");
			
		}	// end of switch
		
}
*/

/****************************************************************************/

/* 
	check the status of some condition							
	Print a result and set the "macro status" flag					
	case 1:																
	 	check for proper mixture by averaging over 2 regions & getting ratio
	case 2:													
	 	look for dust particles in diff diff data 
		
*/

/*
dochk(n)
int n;

{
	float sumair,summix;
	DATAWORD	idat(int,int);
	int i;
	
	extern int macval,maccount,macincrement;
	
	switch(n) {

	case 2:				// look for dust particles in diff diff data 

		for(i=stair; i<endair; i++) {
			if( abs(idat(i,chan)-idat(i+2,chan) ) > stmix || idat(i,chan) > endmix) {
				macstatus = 0;
				pprintf("%d\n",macval);
				return;
			}
		}
		macstatus = 2;
		break;
	case 1:				// check for proper mixture by averaging over 2 regions & getting ratio 
	default:
	
		sumair = summix = 0.0;
	
		for(i=stair; i<endair; i++)
			sumair += idat(i,chan);
		
		for(i=stmix; i<endmix; i++)
			summix += idat(i,chan);
	
		result = (summix/ abs(stmix-endmix) ) / (sumair/ abs(stair-endair) );
	
		if(result >= lowlim && result <= uplim) 
			macstatus = n;
		else
			macstatus = 0;
		
		pprintf("%f\n",result);
		break;
	}
}
*/

/****************************************************************************/
/*
	Get a scalar value by averaging over a specified region and dividing by 
		a specified scale factor
*/

float scalarvalue = 0.0; 	// this is calculated in getscalar() and used in mulscalar() 
/*
getscalar(sfact)

int sfact;

{
	float sum = 0;

	int i;
	
	if(sfact == 0) sfact = 1;
	
	for(i=stair; i<endair; i++)
		sum += idat(i,chan);
	scalarvalue = sum/ abs(stair-endair)/sfact;
	pprintf("%f\n",scalarvalue);

}
*/
/****************************************************************************/
/*
	Multiply the data array by the scalar value calculated with the GETSCA command
*/
/*
mulscalar()
{
    extern TWOBYTE header[];
    extern TWOBYTE trailer[];
    extern DATAWORD *datpt,min,max;
	extern int		maxint,npts,doffset;
	int oldsf,newsf,i;
	float x;

	// Work out (positive) scale factors 
	oldsf = trailer[SFACTR];
	newsf = (max*oldsf*scalarvalue)/maxint;
	i = (min*oldsf*scalarvalue)/maxint;
	if (newsf < 0 ) newsf = -newsf;
	if (i < 0 ) i = -i;
	if (i > newsf) newsf = i;
	newsf++;

        i = 0;
        while ( i < npts) {
              x = *(datpt+i+doffset)*oldsf*scalarvalue/newsf;
			  *(datpt+i+doffset) = x;
              i++;
        }
	trailer[SFACTR]=newsf;
        maxx();

}
*/
/****************************************************************************/
/*
	Divide the data array by the scalar value calculated with the GETSCA command
*/
/*
divscalar()
{
    extern TWOBYTE header[];
    extern TWOBYTE trailer[];
    extern DATAWORD *datpt,min,max;
	extern int		maxint,npts,doffset;
	int oldsf,newsf,i;
	float x;

	// Work out (positive) scale factors 
	oldsf = trailer[SFACTR];
	newsf = (max*oldsf/scalarvalue)/maxint;
	i = (min*oldsf/scalarvalue)/maxint;
	if (newsf < 0 ) newsf = -newsf;
	if (i < 0 ) i = -i;
	if (i > newsf) newsf = i;
	newsf++;

        i = 0;
        while ( i < npts) {
              x = *(datpt+i+doffset)*oldsf/scalarvalue/newsf;
			  *(datpt+i+doffset) = x;
              i++;
        }
	trailer[SFACTR]=newsf;
        maxx();

}
*/
/****************************************************************************/

