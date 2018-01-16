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

extern	TWOBYTE		header[];
extern	char		txt[];		/* a general purpose text array */

// globals for the graphing functions

extern	float		xmin,xmax,ymin,ymax,xscalerange,scalex,yscalerange,scaley;
extern	float		ix,iy;
extern	RGBColor	RGBBlack,RGBWhite;

extern OMA_Window	oma_wind[];
extern	int			gwnum;
extern short	orgx;
extern short	swidth;


//

int		pdatminmax = true;
int		pstdscrnsize = true;
DATAWORD		ponemax = 256;
DATAWORD		ponemin = 0;
int 	ponewidth = 300;
int 	poneheight = 256;
int		poneplotwhite = 1;	/* the white/black background business */
int		pintegrate = true;
int		pintx = true;
extern	CGContextRef	lgContext;

int printf(),pprintf(),chooseposn(),gets(),openwindow_q();
void moveto(float, float);
void lineto(float, float);

int pset(float x,float y)
{
	ix = (x-xmin)*xscalerange;
	iy = (y-ymin)*yscalerange;
	moveto(ix,iy); 
	return 0;
}

int pdvect(float x,float y)
{
	ix = (x-xmin)*xscalerange;
	iy = (y-ymin)*yscalerange;
	lineto(ix,iy); 
	return 0;
}

// plot integrated

int plotone(char* name)
{

	short 		i,j;
	DATAWORD 	idat(int,int);
	float		x,rp;
	float		xmx,xmn,ymx,ymn;
	DATAWORD	sum,pxstart,pxend,pystart,pyend;

	
	extern	DATAWORD 	min,max,idat(int,int);	// from main OMA code -- min and max of data
	extern	TWOBYTE		header[];
	extern	short		pixsiz;
	extern	Point		substart,subend;

	poneplotwhite = 1;

	if(pdatminmax) {
		ymn = min;
		ymx = max;  
		if(min == max)  ymx += 1.0;  }
	else {
		ymn = ponemin;
		ymx = ponemax;
	}
	
	if(pintx) {
	
		if( pintegrate && (substart.h == subend.h) ) {
			SysBeep(1);		
			pintegrate = false;
		}
		if(pintegrate) {
			xmn = pxstart = substart.h;
			xmx = pxend = subend.h;
			pystart = substart.v;
			pyend = subend.v;

			if( pxstart < 0 ) 
				pxstart = 0;			
			if( pxend > header[NCHAN]) 
				pxend = header[NCHAN];
			if( pystart < 0 ) 
				pystart = 0;
			if( pyend > header[NTRAK]) 
				pyend = header[NTRAK];

			ymx *= (pyend - pystart +1);
			ymn *= (pyend - pystart +1);

		}
		else {
			xmn = pxstart = 0;
			xmx = pxend = header[NCHAN];
			pystart = pyend = header[NTRAK]/2;
		}
		
		if(pstdscrnsize) {
			if( pixsiz > 0 ) {
				poneheight = pixsiz * header[NTRAK];
				ponewidth = pixsiz * (pxend-pxstart); }
			else {
				poneheight = header[NTRAK]/abs(pixsiz);
				ponewidth = (pxend-pxstart)/abs(pixsiz);
			}
		}		


		if(!ploton(poneplotwhite,poneheight,ponewidth,true,"Linegraphics")){
			beep();
			printf("Problem opening\n");
			return -1;
		} 
		
		scalx(xmn,xmx);
		scaly(ymn,ymx);

	
		for(j=pxstart; j < pxend; j++) {
			sum = 0;
			for(i=pystart; i <=pyend; i++) {
				sum += idat(i,j);
			}
			x = j;
			rp = sum;
			if(j==pxstart){
				pset(x,rp);}
			else{
				pdvect(x,rp);}
		}
		reset();
		return 0;
	}
	else {
	
			if( pintegrate && (substart.v == subend.v) ) {
			SysBeep(1);		
			pintegrate = false;
		}
		if(pintegrate) {
			xmn = pxstart = substart.v;
			xmx = pxend = subend.v;
			pystart = substart.h;
			pyend = subend.h;

			if( pxstart < 0 ) 
				pxstart = 0;			
			if( pxend > header[NTRAK]) 
				pxend = header[NTRAK];
			if( pystart < 0 ) 
				pystart = 0;
			if( pyend > header[NCHAN]) 
				pyend = header[NCHAN];


			ymx *= (pyend - pystart +1);
			ymn *= (pyend - pystart +1);
		}
		else {
			xmn = pxstart = 0;
			xmx = pxend = header[NTRAK];
			pystart = pyend = header[NCHAN]/2;
		}
		
		if(pstdscrnsize) {
			if( pixsiz > 0 ) {
				poneheight = pixsiz * header[NCHAN];
				ponewidth = pixsiz * (pxend-pxstart); }
			else {
				poneheight = header[NCHAN]/abs(pixsiz);
				ponewidth = (pxend-pxstart)/abs(pixsiz);
			}
		}		

		if(!ploton(poneplotwhite,poneheight,ponewidth,true,"Linegraphics")){
			beep();
			printf("Problem opening\n");
			return -1;
		}
		
		scalx(xmn,xmx);
		scaly(ymn,ymx);
	
		for(j=pxstart; j < pxend; j++) {
			sum = 0;
			for(i=pystart; i <=pyend; i++) {
				sum += idat(j,i);
			}

			x = j;
			rp = sum;
			if(j==pxstart){
				pset(x,rp);}
			else{
				pdvect(x,rp);}
		}
		reset();
		
	}
	return 0;
}


/* ___________________________ */

// the plot line tool in the status bar

int do_line_plot(Point *start,Point *end)
{

	short 		ic,it;
	DATAWORD 	idat(int,int);
	float		x,rp,xinc,yinc;
	float		xmx,xmn,ymx,ymn;
	int			dx,dy,length;

	extern	DATAWORD 	min,max,idat(int,int);	/* from main OMA code -- min and max of data*/
	extern	TWOBYTE		header[];
	extern	short		pixsiz;

	dy = end->v - start->v;
	dx = end->h - start->h;
	
	xmn = 0;
	length = xmx = sqrt((float) (dx*dx + dy*dy));
	
	xinc = dx/xmx;
	yinc = dy/xmx;
	
	if(pdatminmax) {
		ymn = min;
		ymx = max;  
		if(min == max)  ymx += 1.0;  }
	else {
		ymn = ponemin;
		ymx = ponemax;
	}
		
	if(pstdscrnsize) {
			if( pixsiz > 0 ) {
				poneheight = pixsiz * header[NTRAK];
				ponewidth = pixsiz * length; }
			else {
				poneheight = header[NTRAK]/abs(pixsiz);
				ponewidth = length/abs(pixsiz);
			}
	}


	if(!ploton(poneplotwhite,poneheight,ponewidth,true,"Plot Tool")) return -1;

	scalx(xmn,xmx);
	scaly(ymn,ymx);

	for(x = 0.0; x <= xmx; x += 1.0) {
		ic = start->h + x*xinc;
		it = start->v + x*yinc;
		rp = idat(it,ic);
		if(x == 0.0){
			pset(x,rp);}
		else{
			pdvect(x,rp);}
	}
	reset();

	return 0;
}
