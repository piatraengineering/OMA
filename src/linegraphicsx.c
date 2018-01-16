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
extern	char			txt[];		/* a general purpose text array */
extern RGBColor	RGBBlack,RGBWhite;

extern OMA_Window	oma_wind[];

extern int			ruler_scale_defined;
extern char			unit_text[UNIT_NAME_LENGTH];
extern float		ruler_scale;

//extern 	GWorldPtr	myGWorldPtrs[]; // offscreen maps

// globals for the graphing functions 

float		xmin,xmax,ymin,ymax,xscalerange,scalex,yscalerange,scaley;
float		ix,iy;

int 	lgwidth = 200;
int 	lgheight = 200;
int	nlevls = 3;
DATAWORD	ctrmax = 0;
DATAWORD	ctrmin = 0;
float	clevls[MAXNOCTRS] = {0.25,.5,0.75,.5,.5,.5,.5,.5,.5,.5};
float clengths[MAXNOCTRS];
float truclevls[MAXNOCTRS];
int		datminmax = true;
int		stdscrnsize = true;
int		colorctrs = true;
int		noctrtyping = true;

int		inwhite = true;

CGContextRef	lgContext;
CGContextRef	lgpdfContext;

int drawing_in_open_window = false;

int printf(),pprintf(),chooseposn(),gets(),openwindow_q();
void moveto(float, float);
void lineto(float, float);
int newpen_q(CGContextRef,int);

int set(float x,float y)
{
	ix = (x-xmin)*xscalerange;
	iy = (y-ymin)*yscalerange;
	iy = lgheight-iy;
	moveto(ix,iy); 
	return 0;
}

int dvect(float x,float y)
{
	ix = (x-xmin)*xscalerange;
	iy = (y-ymin)*yscalerange;
	iy = lgheight-iy;
	lineto(ix,iy); 
	return 0;
}
/*
int newpen(int l)
{
	extern RGBColor	thecolor[];

	RGBForeColor(&thecolor[l]);
	return 0;
}
*/


int contour(char* name)
{
	//static short table[3][3][3]={0,0,8,0,2,5,7,6,9,0,3,4,1,3,1,4,3,0,9,6,7,5,2,0,8,0,0};
	static short table[3][3][3]={{{0,0,8},{0,2,5},{7,6,9}},{{0,3,4},{1,3,1},{4,3,0}},{{9,6,7},{5,2,0},{8,0,0}}};
	
	
	short nt,nc,l,m,isignz[5],icase,m1,m2,m3;
	DATAWORD	imin,imax,iz;
	DATAWORD idat(int,int);
	float		smx;
	int		ctrclr[MAXNOCTRS];


	extern	DATAWORD min,max;		/* from main OMA code -- min and max of data*/
	extern	TWOBYTE		header[];
	extern	short			pixsiz;
	extern	DATAWORD	cmin,cmax;

	//extern	int			gwnum;
	//extern short	orgx;
	//extern short	swidth;

	

	float dx[5];				/*  = {0.,1.,1.,0.,0.5};*/
	float dy[5];				/*  = {0.,0.,1.,1.,0.5};*/
	float dz[5],x1=0,x2=0,y1=0,y2=0;
	
	dx[0] = dx[3] = dy[0] = dy[1] = 0.;
	dx[1] = dx[2] = dy[2] = dy[3] = 1.0;
	dx[4] = dy[4] = 0.5;
	
	if(datminmax) {
		ctrmin = min;
		ctrmax = max;
	}
	if(stdscrnsize) {
		if( pixsiz > 0 ) {
			lgwidth = pixsiz * header[NCHAN];
			lgheight = pixsiz * header[NTRAK]; }
		else {
			lgwidth = header[NCHAN]/abs(pixsiz);
			lgheight = header[NTRAK]/abs(pixsiz);
		}

	}
	
	if(!ploton(inwhite,lgheight,lgwidth,colorctrs,"Contours")) return -1;	
	
	for(l=0; l<nlevls; l++) {
		truclevls[l] = clevls[l]*(ctrmax-ctrmin) + ctrmin;
		clengths[l] = 0.0;
		if(colorctrs) {
			m = (truclevls[l] - cmin) * NCOLOR/(cmax-cmin);
			if(m > NCOLOR) m = NCOLOR;
			if(m < 0) m = 0;
			ctrclr[l] = m;
		}
	}

	smx = header[NCHAN];
	scalx(0.0,smx);
	smx = header[NTRAK];
	scaly(0.0,smx);
    
    for(l=0; l< nlevls; l++) {            
	if(colorctrs) newpen_q(lgContext,ctrclr[l]) ;     /* Pen color */
	iz=truclevls[l] ;         
	
	
	/****** The meat of it *****************************************************/
	for( nt=header[NTRAK]-2; nt>=0; nt--) {
	    for(nc=0; nc<header[NCHAN]-1;nc++) {
		/*        Check values at corners of grid box */
		
		imin = idat(nt,nc);
		if (idat(nt,nc+1) < imin) imin=idat(nt,nc+1);		
		if (idat(nt+1,nc) <imin)  imin=idat(nt+1,nc);
		if (idat(nt+1,nc+1)<imin) imin=idat(nt+1,nc+1);
		imax=idat(nt,nc);
		if (idat(nt,nc+1) > imax)   imax=idat(nt,nc+1);
		if (idat(nt+1,nc) > imax)   imax=idat(nt+1,nc);
		if (idat(nt+1,nc+1) > imax) imax=idat(nt+1,nc+1);
		
		/*       If values not in right range, move to next box */
		if ((imax < truclevls[l]) || (imin > truclevls[l]))  continue;
		
		if (iz < imin) continue;
		if (iz > imax) continue;
		
		/*If box OK, draw where internal 
		 ! triangles cut contour z planes 
		 !    3 *********** 2
		 !    * *         * *
		 !    *   *     *   *
		 !    *      4      *
		 !    *   *     *   *
		 !    * *         * *	
		 !    0 *********** 1		*/
		dz[0]=idat(nt,nc)-iz;        
		dz[1]=idat(nt,nc+1)-iz;      
		dz[2]=idat(nt+1,nc+1)-iz;   
		dz[3]=idat(nt+1,nc)-iz;
		dz[4]=(dz[1]+dz[2]+dz[3]+dz[0])/4.; 
		for(m=0; m<5; m++)
		/*Are points below/above z plane?*/              
		    isignz[m]=(dz[m]<0) ? 0 :((dz[m]>0) ? 2 : 1);
		
		for(m=0; m<4; m++){            /*! Look at each triangle in turn*/
		    m1=m;
		    m2=4;
		    m3=m+1;
		    if (m3==4) m3=0;
		    /*             Lookup instructions: */
		    icase=table[isignz[m3]][isignz[m2]][isignz[m1]];
		    switch(icase) {
			case 1:
			    x1=nc+dx[m1];          /*!  Link 1,2 */
			    y1=nt+dy[m1];
			    x2=nc+dx[m2];
			    y2=nt+dy[m2];
			    break;
			case 2:
			    x1=nc+dx[m2];        	/*! Link 2,3  */
			    y1=nt+dy[m2];
			    x2=nc+dx[m3];
			    y2=nt+dy[m3];
			    break;	
			case 3:
			    x1=nc+dx[m3];           /*! Link 3,1  */
			    y1=nt+dy[m3];
			    x2=nc+dx[m1];
			    y2=nt+dy[m1];
			    break;	
			case 4:
			    x1=nc+dx[m1];        	/*! Link 1, side 2-3  */
			    y1=nt+dy[m1];
			    x2=(dz[m3]*(nc+dx[m2])-dz[m2]*(nc+dx[m3]))/(dz[m3]-dz[m2]);
			    y2=(dz[m3]*(nt+dy[m2])-dz[m2]*(nt+dy[m3]))/(dz[m3]-dz[m2]);
			    break;	
			case 5:
			    x1=nc+dx[m2];           /*! Link 2, side 3-1  */
			    y1=nt+dy[m2];
			    x2=(dz[m1]*(nc+dx[m3])-dz[m3]*(nc+dx[m1]))/(dz[m1]-dz[m3]);
			    y2=(dz[m1]*(nt+dy[m3])-dz[m3]*(nt+dy[m1]))/(dz[m1]-dz[m3]);
			    break;	
			case 6:
			    x1=nc+dx[m3];        	/*! Link 3, side 1-2  */
			    y1=nt+dy[m3];
			    x2=(dz[m2]*(nc+dx[m1])-dz[m1]*(nc+dx[m2]))/(dz[m2]-dz[m1]);
			    y2=(dz[m2]*(nt+dy[m1])-dz[m1]*(nt+dy[m2]))/(dz[m2]-dz[m1]);
			    break;	
			case 7:
			    /*! Link sides 1-2, 2-3  */
			    x1=(dz[m2]*(nc+dx[m1])-dz[m1]*(nc+dx[m2]))/(dz[m2]-dz[m1]);
			    y1=(dz[m2]*(nt+dy[m1])-dz[m1]*(nt+dy[m2]))/(dz[m2]-dz[m1]);
			    x2=(dz[m3]*(nc+dx[m2])-dz[m2]*(nc+dx[m3]))/(dz[m3]-dz[m2]);
			    y2=(dz[m3]*(nt+dy[m2])-dz[m2]*(nt+dy[m3]))/(dz[m3]-dz[m2]);
			    break;  		
			case 9:
			    /*! Link sides 2-3, 3-1  */
			    x1=(dz[m3]*(nc+dx[m2])-dz[m2]*(nc+dx[m3]))/(dz[m3]-dz[m2]);
			    y1=(dz[m3]*(nt+dy[m2])-dz[m2]*(nt+dy[m3]))/(dz[m3]-dz[m2]);
			    x2=(dz[m1]*(nc+dx[m3])-dz[m3]*(nc+dx[m1]))/(dz[m1]-dz[m3]);
			    y2=(dz[m1]*(nt+dy[m3])-dz[m3]*(nt+dy[m1]))/(dz[m1]-dz[m3]);
			    break;
			case 8:
			    // Link sides 3-1, 1-2  
			    x1=(dz[m1]*(nc+dx[m3])-dz[m3]*(nc+dx[m1]))/(dz[m1]-dz[m3]);
			    y1=(dz[m1]*(nt+dy[m3])-dz[m3]*(nt+dy[m1]))/(dz[m1]-dz[m3]);
			    x2=(dz[m2]*(nc+dx[m1])-dz[m1]*(nc+dx[m2]))/(dz[m2]-dz[m1]);
			    y2=(dz[m2]*(nt+dy[m1])-dz[m1]*(nt+dy[m2]))/(dz[m2]-dz[m1]);
			case 0:
			    break;
			    
		    } // end of switch
		    if (icase != 0) {
			set(x1,y1);        // Put pen down
			dvect(x2,y2);		// Link to appropriate point
			
			//printf("%f\t%f\n",x1,y1);
			clengths[l] += sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
			
		    }
		}	 // Next triangle to consider  
	    }     // Next Channel 
	}     // Next Track  
    }      // Next contour level 
    
    //CGContextStrokePath(lgContext );
    //CGContextFlush(lgContext);
    //QDEndCGContext (GetWindowPort(oma_wind[gwnum].gwind), &lgContext);

    reset();
    //gwnum++;
    //orgx += swidth; 
    
    if( ruler_scale_defined ) 
	    smx = ruler_scale;
    else
	    smx = 1.0;

    for(l=0; l<nlevls; l++) 
	    pprintf("%.4f\t%.2f\t",clevls[l],clengths[l]/smx);
	    
    
    if( ruler_scale_defined && unit_text[0] )
	    pprintf("\t%s\n",unit_text);
    else
	    pprintf("\n");
    printf("OMA>");
    setarrow();
    return 0;
}

//short oldvol, current, ref;	

int	linedrawing = true;	

int scalx(float x,float y)
{
	xmin = x;
	xmax = y;
	xscalerange = scalex/(xmax-xmin);
	return 0;
}

int scaly(float x,float y)
{
	ymin = x;
	ymax = y;
	yscalerange = scaley/(ymax-ymin);
	return 0;
}

 



/* check to see if a cmd period was typed during line graphics plotting */

int checkstoprequest()
{
	extern	int			gwnum;
	
	CGrafPtr 		savePort;
	GDHandle		saveDevice;

	gwnum++;
	//GetPort(&orgport);
	GetGWorld(&savePort,&saveDevice);
	gets(nil);
	//SetPort((WindowPtr)orgport);
	SetGWorld(savePort,saveDevice);
	
	gwnum--;
	
	return 0;

}
