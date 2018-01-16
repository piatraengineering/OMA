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
#include "oma_quartz.h"

extern int	no_of_velx;				/* the number of velocities calculated in the x and y directions */
extern int	no_of_vely;
extern int peaks_to_save;			/* the number of peaks to save */
extern int*  peakval;
extern int fftsize;
extern int boxinc;
extern int sum_area;
extern int exclude_radius;
//extern GWorldPtr	myGWorldPtrs[]; // offscreen maps

extern OMA_Window oma_wind[];

extern float* xpeaks;
extern float* ypeaks;

#define INIT 0
#define PLOT 1
#define CLOSE 2
#define SET_WHITE 3
#define SET_BLACK 4
#define INIT_CURRENT 5
#define CLOSE_CURRENT 6

int printf(),query_vector(),rotate_vector(),kill_vector(),massage_vector(),rms_vector(),vector_angle(),FT2D(),chooseposn(),openwindow_q();

/* ************* */
int put_text(char* string, float x, float y)
{
	extern CGContextRef	lgContext,lgpdfContext;	
	extern int 	lgheight;
	CGContextShowTextAtPoint(lgContext,x,lgheight - y,string,strlen(string));
	CGContextShowTextAtPoint(lgpdfContext,x,lgheight - y,string,strlen(string));

	return 0;

}

int pivplot(int icase,float x0,float y0,float x1,float y1, int do_flush)

{
	extern	TWOBYTE		header[];
	extern	short		pixsiz;
	extern int labelplot;
	extern char scratch[];
	extern	int			gwnum;
	extern CGContextRef	lgContext;
	void set_window_type(int,int);

	static int pnum;
	
	float 	dx,dy,len;
	extern int 	lgwidth;
	extern int 	lgheight;
	float		arrowx = 5;
	float		arrowy = 2;

	switch(icase) {
		case INIT_CURRENT:
			QDBeginCGContext(GetWindowPort(FrontWindow()), &lgContext);
			CGContextSetLineWidth(lgContext, 1);
			CGContextSetLineJoin(lgContext,kCGLineJoinRound);
			CGContextSetLineCap(lgContext,kCGLineCapRound);
			CGContextBeginPath(lgContext);
			break;
		case CLOSE_CURRENT:
			CGContextStrokePath(lgContext);
			CGContextFlush(lgContext);
			QDEndCGContext (GetWindowPort(FrontWindow()), &lgContext);
			break;

		case INIT:
			if( x0 == 0.0){
				lgwidth = header[NCHAN];
				lgheight = header[NTRAK];
			} else {
				lgwidth = x1;
				lgheight = y1;			
			}
			x0 = lgwidth;			
			x1 = lgheight;
			if( pixsiz > 0 ) {
				lgwidth = pixsiz * lgwidth;
				lgheight = pixsiz * lgheight; }
			else {
				lgwidth = lgwidth/abs(pixsiz);
				lgheight = lgheight/abs(pixsiz);
			}
			if(!ploton(1,lgheight,lgwidth,0,"PIV")) return(0);
			//oma_wind[gwnum].windowtype = QPIVGRAPH;
			set_window_type(gwnum,QPIVGRAPH);
			scalx(0.0,x0);
			scaly(0.0,x1);
			pnum = 0;
			break;
			
		case PLOT:
		
			dx = x1 - x0;
			dy = y1 - y0;
			len = sqrt(dx*dx +dy*dy);
		
			set(x0,y0);        /* Put pen down*/
			if(labelplot) {
				pnum++;
				sprintf(scratch,"%d",pnum);
				put_text(scratch,x0,y0);
				set(x0,y0);
			}
				
    		dvect(x1,y1);		/* Link to appropriate point*/
			
			if(len != 0.0) {
				x0 = x1 - dx/len*arrowx + dy/len*arrowy;
				y0 = y1 - dx/len*arrowy - dy/len*arrowx;
				set(x0,y0);
				dvect(x1,y1);

				x0 = x1 - dx/len*arrowx - dy/len*arrowy;
				y0 = y1 + dx/len*arrowy - dy/len*arrowx;

				//dvect(x0,y0); 
				//dvect(x1,y1);
				set(x0,y0);		// I like open arrows j.f.
				dvect(x1,y1);	// swap these two for above two to get closed arrows.
			}
			CGContextStrokePath(lgContext );
			if(do_flush) CGContextFlush(lgContext);
			
			break;
			
		case CLOSE:

			reset();
			break;
			
		case SET_WHITE:
			CGContextSetRGBStrokeColor (lgContext, 1, 1, 1, 1);
			//RGBForeColor(&RGBWhite);
			break;
			
		case SET_BLACK:
			CGContextSetRGBStrokeColor (lgContext, 0, 0, 0, 1);
			//RGBForeColor(&RGBBlack);
			break;
			
	} /* end of switch */
	return(1);
}


extern short fftwidth,fftheight;
extern int is_fft_window;
extern int is_auto_window;

int fft_image(Point center_point)
{
	return 0;
}
Ptr Get_fft_from_image_buffer(int allocate_new,Point center_point)
{
	
	extern DATAWORD *datpt;
	extern int 	doffset,ncolor,gwnum;
	extern int histo_array[];
	extern int autoclip;
	extern float loclip,hiclip;
	extern	TWOBYTE		header[];
	extern RGBColor color[256][8];
	extern int      thepalette;
	
	Ptr ptr=0,ptrf=0,ptra=0;
	DATAWORD *point;
	int i,j,k,n2,ni,n;
	int gwf=0,gwa=0;
	int pindx,indx,ncm1;
	float cm,crange,fpindx;
	float 	*ap,*bp,*work1p,*work2p;
	float max;
	
	DATAWORD *imagedat,*fftdat,datval,dvmin,dvmax;
	DATAWORD loval, hival,cmax,cmin;

	int fftarraysize,ulx,uly,binsize,histmax;
	
	if( fftwidth > fftsize || fftheight > fftsize) {
		beep();
		printf("FFT Size Has Changed.\n");
		return NULL;
	}
	if(allocate_new > 0){
		ptrf = ptra = ptr = calloc(fftwidth*fftheight,4);			// window size
		if(ptr == NULL){
			beep();
			printf("memory error\n");
			return ptr;
		}
		oma_wind[gwnum].width = fftwidth;
		oma_wind[gwnum].height = fftheight;
	} else {
		if(is_fft_window){
			for( gwf=0; gwf<= gwnum; gwf++){
				if( oma_wind[gwf].windowtype == QFFT)
					break;
			}
			// check here for actually found the zoom?
			ptrf = oma_wind[gwf].window_rgb_data;			// the fft window number
		}
		if(is_auto_window){
			for( gwa=0; gwa<= gwnum; gwa++){
				if( oma_wind[gwa].windowtype == QAUTO)
					break;
			}
			// check here for actually found the zoom?
			ptra = oma_wind[gwa].window_rgb_data;			// the fft window number
		}
	}


	fftarraysize = fftsize * fftsize;
	n2 = fftsize;

	ap = (float *)malloc(fftarraysize*4);
	bp = (float *)malloc(fftarraysize*4);
	work1p = (float *)malloc(n2*4);
	work2p = (float *)malloc(n2*4);
	fftdat = (DATAWORD*)malloc(fftarraysize*DATABYTES);
	
	if( ap==0 || bp==0 || work1p==0 || work2p == 0 || fftdat == 0) {
		nomemory();
		if(ap != 0) free(ap);
		if(bp != 0) free(bp);
		if(work1p != 0) free(work1p);
		if(work2p != 0) free(work2p);
		if(fftdat != 0) free(fftdat);
		return NULL;
	}
	
	uly = center_point.v - fftheight/2;
	if( uly<0) uly = 0;
	if( uly+fftheight >= header[NTRAK] ) uly = header[NTRAK] - fftheight - 1;
	ulx = center_point.h - fftwidth/2;
	if( ulx<0) ulx = 0;
	if( ulx+fftwidth >= header[NCHAN] ) ulx = header[NCHAN] - fftwidth - 1;
	
	
	// copy the subarray data into an array, get the min and max 

	imagedat = datpt + doffset + ulx + uly*header[NCHAN] -1 ;	// first data pt
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
			
		// get the histogram of the current subarray 
				
		binsize = data_histogram(fftdat,dvmin,dvmax,fftarraysize);
				
		// find the maximum of the histogram, exclude the extreme points 
				
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

		loval = i*binsize + dvmin;
		for(i=HISTOSIZE-2; i > 0; i--) {
			if(histo_array[i] > hival)
				break;
		}
		hival = i*binsize + dvmin;

		//printf("Peak at: %d Floor: %d Ceiling: %d\n",
							//j*binsize + dvmin,loval,hival);
				
		setbounds(fftdat,loval,hival,fftarraysize);
				
	}

	// load up the real arrays with the subarray data
			
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
	
	
	// do the initial fft 
			
	FT2D(ap,bp,work1p,work2p,&fftsize,&n2);
	
	// get the magnitude squared  

	max = 0.0;
	for(i=0; i< fftarraysize;i++) {
		ap[i] =  ap[i]*ap[i] + bp[i]*bp[i]; 	
		if( max < ap[i] ) max = ap[i];
	}

	if( is_fft_window ) {
		// change to DATAWORDs
#ifdef FLOAT
		dvmin = dvmax = ap[0];
		for(i=1; i< fftarraysize;i++) {
			fftdat[i] = ap[i];
			if( fftdat[i] > dvmax) dvmax = fftdat[i];
			if( fftdat[i] < dvmin) dvmin = fftdat[i];
		}
#else
		dvmin = dvmax = ap[0]*MAXDATAVALUE/max;
		for(i=1; i< fftarraysize;i++) {
			fftdat[i] = ap[i] / max * MAXDATAVALUE;
			if( fftdat[i] > dvmax) dvmax = fftdat[i];
			if( fftdat[i] < dvmin) dvmin = fftdat[i];
		}
#endif
		// get histogram 
		do { 
			binsize = data_histogram(fftdat,dvmin,dvmax,fftarraysize);
			k = 0;
			histmax = fftarraysize * 0.99;	/* want to set the max at 99% */
			for(i=0; i< HISTOSIZE; i++) {
				k+= histo_array[i];
				if( k >= histmax ){
					break;
				}
			}
			dvmax = dvmax/2;
			
		}  while ( (i<= 4) && (binsize > 4)); 
		if( i==0 ) i = 1;
		cmax = i*binsize+dvmin;
		cmin = dvmin;
		//printf("%d %d cmin,cmax.\n",cmin,cmax);
		crange = cmax-cmin;
		ncm1 = (ncolor-1);
		cm = cmin;
		
		point = fftdat;
	
		n = i = 0;
		while(++i <= fftheight){
			j = 0;
			//dpoint += extra;
			while( ++j <= fftwidth){
				indx = *(point++) - cm;
				fpindx = (float)indx * ncm1;
				pindx = fpindx/ crange;
				if( pindx > ncm1)
					pindx = ncm1;
				if( pindx < 0)
					pindx = 0;
				++pindx;
				*(ptrf+n++) = pindx;
				*(ptrf+n++) = color[pindx][thepalette].red/256;
				*(ptrf+n++) = color[pindx][thepalette].green/256;
				*(ptrf+n++) = color[pindx][thepalette].blue/256;
			}
		}
	}
	if( is_auto_window && allocate_new != 1) {
		/* play with signs */

		k = 0;
		for(i=1; i<=fftsize; i++) {
			for(j=1; j<=fftsize; j++) {
				if( 1 & (i+j)) 
					ap[k] =  -ap[k]; 	// for optical rather than standard ordering 
				bp[k] = 0.0; 
				k++;
			}
		}
		

		// do the second fft 
		
		ni = -n2;

		FT2D(ap,bp,work1p,work2p,&ni,&ni);
		

		// get the magnitude and the maximum  
			
		max = 0.0;
		for(i=0; i<fftarraysize; i++) {
			ap[i] = sqrt(ap[i]*ap[i] + bp[i]*bp[i]); 
			if( ap[i] > max )
				max = ap[i];
		}
		//printf(" Max is %f.\n",max);
#ifdef FLOAT
		dvmin = dvmax = ap[0];
		for(i=1; i< fftarraysize;i++) {
			fftdat[i] = ap[i];
			if( fftdat[i] > dvmax) dvmax = fftdat[i];
			if( fftdat[i] < dvmin) dvmin = fftdat[i];
		}
#else
		dvmin = dvmax = ap[0]*MAXDATAVALUE/max;	
		for(i=1; i<fftarraysize; i++) {
			fftdat[i] = ap[i]*MAXDATAVALUE/max;
			if( fftdat[i] > dvmax) dvmax = fftdat[i];
			if( fftdat[i] < dvmin) dvmin = fftdat[i];
		}
#endif		
		// get histogram
		do { 
			binsize = data_histogram(fftdat,dvmin,dvmax,fftarraysize);
			k = 0;
			histmax = fftarraysize * 0.99;	/* want to set the max at 99% */
			for(i=0; i< HISTOSIZE; i++) {
				k+= histo_array[i];
				if( k >= histmax ){
					break;
				}
			}
			dvmax /= 2;
			
		} while ( (i<= 4) && (binsize > 4) ); 
		if( i==0 ) i = 1;
		cmax = i*binsize+dvmin;
		cmin = dvmin;
			
		//printf("%d %d cmin,cmax.\n",cmin,cmax);		
/*	
		for( gw=0; gw<= gwnum; gw++){
			if( oma_wind[gw].windowtype == AUTO)
				break;
		}
*/
		crange = cmax-cmin;
		ncm1 = (ncolor-1);
		cm = cmin;

		point = fftdat;
	
		n = i = 0;
		while(++i <= fftheight){
			j = 0;
			while( ++j <= fftwidth){
				indx = *(point++) - cm;
				fpindx = (float)indx * ncm1;
				pindx = fpindx/ crange;
				if( pindx > ncm1)
					pindx = ncm1;
				if( pindx < 0)
					pindx = 0;
				++pindx;
				*(ptra+n++) = pindx;
				*(ptra+n++) = color[pindx][thepalette].red/256;
				*(ptra+n++) = color[pindx][thepalette].green/256;
				*(ptra+n++) = color[pindx][thepalette].blue/256;
			}
		}

	
	}
	free(ap);
	free(bp);
	free(work1p);
	free(work2p);
	free(fftdat);

	MTViewData* myViewptr;	
	if(is_fft_window && !allocate_new){
		myViewptr = (MTViewData*) oma_wind[gwf].view_data_ptr;
		HIViewSetNeedsDisplay(myViewptr->theView, true);
	}
	if(is_auto_window && !allocate_new){
		myViewptr = (MTViewData*) oma_wind[gwa].view_data_ptr;
		HIViewSetNeedsDisplay(myViewptr->theView, true);
	}
	if(is_auto_window && allocate_new)
		return ptra;
	if(is_fft_window && allocate_new)
		return ptrf;

	return NULL;
	
}
/* ************* */


/*__________________________________________________________________________*/
	
int change_vector(Point point, short width, short height)
{
	extern int	no_of_velx;				/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int 	boxinc;
	extern int 	fftsize;
	extern float pivscale;
	
	int nx,ny,num,nump;
	float x,y,scale,x0,x1,y0,y1;
	
	//printf("%d by %d\n",width,height);
	
	x = point.h * (no_of_velx+1);
	x /= width;
	y = point.v * (no_of_vely+1);
	y /= height;
	
	nx = x - 0.5;
	ny = y - 0.5;
	num = (ny * no_of_velx) + nx;
	nump = num*peaks_to_save+1;
	
	x0 = nx*boxinc + fftsize/2;		/* the center of this box (used in plotting) */
	y0 = ny*boxinc + fftsize/2;

	scale = pivscale - 1.0;
	x1 = scale*(xpeaks[nump] - fftsize/2);
	y1 = scale*(ypeaks[nump] - fftsize/2);
			
	x1 = nx*boxinc + xpeaks[nump] + x1;
	y1 = ny*boxinc + ypeaks[nump] + y1;
	
	get_vect_dir(x0,y0,&x1,&y1);
	pivplot(INIT_CURRENT,0,0,0,0,0);
	pivplot(SET_WHITE,x0,y0,x1,y1,0);
	pivplot(PLOT,x0,y0,x1,y1,1);

	piv_reorder( num+1,1);
	
	// printf("%d %d x y.\n",nx,ny); 
	
	x1 = scale*(xpeaks[nump] - fftsize/2);
	y1 = scale*(ypeaks[nump] - fftsize/2);
			
	x1 = nx*boxinc + xpeaks[nump] + x1;
	y1 = ny*boxinc + ypeaks[nump] + y1;
	
	get_vect_dir(x0,y0,&x1,&y1);
	
	pivplot(SET_BLACK,x0,y0,x1,y1,0);
	pivplot(PLOT,x0,y0,x1,y1,1);
	pivplot(CLOSE_CURRENT,0,0,0,0,0);
	return 0;
}

/*__________________________________________________*/
int base_vector (Point point, short width, short height, int mode)
{
    extern int	no_of_velx;				/* the number of velocities calculated in the x and y directions */
    extern int	no_of_vely;
    extern int	no_of_vel;
    int	nx, ny;
    float	x, y;
    
    if(no_of_velx == 0 && no_of_vely == 0) return 0;  // must not be a PIV image run away

    //Get coordinates of the currently selected vector.
    x = point.h * (no_of_velx+1);
    x /= width;
    y = point.v * (no_of_vely+1);
    y /= height;
    nx = x - 0.5;
    ny = y - 0.5;
    if (no_of_vel > 0) { // PIV along contour needs to be handled differently
        switch (mode) {
            case 1:
                change_vector(point,width,height);
                break;
            case 2:
                //zero_vector(point,width,height);
                // these routines must be in an older version with contour processing; remove for now
                break;
            case 3:
            case 4:
            default:
                //vector_info(point,width,height);
                // these routines must be in an older version with contour processing; remove for now
                break;
        }
    } else {
		//printf("%d %d\n",nx,ny);
        switch (mode) {
            case 0:
                query_vector (nx, ny);
                break;
            case 1:
                rotate_vector (nx, ny);
                break;
            case 2:
                kill_vector (nx, ny);
                break;
            case 3:
                massage_vector (nx, ny);
                break;
            case 4:
                rms_vector (nx, ny);
                break;
            case 5:
                vector_angle (nx, ny);
                break;
            default:
                query_vector (nx, ny);
                break;
        }
    }
    return 0;
}

