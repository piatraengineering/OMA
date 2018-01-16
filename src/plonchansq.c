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


#define	SBWIDE			0						/* the scroll bar width */
#define MAXSCROLLVAL	(256-3*SBWIDE-TBTHICK+3)

//#pragma segment PlotchansSeg

extern	WindowPtr		scrollwindow;
extern	ControlHandle	scroll;
extern	Rect			srect,fatsrect,awndrect;	/* the scroll bar rectangle and source graphwindow rect*/
extern	int				inversetable[256];
extern	WindowRef		ptrToGwind;
extern	int 			vert_plot;		/* for verticle 1-D plots */
extern	int				gwind_no;

// keep track of data on plot rows and columns
//unsigned int window_width[NGWINDS+1],window_height[NGWINDS+1];
//RCMarker row_marker[NGWINDS+1];
//RCMarker col_marker[NGWINDS+1];


extern	int oldvpos;
extern	int	lineongraph;


extern	int			gwnum;
extern HIViewID    mtViewID;

extern OMA_Window oma_wind[];

//extern	WindowPtr	gwind[];
extern	WindowPtr	Ctwind;
extern WindowRef Status_window, RGB_Tools_window;

//extern 	GWorldPtr	myGWorldPtrs[]; // offscreen maps
extern	RGBColor	RGBBlack,RGBWhite;
//extern	short		windowtype[];
//extern Rect 		copyRect[];
extern Str255  pstring; /* a pascal general purpose text array */

extern Rect 	r;
extern short	orgx;
extern short	swidth;


int printf(),chooseposn();

int openwindow_q(int swwidth,int swheight,char* name,int windowtype)
{
		//create a blank window
		OSErr err;
		//int i =gwnum;
		OSStatus CreateMouseTrackingView(HIViewRef parentView, const Rect* inBounds, HIViewID* inViewID);
		
		err = CreateNewWindow(kDocumentWindowClass,windowtype | kWindowCompositingAttribute
						| kWindowStandardHandlerAttribute,&r,&oma_wind[gwnum].gwind);
		
		SetWindowTitleWithCFString(oma_wind[gwnum].gwind,CFStringCreateWithCString(kCFAllocatorDefault,name,kCFStringEncodingASCII));
		oma_wind[gwnum].opaque = 1.0;
		oma_wind[gwnum].width = swwidth;
		oma_wind[gwnum].height = swheight;
		
		HIViewRef contentView;
		err = HIViewFindByID( HIViewGetRoot(oma_wind[gwnum].gwind), kHIViewWindowContentID, &contentView );
		require_noerr( err, troubles );

		Rect	viewBounds;
		GetWindowPortBounds(oma_wind[gwnum].gwind, &viewBounds);
		err = CreateMouseTrackingView(contentView, &viewBounds, &mtViewID);
		require_noerr( err, CantCreateMTView );
		
		ShowWindow(oma_wind[gwnum].gwind);
		//gwnum++;

		
	return 1;	// successful
troubles:
CantCreateMTView:
	return 0;	// problems
}

int draw_verticle_line_on_data_window(int data_window_index, int vpos){
    CGRect dstRect;
	HIViewRef   contentView;
	HIViewFindByID(HIViewGetRoot(oma_wind[data_window_index].gwind), kHIViewWindowContentID, &contentView);
	HIViewGetBounds(contentView, &dstRect);

	CGContextRef	myContext;
	QDBeginCGContext(GetWindowPort(oma_wind[data_window_index].gwind), &myContext);
	CGContextSetLineWidth (myContext, 1);
	CGContextSetRGBStrokeColor (myContext, 1, 1, 1, 1);
	CGContextSetBlendMode(myContext,kCGBlendModeDifference);
	CGContextBeginPath (myContext);
	CGContextMoveToPoint( myContext, vpos, 0 );					
	CGContextAddLineToPoint( myContext, vpos, dstRect.size.height );
	CGContextStrokePath( myContext );
	CGContextFlush(myContext);
	QDEndCGContext (GetWindowPort(oma_wind[data_window_index].gwind), &myContext);
	return 0;
}
int draw_horizontal_line_on_data_window(int data_window_index, int vpos){
    CGRect dstRect;
	HIViewRef   contentView;
	HIViewFindByID(HIViewGetRoot(oma_wind[data_window_index].gwind), kHIViewWindowContentID, &contentView);
	HIViewGetBounds(contentView, &dstRect);

	CGContextRef	myContext;
	QDBeginCGContext(GetWindowPort(oma_wind[data_window_index].gwind), &myContext);
	CGContextSetLineWidth (myContext, 1);
	CGContextSetRGBStrokeColor (myContext, 1, 1, 1, 1);
	CGContextSetBlendMode(myContext,kCGBlendModeDifference);
	CGContextBeginPath (myContext);
	CGContextMoveToPoint( myContext, 0, vpos );					
	CGContextAddLineToPoint( myContext,  dstRect.size.width, vpos );
	CGContextStrokePath( myContext );
	CGContextFlush(myContext);
	QDEndCGContext (GetWindowPort(oma_wind[data_window_index].gwind), &myContext);
	return 0;
}

int dochanplot_q(int data_window_index)
{
	Point			mousePt;
	int				rowby,vpos,inc,xloc,yloc;
	float			fvpos,fvariable;
	unsigned char	*dpoint;
	Rect			viewrect;
	float				increment;
	int				scroll_wind_no=0;
	int				generate_pdf = 0;
	CGContextRef	SetupPDFcontext(int);
	
	CGPoint			p1;
	CGContextRef	myContext;
	HIViewRef   contentView;
	CGRect dstRect;
	extern	char	txt[];
	
	if( data_window_index < 0) {	// generate pdf data so this can be coppied to the clipboard
		generate_pdf = 1;
		data_window_index = -data_window_index - 1;
		if( vert_plot) {
			scroll_wind_no = oma_wind[data_window_index].col_marker.window;
		}else {
			scroll_wind_no = oma_wind[data_window_index].row_marker.window;
		}
		if( oma_wind[scroll_wind_no].pdfData != NULL){
			CGDataConsumerRelease(oma_wind[scroll_wind_no].consumer);		// this also releases the pdfData, via MyCFDataRelease
			//free(oma_wind[scroll_wind_no].pdfData);
			oma_wind[scroll_wind_no].pdfData = NULL;
		}	
	}

	ptrToGwind = oma_wind[data_window_index].gwind;
	SetPortWindowPort((WindowRef)ptrToGwind);
	
	// reset this in case the window isn't all on sceen
	// this has the size of the data window -- should all be on the screen, but not necessarily the same size as off screen data
	GetWindowPortBounds((WindowRef)ptrToGwind,&awndrect);
		
	GetMouse(&mousePt);
	//printf("quartz dochanplot of window %d\n",data_window_index);
	
	rowby = oma_wind[data_window_index].width*4;

	dpoint =  (unsigned char*) oma_wind[data_window_index].window_rgb_data;
		
	GetWindowPortBounds(scrollwindow,&viewrect);
	
	HIViewFindByID(HIViewGetRoot(scrollwindow), kHIViewWindowContentID, &contentView);
	HIViewGetBounds(contentView, &dstRect);

	if(generate_pdf){
		myContext = SetupPDFcontext(scroll_wind_no);
	} else {
		QDBeginCGContext(GetWindowPort(scrollwindow), &myContext);
	}
	CGContextSetLineWidth (myContext, 1);
	CGContextSetRGBStrokeColor (myContext, 0, 0, 0, 1);
	CGContextSetRGBFillColor (myContext, 1, 1, 1, 1);
	
	CGContextSelectFont (myContext,"Helvetica",18,kCGEncodingMacRoman);
	CGContextSetTextDrawingMode (myContext,kCGTextFill);
		
	CGContextBeginPath (myContext);
	
	if( vert_plot) {
		fvariable = (awndrect.bottom);
		increment = fvariable/oma_wind[data_window_index].height;
		//if (increment < 1.) increment = 1.;
		if(oma_wind[data_window_index].col_marker.lineongraph && !generate_pdf) {
			vpos = mousePt.h;
		} else {
			vpos = oma_wind[data_window_index].col_marker.where;
			// there isn't a line on the graph yet
			draw_verticle_line_on_data_window(data_window_index,vpos);
		}
		if(vpos < 0) vpos = 0;
		if(vpos > awndrect.right-1) vpos = awndrect.right-1;
		fvpos = vpos;
		inc = (fvpos/increment+.5);
		dpoint += inc*4;
		//SetPortWindowPort(scrollwindow);
		if(vpos != oma_wind[data_window_index].col_marker.where || generate_pdf 
				|| !oma_wind[data_window_index].col_marker.lineongraph){
			CGContextFillRect (myContext,dstRect);
			CGContextStrokeRect(myContext,dstRect);
			for (xloc=0; xloc<awndrect.bottom; xloc+= 1) {				// plot the profile 
				inc = xloc/increment+.5;
				yloc = *(dpoint+rowby*inc);

				//yloc = *(dpoint);
				//
				//dpoint += rowby;					
				p1.x = xloc;
				p1.y = yloc;    
				if(xloc == 0) 
					CGContextMoveToPoint( myContext, p1.x, p1.y );					
				else {
					CGContextAddLineToPoint( myContext, p1.x, p1.y );
				}
			}
			oma_wind[data_window_index].col_marker.where = vpos;
			oma_wind[data_window_index].col_marker.lineongraph = 1;

			sprintf(txt,"Column %d",vpos);
			CGContextSetRGBFillColor (myContext, 0, 0, 0, 1);
			CGContextShowTextAtPoint(myContext,10,256-20,txt,strlen(txt));
		}
				
	} else {
		fvariable = (awndrect.right+1);
		increment = fvariable/oma_wind[data_window_index].width;
		//if (increment == 0) increment = 1;
		if(oma_wind[data_window_index].row_marker.lineongraph  && !generate_pdf) {
			vpos = mousePt.v;
		} else {
			vpos = oma_wind[data_window_index].row_marker.where;
			// there isn't a line on the graph yet
			draw_horizontal_line_on_data_window(data_window_index, vpos);
		}
		if(vpos < 0) vpos = 0;
		if(vpos > awndrect.bottom-1) vpos = awndrect.bottom-1;
		fvpos = vpos;
		inc = (fvpos/increment+.5);
		dpoint += rowby*inc;				// first data point address 
		if(vpos != oma_wind[data_window_index].row_marker.where || generate_pdf  
				|| !oma_wind[data_window_index].row_marker.lineongraph){
			CGContextFillRect (myContext,dstRect);
			CGContextStrokeRect(myContext,dstRect);
			for (xloc=0; xloc<awndrect.right; xloc+= 1) {				// plot the profile 
				inc = xloc/increment+.5;
				yloc = *(dpoint+4*inc);
				p1.x = xloc;
				p1.y = yloc;    
				if(p1.x == 0) 
					CGContextMoveToPoint( myContext, p1.x, p1.y );					
				else {
					CGContextAddLineToPoint( myContext, p1.x, p1.y );
				}
			}

			oma_wind[data_window_index].row_marker.where = vpos;
			oma_wind[data_window_index].row_marker.lineongraph = 1;
			sprintf(txt,"Row %d",vpos);
			CGContextSetRGBFillColor (myContext, 0, 0, 0, 1);
			CGContextShowTextAtPoint(myContext,10,256-20,txt,strlen(txt));
		}
		
	}
	CGContextStrokePath( myContext );

	if(generate_pdf){ 
		CGContextFlush(myContext);
		CGContextEndPage(myContext);
		CGContextRelease(myContext);
	} else {
		CGContextFlush(myContext);
		QDEndCGContext (GetWindowPort(scrollwindow), &myContext);
	}

	return 0;
}

// this routine is resopnsible for 
//	opening the new window
//	setting the row and column markers properly
//  calling dochanplot, which actually draws the graph
int plotchans_q(int n)	
{
	
	// gwnum is the number of the window to be opened here
	// gwind_no is the number of the current front window -- and it has to be a bitmap window
	WindowPtr theActiveWindow;
	
	extern int vert_plot;
	extern short toolselected;
	extern int	showselection;
	extern int	showruler;
	extern int	plotline;
	extern int	docalcs;
	
	void set_window_type(int,int);

	theActiveWindow = FrontWindow();	//-- this is depricated now
										// use theActiveWindow = ActiveNonFloatingWindow();
	gwind_no = activegwnum(theActiveWindow);
	
	if(theActiveWindow == Ctwind || theActiveWindow == Status_window || theActiveWindow == RGB_Tools_window
		|| abs(oma_wind[gwind_no].windowtype) == LINEGRAPH ||oma_wind[gwind_no].windowtype == QPIVGRAPH){
		SysBeep(1);
		return -1;
	}
	
	vert_plot = n;
	
	GetWindowPortBounds(theActiveWindow,&awndrect);
	//oma_wind[gwnum].windowtype = QROWCOL;
	set_window_type(gwnum,QROWCOL);
	
	if (vert_plot) {
		chooseposn(awndrect.bottom + SBWIDE,256,1);
		
		if(!openwindow_q(awndrect.bottom + SBWIDE,256,"Columns",kWindowStandardFloatingAttributes)) return -1;	
		oma_wind[gwind_no].col_marker.window = gwnum;	// this says this window has a plot row on it
		oma_wind[gwnum].col_marker.window = gwind_no;	// point back to the data window too
		oma_wind[gwind_no].col_marker.where = awndrect.right/2;
		oma_wind[gwind_no].col_marker.lineongraph = 0; 

	} else {
		chooseposn(awndrect.right + SBWIDE,256,1);
		if(!openwindow_q(awndrect.right + SBWIDE,256,"Rows",kWindowStandardFloatingAttributes)) return -1;	
		oma_wind[gwind_no].row_marker.window = gwnum;	// this says this window has a plot row on it
		oma_wind[gwnum].row_marker.window = gwind_no;	// point back to the data window too
		oma_wind[gwind_no].row_marker.where = awndrect.bottom/2;
		oma_wind[gwind_no].row_marker.lineongraph = 0;
	}
	
	//SetPort(GetWindowPort(oma_wind[gwnum].gwind));
	
	
	scrollwindow = oma_wind[gwnum].gwind;

	dochanplot_q(gwind_no);
	dochanplot_q(-gwind_no-1);		// generate pdf data too for this initial plot
	
	toolselected = 0;
	docalcs = false;
	showselection = false;
	showruler = false;
	plotline = false;

	update_status();

	//reset();		// this did other stuff but also did the folowing updates
	gwnum++;
	orgx += swidth; 
	
	return 0;
}

int plot_both(){
	WindowRef the_wind = GetUserFocusWindow();
	int err;	
	err = plotchans_q(1);
	if(err) return(err);
	SelectWindow(the_wind);
	err = plotchans_q(0);
	return(err);
	
}


int		histo[256];

int		hautoscale = true;
int		hstdscrnsize = false;
DATAWORD		histmax = 0;
DATAWORD		histmin = 0;
int 	histwidth = 256;
int 	histheight = 256;
int		hclear = true;


int histogram_q(char* name)
{

	int		i,gwind_no;
	int				xloc,vpos;
	int scalx(float,float),scaly(float,float);
	void moveto(float , float ),lineto(float , float );
	unsigned char	*dpoint;
	
	WindowPtr theActiveWindow;
	
	theActiveWindow = FrontWindow();
	gwind_no = activegwnum(theActiveWindow);

	if(theActiveWindow == Ctwind || theActiveWindow == Status_window 
	   || theActiveWindow == RGB_Tools_window || oma_wind[gwind_no].windowtype != QBITMAP){
		SysBeep(1);
		return -1;	
	}
	
	if(!ploton(false,histwidth,histheight,false,"Histogram")) return -1;	

	scalx(0.0,256.0);
	scaly(0.0,histheight);

	
	if(hautoscale) {
	
		if(hclear)
			for(i=0; i<=255; i++) histo[i] = 0;

		dpoint =  (unsigned char*) oma_wind[gwind_no].window_rgb_data;
		for( vpos=0; vpos < oma_wind[gwind_no].width*oma_wind[gwind_no].height; vpos++) {
				histo[*(dpoint)] += 1;
				dpoint += 4;
		}
		histmax = histmin = 0;
		for(xloc=0; xloc<256; xloc++)
			if( histo[xloc] > histmax ) histmax = histo[xloc];
		}
		
	for (xloc=0; xloc<256; xloc++) {
		
		if(xloc == 0) 
			
			//CGContextMoveToPoint( histContext, xloc*histwidth/255, (histo[xloc]*histheight/histmax) );	
			moveto( xloc*histwidth/255, (histo[xloc]*histheight/histmax) );							
		else {
			
			//CGContextAddLineToPoint( histContext, xloc*histwidth/255, (histo[xloc]*histheight/histmax) );
			lineto( xloc*histwidth/255, (histo[xloc]*histheight/histmax) );
		}
		
	}

	reset();

	return 0;
}
/*
 LABEL text_string
    Display the text_string in the upper left of the current window.
*/
int label(int n, int index)				// display a text string in the window 
{
	
	WindowPtr theActiveWindow;
	CGContextRef	myContext;
	OSErr err;
    
    int xpos, ypos,i;
	
	extern	char	txt[],cmnd[];
	
	theActiveWindow = FrontWindow();
	
	if(theActiveWindow == Ctwind || theActiveWindow == Status_window || theActiveWindow == RGB_Tools_window){
		//SysBeep(1);
        beep();
		return -1;
	}
	
	gwind_no = activegwnum(theActiveWindow);
    xpos = 10;
    ypos = oma_wind[gwind_no].height-30;
	
	err = QDBeginCGContext(GetWindowPort(theActiveWindow), &myContext);
	CGContextBeginPath (myContext);
	CGContextSelectFont (myContext,"Helvetica",18,kCGEncodingMacRoman);
	CGContextSetTextDrawingMode (myContext,kCGTextFillStroke);
	CGContextSetRGBFillColor (myContext, 1, 1, 1, 1);
	CGContextSetLineWidth (myContext, .25);
	sprintf(txt,"%s",&cmnd[index]);
	CGContextShowTextAtPoint(myContext,xpos,ypos,txt,strlen(txt));
	CGContextFlush(myContext);
	QDEndCGContext (GetWindowPort(theActiveWindow), &myContext);
    
    
    if( oma_wind[gwind_no].num_annotations < MAX_ANNOTATIONS){
        i = oma_wind[gwind_no].num_annotations++;   // one more annotation
        oma_wind[gwind_no].annotations[i].type = WINDOW_LABEL;
        oma_wind[gwind_no].annotations[i].xpos = xpos;
        oma_wind[gwind_no].annotations[i].ypos = ypos;
        oma_wind[gwind_no].annotations[i].label = malloc(strlen(txt));
        strcpy(oma_wind[gwind_no].annotations[i].label, txt);
    }
	
	return 0;
}

/*
 MARKPIXEL x y
    Display an X centered at pixel x, y.
 */

int markpixel(int n, int index){
    
    WindowPtr theActiveWindow;
	CGContextRef	myContext;
	OSErr err;
    float x,y;
    int xpos, ypos,narg,i;
    
    CGRect dstRect;
	HIViewRef   contentView;
	
	extern	char	txt[],cmnd[];
    extern short header[],image_is_color;
	
	theActiveWindow = FrontWindow();
	
	if(theActiveWindow == Ctwind || theActiveWindow == Status_window || theActiveWindow == RGB_Tools_window){
        beep();
        printf("Not a graphics window.\n");
		return -1;
	}
    if((narg = sscanf(&cmnd[index],"%f %f", &x,&y)) != 2){
        beep();
        printf("Command is MARKPIXEL x y\n");
        return -1;
    }
	gwind_no = activegwnum(theActiveWindow);
    ;
    
	HIViewFindByID(HIViewGetRoot(oma_wind[gwind_no].gwind), kHIViewWindowContentID, &contentView);
	HIViewGetBounds(contentView, &dstRect);
    ;
    
    if(image_is_color) y *=3;
    
    xpos = x * (float)dstRect.size.width/(float)header[NCHAN];
    ypos = dstRect.size.height - y * (float)dstRect.size.height/(float)header[NTRAK];
	
    if( oma_wind[gwind_no].num_annotations < MAX_ANNOTATIONS){
        i = oma_wind[gwind_no].num_annotations++;   // one more annotation
        oma_wind[gwind_no].annotations[i].type = WINDOW_MARKER;
        oma_wind[gwind_no].annotations[i].xpos = xpos;
        oma_wind[gwind_no].annotations[i].ypos = ypos;
    }

    
    
	err = QDBeginCGContext(GetWindowPort(theActiveWindow), &myContext);
	CGContextSetLineWidth (myContext, 1);
	CGContextSetRGBStrokeColor (myContext, 1, 1, 1, 1);
	CGContextSetBlendMode(myContext,kCGBlendModeDifference);
	CGContextBeginPath (myContext);
	CGContextMoveToPoint( myContext, xpos-MARKSIZE, ypos-MARKSIZE );					
	CGContextAddLineToPoint( myContext, xpos+MARKSIZE, ypos+MARKSIZE );
	CGContextMoveToPoint( myContext, xpos-MARKSIZE, ypos+MARKSIZE );					
	CGContextAddLineToPoint( myContext, xpos+MARKSIZE, ypos-MARKSIZE );
	CGContextStrokePath( myContext );
	CGContextFlush(myContext);
	QDEndCGContext (GetWindowPort(theActiveWindow), &myContext);
    
	return 0;
}


int dmnmx_q()				// display the color min and max in the window 
{
	
	WindowPtr theActiveWindow;
	CGContextRef	myContext;
	OSErr err;
    int xpos,ypos,i;
	
	extern	char	txt[];
	extern DATAWORD cmin,cmax;
	
	theActiveWindow = FrontWindow();

	if(theActiveWindow == Ctwind || theActiveWindow == Status_window || theActiveWindow == RGB_Tools_window){
		SysBeep(1);
		return -1;
	}
	
	gwind_no = activegwnum(theActiveWindow);
	
	err = QDBeginCGContext(GetWindowPort(theActiveWindow), &myContext);
    CGContextBeginPath (myContext);
    CGContextSelectFont (myContext,"Helvetica",18,kCGEncodingMacRoman);
    CGContextSetTextDrawingMode (myContext,kCGTextFillStroke);
    CGContextSetRGBFillColor (myContext, 1, 1, 1, 1);
    CGContextSetLineWidth (myContext, .25);
    
    sprintf(txt,DATAFMT,cmin);
    xpos = 10; ypos = 30;
    
    if( oma_wind[gwind_no].num_annotations < MAX_ANNOTATIONS){
        i = oma_wind[gwind_no].num_annotations++;   // one more annotation
        oma_wind[gwind_no].annotations[i].type = WINDOW_LABEL;
        oma_wind[gwind_no].annotations[i].xpos = xpos;
        oma_wind[gwind_no].annotations[i].ypos = ypos;
        oma_wind[gwind_no].annotations[i].label = malloc(strlen(txt));
        strcpy(oma_wind[gwind_no].annotations[i].label, txt);
    }
     
    CGContextShowTextAtPoint(myContext,xpos,ypos,txt,strlen(txt));
    
    sprintf(txt,DATAFMT,cmax);
    ypos = 10;
    
    if( oma_wind[gwind_no].num_annotations < MAX_ANNOTATIONS){
        i = oma_wind[gwind_no].num_annotations++;   // one more annotation
        oma_wind[gwind_no].annotations[i].type = WINDOW_LABEL;
        oma_wind[gwind_no].annotations[i].xpos = xpos;
        oma_wind[gwind_no].annotations[i].ypos = ypos;
        oma_wind[gwind_no].annotations[i].label = malloc(strlen(txt));
        strcpy(oma_wind[gwind_no].annotations[i].label, txt);
    }
     
    CGContextShowTextAtPoint(myContext,xpos,ypos,txt,strlen(txt));
    
    CGContextFlush(myContext);
	QDEndCGContext (GetWindowPort(theActiveWindow), &myContext);
    
	return 0;
}
