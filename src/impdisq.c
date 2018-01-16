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

// These are the convert-to-quartz routines
// Window display startes from the Apple MouseTracking example code

OMA_Window	oma_wind[NGWINDS+1];

extern TWOBYTE	header[];

extern DATAWORD	crange;					// the range of colors defined by the curret cmin and cmax
extern int gwnum;
extern Rect 	r;

extern short pixsiz;
extern DATAWORD *datpt;
extern TWOBYTE header[];
extern int display_is_off;
extern char	   lastname[];
extern char    cmnd[];
extern int 		doffset;
extern int 		ncolor;
extern int      thepalette;

extern short toolselected;
extern Point substart,subend;
extern int  docalcs,plotline;

extern short newwindowflag;
extern Boolean autoscale;
extern DATAWORD min,max;
extern DATAWORD cmin,cmax;
extern int have_max;
extern short	orgx;
extern short	swidth;

extern float    opaque_value;

extern	int 	vert_plot;		// for verticle 1-D plots
extern WindowPtr scrollwindow;

extern short zwidth,zheight;
extern int is_zoom_window,is_auto_window,is_fft_window,is_zoom_rgb_window;
extern int  showselection;
extern int  showruler;

int needNewImage = 0;
int needNewZoomImage = 0;

// -----------------------------------------------------------------------------
/*
 WindowPtr 	gwind;
 int			windowtype;
 float   	opaque;
 RCMarker 	row_marker;
 RCMarker 	col_marker;
 
 //	quartz only
 Ptr 		window_rgb_data;
 Ptr			view_data_ptr;
 int			width;
 int			height;
 CFDataRef	pdfData;			// for some linegraphics to allow copy of window contents as pdf
 CGDataConsumerRef consumer;		// for some linegraphics to allow copy of window contents as pdf
 
 #define BITMAP		1		// specifies that a window contains a bitmap
 #define LINEGRAPH	2		// specifies that a window contains linegraphics
 #define ZOOM		3		// specifies that a window containes a zoomed image
 #define FFT			4		// specifies that a window contains the FFT of a subregion
 #define AUTO		5		// Autocorrelation Window
 #define RGBMAP		6		// RGB window
 
 #define QBITMAP		-1		// specifies that a window contains a bitmap
 #define QLINEGRAPH	-2		// specifies that a window contains linegraphics
 #define QZOOM		-3		// specifies that a window containes a zoomed image
 #define QFFT		-4		// specifies that a window contains the FFT of a subregion
 #define QAUTO		-5		// Autocorrelation Window
 #define QRGBMAP		-6		// RGB window
 #define QPIVGRAPH	-7		// PIV window
 #define QRGBZOOM	-8		// specifies that a window containes a zoomed rgb image
 #define QROWCOL		2
 
 */
// -----------------------------------------------------------------------------
void set_window_type(int n,int type)
{
	oma_wind[n].windowtype = type;
	oma_wind[n].row_marker.window = -1;
	oma_wind[n].col_marker.window = -1;
    oma_wind[n].num_annotations=0;
	switch (type) {
		case QFFT:
		case QAUTO:
		case QZOOM:
		case QRGBZOOM:
		case QBITMAP:
		case QRGBMAP:
			oma_wind[n].pdfData = NULL;
			oma_wind[n].consumer = NULL;
			break;
		case QLINEGRAPH:
		case QPIVGRAPH:
			oma_wind[n].window_rgb_data =  NULL;
			break;
		case QROWCOL:
			oma_wind[n].pdfData = NULL;
			oma_wind[n].consumer = NULL;
			oma_wind[n].window_rgb_data =  NULL;
			break;
            
	}
    
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------



RGBColor color[256][8];	// the colors in the color palettes
int printf(),pprintf(), chooseposn(short, short, short);
Ptr Get_fft_from_image_buffer(int allocate_new,Point center_point);


HIViewID    mtViewID = { kMTViewSignature, 0 };

int data_window = 0;

int viewgwnum(Ptr data)		// return the index of the view data passed as an argument
{
	int	tmp;
	
	for(tmp=0;tmp<gwnum;tmp++) {
		if(oma_wind[tmp].view_data_ptr == data) {
            //			printf("Window %d -- %d\n",tmp,wind);
			return(tmp);
		}
		//printf("Windowx %d -- %d  %d\n",tmp,wind,gwind[tmp]);
  	}
  	return 0;
}


//-----------------------------------------------------------------------------------
// Called for each CGPathElement when scanning the CGPath in CGPathApply()
static void MyCGPathApplier(void* info, const CGPathElement* element)
{
    CGContextRef ctx = (CGContextRef)info;
    switch (element->type)
    {
        case kCGPathElementMoveToPoint:
            CGContextMoveToPoint(ctx, element->points[0].x, element->points[0].y);
            break;
            
        case kCGPathElementAddLineToPoint:
            CGContextAddLineToPoint(ctx, element->points[0].x, element->points[0].y);
            break;
            
        default:	// we know our path only contains line segments
            break;
    }
}

//-----------------------------------------------------------------------------------
Point		qdPt;

static void DrawTheMTView(CGContextRef ctx, MTViewData* data)
{
    CGRect dstRect;
	HIViewRef   contentView;
	int gw = -1;
    OSStatus err;
    CGImageRef LoadImageFromData(int width, int height, void* imagedata);
    Ptr Get_rgb_from_image_buffer(int allocate_new);
    Ptr Get_color_rgb_from_image_buffer(int allocate_new);
    Ptr Get_zoom_rgb_from_image_buffer(int allocate_new, Point zoom_point);
    
	// Draw the image first, before stroking the path; otherwise the path gets overwritten
    if (data->theImage != NULL){
		//printf("data window %d\n",data_window);
		HIViewFindByID(HIViewGetRoot(oma_wind[data_window].gwind), kHIViewWindowContentID, &contentView);
		HIViewGetBounds(contentView, &dstRect);
		if(is_zoom_window) {
			for( gw=0; gw<= gwnum; gw++){
				if( oma_wind[gw].windowtype == QZOOM)
                    break;
			}
			if(oma_wind[gw].view_data_ptr == (void*) data){
				HIViewFindByID(HIViewGetRoot(oma_wind[gw].gwind), kHIViewWindowContentID, &contentView);
				HIViewGetBounds(contentView, &dstRect);
                needNewZoomImage = 1;
				//printf("zoom %d\n",gw);
			}
		}
		if(is_zoom_rgb_window) {
			for( gw=0; gw<= gwnum; gw++){
				if( oma_wind[gw].windowtype == QRGBZOOM)
					break;
			}
			if(oma_wind[gw].view_data_ptr == (void*) data){
				HIViewFindByID(HIViewGetRoot(oma_wind[gw].gwind), kHIViewWindowContentID, &contentView);
				HIViewGetBounds(contentView, &dstRect);
                needNewZoomImage = 1;
				//printf("zoom %d\n",gw);
			}
		}
		if(is_fft_window) {
			for( gw=0; gw<= gwnum; gw++){
				if( oma_wind[gw].windowtype == QFFT)
                    break;
			}
			if(oma_wind[gw].view_data_ptr == (void*) data){
				HIViewFindByID(HIViewGetRoot(oma_wind[gw].gwind), kHIViewWindowContentID, &contentView);
				HIViewGetBounds(contentView, &dstRect);
				//printf("zoom %d\n",gw);
			}
		}
		if(is_auto_window) {
			for( gw=0; gw<= gwnum; gw++){
				if( oma_wind[gw].windowtype == QAUTO)
                    break;
			}
			if(oma_wind[gw].view_data_ptr == (void*) data){
				HIViewFindByID(HIViewGetRoot(oma_wind[gw].gwind), kHIViewWindowContentID, &contentView);
				HIViewGetBounds(contentView, &dstRect);
				//printf("zoom %d\n",gw);
			}
		}
		//dstRect = CGRectMake(0, 0, CGImageGetWidth(data->theImage), CGImageGetHeight(data->theImage));]
        //if((needNewImage == 1 && gw == -1) || (needNewImage == 2 && gw > -1)){
        if(needNewImage==1){
            CGImageRelease(data->theImage);
            if(oma_wind[data_window].windowtype == QBITMAP)
                oma_wind[data_window].window_rgb_data = Get_rgb_from_image_buffer(1);
            else if (oma_wind[data_window].windowtype == QRGBMAP)
                oma_wind[data_window].window_rgb_data = Get_color_rgb_from_image_buffer(1);
            data->theImage = LoadImageFromData(oma_wind[data_window].width,oma_wind[data_window].height,oma_wind[data_window].window_rgb_data);
            needNewImage = 0;
        }
        
        if(needNewZoomImage==1){
            CGImageRelease(data->theImage);
            if(is_zoom_window) oma_wind[gw].window_rgb_data = Get_zoom_rgb_from_image_buffer(ALLOCATE,qdPt);
            if(is_zoom_rgb_window) oma_wind[gw].window_rgb_data = Get_zoom_rgb_from_image_buffer(ALLOCATERGB,qdPt);
            data->theImage = LoadImageFromData(oma_wind[gw].width,oma_wind[gw].height,oma_wind[gw].window_rgb_data);
            needNewZoomImage = 0;
        }
        
        //HIViewGetBounds(data->theView, &dstRect);
        
		err = HIViewSetFrame(data->theView, &dstRect );     // with ctx here it gave an error
        // with contview here, it draws wrong place
		err = HIViewDrawCGImage(ctx, &dstRect, data->theImage);
        //CGContextFlush(ctx);
        //free(data->theImage);
        
		//CGContextDrawImage(ctx, dstRect, data->theImage);
        
	}
    
    if (data->thePath != NULL){
		//printf("path not null!\n"); // -- this happens when lines are drawn on the image
		CGPathApply(data->thePath, (void*)ctx, MyCGPathApplier);
		CGContextSetRGBStrokeColor (ctx, 1, 1, 1, 1);
		CGContextSetBlendMode(ctx,kCGBlendModeDifference);
        
		CGContextStrokePath(ctx);
	}
}   // DrawTheMTView

// -----------------------------------------------------------------------------
static HIPoint
QDGlobalToHIViewLocal( const Point inGlobalPoint, const HIViewRef inDestView)
{
    HIPoint viewPoint = CGPointMake(inGlobalPoint.h, inGlobalPoint.v);
    
    HIPointConvert(&viewPoint, kHICoordSpace72DPIGlobal, NULL, kHICoordSpaceView, inDestView);
    return viewPoint;
}

Point showcoords_q(HIPoint pt, int line,int window_number)
{
    int nc,nt;
    Point answer;
    float x,y;
    
	int printxyzstuff();
	CGRect dstRect;
	HIViewRef   contentView;
    
    //extern short	windowtype[];
    //GetPortBounds(GetQDGlobalsThePort(),&portRect);
	HIViewFindByID(HIViewGetRoot(FrontNonFloatingWindow()), kHIViewWindowContentID, &contentView);
	HIViewGetBounds(contentView, &dstRect);
    
    x =  pt.x * (header[NCHAN]-1);
    x /= dstRect.size.width;
	if(oma_wind[window_number].windowtype == QRGBMAP)
		y =  pt.y * (header[NTRAK]/3-1);
	else
		y =  pt.y * (header[NTRAK]-1);
    y /= dstRect.size.height;
    nc = x + 0.5;
    nt = y + 0.5;
    
    if(nc > (header[NCHAN]-1)) nc = header[NCHAN]-1;
    if(nc < 0) nc = 0;
	if(oma_wind[window_number].windowtype == QRGBMAP){
		if(nt > (header[NTRAK]/3-1)) nt = header[NTRAK]/3-1;
	}else{
		if(nt > (header[NTRAK]-1)) nt = header[NTRAK]-1;
	}
    if(nt < 0) nt = 0;
    
    if( line >= 0 )			// print if line is 0 or 1, otherwise just return
        printxyzstuff(nc,nt,line);
    
    answer.h = nc;
    answer.v = nt;
    return(answer);
}



//-----------------------------------------------------------------------------------
static void DoTheTracking(EventRef inEvent, MTViewData* data)
{
    OSErr err;
    MouseTrackingResult	mouseResult;
    ControlPartCode	part;
    // Point		qdPt;
    HIPoint		where,p1,rectlines[5],where_box;
    CGRect dstRect;
    HIViewRef   contentView;
    extern int alter_vectors;
    extern int			thereisabox;
    extern float aspect_ratio;
    
    int dochanplot_q();
    Ptr Get_zoom_rgb_from_image_buffer(int allocate_new, Point zoom_point);
    int base_vector (Point point, short width, short height, int mode);
    
    // Extract the mouse location (local coordinates!)
    err = GetEventParameter(inEvent, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(HIPoint), NULL, &where);
    
    // update the coordinates and show the value in the status window
    int wn;
    wn = activegwnum(FrontNonFloatingWindow());
    data_window =  wn;
    //printf("%g %g\n",where.x,where.y);
    substart = showcoords_q(where,0,wn);
    
    
    p1 = where;		// save location of the first point
    
    // handle the toggle of the alpha channels of the window
    UInt32 modifiers;
    EventMouseButton button;
    GetEventParameter(inEvent,kEventParamMouseButton,typeMouseButton,NULL,sizeof(typeMouseButton),NULL,&button);
    GetEventParameter(inEvent,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(typeUInt32),NULL,&modifiers);
    if(modifiers & controlKey || button == kEventMouseButtonSecondary){ // the right button
        if(oma_wind[wn].opaque == 1.0)
            oma_wind[wn].opaque = opaque_value;
        else
            oma_wind[wn].opaque = 1.0;
        SetWindowAlpha (FrontNonFloatingWindow(),oma_wind[wn].opaque);
    }
    
    CGAffineTransform m = CGAffineTransformIdentity;
    
    // Reset the path
    if (data->thePath != NULL)
        CGPathRelease(data->thePath);
    
    data->thePath = CGPathCreateMutable();
    
    CGPathMoveToPoint(data->thePath, &m, where.x, where.y);
    
    
    // now check to update column or row plots
    if( oma_wind[wn].windowtype == QBITMAP && toolselected == 0 &&
       (oma_wind[wn].row_marker.window != -1 || oma_wind[wn].col_marker.window != -1)) {
        HIViewFindByID(HIViewGetRoot(FrontNonFloatingWindow()), kHIViewWindowContentID, &contentView);
        HIViewGetBounds(contentView, &dstRect);
        CGPathMoveToPoint(data->thePath, &m, where.x, where.y);
        if(oma_wind[wn].row_marker.window != -1){
            vert_plot = 0;
            scrollwindow = oma_wind[oma_wind[wn].row_marker.window].gwind;
            dochanplot_q(wn);
            //SetPortWindowPort(oma_wind[wn].gwind);
            CGPathAddLineToPoint(data->thePath, &m, 0.0, where.y);
            p1.x = dstRect.size.width;
            CGPathAddLineToPoint(data->thePath, &m, p1.x, where.y);
            CGPathAddLineToPoint(data->thePath, &m, where.x, where.y);
        }
        if(oma_wind[wn].col_marker.window != -1){
            vert_plot = 1;
            scrollwindow = oma_wind[oma_wind[wn].col_marker.window].gwind;
            dochanplot_q(wn);
            CGPathAddLineToPoint(data->thePath, &m, where.x, 0.0);
            where.y = dstRect.size.height;
            CGPathAddLineToPoint(data->thePath, &m, where.x, where.y);
            //SetPortWindowPort(oma_wind[wn].gwind);
        }
    }
    
    while (true)
    {
        // Watch the mouse for change: qdPt comes back in global coordinates!
        TrackMouseLocation((GrafPtr)(-1), &qdPt, &mouseResult);
        SetThemeCursor( kThemeCrossCursor );
        
        // Bail out when the mouse is released
        if ( mouseResult == kMouseTrackingMouseReleased ){
            SetThemeCursor( kThemeArrowCursor );
            if(oma_wind[wn].windowtype != QBITMAP && oma_wind[wn].windowtype != QRGBMAP){
                // see if this a LINEGRAPHICS window
                if( oma_wind[wn].windowtype == QPIVGRAPH ) {
                    //HIViewFindByID(HIViewGetRoot(oma_wind[wn].gwind), kHIViewWindowContentID, &contentView);
                    //HIViewGetBounds(contentView, &dstRect);
                    //short w = dstRect.size.width;		// same as oma_wind[wn].width
                    //short h = dstRect.size.height;	// same as oma_wind[wn].height
                    short w = header[NCHAN];			// this is the right thing to use since substart is defined relative to this
                    short h = header[NTRAK];
                    base_vector( substart,w,h,alter_vectors);
                    break;
                }
                return;
            }
            if(oma_wind[wn].row_marker.window != -1){	// there is a row plot assicuated with this window
                // save a pdf
                vert_plot = 0;
                scrollwindow = oma_wind[oma_wind[wn].row_marker.window].gwind;
                dochanplot_q(-wn-1);
                //SetPortWindowPort(oma_wind[wn].gwind);
            }
            if(oma_wind[wn].col_marker.window != -1){	// there is a column plot assicuated with this window
                // save a pdf
                vert_plot = 1;
                scrollwindow = oma_wind[oma_wind[wn].col_marker.window].gwind;
                dochanplot_q(-wn-1);
                //SetPortWindowPort(oma_wind[wn].gwind);
            }
            
            HIViewSetNeedsDisplay(data->theView, true);
            
            break;
        }
        
        // Need to convert from global
        where = QDGlobalToHIViewLocal(qdPt, data->theView);
        if (toolselected == 0) qdPt = showcoords_q(where,0,wn);
        if(is_zoom_window){
            Get_zoom_rgb_from_image_buffer(USEQZOOM,qdPt);
        }
        if(is_zoom_rgb_window){
            Get_zoom_rgb_from_image_buffer(USEQRGBZOOM,qdPt);
        }
        if(is_fft_window){
            Get_fft_from_image_buffer(0,qdPt);
        }
        if(is_auto_window){
            Get_fft_from_image_buffer(0,qdPt);
        }
        if(oma_wind[wn].windowtype != QBITMAP && oma_wind[wn].windowtype != QRGBMAP) continue;
        
        // start the path at the first point
        if (data->thePath != NULL)
            CGPathRelease(data->thePath);
        data->thePath = CGPathCreateMutable();
        
        if(toolselected == 3 ||toolselected == 4) {
            // Draw the line
            CGPathMoveToPoint(data->thePath, &m, p1.x, p1.y);
            CGPathAddLineToPoint(data->thePath, &m, where.x, where.y);
            subend = showcoords_q(where,1,wn);
        }
        
        if(toolselected == 1 ||toolselected == 2) {
            // Draw the rectangle
            float box_height,box_width;
            where_box = where;
            if(aspect_ratio != 0.){
                box_height = where.y - p1.y;
                box_width = where.x - p1.x;
                if(box_height*box_width >= 0.0) //they have the same sign
                    where_box.x = p1.x + box_height*aspect_ratio;
                else
                    where_box.x = p1.x - box_height*aspect_ratio;
            }
            rectlines[0].x = rectlines[4].x = p1.x;
            rectlines[0].y = rectlines[4].y = where_box.y;
            rectlines[1].x = where_box.x;
            rectlines[1].y = where_box.y;
            rectlines[2].x = where_box.x;
            rectlines[2].y = p1.y;
            rectlines[3].x = p1.x;
            rectlines[3].y = p1.y;
            
            CGPathAddLines(data->thePath, &m, rectlines,5);
            subend = showcoords_q(where_box,1,wn);
            thereisabox = 1;
        }
        
        // now check to update column or row plots
        if( (oma_wind[wn].windowtype == QBITMAP || oma_wind[wn].windowtype == QRGBMAP)  && toolselected == 0 &&
           (oma_wind[wn].row_marker.window != -1 || oma_wind[wn].col_marker.window != -1)) {
            HIViewFindByID(HIViewGetRoot(FrontNonFloatingWindow()), kHIViewWindowContentID, &contentView);
            HIViewGetBounds(contentView, &dstRect);
            CGPathMoveToPoint(data->thePath, &m, where.x, where.y);
            if(oma_wind[wn].row_marker.window != -1){
                vert_plot = 0;
                scrollwindow = oma_wind[oma_wind[wn].row_marker.window].gwind;
                dochanplot_q(wn);
                //SetPortWindowPort(oma_wind[wn].gwind);
                CGPathAddLineToPoint(data->thePath, &m, 0.0, where.y);
                p1.x = dstRect.size.width;
                CGPathAddLineToPoint(data->thePath, &m, p1.x, where.y);
                CGPathAddLineToPoint(data->thePath, &m, where.x, where.y);
            }
            if(oma_wind[wn].col_marker.window != -1){
                vert_plot = 1;
                scrollwindow = oma_wind[oma_wind[wn].col_marker.window].gwind;
                dochanplot_q(wn);
                CGPathAddLineToPoint(data->thePath, &m, where.x, 0.0);
                where.y = dstRect.size.height;
                CGPathAddLineToPoint(data->thePath, &m, where.x, where.y);
                //SetPortWindowPort(oma_wind[wn].gwind);
            }
        }
        
        part = 0;
        SetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, sizeof(ControlPartCode), &part);
        
        /*
        if(is_zoom_window){
            needNewImage = 1;
        }
        */
        HIViewSetNeedsDisplay(data->theView, true);
    }
    
    if(docalcs) {				// this takes care of the calculate rectange, measure, and plot selected line cases
        pprintf("\n");
        calc(&substart,&subend);
        printf("OMA>");
    }
    //if(plotline)	no_draw = 1;
    
    // Send back the part upon which the mouse was released
    
    part = kControlEntireControl;
    SetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, sizeof(ControlPartCode), &part);
    
}

//-----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------
//	GWorldImageBufferRelease
// -----------------------------------------------------------------------------
//
void
GWorldImageBufferRelease(
                         void*						inInfo,
                         const void*					inData,
                         size_t						inSize )
{
#pragma unused( inData, inSize )
	//printf("image release %d\n",inSize);
    //without this, there is a memory leak
	if(inData!=0){
        DisposePtr( (Ptr) inData );
    }
}

CGImageRef LoadImageFromData(int width, int height, void* imagedata)
{
    CGImageRef image = NULL;
	CGDataProviderRef	provider;
	CGColorSpaceRef		colorspace;
	size_t			rowbytes;
	//void*			dataPtr;
	//CGContextRef context;
	
	int nth;
    if( pixsiz > 0 ){
		nth = 1;
	} else {
		nth = abs(pixsiz);
	}
	//width  /= nth;
	//height /= nth;
	
	rowbytes = width * 4;
	//dataPtr = malloc( height * rowbytes );
	colorspace = CGColorSpaceCreateDeviceRGB();
	//context = CGBitmapContextCreate(dataPtr, width, height,8, rowbytes , colorspace,kCGImageAlphaNoneSkipFirst);
	
	//provider = CGDataProviderCreateWithData( NULL,imagedata, height * rowbytes,
    //GWorldImageBufferRelease );
    
    provider = CGDataProviderCreateWithData( NULL,imagedata, height * rowbytes,GWorldImageBufferRelease );
    
	// create an image
	image = CGImageCreate( width, height, 8, 32, rowbytes, colorspace,
                          kCGImageAlphaNoneSkipFirst, provider, NULL, false, kCGRenderingIntentDefault );
	//CGBitmapContext
    
	//printf("image initialized: %d\n",sizeof(image));
	
	CGDataProviderRelease(provider);
    return image;
}

static OSStatus
MTViewHandler( EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData )
{
    
    OSStatus	    err = eventNotHandledErr;
    HIPoint	    where;
    ControlPartCode part;
    UInt32	    eventClass = GetEventClass( inEvent );
    UInt32	    eventKind = GetEventKind( inEvent );
    MTViewData*	    data = (MTViewData*)inUserData;		// pointless for the kEventHIObjectConstruct event
	//WindowRef theWindow;
	//HIObjectRef theObjectRef;
	int the_wind_no;
    
    
	
    
	
	extern int erase_from_command;
	
    switch ( eventClass )
    {
		case kEventClassHIObject:
			switch ( eventKind )
        {
            case kEventHIObjectConstruct:
            {
                data = (MTViewData*)malloc(sizeof(MTViewData));
                
                oma_wind[gwnum].view_data_ptr = (Ptr)data;
                
                err = GetEventParameter( inEvent, kEventParamHIObjectInstance, typeHIObjectRef, NULL, sizeof(HIObjectRef*), NULL, &data->theView );
                require_noerr( err, ParameterMissing );
                
                SetEventParameter( inEvent, kEventParamHIObjectInstance, typeVoidPtr, sizeof(MTViewData), &data );
            }
                break;
				
            case kEventHIObjectInitialize:
                err = CallNextEventHandler( inCallRef, inEvent );
                if ( err == noErr ) {
                    Rect bounds;
                    // Extract the bounds from the initialization event that we set up
                    err = GetEventParameter(inEvent, kCanvasBoundsParam, typeQDRectangle, NULL, sizeof(Rect), NULL, &bounds);
                    require_noerr(err, ParameterMissing);
                    SetControlBounds(data->theView, &bounds);
                    // Also initialize our MTViewData
                    //data->theImage = LoadImageFromData(header[NCHAN],header[NTRAK],oma_wind[gwnum].window_rgb_data);
                    if(abs(oma_wind[gwnum].windowtype) == LINEGRAPH || oma_wind[gwnum].windowtype == QPIVGRAPH )
                        data->theImage = NULL;
                    else
                        data->theImage = LoadImageFromData(oma_wind[gwnum].width,oma_wind[gwnum].height,oma_wind[gwnum].window_rgb_data);
                    data->thePath = NULL;
                }
                break;
				
            case kEventHIObjectDestruct:
                /*
                 err = GetEventParameter(inEvent,kEventParamWindowRef,typeWindowRef,NULL,sizeof(typeWindowRef),NULL,&theWindow);
                 err = GetEventParameter( inEvent, kEventParamHIObjectInstance, typeHIObjectRef, NULL, sizeof(HIObjectRef*), NULL, &theObjectRef );
                 HIViewRef view = HIViewGetSuperview(data->theView);
                 theWindow = HIViewGetWindow(view);
                 //theWindow = HIObjectGetEventTarget(data->theView);
                 */
                
                the_wind_no = viewgwnum((Ptr)data);
                //printf("destruct %d\n",viewgwnum((Ptr)data));
                
                CGImageRelease(data->theImage);
                if (data->thePath != NULL)
                    CGPathRelease(data->thePath);
                
                free(inUserData);
                
                CGContextRef ctx;
                EventParamType return_type;
                GetEventParameter( inEvent, kEventParamCGContextRef, typeCGContextRef, &return_type, sizeof(CGContextRef), NULL, &ctx );
                if (ctx != NULL && typeCGContextRef == return_type ){
                    CGContextRelease(ctx);
                    printf("release context\n");
                }
                if(!erase_from_command) {
                    closewindow(the_wind_no,1);
                }
                erase_from_command = 0;
                break;
        }
			break;	// kEventClassHIObject
            
            
		case kEventClassControl:
			switch ( eventKind )
        {
            case kEventControlDraw:
            {
                CGContextRef ctx;
                GetEventParameter( inEvent, kEventParamCGContextRef, typeCGContextRef, NULL, sizeof(CGContextRef), NULL, &ctx );
                
                //CallNextEventHandler(inCallRef, inEvent);	// Erase old content
                //printf("window %d\n",viewgwnum(data->theView));
                DrawTheMTView(ctx, (MTViewData*)inUserData);
                err = noErr;
            }
                break;
				
            case kEventControlHitTest:
                GetEventParameter(inEvent, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(HIPoint), NULL, &where);
                part = kControlContentMetaPart;
                SetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, sizeof(ControlPartCode), &part);
                /*
                 int wn;
                 wn = activegwnum(FrontWindow());
                 
                 UInt32 modifiers;
                 EventMouseButton button;
                 GetEventParameter(inEvent,kEventParamMouseButton,typeMouseButton,NULL,sizeof(typeMouseButton),NULL,&button);
                 GetEventParameter(inEvent,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(typeUInt32),NULL,&modifiers);
                 if(modifiers & controlKey || button == kEventMouseButtonSecondary){ // the right button
                 if(oma_wind[wn].opaque == 1.0)
                 oma_wind[wn].opaque = opaque_value;
                 else
                 oma_wind[wn].opaque = 1.0;
                 SetWindowAlpha (FrontWindow(),oma_wind[wn].opaque);
                 }
                 */
                err = noErr;
                break;
                /*
                 case kEventControlClick:		//kEventControlGetClickActivation: works first time but does things in the wrong window
                 {
                 beep();
                 int wn;
                 wn = activegwnum(FrontWindow());
                 
                 UInt32 modifiers;
                 EventMouseButton button;
                 err = GetEventParameter(inEvent,kEventParamMouseButton,typeMouseButton,NULL,sizeof(typeMouseButton),NULL,&button);
                 err = GetEventParameter(inEvent,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(typeUInt32),NULL,&modifiers);
                 if(modifiers & controlKey || button == kEventMouseButtonSecondary){ // the right button
                 if(oma_wind[wn].opaque == 1.0)
                 oma_wind[wn].opaque = opaque_value;
                 else
                 oma_wind[wn].opaque = 1.0;
                 //SetWindowAlpha (FrontWindow(),opaque[wn]);
                 }
                 err = noErr;
                 }
                 break;
                 */
            case kEventControlTrack:
                DoTheTracking(inEvent, data);
                break;
                
        }
			break;	// kEventClassControl
            /*
             case kEventClassWindow:
             
             switch ( eventKind )
             {
             case kEventWindowClose:
             err = GetEventParameter(inEvent,kEventParamWindowRef,typeWindowRef,NULL,sizeof(typeWindowRef),NULL,&theWindow);
             printf("close %d\n",activegwnum(theWindow));
             break;
             }
             break;	//	kEventClassWindow
             */
    }
    
    
ParameterMissing:
	return err;
}


static OSStatus MyMTViewRegister(CFStringRef myClassID)
{
    OSStatus                err = noErr;
    static HIObjectClassRef sMyViewClassRef = NULL;
	
    if ( sMyViewClassRef == NULL )
    {
        EventTypeSpec eventList[] = {
            
			
            
            { kEventClassHIObject, kEventHIObjectConstruct },
            { kEventClassHIObject, kEventHIObjectInitialize },
            { kEventClassHIObject, kEventHIObjectDestruct },
            
            { kEventClassControl, kEventControlDraw },
            { kEventClassControl, kEventControlHitTest },
			{ kEventClassControl, kEventControlTrack },
			
			//{ kEventClassControl, kEventControlClick },	// tried to use this to get action on first click in window
            // figure that out later
            
			//{ kEventClassWindow, kEventWindowClose },		// tried to use this to get the window for close bookkeeping
            // never got any event from it????
            
        };
		
        err = HIObjectRegisterSubclass( myClassID,
                                       kHIViewClassID,					// base class ID
                                       0,						// option bits
                                       MTViewHandler,					// construct proc
                                       GetEventTypeCount( eventList ),
                                       eventList,
                                       NULL,						// construct data
                                       &sMyViewClassRef );
    }
    return err;
}

OSStatus
CreateMouseTrackingView(HIViewRef parentView, const Rect* inBounds, HIViewID* inViewID)
{
#define kCanvasClassID	CFSTR( "org.oma.window.canvasview" )
    
    OSStatus	err;
    EventRef	event;
    HIViewRef	theView;
	
    // Register this class
    err = MyMTViewRegister(kCanvasClassID);
    require_noerr( err, CantRegister );
	
    // Make an initialization event
    err = CreateEvent( NULL, kEventClassHIObject, kEventHIObjectInitialize, GetCurrentEventTime(), 0, &event );
    require_noerr( err, CantCreateEvent );
    
    // If bounds were specified, push them into the initialization event
    // so that they can be used in the initialization handler.
    if ( inBounds != NULL )
    {
        err = SetEventParameter(event, kCanvasBoundsParam, typeQDRectangle, sizeof(Rect), inBounds);
        require_noerr( err, CantSetParameter );
    }
    err = HIObjectCreate(kCanvasClassID, event, (HIObjectRef*)&theView);
    require_noerr(err, CantCreate);
	
    if (parentView != NULL)
    {
        err = HIViewAddSubview(parentView, theView);
    }
    
    SetControlID(theView, inViewID);	// useful if a handler needs to call GetControlByID()
    HIViewSetVisible(theView, true);
	
CantCreate:
CantSetParameter:
CantCreateEvent:
    ReleaseEvent( event );
CantRegister:
    return err;
}
//-----------------------------------------------------------------------------------
static OSStatus ResizeHandler(EventHandlerCallRef inRef, EventRef inEvent, void* inUserData)
{
    WindowRef   w  = (WindowRef)inUserData;
    OSStatus	err = eventNotHandledErr;
    UInt32	attributes = 0;
    
    GetEventParameter(inEvent, kEventParamAttributes, typeUInt32, NULL, sizeof(UInt32), NULL, &attributes);
    
    if ( attributes & kWindowBoundsChangeSizeChanged )	// don't care about kWindowBoundsChangeOriginChanged
    {
        HIViewRef   contentView, canvasView;
        HIRect	    bounds;
        
        
        HIViewFindByID(HIViewGetRoot(w), kHIViewWindowContentID, &contentView);
        HIViewGetBounds(contentView, &bounds);
        err = GetControlByID(w, &mtViewID, &canvasView);
        if (err == noErr)
            HIViewSetFrame(canvasView, &bounds );
    }
    return err;
}


//-----------------------------------------------
Ptr Get_rgb_from_image_buffer(int allocate_new)
{
	Ptr ptr;
	DATAWORD* point = datpt+doffset;
	long k = 0, i,j,n=0;
	int ntrack = header[NTRAK];
	int nchan = header[NCHAN];
	int nth;
	float pix_scale;
    
	float fpindx;
	DATAWORD crange,cm,ncm1,indx;
	int pindx;
	
    
    
	crange = cmax - cmin;
	ncm1 = (ncolor-1);
	cm = cmin;
	
	if( pixsiz > 0 ){
		nth = 1;
		pix_scale = 1.0;
	} else {
		nth = abs(pixsiz);
		pix_scale=1.0/nth;
	}
	
	if(allocate_new)
		ptr = calloc(header[NCHAN]/nth*header[NTRAK]/nth,4);
	else{
		// try and reuse the same window, but be sure the size is the same
		if( oma_wind[gwnum-1].width == header[NCHAN]/nth &&
           oma_wind[gwnum-1].height == header[NTRAK]/nth) {
            ptr = oma_wind[gwnum-1].window_rgb_data;
		} else {
            // this is evidently freed later
            // or maybe not always -- trying to fix leaks
            if(oma_wind[gwnum-1].window_rgb_data != NULL){
            	free(oma_wind[gwnum-1].window_rgb_data);
                oma_wind[gwnum-1].window_rgb_data = NULL;
            }
            return NULL;
		}
        return ptr; // this will be called later, so don't do calc now
	}
	if(ptr == NULL){
		beep();
		printf("memory error\n");
		return ptr;
	}
	oma_wind[gwnum].width = header[NCHAN]/nth;
	oma_wind[gwnum].height = header[NTRAK]/nth;
	if( pixsiz > 0 ) {
		for(i=0; i < ntrack; i++){
			for(j=0; j < nchan; j++){
				indx = *(point+k++) - cm;
				fpindx = (float)indx * ncm1;
				pindx = fpindx/crange;
				if( pindx > ncm1)
					pindx = ncm1;
				if( pindx < 0)
					pindx = 0;
				++pindx;
				*(ptr+n++) =pindx;
				*(ptr+n++) = color[pindx][thepalette].red/256;
				*(ptr+n++) = color[pindx][thepalette].green/256;
				*(ptr+n++) = color[pindx][thepalette].blue/256;
			}
		}
	}else {
		i = 0;
		while(++i < ntrack/nth){
			j = 0;
			while( j++ < nchan/nth){
				indx = *(point+k) - cm;
				k += nth;
				fpindx = (float)indx * ncm1;
				pindx = fpindx/crange;
				if( pindx > ncm1)
					pindx = ncm1;
				if( pindx < 0)
					pindx = 0;
				++pindx;
				//*(ptr+n++) = 0xFF;
				*(ptr+n++) =pindx;
				*(ptr+n++) = color[pindx][thepalette].red/256;
				*(ptr+n++) = color[pindx][thepalette].green/256;
				*(ptr+n++) = color[pindx][thepalette].blue/256;
			}
            k = i * nth * nchan;
		}
	}
    
    
	return ptr;
    
}
//__________________________________________________________________________________
int trim_name(char* lastname)
{
	int length,i,j;
	length = strlen(lastname);
	for(i=length-1; i>0; i--){
		if(lastname[i] == '/')
			break;
	}
	if(i <= 2) return 0;	// nothing to be done here -- the name is short anyway
	strcpy(lastname, "...");
	for(j=0; j<length-i;j++){
		lastname[j+3] = lastname[j+i+1];
	}
	return 0;
}



int dquartz(int n,int index)
{
    extern char txt[];
	OSStatus	err=0;
	int			width;
	int			height;
	HIViewID    mtViewID = { kMTViewSignature, 0 };
	HIViewRef myView;
	
	if(display_is_off) return 0;
	
	if(autoscale) {
		if(!have_max) find_minmax();
		cmin = min;
		cmax = max;
		update_status();
	}
	
	if(lastname[0] == 0)
		strcpy(lastname,"Graphics");
    
	width = header[NCHAN];
	height = header[NTRAK];
	
    
	if(newwindowflag || gwnum == 0) {			// if no graphics window there, always open one
		chooseposn(width,height,pixsiz);
        
		if(index) {
			strcpy(txt,&cmnd[index]);		// save a copy of the name for labeling
		}else {
		    strcpy(txt,lastname);
		}//		graphics windows
		trim_name(txt);
		oma_wind[gwnum].window_rgb_data = Get_rgb_from_image_buffer(1);
        
		//create a blank window
		CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes | kWindowCompositingAttribute
						| kWindowStandardHandlerAttribute,&r,&oma_wind[gwnum].gwind);
		
		SetWindowTitleWithCFString(oma_wind[gwnum].gwind,CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingASCII));
		orgx += swidth;
		//oma_wind[gwnum].windowtype = QBITMAP;
		set_window_type(gwnum,QBITMAP);
		oma_wind[gwnum].opaque = 1.0;
	} else {
		if(oma_wind[gwnum-1].windowtype != QBITMAP){
			beep();
			printf("Can't auto-update window.\n");
			return -1;
		}
		
		// check to see if the current window is the right size
		if(Get_rgb_from_image_buffer(0) == NULL){
			// It's not the right size, so we have to start over
			closewindow(gwnum-1,0);
			chooseposn(width,height,pixsiz);
			oma_wind[gwnum].window_rgb_data = Get_rgb_from_image_buffer(1);
			//create a blank window
			CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes | kWindowCompositingAttribute
							| kWindowStandardHandlerAttribute,&r,&oma_wind[gwnum].gwind);
			SetWindowTitleWithCFString(oma_wind[gwnum].gwind,CFStringCreateWithCString(kCFAllocatorDefault,lastname,kCFStringEncodingASCII));
			orgx += swidth;
			//oma_wind[gwnum].windowtype = QBITMAP;
			set_window_type(gwnum,QBITMAP);
			oma_wind[gwnum].opaque = 1.0;
		} else {
			err = HIViewFindByID( HIViewGetRoot(oma_wind[gwnum-1].gwind), mtViewID, &myView);
            needNewImage = 1;
			err = HIViewSetNeedsDisplay(myView,true);
            
			return err;
		}
	}
    
	HIViewRef contentView;
	err = HIViewFindByID( HIViewGetRoot(oma_wind[gwnum].gwind), kHIViewWindowContentID, &contentView );
    require_noerr( err, CantFindView );
    
    Rect	viewBounds;
    GetWindowPortBounds(oma_wind[gwnum].gwind, &viewBounds);
    err = CreateMouseTrackingView(contentView, &viewBounds, &mtViewID);
    require_noerr( err, CantCreateMTView );
    
    // The window is resizeable, and we want the embedded view to get resized, too
    EventTypeSpec	event = {kEventClassWindow, kEventWindowBoundsChanged};
    err = InstallEventHandler(GetWindowEventTarget(oma_wind[gwnum].gwind), ResizeHandler, 1, &event, oma_wind[gwnum].gwind, NULL);
    require_noerr( err, CantInstallHandler );
	
	data_window = gwnum;
	ShowWindow(oma_wind[gwnum].gwind);
	gwnum++;
	
CantInstallHandler:
CantCreateMTView:
CantFindView:
    
	return err;
}
//__________________________________________________________________________________

int scale_pixval(DATAWORD val)
{
    extern DATAWORD cmin;
    extern DATAWORD crange;
    int pval;
    float fpval;
    
    fpval = (val-cmin) * 255.0;
    pval = fpval/crange;
    if( pval > 255)
        pval = 255;
    if( pval < 0)
        pval = 0;
    return pval;
}


Ptr Get_color_rgb_from_image_buffer(int allocate_new)
{
	Ptr ptr;
	DATAWORD* point = datpt+doffset;
	long k = 0, i,j,n=0;
	int ntrack = header[NTRAK];
	int nchan = header[NCHAN];
	int nth,intensity;
	float pix_scale;
	DATAWORD *pt_green,*pt_blue;
    
	//float fpindx;
	extern DATAWORD crange;
	extern float r_scale,g_scale,b_scale;
    extern float r_gamma,g_gamma,b_gamma;
    float r,g,b;
    extern DATAWORD rmax,gmax,bmax;
	//int pindx;
    
	crange = cmax - cmin;
	//ncm1 = (ncolor-1);
	//cm = cmin;
	
	if( pixsiz > 0 ){
		nth = 1;
		pix_scale = 1.0;
	} else {
		nth = abs(pixsiz);
		pix_scale=1.0/nth;
	}
	
	if(allocate_new)
		ptr = calloc(header[NCHAN]/nth*header[NTRAK]/nth/3,4);
	else{
		// try and reuse the same window, but be sure the size is the same
		if( oma_wind[gwnum-1].width == header[NCHAN]/nth &&
           oma_wind[gwnum-1].height == header[NTRAK]/nth/3) {
            ptr = oma_wind[gwnum-1].window_rgb_data;
		} else {
            // this is evidently freed later
            // or maybe not always -- trying to fix leaks
            if(oma_wind[gwnum-1].window_rgb_data != NULL){
            	free(oma_wind[gwnum-1].window_rgb_data);
                oma_wind[gwnum-1].window_rgb_data = NULL;
            }
            return NULL;
		}
        return ptr; // this will be called later, so don't do calc now
	}
	if(ptr == NULL){
		beep();
		printf("memory error\n");
		return ptr;
	}
	oma_wind[gwnum].width = header[NCHAN]/nth;
	oma_wind[gwnum].height = header[NTRAK]/nth/3;
	pt_green = point + nchan*ntrack/3;
	pt_blue =  pt_green + nchan*ntrack/3;
    
	if( pixsiz > 0 ) {
		for(i=0; i < ntrack/3; i++){
			for(j=0; j < nchan; j++){
                if (r_gamma != 1.) {
                    r = *(point+k)/rmax;
                    *(ptr+n+1) = scale_pixval(rmax*r_scale*powf(r,1./r_gamma));
                } else {
                    *(ptr+n+1) = scale_pixval(*(point+k)*r_scale);
                }
                if (g_gamma != 1.) {
                    g = *(pt_green+k)/gmax;
                    *(ptr+n+2) = scale_pixval(gmax*g_scale*powf(g,1./g_gamma));
                } else {
                    *(ptr+n+2) = scale_pixval(*(pt_green+k)*g_scale);
                }
                if (b_gamma != 1.) {
                    b = *(pt_blue+k++)/bmax;
                    *(ptr+n+3) = scale_pixval(bmax*b_scale*powf(b,1./b_gamma));
                } else {
                    *(ptr+n+3) = scale_pixval(*(pt_blue+k++)*b_scale);
                }
				intensity = ( (unsigned char) *(ptr+n+1) + (unsigned char) *(ptr+n+2) + (unsigned char) *(ptr+n+3))/3;
				*(ptr+n) = intensity;
				n += 4;
			}
		}
	}else {
		i = 0;
		while(++i < ntrack/nth/3){
			j = 0;
			while( j++ < nchan/nth){
                if (r_gamma != 1.) {
                    r = *(point+k)/rmax;
                    *(ptr+n+1) = scale_pixval(rmax*r_scale*powf(r,1./r_gamma));
                } else {
                    *(ptr+n+1) = scale_pixval(*(point+k)*r_scale);
                }
                if (g_gamma != 1.) {
                    g = *(pt_green+k)/gmax;
                    *(ptr+n+2) = scale_pixval(gmax*g_scale*powf(g,1./g_gamma));
                } else {
                    *(ptr+n+2) = scale_pixval(*(pt_green+k)*g_scale);
                }
                if (b_gamma != 1.) {
                    b = *(pt_blue+k)/bmax;
                    *(ptr+n+3) = scale_pixval(bmax*b_scale*powf(b,1./b_gamma));
                } else {
                    *(ptr+n+3) = scale_pixval(*(pt_blue+k)*b_scale);
                }
				intensity = ( (unsigned char) *(ptr+n+1) + (unsigned char) *(ptr+n+2) + (unsigned char) *(ptr+n+3))/3;
				*(ptr+n) = intensity;
				k += nth;
				n += 4;
			}
			k = i * nth * nchan;
		}
	}
	return ptr;
}

// ---------------------------------------------------------------------------//
int drgbq(int n,int index)
{
    
	OSStatus	err=0;
	int			width;
	int			height;
	HIViewID    mtViewID = { kMTViewSignature, 0 };
	HIViewRef myView;
	
	if(display_is_off) return 0;
	
	if(autoscale) {
		if(!have_max) find_minmax();
		cmin = min;
		cmax = max;
		update_status();
	}
	
	if(lastname[0] == 0)
		strcpy(lastname,"Graphics");
    
	width = header[NCHAN];
	height = header[NTRAK];
	
	if(newwindowflag || gwnum == 0) {		/* if no graphics window there, always open one */
		chooseposn(width,height/3,pixsiz);
        
		if(index) {
			strcpy(lastname,&cmnd[index]);		// save a copy of the name for labeling
		}					  	   				//		graphics windows
		trim_name(lastname);
		oma_wind[gwnum].window_rgb_data = Get_color_rgb_from_image_buffer(1);
		//oma_wind[gwnum].windowtype = RGBMAP;
		set_window_type(gwnum,QRGBMAP);
		//create a blank window
		CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes | kWindowCompositingAttribute
						| kWindowStandardHandlerAttribute,&r,&oma_wind[gwnum].gwind);
		
		SetWindowTitleWithCFString(oma_wind[gwnum].gwind,CFStringCreateWithCString(kCFAllocatorDefault,lastname,kCFStringEncodingASCII));
		orgx += swidth;
        
		oma_wind[gwnum].opaque = 1.0;
	} else {
		if(oma_wind[gwnum-1].windowtype != QRGBMAP){
			beep();
			printf("Can't auto-update window.\n");
			return -1;
		}
		
		// check to see if the current window is the right size
		if(Get_color_rgb_from_image_buffer(0) == NULL){
			// It's not the right size, so we have to start over
			closewindow(gwnum-1,0);
			chooseposn(width,height/3,pixsiz);
			oma_wind[gwnum].window_rgb_data = Get_color_rgb_from_image_buffer(1);
			//create a blank window
			CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes | kWindowCompositingAttribute
							| kWindowStandardHandlerAttribute,&r,&oma_wind[gwnum].gwind);
			SetWindowTitleWithCFString(oma_wind[gwnum].gwind,CFStringCreateWithCString(kCFAllocatorDefault,lastname,kCFStringEncodingASCII));
			orgx += swidth;
			//oma_wind[gwnum].windowtype = RGBMAP;
			set_window_type(gwnum,QRGBMAP);
			oma_wind[gwnum].opaque = 1.0;
		} else {
			err = HIViewFindByID( HIViewGetRoot(oma_wind[gwnum-1].gwind), mtViewID, &myView);
            needNewImage = 1;
			err = HIViewSetNeedsDisplay(myView,true);
			return err;
		}
	}
    
	HIViewRef contentView;
	err = HIViewFindByID( HIViewGetRoot(oma_wind[gwnum].gwind), kHIViewWindowContentID, &contentView );
    //require_noerr( err, troubles );
    
    Rect	viewBounds;
    GetWindowPortBounds(oma_wind[gwnum].gwind, &viewBounds);
    err = CreateMouseTrackingView(contentView, &viewBounds, &mtViewID);
    //require_noerr( err, CantCreateMTView );
    
    // The window is resizeable, and we want the embedded view to get resized, too
    EventTypeSpec	event = {kEventClassWindow, kEventWindowBoundsChanged};
    err = InstallEventHandler(GetWindowEventTarget(oma_wind[gwnum].gwind), ResizeHandler, 1, &event, oma_wind[gwnum].gwind, NULL);
    require_noerr( err, CantInstallHandler );
	
	data_window = gwnum;
	ShowWindow(oma_wind[gwnum].gwind);
	gwnum++;
	
CantInstallHandler:
	return err;
}

// ---------------------------------------------------------------------------//

Ptr Get_zoom_rgb_from_image_buffer(int allocate_new, Point zoom_point)
{
	
	Ptr ptr=0;
	DATAWORD *point, *pointg, *pointb;
	long i,j,n=0;
	//int ntrack = header[NTRAK];
	//int nchan = header[NCHAN];
	int gw=0;
	//float pix_scale;
    
	float fpindx;
	DATAWORD crange,cm,ncm1,indx;
	int pindx;
	
	extern short zheight,zwidth;
    
    
	crange = cmax-cmin;
	ncm1 = (ncolor-1);
	cm = cmin;
    
	i = zoom_point.v - zheight/2;
	if( i<0) i = 0;
	if( i+zheight >= header[NTRAK] ) i = header[NTRAK] - zheight - 1;
	j = zoom_point.h - zwidth/2;
	if( j<0) j = 0;
	if( j+zwidth >= header[NCHAN] ) j = header[NCHAN] - zwidth - 1;
	
	// printf("Start at %d %d.\n",j,i);
	
	if(allocate_new == ALLOCATE || allocate_new == ALLOCATERGB){
		ptr = calloc(zwidth*zheight,4);			// zoom window size
		oma_wind[gwnum].width = zwidth;
		
		oma_wind[gwnum].height = zheight;
	} else if(allocate_new == USEQZOOM) {
		for( gw=0; gw<= gwnum; gw++){
			if( oma_wind[gw].windowtype == QZOOM)
				break;
		}
		// check here for actually found the zoom?
		ptr = oma_wind[gw].window_rgb_data;			// the zoom window number
	} else if(allocate_new == USEQRGBZOOM) {
		for( gw=0; gw<= gwnum; gw++){
			if( oma_wind[gw].windowtype == QRGBZOOM)
				break;
		}
		// check here for actually found the zoom?
		ptr = oma_wind[gw].window_rgb_data;			// the zoom window number
	}
	
    
	if(ptr == NULL){
		beep();
		printf("memory error\n");
		return ptr;
	}
    
    // this is from drag event -- don't calculate now
    MTViewData* myViewptr;
	if(allocate_new != ALLOCATE && allocate_new != ALLOCATERGB){ 
		myViewptr = (MTViewData*) oma_wind[gw].view_data_ptr;
        needNewImage = 1;
		HIViewSetNeedsDisplay(myViewptr->theView, true);
        
	}

	
	if(allocate_new == ALLOCATE || allocate_new == USEQZOOM){   // zoom
		point = datpt+doffset + i*header[NCHAN] + j;
		for(i=1; i <= zheight; i++){
			for(j=1; j <= zwidth; j++){
				indx = *(point++) - cm;
				fpindx = (float)indx * ncm1;
				pindx = fpindx/crange;
				if( pindx > ncm1)
					pindx = ncm1;
				if( pindx < 0)
					pindx = 0;
				++pindx;
				*(ptr+n++) = pindx;
				*(ptr+n++) = color[pindx][thepalette].red/256;
				*(ptr+n++) = color[pindx][thepalette].green/256;
				*(ptr+n++) = color[pindx][thepalette].blue/256;
			}
			point += header[NCHAN] - j + 1;
		}
	} else {    // zoomRGB
		//printf("%d %d %d %d\n",header[NCHAN],header[NTRAK],zwidth,zheight);
		//printf("%d %d\n",i,j);
		point = datpt+doffset + i*header[NCHAN] + j;
		pointg = point + header[NCHAN]*header[NTRAK]/3;
		pointb = pointg + header[NCHAN]*header[NTRAK]/3;
		for(i=1; i <= zheight; i++){
			for(j=1; j <= zwidth; j++){
				// red
				indx = *(point++) - cm;
				fpindx = (float)indx * ncm1;
				pindx = fpindx/crange;
				if( pindx > ncm1)
					pindx = ncm1;
				if( pindx < 0)
					pindx = 0;
				++pindx;
				*(ptr+n++) = pindx;	// just store the red here for now
				*(ptr+n++) = pindx;
				// green
				indx = *(pointg++) - cm;
				fpindx = (float)indx * ncm1;
				pindx = fpindx/crange;
				if( pindx > ncm1)
					pindx = ncm1;
				if( pindx < 0)
					pindx = 0;
				++pindx;
				*(ptr+n++) = pindx;
				// blue
				indx = *(pointb++) - cm;
				fpindx = (float)indx * ncm1;
				pindx = fpindx/crange;
				if( pindx > ncm1)
					pindx = ncm1;
				if( pindx < 0)
					pindx = 0;
				++pindx;
				*(ptr+n++) = pindx;
			}
			point += header[NCHAN] - j + 1;
			pointg += header[NCHAN] - j + 1;
			pointb += header[NCHAN] - j + 1;
		}
	}
	
	
	return ptr;
    
}


/* ************* */

int dzoom_q(int n)
{
	extern short zwidth,zheight,toolselected;
	OSErr err;
	Point zp;
	
	if( is_zoom_window ){
		beep();
		printf("Zoom Window Already Exists.\n");
		return -1;
	}
	
	if( n<= 0) n = 1;
	
	zheight = (subend.v - substart.v);
	zwidth = (subend.h - substart.h);
	zp.h = header[NCHAN]/2;
	zp.v = header[NTRAK]/2;
	
	if (zheight <= 4) zheight = 32;
	if (zwidth <= 4) zwidth = 32;
	
	printf("%d by %d. %d pixsiz.\n",zwidth,zheight,n);
	
	chooseposn(zwidth,zheight,n);
	
	//if(!openwindow(zwidth,zheight,"Zoom",documentProc)) return -1;
	//create a blank window
	oma_wind[gwnum].window_rgb_data = Get_zoom_rgb_from_image_buffer(ALLOCATE,zp);
	
	CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes | kWindowCompositingAttribute
					| kWindowStandardHandlerAttribute,&r,&oma_wind[gwnum].gwind);
	
	SetWindowTitleWithCFString(oma_wind[gwnum].gwind,CFStringCreateWithCString(kCFAllocatorDefault,"Zoom",kCFStringEncodingASCII));
	orgx += swidth;
	//oma_wind[gwnum].windowtype = QZOOM;
	set_window_type(gwnum,QZOOM);
	oma_wind[gwnum].opaque = 1.0;
    
	is_zoom_window = 1;
    
	HIViewRef contentView;
	err = HIViewFindByID( HIViewGetRoot(oma_wind[gwnum].gwind), kHIViewWindowContentID, &contentView );
    //require_noerr( err, troubles );
    
    Rect	viewBounds;
    GetWindowPortBounds(oma_wind[gwnum].gwind, &viewBounds);
    err = CreateMouseTrackingView(contentView, &viewBounds, &mtViewID);
    //require_noerr( err, CantCreateMTView );
    
    // The window is resizeable, and we want the embedded view to get resized, too
    EventTypeSpec	event = {kEventClassWindow, kEventWindowBoundsChanged};
    err = InstallEventHandler(GetWindowEventTarget(oma_wind[gwnum].gwind), ResizeHandler, 1, &event, oma_wind[gwnum].gwind, NULL);
    //require_noerr( err, CantInstallHandler );
	
	data_window = gwnum;
	ShowWindow(oma_wind[gwnum].gwind);
	gwnum++;
    
    //
	toolselected = 0;
	docalcs = false;
	showselection = false;
	showruler = false;
	plotline = false;
	update_status();
    //
	return 0;
}

/* ************* */

int dzoomrgb_q(int n)
{
	extern short zwidth,zheight,toolselected;
	OSErr err;
	Point zp;
	
	if( is_zoom_rgb_window ){
		beep();
		printf("Zoom RGB Window Already Exists.\n");
		return -1;
	}
	
	if( n<= 0) n = 1;
	
	zheight = (subend.v - substart.v);
	zwidth = (subend.h - substart.h);
	zp.h = header[NCHAN]/2;
	zp.v = header[NTRAK]/3/2;
	
	if (zheight <= 4) zheight = 32;
	if (zwidth <= 4) zwidth = 32;
	
	printf("%d by %d. %d pixsiz.\n",zwidth,zheight,n);
	
	chooseposn(zwidth,zheight,n);
	
	//if(!openwindow(zwidth,zheight,"RGB Zoom",documentProc)) return -1;
	//create a blank window
	oma_wind[gwnum].window_rgb_data = Get_zoom_rgb_from_image_buffer(ALLOCATERGB,zp);
	
	CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes | kWindowCompositingAttribute
					| kWindowStandardHandlerAttribute,&r,&oma_wind[gwnum].gwind);
	
	SetWindowTitleWithCFString(oma_wind[gwnum].gwind,CFStringCreateWithCString(kCFAllocatorDefault,"RGB Zoom",kCFStringEncodingASCII));
	orgx += swidth;
	//oma_wind[gwnum].windowtype = QZOOM;
	set_window_type(gwnum,QRGBZOOM);
	oma_wind[gwnum].opaque = 1.0;
	
	is_zoom_rgb_window = 1;
	
	HIViewRef contentView;
	err = HIViewFindByID( HIViewGetRoot(oma_wind[gwnum].gwind), kHIViewWindowContentID, &contentView );
    //require_noerr( err, troubles );
    
    Rect	viewBounds;
    GetWindowPortBounds(oma_wind[gwnum].gwind, &viewBounds);
    err = CreateMouseTrackingView(contentView, &viewBounds, &mtViewID);
    //require_noerr( err, CantCreateMTView );
	
    // The window is resizeable, and we want the embedded view to get resized, too
    EventTypeSpec	event = {kEventClassWindow, kEventWindowBoundsChanged};
    err = InstallEventHandler(GetWindowEventTarget(oma_wind[gwnum].gwind), ResizeHandler, 1, &event, oma_wind[gwnum].gwind, NULL);
    //require_noerr( err, CantInstallHandler );
	
	data_window = gwnum;
	ShowWindow(oma_wind[gwnum].gwind);
	gwnum++;
	
	//
	toolselected = 0;
	docalcs = false;
	showselection = false;
	showruler = false;
	plotline = false;
	update_status();
	//
	return 0;
}
/* ************* */

short fftwidth,fftheight;
int is_fft_window = 0;
int is_auto_window = 0;

extern int fftsize;

int dfft(int n)
{
	extern short fftwidth,fftheight;
	extern	int		gwnum;					/* the graphics window counter */
	extern	short	orgx,swidth;
	extern Rect 	r;
	extern short toolselected;
	extern int  showselection,showruler,docalcs,plotline,data_window;
	extern HIViewID    mtViewID;
	OSStatus ResizeHandler(EventHandlerCallRef, EventRef, void* );
	OSStatus CreateMouseTrackingView(HIViewRef, const Rect*, HIViewID*);
	
	OSErr err;
	Point zp;
    
	if( is_fft_window ){
		beep();
		printf("FFT Window Already Exists.\n");
		return -1;
	}
	
	if( n<= 0) n = 1;
	
	fftheight = fftwidth = fftsize;
	zp.h = header[NCHAN]/2;
	zp.v = header[NTRAK]/2;
	
	chooseposn(fftwidth,fftheight,n);
	is_fft_window = 1;
	
	//if(!openwindow(fftwidth,fftheight,"FFT",documentProc)) return -1;
	oma_wind[gwnum].window_rgb_data = Get_fft_from_image_buffer(1,zp);
    
	CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes | kWindowCompositingAttribute
					| kWindowStandardHandlerAttribute,&r,&oma_wind[gwnum].gwind);
	
	SetWindowTitleWithCFString(oma_wind[gwnum].gwind,CFStringCreateWithCString(kCFAllocatorDefault,"FFT",kCFStringEncodingASCII));
	orgx += swidth; 
	//oma_wind[gwnum].windowtype = QFFT;
	set_window_type(gwnum,QFFT);	
	oma_wind[gwnum].opaque = 1.0;
	
	
	
	HIViewRef contentView;
	err = HIViewFindByID( HIViewGetRoot(oma_wind[gwnum].gwind), kHIViewWindowContentID, &contentView );
    //require_noerr( err, troubles );
    
    Rect	viewBounds;
    GetWindowPortBounds(oma_wind[gwnum].gwind, &viewBounds);
    err = CreateMouseTrackingView(contentView, &viewBounds, &mtViewID);
    //require_noerr( err, CantCreateMTView );
    
    // The window is resizeable, and we want the embedded view to get resized, too
    EventTypeSpec	event = {kEventClassWindow, kEventWindowBoundsChanged};
    err = InstallEventHandler(GetWindowEventTarget(oma_wind[gwnum].gwind), ResizeHandler, 1, &event, oma_wind[gwnum].gwind, NULL);
    //require_noerr( err, CantInstallHandler );
	
	data_window = gwnum;
	ShowWindow(oma_wind[gwnum].gwind);
	
	gwnum++;
	toolselected = 0;
	docalcs = false;
	showselection = false;
	showruler = false;
	plotline = false;
	update_status();
	return 0;
}

int dautoc(int n)
{
	extern short fftwidth,fftheight;
	extern	int		gwnum;					/* the graphics window counter */
	extern	short	orgx,swidth;
	extern Rect 	r;
	extern short toolselected;
	extern int  showselection,showruler,docalcs,plotline,data_window;
	extern HIViewID    mtViewID;
	OSStatus ResizeHandler(EventHandlerCallRef, EventRef, void* );
	OSStatus CreateMouseTrackingView(HIViewRef, const Rect*, HIViewID*);
    
	OSErr err;
	Point zp;
    
	if( is_auto_window ){
		beep();
		printf("Autocorrelation Window Already Exists.\n");
		return -1;
	}
	
	if( n<= 0) n = 1;
	
	fftheight = fftwidth = fftsize;
	zp.h = header[NCHAN]/2;
	zp.v = header[NTRAK]/2;
    
	chooseposn(fftwidth,fftheight,n);
	is_auto_window = 1;
    
	//if(!openwindow(fftwidth,fftheight,"Auto",documentProc)) return -1;
	oma_wind[gwnum].window_rgb_data = Get_fft_from_image_buffer(2,zp);
    
	CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes | kWindowCompositingAttribute
					| kWindowStandardHandlerAttribute,&r,&oma_wind[gwnum].gwind);
	
	SetWindowTitleWithCFString(oma_wind[gwnum].gwind,CFStringCreateWithCString(kCFAllocatorDefault,"Auto",kCFStringEncodingASCII));
	orgx += swidth; 
	//oma_wind[gwnum].windowtype = QAUTO;	
	set_window_type(gwnum,QAUTO);
	oma_wind[gwnum].opaque = 1.0;
	
	
	HIViewRef contentView;
	err = HIViewFindByID( HIViewGetRoot(oma_wind[gwnum].gwind), kHIViewWindowContentID, &contentView );
    //require_noerr( err, troubles );
    
    Rect	viewBounds;
    GetWindowPortBounds(oma_wind[gwnum].gwind, &viewBounds);
    err = CreateMouseTrackingView(contentView, &viewBounds, &mtViewID);
    //require_noerr( err, CantCreateMTView );
    
    // The window is resizeable, and we want the embedded view to get resized, too
    EventTypeSpec	event = {kEventClassWindow, kEventWindowBoundsChanged};
    err = InstallEventHandler(GetWindowEventTarget(oma_wind[gwnum].gwind), ResizeHandler, 1, &event, oma_wind[gwnum].gwind, NULL);
    //require_noerr( err, CantInstallHandler );
	
	data_window = gwnum;
	ShowWindow(oma_wind[gwnum].gwind);
    
	gwnum++;
	toolselected = 0;
	docalcs = false;
	showselection = false;
	showruler = false;
	plotline = false;
	update_status();
	return 0;
}

