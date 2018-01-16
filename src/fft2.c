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

extern int	alter_vectors;

extern int	have_max;
extern char cmnd[];
extern short xloc[];
extern short yloc[]; 
extern int biggies[];
extern Point substart,subend;
extern int	no_of_velx,no_of_vely,peaks_to_save,fftsize,boxinc;
extern float *xpeaks,*ypeaks;	

extern int	npts,doffset;
extern DATAWORD *datpt,min,max;		/* the data pointer */
extern DATAWORD idat(int,int);
extern TWOBYTE header[];

extern float vel(),vx(),vy();

int im_points = 0;
float* xp_loc = 0;
float* yp_loc = 0;
int* p_inten = 0;
int* p_area = 0;
int pim_x = 0;
int pim_y = 0;
int pim_thresh = 0;
int* p_pairs = 0;
int* p_pairs_tot = 0;
int npairs = 0;
int npairs_tot = 0;
int new_points_flag = 1;

int 	no_of_vel=0;		/* for velocities along contour lines -- not now implemented */
int     piv_cnt;  			/* flag = 1 --> count nonzero vectors when using PIVADD*/
float* xpeaks2 = 0;
float* ypeaks2 = 0;

int printf(),pprintf(),printxyzstuff(),multiply(),FT2D(),pivarith();


#define PAIRS 2048


/* ************** */

int peakim(int n,int index)
/* 	replace the image data by a peaks only image */
/*	PEAKIM peakval sum_area exclude_radius
	points greater that peakval are included
*/
{
	int i,m,rad;
		
	DATAWORD *dpt;
	
	extern int	npts,doffset;
	extern DATAWORD *datpt,min,max;		/* the data pointer */
	extern DATAWORD idat(int,int);
	extern TWOBYTE header[];


	/* Check to see if there was a second argument */
	
	m = 0;
	rad = 1;
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d %d",&n,&m,&rad);
			break;
		}
	}
		
	
	
	/* go and get the peaks */
	
	dpt = datpt+doffset;
	
	peaks(header[NCHAN],header[NTRAK],m,rad,dpt);	
	
	while( dpt < datpt+doffset+npts){		/* clear the image */
		*dpt++ = 0;
	}
	dpt = datpt+doffset;
	
	for(i=0; i<NPEAK; i++) {
		if( biggies[i]/m/m < n)
			break;
		*(dpt + header[NCHAN]*yloc[i] + xloc[i]) = n;
	}
	printf("%d peaks.\n",i);
	min = 0;
	max = n;
	have_max = 1;

	setarrow();
	return 0;
}

/* ************** */

int pntimg()
/* 	Replace the image data by a points only image.  Points must first be generated 
	via the POINTS command.
*/
{
	int xl,yl,i;
		
	DATAWORD *dpt,val;
	
	extern int	npts,doffset;
	extern DATAWORD *datpt;		/* the data pointer */
	extern DATAWORD idat(int,int);
	extern TWOBYTE header[];
	
	if(im_points == 0) {
		beep();
		printf("No points defined.\n");
		return -1;
	}

	
	
	dpt = datpt+doffset;
	
	while( dpt < datpt+doffset+npts){		/* clear the image */
		*dpt++ = 0;
	}
	dpt = datpt+doffset;
	
	for(i=0; i<im_points; i++) {
		xl = xp_loc[i] + 0.5;
		yl = yp_loc[i] + 0.5;
		val = p_inten[i]/p_area[i];
		if( xl >= header[NCHAN] || yl >= header[NTRAK]) {
			beep();
			printf("Current image is too small.\n");
			return -1;
		}
		*(dpt + header[NCHAN]*yl + xl) = val;
	}
	have_max = 0;

	setarrow();
	return 0;
}
/* ************** */

int primg(int type)
/* 	Replace the image data by a point-pair image.  Points must first be generated 
	via the POINTS command and pairs generated via the PAIRS command.
*/
{
	int xl,yl,i,j,x;
		
	DATAWORD *dpt,val;
	
	extern int	npts,doffset;
	extern DATAWORD *datpt;		/* the data pointer */
	extern DATAWORD idat(int,int);
	extern TWOBYTE header[];

	if(npairs == 0) {
		beep();
		printf("No points defined.\n");
		return -1;
	}
	
	
	dpt = datpt+doffset;
	
	while( dpt < datpt+doffset+npts){		/* clear the image */
		*dpt++ = 0;
	}
	dpt = datpt+doffset;
	
	for(i=0; i<npairs; i++) {
		j = p_pairs[i];
		xl = xp_loc[j] + 0.5;
		yl = yp_loc[j] + 0.5;
		val = p_inten[j]/p_area[j];
		if( xl >= header[NCHAN] || yl >= header[NTRAK]) {
			beep();
			printf("Current image is too small.\n");
			return -1;
		}
		if(type == 0)
			*(dpt + header[NCHAN]*yl + xl) = val;
		else if(type == 1)
			*(dpt + header[NCHAN]*yl + xl) = p_inten[j];
		else if(type == 2) {
			if( i & 1 ) {
				x = p_inten[j-1]*100/p_inten[j];
				*(dpt + header[NCHAN]*yl + xl) = x;
			} else {
				*(dpt + header[NCHAN]*yl + xl) = 100;
			}
		}
	}
	have_max = 0;

	setarrow();
	return 0;
}

/* ************** */

int prinfo()
/* Provide info on point pairs.  Points must first be generated 
	via the POINTS command and pairs generated via the PAIRS command.
*/
{
	int i,j,k,diff;
		
	int val1,val2,v1_ave,v2_ave,a1_ave,a2_ave;
	float ratio,ratio_ave;
	
	
	
	extern DATAWORD idat(int,int);
	

	if(npairs == 0) {
		beep();
		printf("No points defined.\n");
		return -1;
	}
	
	printf("Pair\tI1\tI2\tA1\tA2\tRatio\tDiff.\n");
	
	v1_ave = v2_ave = a1_ave = a2_ave = 0;
	ratio_ave = 0.0;
	
	for(i=0; i<npairs; i+=2) {
		j = p_pairs[i];
		k = p_pairs[i+1];
		val1 = p_inten[j]/p_area[j];
		v1_ave += val1;
		a1_ave += p_area[j];
		val2 = p_inten[k]/p_area[k];
		v2_ave += val2;
		a2_ave += p_area[k];
		ratio = (float)val1/(float)val2;
		ratio_ave += ratio;
		diff = val1-val2;
		printf("%d\t%d\t%d\t%d\t",i/2+1,val1,val2,p_area[j]);
		printf("%d\t%.2f\t%d\n",p_area[k],ratio,diff);
	}
		
		pprintf("%d\t%.2f\t%.2f\t",npairs/2,(float)v1_ave/(float)(npairs/2),
				(float)v2_ave/(float)(npairs/2));
		pprintf("%.2f\t%.2f\t",(float)a1_ave/(float)(npairs/2),
				(float)a2_ave/(float)(npairs/2));
		pprintf("%.2f\t%.2f\n",ratio_ave/(float)(npairs/2),
				(float)(v1_ave-v2_ave)/(float)(npairs/2));

	setarrow();
	return 0;
}

/* ************** */

int points(int n)
/*  */
/*	POINTS clipval 
	points greater that clipval are included	
*/
{
	int nx,ny;
		
	DATAWORD *dpt;
	
	extern int	doffset;
	extern DATAWORD *datpt;		/* the data pointer */
	extern TWOBYTE header[];
	
	
	
	new_points_flag = 1;
	
	/* go and get the peaks */
	
	dpt = datpt+doffset;
	nx = header[NCHAN];
	ny = header[NTRAK];
	get_points(nx,ny,0,0,n,dpt);	
	pim_x = nx;
	pim_y = ny;
	xp_loc = (float *) realloc(xp_loc,im_points*4);
	yp_loc = (float *) realloc(yp_loc,im_points*4);
	p_inten = (int *) realloc(p_inten,im_points*4);
	p_area = (int *) realloc(p_area,im_points*4);
	
	setarrow();
	return 0;
}

#define DATA(X,Y)	(*(dpt+(X)+(Y)*nx))
#define STACK_SIZE	10000
#define POINT_BUFF_SIZE	256

int get_points(int nx,int ny,int offx,int offy,int threshold,DATAWORD* dpt)
{
	
	int i,j,area,inten,count,x,y;
	static long bufsize;
	DATAWORD val;
	float xcent,ycent;
	short *xpt,*ypt,*xpt2,*ypt2;
	
	xpt = xpt2 = (short *)malloc(STACK_SIZE*2);
	ypt = ypt2 = (short *)malloc(STACK_SIZE*2);

	if( new_points_flag ) {
		im_points = 0;
	
		bufsize = POINT_BUFF_SIZE;
	
		if( xp_loc != 0) free(xp_loc);
		if( yp_loc != 0) free(yp_loc);
		if( p_inten != 0) free(p_inten);
		if( p_area != 0) free(p_area);
	
		xp_loc = (float *) malloc(bufsize*4);
		yp_loc = (float *) malloc(bufsize*4);
		p_inten = (int *) malloc(bufsize*4);
		p_area = (int *) malloc(bufsize*4);

		if( xp_loc==0 || yp_loc==0 || p_inten==0 || p_area == 0 || xpt==0 || 
			ypt==0) {
			nomemory();
			if(xp_loc != 0) free(xp_loc);
			if(yp_loc != 0) free(yp_loc);
			if(p_inten != 0) free(p_inten);
			if(p_area != 0) free(p_area);
			if(xpt != 0) free(xpt);
			if(ypt != 0) free(ypt);
			return -1;
		}
	}
	pim_thresh = threshold;
	
	for(j=0; j< ny; j++){
		for(i=0; i<nx; i++) {
			/* printf("%d\n",DATA(i,j)); */
			if(DATA(i,j) < threshold)
				continue;
			
			/* setup this first point */
			
			*xpt = i;
			*ypt = j;
			count = 1;
			area = inten = 0;
			xcent = ycent = 0.0;
			
			while( count > 0 ) {
			
				/* count the last point on the stack */
				x = *xpt;
				y = *ypt;
				val = DATA(x,y);
				xcent += x*val;
				ycent += y*val;
				inten += val;
				area++;
				DATA(x,y) = 0;
				count--;
				xpt--;
				ypt--;
			
				/* look at the four directly connected neighbors */
				
				/* look left */
				if( x > 0) {
					if(DATA(x-1,y) >= threshold){
						*(++xpt) = x-1;
						*(++ypt) = y;
						if( ++count >= STACK_SIZE) goto OverFlow;						
					}
				}
						
				/* look right */
				if( x < (nx-1)) {
					if(DATA(x+1,y) >= threshold){
						*(++xpt) = x+1;
						*(++ypt) = y;
						if( ++count >= STACK_SIZE) goto OverFlow;
					}
				}
				/* look up */
				if( y > 0) {
					if(DATA(x,y-1) >= threshold){
						*(++xpt) = x;
						*(++ypt) = y-1;
						if( ++count >= STACK_SIZE) goto OverFlow;
					}
				}
						
				/* look down */
				if( y < (ny-1)) {
					if(DATA(x,y+1) >= threshold){
						*(++xpt) = x;
						*(++ypt) = y+1;
						if( ++count >= STACK_SIZE) goto OverFlow;
					}
				}
			
			}
			/* have now finished one point -- have the centroid, area, & intensity */

			xpt++;	/* make the stack right */
			ypt++;
			
			xp_loc[im_points] = xcent/inten + offx;
			yp_loc[im_points] = ycent/inten + offy;
			p_area[im_points] = area;
			p_inten[im_points] = inten;
			
			im_points++;
			
			if(im_points >= bufsize) {
				/* printf("realloc: %d.\n",bufsize); */
				bufsize += POINT_BUFF_SIZE;
				xp_loc = (float *) realloc(xp_loc,bufsize*4);
				yp_loc = (float *) realloc(yp_loc,bufsize*4);
				p_inten = (int *) realloc(p_inten,bufsize*4);
				p_area = (int *) realloc(p_area,bufsize*4);
				if( xp_loc==0 || yp_loc==0 || p_inten==0 || p_area == 0 ) {
					pprintf("Can't store %d points. Out of memory.\n",bufsize) ;
					if(xp_loc != 0) free(xp_loc);
					if(yp_loc != 0) free(yp_loc);
					if(p_inten != 0) free(p_inten);
					if(p_area != 0) free(p_area);
					free(xpt2);
					free(ypt2);
					return -1;
				}

			}	
			
		}	

	}
	printf("%d Points.\n",im_points);

	have_max = 0;
	
	free(xpt);
	free(ypt);
	
	return(1);
	
OverFlow:
	printf("Stack Overflow -- single point area too large.\n");
	free(xpt2);
	free(ypt2);
	return(-1);
}
/* ************** */

int pntmos(int n,int index)
/* Find the points (particle images usually) in a large image.
*/
{
	int nx,ny;
	int ulx,uly;
	int i,j,k,histmax,binsize,clipval;
	
	DATAWORD loval, hival;
	DATAWORD *imagedat,*fftdat,datval,dvmin,dvmax;
	
	float scale = 0.5;
	
	extern int	doffset;
	extern DATAWORD *datpt;		/* the data pointer */
	extern TWOBYTE header[];
	extern int histo_array[];	
	extern int fftsize,autoclip;
	
	extern float loclip,hiclip;


	/* Get one floating point argument */
		
	sscanf(&cmnd[index],"%f",&scale);
	
	printf("%.2f Scale Factor.\n",scale);

	/* allocate memory */
	
	fftdat = (DATAWORD*)malloc(fftsize*fftsize*DATABYTES);
	if( fftdat == 0) {
		nomemory();
		free(fftdat);
		return -1;
	}
	
	
	new_points_flag = 1;
	
	/* calculate how many subimages will be processed */
		
	printf("%d by %d\n",header[NCHAN]/fftsize +1,header[NTRAK]/fftsize +1);

	/* get the next subarray of the current image */
	
	for(uly = 0; uly < header[NTRAK]; uly += fftsize){
		if( uly+fftsize < header[NTRAK] )
			ny = fftsize;
		else
			ny = header[NTRAK] - uly;
		for(ulx = 0; ulx < header[NCHAN]; ulx += fftsize){
		
			if( ulx !=0 || uly !=0 )
				new_points_flag = 0;
			
			/* print the location of this box */
			
			if( ulx+fftsize < header[NCHAN] )
				nx = fftsize;
			else
				nx = header[NCHAN] - ulx;
			
			printxyzstuff(ulx,uly,0);
			printxyzstuff(ulx+nx-1,uly+ny-1,1);
			
			/* copy the subarray data into an array, get the min and max */

			imagedat = datpt + doffset + ulx + uly*header[NCHAN] -1 ;	/* first data pt */
			k = 0;
			dvmax = dvmin = *(imagedat+1);
			for(i=1; i<=ny; i++) {
				for(j=1; j<=nx; j++) {
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
				
				binsize = data_histogram(fftdat,dvmin,dvmax,fftsize*fftsize);
				
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
				loval = i*binsize + dvmin;
				for(i=HISTOSIZE-2; i > 0; i--) {
					if(histo_array[i] > hival)
						break;
				}
				hival = i*binsize + dvmin;

				/* printf("Peak at: %d Floor: %d Ceiling: %d\n",
									j*binsize + dvmin,loval,hival); */
				
				dvmin = loval;
				dvmax = hival;
				
			}
			
			clipval = dvmin + scale*(dvmax-dvmin);
			/* printf("nx: %d, ny: %d, ulx: %d, uly: %d, clip : %d\n",nx,ny,ulx,uly,clipval); */
			printf("Clip at %d\n",clipval);
			if( get_points(nx,ny,ulx,uly,clipval,fftdat) == -1)
				break;	/* error */
		}
	}	
	pim_x = header[NCHAN];
	pim_y = header[NTRAK];
	xp_loc = (float *) realloc(xp_loc,im_points*4);
	yp_loc = (float *) realloc(yp_loc,im_points*4);
	p_inten = (int *) realloc(p_inten,im_points*4);
	p_area = (int *) realloc(p_area,im_points*4);

	setarrow();
	return 0;
}

//
int getpairs(float dx,float dy,float rad)

{

	int i,j;
	float xp1,xp2,yp1,yp2,dist;
	
	rad = rad*rad;
	npairs = 0;
	
	if( p_pairs != 0) free(p_pairs);
	
	p_pairs = (int *) malloc(PAIRS*4);
	
	for(i=0; i<im_points; i++) {
	
		/* first, check to see if this point is within the region being considered */
		if ( xp_loc[i] < (float)substart.h || xp_loc[i] > (float)subend.h )
			continue;
		if ( yp_loc[i] < (float)substart.v || yp_loc[i] > (float)subend.v )
			continue;
			
		xp1 = xp_loc[i] + dx;
		yp1 = yp_loc[i] + dy;
		xp2 = xp_loc[i] - dx;
		yp2 = yp_loc[i] - dy;
		/* check for matches */
		for(j=i+1; j<im_points; j++) {
			if ( xp_loc[j] < (float)substart.h || xp_loc[j] > (float)subend.h )
				continue;
			if ( yp_loc[j] < (float)substart.v || yp_loc[j] > (float)subend.v )
				continue;

			dist = (xp1-xp_loc[j])*(xp1-xp_loc[j]) + (yp1-yp_loc[j])*(yp1-yp_loc[j]);
			if( dist < rad ) {
				/* have a pair here */
				p_pairs[npairs++] = i;
				p_pairs[npairs++] = j;
				if( npairs >= PAIRS-2 ) {
					beep();
					printf("Too many pairs.\n");
					return -1;
				}	
				break;
			}
			dist = (xp2-xp_loc[j])*(xp2-xp_loc[j]) + (yp2-yp_loc[j])*(yp2-yp_loc[j]);
			if( dist < rad ) {
				/* have a pair here */
				p_pairs[npairs++] = i;
				p_pairs[npairs++] = j;
				if( npairs >= PAIRS-2 ) {
					beep();
					printf("Too many pairs.\n");
					return -1;
				}	
				break;
			}
 		}
	}
	printf("%d pairs found.\n",npairs/2);
	return 0;
}

/* ************** */

int pairpr(int n,int index)
/* 	Process pair statistics for an entire image. Assumes that matching PIV and
	POINT have already been calculated (e.g., via the PIV and POINTS or PNTMOS
	commands).  Prints out statistics corresponding to pairs for each velocity
	vector.
*/
{

	int i;
	int ulx,uly,nc,nt;
	int num = 0;
		
	float radius = 1.0;
	float dx,dy;
	
	extern int fftsize;
	
	if( no_of_velx == 0 || im_points == 0) {
		beep();
		printf("Must have valid PIV and POINT data.\n");
		return -1;
	}
	
	/* Get one floating point argument */
		
	sscanf(&cmnd[index],"%f",&radius);
	
	printf("%.2f Pair Radius.\n",radius);
	
	npairs_tot = 0;

	if( p_pairs_tot != 0) free(p_pairs_tot);
	
	p_pairs_tot = (int *) malloc(PAIRS*4);

	
	
	/* how many subimages will be processed */
		
	printf("%d by %d\n",no_of_velx,no_of_vely);

	/* get the next subarray of the current image */
	
	nc = (no_of_velx-1)*boxinc+fftsize;
	nt = (no_of_vely-1)*boxinc+fftsize;

	for(uly = 0; uly <= nt - fftsize; uly += boxinc){
		for(ulx = 0; ulx <= nc - fftsize; ulx += boxinc){
		
			substart.h = ulx;
			substart.v = uly;
			subend.h = ulx + fftsize-1;
			subend.v = uly + fftsize-1;
			
			dx = *(xpeaks+num) - *(xpeaks+num+1);
			dy = *(ypeaks+num) - *(ypeaks+num+1);

			keylimit(-1);			/* disable printing */
									
			getpairs(dx,dy,radius);
			
			prinfo();
			keylimit(-2);			/* reset printing  to previous mode*/

			num += peaks_to_save;

			/* save these pairs */
			
			
			for(i=0; i< npairs; i++) {
				p_pairs_tot[npairs_tot++] = p_pairs[i];
				if( npairs_tot >= PAIRS) {
					beep();
					printf("OVerflow.\n");
					setarrow();
					return -1;
				}
			}
		}
	}
	for(i=0; i<npairs_tot; i++) {
		p_pairs[i] = p_pairs_tot[i];
	}
	npairs = npairs_tot;
	printf("Total of %d pairs.\n",npairs_tot/1+1);
	setarrow();
	return 0;

}

/* ************** */

int pairs(int n,int index)
/* 	find pairs of particles separated by dx, dy (to within radius)
	asssumes arrays exist that contain image points */
/*	PAIRS dx dy radius
*/
{
	int i;
	
	float dx,dy,rad;
	
	
	extern DATAWORD idat(int,int);
	extern TWOBYTE header[];


	/* Get 3 floating point arguments */
	
	rad = 1;
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			i = sscanf(&cmnd[index],"%f %f %f",&dx,&dy,&rad);
			if( i != 3) {
				beep();
				printf("Three arguments needed.\n");
				return -1;
			}
			break;
		}
	}
		
	
	
	/* go and get the pairs that lie within the bounds of the current image */
	
	substart.h = substart.v = 0;
	subend.h = header[NCHAN] - 1;
	subend.v = header[NTRAK] - 1;
	
	getpairs(dx,dy,rad);	

	setarrow();
	return 0;
}



/* ************** */
int mask(int n, int index)
	/* make a rectangular mask */
{
	DATAWORD *datp;
	
    extern DATAWORD *datpt;
	extern TWOBYTE header[];
	extern int	doffset;
	extern char cmnd[];
	
	int width, height,i,j,x0,y0;
	
	width = height = n;
	
	have_max = 0;	/* won't have max right */

	/* Check to see if there was a second argument */
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d",&width,&height);
			break;
		}
	}
	
	if( width < 1 || height < 1 || width > header[NCHAN] || height > header[NTRAK]) {
		printf("Illegal mask size.\n");
		beep();
		return -1;
	}
	
	datp = datpt + doffset;
	x0 = (header[NCHAN]-width)/2;
	y0 = (header[NTRAK]-height)/2;
	for(i=0; i<header[NTRAK]; i++) {
		for(j=0; j<header[NCHAN]; j++) {
			if( j >= x0 && j < (header[NCHAN]-x0) &&
			    i >= y0 && i < (header[NTRAK]-y0) )
				*datp++ = 1;
			else 
				*datp++ = 0;
		}
	}
	setarrow();	
	return 0;
}
	
/* ************** */
//the following routines are from jonathan frank's oma version
/* ************** */
	
int crossc(int n, int index)		/* perform cross-correlation of image in buffer with specified file */
{
	extern char     cmnd[],*fullname(),headcopy[];
    extern TWOBYTE  header[],trailer[];
    extern DATAWORD *datpt,mathbuf[];
	
	DATAWORD *datp;
	FILE *fp, *fopen();
    int i,j,k,n1,n2,ni,size;
	TWOBYTE *pointer;	/* point to things in the header */	
	int filesf;		/* Scale factor for file data */
	int ramsf;		/* Scale factor for data in RAM */
	
	int mathlen;
	float max,dummy;     
	float *ap1,*bp1,*ap2,*bp2,*work1,*work2;

	/* Open the file and read the header */

    if ((fp = fopen(fullname(&cmnd[index],GET_DATA),"r")) == NULL) {
		beep();
		printf(" File Not Found.\n");
		return -1;
	}
    fread(headcopy,256,2,fp);

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
	        fclose(fp);
		return -1;
	}


	n1 = header[NCHAN];
	n2 = header[NTRAK];
	size = n1 * n2;
	

	/***** Allocate memory *****/
		
	ap1 = (float *)malloc(size*4);
	bp1 = (float *)malloc(size*4);
	ap2 = (float *)malloc(size*4);
	bp2 = (float *)malloc(size*4);		
	work1 = (float *)malloc(n2*4);
	work2 = (float *)malloc(n2*4);
	
	if( ap1==0 || bp1==0 || ap2==0 || bp2==0 || work1==0 || work2 == 0) {
		nomemory();
		if(ap1 != 0) free(ap1);
		if(bp1 != 0) free(bp1);
		if(ap2 != 0) free(ap2);
		if(bp2 != 0) free(bp2);		
		if(work1 != 0) free(work1);
		if(work2 != 0) free(work2);
		return -1;
	}
	
	/**** load data from image1 into array *****/

	datp = datpt + doffset;
	k = 0;
	for(i=1; i<=header[NTRAK]; i++) {
		for(j=1; j<=header[NCHAN]; j++) {
			ap1[k] = *(datp++);
			bp1[k] = 0.0;
			k++;
		}
	}
	
	/***** do fft of image 1 *****/

	ni = n2;
	FT2D(ap1,bp1,work1,work2,&ni,&ni);
	
	printf("fft1 complete\n");
	
	/***** load data from image2 into arrray *****/
	
	i = 0;
	k = doffset;
	datp = datpt + doffset;
	while( i < size) {
		mathlen = fread(mathbuf,DATABYTES,MATHLEN,fp);
		
		
		for(j=k; j < mathlen; j++){
			ap2[i] = mathbuf[j];
			bp2[i] = 0.0; 
			i++;
		}
		k = 0;
	}
	fclose(fp);
	printf("image2 loaded\n");
	/***** do fft of image 2 *****/
	
	ni = n2;
	FT2D(ap2,bp2,work1,work2,&ni,&ni);
	
	printf("fft2 complete\n");
	/***** multiply fft1 and fft2 *****/
	
	k = 0;
	for(i=1; i<=header[NTRAK]; i++) {
		for(j=1; j<=header[NCHAN]; j++) {
		    if( 1 & (i+j)){ 
				dummy = -ap1[k]*ap2[k] - bp1[k]*bp2[k];  /* introduces a shift of NTRAK/2,NCHAN/2 */
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
	FT2D(ap1,bp1,work1,work2,&ni,&ni);
	
	 printf("inverse fft complete\n");
	/***** return magnitude ****/
	
	max = 0.0;
	for(i=0; i<size; i++) {
		ap1[i] = sqrt(ap1[i]*ap1[i] + bp1[i]*bp1[i]); 
		if( ap1[i] > max )
			max = ap1[i];
	}
	printf("max of inverse = %.2f\n",max);
	
	datp = datpt + doffset;
	k = 0;
	for(i=1; i<=header[NTRAK]; i++) {
		for(j=1; j<=header[NCHAN]; j++) {
			*(datp++) = ap1[k]*MAXDATAVALUE/max; 
			k++;
		}
	}
	printf("assignment complete\n");
	
	have_max = 0;
	free(ap1);
	free(bp1);
	free(ap2);
	free(bp2);		
	free(work1);
	free(work2); 

	setarrow();
	return 0;
}

/* ************** */
/* ***************** */

int pivdir()				/* alter vector file to save vectors in preferred direction */
{
	int m,i,n,num_vel;
	float cent_x,cent_y,x1,y1;
	
	if(no_of_velx == 0 && no_of_vel == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	
	
	if(no_of_vel > 0)
		num_vel = no_of_vel;
	else
		num_vel = no_of_velx*no_of_vely;
	m = 0;
    for( i=0; i< num_vel; i++) {
		cent_x = xpeaks[m];
		cent_y = ypeaks[m];
	/*	pprintf("center at %.2f\t%.2f",cent_x,cent_y); */
		m++;
		for(n=1; n<peaks_to_save; n++) {
			x1 = xpeaks[m];
			y1 = ypeaks[m];
		/*	pprintf("x1 = %.2f\ty1 = %.2f\n",x1,y1); */
			get_vect_dir(cent_x,cent_y,&x1,&y1);
		/*	pprintf("new x1 = %.2f\ty1 = %.2f\n",x1,y1); */
			xpeaks[m] = x1;
			ypeaks[m] = y1;
			m++;
		}
    }
	setarrow();
	return 0;
}

/* ***************** */

int pivavg()			/* calculates average vector over entire vector field */
{
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern float* xpeaks;
	extern float* ypeaks;
	extern Variable user_variables[];	
	
	int i,num_vectors;
	float dxav,dyav,magav;
	
	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	
	dxav = dyav = 0.0;
	num_vectors = no_of_velx*no_of_vely;
	for( i=0; i< num_vectors*peaks_to_save; i += peaks_to_save) {
		dxav += xpeaks[i+1] - xpeaks[i];
		dyav += ypeaks[i+1] - ypeaks[i];
    }
	dxav /= num_vectors;
	dyav /= num_vectors;
	magav = sqrt(dxav*dxav + dyav*dyav);
	printf("avg vector = %.2f\t%.2f\t%.2f\n",dxav,dyav,magav);
	// return values available as variables
	user_variables[0].fvalue = dxav;
	user_variables[0].is_float = 1;
	user_variables[1].fvalue = dyav;
	user_variables[1].is_float = 1;
	user_variables[2].fvalue = magav;
	user_variables[2].is_float = 1;
	
	return 0;
}

/* ***************** */

/* ************************* Vector Arithmetic ********************** */

/* ***************** */

int pivplus(int n, int index)      /* add a constant vector to the currently loaded vectors 
						 components dx and dy are real numbers */
{
	extern char cmnd[];	
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern float* xpeaks;
	extern float* ypeaks;
	
	int narg,i,m,num_vel;
	float dx,dy;

	if(no_of_velx == 0 && no_of_vel == 0) {
		beep();
		printf("Must have valid PIV image first.\n");
		return -1;
	}

	narg = sscanf(&cmnd[index],"%f %f",&dx,&dy);
	if(narg == 0){
		return -1;
	} else{
		if(narg == 1){
			dy = 0;
		}
	}

	if(no_of_vel > 0)
		num_vel = no_of_vel;
	else
		num_vel = no_of_velx*no_of_vely;
		
	m = 0;
	for( i=0; i< num_vel; i++) {
		m++;
		for(n=1; n<peaks_to_save; n++) {
			xpeaks[m] += dx;
			ypeaks[m] += dy;
			m++;
		}
    }
    return 0;
}

/* ***************** */

int pivminus(int n, int index)	/* subtract a constant vector from the currently loaded vectors */
{
	extern char cmnd[];	
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern float* xpeaks;
	extern float* ypeaks;
	
	int narg,i,m,num_vel;
	float dx,dy;

	if(no_of_velx == 0 && no_of_vel == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}

	narg = sscanf(&cmnd[index],"%f %f",&dx,&dy);
	if(narg == 0){
		return -1;
	} else{
		if(narg == 1){
			dy = 0.0;
		}
	}
	
	if(no_of_vel > 0)
		num_vel = no_of_vel;
	else
		num_vel = no_of_velx*no_of_vely;
		
	m = 0;
	for( i=0; i< num_vel; i++) {
		m++;
		for(n=1; n<peaks_to_save; n++) {
			xpeaks[m] -= dx;
			ypeaks[m] -= dy;
			m++;
		}
    }
    return 0;
}
/* ***************** */

int pivmul(int n, int index)	/* multiply the currently loaded vectors by a constant */
{
    extern char cmnd[];
    extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
    extern int	no_of_vely;
    extern int peaks_to_save;		/* the number of peaks to save */
    extern float* xpeaks;
    extern float* ypeaks;
    	int narg,i,m,j,num_vel;
        float scal,dx,dy;
        	if(no_of_velx == 0 && no_of_vel == 0) {
                    printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}

	narg = sscanf(&cmnd[index],"%f",&scal);
	if(narg == 0){
		return -1;
	}
	
	if(no_of_vel > 0)
		num_vel = no_of_vel;
	else
		num_vel = no_of_velx*no_of_vely;
		
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
}

/* ***************** */

int pivsqr()	/* get square root of each component of the currently loaded vectors  */
 
{
		
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern float* xpeaks;
	extern float* ypeaks;
	
	int i,m,j,n;
	float dx,dy;

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}

	m = 0;
	for( i=0; i< no_of_velx*no_of_vely; i++) {
		j = m;
		m++;
		for(n=1; n<peaks_to_save; n++) {
			dx = xpeaks[m] - xpeaks[j];
			dy = ypeaks[m] - ypeaks[j]; 
			xpeaks[m] = xpeaks[j] + sqrt(dx);
			ypeaks[m] = ypeaks[j] + sqrt(dy);
			m++;
		}
    }
    return 0;
}

/* ***************** */

int magsqr()	/* get magnitude squared of each vector and store in x-component  */
 
{
		
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern float* xpeaks;
	extern float* ypeaks;
	
	int i,m,j,n;
	float dx,dy;

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}

	m = 0;
	for( i=0; i< no_of_velx*no_of_vely; i++) {
		j = m;
		m++;
		for(n=1; n<peaks_to_save; n++) {
			dx = xpeaks[m] - xpeaks[j];
			dy = ypeaks[m] - ypeaks[j]; 
			xpeaks[m] = xpeaks[j] + dx*dx + dy*dy;
			ypeaks[m] = ypeaks[j];   /* set y-component to zero */
			m++;
		}
    }
    return 0;
}
/* ***************** */

int pivdiv(int n, int index)	/* divide the currently loaded vectors by a constant */
{
	extern char cmnd[];	
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern float* xpeaks;
	extern float* ypeaks;
	
	int narg,i,m,j,num_vel;
	float scal,dx,dy;

	if(no_of_velx == 0 && no_of_vel == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}

	narg = sscanf(&cmnd[index],"%f",&scal);
	if(narg == 0){
		return -1;
	}
	if (scal == 0.0){
		printf("Cannot divide by zero\n");
		return -1;
	}
	
	if(no_of_vel > 0)
		num_vel = no_of_vel;
	else
		num_vel = no_of_velx*no_of_vely;
			
	m = 0;
	for( i=0; i< num_vel; i++) {
		j = m;
		m++;
		for(n=1; n<peaks_to_save; n++) {
			dx = xpeaks[m] - xpeaks[j];
			dy = ypeaks[m] - ypeaks[j]; 
			xpeaks[m] = xpeaks[j] + dx/scal;
			ypeaks[m] = ypeaks[j] + dy/scal;
			m++;
		}
    }
    return 0;
}
/* ***************** */

int pivref(int n, int index)	/* reformat piv data to have cp as center pixel,  np peaks*/
{
	extern char cmnd[];	
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern float* xpeaks;
	extern float* ypeaks;
	extern int* peakval;
	
	int narg,i,m,j,num_vel,np,cp,*newpeak,dx;
	float *newx,*newy;

	if(no_of_velx == 0 && no_of_vel == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}

	narg = sscanf(&cmnd[index],"%d %d",&cp,&np);
	if(narg != 2){
		beep();
		printf("usage: PIVREF centerpixel npeaks\n");
		return -1;
	}
	
	if(no_of_vel > 0)
		num_vel = no_of_vel;
	else
		num_vel = no_of_velx*no_of_vely;
			
	
	/* allocate memory */
	
	newx = (float *)malloc(no_of_velx * no_of_vely * np * 4);
	newy = (float *)malloc(no_of_velx * no_of_vely * np * 4);
	newpeak = (int*)malloc(no_of_velx * no_of_vely * np * 4);
	
	if( xpeaks == 0 || ypeaks == 0 || peakval == 0) {
			nomemory();
			if(newx != 0) free(newx);
			if(newy != 0) free(newy);
			if(newpeak != 0) free(newpeak);
			return -1;
	}

	j = m = 0;
	dx = cp - xpeaks[0];
	
	if( np <= peaks_to_save){		// make less peaks
	  for( i=0; i< num_vel; i++) {
		for(n=0; n<peaks_to_save; n++) {
		  if( n < np ){
			newx[m] = xpeaks[j]+dx;
			newy[m] = ypeaks[j]+dx;
			newpeak[m] = peakval[j];
			m++;
		  }
		  j++;
		}
      }
      peaks_to_save = np;
	}
	
    if(xpeaks != 0) free(xpeaks);		/* get rid of old data if there is any */
	if(ypeaks != 0) free(ypeaks);
	if(peakval != 0) free(peakval);
	
	xpeaks = newx;
	ypeaks = newy;
	peakval = newpeak;
	
	return 0;

}
/* ***************** */

int pivdivf()	/* divide the currently loaded vectors by scalar file in buffer*/
{
		
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern float* xpeaks;
	extern float* ypeaks;
	extern int	doffset;
	extern DATAWORD *datpt;
	
	DATAWORD scal;
	int i,m,j,n;
	float dx,dy;

	if(no_of_velx == 0) {
		beep();
		printf("Must have valid PIV image first.\n");
		return -1;
	}
	if(no_of_velx != header[NCHAN] || no_of_vely != header[NTRAK])  {
		beep();
		printf("Image and Velocity field must have same dimensions.\n");
		return -1;
	}
	
	m = 0;
	for( i=0; i< no_of_velx*no_of_vely; i++) {
		scal = *(datpt + doffset + i);
		j = m;
		m++;
		if(scal == 0){    /* don't want to divide by zero */
			m += peaks_to_save-1;
		} else {
			for(n=1; n<peaks_to_save; n++) {
				dx = xpeaks[m] - xpeaks[j];
				dy = ypeaks[m] - ypeaks[j]; 
				xpeaks[m] = xpeaks[j] + dx/(float)scal;
				ypeaks[m] = ypeaks[j] + dy/(float)scal;
				m++;
			}
		}
    }
    return 0;
}
/* ***************** */

int pivmulf()	// multiply the currently loaded vectors by scalar file in buffer
{
	
	extern int	no_of_velx;			// the number of velocities calculated in the x and y directions 
	extern int	no_of_vely;
	extern int peaks_to_save;		// the number of peaks to save 
	extern float* xpeaks;
	extern float* ypeaks;
	extern int	doffset;
	extern DATAWORD *datpt;
	
	DATAWORD scal;
	int i,m,j,n;
	float dx,dy;

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	if(no_of_velx != header[NCHAN] || no_of_vely != header[NTRAK])  {
		printf("Image and Velocity field must have same dimensions.\n");
		beep();
		return -1;
	}
	
	m = 0;
	for( i=0; i< no_of_velx*no_of_vely; i++) {
		scal = *(datpt + doffset + i);
		j = m;
		m++;
		for(n=1; n<peaks_to_save; n++) {
			dx = xpeaks[m] - xpeaks[j];
			dy = ypeaks[m] - ypeaks[j]; 
			xpeaks[m] = xpeaks[j] + dx*(float)scal;
			ypeaks[m] = ypeaks[j] + dy*(float)scal;
			m++;
		}
    }
    return 0;
}


/* ***************** */
/*
int pivmulf()	// multiply the currently loaded vectors by scalar file in buffer
{
	
	extern int	no_of_velx;			// the number of velocities calculated in the x and y directions 
	extern int	no_of_vely;
	extern int peaks_to_save;		// the number of peaks to save 
	extern float* xpeaks;
	extern float* ypeaks;
	extern int	doffset;
	extern DATAWORD *datpt;
	
	int i,m,j,scal,n;
	float dx,dy;

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	if(no_of_velx != header[NCHAN] || no_of_vely != header[NTRAK])  {
		printf("Image and Velocity field must have same dimensions.\n");
		beep();
		return -1;
	}
	
	m = 0;
	for( i=0; i< no_of_velx*no_of_vely; i++) {
		scal = *(datpt + doffset + i);
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
}
*/

/* ***************** */

int pivsub(int n, int index)				/* subtract vector file from currently loaded vectors  */
{
	int err;
	
	err = pivarith(SUB,index);
	return err;
}

#define SUBC 5   /* for conditional velocity subtraction */

/* ***************** */

int pivsubc(int n, int index)				/* subtract vector file: only do subtraction from nonzero 
								vectors in currently loaded vector field  */
{
	int err;
	
	err = pivarith(SUBC,index);
	return err;
}
/* ***************** */

int pivadd(int n, int index)				/* add vector file to currently loaded vectors  */
{
	int err;
	
	err = pivarith(ADD,index);
	return err;
}

/* ***************** */

int pivarith(int code,int index)   /* vector arithmetic operations on piv files */
{
	extern char cmnd[],*fullname();	
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int  no_of_vel;			/*the number of velocities calculated along a contour */
	extern int peaks_to_save;		/* the number of peaks to save */
	extern int piv_cnt;				/* flag for counting vectors in averages */
	extern float* xpeaks;
	extern float* ypeaks;
	extern float* xpeaks2;
	extern float* ypeaks2;
	extern int	doffset;
	extern DATAWORD *datpt;
	
	float cent_x,cent_y,cent_x2,cent_y2,dx,dy,dx2,dy2,x,y;
	int m,i,n,nx,ny,num,nump;
	int num_vel,no_of_velx2,no_of_vely2,peaks_to_save2,no_of_vel2;
	
	/* make sure PIV image is currently loaded */
	
	if(no_of_velx == 0 && no_of_vel == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}

	if(no_of_vel > 0 && code == ADD) {
		printf("Not implemented for addition with contours.\n");
		beep();
		return -1;
	}
		
	if(no_of_vel == 0) {         
		num_vel = no_of_velx*no_of_vely;
	}else{  /* when piv data is along contour */
		num_vel = no_of_vel;
	}
	
	/* allocate memory */
	
	if(xpeaks2 != 0) free(xpeaks2);		/* get rid of old data if there is any */
	if(ypeaks2 != 0) free(ypeaks2);
	
	xpeaks2 = (float *)malloc(num_vel * peaks_to_save * 4);
	ypeaks2 = (float *)malloc(num_vel * peaks_to_save * 4);
	
	if( xpeaks2 == 0 || ypeaks2 == 0) {
		nomemory();
		if(xpeaks2 != 0) free(xpeaks2);
		if(ypeaks2 != 0) free(ypeaks2);
		return -1;
	}
	
	/* store piv data in another array */
		
	no_of_vel2 = no_of_vel;
	no_of_velx2 = no_of_velx;
	no_of_vely2 = no_of_vely;
	peaks_to_save2 = peaks_to_save;
	
	m = 0;
    for( i=0; i< num_vel; i++) {
		for(n=0; n<peaks_to_save; n++) {
			xpeaks2[m] = xpeaks[m];
			ypeaks2[m] = ypeaks[m];
			m++;
		}
    }
	
	/* get file */

	if(!getpivdata(&cmnd[index])) return -1;
	
	if(no_of_vel2 == 0){ /* 1st file is a regular vector file */
		/* check that files are the same size */
	
		if( no_of_velx2 != no_of_velx || no_of_vely2 != no_of_vely || peaks_to_save2 != peaks_to_save ){
			printf("PIV files are not the same size\n");
			return -1;
		}
	}else { /* 1st file is a contour vector file */
		/* check that file to be subtracted is a regular grid vector file */
		
		if( no_of_velx < 0 && no_of_vely < 0){
			printf("Only regular grid vector files can be subtracted from contour vector files\n");
			return -1;
		}
		if( peaks_to_save2 != peaks_to_save){
			printf("PIV files do not have the same peaks_to_save\n");
			return -1;
		}
	}
	
	/* do arithmetic */
	
	switch(code) {
	
		case SUB:

				if(no_of_vel2 > 0) { /* find nearest vector to subtract. The subtracted file must
										be a regular grid vector file */
					m = 0;
					for( i=0; i< no_of_vel2; i++) { /* step thru contour velocities */

						x = xpeaks2[m] * (no_of_velx+1);
						x /= header[NCHAN];
						y = ypeaks2[m] * (no_of_vely+1);
						y /= header[NTRAK];
	
						nx = x - 0.5;
						ny = y - 0.5;
						num = (ny * no_of_velx) + nx;
						nump = num*peaks_to_save;
	
						dx = xpeaks[nump+1] - xpeaks[nump];
						dy = ypeaks[nump+1] - ypeaks[nump];

						m++;
						for(n=1; n<peaks_to_save; n++) {
							xpeaks2[m] -= dx;
							ypeaks2[m] -= dy;
							m++;
						}
					}
				
				
				}else {
					m = 0;
    				for( i=0; i< no_of_velx*no_of_vely; i++) {
						cent_x2 = xpeaks2[m];
						cent_y2 = ypeaks2[m];			
						cent_x = xpeaks[m];
						cent_y = ypeaks[m];
						xpeaks[m] = cent_x2;	/* save central peaks of original vector file in the result*/
						ypeaks[m] = cent_y2;
						m++;
						for(n=1; n<peaks_to_save; n++) {
							xpeaks[m] = xpeaks2[m] + cent_x - xpeaks[m];
							ypeaks[m] = ypeaks2[m] + cent_y - ypeaks[m];
							m++;
						}
   					 }
				}
				 break;
		case SUBC:							/* For subtracting from a conditional velocity field */

				if(no_of_vel2 > 0) { /* find nearest vector to subtract. The subtracted file must
										be a regular grid vector file */
					m = 0;
					for( i=0; i< no_of_vel2; i++) { /* step thru contour velocities */

						x = xpeaks2[m] * (no_of_velx+1);
						x /= header[NCHAN];
						y = ypeaks2[m] * (no_of_vely+1);
						y /= header[NTRAK];
	
						nx = x - 0.5;
						ny = y - 0.5;
						num = (ny * no_of_velx) + nx;
						nump = num*peaks_to_save;
	
						dx = xpeaks[nump+1] - xpeaks[nump];
						dy = ypeaks[nump+1] - ypeaks[nump];
						dx2 = xpeaks2[m] - xpeaks2[m+1];
						dy2 = ypeaks2[m] - ypeaks2[m+1];
						
						m++;
						for(n=1; n<peaks_to_save; n++) {
							if(dx2*dx2 + dy2*dy2 != 0.0){ 
								xpeaks2[m] -= dx;
								ypeaks2[m] -= dy;
							}
							m++;
						}
					}
				
				
				}else {

					m = 0;
    				for( i=0; i< no_of_velx*no_of_vely; i++) {
						cent_x = xpeaks[m];
						cent_y = ypeaks[m];
						xpeaks[m] = xpeaks2[m];	/* save central peaks of original vector file in the result*/
						ypeaks[m] = ypeaks2[m];
						dx2 = xpeaks2[m+1] - xpeaks2[m];
						dy2 = ypeaks2[m+1] - ypeaks2[m];
						m++;
						for(n=1; n<peaks_to_save; n++) {
							if(dx2*dx2 + dy2*dy2 != 0.0){ 
								xpeaks[m] = xpeaks2[m] + cent_x - xpeaks[m];
								ypeaks[m] = ypeaks2[m] + cent_y - ypeaks[m];
							}else {
								xpeaks[m] = xpeaks2[m];   /* No subtraction if original vector = 0.0 */
								ypeaks[m] = ypeaks2[m];
							}
							m++;
						}
   					 }
				 }
				 break;
		case ADD:
				m = 0;
    			for( i=0; i< no_of_velx*no_of_vely; i++) {
					cent_x2 = xpeaks2[m];
					cent_y2 = ypeaks2[m];			
					cent_x = xpeaks[m];
					cent_y = ypeaks[m];
					xpeaks[m] = cent_x2;	/* save central peaks of original vector file in the result*/
					ypeaks[m] = cent_y2;
					m++;
					dx = cent_x - xpeaks[m];
					dy = cent_y - ypeaks[m];
					/* If record flag set, increment counts for computing average */
					/* Don't count point in avg. if dx = dy = 0.0 */
					if((piv_cnt == 1) && (dx*dx + dy*dy != 0.0)){ 
					   	*(datpt + doffset + i) += 1;
					}
					for(n=1; n<peaks_to_save; n++) {
						xpeaks[m] = xpeaks2[m] - cent_x + xpeaks[m];
						ypeaks[m] = ypeaks2[m] - cent_y + ypeaks[m];
						m++;
					}
   				 }
				 have_max = 0;
       			 keylimit(-1);			/* disable printing */
				 maxx();
				 keylimit(-2);			/* reset printing  to previous mode*/
				 break;
	}
	
	if(no_of_vel2 > 0) { /* 1st file is contour. Need to switch arrays. */

		no_of_vel = no_of_vel2;
		no_of_velx = no_of_velx2;
		no_of_vely = no_of_vely2;

		/* allocate memory */
	
		if(xpeaks != 0) free(xpeaks);		/* get rid of subtracted file data */
		if(ypeaks != 0) free(ypeaks);
		xpeaks = (float *)malloc(no_of_vel * peaks_to_save * 4);
		ypeaks = (float *)malloc(no_of_vel * peaks_to_save * 4);
		if( xpeaks == 0 || ypeaks == 0) {
			nomemory();
			if(xpeaks != 0) free(xpeaks);
			if(ypeaks != 0) free(ypeaks);
			return -1;
		}

		/* reasign array values */
		m = 0;
	    for( i=0; i< no_of_vel; i++) {
			for(n=0; n<peaks_to_save; n++) {
				xpeaks[m] = xpeaks2[m];
				ypeaks[m] = ypeaks2[m];
				m++;
			}
   		}
	}

/*	free(xpeaks2);
	free(ypeaks2);*/
	return 0;

}

/* ***************** */
/* ************** */

int pairls()
/* List dx dy mag for pairs.  Points must first be generated 
	via the POINTS command and pairs generated via the PAIRS command.
*/
{
	int i,j,k;
		
	float dx,dy,mag;
	
	
	extern DATAWORD idat(int,int);
	
	if(npairs == 0) {
		beep();
		printf("No points defined.\n");
		return -1;
	}
	

	for(i=0; i<npairs; i+=2) {
		j = p_pairs[i];
		k = p_pairs[i+1];
		dx = fabs(xp_loc[j] - xp_loc[k]);
		dy = fabs(yp_loc[j] - yp_loc[k]);
		mag = sqrt(dx*dx + dy*dy);
		printf("%.2f\t%.2f\t%.2f\n",dx,dy,mag);
	}
		
	setarrow();
	return 0;
}

/* ************** */
/* *************************************************************** */

int cent(int n,int index)   /* select rectangle of size n x m about center of image */
{
	extern Point substart,subend;	/* these two points define a data subarray */
	extern int printxyzstuff();
	extern Variable user_variables[];
	
	int x0,y0,xf,yf,narg,m;
	
	narg = sscanf(&cmnd[index],"%d %d",&n,&m);

	if( n == 0) n=64;
	if(narg <2) {
		m = n;
	}
	if( m == 0) m=64;
	printf("%d by %d\n",n,m);
	x0 = (header[NCHAN] - n)/2;
	y0 = (header[NTRAK] - m)/2;
	xf = x0 + n - 1;
	yf = y0 + m - 1;
	
	substart.h = x0;
	substart.v = y0;
	subend.h = xf;
	subend.v = yf;
	
	user_variables[0].ivalue = substart.h;
	user_variables[0].is_float = 0;
	user_variables[1].ivalue = substart.v;
	user_variables[1].is_float = 0;
	user_variables[2].ivalue = subend.h;
	user_variables[2].is_float = 0;
	user_variables[3].ivalue = subend.v;
	user_variables[3].is_float = 0;
	
	
	printxyzstuff(x0,y0,0);
	printxyzstuff(xf,yf,1);
	

	printf("%d %d %d %d\n",substart.h,substart.v,subend.h,subend.v);
	return 0;
}

/* *************************************************************** */

int chngpix(int n,int index)   /* to change pixel values enter  chanel trak value */
{
	int narg,x,y,value;
	extern DATAWORD *datpt;
	
	narg = sscanf(&cmnd[index],"%d %d %d",&x,&y,&value);

	if(narg != 3) {
		beep();
		printf("Need 3 Arguments.\n"); 
		return -1;
	}
	
	*(datpt+x+y*header[NCHAN]+doffset) = value;
	
	have_max = 0;
    maxx();
    return 0;
}


/* *************************************************************** */

int makcirc()				/* draw cos(r) function */

{
	extern DATAWORD *datpt;
	int nc,nt;
	DATAWORD *datp;
	float radius,dist,x,y;
	
	datp = datpt+doffset;
	radius = 45.0;
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			x = nc-63;
			y = nt-63;
			dist = sqrt(y*y + x*x);
			if (dist < radius){
				*(datp++) = 100*cos(dist/radius*PI/2);
			}else{
				*(datp++) = 0;
			}
		}
	}
	have_max = 0;
    maxx();
    return 0;
}

/* *************************************************************** */

int makring()				/* draw ring with cos(r) function at some radius */

{
	extern DATAWORD *datpt;
	int nc,nt;
	DATAWORD *datp;
	float dist,x,y,radius,width;
	
	datp = datpt+doffset;
	radius = 45.0;
	width = 10.0;
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			x = nc-63;
			y = nt-63;
			dist = fabs(sqrt(y*y + x*x) - radius);
			if (dist <width){
				*(datp++) = 100*cos(dist/width*PI/2);
			}else{
				*(datp++) = 0;
			}
		}
	}
	have_max = 0;
    maxx();
    return 0;
}

/* *************************************************************** */

int pivrnd(int n,int index)
{
	int narg,k,pos;
	int x1,x2,y1,y2,dx,dy,value,num_pairs;
	
	narg = sscanf(&cmnd[index],"%d %d %d %d",&dx,&dy,&value,&num_pairs);

	if(narg != 4) {
		beep();
		printf("Need 4 Arguments.\n"); 
		return -1;
	}
	
	if(dx > header[NCHAN]-1 || dy > header[NTRAK]-1) {
		beep();
		printf("Displacement is too large.\n"); 
		return -1;
	}	
	
	k = 0;
	while (k<num_pairs) {
		pos = (float)rand()/(float)RAND_MAX*npts;
		x1 = pos % header[NCHAN];
		y1 = pos/header[NCHAN];
		x2 = x1 + dx;
		y2 = y1 + dy;
		if(x2 >= 0 && x2 < header[NCHAN] && y2 >= 0 && y2 < header[NTRAK]) {
			*(datpt+x1+y1*header[NCHAN]+doffset) = value;
			*(datpt+x2+y2*header[NCHAN]+doffset) = value;
			k++;
		}
	}

	have_max = 0;
    maxx();
    return 0;
}

/* ************************************************************ */


int pivrmv(int n,int index)				/* try to remove bad vectors -- those with deviation > given value
							   ->set bad vectors = 0.0, replace by local avg if requested*/
{
	
    extern char cmnd[];
	
	float vel_ave,vx_ave,vy_ave;
	float dmag,dang,da,dv,da1,dv1,deviation,neighbors,dot;
	float ulim_vx,llim_vx,ulim_vy,llim_vy,stdev_vx,stdev_vy,sum_y,sum_x,vx_mem[9],vy_mem[9];
	float maxdev = 0.75;   /* threshold for redoing average of neighboring vectors 
								to filter out bad ones from ave */
	int ptx,pty,i,j,criteria_met,replace;
	int numfixed = 0;
	int numbad = 0;
	int numbetter = 0;

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
			
			vel_ave = vx_ave = vy_ave = neighbors = 0.0;
			for( i= -1; i<=1; i++) {
				for( j= -1; j<=1; j++) {
					if ( i == 0 && j == 0 ) continue;					/* don't count this point */
					if ( ptx+j >= no_of_velx ) break;	/* don't run off the page */
					if ( ptx+j < 0 ) continue;
					if ( pty+i < 0 || pty+i >= no_of_vely ) break;
					if ( vx(ptx+j,pty+i,1) == 0.0 && vy(ptx+j,pty+i,1) == 0.0) continue;  /* don't include previously removed vectors in local avg */
					neighbors += 1.0;
					vel_ave += vel(ptx+j,pty+i,1);
					vx_ave += vx(ptx+j,pty+i,1);
					vy_ave += vy(ptx+j,pty+i,1);
					vx_mem[(int)neighbors-1] = vx(ptx+j,pty+i,1);
					vy_mem[(int)neighbors-1] = vy(ptx+j,pty+i,1);
				}
			}
			vel_ave /= neighbors;
			vx_ave /= neighbors;
			vy_ave /= neighbors;
			
			
			/* get statisitics on neighbors */
			sum_x = sum_y = 0.0;
			for( i= 0; i<neighbors; i++ ){
				sum_x += (vx_mem[i] - vx_ave)*(vx_mem[i] - vx_ave);
				sum_y += (vy_mem[i] - vy_ave)*(vy_mem[i] - vy_ave);
			}
			stdev_vx = sqrt( sum_x/(neighbors-1) );
			stdev_vy = sqrt( sum_y/(neighbors-1) );

			if (stdev_vx/vx_ave > maxdev || stdev_vy/vy_ave > maxdev){

				/* redo average excluding vectors that differ from the first pass average by more than +-stdev */
				ulim_vx = vx_ave + stdev_vx;
				llim_vx = vx_ave - stdev_vx;
				ulim_vy = vy_ave + stdev_vy;
				llim_vy = vy_ave - stdev_vy;
				vel_ave = vx_ave = vy_ave = neighbors = 0.0;

				for( i= -1; i<=1; i++) {
					for( j= -1; j<=1; j++) {
						if ( i == 0 && j == 0 ) continue;					/* don't count this point */
						if ( ptx+j >= no_of_velx ) break;	/* don't run off the page */
						if ( ptx+j < 0 ) continue;
						if ( pty+i < 0 || pty+i >= no_of_vely ) break;
						if ( vx(ptx+j,pty+i,1) == 0.0 && vy(ptx+j,pty+i,1) == 0.0) continue;  /* don't include previously removed vectors in local avg */
						if ( vx(ptx+j,pty+i,1) < llim_vx || vx(ptx+j,pty+i,1) > ulim_vx ) continue;
						if ( vy(ptx+j,pty+i,1) < llim_vy || vy(ptx+j,pty+i,1) > ulim_vy ) continue;
						neighbors += 1.0;
						vel_ave += vel(ptx+j,pty+i,1);
						vx_ave += vx(ptx+j,pty+i,1);
						vy_ave += vy(ptx+j,pty+i,1);
					}
				}
				vel_ave /= neighbors;
				vx_ave /= neighbors;
				vy_ave /= neighbors;
			 }
			 
			dot = vx_ave*vx(ptx,pty,1) + vy_ave*vy(ptx,pty,1);
			da = fabs(acos(dot / (vel_ave*vel(ptx,pty,1))));
			dv = fabs((vel(ptx,pty,1)-vel_ave) )/ vel_ave;
			
			deviation = da + dv;	/* this expresses how far off things are in a single number */
			
			if( da > dang || dv > dmag ) {
				/* deviates too much, try other peaks */
		
				numbad++;
				criteria_met = 0;
				
				for( i = 2; i < peaks_to_save; i++ ) {
					/* piv_reorder( ptx + pty*no_of_velx + 1 ); */
					
					dot = vx_ave*vx(ptx,pty,i) + vy_ave*vy(ptx,pty,i);
					da1 = fabs(acos(dot / (vel_ave*vel(ptx,pty,i))));
					dv1 = fabs((vel(ptx,pty,i)-vel_ave) )/ vel_ave;

					if( da1 < dang && dv1 < dmag ) {
						/* have found one that fits the criterion, do the switch */
						j = (ptx + pty*no_of_velx)*peaks_to_save;
						swap_peaks(j+1,j+i);
						criteria_met = 1;
						numfixed++;
						break;
					}
				}
				if ( !criteria_met ) {							/* set velocity = 0.0 */
					j = (ptx + pty*no_of_velx)*peaks_to_save;
					xpeaks[j+1] = xpeaks[j];
					ypeaks[j+1] = ypeaks[j];
				}

			}			
		}
	}
	
	
	if( replace ) {
	/* go through all the velocity vectors and replace the ones that are set to 0.0 with the local avg */
		numbetter = 0;
		
		for(pty = 0; pty < no_of_vely; pty++ ){
			for(ptx = 0; ptx < no_of_velx; ptx++ ){
			
				if ( vel(ptx,pty,1) == 0.0 )  {
					/* get the average velocity in this region excluding this center point */
			
					vel_ave = vx_ave = vy_ave = neighbors = 0.0;
					for( i= -1; i<=1; i++) {
						for( j= -1; j<=1; j++) {
							if ( i == 0 && j == 0 ) continue;					/* don't count this point */
							if ( ptx+j >= no_of_velx ) break;	/* don't run off the page */
							if ( ptx+j < 0 ) continue;
							if ( pty+i < 0 || pty+i >= no_of_vely ) break;
							if ( vx(ptx+j,pty+i,1) == 0.0 && vy(ptx+j,pty+i,1) == 0.0) continue;  /* don't include previously removed vectors in local avg */
							neighbors += 1.0;
							vel_ave += vel(ptx+j,pty+i,1);
							vx_ave += vx(ptx+j,pty+i,1);
							vy_ave += vy(ptx+j,pty+i,1);
							vx_mem[(int)neighbors-1] = vx(ptx+j,pty+i,1);
							vy_mem[(int)neighbors-1] = vy(ptx+j,pty+i,1);
						}
					}
					if(neighbors != 0.0){
						vel_ave /= neighbors;
						vx_ave /= neighbors;
						vy_ave /= neighbors;
				
						j = (ptx + pty*no_of_velx)*peaks_to_save;
						xpeaks[j+1] = xpeaks[j] + vx_ave;
						ypeaks[j+1] = ypeaks[j] + vy_ave;
						numbetter++;
					}
				}
			}
		}
	}
	
	pprintf("%d bad vectors.\n%d good vectors found.\n",numbad,numfixed);
	if(replace)
		pprintf("%d interpolated.\n",numbetter);
	setarrow();	
	return 0;
}

/* ************************************************************ */

/* ************************************************************ */

int pivx(int n)	/* extract x-component */
{
		
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern float* ypeaks;
	
	int i,m,j;

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}

	m = 0;
	for( i=0; i< no_of_velx*no_of_vely; i++) {
		j = m;
		m++;
		for(n=1; n<peaks_to_save; n++) {
			ypeaks[m] = ypeaks[j];
			m++;
		}
    }
    return 0;
}

/* *********************************************************** */

int pivy(int n)	/* extract y-component */
{
		
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	extern int peaks_to_save;		/* the number of peaks to save */
	extern float* xpeaks;
	
	
	int i,m,j;

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}

	m = 0;
	for( i=0; i< no_of_velx*no_of_vely; i++) {
		j = m;
		m++;
		for(n=1; n<peaks_to_save; n++) {
			xpeaks[m] = xpeaks[j];
			m++;
		}
    }
    return 0;
}

/* ************************************************************ */

int vecimg(int n)	/* record vector magnitudes scaled by n in image buffer */
{
	extern char cmnd[];	
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	
	
	int ptx,m,pty;

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	if(n==0) n = 1;
	
	/* Set up image buffer */
	keylimit(-1);			/* disable printing */
	multiply(0,0);
	sprintf(cmnd,"%d %d",no_of_velx,no_of_vely);
	maknew(1,0);				/* make image same dimensions as vector field */
	keylimit(-2);			/* reset printing  to previous mode*/

	m = 0;
	for(pty = 0; pty < no_of_vely; pty++ ){
		for(ptx = 0; ptx < no_of_velx; ptx++ ){
			*(datpt+doffset+m) = n*vel(ptx,pty,1);
			m++;
		}
	}	
	have_max = 0;
	maxx();
	return 0;
}

/* ************************************************************ */
int vecimx(int n)	/* record x-component of velocity (scaled by n) in image buffer */
{
	extern char cmnd[];	
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	
	
	int ptx,m,pty;

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	if(n==0) n = 1;
	
	/* Set up image buffer */
	keylimit(-1);			/* disable printing */
	multiply(0,0);
	sprintf(cmnd,"%d %d",no_of_velx,no_of_vely);
	maknew(1,0);				/* make image same dimensions as vector field */
	keylimit(-2);			/* reset printing  to previous mode*/

	m = 0;
	for(pty = 0; pty < no_of_vely; pty++ ){
		for(ptx = 0; ptx < no_of_velx; ptx++ ){
			*(datpt+doffset+m) = n*vx(ptx,pty,1);
			m++;
		}
	}	
	have_max = 0;
	maxx();
	return 0;
}

/* ************************************************************ */
int vecimy(int n)	/* record y-component of velocity (scaled by n) in image buffer */
{
	extern char cmnd[];	
	extern int	no_of_velx;			/* the number of velocities calculated in the x and y directions */
	extern int	no_of_vely;
	
	
	int ptx,m,pty;

	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return -1;
	}
	if(n==0) n = 1;
	
	/* Set up image buffer */
	keylimit(-1);			/* disable printing */
	multiply(0,0);
	sprintf(cmnd,"%d %d",no_of_velx,no_of_vely);
	maknew(1,0);				/* make image same dimensions as vector field */
	keylimit(-2);			/* reset printing  to previous mode*/

	m = 0;
	for(pty = 0; pty < no_of_vely; pty++ ){
		for(ptx = 0; ptx < no_of_velx; ptx++ ){
			*(datpt+doffset+m) = n*vy(ptx,pty,1);
			m++;
		}
	}	
	have_max = 0;
	maxx();
	return 0;
}

 /* ************************************************************ */
extern int	histo[];

int myhist(int n,int index)
{ /* Calculate histogram using data in buffer. There are always 256
	  bins. The binsize is determined by the specified min,max.  */

	extern DATAWORD *datpt, min, max;
	
	int i,bin,hmin,hmax,low,hi;
	float binsize;
	
/*	if(hclear)  */
		for(i=0; i<=255; i++) histo[i] = 0;
		
	if( sscanf(&cmnd[index],"%d %d",&hmin,&hmax) == 2) {
		if( min >= max){
			printf("Bad range specified. Argument is: min max\n");
			return -1;
		}
	} else{
		have_max = 0; /* make sure we have the correct max and min values */
		maxx();
		hmin = min;
		hmax = max;
	}
	
	binsize = ((float)hmax - (float)hmin)/(float)HISTOSIZE;
    printf("binsize = %f\n",binsize);
	for ( i=0; i< npts;i++) {
		bin = ( (float)(*(datpt+i+doffset)) - (float)hmin )/ binsize;
		if(bin < 0) 
			bin = 0;
		if(bin > HISTOSIZE-1)
			bin = HISTOSIZE-1;
		histo[bin]++;
    }
	
	for( i=0; i<HISTOSIZE; i++) {
		low = i*binsize+hmin;
		hi = (i+1)*binsize+hmin;
		printf("%d to %d\t%d\n",low,hi,histo[i]);
	}

	return 0;
}

 /* ************************************************************ */
/* ************************************************************ */

 /* ************************************************************ */

 /* ************************************************************ */

int smooga(int n,int index)
			/* smooth the data with a Gaussian kernel */
{
	
	DATAWORD *datp,*datp2;
    extern DATAWORD *datpt;
	extern int	doffset;
	DATAWORD idat(int,int);
	
	float *kern;
	float sig,sum,count;
	int dx,dy,i,j,m,size,nt,nc;
	int kx,ky,ksize;
	
	if(n <= 0) n = 1;
	
	dx = dy = n;	
	kx = ky = 2*n+1;	/* the smoothing kernel is 2n+1 x 2n+1 pixels */
	sig = dx/1.75;		/* stdev of gaussian */
	ksize = kx*ky;
	
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */

	datp2 = datp = (DATAWORD*)malloc(size);
	kern = (float *)malloc(ksize*4);
	if(datp == 0 || kern == 0) {
		nomemory();
		if(kern != 0) free(kern);
		return -1;
	}

	m = 0;
	for(i=-dx;i<=dx;i++){   /* setup kernel */
		for(j=-dy;j<=dy;j++){
			kern[m] = exp( -(i*i+j*j)/(2.0*sig*sig) )/( sig*sqrt(2.0*PI) );
			/* printf("%d %0.3f\n",m,kern[m]); */
			m++;		
		}
	}

	
	for(nc=0; nc<doffset; nc++) 
		*(datp++) = *(datpt+nc);	/* copy the CCD header */
	for(nt=0; nt<header[NTRAK]; nt++) {
		
		for(nc=0; nc<header[NCHAN];nc++){
			sum = 0;
			count = 0;
			m = 0;
			for(i=-dx; i<=dx; i++) {
				for(j=-dy; j<=dy; j++) {
					if( (nt+j) < header[NTRAK] && 
					    (nc+i) < header[NCHAN] &&
					    (nt+j) >= 0 && (nc+i) >= 0) {
						count += kern[m];
						sum += (float)idat(nt+j,nc+i)*kern[m];
					}
					m++;
				}
			}

			*(datp++) = sum/count;
		}
	}
	free(kern);
	free(datpt);
	datpt = datp2;
	have_max = 0;
	setarrow();	
	return 0;
}



