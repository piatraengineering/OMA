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

/* Still to do:
 *	allow multiple "plot 1-d windows"
 *	resizing of graph and 1-d plot ?  -- disable resizing for both if both are present?
 *	handle case when data graphics window is closed -- freeze 1-d graphics then
 *	label both data graphics and 1-d graphics
 *	write track # in 1-d graphics window
 *
 *
 *	process arrow up and down case properly
 *	
 *	all the same for other direction 
 */


#define	SBWIDE			0						/* the scroll bar width */
#define MAXSCROLLVAL	(256-3*SBWIDE-TBTHICK+3)

//#pragma segment PlotchansSeg

WindowPtr		scrollwindow;
ControlHandle	scroll;
Rect			srect,fatsrect,awndrect;	/* the scroll bar rectangle and source graphwindow rect*/
int				inversetable[256];
WindowRef		ptrToGwind;
int 			vert_plot = 0;		/* for verticle 1-D plots */
int				gwind_no;

// keep track of data on plot rows and columns
//unsigned int window_width[NGWINDS+1],window_height[NGWINDS+1];
//RCMarker row_marker[NGWINDS+1];
//RCMarker col_marker[NGWINDS+1];


int oldvpos;
int	lineongraph = 0;


extern	int			gwnum;

extern OMA_Window oma_wind[];

extern	WindowPtr	Ctwind;
extern WindowRef Status_window;

//extern 	GWorldPtr	myGWorldPtrs[]; // offscreen maps
extern	RGBColor	RGBBlack,RGBWhite;
//extern	short		windowtype[];
//extern Rect 		copyRect[];
extern Str255  pstring; /* a pascal general purpose text array */

int printf(),chooseposn();

int getwindowdata(int n)
{
// -- this gets things from the ofscreen map
// -- used to be from the displayed window -- could change it, not clear it's necessary though
/*
	int		i;
	int				xloc,rowby,vpos;
	unsigned char	*dpoint;
	Rect			sr;
	DATAWORD 		*datptcopy;
	RGBColor		clr;
	WindowPtr theActiveWindow;
        
	extern  DATAWORD *datpt;
	extern int doffset,have_max;
	extern TWOBYTE header[],trailer[];
	
	theActiveWindow = FrontWindow();
	gwind_no = activegwnum(theActiveWindow);
	
	if(theActiveWindow == Ctwind || theActiveWindow == Status_window){
		SysBeep(1);
		return -1;	
	}
	
	//awndrect =  (**(qd.thePort->visRgn)).rgnBBox;
	//GetWindowPortBounds(theActiveWindow,&awndrect);
	awndrect = copyRect[gwind_no];
	
	header[NX0] = header[NY0] = 0;		// need to set up a reasonable header 
	header[NDX] = header[NDY] = 1;
	
	i = header[NCHAN];					// save copies of these in case there isn't enough memory
	vpos = header[NTRAK];
	
	header[NCHAN] = awndrect.right;
	header[NTRAK] = awndrect.bottom;
	
	printf("%d %d c t\n",header[NCHAN],header[NTRAK]);
	
	if( checkpar() ) {
		header[NCHAN] = i;					// restore old values 
		header[NTRAK] = vpos;
		return -1;
	}
	datptcopy = datpt+doffset;
		

	//ptrToGwind = (CGrafPtr)theActiveWindow;
	//rowby = ((*(ptrToGwind->portPixMap))->rowBytes) & 0x1FFF;
	//sr = (*(ptrToGwind->portPixMap))->bounds;
	

	//rowby = ((*(mycgrafptr[gwind_no]->portPixMap))->rowBytes) & 0x1FFF;
	//rowby = GetPixRowBytes(GetPortPixMap(myGWorldPtrs[gwind_no]));
	//sr = (*(mycgrafptr[gwind_no]->portPixMap))->bounds;
	sr = copyRect[gwind_no];
	
	for( vpos=0; vpos < awndrect.bottom; vpos++) {
		//dpoint = (*(ptrToGwind->portPixMap))->baseAddr;
		//dpoint =  (unsigned char*) GetPixBaseAddr(GetGWorldPixMap(myGWorldPtrs[gwind_no]));
		dpoint += rowby*(vpos-sr.top) - sr.left;
		for(xloc=0; xloc<awndrect.right; xloc++){
			if(n == 0) 
				*(datptcopy++) = *(dpoint++);
			else {
				Index2Color(*(dpoint++),&clr);
				*(datptcopy++) = (clr.red >> 8) + (clr.green>>8) + (clr.blue>>8);
			}
		}
	}

	header[NCHAN] = awndrect.right;
	header[NTRAK] = awndrect.bottom;
	
	trailer[SFACTR] = 1;
	have_max = 0;

	maxx();
*/
	return 0;
}

