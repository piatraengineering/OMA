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

	
int	alter_vectors = 0;
extern int 	no_of_vel;


/* Some definitions and static globals to be used by piv related commands */
#define READMODE O_RDONLY  /* Read file mode. */
#define INIT 0
#define PLOT 1
#define CLOSE 2
#define INIT_CURRENT 5
#define CLOSE_CURRENT 6


#define NO_PREF 0
#define POS_X 1
#define NEG_X 2
#define POS_Y 3
#define NEG_Y 4
#define AUTO_DIR 5

/**************************************************************************************/
//		L O C A L   V A R I A B L E S   B E L O W . . . .
/**************************************************************************************/
float accuracy = 0.65;	// Threshold of accuracy for vector comparison 0=different 1.0= identical
int show_auto=0;	// if set, replace image data with auto-correlation and exit

int verbosity = 0;
int	no_of_velx = 0;				/* the number of velocities calculated in the x and y directions */
int	no_of_vely = 0;
int peaks_to_save = 6;			/* the number of peaks to save */
#ifdef FLOAT
DATAWORD* peakval;
#define PEAKVAL_SIZE sizeof(DATAWORD)
#define PEAKVAL_TYPE DATAWORD
#else
int*  peakval = 0;
#define PEAKVAL_SIZE sizeof(int)
#define PEAKVAL_TYPE DATAWORD
#endif
int fftsize = 128;
int boxinc = 64;
int sum_area;
int exclude_radius;
int plotduringpiv = 1;
int preprocesspiv = 0;
int peakmethod = 0;
int plotarrowheads = 1;
int plot_dir_pref = NO_PREF;
int labelplot = 0;
int c_flag = 0;
float pivscale = 1.0;
float pivdelay = 1.0;
float pivpixel = 1.0;
char PreMacro[256] = "piv_prep";
int autoclip = 0;		/* flag to determine whether or not to auto-clip */
//int cross_correlation = 0;	/* flag is set to 1 if piv was done by cross correlation */
int localavg = 0;		/* average of subregion of data */
int shiftx = 0;		/* shift values used in the cross-correlation (see xpiv) */
int shifty = 0;

char sstring[CHPERLN];


float loclip = 0.1;
float hiclip = 0.1;

float* xpeaks = 0;
float* ypeaks = 0;	
float* dirval = 0;			/* for vector direction info */
float* imagin = 0;


extern int	have_max;
extern char    txt[];
/* ***************** */
extern DATAWORD *backdat;		/* the data pointer for backgrounds */
extern unsigned int backsize;
/*
	fft 0 n		return the magnitude scaled by n
	fft 1 n		return the real part scaled by n
	fft 2 n		return the imaginary part scaled by n
	fft 3 n		return inverse - input is real part scaled by n
	fft 4 n		return inverse - input is imaginary part scaled by n
	fft 5 n		return the log of the magnitude squared scaled by n
	fft 6 n		return the magnitude squared scaled by n
	fft 7 n		return the autocorrelation scaled by n
	fft 8 n		return inverse - input is a filter scaled by n
	
 */

float 	*a,*b,*work1,*work2;
float	*x,*y;
Boolean	is_fft = false;
Boolean	is_fft_memory = false;

int printf(),pprintf(), printxyzstuff(),condtnl(),fastf(),FT2D(),multiply(),setpositive(),clip();
int peak_refine();

int dofft(int n,int index)
					/* fourier transform of the data array */
{
	int size,i,j,k,is_power_2();
	float scale;
	
	DATAWORD *datp;

	static int n1 = 0;
	static int n2 = 0;
	
    extern DATAWORD *datpt;
	extern TWOBYTE header[];
	extern int	doffset;
	extern char cmnd[];
	
	scale = 1.0;		/* in case no scale factor is specified */
	have_max = 0;	/* won't have max right after all this */

	/* Check to see if there was a second argument */
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %f",&n,&scale);
			break;
		}
	}
	
	
	size = header[NCHAN] * header[NTRAK];
	datp = datpt + doffset;
	
	i = header[NCHAN];
	j = header[NTRAK];
	
	if( (i == 1 && !is_power_2(j))  ||
		(j == 1 && !is_power_2(i)) ) {
		beep();
		printf("Array size must be a power of 2.\n");
		return -1;
	}
	if( i!= 1 && j != 1) {
		if ( !is_power_2(j) || !is_power_2(i) ) {
			beep();
			printf("Image size must be a power of 2.\n");
			return -2;
		}
	}
		
	

	/* --------------------------------------------------------- */
	/*			1 D Case		returns magnitude only			 */
	/* --------------------------------------------------------- */
	
	if( header[NCHAN] == 1 || header[NTRAK] == 1 ) {	/* the 1-D case */
		printf(" 1-D transform returns magnitude only.\n");
		x = (float *)malloc(size*4);
		y = (float *)malloc(size*4);

		if( x==0 || y==0 ) {
			nomemory();
			return -3;
		}

	
		for(i=0; i<size; i++){
			x[i] = *(datp++);
			y[i] = 0.0;
		}

		fastf(x,y,&size);

		datp = datpt + doffset;
		for(i=0; i<size; i++){
			*(datp++) = sqrt(x[i]*x[i] + y[i]*y[i])*scale; 	
		/*	*(datp++) = x[i]*scale;		*/
		}
		free(x);
		free(y);
		setarrow();
		return 0;
		
	}
	
	/* --------------------------------------------------------- */
	/*						End	1 D Case						 */
	/* --------------------------------------------------------- */

	
	/* --------------------------------------------------------- */
	/*					Inverse Transform Cases					 */
	/* --------------------------------------------------------- */

	if( n == 3) {
		if( !is_fft_memory ) {
			beep();
			printf("No FFT. Can't do inverse.\n"); 
			setarrow();
			return -1;
		}

		if( n1 != header[NCHAN] || n2 != header[NTRAK] ) {
			beep();
			printf("Size of FFT Does Not Match Current Image.\n");
			setarrow();
			return -2;
		}
			
		for(i=0; i<header[NTRAK]*header[NCHAN]; i++) {
			a[i] =  *(datp++);
			a[i] /= scale; 
		}
		n1 = -n1;
		n2 = -n2;

		FT2D(a,b,work1,work2,&n1,&n2);

		datp = datpt + doffset;
		k = 0;
		for(i=1; i<=header[NTRAK]; i++) {
			for(j=1; j<=header[NCHAN]; j++) {
				if( 1 & (i+j)) 
					*(datp++) = -a[k]; 	/* for optical rather than standard ordering */
				else
					*(datp++) = a[k]; 
				k++;
			}
		}
		setarrow();	
		return 0;

	}
	if( n == 4) {
		if( !is_fft_memory ) {
			beep();
			printf("No FFT. Can't do inverse.\n");
			setarrow();
			return -3;
		}
		if( n1 != header[NCHAN] || n2 != header[NTRAK] ) {
			beep();
			printf("Size of FFT Does Not Match Current Image.\n");
			setarrow();
			return -4;
		}

		for(i=0; i<header[NTRAK]*header[NCHAN]; i++) {
			b[i] =  *(datp++);
			b[i] /= scale; 
		}
		n1 = -n1;
		n2 = -n2;

		FT2D(a,b,work1,work2,&n1,&n2);

		datp = datpt + doffset;
		k = 0;
		for(i=1; i<=header[NTRAK]; i++) {
			for(j=1; j<=header[NCHAN]; j++) {
				if( 1 & (i+j)) 
					*(datp++) = -a[k]; 	/* for optical rather than standard ordering */
				else
					*(datp++) = a[k]; 
				k++;
			}
		}
		setarrow();	
		return 0;

	}

	if( n == 8) {
		if( !is_fft_memory ) {
			beep();
			printf("No FFT. Can't do inverse.\n");
			setarrow();
			return -5;
		}
		if( n1 != header[NCHAN] || n2 != header[NTRAK] ) {
			beep();
			printf("Size of FFT Does Not Match Current Image.\n");
			setarrow();
			return -6;
		}

		printf("Using Input as a Filter.\n");

		for(i=0; i<header[NTRAK]*header[NCHAN]; i++) {
			a[i] = a[i] * (*(datp)) / scale;
			b[i] = b[i] * (*(datp++)) / scale; 
		}
		n1 = -n1;
		n2 = -n2;

		FT2D(a,b,work1,work2,&n1,&n2);

		datp = datpt + doffset;
		k = 0;
		for(i=1; i<=header[NTRAK]; i++) {
			for(j=1; j<=header[NCHAN]; j++) {
				if( 1 & (i+j)) 
					*(datp++) = -a[k]; 	/* for optical rather than standard ordering */
				else
					*(datp++) = a[k]; 
				k++;
			}
		}
		setarrow();	
		return 0;

	}
	/* --------------------------------------------------------- */
	/*				End of Inverse Transform Cases				 */
	/* --------------------------------------------------------- */

	if( n1 != header[NCHAN] || n2 != header[NTRAK] ) {
		if( is_fft_memory) {		/* sizes have changed, have to reallocate */
			free(a);
			free(b);
			free(work1);
			free(work2);
			is_fft_memory = false;
		}
	}


	n1 = header[NCHAN];
	n2 = header[NTRAK];

	
	if( !is_fft_memory ) {
		a = (float *)malloc(size*4);
		b = (float *)malloc(size*4);
		work1 = (float *)malloc(n2*4);
		work2 = (float *)malloc(n2*4);
		if( a==0 || b==0 || work1==0 || work2 == 0) {
			nomemory();
			return -1;
		}
		else {
			is_fft_memory = true;
		}
	}
	
	k = 0;
	for(i=1; i<=header[NTRAK]; i++) {
		for(j=1; j<=header[NCHAN]; j++) {
			if( 1 & (i+j)) 
				a[k] =  -(*(datp++)); 	/* for optical rather than standard ordering */
			else
				a[k] = *(datp++);
			b[k] = 0.0; 
			k++;
		}
	}


	FT2D(a,b,work1,work2,&n1,&n2);
	
	datp = datpt + doffset;
	
	switch (n) {
		case 0:
			for(i=0; i<header[NTRAK]*header[NCHAN]; i++) {
				*(datp++) = sqrt(a[i]*a[i] + b[i]*b[i])*scale; 	
			}
			break;
		case 1:
			for(i=0; i<header[NTRAK]*header[NCHAN]; i++) {
				*(datp++) = a[i]*scale; 	
			}
			break;
		case 2:
			for(i=0; i<header[NTRAK]*header[NCHAN]; i++) {
				*(datp++) = b[i]*scale; 	
			}
			break;
		case 5:
			for(i=0; i<header[NTRAK]*header[NCHAN]; i++) {
				*(datp++) = log(a[i]*a[i] + b[i]*b[i])*scale; 	
			}
			break;
		case 6:
			for(i=0; i<header[NTRAK]*header[NCHAN]; i++) {
				*(datp++) = (a[i]*a[i] + b[i]*b[i])*scale; 	
			}
			break;
		case 7:
			k = 0;
			for(i=1; i<=header[NTRAK]; i++) {
				for(j=1; j<=header[NCHAN]; j++) {
					a[k] = (a[k]*a[k] + b[k]*b[k]); 
					if( 1 & (i+j)) 
						a[k] =  -a[k]; 	/* for optical rather than standard ordering */
					b[k] = 0.0; 
					k++;
				}
			}
			n1 = -n1;
			n2 = -n2;

			FT2D(a,b,work1,work2,&n1,&n2);
	
			for(i=0; i<header[NTRAK]*header[NCHAN]; i++) {
				*(datp++) = sqrt(a[i]*a[i] + b[i]*b[i])*scale; 	
			}
			break;
		case 9:
			k = 0;
			for(i=1; i<=header[NTRAK]; i++) {
				for(j=1; j<=header[NCHAN]; j++) {
					if( 1 & (i+j)) 
						b[k] =  -b[k]; 	/* for optical rather than standard ordering */
					k++;
				}
			}

			for(i=0; i<header[NTRAK]*header[NCHAN]; i++) {
				*(datp++) = b[i]*scale; 	
			}
			break;

	}	/* end switch */
			
	
	setarrow();	
	return 0;
}

int is_power_2(int i)			/* checks for power of 2 < 2^20 and >= 4 */
{
	int mask = 1;
	int bits_set = 0;
	int j;
	for(j=0; j<20; j++) {
		if( mask & i) 
			bits_set++;	
		mask = mask << 1;
	}
	if( bits_set != 1  || i<4)
		return(0);
	else
		return(1);
}

/*---------------------------------------------------------------------------*/

#define SMALL_VALUE -32767

int xloc[NPEAK];
int yloc[NPEAK]; 
int biggies[NPEAK];

int getpeak(n,index)
int n,index;

/* 	find peaks in the data */
/*	PEAK n m rad
	find n peaks n>0 -> largest, [ n<0 -> smallest -- not implemented yet ]
	if m is specified, find the peaks associated within an m x m pixel area.
	determine the centroid and return the average.
	rad is the exclude radius.
	
	Notes: may miss some points in the process of sorting and eliminating
	       points that belong to the same peak -- for example, find the 2
		   largest points in the following would be a problem:
		   
		   xxx
		   
		   xxxx
		   xxxxxxxxxxxxx
		   xxxxxxxx
		   
		   
		   xx
		   
		   To try to avoid this, keep track of more points than eventually asked for.
		   Not sure how rigorus this is. -- number of extra points needed depends
		   on the value of m -- something like the area of a single point - 
		   3.14*m^2 [ * (n-1)]
		   For now, just use all 128 in array.
*/

{
	int i,m,rad;
		
	Point substart,subend;
	
	int peaks(TWOBYTE,TWOBYTE,int,int,DATAWORD*);
	
	extern int	doffset;
	extern DATAWORD *datpt;		/* the data pointer */
	extern DATAWORD idat(int,int);
	extern TWOBYTE header[];
	extern char cmnd[];


	/* Check to see if there was a second argument */
	
	m = 0;
	rad = 1;
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d %d",&n,&m,&rad);
			break;
		}
	}
	
	/* clear the central point(s) --- this should be made optional */
	
	 /* *( header[NCHAN]/2 + header[NTRAK]/2*header[NTRAK] + datpt + doffset) = 0; */
	
	
	
	/* go and get the peaks */
	
	peaks(header[NCHAN],header[NTRAK],m,rad,datpt+doffset);
	
	if( m == 0) {
		for (i= 0; i< n; i++) {
			pprintf(" %d\t%d\t%d\t%d\n",i+1,idat(yloc[i],xloc[i]),xloc[i],yloc[i]);
		}
	} else {
		for (i= 0; i< n; i++) {
			substart.h = xloc[i];
			substart.v = yloc[i];
			subend.h = xloc[i]+m-1;
			subend.v = yloc[i]+m-1;
			calc(&substart,&subend);
		}
	}
	setarrow();
	return 0;
}


/* find the peaks */

int peaks(TWOBYTE nx,TWOBYTE ny,int m,int rad,DATAWORD* dpt)
/*  m is the number of pixels to average over */
/* rad is the "exclude peaks" radius */
{

	int ic,it,i,j,k;
	DATAWORD datval;
	int dist;

	for(i=0; i<NPEAK; i++)
		biggies[i] = xloc[i] = yloc[i] = SMALL_VALUE;

	if( m == 0 ) {    /* consider single pixels of the entire image -- edges included */
		for (it= 0; it< ny; it++) {
			for (ic= 0; ic< nx; ic++) {
				datval = *dpt++;
				if( datval > biggies[NPEAK-1] ) {		/* if this pt is greater than the smallest of the big ones */
					for(j = 0; j < NPEAK; j++) {
						if( datval > biggies[j] ) {
							for( k = NPEAK-1; k > j; k-- ) {	/* push the others down one */
								biggies[k] = biggies[k-1];
								xloc[k] = xloc[k-1];
								yloc[k] = yloc[k-1];
							}
							biggies[j] = datval;
							xloc[j] = ic;
							yloc[j] = it;
							break;
						}
					}
				}
			}
		}
	} else {		/* exclude outside edge all round -- sum in m x m area */
		for (it= m; it< ny-m; it++) {
			for (ic= m; ic< nx-m; ic++) {
				datval = 0;
				for( j=0; j<m; j++) {
					for( k=0; k<m; k++) {
						datval += *(dpt + (it+j)*nx + ic+k);
					}
				}
				
				/* if this pt is greater than the smallest of the big ones */
				if( datval > biggies[NPEAK-1] ) {	
					/* see if this point is close to any of the others in the list */
					for(j = 0; j < NPEAK; j++) {
						dist = (xloc[j]-ic)*(xloc[j]-ic) + (yloc[j]-it)*(yloc[j]-it);
						/* if it is close */
						if( dist <= rad*rad ) {
							/* if the value isn't bigger forget it */
							if( datval <= biggies[j] ){
								goto forget_it;
							}
							/* otherwise, get rid of the old point close by */
							/* this is done by moving the others up */
							for( k=j; k< NPEAK-1; k++ ) {
								biggies[k] = biggies[k+1];
								xloc[k] = xloc[k+1];
								yloc[k] = yloc[k+1];
							}
							/*also erase the last*/
							biggies[NPEAK-1] = xloc[NPEAK-1] = yloc[NPEAK-1] = SMALL_VALUE;

						}
					}
					
					/* put the point in its proper place in the list */
					for(j = 0; j < NPEAK; j++) {
						if( datval > biggies[j] ) {
							for( k = NPEAK-1; k > j; k-- ) {	/* push the others down one */
								biggies[k] = biggies[k-1];
								xloc[k] = xloc[k-1];
								yloc[k] = yloc[k-1];
							}
							biggies[j] = datval;
							xloc[j] = ic;
							yloc[j] = it;
							break;
						}
					}
				}
forget_it:	;
			}
		}
	
	}
	return 0;
	
}

//
int get_vect_dir(float x0,float y0,float *x1,float *y1)
{
    switch( plot_dir_pref ) {
        case POS_X:
            if( *x1 < x0 ) {
                *x1 = 2*x0 - *x1;
                *y1 = 2*y0 - *y1;
            }
            break;
        case NEG_X:
            if( x0 < *x1 ) {
                *x1 = 2*x0 - *x1;
                *y1 = 2*y0 - *y1;
            }
            break;
        case POS_Y:
            if( *y1 < y0 ) {
                *x1 = 2*x0 - *x1;
                *y1 = 2*y0 - *y1;
            }
            break;
        case NEG_Y:
            if( y0 < *y1 ) {
                *x1 = 2*x0 - *x1;
                *y1 = 2*y0 - *y1;
            }
            break;
        default:
            break;
    }
    return 0;
}

/* ************** */

int piv(int n, int index)
{
    extern DATAWORD *datpt;
    extern int	doffset;
    extern int stopstatus;
    extern int histo_array[];
    extern char cmnd[];
    extern TWOBYTE header[];

    float 	*ap,*bp,*work1p,*work2p;
    float max,v,x0=0,y0=0,x1=0,y1=0,scale=0,dx,dy,vmag;
    DATAWORD loval, hival;
    int fftarraysize;
    TWOBYTE nc_nt;
    int i,j=0,k,n2,rad,histmax,binsize,ni,numctr=0,ix,iy,min;
    int ulx,uly;
    int mystopstatus;
    int num = 0;
    int sum_rad;
    /* conditional velocity variables and function */
    int condtnl();
    int c_val; /* flag for deteriming whether conditional velocity
        should be evaluated          */
    int backfill;  /* flag for filling in background for cond. vel. */
    DATAWORD *imagedat,*fftdat,*bcopy=0,datval,dvmin,dvmax;
    float dval, dval_sum;
    /* Check to see if there was a second argument */
    rad = n;
    for ( i = index; cmnd[i] != EOL; i++) {
        if(cmnd[i] == ' ') {
            sscanf(&cmnd[index],"%d %d",&n,&rad);
            break;
        }
    }
    sum_area = n;
    exclude_radius = rad;
    n2 = fftsize;			/* the number of rows in the fft sub box */
    nc_nt = fftsize;
    fftarraysize = fftsize * fftsize;
    if( plotduringpiv) {
        /* setup to plot results as we go */
        if( !pivplot(INIT,0.0,0.0,0.0,0.0,0) ) {
            beep();
            return -1;
        }
        scale = pivscale - 1.0;
    }
    /* calculate how many vectors will be found */
    no_of_velx = (header[NCHAN]-fftsize)/boxinc +1;
    no_of_vely = (header[NTRAK]-fftsize)/boxinc +1;
    no_of_vel = 0;	/* reset to indicate vectors are not along contour */
    pprintf("%d by %d\n",no_of_velx,no_of_vely);
    /* allocate memory */
    if(xpeaks != 0) free(xpeaks);		/* get rid of old data if there is any */
    if(ypeaks != 0) free(ypeaks);
    if(peakval != 0) free(peakval);
    if(dirval != 0) free(dirval);
    ap = (float *)malloc(fftarraysize*4);
    bp = (float *)malloc(fftarraysize*4);
    work1p = (float *)malloc(n2*4);
    work2p = (float *)malloc(n2*4);
    fftdat = (DATAWORD*)malloc(fftarraysize*DATABYTES);
    xpeaks = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
    ypeaks = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
    peakval = (PEAKVAL_TYPE*)malloc(no_of_velx * no_of_vely * peaks_to_save * PEAKVAL_SIZE);
    dirval = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
    if( ap==0 || bp==0 || work1p==0 || work2p == 0 || fftdat == 0
        || xpeaks == 0 || ypeaks == 0 || peakval == 0 || dirval == 0 ) {
        nomemory();
        if(ap != 0) free(ap);
        if(bp != 0) free(bp);
        if(work1p != 0) free(work1p);
        if(work2p != 0) free(work2p);
        if(fftdat != 0) free(fftdat);
        if(xpeaks != 0) free(xpeaks);
        if(ypeaks != 0) free(ypeaks);
        if(peakval != 0) free(peakval);
        if(dirval != 0) free(dirval);
        if( plotduringpiv) pivplot(CLOSE,x0,y0,x1,y1,0);
        return -1;
    }
    
    mystopstatus = stopstatus;
    /* get the next subarray of the current image */
    for(uly = 0; uly <= header[NTRAK] - fftsize; uly += boxinc){
        for(ulx = 0; ulx <= header[NCHAN] - fftsize; ulx += boxinc){
            /* print the location of this box */
            printxyzstuff(ulx,uly,0);
            printxyzstuff(ulx+fftsize-1,uly+fftsize-1,1);
            x0 = ulx + fftsize/2;		/* the center of this box (used in plotting) */
            y0 = uly + fftsize/2;
            /* check binary image in background buffer to see if velocity should be evaluated */
            if (c_flag){                          /* Are we doing conditional velocities? */
                c_val = condtnl(ulx,uly,fftsize);
                if (!c_val){		/* if velocity is not to be found, set v = 0 */
                    for(k=0; k<peaks_to_save; k++){
                        xpeaks[num] = fftsize/2;
                        ypeaks[num] = fftsize/2;
                        peakval[num] = 1000;
                        num++;
                    }
                    goto skip;
                }
            }
            /* copy the subarray data into an array, get the min and max */
            imagedat = datpt + doffset + ulx + uly*header[NCHAN] -1 ;	/* first data pt */
            if (c_flag && localavg != 0) {
                backfill = 1;
                bcopy = backdat + doffset + ulx + uly*header[NCHAN] -1;
            }else {
                backfill = 0;
            }
            k = 0;
            dvmax = dvmin = *(imagedat+1);
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    if (backfill && *(bcopy+j) == 0) /* fill in background with localavg where necessary */
                        datval = localavg;
                    else
                        datval = *(imagedat+j);
                    if( datval > dvmax) dvmax = datval;
                    if( datval < dvmin) dvmin = datval;
                    fftdat[k] = datval;
                    k++;
                }
                imagedat += header[NCHAN];
                if (backfill)
                    bcopy += header[NCHAN];
            }
            if( autoclip ) {
                /* get the histogram of the current subarray */
                binsize = data_histogram(fftdat,dvmin,dvmax,fftarraysize);
                /* find the maximum of the histogram, exclude the extreme points */
                histmax = 0;
                for(i=1; i< HISTOSIZE-1; i++) {
                    if( histo_array[i] > histmax ){
                        histmax = histo_array[i];
                        j = i;
                    }
                }
                loval = loclip * histmax;
                hival = hiclip * histmax;
                for(i=0; i< HISTOSIZE-1; i++) {
                    if(histo_array[i] > loval)
                        break;
                }
                printf("%d  %d  %d val, histmax,\n",
                         j*binsize + dvmin,histmax,i);
                loval = i*binsize + dvmin;
                for(i=HISTOSIZE-2; i > 0; i--) {
                    if(histo_array[i] > hival)
                        break;
                }
                hival = i*binsize + dvmin;
                printf("Peak at: %d Floor: %d Ceiling: %d\n",
                         j*binsize + dvmin,loval,hival);
                setbounds(fftdat,loval,hival,fftarraysize);
            }
            /* load up the real arrays with the subarray data */
            k = 0;
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    if( 1 & (i+j))
                        ap[k] =  -fftdat[k]; 	/* for optical rather than standard ordering */
                    else
                        ap[k] = fftdat[k];
                    bp[k] = 0.0;
                    k++;
                }
            }
            
            /* do the initial fft */
            FT2D(ap,bp,work1p,work2p,&n2,&n2);
            
            /* are we finding the direction? */
            if(plot_dir_pref == AUTO_DIR) {
                if( imagin == 0 ) {
                    /* allocate the arrays */
                    imagin = (float *)malloc(fftsize*fftsize * sizeof(float));
                    if( imagin==0) {
                        nomemory();
                        plot_dir_pref = NO_PREF;
                        goto past_auto_dir;
                    }
                }
                /* save the imaginary part */
                for(j=0; j<fftsize*fftsize; j++) {
                    imagin[j] = bp[j];
                }
            }
past_auto_dir:
                /* get the magnitude squared and play with signs */
                k = 0;
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    if( 1 & (i+j))
                        ap[k] =  -ap[k]*ap[k] - bp[k]*bp[k]; 	/* for optical rather than standard ordering */
                    else
                        ap[k] = ap[k]*ap[k] + bp[k]*bp[k];
                    bp[k] = 0.0;
                    k++;
                }
            }
            
            /* do the second fft [inverse] */
            ni = -n2;
            FT2D(ap,bp,work1p,work2p,&ni,&ni);
            
            /* get the magnitude and the maximum */
            max = 0.0;
            for(i=0; i<fftarraysize; i++) {
                ap[i] = sqrt(ap[i]*ap[i] + bp[i]*bp[i]);
                if( ap[i] > max )
                    max = ap[i];
            }
            sprintf(sstring," Max is %f.\n",max);
            printf(sstring);
#ifdef FLOAT
            for(i=0; i<fftarraysize; i++) {
                fftdat[i] = ap[i];
            }

#else
            for(i=0; i<fftarraysize; i++) {
                fftdat[i] = ap[i]*MAXDATAVALUE/max;
            }
#endif                        
            /* find peaks -- */
            peaks(nc_nt,nc_nt/2+2*n,n,rad,fftdat);	 /* search half the image (plus a bit) */
            /* elimnate duplicate points */
            k = NPEAK;
            /*
             for(i=0; i< k-1; i++) {
                 if(biggies[i] == biggies[i+1]) {
                     for(j=i+1; j<k-1; j++) {
                         biggies[j] = biggies[j+1];
                         xloc[j] = xloc[j+1];
                         yloc[j] = yloc[j+1];
                     }
                     k--;
                 }
             }
             */
            sum_rad = 4;
            /*			biggies[0] = max;
            yloc[0] = xloc[0] = fftsize/2;
             */
            /* find maximums outside first  max */
            /* NEEDS TO BE MODIFIED TO LOOK FOR MORE THAN ONE MAX OUTSIDE CENTRAL PEAK */
            /*			for(k=1; k<peaks_to_save; k++){
                biggies[k] = 0;
                for(i=0; i<fftsize; i++){
                    for(j=0; j<fftsize; j++){
                        dval = *(ap + (i)*fftsize + j);
                        if((i<yloc[k-1]-sum_rad || i>yloc[k-1]+sum_rad) && (j<xloc[k-1]-sum_rad || j>xloc[k-1]+sum_rad)){
                            if(dval>biggies[k]){
                                biggies[k] = dval;
                                yloc[k] = i;
                                xloc[k] = j;
                            }
                        }
                    }
                }
            }
             */
            for(k=0; k<peaks_to_save; k++){
                /* calculate x & y center of mass -- use this for the actual peak location */
                x1 = y1 = 0.0;
                dval_sum = 0;
                min = max;
                /* find local minimum */
                for(i=yloc[k]; i<yloc[k]+sum_area; i++) {
                    for(j=xloc[k]; j<xloc[k]+sum_area; j++) {
                        if(i<0) i = 0;
                        if(j<0) j = 0;
                        if(i>fftsize-1) i =fftsize-1;
                        if(j>fftsize-1) j =fftsize-1;
                        dval = *(ap + (i)*fftsize + j);
                        if(dval<min)
                            min = dval;
                    }
                }
                for(i=yloc[k]; i<yloc[k]+sum_area; i++) {
                    for(j=xloc[k]; j<xloc[k]+sum_area; j++) {
                        if(i<0) i = 0;
                        if(j<0) j = 0;
                        if(i>fftsize-1) i =fftsize-1;
                        if(j>fftsize-1) j =fftsize-1;
                        dval = *(ap + (i)*fftsize + j) - min;
                        /*		printf("dval = %.2f\n",dval);  */
                        dval_sum += dval;
                        x1 += j * dval;			/* x center of mass */
                        y1 += i * dval;			/* y center of mass */
                    }
                }
                x1 /= dval_sum;
                y1 /= dval_sum;
                sprintf(sstring,"%.2f\t%.2f",x1,y1);
                printf(sstring);
                if(n>0){
                    printf("\t%d\t",biggies[k]/n/n);
                }else{
                    printf("\t%d\t",biggies[k]);
                }
                xpeaks[num] = x1;
                ypeaks[num] = y1;
                peakval[num] = biggies[k];
                /* are we finding the direction? */
                if(plot_dir_pref == AUTO_DIR) {
                    if( k > 0 ) {
                        /* this version of the direction finder looks at the imaginary part
                        of the first FFT in a region determined by the direction and
                         magnitude of the velocity.  As written it looks in a 3 x 3 pixel
                         area surrounding the pixel in the center of the region. It doesn't
                         seem to work very well on test data. Looking at a single pixel
                         or at a few pixels in the direction of the vector didn't work
                         any better.
                         */
#ifdef Direction_Finder_That_Doesnt_Work
                        dx = xpeaks[num] - xpeaks[numctr];
                        dy = ypeaks[num] - ypeaks[numctr];
                        vmag = sqrt(dx*dx + dy* dy);	/* the magnitude of the velocity */
                        v = fftsize/2.0/vmag;			/* the distance in wave number space
                            (in pixels) to look for the gradient */
                        ix = v * dx/vmag + .5 + fftsize/2;
                        iy = v * dy/vmag + .5 + fftsize/2;
                        dirval[num] = 0.0;
                        for( i = -1; i <= 1; i++) {
                            for(j = -1; j<= 1; j++) {
                                dirval[num] += imagin[ ix+j + (iy+i)*fftsize];
                            }
                        }
                        if( k == 1) {
                            printf("\npoint %d %d is center of region.\n",ix,iy);
                            sprintf(sstring,"dirval is %f.\n",dirval[num]);
                            printf(sstring);
                        }
                        /* this version of the direction finder looks at the imaginary part
                            of the first FFT in a region determined by the direction and
                         magnitude of the velocity.  It is similar to the one above but
                         looks at single points at equal spacing in k space. Like looking
                         at a constant phase with respect to the  diffraction pattern.  It
                         also doesn't work well on the synthetic test data.
                         */
                        dx = xpeaks[num] - xpeaks[numctr];
                        dy = ypeaks[num] - ypeaks[numctr];
                        vmag = sqrt(dx*dx + dy* dy);	/* the magnitude of the velocity */
                        v = fftsize/1.0/vmag;			/* the distance in wave number space
                            (in pixels) to look for the gradient */
                        dirval[num] = 0.0;
                        i = 1;
                        ix = v * dx/vmag * i + .5 + fftsize/2;
                        iy = v * dy/vmag * i + .5 + fftsize/2;
                        while( ix < fftsize && iy < fftsize && ix >= 0 && iy >= 0) {
                            dirval[num] += imagin[ ix + iy*fftsize];
                            i+= 1;
                            /* if( k == 1) {
                                printf("\npoint %d %d is center of region.",ix,iy);
                            } */
                            ix = v * dx/vmag * i + .5 + fftsize/2;
                            iy = v * dy/vmag * i + .5 + fftsize/2;
                        }
                        if( k == 1) {
                            sprintf(sstring,"dirval is %f.\n",dirval[num]);
                            printf(sstring);
                        }
#endif
                        dx = xpeaks[num] - xpeaks[numctr];
                        dy = ypeaks[num] - ypeaks[numctr];
                        vmag = sqrt(dx*dx + dy* dy);	/* the magnitude of the velocity */
                        dirval[num] = 0.0;
                        ix = fftsize/2;
                        iy = fftsize/2;
                        x1 = ix;
                        y1 = iy;
                        while( ix < fftsize && iy < fftsize && ix >= 0 && iy >= 0) {
                            dirval[num] += imagin[ ix + iy*fftsize];
                            x1 += dx/vmag;
                            y1 += dy/vmag;
                            ix = x1;
                            iy = y1;
                        }
                        if( k == 1) {
                            sprintf(sstring,"dirval is %f.\n",dirval[num]);
                            printf(sstring);
                        }
                        /**/
                    } else {
                        dirval[num] = 0.0;
                        numctr = num;
                        /* the first one doesn't count since it takes 2 peaks
                            to make a vector */
                    }
                }
                num++;
            }
skip:
                /* find velocities */
                x1 = ulx + xpeaks[num-peaks_to_save+1];
            y1 = uly + ypeaks[num-peaks_to_save+1];
            v = (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0);
            v = sqrt(v);
            sprintf(sstring,"\nv =\t%.2f\n",v);
            printf(sstring);
            if( plotduringpiv) {
                x1 = scale*(xpeaks[num-peaks_to_save+1] - fftsize/2);
                y1 = scale*(ypeaks[num-peaks_to_save+1] - fftsize/2);
                x1 = ulx + xpeaks[num-peaks_to_save+1] + x1;
                y1 = uly + ypeaks[num-peaks_to_save+1] + y1;
                get_vect_dir(x0,y0,&x1,&y1);
                pivplot(PLOT,x0,y0,x1,y1,1);
            }
            checkstoprequest();
            if( mystopstatus != stopstatus) {
                pprintf("piv command stopped.\n");
                if(uly == 0) {					/* still in the first row */
                    no_of_vely = 1;
                    no_of_velx = (ulx)/boxinc + 1;
                }
                else {
                    no_of_vely = (uly)/boxinc;
                }
                printf("New velocity array size is %d by %d.\n",no_of_velx,no_of_vely);
                goto loopexit;
            }
            if( show_auto ) { /* replace image data with auto-correlation and exit */
                keylimit(-1);
                multiply(0,0);
                sprintf(cmnd,"%d %d",fftsize,fftsize);
                maknew(1,0);
                k = 0;
                min = fftdat[0];
                for(i=0; i<fftarraysize; i++) {
                    if(fftdat[i] < min) min = fftdat[i];
                }
                for(i=0; i<fftarraysize; i++) {
                    *(datpt+doffset+i) = fftdat[i]-min;
                }
                have_max = false;
                maxx();
                keylimit(-2);
                goto loopexit;
            }
        } /* loop to next subarray */
    }
loopexit:
        free(ap);
    free(bp);
    free(work1p);
    free(work2p);
    free(fftdat);
    if( plotduringpiv) pivplot(CLOSE,x0,y0,x1,y1,0);
    setarrow();
	return 0;
}


int oldpiv(int n,int index)
{
    extern DATAWORD *datpt;
    extern int	doffset;
    extern int stopstatus;
    extern int histo_array[];
    extern char cmnd[];
    extern TWOBYTE header[];
    float 	*ap,*bp,*work1p,*work2p;
    float max,v,x0=0,y0=0,x1=0,y1=0,scale=1,dx,dy,vmag;
    DATAWORD loval, hival;
    int fftarraysize;
    TWOBYTE nc_nt;
    int i,j=0,k,n2,rad,histmax,binsize,ni,numctr=0,ix,iy;
    int ulx,uly;
    int mystopstatus;
    int num = 0;
    DATAWORD *imagedat,*fftdat,datval,dvmin,dvmax;
    /* Check to see if there was a second argument */
    rad = n;
    for ( i = index; cmnd[i] != EOL; i++) {
        if(cmnd[i] == ' ') {
            sscanf(&cmnd[index],"%d %d",&n,&rad);
            break;
        }
    }
    sum_area = n;
    exclude_radius = rad;
    n2 = fftsize;			/* the number of rows in the fft sub box */
    nc_nt = fftsize;
    fftarraysize = fftsize * fftsize;
    if( plotduringpiv) {
        /* setup to plot results as we go */
        if( !pivplot(INIT,0.0,0.0,0.0,0.0,0) ) {
            beep();
            return -1;
        }
        scale = pivscale - 1.0;
    }
    /* calculate how many vectors will be found */
    no_of_velx = (header[NCHAN]-fftsize)/boxinc +1;
    no_of_vely = (header[NTRAK]-fftsize)/boxinc +1;
    printf("%d by %d\n",no_of_velx,no_of_vely);
    /* allocate memory */
    if(xpeaks != 0) free(xpeaks);		/* get rid of old data if there is any */
    if(ypeaks != 0) free(ypeaks);
    if(peakval != 0) free(peakval);
    if(dirval != 0) free(dirval);
    ap = (float *)malloc(fftarraysize*4);
    bp = (float *)malloc(fftarraysize*4);
    work1p = (float *)malloc(n2*4);
    work2p = (float *)malloc(n2*4);
    fftdat = (DATAWORD*)malloc(fftarraysize*DATABYTES);
    xpeaks = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
    ypeaks = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
    peakval = (PEAKVAL_TYPE*)malloc(no_of_velx * no_of_vely * peaks_to_save * PEAKVAL_SIZE);
    dirval = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
    if( ap==0 || bp==0 || work1p==0 || work2p == 0 || fftdat == 0
        || xpeaks == 0 || ypeaks == 0 || peakval == 0 || dirval == 0 ) {
        nomemory();
        if(ap != 0) free(ap);
        if(bp != 0) free(bp);
        if(work1p != 0) free(work1p);
        if(work2p != 0) free(work2p);
        if(fftdat != 0) free(fftdat);
        if(xpeaks != 0) free(xpeaks);
        if(ypeaks != 0) free(ypeaks);
        if(peakval != 0) free(peakval);
        if(dirval != 0) free(dirval);
        if( plotduringpiv) pivplot(CLOSE,0.0,0.0,0.0,0.0,0);
        return -2;
    }
    
    //	cross_correlation = 0;
    mystopstatus = stopstatus;
    /* get the next subarray of the current image */
    for(uly = 0; uly <= header[NTRAK] - fftsize; uly += boxinc){
        for(ulx = 0; ulx <= header[NCHAN] - fftsize; ulx += boxinc){
            /* print the location of this box */
            printxyzstuff(ulx,uly,0);
            printxyzstuff(ulx+fftsize-1,uly+fftsize-1,1);
            x0 = ulx + fftsize/2;		/* the center of this box (used in plotting) */
            y0 = uly + fftsize/2;
            /* copy the subarray data into an array, get the min and max */
            imagedat = datpt + doffset + ulx + uly*header[NCHAN] -1 ;	/* first data pt */
            k = 0;
            dvmax = dvmin = *(imagedat+1);
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    datval = *(imagedat+j);
                    if( datval > dvmax) dvmax = datval;
                    if( datval < dvmin) dvmin = datval;
                    fftdat[k] = datval;
                    k++;
                }
                imagedat += header[NCHAN];
            }
            if( autoclip ) {
                /* get the histogram of the current subarray */
                binsize = data_histogram(fftdat,dvmin,dvmax,fftarraysize);
                /* find the maximum of the histogram, exclude the extreme points */
                histmax = 0;
                for(i=1; i< HISTOSIZE-1; i++) {
                    if( histo_array[i] > histmax ){
                        histmax = histo_array[i];
                        j = i;
                    }
                }
                loval = loclip * histmax;
                hival = hiclip * histmax;
                for(i=0; i< HISTOSIZE-1; i++) {
                    if(histo_array[i] > loval)
                        break;
                }
                printf("%d  %d  %d val, histmax,\n",
                       j*binsize + dvmin,histmax,i);
                loval = i*binsize + dvmin;
                for(i=HISTOSIZE-2; i > 0; i--) {
                    if(histo_array[i] > hival)
                        break;
                }
                hival = i*binsize + dvmin;
                printf("Peak at: %d Floor: %d Ceiling: %d\n",
                       j*binsize + dvmin,loval,hival);
                setbounds(fftdat,loval,hival,fftarraysize);
            }

            /* load up the real arrays with the subarray data */
            k = 0;
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    if( 1 & (i+j))
                        ap[k] =  -fftdat[k]; 	/* for optical rather than standard ordering */
                    else
                        ap[k] = fftdat[k];
                    bp[k] = 0.0;
                    k++;
                }
            }
            
            /* do the initial fft */
            FT2D(ap,bp,work1p,work2p,&n2,&n2);
            
            /* are we finding the direction? */
            if(plot_dir_pref == AUTO_DIR) {
                if( imagin == 0 ) {
                    /* allocate the arrays */
                    imagin = (float *)malloc(fftsize*fftsize * 4);
                    if( imagin==0) {
                        nomemory();
                        plot_dir_pref = NO_PREF;
                        goto past_auto_dir;
                    }
                }
                /* save the imaginary part */
                for(j=0; j<fftsize*fftsize; j++) {
                    imagin[j] = bp[j];
                }
            }
past_auto_dir:
                /* get the magnitude squared and play with signs */
                k = 0;
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    if( 1 & (i+j))
                        ap[k] =  -ap[k]*ap[k] - bp[k]*bp[k]; 	/* for optical rather than standard ordering */
                    else
                        ap[k] = ap[k]*ap[k] + bp[k]*bp[k];
                    bp[k] = 0.0;
                    k++;
                }
            }
            
            /* do the second fft [inverse] */
            ni = -n2;
            FT2D(ap,bp,work1p,work2p,&ni,&ni);
            
            /* get the magnitude and the maximum */
            max = 0.0;
            for(i=0; i<fftarraysize; i++) {
                ap[i] = sqrt(ap[i]*ap[i] + bp[i]*bp[i]);
                if( ap[i] > max )
                    max = ap[i];
            }
            //printf(" Max is %f.\n",max);
#ifdef FLOAT
            for(i=0; i<fftarraysize; i++) {
                fftdat[i] = ap[i];
            }

#else
            for(i=0; i<fftarraysize; i++) {
                fftdat[i] = ap[i]*MAXDATAVALUE/max;
            }
#endif            
            /* find peaks -- */
            peaks(nc_nt,nc_nt/2+2*n,n,rad,fftdat);	/* search half the image (plus a bit) */
            /* elimnate duplicate points */
            k = NPEAK;
            /*
             for(i=0; i< k-1; i++) {
                 if(biggies[i] == biggies[i+1]) {
                     for(j=i+1; j<k-1; j++) {
                         biggies[j] = biggies[j+1];
                         xloc[j] = xloc[j+1];
                         yloc[j] = yloc[j+1];
                     }
                     k--;
                 }
             }
             */
            for(k=0; k<peaks_to_save; k++){
                /* calculate x & y center of mass -- use this for the actual peak location */
                x1 = y1 = 0.0;
                for(i=yloc[k]; i<yloc[k]+n; i++) {
                    for(j=xloc[k]; j<xloc[k]+n; j++) {
                        datval = *(fftdat + (i)*fftsize + j);
                        x1 += j * datval;			/* x center of mass */
                        y1 += i * datval;			/* y center of mass */
                    }
                }
                x1 /= biggies[k];
                y1 /= biggies[k];
                /*
                 pprintf("%.2f\t%.2f",x1,y1);
                 pprintf("\t%d\t",biggies[k]/n/n);
                 */
                xpeaks[num] = x1;
                ypeaks[num] = y1;
                peakval[num] = biggies[k];
                /* are we finding the direction? */
                if(plot_dir_pref == AUTO_DIR) {
                    if( k > 0 ) {
                        /* this version of the direction finder looks at the imaginary part
                        of the first FFT in a region determined by the direction and
                         magnitude of the velocity.  As written it looks in a 3 x 3 pixel
                         area surrounding the pixel in the center of the region. It doesn't
                         seem to work very well on test data. Looking at a single pixel
                         or at a few pixels in the direction of the vector didn't work
                         any better.
                         */
#ifdef Direction_Finder_That_Doesnt_Work
                        dx = xpeaks[num] - xpeaks[numctr];
                        dy = ypeaks[num] - ypeaks[numctr];
                        vmag = sqrt(dx*dx + dy* dy);	/* the magnitude of the velocity */
                        v = fftsize/2.0/vmag;			/* the distance in wave number space
                            (in pixels) to look for the gradient */
                        ix = v * dx/vmag + .5 + fftsize/2;
                        iy = v * dy/vmag + .5 + fftsize/2;
                        dirval[num] = 0.0;
                        for( i = -1; i <= 1; i++) {
                            for(j = -1; j<= 1; j++) {
                                dirval[num] += imagin[ ix+j + (iy+i)*fftsize];
                            }
                        }
                        if( k == 1) {
                            printf("\npoint %d %d is center of region.\n",ix,iy);
                            printf("dirval is %f.\n",dirval[num]);
                        }
                        /* this version of the direction finder looks at the imaginary part
                            of the first FFT in a region determined by the direction and
                         magnitude of the velocity.  It is similar to the one above but
                         looks at single points at equal spacing in k space. Like looking
                         at a constant phase with respect to the  diffraction pattern.  It
                         also doesn't work well on the synthetic test data.
                         */
                        dx = xpeaks[num] - xpeaks[numctr];
                        dy = ypeaks[num] - ypeaks[numctr];
                        vmag = sqrt(dx*dx + dy* dy);	/* the magnitude of the velocity */
                        v = fftsize/1.0/vmag;			/* the distance in wave number space
                            (in pixels) to look for the gradient */
                        dirval[num] = 0.0;
                        i = 1;
                        ix = v * dx/vmag * i + .5 + fftsize/2;
                        iy = v * dy/vmag * i + .5 + fftsize/2;
                        while( ix < fftsize && iy < fftsize && ix >= 0 && iy >= 0) {
                            dirval[num] += imagin[ ix + iy*fftsize];
                            i+= 1;
                            /* if( k == 1) {
                                printf("\npoint %d %d is center of region.",ix,iy);
                            } */
                            ix = v * dx/vmag * i + .5 + fftsize/2;
                            iy = v * dy/vmag * i + .5 + fftsize/2;
                        }
                        if( k == 1) {
                            printf("dirval is %f.\n",dirval[num]);
                        }
#endif
                        dx = xpeaks[num] - xpeaks[numctr];
                        dy = ypeaks[num] - ypeaks[numctr];
                        vmag = sqrt(dx*dx + dy* dy);	/* the magnitude of the velocity */
                        dirval[num] = 0.0;
                        ix = fftsize/2;
                        iy = fftsize/2;
                        x1 = ix;
                        y1 = iy;
                        while( ix < fftsize && iy < fftsize && ix >= 0 && iy >= 0) {
                            dirval[num] += imagin[ ix + iy*fftsize];
                            x1 += dx/vmag;
                            y1 += dy/vmag;
                            ix = x1;
                            iy = y1;
                        }
                        if( k == 1) {
                            printf("dirval is %f.\n",dirval[num]);
                        }
                        /**/
                    } else {
                        dirval[num] = 0.0;
                        numctr = num;
                        /* the first one doesn't count since it takes 2 peaks
                            to make a vector */
                    }
                }
                num++;
            }
            /* find velocities */
            x1 = ulx + xpeaks[num-peaks_to_save+1];
            y1 = uly + ypeaks[num-peaks_to_save+1];
            v = (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0);
            v = sqrt(v);
            pprintf("v =\t%.2f\tx=\t%.2f\ty=\t%.2f\n",
                    v,xpeaks[num-peaks_to_save+1],ypeaks[num-peaks_to_save+1]);
            if( plotduringpiv) {
                x1 = scale*(xpeaks[num-peaks_to_save+1] - fftsize/2);
                y1 = scale*(ypeaks[num-peaks_to_save+1] - fftsize/2);
                x1 = ulx + xpeaks[num-peaks_to_save+1] + x1;
                y1 = uly + ypeaks[num-peaks_to_save+1] + y1;
                pivplot(PLOT,x0,y0,x1,y1,1);
            }
            checkstoprequest();
            if( mystopstatus != stopstatus) {
                pprintf("piv command stopped.\n");
                if(uly == 0) {					/* still in the first row */
                    no_of_vely = 1;
                    no_of_velx = (ulx)/boxinc + 1;
                }
                else {
                    no_of_vely = (uly)/boxinc;
                }
                pprintf("New velocity array size is %d by %d.\n",no_of_velx,no_of_vely);
                goto loopexit;
            }
        }
    }
loopexit:
    free(ap);
    free(bp);
    free(work1p);
    free(work2p);
    free(fftdat);
    if( plotduringpiv) pivplot(CLOSE,x0,y0,x1,y1,0);
    setarrow();
    return 0;
}
/* ********** */


int pivsave(int n,int index)		/* save the data calculated by the last piv command in a file */
{

	int	i = 0;
	int m;
	FILE *fp;
	
	
    extern char     cmnd[],*fullname();
	
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern PEAKVAL_TYPE*  peakval;
	extern int fftsize;
	extern int boxinc;

	extern float* xpeaks;
	extern float* ypeaks;	

	
	n = 0;
	
	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	
	fp = fopen(fullname(&cmnd[index],SAVE_DATA),"w");
	
	if( fp != NULL) {
		fprintf( fp,"%d\t%d\t%d\tx locations, y locations, peaks/location\n",
					no_of_velx,no_of_vely,peaks_to_save);
		fprintf( fp,"%d\t%d\tfft size, increment\n",
					fftsize,boxinc);

		fprintf( fp,"x coord\ty coord\tvalue\n");
		m = 0;
        for( i=0; i< no_of_velx*no_of_vely; i++) {
			for(n=0; n<peaks_to_save; n++) {
#ifdef FLOAT
				fprintf( fp,"%f\t%f\t%g\t",xpeaks[m],ypeaks[m],peakval[m]);
#else
                fprintf( fp,"%f\t%f\t%d\t",xpeaks[m],ypeaks[m],peakval[m]);
#endif

				m++;
			}
			fprintf( fp,"\n");
        }
		
		fprintf( fp,"%d\t%d\tsum area, exclude radius\n",
					sum_area,exclude_radius);

    	fclose(fp);
	
		settext(&cmnd[index]);
		fileflush(&cmnd[index]);	/* for updating directory */
		
	}
	else {
		beep();
		return -2;
	}
	return 0;

}
/* ***************** */

int pivinc(int n)		/* set the increment for successive calcs in the PIV command  */		
{
	boxinc = n;	
	return 0;
}
/* ***************** */

int pivsiz(int n)		/* set the size of the fft done by the PIV command */
{
	if( !is_power_2(n) ) {
		beep();
		printf("PIVSIZ must be a power of 2.\n");
		return -1;
	}
	fftsize = n;
	return 0;	
}

/* ***************** */
/*
pivalt(n,flag)		// alter a point calculated by the PIV command 

int	n,flag;

{
	
	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}

#ifdef Mac
	alter_vectors = n;
	return 0;
	
#else

	piv_reorder(n,flag);
	return 0;
	
#endif

}
*/
/****************************************************************************
P I V A L T  -	Change the effect that a mouse click has when done
 in a LINEGRAPHICS PIV image
 ****************************************************************************/
int pivalt(int n, int flag)
{
    // Check for Valid PIV image
    if(no_of_velx == 0 && no_of_vel == 0) {
        beep();
        printf("Must have valid PIV image first.\n");
        return -1;
    }
    // 'alter_vectors' is a global parameter that gives determines what action is performed
    alter_vectors = n;
    
    // Print out a message to tell us what MODE we are now in
    switch (alter_vectors) {
        case 0:
            printf("INQUIRY mode\n");
            alter_vectors = 6;
            break;
        case 1:
            printf("CYCLE mode\n");
            break;
        case 2:
            printf("DELETE mode\n");
            break;
        case 3:
            printf("MASSAGE mode\n");
            break;
        case 4:
            printf("NEIGHBOURHOOD STATISTICS mode\n");
            break;
        case 5:
            printf("VECTOR ANGLE mode\n");
            break;
        default:
            printf("Unknown mode\n");
            break;
    }
    return 0;
}
// ************
int printvel(float x0,float y0,float x1,float y1)
{
    float v;

    v = (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0);
    v = sqrt(v);
    sprintf(sstring, "\t%.2f\t%.2f\t%.2f",x1,y1,v);
    printf(sstring);
    return 0;
}

// ************

int piv_reorder(int n,int flag)
{
    int i,offset,pv;
    float x,y;

    if( n == 0 || n > no_of_velx*no_of_vely) {
        beep();
        printf("Not a valid point.\n");
        return -1;
    }

    /* if n<0, replace peak 2 with a copy of peak 1 (will give 0 velocity) */
    if( n < 0 ){
        n = abs(n) - 1;
        offset = n*peaks_to_save;
        printf("Old velocity: ");
        printvel(xpeaks[offset],ypeaks[offset],xpeaks[offset+1],ypeaks[offset+1]);
        xpeaks[offset+1] = xpeaks[offset];
        ypeaks[offset+1] = ypeaks[offset];
        peakval[offset+1] = peakval[offset];
        if (flag) {
            printf("New velocity: ");
            printvel(xpeaks[offset],ypeaks[offset],xpeaks[offset+1],ypeaks[offset+1]);
        }
    } else {
        /* for the nth velocity, make the second peak the last peak */
        /* rotate all others up one position */
        offset = (n-1)*peaks_to_save;
        if( flag ) {
            printf("Old velocity: ");
            printvel(xpeaks[offset],ypeaks[offset],xpeaks[offset+1],ypeaks[offset+1]);
        }
        x = xpeaks[offset+1];
        y = ypeaks[offset+1];
        pv = peakval[offset+1];
        for(i=1; i< peaks_to_save-1; i++){
            xpeaks[offset+i] = xpeaks[offset+i+1];
            ypeaks[offset+i] = ypeaks[offset+i+1];
            peakval[offset+i] = peakval[offset+i+1];
        }
        xpeaks[offset+peaks_to_save-1] = x;
        ypeaks[offset+peaks_to_save-1] = y;
        peakval[offset+peaks_to_save-1] = pv;
        if( flag ) {
            printf("\t->\t");
            printvel(xpeaks[offset],ypeaks[offset],xpeaks[offset+1],ypeaks[offset+1]);
            printf("\n");
        }
    }
    return 0;
}


/* ***************** */

#define NO_OF_SETTINGS 7
int pivset(int n,int index)		/* some settings used by the PIV command */
{
    extern char scratch[];
    extern char cmnd[];
	
    static Cname dirprefs[] = {{"None  "},{"+X    "},{"-X    "},{"+Y    "},{"-Y    "},{"Auto  "}};
    int type = 1;
    int i,nch=0;

    /* Check to see if there was a second argument */
    for ( i = index; cmnd[i] != EOL; i++) {
        if(cmnd[i] == ' ') { 
            /* there is a second part to this, lets copy it */
            i++;	/* start after the space */
            for(nch = 0; cmnd[i] != EOL; nch++) {
                scratch[nch] = cmnd[i++];
            }
            scratch[nch] = EOL;
            index = -1;		/* a flag to say that we already have the argument */
            break;
        }
    }

    if( n == 0 ) {
        printf("FFT size for PIV processing is: %d\n",fftsize);
        printf("Increment for PIV processing is: %d\n",boxinc);
        printf("Setable parameters are:\n");
        printf("1. Plot Results Flag: %d\n", plotduringpiv);
        printf("2. Auto Clip Flag: %d\n", autoclip);
        if( autoclip)
            printf("   low_clip: %.2f    high_clip: %.2f\n", loclip,hiclip);
        printf("3. PIV Plot Scale Factor: %.2f\n", pivscale);
        printf("4. PIV Plot Direction Preference: %s.\n", dirprefs[plot_dir_pref].name);
        printf("5. Label Plot Flag: %d\n", labelplot);
        printf("6. Shift in XCorr: %d  %d\n", shiftx,shifty);
        printf("7. Conditional Velocity Flag: %d\n\n", c_flag);
        printf("Type Setting to Change. ");
        getlin(scratch);
        type = 0;
        sscanf(scratch,"%d",&type);
        if( type < 1 || type > NO_OF_SETTINGS) return -1;
    } else {
        type = n;
    }
    switch( type ) {
        case 1:
            if( plotduringpiv == 0)
                plotduringpiv = 1;
            else
                plotduringpiv = 0; 
            break;
        case 2:
            if( index != -1) {
                printf("Type value of Auto Clip Flag [low_clip high_clip]. ");
                getlin(scratch);
            }
            n = sscanf(scratch,"%d %f %f",&autoclip,&loclip,&hiclip);
            if( n < 1)
                break;
                if( autoclip == 0)
                    break;
            else
                autoclip = 1;
            break;
        case 3:
            if( index != -1) {
                printf("Type PIV Plot Scale Factor. ");
                getlin(scratch);
            }
            sscanf(scratch,"%f",&pivscale);
            break;
        case 4:
            if( index != -1) {
                printf("Type Plot Direction Preference (+x, -x, +y, -y,None,Auto).  ");
                nch = getlin(scratch);
            }
            for(i=0; i<nch; i++) {
                switch( scratch[i] ) {
                    case '+':
                        i++;
                        if( scratch[i] == 'X' || scratch[i] == 'x' ) 
                            plot_dir_pref = POS_X;
                        else
                            plot_dir_pref = POS_Y;
                        i = nch;	
                        break;
                    case '-':
                        i++;
                        if( scratch[i] == 'X' || scratch[i] == 'x' ) 
                            plot_dir_pref = NEG_X;
                        else
                            plot_dir_pref = NEG_Y;
                        i = nch;	
                        break;
                    case 'n':
                    case 'N':
                        plot_dir_pref = NO_PREF;
                        i = nch;
                        break;
                    case 'a':
                    case 'A':
                        plot_dir_pref = AUTO_DIR;
                        i = nch;
                        break;
                    default:
                        break;
                }
            }			
            break;
        case 5:
            if (labelplot){
                labelplot = 0;
            } else {
                labelplot = 1;
            }
            break;
        case 6:
            if( index != -1) {
                printf("Type Shift x,y for XCorr. ");
                getlin(scratch);
            }
            sscanf(scratch,"%d %d",&shiftx,&shifty);
            break;
        case 7:
            if (c_flag){
                c_flag = 0;
            } else {
                c_flag = 1;
            }
            break;
        default:
            printf("Unknown Option.\n");
    } /* end of switch */

    printf("Current parameters are:\n");
    printf("1. Plot Results Flag: %d\n", plotduringpiv);
    printf("2. Auto Clip Flag: %d\n", autoclip);
    if( autoclip)
        printf("   low_clip: %.2f    high_clip: %.2f\n", loclip,hiclip);
    printf("3. PIV Plot Scale Factor: %.2f\n", pivscale);
    printf("4. PIV Plot Direction Preference: %s.\n", dirprefs[plot_dir_pref].name);
    printf("5. Label Plot Flag: %d\n", labelplot);
    printf("6. Shift in XCorr: %d  %d\n", shiftx,shifty);
    printf("7. Conditional Velocity Flag: %d\n\n", c_flag);
    return 0;
}
/* ***************** */

int pivget(int n,int index)		/* restore the data saved in a file */
{
	extern char cmnd[],*fullname();
	int err;
	
	err = getpivdata(&cmnd[index]);	
	return err;
}


int getpivdata(char* name)		
{

	
    extern char *fullname(),scratch[];
	
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern PEAKVAL_TYPE*  peakval;
	extern int fftsize;
	extern int boxinc;

	extern float* xpeaks;
	extern float* ypeaks;	

	int	i = 0;
	int m,n,k;
	FILE *fp;
	
	//FInfo fileinfo;

	//err = getvol(oldname,&oldvol);
	//err = setvol(fullname(name,GET_DATA),0);
	//err = getvol(oldname,&vol);
/*	
	if ( getfinfo(name,vol,&fileinfo) < 0 ){		// need to be sure this is the right kind of file 
	  	printf("File Not Found.\n");
		beep();
		return(0);
	}

	if(fileinfo.fdType != 'TEXT') {
	  	printf("File Type Must be 'TEXT'.\n");
		beep();
		return(0);
	}
*/

	fullname(name,GET_DATA);

	fp = fopen(name,"r");

	if( fp != NULL) {	
		
		fscanf( fp,"%d\t%d\t%d\t",&k,&m,&n);
		
		/* read the text label to be sure this is a valid piv file */
		// don't try and compare the \n, since that may be different between os9 and osx
		fread(scratch,strlen("x locations, y locations, peaks/location"),1,fp);

		if( memcmp(scratch,"x locations, y locations, peaks/location",
					strlen("x locations, y locations, peaks/location")) != 0) {
			printf("Not a valid PIV file.\n");
			beep();
			fclose(fp);
#ifdef Mac
			//err = setvol("", oldvol);
#endif
			return(-1);
		}
		
		/* OK to make assignments now */
		no_of_velx = k;
		no_of_vely = m;
		peaks_to_save = n;


		fscanf( fp,"%d\t%d",&fftsize,&boxinc);

		fread(scratch,strlen("\tfft size, increment\n"),1,fp);		/* skip past more text */

		fread(scratch,strlen("x coord\ty coord\tvalue\n"),1,fp);		/* skip past more text */
		
		printf("%d %d %d %d %d x y np,fft,inc\n",no_of_velx,no_of_vely,peaks_to_save,fftsize,
												boxinc);
		
		/* allocate memory */
	
		if(xpeaks != 0) free(xpeaks);		/* get rid of old data if there is any */
		if(ypeaks != 0) free(ypeaks);
		if(peakval != 0) free(peakval);


		xpeaks = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
		ypeaks = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
		peakval = (PEAKVAL_TYPE*)malloc(no_of_velx * no_of_vely * peaks_to_save * PEAKVAL_SIZE);
	
		if( xpeaks == 0 || ypeaks == 0 || peakval == 0) {
			nomemory();
			if(xpeaks != 0) free(xpeaks);
			if(ypeaks != 0) free(ypeaks);
			if(peakval != 0) free(peakval);
			return -2;
		}

		
		m = 0;
        for( i=0; i< no_of_velx*no_of_vely; i++) {
			for(n=0; n<peaks_to_save; n++) {
#ifdef FLOAT
				fscanf( fp,"%f\t%f\t%g",&(xpeaks[m]),&(ypeaks[m]),&(peakval[m]));
#else
                fscanf( fp,"%f\t%f\t%d",&(xpeaks[m]),&(ypeaks[m]),&(peakval[m]));
#endif
				m++;
			}
        }
		
		fscanf( fp,"%d\t%d",&sum_area,&exclude_radius);
					
		printf("%d %d sum area, exclude radius\n",sum_area,exclude_radius);

    	fclose(fp);
#ifdef Mac
		//err = setvol("", oldvol);
#endif
		return(1);
		
	}
	else {
		beep();
		printf("File %s Not Found.\n",name);
		return -3;
	}

}



int getpivfromfinder(char* name)		
{
	extern char    scratch[];
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern PEAKVAL_TYPE*  peakval;
	extern int fftsize;
	extern int boxinc;
	
	extern float* xpeaks;
	extern float* ypeaks;	
	
	int	i = 0;
	int m,n,k;
	FILE *fp;
	
	// Open the file pointed to from the Finder. Patha and extension are already included
	fp = fopen(name,"r");
	if( fp != NULL) {	
		// Read in the size and peaks
		fscanf( fp,"%d\t%d\t%d\t",&k,&m,&n);
		
		// compare label to ensure a VALID PIV file
		fread(scratch,strlen("x locations, y locations, peaks/location"),1,fp);
		if( memcmp(scratch,"x locations, y locations, peaks/location",
				   strlen("x locations, y locations, peaks/location")) != 0) {
			beep();
			printf("Not a valid PIV file.\n");
			fclose(fp);
			return(-1);
		}
		
		// Make assignments to our extern variables now that all is OK
		no_of_velx = k;
		no_of_vely = m;
		peaks_to_save = n;
		
		// Read the next line (FFTSIZ, BOXINC)
		fscanf( fp,"%d\t%d",&fftsize,&boxinc);
		
		// Skip next label
		fread(scratch,strlen("\tfft size, increment\n"),1,fp);
		fread(scratch,strlen("x coord\ty coord\tvalue\n"),1,fp);	

		// Echo the file size and stats to OMA window
		printf("%d %d %d %d %d x y np,fft,inc\n",no_of_velx,no_of_vely,peaks_to_save,fftsize, boxinc);
												  
		// Free old memory
		if(xpeaks != 0) free(xpeaks);
		if(ypeaks != 0) free(ypeaks);
		if(peakval != 0) free(peakval);
		
		// Allocate memory
		xpeaks = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
		ypeaks = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
		peakval = (PEAKVAL_TYPE*)malloc(no_of_velx * no_of_vely * peaks_to_save * PEAKVAL_SIZE);
		if( xpeaks == 0 || ypeaks == 0 || peakval == 0) {
			nomemory();
			if(xpeaks != 0) free(xpeaks);
			if(ypeaks != 0) free(ypeaks);
			if(peakval != 0) free(peakval);
			return -2;
		}
		
		// Set up loop and cumulative peak counter
		m = 0;
		for( i=0; i< no_of_velx*no_of_vely; i++) {
			for(n=0; n<peaks_to_save; n++) {
#ifdef FLOAT
				fscanf( fp,"%f\t%f\t%g",&(xpeaks[m]),&(ypeaks[m]),&(peakval[m]));
#else
				fscanf( fp,"%f\t%f\t%d",&(xpeaks[m]),&(ypeaks[m]),&(peakval[m]));
#endif
				m++;
			}
		}
		
		// Read in the concluding label in text file
		fscanf( fp,"%d\t%d",&sum_area,&exclude_radius);
		printf("%d %d sum area, exclude radius\n",sum_area,exclude_radius);
		
		// Close and exit
		fclose(fp);
		return(1);
	} else {
		beep();
		printf("File %s Not Found.\n",name);
		return -3;
	}
}

/* ***************** */
int pivlst()
{
	int ulx,uly,k,nc,nt;
	int num = 0;


	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	
	nc = (no_of_velx-1)*boxinc+fftsize;
	nt = (no_of_vely-1)*boxinc+fftsize;

	

	/* get the next subarray of the current image */
		
	for(uly = 0; uly <= nt - fftsize; uly += boxinc){
		for(ulx = 0; ulx <= nc - fftsize; ulx += boxinc){
			for(k=0; k<peaks_to_save; k++){
				printf("%.2f\t%.2f",xpeaks[num],ypeaks[num]);
				printf("\t%d\t",peakval[num]);
				num++;
			}
			printf("\n");
		}
	}
	
	setarrow();	
	return 0;
}


/* ***************** */  /* The graphics independent PIV plotting stuff */

int pivplt(int n,int index)
{
	extern char scratch[];
    extern char cmnd[];
	
	float x0=0,y0=0,x1=0,y1=0,scale=1;

	int ulx,uly,nc,nt;
	int num;

	num = 1;				
	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	
	if( sscanf(&cmnd[index],"%f %s",&scale,scratch) < 1)
		scale = n;
		
	if(n == 0)
		scale = pivscale;

	if(scale == 0.0) scale = 1.0;
	pivscale = scale;
	
	scale -= 1.0;	/* the scale factor input is a multiplicitive factor */
	
	

	nc = (no_of_velx-1)*boxinc+fftsize;
	nt = (no_of_vely-1)*boxinc+fftsize;
	
	x1 = nc;
	y1 = nt;
	
	if( !pivplot(INIT,1.0,0.0,x1,y1,0) ) {
		beep();
		return -2;
	}

	

	/* get the next subarray of the current image */
		
	for(uly = 0; uly <= nt - fftsize; uly += boxinc){
		for(ulx = 0; ulx <= nc - fftsize; ulx += boxinc){
			
			x0 = ulx + fftsize/2;		/* the center of this box (used in plotting) */
			y0 = uly + fftsize/2;
				
			/* plot velocities */
			
			x1 = scale*(xpeaks[num] - fftsize/2);
			y1 = scale*(ypeaks[num] - fftsize/2);
			
			x1 = ulx + xpeaks[num] + x1;
			y1 = uly + ypeaks[num] + y1;
			
			num += peaks_to_save;
			
			get_vect_dir(x0,y0,&x1,&y1);
			
			pivplot(PLOT,x0,y0,x1,y1,0);
			
		}
	}
	pivplot(CLOSE,x0,y0,x1,y1,0); 
	setarrow();
	return 0;
		
}


/* ***************** */

int pivsht(int n,int index)				/* remove short vectors */
{

    extern char cmnd[];
	
	float vx(),vy(),vel();
	float exrad;
	int ptx,pty,i,nshort,nfixed;

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}

	if( sscanf(&cmnd[index],"%f",&exrad) < 1)
		exrad = exclude_radius;
		
	if(n == 0)
		exrad = exclude_radius;

		
	

	nshort = nfixed = 0;
	
	/* go through all the velocity vectors */
		
	for(pty = 0; pty < no_of_vely; pty++ ){
		for(ptx = 0; ptx < no_of_velx; ptx++ ){
			
			if( vel(ptx,pty,1) <= exrad) {
				nshort++;
				/* this very likely a bad vector since it is less than exclude radius */
				for( i = 0; i < peaks_to_save - 2; i++ ) {
					piv_reorder( ptx + pty*no_of_velx + 1, -1 );		/* switch but don't print */
					if( vel(ptx,pty,1) > exrad) {
						nfixed++;
						break;
					}
				}
			
			}			
		}
	}
	printf("%d Vectors shorter than %.2f found. %d were fixed.\n",nshort,exrad,nfixed);
	setarrow();	
	return 0;
}

float vx(int ix,int iy,int peaknum)
{
	int num;
	
	num = (ix + no_of_velx*iy)*peaks_to_save;
	
	return( xpeaks[num+peaknum] - xpeaks[num] );
}

float vy(int ix,int iy,int peaknum)
{
	int num;
	
	num = (ix + no_of_velx*iy)*peaks_to_save;
	
	return( ypeaks[num+peaknum] - ypeaks[num] );
}

float vel(int ix,int iy,int peaknum)
{
	float vx(),vy();
	
	return( sqrt( vx(ix,iy,peaknum)*vx(ix,iy,peaknum) + vy(ix,iy,peaknum)*vy(ix,iy,peaknum)) );
}

float angle( int ix, int iy, int peaknum)
{

	float x,y;
	
	x = vx(ix,iy,peaknum);
	y = vy(ix,iy,peaknum);
	if( x == 0.0){
		if(y > 0.0) return (90.0);
		if(y < 0.0) return (-90.0);
		return (0.0);
	}
	return(180.0 * atan(y/x) / PI);
	
}


/* ***************** */

int pivbad(int n,int index)				/* try to remove bad vectors -- those with deviation > given value*/
{
    extern char cmnd[];
	
	float vx(),vy(),vel(),ang_ave,vel_ave,ang,vx_ave,vy_ave;
	float dmag,dang,da,dv,da1,dv1,deviation,dev1,neighbors;
	int ptx,pty,i,j,criteria_met,replace;
	int numfixed = 0;
	int numbad = 0;
	int numbetter = 0;
	int best_peak,worst_peak;

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	
	replace = 0;	/* flag that determines if a "bad" vector should be replaced with local
						average if no "good" vector can be found */

	if( sscanf(&cmnd[index],"%f %f %d",&dmag,&dang, &replace) < 2) {
		dmag = 1.0;
		dang = 0.5;
	}
		
	if(n == 0) {
		dmag = 1.0;
		dang = 0.5;
	}
		
	

	/* go through all the velocity vectors */
		
	for(pty = 0; pty < no_of_vely; pty++ ){
		for(ptx = 0; ptx < no_of_velx; ptx++ ){
			
			/* get the average velocity in this region excluding this center point */
			
			ang_ave = vel_ave = vx_ave = vy_ave = neighbors = 0.0;
			for( i= -1; i<=1; i++) {
				for( j= -1; j<=1; j++) {
					if ( i == 0 && j == 0 ) break;					/* don't count this point */
					if ( ptx+j < 0 || ptx+j >= no_of_velx ) break;	/* don't run off the page */
					if ( pty+i < 0 || pty+i >= no_of_vely ) break;
					neighbors += 1.0;
					ang = atan(vy(ptx+j,pty+i,1)/vx(ptx+j,pty+i,1));
					if( ang < 0.0 ) 
						ang += PI;
					ang_ave += ang;
					vel_ave += vel(ptx+j,pty+i,1);
					vx_ave += vx(ptx+j,pty+i,1);
					vy_ave += vy(ptx+j,pty+i,1);
				}
			}
			ang_ave /= neighbors;
			vel_ave /= neighbors;
			vx_ave /= neighbors;
			vy_ave /= neighbors;

			ang = atan( vy(ptx,pty,1)/vx(ptx,pty,1));
			if( ang < 0.0 ) 
				ang += PI;
			
						
			da = fabs( ang - ang_ave );
			dv = fabs((vel(ptx,pty,1)-vel_ave) )/ vel_ave;
			
			deviation = da + dv;	/* this expresses how far off things are in a single number */
			
			/* printf("%f %f ang, ang_ave\n",ang, ang_ave); */
			
			if( da > dang || dv > dmag ) {
				/* deviates too much, try for something better */
				
				numbad++;
				
				criteria_met = 0;
				best_peak = worst_peak = 1;
				da1 = da;
				dv1 = dv;
				
				for( i = 2; i < peaks_to_save; i++ ) {
					/* piv_reorder( ptx + pty*no_of_velx + 1 ); */

					ang = atan( vy(ptx,pty,i)/vx(ptx,pty,i));
					if( ang < 0.0 ) 
						ang += PI;
					
					da1 = fabs( ang - ang_ave);
					dv1 = fabs((vel(ptx,pty,i)-vel_ave) )/ vel_ave;

					if( da1 < dang && dv1 < dmag ) {
						/* have found one that fits the criterion, do the switch */
						j = (ptx + pty*no_of_velx)*peaks_to_save;
						swap_peaks(j+1,j+i);
						criteria_met = 1;
						numfixed++;
						break;
					}
					dev1 = da1+dv1;
					if( dev1 > deviation )
						worst_peak = i;
					if( dev1 < deviation )
						best_peak = i;

				}
				if ( !criteria_met ) {
					j = (ptx + pty*no_of_velx)*peaks_to_save;
					if( replace ) {
						
						swap_peaks(j+1,j+worst_peak);	/* get rid of the worst peak */
						
						xpeaks[j+1] = vx_ave + xpeaks[j];
						ypeaks[j+1] = vy_ave + ypeaks[j];
						peakval[j+1] = 0;		/* label this as a replaced one */
					
					} else {
						if(best_peak != 1) {
							numbetter++;
							swap_peaks(j+1,j+best_peak);
						}
					}
				}
			}			
		}
	}
	pprintf("%d bad vectors.\n%d good vectors found.\n",numbad,numfixed);
	if( !replace)
		pprintf("%d improved.\n",numbetter);
	setarrow();	
	return 0;
}

int swap_peaks(int i,int j)
{
	float loc;
	int pv;

	loc = xpeaks[i];
	xpeaks[i] = xpeaks[j];
	xpeaks[j] = loc;
	loc = ypeaks[i];
	ypeaks[i] = ypeaks[j];
	ypeaks[j] = loc;
	pv = peakval[i];
	peakval[i] = peakval[j];
	peakval[j] = pv;
	return 0;
}
/* ***************** */


int vorticity(int n)				/* calculate the vorticity */
{

	float	*vort_array,vort,vortmax=0,vortmin=0,vx(),vy();
	int i,j,k;

	extern int	doffset;
	extern DATAWORD *datpt;		/* the data pointer */
	extern TWOBYTE header[];
	
	

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	
	vort_array = (float *)malloc((no_of_vely-2)*(no_of_velx-2)*4);
	if( vort_array == 0) {
		nomemory();
		return -2;
	}

	
	
	if( n == 0 ) {		/* this tells how big the integers will be */
		n = 5000;
	}
	
	/* go through all the internal velocity vectors (i.e., exclude edges) */
	
	k = 0;	
	for(j = 1; j < no_of_vely-1; j++ ){
		for(i = 1; i < no_of_velx-1; i++ ){
			/* calculation method from Reuss et al. -- circulation around
			   a closed contour surrounding the point */
			 vort = vx(i,j-1,1) + 0.5 * ( vx(i+1,j-1,1) + vy(i+1,j-1,1) )
				  + vy(i+1,j,1) - 0.5 * ( vx(i+1,j+1,1) - vy(i+1,j+1,1) )
				  - vx(i,j+1,1) - 0.5 * ( vx(i-1,j+1,1) + vy(i-1,j+1,1) )
				  - vy(i-1,j,1) - 0.5 * ( - vx(i-1,j-1,1) + vy(i-1,j-1,1) ) ;
			if( i == 1 && j == 1)
				vortmax = vortmin = vort;
			*(vort_array+k++) = vort;
			if( vort > vortmax) vortmax = vort;
			if( vort < vortmin) vortmin = vort;
		}
	}
	printf("Vorticity Max: %f\n",vortmax);
	printf("Vorticity Min: %f\n",vortmin);
	if( fabs(vortmin) > vortmax ) vortmax = fabs(vortmin);
	printf("Normalization: %d/%f \n",n,vortmax);

	header[NCHAN] = no_of_velx-2;
	header[NTRAK] = no_of_vely-2;
	checkpar();
	for(i=0; i< (no_of_vely-2)*(no_of_velx-2); i++) {
		vort  = *(vort_array+i) * n / vortmax;
		*(datpt+i+doffset) = vort;
	}
	
	free(vort_array);

	have_max = 0;
	maxx();
	setarrow();
	return 0;

		
}
/* ***************** */

int vorticity2(int n)				/* calculate the vorticity another way*/
{

	float	*vort_array,vort,vortmax=0,vortmin=0,vx(),vy();
	int i,j,k;

	extern int	doffset;
	extern DATAWORD *datpt;		/* the data pointer */
	extern TWOBYTE header[];
	
	

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	
	vort_array = (float *)malloc((no_of_vely-2)*(no_of_velx-2)*4);
	if( vort_array == 0) {
		nomemory();
		return -2;
	}

	
	
	if( n == 0 ) {		/* this tells how big the integers will be */
		n = 5000;
	}
	
	/* go through all the internal velocity vectors (i.e., exclude edges) */
	
	k = 0;	
	for(j = 1; j < no_of_vely-1; j++ ){
		for(i = 1; i < no_of_velx-1; i++ ){
			/* calculation method from Reuss et al. -- circulation around
			   a closed contour surrounding the point */
			 vort = ( vy(i+1,j,1) - vy(i,j,1) ) - ( vx(i,j+1,1) - vx(i,j,1) );
			if( i == 1 && j == 1)
				vortmax = vortmin = vort;
			*(vort_array+k++) = vort;
			if( vort > vortmax) vortmax = vort;
			if( vort < vortmin) vortmin = vort;
		}
	}
	printf("Vorticity Max: %f\n",vortmax);
	printf("Vorticity Min: %f\n",vortmin);
	if( fabs(vortmin) > vortmax ) vortmax = fabs(vortmin);
	printf("Normalization: %d/%f \n",n,vortmax);

	header[NCHAN] = no_of_velx-2;
	header[NTRAK] = no_of_vely-2;
	checkpar();
	for(i=0; i< (no_of_vely-2)*(no_of_velx-2); i++) {
		vort  = *(vort_array+i) * n / vortmax;
		*(datpt+i+doffset) = vort;
	}
	
	free(vort_array);

	have_max = 0;
	maxx();
	setarrow();
	return 0;

		
}

/* ***************** */

int pivcrs(int n,int index)		// cross correlation piv
								// the second image is stored in the background image (use the "SBACK filename" command)
{

    extern DATAWORD *datpt;
    extern int	doffset;

    extern int stopstatus;
    extern int histo_array[];
    extern char cmnd[];
    extern TWOBYTE header[];

    float 	*ap1,*bp1,*ap2,*bp2,*work1p,*work2p;
    float max,min,v,x0=0,y0=0,x1,y1,scale=1,dummy;
    

    DATAWORD loval1, hival1,loval2,hival2;


    int fftarraysize;
    TWOBYTE nc_nt;

    int i,j=0,k,n2,rad,histmax,binsize,ni,numctr;
    int ulx,uly;
    int mystopstatus;
    int num = 0;

    DATAWORD *imagedat1,*imagedat2,*fftdat1,*fftdat2,datval,dvmin1,dvmax1,dvmin2,dvmax2;

    /* be sure the second image is already lodad and the right size */

    if(backdat == 0) {
        beep();
        printf("Must load second image using SBACK first.\n");
        return -1;
    }
    if( (header[NCHAN]*header[NTRAK]+MAXDOFFSET)*DATABYTES != backsize) {
        beep();
        printf("Image Sizes Conflict.\n");
        return -2;
    }

    peaks_to_save = 6;


    /* Check to see if there was a second argument */

    rad = n;

    for ( i = index; cmnd[i] != EOL; i++) {
        if(cmnd[i] == ' ') {
            sscanf(&cmnd[index],"%d %d",&n,&rad);
            break;
        }
    }

    sum_area = n;
    exclude_radius = rad;


    n2 = fftsize;			/* the number of rows in the fft sub box */
    nc_nt = fftsize;


    fftarraysize = fftsize * fftsize;

    if( plotduringpiv) {
        /* setup to plot results as we go */
        if( !pivplot(INIT,0.0,0.0,0.0,0.0,0) ) {
            beep();
            return -1;
        }
        scale = pivscale - 1.0;
    }


    /* calculate how many vectors will be found */

    no_of_velx = (header[NCHAN]-fftsize)/boxinc +1;
    no_of_vely = (header[NTRAK]-fftsize)/boxinc +1;

    printf("%d by %d\n",no_of_velx,no_of_vely);


    /* allocate memory */


    if(xpeaks != 0) free(xpeaks);		/* get rid of old data if there is any */
    if(ypeaks != 0) free(ypeaks);
    if(peakval != 0) free(peakval);
    if(dirval != 0) free(dirval);


    ap1 = (float *)malloc(fftarraysize*4);
    bp1 = (float *)malloc(fftarraysize*4);
    ap2 = (float *)malloc(fftarraysize*4);
    bp2 = (float *)malloc(fftarraysize*4);

    work1p = (float *)malloc(n2*4);
    work2p = (float *)malloc(n2*4);
    fftdat1 = (DATAWORD*)malloc(fftarraysize*DATABYTES);
    fftdat2 = (DATAWORD*)malloc(fftarraysize*DATABYTES);
    xpeaks = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
    ypeaks = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
    peakval = (PEAKVAL_TYPE*)malloc(no_of_velx * no_of_vely * peaks_to_save * PEAKVAL_SIZE);
    dirval = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);

    if( ap1==0 || bp1==0 || ap2==0 || bp2==0 || work1p==0 || work2p == 0 || fftdat1 == 0 || fftdat2 == 0
		      || xpeaks == 0 || ypeaks == 0 || peakval == 0 || dirval == 0 ) {
        nomemory();
        if(ap1 != 0) free(ap1);
        if(bp1 != 0) free(bp1);
        if(ap2 != 0) free(ap1);
        if(bp2 != 0) free(bp1);
        if(work1p != 0) free(work1p);
        if(work2p != 0) free(work2p);
        if(fftdat1 != 0) free(fftdat1);
        if(fftdat2 != 0) free(fftdat2);
        if(xpeaks != 0) free(xpeaks);
        if(ypeaks != 0) free(ypeaks);
        if(peakval != 0) free(peakval);
        if(dirval != 0) free(dirval);
        if( plotduringpiv) pivplot(CLOSE,0.0,0.0,0.0,0.0,0);
        return -2;
    }


    
    mystopstatus = stopstatus;
    //	cross_correlation = 1;

    /* get the next subarray of the current image */

    for(uly = 0; uly <= header[NTRAK] - fftsize; uly += boxinc){
        for(ulx = 0; ulx <= header[NCHAN] - fftsize; ulx += boxinc){

            /* print the location of this box */

            printxyzstuff(ulx,uly,0);
            printxyzstuff(ulx+fftsize-1,uly+fftsize-1,1);

            x0 = ulx + fftsize/2;		/* the center of this box (used in plotting) */
            y0 = uly + fftsize/2;


            /* copy the subarray data into an array, get the min and max */

            imagedat1 = datpt + doffset + ulx + uly*header[NCHAN] -1 ;	/* first data pt */
            imagedat2 = backdat + doffset + ulx + uly*header[NCHAN] -1 ;	/* first data pt */
            k = 0;
            dvmax1 = dvmin1 = *(imagedat1+1);
            dvmax2 = dvmin2 = *(imagedat2+1);
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    datval = *(imagedat1+j);
                    if( datval > dvmax1) dvmax1 = datval;
                    if( datval < dvmin1) dvmin1 = datval;
                    fftdat1[k] = datval;
                    datval = *(imagedat2+j);
                    if( datval > dvmax2) dvmax2 = datval;
                    if( datval < dvmin2) dvmin2 = datval;
                    fftdat2[k] = datval;
                    k++;
                }
                imagedat1 += header[NCHAN];
                imagedat2 += header[NCHAN];
            }

            if( autoclip ) {

                /* get the histogram of the current subarray */

                binsize = data_histogram(fftdat1,dvmin1,dvmax1,fftarraysize);

                /* find the maximum of the histogram, exclude the extreme points */

                histmax = 0;
                for(i=1; i< HISTOSIZE-1; i++) {
                    if( histo_array[i] > histmax ){
                        histmax = histo_array[i];
                        j = i;
                    }
                }
                loval1 = loclip * histmax;
                hival1 = hiclip * histmax;
                for(i=0; i< HISTOSIZE-1; i++) {
                    if(histo_array[i] > loval1)
                        break;
                }
                printf("%d  %d  %d val, histmax,\n",
                       j*binsize + dvmin1,histmax,i);
                loval1 = i*binsize + dvmin1;
                for(i=HISTOSIZE-2; i > 0; i--) {
                    if(histo_array[i] > hival1)
                        break;
                }
                hival1 = i*binsize + dvmin1;

                printf("Peak at: %d Floor: %d Ceiling: %d\n",
                       j*binsize + dvmin1,loval1,hival1);

                setbounds(fftdat1,loval1,hival1,fftarraysize);

                // clip the second image in the same way

                /* get the histogram of the current subarray */

                binsize = data_histogram(fftdat2,dvmin2,dvmax2,fftarraysize);

                /* find the maximum of the histogram, exclude the extreme points */

                histmax = 0;
                for(i=1; i< HISTOSIZE-1; i++) {
                    if( histo_array[i] > histmax ){
                        histmax = histo_array[i];
                        j = i;
                    }
                }
                loval2 = loclip * histmax;
                hival2 = hiclip * histmax;
                for(i=0; i< HISTOSIZE-1; i++) {
                    if(histo_array[i] > loval2)
                        break;
                }
                printf("%d  %d  %d val, histmax,\n",
                       j*binsize + dvmin2,histmax,i);
                loval2 = i*binsize + dvmin2;
                for(i=HISTOSIZE-2; i > 0; i--) {
                    if(histo_array[i] > hival2)
                        break;
                }
                hival2 = i*binsize + dvmin2;

                printf("Peak at: %d Floor: %d Ceiling: %d\n",
                       j*binsize + dvmin2,loval2,hival2);

                setbounds(fftdat2,loval2,hival2,fftarraysize);

            }

            /* load up the real arrays with the subarray data from both images */

            k = 0;
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    ap1[k] = fftdat1[k];
                    bp1[k] = 0.0;
                    ap2[k] = fftdat2[k];
                    bp2[k] = 0.0;
                    k++;
                }
            }

            

            /* do the initial fft */

            FT2D(ap1,bp1,work1p,work2p,&n2,&n2);


            

            /* now do the second fft */

            FT2D(ap2,bp2,work1p,work2p,&n2,&n2);

            

            /***** multiply fft1 and fft2 *****/

            k = 0;
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    if( 1 & (i+j)){
                        dummy = -ap1[k]*ap2[k] - bp1[k]*bp2[k];  /* introduces a shift of fftsize/2,fftsize/2 */
                        bp1[k] = -ap1[k]*bp2[k] + ap2[k]*bp1[k]; /*	 to the result of the inverse fft */
                        ap1[k] = dummy;
                    }else{
                        dummy = ap1[k]*ap2[k] + bp1[k]*bp2[k];
                        bp1[k] = ap1[k]*bp2[k] - ap2[k]*bp1[k];
                        ap1[k] = dummy;
                    }
                    k++;
                }
            }

            /***** do inverse fft *****/

            ni = -n2;
            FT2D(ap1,bp1,work1p,work2p,&ni,&ni);
            


            /***** return magnitude ****/

            max = ap1[0];
            min = ap1[0];
            for(i=0; i<fftarraysize; i++) {
                ap1[i] = sqrt(ap1[i]*ap1[i] + bp1[i]*bp1[i]);
                if( ap1[i] > max )
                    max = ap1[i];
                if( ap1[i] < min )
                    min = ap1[i];

            }

            //printf(" Max is %f.\n",max);

			// some conditional code here for FLOAT
#ifdef FLOAT
            for(i=0; i<fftarraysize; i++) {
                fftdat1[i] = ap1[i];
            }

#else
            for(i=0; i<fftarraysize; i++) {
                fftdat1[i] = (ap1[i]-min)*MAXDATAVALUE/max;
            }
#endif
            /* find peaks -- */

            peaks(nc_nt,nc_nt,n,rad,fftdat1);	/* search the image */

            /* 	For cross-correlation (this case), the main peak has the velocity.
                For auto-correlation, the largest peak is always at 0 displacement and
                therefore not of interest.
                Because the files should be the same for velocities calculated with
                either approach, the main peak is stored in
                element 1 of the peak array. Element 0 has the center of the fft box.
                */

            for(k=peaks_to_save-1; k>0; k--){
                xloc[k] = xloc[k-1];
                yloc[k] = yloc[k-1];
                biggies[k] = biggies[k-1];
            }
            xloc[0] = yloc[0] = fftsize/2;
            for(k=0; k<peaks_to_save; k++){
                /*
                // calculate x & y center of mass -- use this for the actual peak location 
                x1 = y1 = sum = 0.0;
                if(k != 0){
                    for(i=yloc[k]-n; i<=yloc[k]+n; i++) {
                        for(j=xloc[k]-n; j<=xloc[k]+n; j++) {
                            datval = *(fftdat1 + (i)*fftsize + j);
                            x1 += j * datval;			// x center of mass
                            y1 += i * datval;			// y center of mass
                            sum += datval;
                        }
                    }
                    x1 = x1/sum;
                    y1 = y1/sum;
                } else  {
                    x1 = xloc[k];
                    y1 = yloc[k];
               }
               */
                //pprintf("%.2f\t%.2f",x1,y1);
                //pprintf("\t%d\t",biggies[k]/n/n);
                
                
    
                if( k == 1) {		// refine the peak for the main velocity only 
                    peak_refine(&x1,&y1,xloc[k],yloc[k],5,fftdat1,fftsize);
                } else {
                    x1 = xloc[k];
                    y1 = yloc[k];
                }
                
                
                xpeaks[num] = x1;
                ypeaks[num] = y1;
                peakval[num] = biggies[k];
               
                /*
                 pprintf("%.2f\t%.2f",xpeaks[num],ypeaks[num]);
                 pprintf("\t%d\t",biggies[k]/n/n);
                 */

                dirval[num] = 0.0;
                numctr = num;

                num++;
            }

            /* find velocities */

            x1 = ulx + xpeaks[num-peaks_to_save+1];
            y1 = uly + ypeaks[num-peaks_to_save+1];
            v = (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0);
            v = sqrt(v);

            pprintf("v =\t%.2f\tx=\t%.2f\ty=\t%.2f\n",
                    v,xpeaks[num-peaks_to_save+1],ypeaks[num-peaks_to_save+1]);

            if( plotduringpiv) {

                x1 = scale*(xpeaks[num-peaks_to_save+1] - fftsize/2);
                y1 = scale*(ypeaks[num-peaks_to_save+1] - fftsize/2);

                x1 = ulx + xpeaks[num-peaks_to_save+1] + x1;
                y1 = uly + ypeaks[num-peaks_to_save+1] + y1;

                pivplot(PLOT,x0,y0,x1,y1,1);
            }

            checkstoprequest();

            if( mystopstatus != stopstatus) {
                pprintf("piv command stopped.\n");
                if(uly == 0) {					/* still in the first row */
                    no_of_vely = 1;
                    no_of_velx = (ulx)/boxinc + 1;
                }
                else {
                    no_of_vely = (uly)/boxinc;
                }

                pprintf("New velocity array size is %d by %d.\n",no_of_velx,no_of_vely);
                goto loopexit;
            }
        }
    }
loopexit:
free(ap1);
free(bp1);
free(ap2);
free(bp2);
free(work1p);
free(work2p);
free(fftdat1);
free(fftdat2);
if( plotduringpiv) pivplot(CLOSE,x0,y0,x1,y1,0);
setarrow();
return 0;
}

/* ***************** */

int peak_refine(ax,ay,ix,iy,num,datp,ny)
float *ax,*ay;		// pointers to where the refined location is stored
int ix,iy,num;		// pixel location and number of pixels on either side to sum over
DATAWORD *datp;		// the pointer to the data
int ny;				// y size of image in pixels

{
    float x,y,g,xm=0,ym=0;
    float dl = .01;	// step size in the search for the max;
    float max=-1e10;
    float whittaker();
    int i;

    y = iy;
    i = 0;
    for( x = ix-.5; x<= ix+.5; x = ix-.5 + dl*i) {
        g = whittaker(x,y,ix,iy,num,datp,ny);
        if ( g > max) {
            xm = x;
            max = g;
        }
        i++;
    }
    x=xm;

    max=-1e10;
    i = 0;
    for( y = iy-.5; y<= iy+.5; y = iy-.5 + dl*i) {
        g = whittaker(x,y,ix,iy,num,datp,ny);
        if ( g > max) {
            ym = y;
            max = g;
        }
        i++;
    }
    max=-1e10;
    y = ym;
    /*
        // this extra iteration doesn't seem to change things
     i = 0;
     for( x = ix-.5; x<= ix+.5; x = ix-.5 + dl*i) {
         g = whittaker(x,y,ix,iy,num,datp,ny);
         if ( g > max) {
             xm = x;
             max = g;
         }
         i++;
     }
     printf("%f \n",xm);
     */
    *ax = xm;
    *ay = ym;
    return 0;
}


float whittaker(x,y,ix,iy,num,datp,ny)
float x,y;			// point that function is evaluated at
int ix,iy,num;		// pixel location and number of pixels on either side to sum over
DATAWORD *datp;		// the pointer to the data
int ny;				// y size of image in pixels
{
    int i,j,ii,jj;
    float g,c,sinc();
    DATAWORD datval;

    g = 0.0;
	
    for(i= iy-num/2; i <= iy+num/2 ; i++) {
		if(i >= ny) ii = ny-1;
		else ii = i;
        for(j= ix-num/2; j<= ix+num/2; j++) {
			// this check and the one above are to be sure that we don't run off the end of the (assumed square) fft region
			if(j >= ny) jj = ny-1;
			else jj = j;
            datval = *(datp + (ii)*ny + jj);
            c = datval * sinc((x-jj)*PI) * sinc((y-ii)*PI);
            g += c;
        }
    }
    return g;
}


float sinc(x)
float x;
{
    if (x==0.0)
        return (1.0);
    return (sin(x)/x);
}
/* ************************************ */
int of2piv(int n,int index)		// Command: of2piv %f %d xfile yfile
                 // Convert optical flow images into PIV vectors
                 // Reads in OMA image formats for x and y velocities
                 // User inputs how often to plot a vector (use fft boxinc)
                 // scale factor (%f scale) and average flage (%d 0=off 1=on)

{
    extern char     cmnd[],*fullname();
   	extern TWOBYTE header[];
    extern int	doffset;
    extern char    txt[];
    extern int npts;

    //	extern int	no_of_velx;			/* the number of velocities in the x and y directions */
    //	extern int	no_of_vely;
    //	extern int peaks_to_save;		/* the number of peaks to save */
    //	extern int fftsize;
//	extern int boxinc;

//	extern float* xpeaks;
//	extern float* ypeaks;	
//	extern int*  peakval;


    int i, j=0, k=0, nt, nc, center, usedpts, isize;
    int xsum, ysum, count;
    int num;
    int avgflag=1;
    float scale;
    int xsize, ysize;
    extern unsigned int fd;
    extern DATAWORD *datpt;

    float *xavg, *yavg;
    DATAWORD *xvel=0,*yvel=0, *ptr2;

    char xfil[128],yfil[128];

    for ( i = index; cmnd[i] != EOL; i++) {
        if(cmnd[i] == ' ') {
            sscanf(&cmnd[index],"%f %d %s %s",&scale,&avgflag,xfil,yfil);
            break;
        }
    }

    peaks_to_save = 2;
    no_of_velx = (header[NCHAN]-fftsize)/boxinc +1;
    no_of_vely = (header[NTRAK]-fftsize)/boxinc +1;

    printf("%d by %d\n",no_of_velx,no_of_vely);
    printf("Increment: %d\n",boxinc);
    printf("Average (1 is on): %d\n",avgflag);

    if(xpeaks != 0) free(xpeaks);		/* get rid of old data if there is any */
    if(ypeaks != 0) free(ypeaks);
    if(peakval != 0) free(peakval);
    /*	ALLOCATE MEMORY 	*/
    xpeaks = (float*) malloc(no_of_velx * no_of_vely * peaks_to_save*sizeof(float)*8);
    ypeaks = (float*) malloc(no_of_velx * no_of_vely * peaks_to_save*sizeof(float)*8);
    xavg = (float*) malloc(no_of_velx * no_of_vely * peaks_to_save*sizeof(float)*8);
    yavg = (float*) malloc(no_of_velx * no_of_vely * peaks_to_save*sizeof(float)*8);
    peakval = (PEAKVAL_TYPE*) malloc(no_of_velx * no_of_vely * peaks_to_save*PEAKVAL_SIZE*8);

    if( xpeaks == 0 || ypeaks == 0 || peakval == 0 || xavg ==0 || yavg == 0) {
        nomemory();
        if(xpeaks != 0) free(xpeaks);
        if(ypeaks != 0) free(ypeaks);
        if(peakval != 0) free(peakval);
        if(xavg != 0) free(xavg);
        if(yavg != 0) free(yavg);
        return -1;
    }

    /*	GET X and Y VELOCITY DATA	*/

    fullname(xfil,GET_DATA);
    i=0;
    while( (*(xfil+i) != '\0') ) {
		txt[i] = *(xfil+i);
		i++;
	}
    txt[i]='\0';
    printf("X velocity from file: %s\n",txt);
    keylimit(-1);			/* disable printing */
    getfile (0,-1);			/* get the X velocity file */
    keylimit(-2);

    if(fd == -1){
        goto cleanhouse;
    }

    xsize = header[NCHAN];
    ysize = header[NTRAK];
    printf("X,Y: %d %d\n",xsize,ysize);
    npts = isize = header[NCHAN]*header[NTRAK];
    xvel = (DATAWORD*) malloc(isize*DATABYTES);
    if(xvel == 0) {
        nomemory();
        return -1;
    }

    ptr2=datpt+doffset;
    for(i=0; i<isize; i++)
        *(xvel + i) = *(ptr2+i);

    fullname(yfil,GET_DATA);
    i=0;
    while( (*(yfil+i) != '\0') ){
		txt[i] = *(yfil+i);
		i++;
	}
    txt[i]='\0';
    printf("Y velocity from file: %s\n",txt);
    keylimit(-1);			/* disable printing */
    getfile (0,-1);				/* get the Y velocity file */
    keylimit(-2);

    if(fd == -1){
        goto cleanhouse;
    }

    if( (xsize != header[NCHAN]) || (ysize != header[NTRAK]) ) {
        beep();
        printf("Image sizes are different. \n");
        goto cleanhouse;
    }
    yvel = (DATAWORD*) malloc(isize*DATABYTES);
    if(yvel == 0) {
        nomemory();
        return -1;
    }

    ptr2=datpt+doffset;
    for(i=0; i<isize; i++)
        *(yvel + i) = *(ptr2+i);

    /* CALCULATE VELOCITIES - POINT or MULTI-POINT */

    num = 0;
    count = 0;

    
    for(nt = fftsize/2; nt<(ysize-fftsize/2+1); nt+=boxinc) {
        for(nc = fftsize/2; nc<(xsize-fftsize/2+1); nc+=boxinc) {
            center=nt*xsize + nc;
            xsum = 0; ysum = 0; xavg[num] = 0.0; yavg[num] = 0.0; usedpts = 0;
            if(avgflag == 1) {
                for(i=(-fftsize/2); i<fftsize/2; i++) { /* data in box */
                    for(j=(-fftsize/2); j<fftsize/2; j++) {
                        if( (nt+j)< ysize && (nc+i)< xsize){
                            if((nt+j) >= 0 && (nc+i) >= 0){
                                k = (nt + j)*xsize + nc + i;
                                xsum+=xvel[k];
                                ysum+=yvel[k];
                                usedpts++;
                            }
                        }
                    }
                }
                xavg[count]=(float)xsum/(float)usedpts;
                yavg[count]=(float)ysum/(float)usedpts;
            }
            else{ xavg[count]=xvel[center]; yavg[count]=yvel[center]; }
            /* ASSIGN VELOCITY DATA TO PIV ARRAYS */
            xpeaks[num] = (float) fftsize/2;
            ypeaks[num] = (float) fftsize/2;
            peakval[num] = 1;
            num++;
            xpeaks[num] = (float) (xavg[count]/scale + (fftsize/2));
            ypeaks[num] = (float) (yavg[count]/scale + (fftsize/2));
            peakval[num] = 1;
            num++;
            count++;

        }
    }
printf("Points: %d %d\n",count,num);

/* ---------------------------------- */

cleanhouse:
if(xvel!=0) {
    //printf("Free xvel\n");
    free(xvel);	
		//xvel = 0;
	}
	if(yvel!=0) {
		//printf("Free yvel\n");
		free(yvel);	
		//yvel = 0;
	}
	if(xavg!=0) {
		//printf("Free xavg\n");
		free(xavg);	
		//xavg = 0;
	}
	if(yavg!=0) {
		//printf("Free yavg\n");
		free(yavg);	
		//yavg = 0;
	}

	have_max = 0;
	maxx();
	setarrow();
	return(0);
}

/**************************************************************************************/
//		P K A L T . c
//		
//		File for custom commands for OMA
//		4th November 1998
/**************************************************************************************/

#define SET_WHITE 3
#define SET_BLACK 4


extern TWOBYTE header[];
extern char cmnd[];
extern DATAWORD *datpt,min,max;	
extern int	npts,doffset;
extern int	no_of_velx,no_of_vely,peaks_to_save,fftsize,boxinc;

extern int histo_array[];
extern int stopstatus;

/**************************************************************************************/
//		F U N C T I O N   P R O T O T Y P E S
/**************************************************************************************/
int		nullfn();
int		makpiv(int n, int index);
int		xcorr(int n, int index);
int		pivmassage();
int		pivmagnitude(int n, int index);
int		pivangle(int n, int index);
int		pivfil();
void		massage_vector(int nx, int ny);
void	 	rms_vector(int nx, int ny);
void		query_vector(int numx, int numy);
void		vector_angle(int numx, int numy);
void		kill_vector(int numx, int numy);
void		rotate_vector(int numx, int numy);
Boolean	CheckMagnitude (float Vel1, float Vel2);
Boolean	CheckAngle(float angle1, float angle2, float tolerance);



/**************************************************************************************/
//		C O M M A N D   A N D   R O U T I N E   L I S T  
/**************************************************************************************/
/*
"MAKPIV",
"NULLFN",
"PIVANG",
"PIVFIL",
"PIVMAG",
"PIVMSG",
"XCORR ",
*/
/* Declare the routine name in the list below. */
/*
int	makpiv();		// generate a random PIV image
int nullfn();		// null function
int pivangle();		// Kill any vector in image that points in wrong direction
int	pivfil();		// Interpolate zero vectors in field from neighbours
int	pivmagnitude();	// Kill any vector in image that differ from neighbourhood average
int	pivmassage();	// Automatically click all good vectors in image PIVALT 3 style
int	xcorr();		// performs cross-correlation on buffer & file images
*/
/* Add the address of the routine to an address array below */

/*****************************************************************************
     M A K P I V  - randomly generate a piv particle image
      *****************************************************************************/
int makpiv(int n, int index)
{
    int			arg_int;		// command line arguments
    int			randomX = 0;	// position of particle image X
    int			randomY = 0;	// position of particle image Y
    int			random_int;		// random number holder
    int			no_of_points;	// number of PIV particle images
    int			i;		// loop counters
    int			j;		// actual point
    int			nn, n0, np;		// top row neighbours
    int			neg, pos;		// left and right neighours
    int			pn, p0, pp;		// bottom row neighbours
    int			narg;		// number of arguments on command line

    // Read command line value
    narg = sscanf(&cmnd[index],"%d",&arg_int);
    if(narg == 0){
        arg_int = -1.0;
    }

    // Set meaningful values into variables
    if (arg_int > 0 && arg_int < 20000) {
        no_of_points = arg_int;
    } else {
        no_of_points = 20000;
    }

    // First initialise background image to zero
    setpositive(0);
    clip(0);

    

    // loop for each particle in image
    for (i = 0; i<= no_of_points; i++){

        // Get random point
        random_int = rand();
        if ( random_int < 0 ) random_int = -random_int;
        randomX = 1 + random_int%header[NCHAN];
        random_int = rand();
        if ( random_int < 0 ) random_int = -random_int;
        randomY = 1 + random_int%header[NTRAK];

        // Adjust random point away from edge
        if (randomX < 5) randomX = 5;
        if (randomY < 5) randomY = 5;
        if (randomX > (header[NCHAN] - 5)) randomX = header[NCHAN] - 5;
        if (randomY > (header[NTRAK] - 5)) randomY = header[NTRAK] - 5;

        // Draw in ficticous point
        j	= randomY * header[NCHAN] + randomX;
        nn	= (randomY - 1) * header[NCHAN] + (randomX - 1);
        n0	= (randomY - 1) * header[NCHAN] + randomX;
        np	= (randomY - 1) * header[NCHAN] + (randomX + 1);
        neg	= randomY * header[NCHAN] + (randomX - 1);
        pos	= randomY * header[NCHAN] + (randomX + 1);
        pn	= (randomY + 1) * header[NCHAN] + (randomX - 1);
        p0	= (randomY + 1) * header[NCHAN] + randomX;
        pp	= (randomY + 1) * header[NCHAN] + (randomX + 1);
        *(datpt+doffset+j) = 2500;
        *(datpt+doffset+nn) = 80;
        *(datpt+doffset+n0) = 250;
        *(datpt+doffset+np) = 80;
        *(datpt+doffset+neg) = 250;
        *(datpt+doffset+pos) = 250;
        *(datpt+doffset+pn) = 80;
        *(datpt+doffset+p0) = 250;
        *(datpt+doffset+pp) = 80;

    }


    setarrow();
    have_max = false;
    maxx();
	return 0;
    
} 

/****************************************************************************
P I V E X E  - Execute throughput PIV processing algorithm  ****************************************************************************/
int piv_execute(int n, int index)
{
    extern char *fullname();
    extern int execut();
    int err;
    err = execut(PreMacro);
    return err;
} /* piv_execute PIVEXE */


/****************************************************************************
     P I V C N V  - Convert Pixels/Delay to m/s using parameters from dialog  ****************************************************************************/
int piv_convert(int n, int index)
{
    
    extern int no_of_velx;	/* the number of velocities calculated in the x and y directions */
    extern int no_of_vely;
    extern int peaks_to_save;	/* the number of peaks to save */
    extern float* xpeaks;
    extern float* ypeaks;
    int i,m,j,num_vel;
    float dx,dy;
    float scal = 0.0;

    // Get command line arguments
    //narg = sscanf(&cmnd[index],"%f %f",&pivpixel,&pivdelay) ;

    if(no_of_velx == 0 && no_of_vel == 0) {
        beep();
        printf("Must have valid PIV image first.\n");
        return -1;
    }
    if(no_of_vel > 0)
        num_vel = no_of_vel;
    else
        num_vel = no_of_velx*no_of_vely;

    if (pivdelay) scal = pivpixel/pivdelay;
    m = 0;
    for( i=0; i< num_vel; i++) {
        j = m;
        m++;
        for(n=1; n<peaks_to_save; n++) {
            dx = xpeaks[m] - xpeaks[j];
            dy = ypeaks[m] - ypeaks[j];
            xpeaks[m] = xpeaks[j] + dx*scal;
            ypeaks[m] = ypeaks[j] + dy*scal;
            m++;
        }
    }
    return 0;
} /* piv_convert PIVCNV */

     /****************************************************************************
     X C O R R - perform X-corr of image in buffer with image file
      based on XPIV in custom.c
 ****************************************************************************/

int xcorr(int n, int index)
     {
    Boolean		c_val;		/* flag for conditional PIV evaluation */
    DATAWORD	*frameA_ptr = 0;	// pointer to frame A
    DATAWORD	*tDWORD_ptr;		// temporary DATAWORD pointer
    DATAWORD	subreg_MIN;			// min...
    DATAWORD	subreg_MAX;			// ... and max values in PIV sub-region
    DATAWORD	*fftdat;			// Array to hold current PIV sub-region
    DATAWORD	tDWORD;				// temporary DATAWORD identifier
    DATAWORD	loval, hival;		// The low and high clip values based on PIVSET
    float		*before_A,*after_A;	// buffers for FFT
    float		*before_B,*after_B;	// buffers for FFT
    float		*work1, *work2;		// working storage buffers
    float		x0 = 0, y0 = 0;		// stores the location of the vector tail
    float		x1 = 0, y1 = 0;		// stores the location of the vector head
    float		velocity;			// Velocity value
    float		temp_float;			// temporary float variable
    float		FFT_max;			// Maximum value in inverse FFT
    float		scale=1;				// PIVPLT scale variable
    float		dval,dval_sum;
    int		FFT_area;	// number of pixels in PIV sub-region
    int		FFT_rows;	// number of rows in PIV sub-region
    int		i, j, k;	// loop counters
    int		area;		// sum_area read from command line
    int		radius;		// exclude_radius read from command line
    int		ulx, uly;	// upper left corner X and Y
    int		binsize;	// histogram bin size of FFT subregion
    int		mystopstatus;	// Interupt state
    int		num = 0;	// counter to keep track of peak number
    float		histmax;	// maximum value in PIV sub-region from histogram
    int		temp_int;	// temporary integer variable
    int		min;
    short		narg;		// number of command line arguments
    TWOBYTE	FFT_width;	// width of PIV sub-region (used for peak scanning)
    TWOBYTE	frameA_x;	// x extent of frame A
    TWOBYTE	frameA_y;	// y extent of frame A
                      // copy current data into the piv1 array
	char *fullname();
    frameA_x	= header[NCHAN];
    frameA_y	= header[NTRAK];
    // Allocate memory for frame A (from image buffer)
    if (frameA_ptr != 0) free(frameA_ptr);
    frameA_ptr = (DATAWORD*) malloc(frameA_x * frameA_y * sizeof(DATAWORD));
    if(frameA_ptr == 0) {
        nomemory();
        return -1;
    }
    // Transfer buffer image into frameA_ptr structure
    for(i=0; i < (frameA_x*frameA_y); i++)
        *(frameA_ptr + i) = *(datpt+i+doffset);
    // Get command line arguments (filename, sum area, exclude radius)
    for(i=0; cmnd[index] != ' '; txt[i++] = cmnd[index++] ) ;
    txt[i] = 0;
    narg = sscanf(&cmnd[index],"%d %d",&area,&radius);
    if( narg < 2 ) {	// use default values
        //beep();
        area	= 3;
        radius	= 5;
        printf("Using default parameters: %d %d\n", area, radius );
    }
    // Load frame B (file specified in command line)
    fullname(txt,GET_DATA);
    getfile (0,-1);
    // Check that both frames are the same size
    if( (frameA_x != header[NCHAN]) || (frameA_y != header[NTRAK]) ) {
        if (frameA_ptr != 0) free(frameA_ptr);
        //omaErrorDialog("Images are different sizes","xcorr(); in pkalt.c");
        return -1;
    }
    // it is now OK to change the external PIV variables to command line values
    sum_area = area;
    exclude_radius = radius;
    // If we want to plot as we go...then open the linegraphics window
    if( plotduringpiv) {
        if( !pivplot(INIT,0.0,0.0,0.0,0.0,0) ) {
            beep();
            return -1;
        }
        scale = pivscale - 1.0;
    }
    // Set PIV sub-region dimension variables
    FFT_rows	= fftsize;
    FFT_width	= fftsize;
    FFT_area	= fftsize * fftsize;

    // Calculate how many vectors will be found
    no_of_velx = (header[NCHAN]-fftsize)/boxinc +1;
    no_of_vely = (header[NTRAK]-fftsize)/boxinc +1;
    no_of_vel = 0;	// We are not doing PIV along a contour
                   // Echo size of vector field before we start
    pprintf("%d by %d\n",no_of_velx,no_of_vely);
    // Allocate memory for working structures
    if(xpeaks != 0) free(xpeaks);
    if(ypeaks != 0) free(ypeaks);
    if(peakval != 0) free(peakval);
    if(dirval != 0) free(dirval);
    before_A	= (float *) malloc( FFT_area * sizeof(float));
    after_A		= (float *) malloc( FFT_area * sizeof(float));
    before_B	= (float *) malloc( FFT_area * sizeof(float));
    after_B		= (float *) malloc( FFT_area * sizeof(float));
    work1	= (float *) malloc( FFT_rows * sizeof(float));
    work2	= (float *) malloc( FFT_rows * sizeof(float));
    xpeaks	= (float *) malloc( no_of_velx * no_of_vely * peaks_to_save * sizeof(float));
    ypeaks	= (float *) malloc( no_of_velx * no_of_vely * peaks_to_save * sizeof(float));
    dirval	= (float *) malloc( no_of_velx * no_of_vely * peaks_to_save * sizeof(float));
    fftdat	= (DATAWORD*) malloc( FFT_area * DATABYTES);
    peakval	= (PEAKVAL_TYPE*) malloc( no_of_velx * no_of_vely * peaks_to_save * PEAKVAL_SIZE);
    if( before_A==0 || after_A==0 || before_B==0 || after_B==0 ||work1==0 || work2 == 0 || fftdat == 0
        || xpeaks == 0 || ypeaks == 0 || peakval == 0 || dirval == 0 ) {
        nomemory();
        if (before_A != 0)	free(before_A);
        if (after_A != 0)	free(after_A);
        if (before_B != 0)	free(before_B);
        if (after_B != 0)	free(after_B);
        if (work1 != 0)		free(work1);
        if (work2 != 0)		free(work2);
        if (fftdat != 0)	free(fftdat);
        if (xpeaks != 0)	free(xpeaks);
        if (ypeaks != 0)	free(ypeaks);
        if (peakval != 0)	free(peakval);
        if (dirval != 0)	free(dirval);
        if (plotduringpiv)	pivplot(CLOSE,x0,y0,x1,y1,0);
        return 0;
    }
    // Things are ready to go...
    
    mystopstatus = stopstatus;
    // Major loop for each frame interrogation sub-region
    for(uly = 0; uly <= header[NTRAK] - fftsize; uly += boxinc){
        for(ulx = 0; ulx <= header[NCHAN] - fftsize; ulx += boxinc){
            // set the first peak to center of cross-correlation
            xpeaks[num] = fftsize/2;
            ypeaks[num] = fftsize/2;
            peakval[num] = MAXDATAVALUE;
            num++;
            // Print the location of this box in status window
            printxyzstuff(ulx,uly,0);
            printxyzstuff(ulx+fftsize-1,uly+fftsize-1,1);
            // set the vector tail to the middle of the current PIV sub-region
            x0 = ulx + fftsize/2;
            y0 = uly + fftsize/2;
            // if rectangle in image2 is out of bounds, set velocity = 0
            if (ulx+shiftx < 0 || uly+shifty < 0 || ulx+shiftx+fftsize > header[NCHAN]
                || uly+shifty+fftsize > header[NTRAK]){
                for(k=0; k<peaks_to_save-1; k++){
                    xpeaks[num] = fftsize/2;
                    ypeaks[num] = fftsize/2;
                    peakval[num] = 1000;
                    num++;
                }
                goto skip;
            }
            // check background buffer to see if velocity should be evaluated
            if (c_flag){
                c_val = condtnl(ulx,uly,fftsize);
                if (!c_val){ // if velocity is not to be found then set it to zero
                    for(k=0; k<peaks_to_save-1; k++){
                        xpeaks[num] = fftsize/2;
                        ypeaks[num] = fftsize/2;
                        peakval[num] = 1000;
                        num++;
                    }
                    goto skip;
                }
            }
            // copy frame A sub-region into an array, get the min and max, and clip
            k = 0;
            tDWORD_ptr = frameA_ptr + ulx + uly*header[NCHAN] -1 ;
            subreg_MAX = subreg_MIN = *(tDWORD_ptr+1);
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    tDWORD = *(tDWORD_ptr+j);
                    if( tDWORD < subreg_MIN) subreg_MIN = tDWORD;
                    if( tDWORD > subreg_MAX) subreg_MAX = tDWORD;
                    fftdat[k] = tDWORD;
                    k++;
                }
                tDWORD_ptr += header[NCHAN];
            }
            // autoclip means that clip values have been set and should be used
            if ( autoclip ) {
                // get the histogram of the current subarray
                binsize = data_histogram(fftdat,subreg_MIN,subreg_MAX,FFT_area);
                // find the maximum of the histogram, exclude the extreme points
                histmax = 0;
                for(i=1; i< HISTOSIZE-1; i++) {
                    if( histo_array[i] > histmax ){
                        histmax = histo_array[i];
                    }
                }
                // Calculate low clip values from histogram
                loval = loclip * histmax;
                for(i=0; i< HISTOSIZE-1; i++) {
                    if(histo_array[i] > loval)
                        break;
                }
                loval = i*binsize + subreg_MIN;
                // Calculate high clip values from histogram
                hival = hiclip * histmax;
                for(i=HISTOSIZE-2; i > 0; i--) {
                    if(histo_array[i] > hival)
                        break;
                }
                hival = i*binsize + subreg_MIN;
                // clip fftdat to values b/w loval and hival
                setbounds(fftdat,loval,hival,FFT_area);
            }
            // load data from frame A into a float (FFT) array
            k = 0;
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    before_A[k]	= fftdat[k];
                    after_A[k]	= 0.0;
                    k++;
                }
            }
            //printf("Image1 loaded\n");
            
            // copy frame B sub-region into an array, get the min and max, and clip
            k = 0;
            tDWORD_ptr = datpt + doffset + (ulx+shiftx) + (uly+shifty)*header[NCHAN] -1 ;
            subreg_MAX = subreg_MIN = *(tDWORD_ptr+1);
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    tDWORD = *(tDWORD_ptr+j);
                    if( tDWORD > subreg_MAX) subreg_MAX = tDWORD;
                    if( tDWORD < subreg_MIN) subreg_MIN = tDWORD;
                    fftdat[k] = tDWORD;
                    k++;
                }
                tDWORD_ptr += header[NCHAN];
            }
            // autoclip has been set and high/low values should be used
            if( autoclip ) {
                // get the histogram of the current subarray
                binsize = data_histogram(fftdat,subreg_MIN,subreg_MAX,FFT_area);
                // find the maximum of the histogram, exclude the extreme points
                histmax = 0;
                for(i=1; i< HISTOSIZE-1; i++) {
                    if( histo_array[i] > histmax ){
                        histmax = histo_array[i];
                    }
                }
                // Calculate low clip values from histogram
                loval = loclip * histmax;
                for(i=0; i< HISTOSIZE-1; i++) {
                    if(histo_array[i] > loval)
                        break;
                }
                loval = i*binsize + subreg_MIN;
                // Calculate high clip values from histogram
                hival = hiclip * histmax;
                for(i=HISTOSIZE-2; i > 0; i--) {
                    if(histo_array[i] > hival)
                        break;
                }
                hival = i*binsize + subreg_MIN;
                // clip fftdat to values b/w loval and hival
                setbounds(fftdat,loval,hival,FFT_area);
            }
            // load data from frame A into a float (FFT) array
            k = 0;
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    before_B[k] = fftdat[k];
                    after_B[k] = 0.0;
                    k++;
                }
            }
            //printf("image2 loaded\n");
            
            // FFT on frame A
            temp_int = FFT_rows;
            FT2D(before_A,after_A,work1,work2,&temp_int,&temp_int);
            
            // FFT on frame B
            temp_int = FFT_rows;
            FT2D(before_B,after_B,work1,work2,&temp_int,&temp_int);
            
            // Multiply (in a bizarre fashion) the two FFT arrays together
            // ... values go into before_A and after_A
            k = 0;
            for(i=1; i<=fftsize; i++) {
                for(j=1; j<=fftsize; j++) {
                    if( 1 & (i+j)){ // ????
                        temp_float = -before_A[k]*before_B[k] - after_A[k]*after_B[k];  /* introduces a shift of NTRAK/2,NCHAN/2 */
                        after_A[k] = -before_A[k]*after_B[k] + before_B[k]*after_A[k];  /* to the result of the inverse fft */
                        before_A[k] = temp_float;
                    }else{
                        temp_float = before_A[k]*before_B[k] + after_A[k]*after_B[k];
                        after_A[k] = before_A[k]*after_B[k] - before_B[k]*after_A[k];
                        before_A[k] = temp_float;
                    }
                    k++;
                }
            }
            //perform inverse FFT on float arrays
            temp_int = -FFT_rows;
            FT2D(before_A,after_A,work1,work2,&temp_int,&temp_int);
            
            // get magnitude and the maximum for frame A
            FFT_max = 0.0;
            for(i=0; i<FFT_area; i++) {
                before_A[i] = sqrt(before_A[i]*before_A[i] + after_A[i]*after_A[i]);
                if( before_A[i] > FFT_max )	FFT_max = before_A[i];
            }
            // echo maximum in cross-correlation image
            sprintf(sstring,"max of inverse = %.2f\n", FFT_max);
            printf(sstring);
            // write cross-correlation to fftdat buffer
#ifdef FLOAT
            for(i=0; i<FFT_area; i++) {
                fftdat[i] = before_A[i];
            }
#else
            for(i=0; i<FFT_area; i++) {
                fftdat[i] = before_A[i]*MAXDATAVALUE/FFT_max;
            }
#endif            
            // Find peaks in fftdat (X-corr image). Search half the image plus a bit
            peaks(FFT_width, FFT_width/2+2*area, area, radius, fftdat);
            // elimnate duplicate points
            k = NPEAK;
            for(k=0; k<peaks_to_save-1; k++){
                // calculate x & y centroid -- use this for the actual peak location
                x1 = y1 = 0.0;
                dval_sum = 0;
                min = FFT_max;
                /* find local minimum */
                for(i=yloc[k]; i<yloc[k]+sum_area; i++) {
                    for(j=xloc[k]; j<xloc[k]+sum_area; j++) {
                        if(i<0) i = 0;
                        if(j<0) j = 0;
                        if(i>fftsize-1) i =fftsize-1;
                        if(j>fftsize-1) j =fftsize-1;
                        dval = *(before_A + (i)*fftsize + j);
                        if(dval<min)
                            min = dval;
                    }
                }
                for(i=yloc[k]; i<yloc[k]+sum_area; i++) {
                    for(j=xloc[k]; j<xloc[k]+sum_area; j++) {
                        if(i<0) i = 0;
                        if(j<0) j = 0;
                        if(i>fftsize-1) i =fftsize-1;
                        if(j>fftsize-1) j =fftsize-1;
                        dval = *(before_A + (i)*fftsize + j) - min;
                        dval_sum += dval;
                        x1 += j * dval;			/* x center of mass */
                        y1 += i * dval;			/* y center of mass */
                    }
                }
                x1 /= dval_sum;
                y1 /= dval_sum;
                sprintf(sstring,"%.2f\t%.2f",x1,y1);
                //printf(sstring);
				/*
                if( area > 0 ) {
                    printf("\t%d\t",biggies[k]/(area*area));
                } else {
                    printf("\t%d\t",biggies[k]);
                }
				*/
                xpeaks[num] = x1;
                ypeaks[num] = y1;
                peakval[num] = biggies[k];
                num++;
            }
skip:
            // find velocities
            x1 = ulx + xpeaks[num - peaks_to_save + 1];
            y1 = uly + ypeaks[num - peaks_to_save + 1];
            velocity = ( x1 - x0 ) * ( x1 - x0 ) + (  y1 - y0 ) * ( y1 - y0 );
            velocity = sqrt( velocity );
            sprintf(sstring,"\nv =\t%.2f\tx =\t%.2f\ty =%.2f\n", velocity,x1,y1);
            pprintf(sstring);
            if( plotduringpiv) {
                x1 = scale*(xpeaks[num-peaks_to_save+1] - fftsize/2);
                y1 = scale*(ypeaks[num-peaks_to_save+1] - fftsize/2);
                x1 = ulx + xpeaks[num-peaks_to_save+1] + x1;
                y1 = uly + ypeaks[num-peaks_to_save+1] + y1;
                get_vect_dir(x0,y0,&x1,&y1);
                pivplot(PLOT,x0,y0,x1,y1,1);
            }
            //check interupt status
            checkstoprequest();
            // handle interrupt event
            if( mystopstatus != stopstatus) {
                pprintf("piv command stopped.\n");
                if(uly == 0) {					/* still in the first row */
                    no_of_vely = 1;
                    no_of_velx = (ulx)/boxinc + 1;
                }
                else {
                    no_of_vely = (uly)/boxinc;
                }
                printf("New velocity array size is %d by %d.\n",no_of_velx,no_of_vely);
                goto loopexit;
            }
            // replace image data with auto-correlation and exit
            if( show_auto ) {
                k = 0;
                min = fftdat[0];
                for(i=0; i<FFT_area; i++) {
                    if(fftdat[i] < min) min = fftdat[i];
                }
                for(i=0; i<fftsize; i++) {
                    for(j=0; j<fftsize; j++) {
                        *(datpt+doffset + ulx + uly*header[NCHAN]+j+i*header[NCHAN]) = fftdat[k]-min;
                        k++;
                    }
                }
            }
        }	// End of major loop X
    }		// End of major loop Y

loopexit:
        // free working memory structures
        free(before_A);
    free(after_A);
    free(before_B);
    free(after_B);
    free(work1);
    free(work2);
    free(fftdat);
    // close PIV plotting fuction
    if( plotduringpiv) pivplot(CLOSE,x0,y0,x1,y1,0);
        // free image pointer
        if(frameA_ptr!=0) {
            free(frameA_ptr);
            frameA_ptr = 0;
        }
    // find min and max values in our new image
    have_max = false;
    // reset cursor
    setarrow();
	return 0;
     } 



/****************************************************************************
	P I V M A S S A G E  -	Auto correct a PIV image using an interative 
			application of PIVALT 3 clicking
****************************************************************************/
int pivmassage()
{
    int	cv_x, cv_y;			// current vector position (x, y)
    int				i, j;				// loop counters

    float			Vel;				// Velocity components of the current vector


    // Check for valid PIV vector field in buffer
    if (no_of_velx == 0) {
        printf("Must have valid PIV image first.\n");
        beep();
        return -1;
    }

    // set the watch cursor
    

    // Loop through all the velocity vectors
    for ( cv_y = 0; cv_y < no_of_vely; cv_y++ ){

        // Update watch cursor
        

        for ( cv_x = 0; cv_x < no_of_velx; cv_x++ ){

            // Find the value of this Vector
            Vel	= vel( cv_x, cv_y, 1);

            // Do a PIVALT 3 (massage) click on non-zero vectors surrounding a dud
            if (Vel == 0) {

                // Find the valid vectors in neighbouring region and click
                for( i= -1; i<=1; i++) {
                    for( j= -1; j<=1; j++) {

                        // Check that neighbours are on the page
                        if ( i == 0 && j == 0 ) continue;
                        if ( cv_x + j < 0 ) continue;
                        if ( cv_x + j >= no_of_velx ) continue;
                        if ( cv_y + i < 0 || cv_y + i >= no_of_vely ) continue;

                        // If the vector is non-zero then PIVALT 3 (massage) click it
                        if ( (vel(cv_x + j, cv_y + i, 1)) != 0) {
                            massage_vector( cv_x + j, cv_y + i);
                        }

                    }
                }
            }
        }
    }

    // reset arrow cursor
    setarrow();
	return 0;
} /* pivmassage */



/****************************************************************************
	P I V M A G N I T U D E   -	Strip bad vectors out of a PIV image
****************************************************************************/
int pivmagnitude(int n, int index)
{
    Boolean			SimilarityCheckOK = true;
    int				cv_x, cv_y,			// current vector position (x, y)
        i, j;			// loop counters
    int				NeighbourCnt;
    int				MaskSize = 2;		/* 1 -> 3 by 3
        2 -> 5 by 5
        3 -> 7 by 7 */

    float			VelX, VelY, Vel;	// Velocity components of the current vector
    float			MeanVelX, MeanVelY, MeanVel;	// Mean quantities
    float			Vel_temp;

    float			Arg1;				// Command line arguements
    int				VectorsSeen = 0;
    int				VectorsKilled = 0;

    // Check for valid PIV vector field in buffer
    if (no_of_velx == 0) {
        printf("Must have valid PIV image first.\n");
        beep();
        return -1;
    }

    // Read command line arguments ( Arg1, Arg2 [default = 0.0])
    if ( sscanf(&cmnd[index],"%f", &Arg1) != 1) {
    } else {
        accuracy = Arg1;
    }

    // set the watch cursor
    

    // Loop through all the velocity vectors
    for ( cv_y = 0; cv_y < no_of_vely; cv_y++ ){
        for ( cv_x = 0; cv_x < no_of_velx; cv_x++ ){

            // Find the value of this Vector
            VelX	= vx( cv_x, cv_y, 1);
            VelY	= vy( cv_x, cv_y, 1);
            Vel		= vel( cv_x, cv_y, 1);

            // Initialise loop variables
            NeighbourCnt = 0;
            MeanVelX		= 0;
            MeanVelY		= 0;
            MeanVel			= 0;
            SimilarityCheckOK = true;

            // Find the valid vectors in neighbouring region and average
            for( i= -MaskSize; i<=MaskSize; i++) {
                for( j= -MaskSize; j<=MaskSize; j++) {

                    // Check that neighbours are on the page
                    if ( i == 0 && j == 0 ) continue;
                    if ( cv_x + j < 0 ) continue;
                    if ( cv_x + j >= no_of_velx ) continue;
                    if ( cv_y + i < 0 || cv_y + i >= no_of_vely ) continue;

                    // Check that the vector is non-zero
                    if ( (Vel_temp =vel(cv_x + j, cv_y + i, 1)) == 0) continue;

                    // Otherwise keep track of total
                    NeighbourCnt++;
                    MeanVelX	+= vx( cv_x + j, cv_y + i, 1);
                    MeanVelY	+= vy( cv_x + j, cv_y + i, 1);
                    MeanVel		+= vel(	cv_x + j, cv_y + i, 1);
                }
            }
            // Compute Means
            MeanVelX	/= NeighbourCnt;
            MeanVelY	/= NeighbourCnt;
            MeanVel		/= NeighbourCnt;

            // Check for vector similarity to the surrounds
            if ( !CheckMagnitude(Vel, MeanVel) )	SimilarityCheckOK = false;

            // If vector differs from mean vel OR angle by too much... KILL IT!!
            if ( !SimilarityCheckOK && Vel != 0 ) {
                VectorsKilled++;
                kill_vector( cv_x, cv_y);
            }
            VectorsSeen++;
        }
    }

    // reset arrow cursor
    setarrow();

    printf("%d vectors deleted from %d\n", VectorsKilled, VectorsSeen);
	return 0;
} /* pivmagnitude */



/****************************************************************************
	P I V A N G L E   -	Strip bad vectors out of a PIV image (ANGLE)
****************************************************************************/
int pivangle(int n, int index)
{
    Boolean	SimilarityCheckOK = true;
    int	cv_x, cv_y,			// current vector position (x, y)
        i, j;			// loop counters

    float	Arg1;			// Command line arguements
    int	VectorsSeen = 0;
    int	VectorsKilled = 0;

    int	mustHaveXsame = 2;		// Minimum number of similar vectors in neighbourhood
    int	SimilarCnt;			// Running total of similar vectors

    float	testAngle, firstAngle;		// Angle include between Vectors
    float	Vel_temp;


    // Check for valid PIV vector field in buffer
    if (no_of_velx == 0) {
        beep();
		printf("Must have valid PIV image first.\n");
        return -1;
    }

    // Read command line arguments ( Arg1, Arg2 [default = 0.0])
    if ( sscanf(&cmnd[index],"%f", &Arg1) != 1) {
        Arg1 = 15.0;
    }

    // set the watch cursor
    

    // Loop through all the velocity vectors
    for ( cv_y = 0; cv_y < no_of_vely; cv_y++ ){
        for ( cv_x = 0; cv_x < no_of_velx; cv_x++ ){

            // Find the angular value of the current vector
            firstAngle	= angle(cv_x, cv_y, 1);
            SimilarityCheckOK	= false;
            SimilarCnt	= 0;

            // Find the valid vectors in neighbouring region and average
            for( i= -1; i<=1; i++) {
                for( j= -1; j<=1; j++) {

                    // Check that neighbours are on the page
                    if ( i == 0 && j == 0 ) continue;
                    if ( cv_x + j < 0 ) continue;
                    if ( cv_x + j >= no_of_velx ) continue;
                    if ( cv_y + i < 0 || cv_y + i >= no_of_vely ) continue;

                    // Check that the vector is non-zero
                    if ( (Vel_temp =vel(cv_x + j, cv_y + i, 1)) == 0) continue;

                    // Otherwise find its angle and see if its OK
                    testAngle = angle( cv_x + j, cv_y + i, 1);

                    // Count the number of Similar angles
                    if (CheckAngle(firstAngle, testAngle, Arg1)) SimilarCnt++;

                    // Are there enough others like this one??
                    if ( SimilarCnt >= mustHaveXsame)	SimilarityCheckOK = true;
                }
            }

            // If vector is too weird... KILL IT!!
            if ( !SimilarityCheckOK ) {
                VectorsKilled++;
                kill_vector( cv_x, cv_y);
            }
            VectorsSeen++;
        }
    }

    // reset arrow cursor
    setarrow();

    printf("%d vectors deleted from %d\n", VectorsKilled, VectorsSeen);
    return 0;
} /* pivangle */



/***************************************************************************
		P I V F I L -	Interpolate velocities around available vectors
			from neighbouring vectors. i.e. comes from shift
			function
***************************************************************************/
int pivfil()  
{

    int			i, j;				// loop counters
    int			ptx, pty;			// the current vector [x, y]
    int			numbetter = 0;		// number of vectors interpolated
    float		vx_ave, vy_ave, neighbors;
    Boolean		Filled = false;		// Is the vector field full??
    float		*xpeaks2, *ypeaks2;

    // allocate memory for temporarily storing vectors
    xpeaks2 = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);
    ypeaks2 = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * 4);

    if(xpeaks2 == 0 || ypeaks2 == 0) {
        nomemory();
        if(xpeaks2 != 0) free(xpeaks2);
        if(ypeaks2 != 0) free(ypeaks2);
        return -1;
    }

    // store piv data in another array
    for(i=0; i<no_of_velx * no_of_vely * peaks_to_save;i++){
        xpeaks2[i] = xpeaks[i];
        ypeaks2[i] = ypeaks[i];
    }

    // Set watch cursor
    

    // Keep going until the vector field is filled
    while (!Filled){
        // Reset the 'numbetter' counter at each iteration
        numbetter = 0;

        // replace zero velocities near the edge of the velocity field with the local average
        for(pty = 0; pty < no_of_vely; pty++ ){
            for(ptx = 0; ptx < no_of_velx; ptx++ ){

                if ( vel(ptx,pty,1) == 0.0 )  {

                    // get the average velocity in this region excluding this center point
                    vx_ave = vy_ave = neighbors = 0.0;

                    for( i= -1; i<=1; i++) {
                        if ( pty+i < 0 || pty+i >= no_of_vely ) continue;

                        for( j= -1; j<=1; j++) {

                            if ( i == 0 && j == 0 ) continue;	// don't count this point
                            if ( ptx+j >= no_of_velx ) break;	// don't run off the page
                            if ( ptx+j < 0 ) continue;
                            if ( vx(ptx+j,pty+i,1) == 0.0 && vy(ptx+j,pty+i,1) == 0.0) continue;  // don't include previously removed vectors in local avg

                            neighbors += 1.0;
                            vx_ave += vx(ptx+j,pty+i,1);
                            vy_ave += vy(ptx+j,pty+i,1);
                        }
                    }

                    if(neighbors != 0.0) {
                        vx_ave /= neighbors;
                        vy_ave /= neighbors;

                        j = (ptx + pty*no_of_velx)*peaks_to_save;
                        xpeaks2[j+1] = xpeaks2[j] + vx_ave;
                        ypeaks2[j+1] = ypeaks2[j] + vy_ave;
                        numbetter++;
                    }
                }
            }  // Next column of vectors
        }  // Next row of vectors

        // Transfer vectors from the corrected array into the real PIV buffer
        for(i=0; i<no_of_velx * no_of_vely * peaks_to_save;i++){
            xpeaks[i] = xpeaks2[i];
            ypeaks[i] = ypeaks2[i];
        }

        // There are no more vectors to correct so we may stop now...
        if (numbetter == 0) Filled = true;

    }

    // Free memory structures
    free(xpeaks2);
    free(ypeaks2);

    // Set arrow cursor
    have_max = false;
    setarrow();
    return 0;
}



/****************************************************************************
	M A S S A G E _ V E C T O R  -	massage the 9 by 9 vector
				neighbourhood of the selected vector
****************************************************************************/
void
massage_vector (int nx, int ny)
{
    float	originalx, originaly;
    float	neighbourx, neighboury;
    int	i, j, k, m;
    float	diffx, diffy, diff,
        min_diff, min_peak;

    // get vector information
    originalx	= vx( nx, ny, 1);
    originaly	= vy( nx, ny, 1);

    // compare adjacent vectors and discard those beyond limits.
    for( i= -1; i<=1; i++)
    {
        for( j= -1; j<=1; j++)
        {
            //Quick test that the neighbours actually exist.
            if (i==0 && j==0) continue;			// dont process base vector
            if ( nx+j >= no_of_velx ) break;	// don't run off the page
            if ( nx+j < 0 ) continue;			// first vector in a row
            if ( ny+i < 0 || ny+i >= no_of_vely ) break;	// top or bottom of page

            //initialise our counters for comparison
            min_peak = 1;
            min_diff = 10000;	//something large so that the first

            //peak becomes the best peak initially
            // execute a loop for each of the saved peaks
            for(k=1;k<=peaks_to_save;k++)
            {
                //get value of each peak for this neighbour
                neighbourx	= vx( nx + j, ny + i, k);
                neighboury	= vy( nx + j, ny + i, k);
                //compare to the previous best fit
                diffx	= neighbourx - originalx;
                diffy	= neighboury - originaly;
                diff	= sqrt( diffx * diffx + diffy * diffy); //difference from original vector

                if (diff <= min_diff)
                {
                    min_peak = k;	//this is the best peak so far
                    min_diff = diff;
                }
            }
            //swap this neighbour to the best peak
            for( m=1;m<min_peak;m++)
            {
                rotate_vector(nx+j, ny+i);
            }
        }
    }
} /* massage_vector */



/****************************************************************************
	R M S _ V E C T O R  -	Calculate and print the RMS of the 9 by 9 vector
 			neighbourhood of the selected vector
****************************************************************************/
void
rms_vector (int nx, int ny)
{
    float	originalx, originaly;
    int	i, j;
    float	MemoryX[9],		// temp arrays to hold the values of valid vectors
        MemoryY[9];
    float	MeanVelX, MeanVelY,	// Mean velocity of vector neighbourhood
        RMSVelX, RMSVelY;	// RMS velocity of vector neighbourhood
    int	NeighbourCnt;	// Count of valid neighbours

    // get vector information
    originalx	= vx( nx, ny, 1);
    originaly	= vy( nx, ny, 1);

    // Initialise counters
    NeighbourCnt = 0;
    MeanVelX = 0;
    MeanVelY = 0;
    RMSVelX = 0;
    RMSVelY = 0;

    // compare adjacent vectors and discard those beyond limits.
    for( i= -1; i<=1; i++) {
        for( j= -1; j<=1; j++) {
            //Quick test that the neighbours actually exist.
            if ( nx+j >= no_of_velx ) break;	// don't run off the page
            if ( nx+j < 0 ) continue;		// first vector in a row
            if ( ny+i < 0 || ny+i >= no_of_vely ) break;	// top or bottom of page

            // Check that the vector is non-zero
            if ( vel(nx + j, ny + i, 1) == 0) continue;

            // Otherwise add the value in the temporary arrays
            MemoryX[NeighbourCnt] = vx( nx + j, ny + i, 1);
            MemoryY[NeighbourCnt] = vy( nx + j, ny + i, 1);
            MeanVelX += vx( nx + j, ny + i, 1);
            MeanVelY += vy( nx + j, ny + i, 1);
            NeighbourCnt++;
        }
    }
		
    // Find Mean velocity of neighbourhood
    if (NeighbourCnt) {
        MeanVelX /= (float) NeighbourCnt;
        MeanVelY /= (float) NeighbourCnt;
    } else {
        MeanVelX = 0;
        MeanVelY = 0;
    }

    // Calculate RMS statisitics
    for( i= 0; i<NeighbourCnt; i++ ){
        RMSVelX += ((MemoryX[i] - MeanVelX)*(MemoryX[i] - MeanVelX));
        RMSVelY += ((MemoryY[i] - MeanVelY)*(MemoryY[i] - MeanVelY));
    }
    RMSVelX = sqrt( RMSVelX/((float)NeighbourCnt-1) );
    RMSVelY = sqrt( RMSVelY/((float)NeighbourCnt-1) );

    // Print out the Neighbourhood statistics
    sprintf(sstring, "[%d %d]: RMS [%.2f,%.2f], Mean [%.2f,%.2f]\nOMA>", nx, ny, RMSVelX, RMSVelY, MeanVelX, MeanVelY);
    printf(sstring);

} /* rms_vector */


/****************************************************************************
	Q U E R Y _ V E C T O R  -	Print the velocity information about the 
			selected vector
****************************************************************************/
void
query_vector(int numx, int numy)
{
    int		num, nump;
    float	v, x0, x1, y0, y1;

    // Find position in xpeaks/ypeaks array
    num = (numy * no_of_velx) + numx;
    nump = num*peaks_to_save;

    x0 = xpeaks[nump];
    y0 = ypeaks[nump];

    x1 = xpeaks[nump+1];
    y1 = ypeaks[nump+1];

    v = (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0);
    v = sqrt(v);
    sprintf(sstring, "X:\t%.2f Y:\t%.2f Magnitude - \t%.2f\nOMA>",x1-x0 , y1-y0 , v);
    printf(sstring);
                    
} /* query_vector */



/****************************************************************************
	V E C T O R _ A N G L E  -	Print the velocity information about the 
			selected vector
****************************************************************************/
void
vector_angle(int numx, int numy)
{
    float	Angle;

    // Calculate the angle
    Angle = angle(numx, numy, 1);

    // Print the Angle
    sprintf(sstring, "The Angle is %.3f\n", Angle);
    printf(sstring);

} /* vector_angle */


/****************************************************************************
	K I L L _ V E C T O R -	Sets the velocity of the first (visible) selected
			vector to zero, subsequent peaks are unaffected
****************************************************************************/
void
kill_vector(int numx, int numy)
{	
    int			num, nump;
    float		x0, x1, y0, y1;
    float		scale;
	
    // Calc position in xpeak/ypeak arrays
    num = (numy * no_of_velx) + numx;
    nump = num*peaks_to_save+1;

    // the center of this box (used in plotting)
    x0 = numx*boxinc + fftsize/2;
    y0 = numy*boxinc + fftsize/2;

    scale = pivscale - 1.0;
    x1 = scale*(xpeaks[nump] - fftsize/2);
    y1 = scale*(ypeaks[nump] - fftsize/2);
					
    x1 = numx*boxinc + xpeaks[nump] + x1;
    y1 = numy*boxinc + ypeaks[nump] + y1;

    // Erase vector from plot
    get_vect_dir(x0,y0,&x1,&y1);
	pivplot(INIT_CURRENT,0,0,0,0,0);
    pivplot(SET_WHITE,0,0,0,0,0);
    pivplot(PLOT,x0,y0,x1,y1,1);

    // Set vector to zero
    xpeaks[nump] = 	xpeaks[nump-1];
    ypeaks[nump] = 	ypeaks[nump-1];

    x1 = scale*(xpeaks[nump] - fftsize/2);
    y1 = scale*(ypeaks[nump] - fftsize/2);

    x1 = numx*boxinc + xpeaks[nump] + x1;
    y1 = numy*boxinc + ypeaks[nump] + y1;

    pivplot(SET_BLACK, 0, 0,0,0,0);
	pivplot(CLOSE_CURRENT, 0, 0,0,0,0);
	
} /* kill_vector */


/****************************************************************************
	R O T A T E _ V E C T O R -Sets the velocity of the first (visible) selected
			vector to zero, subsequent peaks are unaffected
****************************************************************************/
void
rotate_vector(int numx, int numy)
{
    int			num, nump;
    float		x0, x1, y0, y1;
    float		scale;
	
    // Calc position in xpeaks/ypeaks arrays
    num = (numy * no_of_velx) + numx;
    nump = num*peaks_to_save+1;

    // the center of this box (used in plotting)
    x0 = numx*boxinc + fftsize/2;	
    y0 = numy*boxinc + fftsize/2;

    scale = pivscale - 1.0;

    x1 = scale*(xpeaks[nump] - fftsize/2);
    y1 = scale*(ypeaks[nump] - fftsize/2);
					
    x1 = numx*boxinc + xpeaks[nump] + x1;
    y1 = numy*boxinc + ypeaks[nump] + y1;

    // Erase vector from plot
    get_vect_dir(x0,y0,&x1,&y1);
	pivplot(INIT_CURRENT, 0,0,0,0,0);
    pivplot(SET_WHITE, 0,0,0,0,0);
    pivplot(PLOT,x0,y0,x1,y1,1);

    // Get vector based on next peak and plot
    piv_reorder( num+1, 0);
	
    x1 = scale*(xpeaks[nump] - fftsize/2);
    y1 = scale*(ypeaks[nump] - fftsize/2);
			
    x1 = numx*boxinc + xpeaks[nump] + x1;
    y1 = numy*boxinc + ypeaks[nump] + y1;
	
    get_vect_dir(x0,y0,&x1,&y1);
	
    pivplot(SET_BLACK,0,0,0,0,0);
    pivplot(PLOT,x0,y0,x1,y1,1);
	pivplot(CLOSE_CURRENT, 0,0,0,0,0);
	
} /* rotate_vector */



/****************************************************************************
	C H E C K M A G N I T U D E  -	Are the two velocities similar?
				Uses the threshold value 'accuracy'
****************************************************************************/
Boolean
CheckMagnitude (float Vel1, float Vel2)
{
    float	ATD, Largest;	// Absolute total difference
    
	
    // What's the difference?
    ATD = abs(Vel1 - Vel2);
	
    // Which is the largest value?
    if (Vel2 >= Vel1)
        Largest = Vel2;
    else
        Largest = Vel1;
	
    //sprintf(sstring, "Vel 1 %.3f Vel 2 %.3f\n", Vel1, Vel2 );
    //printf(sstring);
    if ( ATD/Largest > (1-accuracy) ) {
        sprintf(sstring, "%.3f %.3f %.3f\n", ATD/Largest, Vel1, Vel2);
        printf(sstring);
        return(false);	// test is NOT ok
    } else {
        return(true);	// test is OK
    }
	
}  /* CheckMagnitude */



/****************************************************************************
	C H E C K A N G L E -	Does the vector have ANY neighbours that are
			within 15 deg. of the original.
				YES ->	must be given a chance...
				NO ->	die, bad vector, die!
****************************************************************************/
Boolean
CheckAngle(float angle1, float angle2, float tolerance)
{
    float	temp;

    // Find the difference between the two angles
    temp = abs(angle1 - angle2);

    // Correct values out of range
    if (temp < 0) temp += 360;

    // 'tolerance' is in range
    if (tolerance <= 0.0)	tolerance = 15.0;
    if (tolerance >= 360.0)	tolerance = 15.0;

    if (temp >tolerance)
        return(false);
    else
        return(true);

}  /* CheckAngle */



     /* ************************************************************ */
int condtnl(ulx,uly,fftsize)
     /* Use a binary image in Background buffer to condition velocity
     returns 1 if velocity should be evaluated, 0 if not */
     int ulx, uly, fftsize;
     {
         int sum,datavg,nr,nc,cnt,jump;
         int nrows = fftsize/2;      /* number of rows to sum over */
         DATAWORD *datptcopy,*bcopy;
         extern unsigned int backsize;
         extern int localavg;
         if(backdat == 0) {
             beep();
             printf("Must Define a Background.\n");
             return -1;
         }
         if( (header[NCHAN]*header[NTRAK]+MAXDOFFSET)*DATABYTES != backsize) {
             beep();
             printf("Background Size Conflicts With Data.\n");
             return -1;
         }
         
         datptcopy = datpt + doffset;
         bcopy = backdat + doffset;
         datavg = sum = cnt = localavg = 0;
         jump = fftsize/nrows;
         for(nr=uly+jump; nr<uly + fftsize; nr += jump) {
             for(nc=ulx; nc<ulx + fftsize; nc++) {
                 cnt++;
                 sum += *(bcopy + nc + nr*header[NCHAN] -1);
                 datavg += *(datptcopy + nc + nr*header[NCHAN] -1);
             }
         }
         datavg /= cnt;
         /*  printf("local avg = %d  cnt = %d\n",datavg,cnt);  */
         if(sum < cnt/2) {
             return(0);
         }else {
             if( sum<cnt )          /* If there are some 0's in this subregion of the template set the
                 local average of the data */
                 localavg = datavg;
             return(1);
         }
         setarrow();
     }
     /* ************************************************************ */
     
                               
