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
#define HORSIZE	1024		/* size of the horizon */
#define NPENS	16		/* the number of pens for color drawing */
#define FRACT	0.25	/* the skew factor in surface plots -- 25% of width */

#include "impdefs.h"
#include "oma_quartz.h"

extern	TWOBYTE		header[];
extern	char		txt[];		/* a general purpose text array */

extern OMA_Window	oma_wind[];

/* globals for the graphing functions */

extern	float		xmin,xmax,ymin,ymax,xscalerange,scalex,yscalerange,scaley;
extern	float		ix,iy;


extern	RGBColor	RGBBlack,RGBWhite;

int		sdatminmax = true;
int		sstdscrnsize = false;
int		scolor = true;
DATAWORD		surfmax = 0;
DATAWORD		surfmin = 0;
int 	surfwidth = 400;
int 	surfheight = 400;
int		plotwhite = false;	/* the white/black background business */
int		incrementby = 1;
int		persp = -1;			/* perspective indicator; -1 for standard, +1 for perspective*/
float	fract = 0.25;		/* tilt fraction */
float	fracty = .67;		/* height of view */
float	de,ymn,ymx;
float	horiz[HORSIZE];
int			idf;
float lastx,lasty;

extern CGContextRef	lgContext;
extern CGContextRef	lgpdfContext;

int printf(),openwindow_q(),chooseposn();

int ploton(int whitebgnd,int height,int width,int incolor,char* name)
{

	extern	int			gwnum;
	CGContextRef SetupPDFcontext(int);
	void set_window_type(int,int);
	
	if (height < 10) height = 10;
	if (width < 10) width = 10;

	scalex = width;
	scaley = height;
	
	chooseposn(width,height,1);
	//oma_wind[gwnum].windowtype = QLINEGRAPH;
	set_window_type(gwnum,QLINEGRAPH);
	if(!openwindow_q(width,height,name,kWindowStandardFloatingAttributes)) return 0;
	
	lgpdfContext = SetupPDFcontext(gwnum);

	HIViewRef   contentView;
	CGRect dstRect;
	HIViewFindByID(HIViewGetRoot(oma_wind[gwnum].gwind), kHIViewWindowContentID, &contentView);
	HIViewGetBounds(contentView, &dstRect);

	QDBeginCGContext(GetWindowPort(oma_wind[gwnum].gwind), &lgContext);
	CGContextSetLineWidth (lgContext, 1);
	CGContextSetLineJoin(lgContext,kCGLineJoinRound);
	CGContextSetLineCap(lgContext,kCGLineCapRound);

	if(plotwhite)
		CGContextSetRGBFillColor (lgContext, 1, 1, 1, 1);
	else
		CGContextSetRGBFillColor (lgContext, 0, 0, 0, 1);
	CGContextSetRGBStrokeColor (lgContext, 0, 0, 0, 1);
	CGContextSelectFont (lgContext,"Helvetica",10,kCGEncodingMacRoman);
	CGContextSetTextDrawingMode(lgContext,kCGTextFillStroke);
	CGContextFillRect (lgContext,dstRect);
	CGContextStrokeRect(lgContext,dstRect);
	
	CGContextBeginPath (lgContext);					

	CGContextSetLineWidth (lgpdfContext, 1);
	CGContextSetLineJoin(lgpdfContext,kCGLineJoinRound);
	CGContextSetLineCap(lgpdfContext,kCGLineCapRound);

	if(plotwhite)
		CGContextSetRGBFillColor (lgpdfContext, 1, 1, 1, 1);
	else
		CGContextSetRGBFillColor (lgpdfContext, 0, 0, 0, 1);
	CGContextSetRGBStrokeColor (lgpdfContext, 0, 0, 0, 1);
	CGContextSelectFont (lgpdfContext,"Helvetica",10,kCGEncodingMacRoman);
	CGContextSetTextDrawingMode(lgpdfContext,kCGTextFillStroke);
	CGContextFillRect (lgpdfContext,dstRect);
	CGContextStrokeRect(lgpdfContext,dstRect);
	CGContextBeginPath (lgpdfContext);				
	
	return(true);	
}

int reset()
{
	extern	int			gwnum;
	extern 	short			orgx,swidth;
	
	CGContextStrokePath( lgContext );
	CGContextFlush(lgContext);
	QDEndCGContext (GetWindowPort(oma_wind[gwnum].gwind), &lgContext);
	
	CGContextStrokePath( lgpdfContext );
	CGContextEndPage(lgpdfContext);
	CGContextRelease(lgpdfContext);
	
	gwnum++;
	orgx += swidth;
	return 0;
}



void moveto(float x, float y)
{
lastx = x;
lasty = y;
CGContextMoveToPoint(lgContext,lastx,lasty);
CGContextMoveToPoint(lgpdfContext,lastx,lasty);
}

void lineto(float x, float y)
{
lastx = x;
lasty = y;
CGContextAddLineToPoint(lgContext,lastx,lasty);
CGContextAddLineToPoint(lgpdfContext,lastx,lasty);
}

int newpen_q(CGContextRef ignoredContext,int l)
{
	
	extern RGBColor color[256][8];
	extern int      thepalette;
	float r,g,b;
	r = color[l][thepalette].red;
	g = color[l][thepalette].green;
	b = color[l][thepalette].blue;

	CGContextStrokePath( lgContext );
	CGContextStrokePath( lgpdfContext );
	//CGContextFlush(lgContext);
	CGContextSetRGBStrokeColor (lgContext, r/256/256,  g/256/256, b/256/256, 1);
	CGContextSetRGBStrokeColor (lgpdfContext, r/256/256,  g/256/256, b/256/256, 1);
	CGContextMoveToPoint(lgContext,lastx,lasty);
	CGContextMoveToPoint(lgpdfContext,lastx,lasty);

	return 0;
}

int surface(char* name)
{

	int 		i,j,pen,oldpen=0,incre;
	DATAWORD 	idat(int,int);
	float		x,rp,elev,dx,dy,pr,cdx;
	float		xmx,xmn,clrscalefact;
	float		rp0=0,rp1,rps,xs;
	int extra = 0;
	
	extern	DATAWORD 	idat(int,int);	/* from main OMA code -- min and max of data*/
	extern	TWOBYTE		header[];
	extern	DATAWORD		cmin,cmax;
	
	extern	int			gwnum;
	extern short	orgx;
	extern short	swidth;
	
	CGContextRef SetupPDFcontext(int);
	
	idf = 1;

	for(i=0; i<HORSIZE; i++) horiz[i] = -1e30;
	clrscalefact = NPENS;
	clrscalefact /= (cmax-cmin);
		
	setscreensize();

	dx = fract*header[NCHAN]/header[NTRAK] * incrementby;
	pr = persp;
	dy = (1.0 + pr)/2.0*fract*(ymx-ymn)/header[NTRAK] * incrementby;
	cdx = dx / incrementby;
	elev = 0.0;
	xmn = 0.0;
	xmx = header[NCHAN] - fract * header[NCHAN] * (pr - 1.0) / 2.0;
	
	if(!ploton(plotwhite,surfheight,surfwidth,scolor,name)) return -1;
	
	for(i=header[NTRAK] - 1; i >= 0;i -= incrementby) {
		scalx(xmn,xmx);
		scaly(ymn,(ymx-ymn)*6.0);
		for(j=0; j < header[NCHAN]; j++) {
			x = j;
			rp =  idat(i,j);
			if(!scolor) 
				goto just_plot;
			pen = (rp - cmin) * clrscalefact;
			if(pen>NPENS) pen = NPENS;						
			if(pen<0) pen = 0;
			
			if( j == 0) {
				 newpen_q(lgContext,pen*NCOLOR/NPENS);
				oldpen = pen;
			}
						
			/* pen switch stuff with extra points   */

			if(pen != oldpen) {
				
				extra = abs(oldpen - pen);		/* the number of extra lines */
				rps = (rp - rp0) / extra;
				xs = 1.0 / extra;
				rp1 = rp0;
				x -= 1.0;
				if(oldpen > pen)
					incre = -1;
				else
					incre = 1;
					
				for (; extra>0; extra--){
					oldpen += incre;
					rp1 += rps;
					x += xs ;
					 newpen_q(lgContext,oldpen*NCOLOR/NPENS);
					vect(x,rp1+elev);
				}
				rp0 = rp;
			}
			else {
				rp0 = rp;
just_plot:
				rp += elev;
				if(j==0){
					iset(x,rp);}
				else{
					vect(x,rp);}
			}
		}
		xmn -= dx;
		xmx += dx*pr;
		elev += de;
		ymx += dy;
		ymn -= dy;
		//ReshapeCustomWindow(oma_wind[gwnum].gwind);
	}
	reset();

	return 0;
}

int vect(float x,float y)
{
	float 	dy,xxn,yyn;
	float 	ixn,iyn,idx,ixp=0,iyp=0;
	int		j,iix;
	extern	int		idf;
	
	xxn = (x-xmin)*xscalerange;
	yyn = (y-ymin)*yscalerange;
	ixn = xxn;
	iyn = yyn;
	idx = ixn - ix;
	dy = iyn-iy;
	if(idx != 0) dy = (float)(iyn - iy)/(float)idx;
	iix = ix;
	if(iy >= horiz[iix]) 
		goto pointabove;
	iix = ixn;
	if(iyn < horiz[iix]) {
		moveto(ixn,iyn);
		done2:
		ix = ixn;
		iy = iyn;
		if((idx == 0) && (dy < 0.0) && (idf == 1) ) {
			lineto( ixn, iyn );
		} else
			idf = 0;
		return 0;
	}
	while(true) {					/* ftn label 300 -- pt 1 below horiz, pt 2 at or above */
		for( j=0; j<= idx; j++) {
			ixp = ix+j;
			iyp = iy + j*dy;
			iix = ixp;
			if(horiz[iix] <= iyp) {
				break;
			}
		}
		iix = ixp;
		if(horiz[iix] > iyp) goto done2;
		moveto(ixp,iyp);
		ix = ixp;
		iy = iyp;
		idx -= j;
	
pointabove:							/* ftn label 200  pt 1 is at or above horizon */
		if(idx == 0) {
			ixp = ix;
			iyp = iy;
		}

		for(j=0; j<idx; j++) {
			ixp = ix+j;
			iyp = iy + j*dy;
			iix = ixp;
			if(horiz[iix] > iyp) 
				break;
			iix = ixp;
			horiz[iix] = iyp;
		}
		lineto( ixp,iyp );
		idf = 1;

		if(ixp >= ixn-1) 
			goto done2;
		ix = ixp;
		iy = iyp;
		idx = ixn - ixp;
		dy = (float)(iyn-iyp)/ (float)idx;
	}
	return 0;
}

int iset(float x,float y)
{
	ix = (x-xmin)*xscalerange;
	iy = (y-ymin)*yscalerange;
	moveto(ix,iy);	
	return 0;
}

