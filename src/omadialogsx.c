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

#include "omadialogsx.h"
#include "oma_quartz.h"


int printxyzstuff_nib(int ,int ,short );

int palette_tab[NUMPAL] = {3,8,5,6,7,1,2,4};    // these are the control values that correspond to the
                                                // value stored in thepalette


/*
 * For the Set Contours DLOG
 */
extern	int lgwidth;
extern	int lgheight;
extern	int nlevls;
extern	DATAWORD ctrmax;
extern	DATAWORD ctrmin;
extern	float clevls[];
extern	int datminmax;
extern	int stdscrnsize;
extern	int colorctrs;
extern	int noctrtyping;

/*
 * For the Set Surface DLOG
 */

extern	int			surfwidth;
extern	int			surfheight;
extern	int			plotwhite;
extern	int			incrementby;
extern	DATAWORD	surfmax;
extern	DATAWORD	surfmin;
extern	int			sdatminmax;
extern	int			sstdscrnsize;
extern	int			scolor;
extern	int			persp;

/*
 * For the Set Histogram DLOG
 */

extern	int			hautoscale;
extern	int			hstdscrnsize;
extern	DATAWORD	histmax;
extern	DATAWORD	histmin;
extern	int 		histwidth;
extern	int 		histheight;
extern	int			hclear;

/*
 * For the Line Out Template DLOG
 */

extern	int 	losumx[];	
Rect	loboxes[NUMBER_LINEOUTS];
extern	int     numboxes,alignstart,alignend;
extern	LOLabel	boxlabel[];
int		boxnum = 1;

/*
 * For the Star1 DLOG
 */

extern	int star_time;
extern	int star_treg;
extern	int star_auto;
extern	int star_gain;


extern	unsigned long exposure_time;
extern	unsigned long baud;
extern	unsigned short enable_dcs;
extern	unsigned short dc_restore;
extern	unsigned short abg_state;
extern	unsigned short abg_period;
extern	unsigned short dest_buffer;
extern	unsigned short auto_dark;
extern	unsigned short x2_horizontal;
extern	unsigned short open_shutter;
extern 	int	temp_control;
extern  int serial_port;
extern 	unsigned short head_offset;
extern	float set_temp;


/*
 * For the Status DLOG window
 */

WindowRef Status_window;
WindowRef RGB_Tools_window;

/*
 * For the Set Integrated Plot DLOG
 */

extern	int pdatminmax;
extern	int pstdscrnsize;
extern	DATAWORD ponemax;
extern	DATAWORD ponemin;
extern	int ponewidth;
extern	int poneheight;
extern	int pintegrate;
extern	int pintx;



/*
 * For the Set File Prefixes DLOG
 */

extern	char	saveprefixbuf[];		/* save data file prefix buffer */
extern	char	savesuffixbuf[];		/* save data file suffix buffer */
extern	char	getprefixbuf[];			/* get data file prefix buffer */
extern	char	getsuffixbuf[];			/* get data file suffix buffer */
extern	char	graphicsprefixbuf[];	/* graphics file prefix buffer */
extern	char	graphicssuffixbuf[];	/* graphics file suffix buffer */
extern	char	macroprefixbuf[];		/* macro file prefix buffer */
extern	char	macrosuffixbuf[];		/* macro file suffix buffer */
int loadsettings = 0;

/*
 *
 */
 
 
extern	DATAWORD	min,max;
extern	TWOBYTE		header[];
extern	short		pixsiz;
//extern	GWorldPtr	myGWorldPtrs[]; // offscreen maps

extern OMA_Window	oma_wind[];
 
char	txt[CHPERLN];	/* a general purpose text array */
Str255  pstring; /* a pascal general purpose text array */
char 		curname[CHPERLN];
short 		curvol;


// my version of old routines
 
void setdialogitemtext(Handle ,char* );
void getdialogitemtext(Handle ,char* );

int printf(),pprintf(),drgb(),set_INTEGRATED_Pane_Values(),set_CONTOUR_Pane_Values(),set_HISTOGRAM_Pane_Values(),set_SURFACE_Pane_Values(),
	set_PREFIX_Pane_Values(),set_PIV2_Pane_Values(),set_PIV_Pane_Values(),is_power_2(),do_preference_tab_number(),
	remember_CONTOUR_Pane_Values(),remember_HISTOGRAM_Pane_Values(),remember_SURFACE_Pane_Values(),do_autoupdate(),
	remember_PREFIX_Pane_Values(),remember_PIV2_Pane_Values(),remember_PIV_Pane_Values(),get_PIV_Values(),remember_INTEGRATED_Pane_Values();

#ifdef SensiCam
/****************************************************************************/
/* The SensiCam Dialog */

extern SInt32		timeTable[];
extern UInt32		timeCount;
extern int SensiCamTrigNone;
extern int SensiCamTrigRising;
extern int SensiCamTrigFalling;

extern int		SensiCamStandard;
extern int		SensiCamDoubleShort;
extern int		SensiCamDoubleLong;

extern int		SensiCamCopyDelay;
extern int		SensiCamCopyExposure;
extern  SensiCamRef	theSensiCam;


void SensiCamStatusDialog()
{
	DialogPtr	theDialog;
	short		itemType;
	Handle		itemHdl;
	Rect		itemRect;
	short		itemHit;
	int 		elect,chip;
	unsigned int mode,trig;
	
	
	
	CGrafPtr 		savePort;
	GDHandle		saveDevice;
	
	
	GetGWorld(&savePort,&saveDevice);

	theDialog = GetNewDialog(SensiCamStatusDLOG, nil, (WindowPtr) -1);
	
	SetPortWindowPort(GetDialogWindow(theDialog));
	
	if(SensiCamGetStatus(&chip, &elect, &mode, &trig) != 0){
		chip=mode=trig=elect=-1;
	}
	
	sprintf(txt,"Chip Temperature:  %d",chip);
	GetDialogItem(theDialog, SensiCamTemp1Item, &itemType, &itemHdl, &itemRect);
	setdialogitemtext(itemHdl, txt);

	sprintf(txt,"Electronics Termperature:  %d",elect);
	GetDialogItem(theDialog, SensiCamTemp2Item, &itemType, &itemHdl, &itemRect);
	setdialogitemtext(itemHdl, txt);

	sprintf(txt,"Operating Mode:  %x",mode);
	GetDialogItem(theDialog, SensiCamModeItem, &itemType, &itemHdl, &itemRect);
	setdialogitemtext(itemHdl, txt);

	sprintf(txt,"Trigger Mode:  %x",trig);
	GetDialogItem(theDialog, SensiCamTrigItem, &itemType, &itemHdl, &itemRect);
	setdialogitemtext(itemHdl, txt);
	
	for (;;){
		ModalDialog(nil, &itemHit); 
		if (itemHit == okButton) break;
	}	
	
	DisposeDialog(theDialog);
	SetGWorld(savePort,saveDevice);
	return;

}

void SensiCamSettingsDialog()
{
	DialogPtr	theDialog;
	short		itemType;
	Handle		itemHdl;
	Rect		itemRect;
	short		itemHit;
	int 		i;
	OSErr		error = noErr;
	
	UInt32		current_timeCount;
	
	
	CGrafPtr 		savePort;
	GDHandle		saveDevice;
	
	
	GetGWorld(&savePort,&saveDevice);

	theDialog = GetNewDialog(SensiCamDLOG, nil, (WindowPtr) -1);
	
	SetPortWindowPort(GetDialogWindow(theDialog));
	
	/* 
	// this seems to give errors -- not sure why so I took it out
	error = SensiCam_GetTiming(theSensiCam, timeTable, &current_timeCount);	
	if(error != noErr) {
		beep();
		printf("Timing Error %d\n",error);
	}
	if(current_timeCount != timeCount) {
		beep();
		printf("Only %d pairs read\n",current_timeCount/2);
	}
	//return(error);
	*/
	
	// Fill in values for delay and exposure
	for(i=0; i<timeCount; i++){
		sprintf(txt,"%d",timeTable[i]);
		GetDialogItem(theDialog, SensiCamDel1Item+i, &itemType, &itemHdl, &itemRect);
		setdialogitemtext(itemHdl, txt);
	}
	
	
	// The Radio Buttons
	if(SensiCamDoubleShort){
		setbuttons(theDialog,true,SensiCamDoubleShortItem,SensiCamDoubleLongItem);
		setbuttons(theDialog,true,SensiCamDoubleShortItem,SensiCamStandardItem);
	}

	if(SensiCamDoubleLong){
		setbuttons(theDialog,true,SensiCamDoubleLongItem,SensiCamDoubleShortItem);
		setbuttons(theDialog,true,SensiCamDoubleLongItem,SensiCamStandardItem);
	}
	if(SensiCamStandard){
		setbuttons(theDialog,true,SensiCamStandardItem,SensiCamDoubleLongItem);
		setbuttons(theDialog,true,SensiCamStandardItem,SensiCamDoubleShortItem);
	}

	if(SensiCamTrigNone){
			setbuttons(theDialog,true,SensiCamNoneItem,SensiCamRisingItem);
			setbuttons(theDialog,true,SensiCamNoneItem,SensiCamFallingItem);
	}
	if(SensiCamTrigRising){
			setbuttons(theDialog,true,SensiCamRisingItem,SensiCamNoneItem);
			setbuttons(theDialog,true,SensiCamRisingItem,SensiCamFallingItem);
	}
	if(SensiCamTrigFalling){
			setbuttons(theDialog,true,SensiCamFallingItem,SensiCamRisingItem);
			setbuttons(theDialog,true,SensiCamFallingItem,SensiCamNoneItem);
	}
	
	// Check Boxes
	setcheckbox(theDialog,SensiCamCopyExposure,SensiCamCopyExposureItem);
	setcheckbox(theDialog,SensiCamCopyDelay,SensiCamCopyDelayItem);

	
	// Process Dialog Events
	for (;;){
		ModalDialog(nil, &itemHit); 
		if (itemHit == okButton) break;
		
		if (itemHit >= SensiCamDel1Item && itemHit <= SensiCamTime10Item) { // set the delay
			// This is the "fill all delays according to first" case
			if(itemHit == SensiCamDel1Item && SensiCamCopyDelay){
				GetDialogItem(theDialog, itemHit, &itemType, &itemHdl, &itemRect);
				getdialogitemtext(itemHdl,txt);
				timeTable[0] =  atoi(txt);
				for(i=2; i<timeCount; i+=2){
					timeTable[i] = timeTable[0];
					sprintf(txt,"%d",timeTable[i]);
					GetDialogItem(theDialog, SensiCamDel1Item+i, &itemType, &itemHdl, &itemRect);
					setdialogitemtext(itemHdl, txt);
				}
			}
			// This is the "fill all exposures according to first" case
			if(itemHit == SensiCamTime1Item && SensiCamCopyExposure){
				GetDialogItem(theDialog, itemHit, &itemType, &itemHdl, &itemRect);
				getdialogitemtext(itemHdl,txt);
				timeTable[1] =  atoi(txt);
				for(i=3; i<timeCount ; i+=2){
					timeTable[i] = timeTable[1];
					sprintf(txt,"%d",timeTable[i]);
					GetDialogItem(theDialog, SensiCamDel1Item+i, &itemType, &itemHdl, &itemRect);
					setdialogitemtext(itemHdl, txt);
				}
			}
			
			GetDialogItem(theDialog, itemHit, &itemType, &itemHdl, &itemRect);
			getdialogitemtext(itemHdl,txt);
			
			if((itemHit&1) == (SensiCamDel1Item&1)){
				// this is in the delay colum
				if(itemHit>SensiCamDel1Item && SensiCamCopyDelay){
					// This is a mistake -- the "Copy First" option is selected and the
					// bozo is trying to set a value other than the first
					// set the dialog value back to its proper value
					sprintf(txt,"%d",timeTable[0]);
					setdialogitemtext(itemHdl, txt);
				} else {
					// remember the value
					timeTable[itemHit-SensiCamDel1Item] =  atoi(txt); 
				}
			} else {
				// this is in the exposure colum
				if(itemHit>SensiCamTime1Item && SensiCamCopyExposure){
					// This is a mistake -- the "Copy First" option is selected and the
					// bozo is trying to set a value other than the first
					// set the dialog value back to its proper value
					sprintf(txt,"%d",timeTable[1]);
					setdialogitemtext(itemHdl, txt);
				} else {
					// remember the value
					timeTable[itemHit-SensiCamDel1Item] =  atoi(txt); 
				}
			
			}
				
		
		}

		
		switch(itemHit) {
		// Trigger Radio Buttons
		case SensiCamNoneItem:
			SensiCamTrigNone = true;
			SensiCamTrigRising = false;
			SensiCamTrigFalling = false;
			
			setbuttons(theDialog,true,SensiCamNoneItem,SensiCamRisingItem);
			setbuttons(theDialog,true,SensiCamNoneItem,SensiCamFallingItem);
			break;
		case SensiCamRisingItem:
			SensiCamTrigNone = false;
			SensiCamTrigRising = true;
			SensiCamTrigFalling = false;
			
			setbuttons(theDialog,false,SensiCamNoneItem,SensiCamRisingItem);
			setbuttons(theDialog,true,SensiCamRisingItem,SensiCamFallingItem);
			break;
		case SensiCamFallingItem:
			SensiCamTrigNone = false;
			SensiCamTrigRising = false;
			SensiCamTrigFalling = true;
			
			setbuttons(theDialog,true,SensiCamFallingItem,SensiCamRisingItem);
			setbuttons(theDialog,false,SensiCamNoneItem,SensiCamFallingItem);
			break;
		
		// Exposure Radio Buttons
		case SensiCamStandardItem:
			SensiCamStandard = true;
			SensiCamDoubleShort = false;
			SensiCamDoubleLong = false;
			
			setbuttons(theDialog,true,SensiCamStandardItem,SensiCamDoubleShortItem);
			setbuttons(theDialog,true,SensiCamStandardItem,SensiCamDoubleLongItem);
			break;
		case SensiCamDoubleShortItem:
			SensiCamStandard = false;
			SensiCamDoubleShort = true;
			SensiCamDoubleLong = false;
			
			setbuttons(theDialog,false,SensiCamStandardItem,SensiCamDoubleShortItem);
			setbuttons(theDialog,true,SensiCamDoubleShortItem,SensiCamDoubleLongItem);
			break;
		case SensiCamDoubleLongItem:
			SensiCamStandard = false;
			SensiCamDoubleShort = false;
			SensiCamDoubleLong = true;
			
			setbuttons(theDialog,false,SensiCamStandardItem,SensiCamDoubleLongItem);
			setbuttons(theDialog,false,SensiCamDoubleShortItem,SensiCamDoubleLongItem);
			break;
		// Copy First Item Check Boxes
		case SensiCamCopyDelayItem:
			SensiCamCopyDelay = !SensiCamCopyDelay;
			setcheckbox(theDialog,SensiCamCopyDelay,SensiCamCopyDelayItem);
			if(SensiCamCopyDelay){
				for(i=2; i<timeCount ; i+=2){
					timeTable[i] = timeTable[0];
					sprintf(txt,"%d",timeTable[i]);
					GetDialogItem(theDialog, SensiCamDel1Item+i, &itemType, &itemHdl, &itemRect);
					setdialogitemtext(itemHdl, txt);
				}
			}			
			break;
		case SensiCamCopyExposureItem:
			SensiCamCopyExposure = !SensiCamCopyExposure;
			setcheckbox(theDialog,SensiCamCopyExposure,SensiCamCopyExposureItem);
			if(SensiCamCopyExposure){
				for(i=3; i<timeCount ; i+=2){
					timeTable[i] = timeTable[1];
					sprintf(txt,"%d",timeTable[i]);
					GetDialogItem(theDialog, SensiCamDel1Item+i, &itemType, &itemHdl, &itemRect);
					setdialogitemtext(itemHdl, txt);
				}
			}
			break;
		
		default:
			break;
		}	
			
	}
	
	DisposeDialog(theDialog);

	SetGWorld(savePort,saveDevice);
	
	SaveSensiCamSettings();	// send to camera
	return;
}


#endif

/****************************************************************************/

/* The Set Contours Dialog */
/*
void setcontour()
*/
/****************************************************************************/

/* The Set Surface Dialog */
/*
void setsurface()
*/
/************** Set View Dialog (within Set Surface Dialog *******************/

Point	lllim,urlim;

WindowRef SetSurfaceView_window;

int button_state = 0;
Point   ll,lr,ur,ul;
int     d,sv_wide,hi;
Rect surface_image_rect;
CGContextRef	surfviewContext;

int setscreensize()
{
    extern	TWOBYTE		header[];
    extern	DATAWORD	min,max;
    extern	short		pixsiz;
    extern	float		fract,fracty,ymn,ymx,de;
    
    if(sstdscrnsize) {
        if( pixsiz > 0 ) {
            surfwidth = pixsiz * header[NCHAN] * (1.0 + fract * (1.0 - persp) / 2.0);
            surfheight = pixsiz * header[NTRAK] * (1.0 + fract * (1.0 - persp) / 2.0); }
        else {
            surfwidth = header[NCHAN]/abs(pixsiz) * (1.0 + fract * (1.0 - persp) / 2.0);
            surfheight = header[NTRAK]/abs(pixsiz) * (1.0 + fract * (1.0 - persp) / 2.0);
        }
    }
    
    // also set the min and max for plots 
    
    if(sdatminmax) {
            ymn = min;
            ymx = max; }
    else {
            ymn = surfmin;
            ymx = surfmax;
    }
    if(ymn == ymx) ymx++;
    if(incrementby <= 0)
            incrementby = 1;
    de = fracty * 6.0 * (ymx-ymn) / header[NTRAK] * incrementby;
    return 0;
}

int setviewdialog()
{
    Boolean		checkbounds();
    
    extern	float		fract,de,ymn,ymx;
    extern	TWOBYTE		header[];
	int xright,ybot;
    int xleft,ytop;
	
	// this routine is done once at the start of the "set surface view" dialog window

	QDBeginCGContext(GetWindowPort(SetSurfaceView_window), &surfviewContext);
	CGContextSetLineWidth (surfviewContext, 1);
	CGContextSetRGBFillColor (surfviewContext, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor (surfviewContext, 0, 0, 0, 1);
	CGContextSelectFont (surfviewContext,"Helvetica",18,kCGEncodingMacRoman);
	CGContextSetTextDrawingMode (surfviewContext,kCGTextFillStroke);
	CGContextBeginPath (surfviewContext);
	
	xleft = surface_image_rect.left;
	//xsiz = surface_image_rect.right - surface_image_rect.left;
	xright = surface_image_rect.right;
	ytop = surface_image_rect.top;
	//ysiz = surface_image_rect.bottom - surface_image_rect.top;
	ybot = surface_image_rect.bottom;
    if(surfheight <= surfwidth) {	// use the full width
            ll.h = ul.h = xleft;		// offset from left of window
            ur.h = lr.h = xright;		// go all the way to the right of area
            d = surfheight*(xright-xleft)/surfwidth;	// 
            ur.v = ul.v = ytop + ((ybot-ytop)-d)/2;
            ll.v = lr.v = ur.v + d;
            drawquad(&ll,&lr,&ur,&ul,NULLCHAR,NULLCHAR);  } 	// go draw the quadrilateral 
    else {	// use the full height
            ur.v = ul.v = ytop;
            ll.v = lr.v = ybot;
            d = surfwidth*(ybot-ytop)/surfheight;
            ll.h = ul.h = xleft + ((xright-xleft)-d)/2;
            ur.h = lr.h = ll.h + d;
            drawquad(&ll,&lr,&ur,&ul,NULLCHAR,NULLCHAR);		// go draw the quadrilateral 
    }
    // Set the limits 
    ll.h += OFF;
    ll.v -= OFF;
    lllim = ll;				// set lower left limit 
    ur.h -= OFF;
    ur.v += OFF;
    urlim = ur;				// set upper right limit 
    
    // draw the outline of the initial view 
    
    sv_wide = ur.h - ll.h;
    hi = ll.v - ur.v;
    if(persp != 1) {
            lr.v = ll.v;
            lr.h = ll.h + sv_wide * (1.0 - fract);
            ul.h = ll.h + sv_wide * fract;
            ul.v = ur.v = ll.v - de*header[NTRAK]/(6.0*(ymx-ymn))*hi/incrementby;
            drawquad(&ll,&lr,&ur,&ul,HCHAR,VCHAR);
            }
    else {
            lr.v = ll.v;
            lr.h = ll.h + sv_wide;
            ul.h = ll.h + sv_wide * fract;
            ur.h = lr.h - sv_wide * fract;
            ul.v = ur.v = ll.v - de*header[NTRAK]/(6.0*(ymx-ymn))*hi/incrementby;
            drawquad(&ll,&lr,&ur,&ul,NULLCHAR,XCHAR);
    }
    return 0;
}
int drawviewdialog(Point mousePt)
{
    extern float fract,fracty;
    Boolean checkbounds( );
	SetPortWindowPort(SetSurfaceView_window);	// this is needed so that the mouse coordinates are right
       
    if(persp != 1) {
        d = (mousePt.h - lr.h)*(mousePt.h - lr.h) + (mousePt.v - lr.v)*(mousePt.v - lr.v);
        if( d <= CLOSE){
            if(checkbounds(&mousePt)){
                //drawquad(&ll,&lr,&ur,&ul,HCHAR,VCHAR);	// erase old 
                ul.h -= mousePt.h - lr.h;
                lr.h = mousePt.h;
                drawquad(&ll,&lr,&ur,&ul,HCHAR,VCHAR);
            }

                fract = 1.0 - (float)(lr.h - ll.h)/(float)sv_wide;
        }
    }
    d = (mousePt.h - ur.h)*(mousePt.h - ur.h) + (mousePt.v - ur.v)*(mousePt.v - ur.v);
    if( d <= CLOSE){
        if(persp != 1) {
            if(checkbounds(&mousePt)) {
                //drawquad(&ll,&lr,&ur,&ul,HCHAR,VCHAR);	// erase old 
                ur.v = ul.v = mousePt.v;
                drawquad(&ll,&lr,&ur,&ul,HCHAR,VCHAR);
            }
            fracty = (float)(ll.v - ul.v)/(float)hi;
        }
        else {					
            if(checkbounds(&mousePt)) {
                //drawquad(&ll,&lr,&ur,&ul,NULLCHAR,XCHAR);	// erase old 
                ur.v = ul.v = mousePt.v;
                ur.h = mousePt.h;
                ul.h = ll.h + lr.h - ur.h;
                drawquad(&ll,&lr,&ur,&ul,NULLCHAR,XCHAR);
            }
            fracty = (float)(ll.v - ul.v)/(float)hi;
            fract = (float)(lr.h - ur.h)/(float)sv_wide; 
        }
    }
    return 0;
}

Boolean checkbounds( pt)
Point *pt;
{
    if( pt->h > urlim.h)
            pt->h = urlim.h;
    if( pt->h < lllim.h)
            pt->h = lllim.h;
    if( pt->v < urlim.v)
            pt->v = urlim.v;
    if( pt->v > lllim.v)
            pt->v = lllim.v;

    return(true);
}

int drawquad(ll,lr,ur,ul,ch1,ch2)
Point	*ll,*lr,*ur,*ul;
char	ch1,ch2;

{
	static CGRect clearRect;
	static float yh;
	float x,y,w,h;
	HIViewRef   contentView;
	CGRect dstRect;

	if(ch1 == NULLCHAR && ch2 == NULLCHAR){
		HIViewFindByID(HIViewGetRoot(SetSurfaceView_window), kHIViewWindowContentID, &contentView);
		HIViewGetBounds(contentView, &dstRect);
		yh = dstRect.size.height;
		x = ll->h+1;
		y = yh - ll->v + 1;
		w = ur-> h - ul->h - 2;
		h = yh - ur->v - y - 2;
		clearRect = CGRectMake(x,y,w,h);
	}
	CGContextFillRect(surfviewContext,clearRect);
	CGContextMoveToPoint(surfviewContext,ll->h, yh - ll->v);
	CGContextAddLineToPoint(surfviewContext,(*lr).h, yh - (*lr).v);
	CGContextAddLineToPoint(surfviewContext,ur->h, yh - ur->v);
	CGContextAddLineToPoint(surfviewContext,ul->h, yh - ul->v);
	CGContextAddLineToPoint(surfviewContext,ll->h, yh - ll->v);

	CGContextShowTextAtPoint(surfviewContext, (*lr).h - CHOFFSET, yh - (*lr).v - CHOFFSET - 2, &ch1, 1);
	CGContextShowTextAtPoint(surfviewContext, ur->h - CHOFFSET , yh - ur->v - CHOFFSET, &ch2, 1);

	CGContextStrokePath( surfviewContext );
	CGContextFlush(surfviewContext);
	
    return 0;
}


/****************************************************************************/
/* The Line Out Dialog */
/****************************************************************************/

WindowRef LineOutTemplate_window;
ControlRef LineOutTemplate_done_Field;
ControlID LineOutTemplate_done_ControlID ={ 'Lout', 100 };
CGContextRef	LineOutContext;
Rect lineout_image_rect;
int template_height;
CGRect template_cg_Rect;

//LineOut controls
ControlRef LineOut_total_Field;
ControlID LineOut_total_ControlID ={ kLineout_signature, kLineout_total };
ControlRef LineOut_num_Field;
ControlID LineOut_num_ControlID ={ kLineout_signature, kLineout_num };
ControlRef LineOut_x0_Field;
ControlID LineOut_x0_ControlID ={ kLineout_signature, kLineout_x0 };
ControlRef LineOut_y0_Field;
ControlID LineOut_y0_ControlID ={ kLineout_signature, kLineout_y0 };
ControlRef LineOut_x1_Field;
ControlID LineOut_x1_ControlID ={ kLineout_signature, kLineout_x1 };
ControlRef LineOut_y1_Field;
ControlID LineOut_y1_ControlID ={ kLineout_signature, kLineout_y1 };
ControlRef LineOut_label_Field;
ControlID LineOut_label_ControlID ={ kLineout_signature, kLineout_label };
ControlRef LineOut_sum_dir_Field;
ControlID LineOut_sum_dir_ControlID ={ kLineout_signature, kLineout_sum_dir };
ControlRef LineOut_al_start_Field;
ControlID LineOut_al_start_ControlID ={ kLineout_signature, kLineout_al_start };
ControlRef LineOut_al_end_Field;
ControlID LineOut_al_end_ControlID ={ kLineout_signature, kLineout_al_end };
ControlRef LineOut_prev_Field;
ControlID LineOut_prev_ControlID ={ kLineout_signature, kLineout_prev };
ControlRef LineOut_image_Field;
ControlID LineOut_image_ControlID ={ kLineout_signature, kLineout_image };


int set_LineOut_Values()
{
    CFStringRef text;

	OSErr err;

	SET_CONTROL_VALUE( numboxes,LineOut_total_ControlID, LineOut_total_Field );
	SET_CONTROL_VALUE( boxnum,LineOut_num_ControlID, LineOut_num_Field );
	SET_CONTROL_VALUE(loboxes[boxnum-1].left,LineOut_x0_ControlID, LineOut_x0_Field );
	SET_CONTROL_VALUE(loboxes[boxnum-1].top,LineOut_y0_ControlID, LineOut_y0_Field );
	SET_CONTROL_VALUE(loboxes[boxnum-1].right,LineOut_x1_ControlID, LineOut_x1_Field );
	SET_CONTROL_VALUE(loboxes[boxnum-1].bottom,LineOut_y1_ControlID, LineOut_y1_Field );
	
	SET_CONTROL_SVALUE( &boxlabel[boxnum-1],LineOut_label_ControlID, LineOut_label_Field );
	
	SetControl32BitValue(LineOut_sum_dir_Field, 2 - losumx[boxnum-1]);
	
	SetControl32BitValue(LineOut_al_start_Field, alignstart);
	SetControl32BitValue(LineOut_al_end_Field, alignend);
	
	if(boxnum <= 1) DisableControl(LineOut_prev_Field);
	else EnableControl(LineOut_prev_Field);
	DrawControls (LineOutTemplate_window);     
    
    return 0;
}

int remember_LineOut_Values()
{
    CFStringRef text;
    //int i;
    Size actualSize; 
    
    if(GetControl32BitValue(LineOut_al_end_Field) == 1)
        alignend = true;
    else
        alignend = false;

    if(GetControl32BitValue(LineOut_al_start_Field) == 1)
        alignstart = true;
    else
        alignstart = false;

    if(GetControl32BitValue(LineOut_sum_dir_Field) == 1)
        losumx[boxnum-1] = 1;
    else
        losumx[boxnum-1] = 0;

    GetControlData( LineOut_x0_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    loboxes[boxnum-1].left = CFStringGetIntValue( text );
    CFRelease( text );
	
    GetControlData( LineOut_y0_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    loboxes[boxnum-1].top = CFStringGetIntValue( text );
    CFRelease( text );
	
    GetControlData( LineOut_x1_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    loboxes[boxnum-1].right = CFStringGetIntValue( text );
    CFRelease( text );
	
    GetControlData( LineOut_y1_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    loboxes[boxnum-1].bottom = CFStringGetIntValue( text );
    CFRelease( text );
	
	GetControlData( LineOut_label_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
	CFStringGetCString(text,(char*)&boxlabel[boxnum-1],LOLAB_LENGTH, kCFStringEncodingMacRoman);
	CFRelease( text );
	
    return 0;
}


// Set Lineout Dialog event handler
pascal OSStatus LineOutTemplate_EventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
    OSStatus 		result = eventNotHandledErr;
    HICommand		command;
    Boolean		stopModalLoop = FALSE;
    CFStringRef text;
    Size actualSize; 
	int tmp;
	extern Point		screen_to_ccd_coords();
    
    Point		mousePt;
    UInt32 event_info;
	
	SetPortWindowPort(LineOutTemplate_window);	// this is needed so that the mouse coordinates are right
    
    event_info = GetEventClass (event);
    
    if(event_info == kEventClassCommand){
    
        // Get the HI Command
        GetEventParameter (event, kEventParamDirectObject, typeHICommand, NULL, 
                                sizeof (HICommand), NULL, &command);
        // Look for commands
        switch (command.commandID)
        {
            case 'DONE':
				remember_LineOut_Values();
				QDEndCGContext (GetWindowPort(LineOutTemplate_window), &LineOutContext);
                result = noErr;
				SetThemeCursor( kThemeArrowCursor );
                stopModalLoop = TRUE;
                break;
			case 'NEXT':
				remember_LineOut_Values();
				boxnum++;
				if(boxnum > numboxes) {
					numboxes = boxnum;
				}
				drawtemplate();
				set_LineOut_Values();
				break;
			case 'PREV':
				remember_LineOut_Values();
				boxnum--;
				drawtemplate();
				set_LineOut_Values();
				break;
			
			case 'NBOX':		// doesn't exactly work as desired
				GetControlData( LineOut_num_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
				boxnum = CFStringGetIntValue( text );
				CFRelease( text );
				if(boxnum < 1) boxnum = 1;
				drawtemplate();
				set_LineOut_Values();
				break;
			case 'ALST':
				alignstart = !alignstart;
				break;

			case 'ALND':
				alignend = !alignend;
				break;
			case 'open':
				tmp = loadtemplate(nil);
				if( tmp == 0 ) {							// if new values were loaded 
					boxnum = 1;
					set_LineOut_Values();
				}
				drawtemplate();
				break;
			case 'save':
				remember_LineOut_Values();			// make sure any newly-entered values are saved
				savetemplate(nil);
				drawtemplate();
				break;
        }
        // Stop the modal loop.
        if (stopModalLoop)
        {
            QuitAppModalLoopForWindow((WindowRef)userData);
            
        }
    } else {
        event_info = GetEventKind (event);
        switch(event_info)
        {
             case kEventMouseDown:
					
				GetMouse(&mousePt);
				if(mousePt.h > lineout_image_rect.right){
					result = eventNotHandledErr;			// it's not in the image part -- let it go
					break;
				}
				SetThemeCursor( kThemeCrossCursor );
				set_ul_point(&loboxes[boxnum-1],screen_to_ccd_coords(&mousePt,&lineout_image_rect));
				if( boxnum != 1) {
					if( alignstart ) {
						if( losumx[boxnum-1] ) {
							loboxes[boxnum-1].top = loboxes[0].top; 
						} else {
							loboxes[boxnum-1].left = loboxes[0].left;
						}
					}
				}
				set_LineOut_Values();
                result = noErr;
                break;
             case kEventMouseUp:
                //button_state = 0;
                //printf("up\n");
				SetThemeCursor( kThemeArrowCursor );
                break;
             case kEventMouseDragged:
                result = noErr;
				GetMouse(&mousePt);
				set_lr_point(&loboxes[boxnum-1],screen_to_ccd_coords(&mousePt,&lineout_image_rect));
				if( boxnum != 1) {
					if( alignend ) {
						if( losumx[boxnum-1] ) {
							loboxes[boxnum-1].bottom = loboxes[0].bottom; 
						} else {
							loboxes[boxnum-1].right = loboxes[0].right;
						}
					}
				}
				set_LineOut_Values();
				drawtemplate();

                break;
        }
       
    }
    //Return how we handled the event.
    return result;
}


OSErr LineOutTemplate_CommandHandler()
{
        IBNibRef nibRef;
		SInt16 offset;
		int tmp;
		HIViewRef   contentView;
		CGRect templateRect;
        EventHandlerUPP	dialogUPP;
        
        static EventTypeSpec    eventTypes[4];
		OSStatus err = eventNotHandledErr;

        
        eventTypes[0].eventClass = kEventClassCommand;
        eventTypes[0].eventKind  = kEventCommandProcess;
        eventTypes[1].eventClass = kEventClassMouse;
        eventTypes[1].eventKind  = kEventMouseDragged;
        
        eventTypes[2].eventClass = kEventClassMouse;
        eventTypes[2].eventKind  = kEventMouseDown;
        eventTypes[3].eventClass = kEventClassMouse;
        eventTypes[3].eventKind  = kEventMouseUp;
    
        
        // Get the window from the nib and show it
        err = CreateNibReference( CFSTR("oma"), &nibRef );
        err = CreateWindowFromNib( nibRef, CFSTR("LineOut"), &LineOutTemplate_window );
        DisposeNibReference( nibRef );
        
        // Install our event handler
        dialogUPP =  NewEventHandlerUPP(LineOutTemplate_EventHandler);
        err = InstallWindowEventHandler(LineOutTemplate_window, dialogUPP, 4,(const EventTypeSpec*) &eventTypes, (void *)LineOutTemplate_window, NULL);

        GetControlByID( LineOutTemplate_window, &LineOutTemplate_done_ControlID, &LineOutTemplate_done_Field );
        
        ShowWindow(LineOutTemplate_window );
        
		GetControlByID( LineOutTemplate_window, &LineOut_total_ControlID, &LineOut_total_Field );
		GetControlByID( LineOutTemplate_window, &LineOut_num_ControlID, &LineOut_num_Field );
		GetControlByID( LineOutTemplate_window, &LineOut_x0_ControlID, &LineOut_x0_Field );
		GetControlByID( LineOutTemplate_window, &LineOut_x1_ControlID, &LineOut_x1_Field );
		GetControlByID( LineOutTemplate_window, &LineOut_y0_ControlID, &LineOut_y0_Field );
		GetControlByID( LineOutTemplate_window, &LineOut_y1_ControlID, &LineOut_y1_Field );
		GetControlByID( LineOutTemplate_window, &LineOut_label_ControlID, &LineOut_label_Field );
		GetControlByID( LineOutTemplate_window, &LineOut_sum_dir_ControlID, &LineOut_sum_dir_Field );
		GetControlByID( LineOutTemplate_window, &LineOut_al_start_ControlID, &LineOut_al_start_Field );
		GetControlByID( LineOutTemplate_window, &LineOut_al_end_ControlID, &LineOut_al_end_Field );
		GetControlByID( LineOutTemplate_window, &LineOut_prev_ControlID, &LineOut_prev_Field );
		GetControlByID( LineOutTemplate_window, &LineOut_image_ControlID, &LineOut_image_Field );
		
		GetBestControlRect(LineOut_image_Field, &lineout_image_rect, &offset);	// the full size of the image rect
		
		if(header[NTRAK] <= header[NCHAN]) {
			// use full horizontal 
			tmp = ((lineout_image_rect.bottom - lineout_image_rect.top) - 
				  header[NTRAK]*(lineout_image_rect.bottom - lineout_image_rect.top)/header[NCHAN])/2;
			lineout_image_rect.top += tmp;
			lineout_image_rect.bottom -= tmp;
		} else {
			// use full verticle 
			tmp = ((lineout_image_rect.right - lineout_image_rect.left) - 
				  header[NCHAN]*(lineout_image_rect.right - lineout_image_rect.left)/header[NTRAK])/2;
			lineout_image_rect.left += tmp;
			lineout_image_rect.right -= tmp;
		}
		
		HIViewFindByID(HIViewGetRoot(LineOutTemplate_window), kHIViewWindowContentID, &contentView);
		HIViewGetBounds(contentView, &templateRect);
		template_height = templateRect.size.height;
		
		QDBeginCGContext(GetWindowPort(LineOutTemplate_window), &LineOutContext);
		CGContextSetLineWidth (LineOutContext, 1);
		CGContextSetRGBFillColor (LineOutContext, 1, 1, 1, 1);
		CGContextSetRGBStrokeColor (LineOutContext, 0, 0, 0, 1);

		CGContextBeginPath (LineOutContext);
		template_cg_Rect = CGRectMake(lineout_image_rect.left,template_height - lineout_image_rect.bottom,
			lineout_image_rect.right-lineout_image_rect.left,lineout_image_rect.bottom-lineout_image_rect.top);

		set_LineOut_Values();
		drawtemplate();

        // Run modally
        RunAppModalLoopForWindow(LineOutTemplate_window);
        
        HideWindow(LineOutTemplate_window);
        DisposeWindow(LineOutTemplate_window);
        DisposeEventHandlerUPP(dialogUPP);

        return err;
}


/* The Line Out Template Dialog */

void oma_template()
{	
	extern int			gwnum;					/* the graphics window counter */
	
	extern Point		screen_to_ccd_coords();
	extern TWOBYTE 		header[];
	int dquartz();

	if( gwnum == 0 ) {		/* if the current data is not displayed, do so */
		dquartz(0,0);
	}	
	
	LineOutTemplate_CommandHandler();
	
	return;
}

 
Point screen_to_ccd_coords(Point* scrnpt, Rect* scrnrect)
{
	int nc,nt;
	Point answer;
	
	extern TWOBYTE header[];
	
	nc = (scrnpt->h - scrnrect->left) * (header[NCHAN]-1) / (scrnrect->right - scrnrect->left);
	nt = (scrnpt->v - scrnrect->top) * (header[NTRAK]-1) / (scrnrect->bottom - scrnrect->top);
	if(nc > (header[NCHAN]-1)) nc = header[NCHAN]-1;
	if(nc < 0) nc = 0;
	if(nt > (header[NTRAK]-1)) nt = header[NTRAK]-1;
	if(nt < 0) nt = 0;
		
	answer.h = nc;
	answer.v = nt;
	
	return(answer);

}
Point ccd_to_screen_coords(Point* ccdpt, Rect* scrnrect)
{
	Point answer;
	
	extern TWOBYTE header[];
	
	answer.h = ccdpt->h * (scrnrect->right-scrnrect->left) / (header[NCHAN]-1) + scrnrect->left;
	answer.v = ccdpt->v * (scrnrect->bottom-scrnrect->top) / (header[NTRAK]-1) + scrnrect->top;
	
	return(answer);

}

int set_ul_point(Rect* rect,Point pt)			/* set the upper left point of rect to pt */
{
	rect->top = pt.v;
	rect->left = pt.h;
	return 0;
}

int set_lr_point(Rect* rect,Point pt)			/* set the lower right point of rect to pt */
{
	rect->bottom = pt.v;
	rect->right = pt.h;
	return 0;
}


int drawtemplate()
{
	int tmp;
	
	extern int gwnum;
	MTViewData* data;
	
	data = (MTViewData*) oma_wind[gwnum-1].view_data_ptr;
	CGContextDrawImage(LineOutContext, template_cg_Rect, data->theImage);
	

	/*
	CopyBits( (BitMap*) *GetPortPixMap(myGWorldPtrs[gwnum-1]),
						GetPortBitMapForCopyBits(GetWindowPort(GetDialogWindow(theDialog))),
			 			&copyRect[gwnum-1],&drawrect,0,0);
	*/
	for(tmp=1; tmp<=numboxes; tmp++) {
		if(tmp!=boxnum)
			drawboxes(tmp-1,-1);		/* draw in black */
		else 
			drawboxes(tmp-1,1);
	}
	//drawboxes(boxnum-1,1);		/* draw in srcXor black */
	CGContextFlush(LineOutContext);
	return 0;
}

/*
	Notes on cflag meaning:
            cflag = 0 --> srcXor with White
            cflag = 1 --> srcXor with Black
            cflag = -1 --> scrCopy with White
            cflag = -2 --> scrCopy with Black
*/

int drawboxes(int num1,int cflag)
 {
 	int i;
	Rect screenbox;
	
	extern Boolean boxes_are_ok();
	
	if( !boxes_are_ok(num1) ){
		//beep();
		//printf("Line Out Region/Scan Parameter Mismatch.\nOMA>");
		return -1;
	}

	// cflag = 1 --> draw the box in a template; use srcXor 
	// cflag = 0 --> erase the box in a template; use srcXor 
	// cflag = -1 --> draw the box from impdis; don't use srcXor; draw in black 
	// cflag = -2 --> don't use srcXor; draw in white 

	if(cflag == 1)
			CGContextSetRGBStrokeColor(LineOutContext, 0, 0, 0, 1);
	else
			CGContextSetRGBStrokeColor(LineOutContext, .5, .5, .5, 1);

	i = num1;	

	set_ul_point(&screenbox,ccd_to_screen_coords((Point*)&(loboxes[i].top),&lineout_image_rect));
	set_lr_point(&screenbox,ccd_to_screen_coords((Point*)&(loboxes[i].bottom),&lineout_image_rect));
	CGContextMoveToPoint(LineOutContext,screenbox.left,template_height - screenbox.top);
	CGContextAddLineToPoint(LineOutContext,screenbox.right,template_height - screenbox.top);
	CGContextAddLineToPoint(LineOutContext,screenbox.right,template_height - screenbox.bottom);
	CGContextAddLineToPoint(LineOutContext,screenbox.left,template_height - screenbox.bottom);
	CGContextAddLineToPoint(LineOutContext,screenbox.left,template_height - screenbox.top);
	CGContextStrokePath( LineOutContext );
	
		
 	return 0;
 }

/* check to be sure that the boxes are ok in the light of current scan parameters */ 

Boolean boxes_are_ok(int num1)
{
	int i;
	extern TWOBYTE header[];
	i = num1;
	//for(i=num1; i< num2; i++) {
		if( loboxes[i].top < 0 ||
		    loboxes[i].left < 0 ||
			loboxes[i].right > header[NCHAN]-1 ||
			loboxes[i].bottom > header[NTRAK]-1 ||
			loboxes[i].top > loboxes[i].bottom ||
			loboxes[i].left > loboxes[i].right ) {
				return false;
		}
	//}
	return true;	
}
 



/****************************************************************************/

/****************************************************************************/

/* The Status Dialog Box */



//DialogPtr		colorminmaxdlog = 0;

Boolean			autoupdate = true;
Boolean			autoscale = true;
short			cminmaxinc = 5;

#define CSF 3.0
float r_scale=1.,g_scale=1.,b_scale=1.;
float r_gamma=1.,g_gamma=1.,b_gamma=1.;

//Rect			info_rect = {140, 4, 240, 232};
Rect 			cbarRect = {270, 8, 290, 235};		/* the color bar rectangle */
//Rect			macnumtext,xyztext0,xyztext1;
//Rect			toolrect[NUMTOOLS];

short			toolselected = 0;

int status_window_inited = false;

extern int	showselection;
extern int	showruler;
extern int	plotline;
extern int	docalcs;
extern int	s_font;

CGContextRef the_state;
void save_state(){
	HIViewRef my_vr;
	HIViewFindByID(HIViewGetRoot(FrontWindow()), kHIViewWindowContentID, &my_vr);
	the_state = (CGContextRef)my_vr;
	//CGContextSaveGState(the_state);
}
void restore_state(){
	//CGContextRestoreGState(the_state);
}


/* Routines used by more than one Dialog */

int setbuttons(DialogPtr theDialog,int	thecondition,int	oneitem,int	otheritem)
{
	
	short		itemType;
	Handle		itemHdl;
	Rect		itemRect;

	if(thecondition) {
		GetDialogItem(theDialog, oneitem, &itemType, &itemHdl, &itemRect);
		SetControlValue((ControlRef)itemHdl,1);	
		GetDialogItem(theDialog, otheritem, &itemType, &itemHdl, &itemRect);
		SetControlValue((ControlRef)itemHdl,0);
	}
	else {
		GetDialogItem(theDialog, oneitem, &itemType, &itemHdl, &itemRect);
		SetControlValue((ControlRef)itemHdl,0);	
		GetDialogItem(theDialog, otheritem, &itemType, &itemHdl, &itemRect);
		SetControlValue((ControlRef)itemHdl,1);
	}
	return 0;
}

int setcheckbox(DialogPtr theDialog,int colr,int colritm)
{
	
	short		itemType;
	Handle		itemHdl;
	Rect		itemRect;


	if(colr) {
		GetDialogItem(theDialog, colritm, &itemType, &itemHdl, &itemRect);
		SetControlValue((ControlRef)itemHdl,1);	
	}
	else {
		GetDialogItem(theDialog, colritm, &itemType, &itemHdl, &itemRect);
		SetControlValue((ControlRef)itemHdl,0);	
	}
	return 0;
}

int fileflush(char* name)			/* This is needed to ensure that files written will survive a crash */
{
    return 0;
}

int gethelpfile()

{
    int fd;
	char oldname[CHPERLN];
	extern char contents_path[];

	getcwd(oldname,CHPERLN);
	chdir(contents_path);
	//printf("%s\n",contents_path);
	
    fd = open(HELPFILE,O_RDONLY);  
	chdir(oldname);  
    return fd ;
}

int helpdone(int fp)
{

    close(fp);
    return 0;

}
/****************************************************************************/
// glue routines
//
void setdialogitemtext(Handle itemHdl,char* mytxt)
{
	CopyCStringToPascal(mytxt,pstring);
	SetDialogItemText(itemHdl, pstring);
}
void getdialogitemtext(Handle itemHdl,char* mytxt)
{
	GetDialogItemText(itemHdl, pstring);
	CopyPascalStringToC(pstring,mytxt);

}


/****************************************************************************/
/****************************************************************************/
// 	Handle nib-based dialogs
/****************************************************************************/
/****************************************************************************/

ControlRef status_cmin_Field;
ControlID status_cmin_ControlID ={ kstatus_signature, kstatus_cmin };
ControlRef status_cmax_Field;
ControlID status_cmax_ControlID ={ kstatus_signature, kstatus_cmax };
ControlRef status_dcmn_Field;
ControlID status_dcmn_ControlID ={ kstatus_signature, kstatus_dcmn };
ControlRef status_icmn_Field;
ControlID status_icmn_ControlID ={ kstatus_signature, kstatus_icmn };
ControlRef status_dcmx_Field;
ControlID status_dcmx_ControlID ={ kstatus_signature, kstatus_dcmx };
ControlRef status_icmx_Field;
ControlID status_icmx_ControlID ={ kstatus_signature, kstatus_icmx };
ControlRef status_autoscale_Field;
ControlID status_autoscale_ControlID ={ kstatus_signature, kstatus_autoscale };
ControlRef status_autoupdate_Field;
ControlID status_autoupdate_ControlID ={ kstatus_signature, kstatus_autoupdate };
ControlRef status_cmnmxinc_Field;
ControlID status_cmnmxinc_ControlID ={ kstatus_signature, kstatus_cmnmxinc };




ControlRef status_t1_Field;
ControlID status_t1_ControlID ={ kstatus_signature, kstatus_t1 };
ControlRef status_t2_Field;
ControlID status_t2_ControlID ={ kstatus_signature, kstatus_t2 };
ControlRef status_t3_Field;
ControlID status_t3_ControlID ={ kstatus_signature, kstatus_t3 };
ControlRef status_t4_Field;
ControlID status_t4_ControlID ={ kstatus_signature, kstatus_t4 };
ControlRef status_t5_Field;
ControlID status_t5_ControlID ={ kstatus_signature, kstatus_t5 };
ControlRef status_t6_Field;
ControlID status_t6_ControlID ={ kstatus_signature, kstatus_t6 };
ControlRef status_t7_Field;
ControlID status_t7_ControlID ={ kstatus_signature, kstatus_t7 };

ControlRef status_tool_Field[NUMTOOLS];
ControlID status_tool_ControlID[NUMTOOLS];

ControlRef status_cmni_Field;
ControlID status_cmni_ControlID ={ kstatus_signature, kstatus_cmni };

// colorbar palette control for the status field
ControlRef status_cbar_Field;
ControlID status_cbar_ControlID ={ kstatus_signature, kstatus_cbar };

// RGB related things in the status bar

ControlRef status_rsca_Field;
ControlID status_rsca_ControlID ={ kstatus_signature, kstatus_rsca };
ControlRef status_gsca_Field;
ControlID status_gsca_ControlID ={ kstatus_signature, kstatus_gsca };
ControlRef status_bsca_Field;
ControlID status_bsca_ControlID ={ kstatus_signature, kstatus_bsca };

ControlRef status_rtxt_Field;
ControlID status_rtxt_ControlID ={ kstatus_signature, kstatus_rtxt };
ControlRef status_gtxt_Field;
ControlID status_gtxt_ControlID ={ kstatus_signature, kstatus_gtxt };
ControlRef status_btxt_Field;
ControlID status_btxt_ControlID ={ kstatus_signature, kstatus_btxt };

ControlRef status_rgbm_Field;
ControlID status_rgbm_ControlID ={ kstatus_signature, kstatus_rgbm };
ControlRef status_rrgb_Field;
ControlID status_rrgb_ControlID ={ kstatus_signature, kstatus_rrgb };

ControlRef status_rgam_Field;
ControlID status_rgam_ControlID ={ kstatus_signature, kstatus_rgam };
ControlRef status_ggam_Field;
ControlID status_ggam_ControlID ={ kstatus_signature, kstatus_ggam };
ControlRef status_bgam_Field;
ControlID status_bgam_ControlID ={ kstatus_signature, kstatus_bgam };

ControlRef status_rgamtxt_Field;
ControlID status_rgamtxt_ControlID ={ kstatus_signature, kstatus_rgamtxt };
ControlRef status_ggamtxt_Field;
ControlID status_ggamtxt_ControlID ={ kstatus_signature, kstatus_ggamtxt };
ControlRef status_bgamtxt_Field;
ControlID status_bgamtxt_ControlID ={ kstatus_signature, kstatus_bgamtxt };

ControlRef status_rgbgam_Field;
ControlID status_rgbgam_ControlID ={ kstatus_signature, kstatus_rgbgam };
ControlRef status_rrgbgam_Field;
ControlID status_rrgbgam_ControlID ={ kstatus_signature, kstatus_rrgbgam };


int     tabList[] = {6, 57,52,53,54,55,56};   // Tab UserPane IDs
int     lastTabIndex = 1;



WindowRef oma_prefs_window;


ControlRef SetSurfaceView_done_Field;
ControlID SetSurfaceView_done_ControlID ={ 'Svew', 100 };
ControlRef SetSurfaceView_image_Field;
ControlID SetSurfaceView_image_ControlID ={ 'Svew', 101 };

// PIV controls
ControlRef piv_inc_Field;
ControlID piv_inc_ControlID ={ koma_prefs_signature, koma_prefs_piv_inc };
ControlRef piv_siz_Field;
ControlID piv_siz_ControlID ={ koma_prefs_signature, koma_prefs_piv_siz };
ControlRef piv_scale_Field;
ControlID piv_scale_ControlID ={ koma_prefs_signature, koma_prefs_piv_scale };
ControlRef piv_plot_Field;
ControlID piv_plot_ControlID ={ koma_prefs_signature, koma_prefs_piv_plot };
ControlRef piv_clip_Field;
ControlID piv_clip_ControlID ={ koma_prefs_signature, koma_prefs_piv_clip };
ControlRef piv_label_Field;
ControlID piv_label_ControlID ={ koma_prefs_signature, koma_prefs_piv_label };
ControlRef piv_dir_Field;
ControlID piv_dir_ControlID ={ koma_prefs_signature, koma_prefs_piv_dir };
ControlRef piv_alt_Field;
ControlID piv_alt_ControlID ={ koma_prefs_signature, koma_prefs_piv_alt };

ControlRef tab_Field;
ControlID tab_ControlID ={ koma_prefs_signature, koma_prefs_TAB_ID };

// INTEGRATED controls
ControlRef int_StdSize_Field;
ControlID int_StdSize_ControlID ={ koma_prefs_signature, koma_prefs_int_StdSize };
ControlRef int_DatMin_Field;
ControlID int_DatMin_ControlID ={ koma_prefs_signature, koma_prefs_int_DatMin };
ControlRef int_Int_Field;
ControlID int_Int_ControlID ={ koma_prefs_signature, koma_prefs_int_Int };
ControlRef int_IntX_Field;
ControlID int_IntX_ControlID ={ koma_prefs_signature, koma_prefs_int_IntX };
ControlRef int_Max_Field;
ControlID int_Max_ControlID ={ koma_prefs_signature, koma_prefs_int_Max };
ControlRef int_Min_Field;
ControlID int_Min_ControlID ={ koma_prefs_signature, koma_prefs_int_Min };
ControlRef int_Width_Field;
ControlID int_Width_ControlID ={ koma_prefs_signature, koma_prefs_int_Width };
ControlRef int_Height_Field;
ControlID int_Height_ControlID ={ koma_prefs_signature, koma_prefs_int_Height };

// CONTOUR controls
ControlRef cont_StdSize_Field;
ControlID cont_StdSize_ControlID ={ koma_prefs_signature, koma_prefs_cont_StdSize };
ControlRef cont_DatMin_Field;
ControlID cont_DatMin_ControlID ={ koma_prefs_signature, koma_prefs_cont_DatMin };
ControlRef cont_Max_Field;
ControlID cont_Max_ControlID ={ koma_prefs_signature, koma_prefs_cont_Max };
ControlRef cont_Min_Field;
ControlID cont_Min_ControlID ={ koma_prefs_signature, koma_prefs_cont_Min };
ControlRef cont_Width_Field;
ControlID cont_Width_ControlID ={ koma_prefs_signature, koma_prefs_cont_Width };
ControlRef cont_Height_Field;
ControlID cont_Height_ControlID ={ koma_prefs_signature, koma_prefs_cont_Height };
ControlRef cont_NCtr_Field;
ControlID cont_NCtr_ControlID ={ koma_prefs_signature, koma_prefs_cont_NCtr };
ControlRef cont_Color_Field;
ControlID cont_Color_ControlID ={ koma_prefs_signature, koma_prefs_cont_Color };

ControlRef cont_CValues_Field[MAXNOCTRS];
ControlID cont_CValues_ControlID[MAXNOCTRS];

// HISTOGRAM controls
ControlRef hist_StdSize_Field;
ControlID hist_StdSize_ControlID ={ koma_prefs_signature, koma_prefs_hist_StdSize };
ControlRef hist_DatMin_Field;
ControlID hist_DatMin_ControlID ={ koma_prefs_signature, koma_prefs_hist_DatMin };
ControlRef hist_Max_Field;
ControlID hist_Max_ControlID ={ koma_prefs_signature, koma_prefs_hist_Max };
ControlRef hist_Min_Field;
ControlID hist_Min_ControlID ={ koma_prefs_signature, koma_prefs_hist_Min };
ControlRef hist_Width_Field;
ControlID hist_Width_ControlID ={ koma_prefs_signature, koma_prefs_hist_Width };
ControlRef hist_Height_Field;
ControlID hist_Height_ControlID ={ koma_prefs_signature, koma_prefs_hist_Height };
ControlRef hist_Clear_Field;
ControlID hist_Clear_ControlID ={ koma_prefs_signature, koma_prefs_hist_Clear };

//SURFACE controls
ControlRef surf_StdSize_Field;
ControlID surf_StdSize_ControlID ={ koma_prefs_signature, koma_prefs_surf_StdSize };
ControlRef surf_DatMin_Field;
ControlID surf_DatMin_ControlID ={ koma_prefs_signature, koma_prefs_surf_DatMin };
ControlRef surf_Max_Field;
ControlID surf_Max_ControlID ={ koma_prefs_signature, koma_prefs_surf_Max };
ControlRef surf_Min_Field;
ControlID surf_Min_ControlID ={ koma_prefs_signature, koma_prefs_surf_Min };
ControlRef surf_Width_Field;
ControlID surf_Width_ControlID ={ koma_prefs_signature, koma_prefs_surf_Width };
ControlRef surf_Height_Field;
ControlID surf_Height_ControlID ={ koma_prefs_signature, koma_prefs_surf_Height };
ControlRef surf_Color_Field;
ControlID surf_Color_ControlID ={ koma_prefs_signature, koma_prefs_surf_Color };
ControlRef surf_StdPlot_Field;
ControlID surf_StdPlot_ControlID ={ koma_prefs_signature, koma_prefs_surf_StdPlot };
ControlRef surf_White_Field;
ControlID surf_White_ControlID ={ koma_prefs_signature, koma_prefs_surf_White };
ControlRef surf_Inc_Field;
ControlID surf_Inc_ControlID ={ koma_prefs_signature, koma_prefs_surf_Inc };

//PREFIX controls
ControlRef prefix_SaveP_Field;
ControlID prefix_SaveP_ControlID ={ koma_prefs_signature, koma_prefs_prefix_SaveP };
ControlRef prefix_SaveS_Field;
ControlID prefix_SaveS_ControlID ={ koma_prefs_signature, koma_prefs_prefix_SaveS };
ControlRef prefix_GetP_Field;
ControlID prefix_GetP_ControlID ={ koma_prefs_signature, koma_prefs_prefix_GetP };
ControlRef prefix_GetS_Field;
ControlID prefix_GetS_ControlID ={ koma_prefs_signature, koma_prefs_prefix_GetS };
ControlRef prefix_MacP_Field;
ControlID prefix_MacP_ControlID ={ koma_prefs_signature, koma_prefs_prefix_MacP };
ControlRef prefix_MacS_Field;
ControlID prefix_MacS_ControlID ={ koma_prefs_signature, koma_prefs_prefix_MacS };
ControlRef prefix_SetP_Field;
ControlID prefix_SetP_ControlID ={ koma_prefs_signature, koma_prefs_prefix_SetP };
ControlRef prefix_SetS_Field;
ControlID prefix_SetS_ControlID ={ koma_prefs_signature, koma_prefs_prefix_SetS };
//ControlRef prefix_LoadSet_Field;
//ControlID prefix_LoadSet_ControlID ={ koma_prefs_signature, koma_prefs_prefix_LoadSet };
ControlRef prefix_pal_Field;
ControlID prefix_pal_ControlID ={ koma_prefs_signature, koma_prefs_prefix_pal };
ControlRef prefix_transparent_Field;
ControlID prefix_transparent_ControlID ={ koma_prefs_signature, koma_prefs_prefix_transparent };


extern int fftsize,boxinc,plotduringpiv, autoclip,labelplot,plot_dir_pref,alter_vectors;

extern float  pivscale;

// ----------------------------------------------------------------------
// Show the selected pane, hide the others.

void SelectItemOfTabControl(ControlRef tabControl)
{
    ControlRef userPane;
    ControlRef selectedPane = NULL;
    ControlID controlID;
    UInt16 i;

    SInt16 index = GetControlValue(tabControl);
    
    
    //if(update) lastTabIndex = index;
    //else index = lastTabIndex;
    
    lastTabIndex = index;
    
    controlID.signature = koma_prefs_signature;

    for (i = 1; i < tabList[0] + 1; i++)
    {
        controlID.id = tabList[i];
        GetControlByID(GetControlOwner(tabControl), &controlID, &userPane);
       
        if (i == index) {
            selectedPane = userPane;
        } else {
            SetControlVisibility(userPane,false,false);
        }
    }
    
    if (selectedPane != NULL) {
    	// Codewarrior doesn't know about this; taking it out doesn't seem to matter
        //EnableControl(selectedPane);
        SetControlVisibility(selectedPane, true, true);
    }
    
    Draw1Control(tabControl);
}

// ----------------------------------------------------------------------



// Preferences Dialog event handler
pascal OSStatus oma_prefs_EventHandler (EventHandlerCallRef myHandler, EventRef event, void *userData)
{
    OSStatus 		result = eventNotHandledErr;
    HICommand		command;
    Boolean		stopModalLoop = FALSE;
    int i;
    CFStringRef text;
    Size actualSize;
    OSErr err;
    OSErr SetSurfaceView_CommandHandler();
    
    extern		int	histo[];
    
    // Get the HI Command
    GetEventParameter (event, kEventParamDirectObject, typeHICommand, NULL, 
                            sizeof (HICommand), NULL, &command);
    // Look for commands
    
    
    switch (command.commandID)
    {
        case 'DONE':
            //remember_PIV_Pane_Values();	
            remember_INTEGRATED_Pane_Values();
            remember_CONTOUR_Pane_Values();
            remember_HISTOGRAM_Pane_Values();
            remember_SURFACE_Pane_Values();
            remember_PREFIX_Pane_Values();
            remember_PIV2_Pane_Values();
            assigncolor1();
			SetThemeCursor( kThemeArrowCursor );
            stopModalLoop = TRUE;
            result = noErr;
            break;
        case 'CNCL':
            stopModalLoop = TRUE;
            result = noErr;
            break;
        case 'PANE':
            SelectItemOfTabControl(tab_Field);
            stopModalLoop = FALSE;
            result = noErr;
            break;
        case 'SVEW':
            remember_SURFACE_Pane_Values();
            setscreensize();
            /* Now go show the view */
            //setviewdialog();
            SetSurfaceView_CommandHandler();
            stopModalLoop = FALSE;
            result = noErr;
            break;
        case 'PHIS':
            remember_HISTOGRAM_Pane_Values();
            pprintf("\n Histogram values:\n");
            for(i=0; i<256; i++)
                    pprintf("%d\t%d\r",i,histo[i]);
            stopModalLoop = FALSE;
            result = noErr;
            break;
        case 'CTRS':
            GetControlData( cont_NCtr_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
            nlevls = CFStringGetIntValue( text );
            CFRelease( text );
            if(nlevls < 1) nlevls = 1;
            if(nlevls > MAXNOCTRS) nlevls = MAXNOCTRS;
            for(i=0; i<nlevls; i++){
                clevls[i] = (i+1)*1.0/(nlevls+1);
                SET_CONTROL_FVALUE(clevls[i],cont_CValues_ControlID[i],cont_CValues_Field[i]);
            }

            stopModalLoop = FALSE;
            result = noErr;
            break;
    }
    // Stop the modal loop.
    if (stopModalLoop)
    {
        QuitAppModalLoopForWindow((WindowRef)userData);
        
    }
    //Return how we handled the event.
    return result;
}


// Set SurfaceView Dialog event handler
pascal OSStatus SetSurfaceView_EventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
    OSStatus 		result = eventNotHandledErr;
    HICommand		command;
    Boolean		stopModalLoop = FALSE;
    
    
    
    
    Point		mousePt;
    UInt32 event_info;
    
    event_info = GetEventClass (event);
    
    if(event_info == kEventClassCommand){
    
        // Get the HI Command
        GetEventParameter (event, kEventParamDirectObject, typeHICommand, NULL, 
                                sizeof (HICommand), NULL, &command);
        // Look for commands
        switch (command.commandID)
        {
            case 'DONE':
				QDEndCGContext (GetWindowPort(SetSurfaceView_window), &surfviewContext);
                result = noErr;
				SetThemeCursor( kThemeArrowCursor );
                stopModalLoop = TRUE;
                break;
        }
        // Stop the modal loop.
        if (stopModalLoop)
        {
            QuitAppModalLoopForWindow((WindowRef)userData);
            
        }
    } else {
        event_info = GetEventKind (event);
        switch(event_info)
        {
             case kEventMouseDown:
                button_state = 1;
                result = eventNotHandledErr;
                break;
             case kEventMouseUp:
                button_state = 0;
                printf("up\n");
                break;
             case kEventMouseDragged:
                result = noErr;
                //if(button_state){
                    GetMouse(&mousePt);
                    drawviewdialog(mousePt);
                    //printf("%d %d\n",mousePt.h,mousePt.v);
                //}
                break;
        
        }
       
    }
    
    //Return how we handled the event.
    return result;
}

OSErr oma_prefs_CommandHandler(AppleEvent * theAppleEvent,AppleEvent * reply,SInt32 handlerRefcon)
{
        IBNibRef nibRef;
        
        EventTypeSpec 	dialogSpec = {kEventClassCommand, kEventCommandProcess };
        EventHandlerUPP	dialogUPP;
       
        OSStatus err = eventNotHandledErr;    
        
        // Get the window from the nib and show it
        err = CreateNibReference( CFSTR("oma"), &nibRef );
        err = CreateWindowFromNib( nibRef, CFSTR("OMA_prefs"), &oma_prefs_window );
        DisposeNibReference( nibRef );
        //ShowWindow(oma_prefs_window );

        // Install our event handler
        dialogUPP =  NewEventHandlerUPP (oma_prefs_EventHandler);
        err = InstallWindowEventHandler (oma_prefs_window, dialogUPP, 1, &dialogSpec, (void *) oma_prefs_window, NULL);

        GetControlByID( oma_prefs_window, &tab_ControlID, &tab_Field );
        SetControlValue(tab_Field,lastTabIndex);

        // set all of the pane values
        //set_PIV_Pane_Values();
        set_INTEGRATED_Pane_Values();
        set_CONTOUR_Pane_Values();
        set_HISTOGRAM_Pane_Values();
        set_SURFACE_Pane_Values();
        set_PREFIX_Pane_Values();
        set_PIV2_Pane_Values();
        
        SelectItemOfTabControl(tab_Field);
        
        ShowWindow(oma_prefs_window );

        // Run modally
        RunAppModalLoopForWindow(oma_prefs_window);
        
        HideWindow(oma_prefs_window);
        DisposeWindow(oma_prefs_window);
        DisposeEventHandlerUPP(dialogUPP);

        return err;
}
OSErr SetSurfaceView_CommandHandler()
{
        IBNibRef nibRef;
		SInt16 offset;
        
        //EventTypeSpec 	dialogSpec[2] = {kEventClassCommand, kEventCommandProcess,kEventClassMouse,kEventMouseDragged };
        EventHandlerUPP	dialogUPP;
        
        static EventTypeSpec    eventTypes[4];
		OSStatus err = eventNotHandledErr;
        
        eventTypes[0].eventClass = kEventClassCommand;
        eventTypes[0].eventKind  = kEventCommandProcess;
        eventTypes[1].eventClass = kEventClassMouse;
        eventTypes[1].eventKind  = kEventMouseDragged;
        
        eventTypes[2].eventClass = kEventClassMouse;
        eventTypes[2].eventKind  = kEventMouseDown;
        eventTypes[3].eventClass = kEventClassMouse;
        eventTypes[3].eventKind  = kEventMouseUp;
    
        
        // Get the window from the nib and show it
        err = CreateNibReference( CFSTR("oma"), &nibRef );
        err = CreateWindowFromNib( nibRef, CFSTR("Surface_view"), &SetSurfaceView_window );
        DisposeNibReference( nibRef );
        
        // Install our event handler
        dialogUPP =  NewEventHandlerUPP(SetSurfaceView_EventHandler);
        err = InstallWindowEventHandler(SetSurfaceView_window, dialogUPP, 2,(const EventTypeSpec*) &eventTypes, (void *) SetSurfaceView_window, NULL);

        GetControlByID( SetSurfaceView_window, &SetSurfaceView_done_ControlID, &SetSurfaceView_done_Field );
		GetControlByID( SetSurfaceView_window, &SetSurfaceView_image_ControlID, &SetSurfaceView_image_Field );
		
		GetBestControlRect(SetSurfaceView_image_Field, &surface_image_rect, &offset);	// the full size of the image rect
        
        ShowWindow(SetSurfaceView_window );
        
        setviewdialog();

        // Run modally
        RunAppModalLoopForWindow(SetSurfaceView_window);
        
        HideWindow(SetSurfaceView_window);
        DisposeWindow(SetSurfaceView_window);
        DisposeEventHandlerUPP(dialogUPP);

        return err;
}


//----------------------------------------------------------------------------
//	PIV tab routines
//----------------------------------------------------------------------------
/*
*/
//----------------------------------------------------------------------------
//	INTEGRATED tab routines
//----------------------------------------------------------------------------
int set_INTEGRATED_Pane_Values()
{
    CFStringRef text;
	OSErr err;
    GetControlByID( oma_prefs_window, &int_StdSize_ControlID, &int_StdSize_Field );
    GetControlByID( oma_prefs_window, &int_Int_ControlID, &int_Int_Field );
    GetControlByID( oma_prefs_window, &int_IntX_ControlID, &int_IntX_Field );
    GetControlByID( oma_prefs_window, &int_DatMin_ControlID, &int_DatMin_Field );
    GetControlByID( oma_prefs_window, &int_Max_ControlID, &int_Max_Field );
    GetControlByID( oma_prefs_window, &int_Min_ControlID, &int_Min_Field );
    GetControlByID( oma_prefs_window, &int_Width_ControlID, &int_Width_Field );
    GetControlByID( oma_prefs_window, &int_Height_ControlID, &int_Height_Field );
    
    if(pstdscrnsize) {
        SetControl32BitValue(int_StdSize_Field,1);
    } else {
        SetControl32BitValue(int_StdSize_Field,2);
    }

    if(pdatminmax) SetControl32BitValue(int_DatMin_Field,1);
    else SetControl32BitValue(int_DatMin_Field,2);

    if(pintegrate) SetControl32BitValue(int_Int_Field,1);
    else SetControl32BitValue(int_Int_Field,2);

    if(pintx) SetControl32BitValue(int_IntX_Field,2);
    else SetControl32BitValue(int_IntX_Field,1);
#ifdef FLOAT
    SET_CONTROL_FVALUE(ponemin,int_Min_ControlID,int_Min_Field);
    SET_CONTROL_FVALUE(ponemax,int_Max_ControlID,int_Max_Field);
#else    
    SET_CONTROL_VALUE(ponemin,int_Min_ControlID,int_Min_Field);
    SET_CONTROL_VALUE(ponemax,int_Max_ControlID,int_Max_Field);
#endif
    SET_CONTROL_VALUE(ponewidth,int_Width_ControlID,int_Width_Field);
    SET_CONTROL_VALUE(poneheight,int_Height_ControlID,int_Height_Field);
    
    return 0;
}

int remember_INTEGRATED_Pane_Values()
{
    CFStringRef text;
    
    Size actualSize; 
    
    if(GetControl32BitValue(int_StdSize_Field) == 1)
        pstdscrnsize = true;
    else
        pstdscrnsize = false;

    if(GetControl32BitValue(int_DatMin_Field) == 1)
        pdatminmax = true;
    else
        pdatminmax = false;
    
    if(GetControl32BitValue(int_Int_Field) == 1)
        pintegrate = true;
    else
        pintegrate = false;
        
    if(GetControl32BitValue(int_IntX_Field) == 2)
        pintx = true;
    else
        pintx = false;
    
    GetControlData( int_Min_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
#ifdef FLOAT
	ponemin = CFStringGetDoubleValue( text );
#else
    ponemin = CFStringGetIntValue( text );
#endif
    CFRelease( text );

    GetControlData( int_Max_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
#ifdef FLOAT
	ponemax = CFStringGetDoubleValue( text );
#else
    ponemax = CFStringGetIntValue( text );
#endif
    CFRelease( text );

    GetControlData( int_Width_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    ponewidth = CFStringGetIntValue( text );
    CFRelease( text );

    GetControlData( int_Height_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    poneheight = CFStringGetIntValue( text );
    CFRelease( text );

    return 0;
}
//----------------------------------------------------------------------------
//	CONTOUR tab routines
//----------------------------------------------------------------------------
int set_CONTOUR_Pane_Values()
{
    CFStringRef text;
    int i;
	OSErr err;
    GetControlByID( oma_prefs_window, &cont_StdSize_ControlID, &cont_StdSize_Field );
    GetControlByID( oma_prefs_window, &cont_DatMin_ControlID, &cont_DatMin_Field );
    GetControlByID( oma_prefs_window, &cont_Max_ControlID, &cont_Max_Field );
    GetControlByID( oma_prefs_window, &cont_Min_ControlID, &cont_Min_Field );
    GetControlByID( oma_prefs_window, &cont_Width_ControlID, &cont_Width_Field );
    GetControlByID( oma_prefs_window, &cont_Height_ControlID, &cont_Height_Field );
    GetControlByID( oma_prefs_window, &cont_NCtr_ControlID, &cont_NCtr_Field );
    GetControlByID( oma_prefs_window, &cont_Color_ControlID, &cont_Color_Field );
    
    if(stdscrnsize) {
        SetControl32BitValue(cont_StdSize_Field,1);
    } else {
        SetControl32BitValue(cont_StdSize_Field,2);
    }

    if(datminmax) SetControl32BitValue(cont_DatMin_Field,1);
    else SetControl32BitValue(cont_DatMin_Field,2);

    SetControl32BitValue(cont_Color_Field,colorctrs);
#ifdef FLOAT
	SET_CONTROL_FVALUE(ctrmin,cont_Min_ControlID,cont_Min_Field);
    SET_CONTROL_FVALUE(ctrmax,cont_Max_ControlID,cont_Max_Field);
#else
    SET_CONTROL_VALUE(ctrmin,cont_Min_ControlID,cont_Min_Field);
    SET_CONTROL_VALUE(ctrmax,cont_Max_ControlID,cont_Max_Field);
#endif
    SET_CONTROL_VALUE(lgwidth,cont_Width_ControlID,cont_Width_Field);
    SET_CONTROL_VALUE(lgheight,cont_Height_ControlID,cont_Height_Field);
    SET_CONTROL_VALUE(nlevls,cont_NCtr_ControlID,cont_NCtr_Field);
    
    
    for(i=0; i<MAXNOCTRS; i++){
        cont_CValues_ControlID[i].id = i+koma_prefs_cont_CValues;
        cont_CValues_ControlID[i].signature = koma_prefs_signature;
        GetControlByID( oma_prefs_window, &cont_CValues_ControlID[i], &cont_CValues_Field[i] );
    }
    for(i=0; i<nlevls; i++){
        SET_CONTROL_FVALUE(clevls[i],cont_CValues_ControlID[i],cont_CValues_Field[i]);
    }
    
    
    return 0;
}

int remember_CONTOUR_Pane_Values()
{
    CFStringRef text;
    int i;
    Size actualSize; 
    
    if(GetControl32BitValue(cont_StdSize_Field) == 1)
        stdscrnsize = true;
    else
        stdscrnsize = false;

    if(GetControl32BitValue(cont_DatMin_Field) == 1)
        datminmax = true;
    else
        datminmax = false;
    
    
    GetControlData( cont_Min_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
#ifdef FLOAT
	ctrmin = CFStringGetDoubleValue( text );
#else
    ctrmin = CFStringGetIntValue( text );
#endif
    CFRelease( text );

    GetControlData( cont_Max_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
#ifdef FLOAT
	ctrmax = CFStringGetDoubleValue( text );
#else
    ctrmax = CFStringGetIntValue( text );
#endif
    CFRelease( text );

    GetControlData( cont_Width_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    lgwidth = CFStringGetIntValue( text );
    CFRelease( text );

    GetControlData( cont_Height_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    lgheight = CFStringGetIntValue( text );
    CFRelease( text );

    GetControlData( cont_NCtr_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    nlevls = CFStringGetIntValue( text );
    CFRelease( text );
    
    if(nlevls < 1) nlevls = 1;
    if(nlevls > MAXNOCTRS) nlevls = MAXNOCTRS;

    for(i=0; i<nlevls; i++){
        GetControlData( cont_CValues_Field[i], 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
        clevls[i] = CFStringGetDoubleValue( text );
        CFRelease( text );
    }
            
    colorctrs = GetControl32BitValue(cont_Color_Field);


    return 0;
}
//----------------------------------------------------------------------------
//	HISTOGRAM tab routines
//----------------------------------------------------------------------------
int set_HISTOGRAM_Pane_Values()
{
    CFStringRef text;
	OSErr err;
    GetControlByID( oma_prefs_window, &hist_StdSize_ControlID, &hist_StdSize_Field );
    GetControlByID( oma_prefs_window, &hist_DatMin_ControlID, &hist_DatMin_Field );
    GetControlByID( oma_prefs_window, &hist_Max_ControlID, &hist_Max_Field );
    GetControlByID( oma_prefs_window, &hist_Min_ControlID, &hist_Min_Field );
    GetControlByID( oma_prefs_window, &hist_Width_ControlID, &hist_Width_Field );
    GetControlByID( oma_prefs_window, &hist_Height_ControlID, &hist_Height_Field );
    GetControlByID( oma_prefs_window, &hist_Clear_ControlID, &hist_Clear_Field );
    
    if(hstdscrnsize) {
        SetControl32BitValue(hist_StdSize_Field,1);
    } else {
        SetControl32BitValue(hist_StdSize_Field,2);
    }

    if(hautoscale) SetControl32BitValue(hist_DatMin_Field,1);
    else SetControl32BitValue(hist_DatMin_Field,2);

    if(hclear) SetControl32BitValue(hist_Clear_Field,1);
    else SetControl32BitValue(hist_Clear_Field,2);
#ifdef FLOAT    
    SET_CONTROL_FVALUE(histmin,hist_Min_ControlID,hist_Min_Field);
    SET_CONTROL_FVALUE(histmax,hist_Max_ControlID,hist_Max_Field);
#else
    SET_CONTROL_VALUE(histmin,hist_Min_ControlID,hist_Min_Field);
    SET_CONTROL_VALUE(histmax,hist_Max_ControlID,hist_Max_Field);
#endif    
    SET_CONTROL_VALUE(histwidth,hist_Width_ControlID,hist_Width_Field);
    SET_CONTROL_VALUE(histheight,hist_Height_ControlID,hist_Height_Field);

    return 0;
}

int remember_HISTOGRAM_Pane_Values()
{
    CFStringRef text;
    Size actualSize;
    
    if(GetControl32BitValue(hist_StdSize_Field) == 1)
        hstdscrnsize = true;
    else
        hstdscrnsize = false;

    if(GetControl32BitValue(hist_DatMin_Field) == 1)
        hautoscale = true;
    else
        hautoscale = false;

    if(GetControl32BitValue(hist_Clear_Field) == 1)
        hclear = true;
    else
        hclear = false;
    
    
    GetControlData( hist_Min_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
#ifdef FLOAT
	histmin = CFStringGetDoubleValue( text );
#else
    histmin = CFStringGetIntValue( text );
#endif
    CFRelease( text );

    GetControlData( hist_Max_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
#ifdef FLOAT
	histmax = CFStringGetDoubleValue( text );
#else
    histmax = CFStringGetIntValue( text );
#endif
    CFRelease( text );

    GetControlData( hist_Width_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    histwidth = CFStringGetIntValue( text );
    CFRelease( text );

    GetControlData( hist_Height_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    histheight = CFStringGetIntValue( text );
    CFRelease( text );


    return 0;
}

//----------------------------------------------------------------------------
//	SURFACE tab routines
//----------------------------------------------------------------------------
int set_SURFACE_Pane_Values()
{
    CFStringRef text;
	OSErr err;

    GetControlByID( oma_prefs_window, &surf_StdSize_ControlID, &surf_StdSize_Field );
    GetControlByID( oma_prefs_window, &surf_DatMin_ControlID, &surf_DatMin_Field );
    GetControlByID( oma_prefs_window, &surf_Max_ControlID, &surf_Max_Field );
    GetControlByID( oma_prefs_window, &surf_Min_ControlID, &surf_Min_Field );
    GetControlByID( oma_prefs_window, &surf_Width_ControlID, &surf_Width_Field );
    GetControlByID( oma_prefs_window, &surf_Height_ControlID, &surf_Height_Field );
    GetControlByID( oma_prefs_window, &surf_Color_ControlID, &surf_Color_Field );
    GetControlByID( oma_prefs_window, &surf_White_ControlID, &surf_White_Field );
    GetControlByID( oma_prefs_window, &surf_StdPlot_ControlID, &surf_StdPlot_Field );
    GetControlByID( oma_prefs_window, &surf_Inc_ControlID, &surf_Inc_Field );
    
    if(sstdscrnsize) {
        SetControl32BitValue(surf_StdSize_Field,1);
    } else {
        SetControl32BitValue(surf_StdSize_Field,2);
    }

    if(sdatminmax) SetControl32BitValue(surf_DatMin_Field,1);
    else SetControl32BitValue(surf_DatMin_Field,2);

    if(persp) SetControl32BitValue(surf_StdPlot_Field,2);
    else SetControl32BitValue(surf_StdPlot_Field,1);

#ifdef FLOAT
    SET_CONTROL_FVALUE(surfmin,surf_Min_ControlID,surf_Min_Field);
    SET_CONTROL_FVALUE(surfmax,surf_Max_ControlID,surf_Max_Field);
#else    
    SET_CONTROL_VALUE(surfmin,surf_Min_ControlID,surf_Min_Field);
    SET_CONTROL_VALUE(surfmax,surf_Max_ControlID,surf_Max_Field);
#endif
    SET_CONTROL_VALUE(surfwidth,surf_Width_ControlID,surf_Width_Field);
    SET_CONTROL_VALUE(surfheight,surf_Height_ControlID,surf_Height_Field);
    SET_CONTROL_VALUE(incrementby,surf_Inc_ControlID,surf_Inc_Field);
    
    SetControl32BitValue(surf_Color_Field,scolor);
    SetControl32BitValue(surf_White_Field,plotwhite);

    return 0;
}

int remember_SURFACE_Pane_Values()
{
    CFStringRef text;

    Size actualSize; 
    
    scolor = GetControl32BitValue(surf_Color_Field);
    plotwhite = GetControl32BitValue(surf_White_Field);

    if(GetControl32BitValue(surf_StdSize_Field) == 1)
        sstdscrnsize = true;
    else
        sstdscrnsize = false;

    if(GetControl32BitValue(surf_DatMin_Field) == 1)
        sdatminmax = true;
    else
        sdatminmax = false;

    if(GetControl32BitValue(surf_StdPlot_Field) == 2)
        persp = true;
    else
        persp = false;
    
    
    GetControlData( surf_Min_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
#ifdef FLOAT
    surfmin = CFStringGetDoubleValue( text );
#else
	surfmin = CFStringGetIntValue( text );
#endif
    CFRelease( text );

    GetControlData( surf_Max_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
#ifdef FLOAT
    surfmax = CFStringGetDoubleValue( text );
#else
	surfmax = CFStringGetIntValue( text );
#endif
    CFRelease( text );

    GetControlData( surf_Width_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    surfwidth = CFStringGetIntValue( text );
    CFRelease( text );

    GetControlData( surf_Height_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    surfheight = CFStringGetIntValue( text );
    CFRelease( text );

    GetControlData( surf_Inc_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    incrementby = CFStringGetIntValue( text );
    CFRelease( text );


    return 0;
}
//----------------------------------------------------------------------------
//	PREFIX tab routines
//----------------------------------------------------------------------------
int set_PREFIX_Pane_Values()
{
    CFStringRef text;
    int percent;
    extern float opaque_value;
    extern int thepalette;
	OSErr err;
    GetControlByID( oma_prefs_window, &prefix_GetS_ControlID, &prefix_GetS_Field );
    GetControlByID( oma_prefs_window, &prefix_GetP_ControlID, &prefix_GetP_Field );
    GetControlByID( oma_prefs_window, &prefix_SaveS_ControlID, &prefix_SaveS_Field );
    GetControlByID( oma_prefs_window, &prefix_SaveP_ControlID, &prefix_SaveP_Field );
    GetControlByID( oma_prefs_window, &prefix_MacS_ControlID, &prefix_MacS_Field );
    GetControlByID( oma_prefs_window, &prefix_MacP_ControlID, &prefix_MacP_Field );
    GetControlByID( oma_prefs_window, &prefix_SetS_ControlID, &prefix_SetS_Field );
    GetControlByID( oma_prefs_window, &prefix_SetP_ControlID, &prefix_SetP_Field );
//    GetControlByID( oma_prefs_window, &prefix_LoadSet_ControlID, &prefix_LoadSet_Field );
    GetControlByID( oma_prefs_window, &prefix_pal_ControlID, &prefix_pal_Field );

    GetControlByID( oma_prefs_window, &prefix_transparent_ControlID, &prefix_transparent_Field );
    
    SET_CONTROL_SVALUE( getsuffixbuf, prefix_GetS_ControlID, prefix_GetS_Field );
    SET_CONTROL_SVALUE( getprefixbuf, prefix_GetP_ControlID, prefix_GetP_Field );
    SET_CONTROL_SVALUE( savesuffixbuf, prefix_SaveS_ControlID, prefix_SaveS_Field );
    SET_CONTROL_SVALUE( saveprefixbuf, prefix_SaveP_ControlID, prefix_SaveP_Field );
    SET_CONTROL_SVALUE( macrosuffixbuf, prefix_MacS_ControlID, prefix_MacS_Field );
    SET_CONTROL_SVALUE( macroprefixbuf, prefix_MacP_ControlID, prefix_MacP_Field );
    SET_CONTROL_SVALUE( graphicssuffixbuf, prefix_SetS_ControlID, prefix_SetS_Field );
    SET_CONTROL_SVALUE( graphicsprefixbuf, prefix_SetP_ControlID, prefix_SetP_Field );
    percent = (1 - opaque_value)*100.0+.5;
    SET_CONTROL_VALUE(percent,prefix_transparent_ControlID, prefix_transparent_Field);
    
    SetControl32BitValue(prefix_pal_Field,palette_tab[thepalette]);
    
    
//    SetControl32BitValue(prefix_LoadSet_Field,loadsettings);
    
    return 0;
}

int remember_PREFIX_Pane_Values()
{
    CFStringRef text;
    int percent,pal,i;
    extern float opaque_value;
    extern int thepalette;
    Size actualSize; 
    
    GetControlData( prefix_GetP_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    CFStringGetCString(text,getprefixbuf,PREFIX_CHPERLN, kCFStringEncodingMacRoman);
    CFRelease( text );

    GetControlData( prefix_GetS_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    CFStringGetCString(text,getsuffixbuf,PREFIX_CHPERLN, kCFStringEncodingMacRoman);
    CFRelease( text );

    GetControlData( prefix_SaveP_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    CFStringGetCString(text,saveprefixbuf,PREFIX_CHPERLN, kCFStringEncodingMacRoman);
    CFRelease( text );

    GetControlData( prefix_SaveS_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    CFStringGetCString(text,savesuffixbuf,PREFIX_CHPERLN, kCFStringEncodingMacRoman);
    CFRelease( text );

    GetControlData( prefix_MacP_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    CFStringGetCString(text,macroprefixbuf,PREFIX_CHPERLN, kCFStringEncodingMacRoman);
    CFRelease( text );

    GetControlData( prefix_MacS_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    CFStringGetCString(text,macrosuffixbuf,PREFIX_CHPERLN, kCFStringEncodingMacRoman);
    CFRelease( text );

    GetControlData( prefix_SetP_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    CFStringGetCString(text,graphicsprefixbuf,PREFIX_CHPERLN, kCFStringEncodingMacRoman);
    CFRelease( text );

    GetControlData( prefix_SetS_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    CFStringGetCString(text,graphicssuffixbuf,PREFIX_CHPERLN, kCFStringEncodingMacRoman);
    CFRelease( text );
    
    GetControlData(  prefix_transparent_Field, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
    percent = CFStringGetIntValue( text );
    if(percent >= 0 && percent <= 100){
        opaque_value = (100.0-percent)/100.0;
    }
    CFRelease( text );
    
    pal = GetControl32BitValue(prefix_pal_Field);
    
    for(i=0; i< NUMPAL; i++){
        if(pal == palette_tab[i])
            thepalette = i;
    
    }


    // This is probably a bad idea, so take it out -- this would reload settings whenever preferences were looked at
    // also strange issues when calling getsettings from here that were never resolved
    /*
    loadsettings = GetControl32BitValue(prefix_LoadSet_Field);
    
    if(loadsettings) {
            getsettings();
    }
    */

    

    return 0;
}

//---------------------------------------------------
//---------------------------------------------------
//__________________________________________________________________
//__________________________________________________________________
// PIV2
//__________________________________________________________________
//---------------------------------------------------
//---------------------------------------------------

extern int shiftx,shifty,c_flag,preprocesspiv,plotarrowheads,peakmethod;
extern float loclip,hiclip,pivpixel,pivdelay;
extern char PreMacro[];

WindowRef piv_prefs_window;

// Preprocess Images Controls
ControlRef piv_Preprocess;
ControlID piv_PreprocessID ={ 'Ppiv', 421 };
ControlRef piv_Macro;
ControlID piv_MacroID ={ 'Ppiv', 422 };
ControlRef piv_PreBrowse;
ControlID piv_PreBrowseID ={ 'Ppiv', 423 };

//Auto Clip Controls
ControlRef piv_AutoClip;
ControlID piv_AutoClipID ={ 'Ppiv', 424 };
ControlRef piv_LoValue;
ControlID piv_LoValueID ={ 'Ppiv', 425 };
ControlRef piv_HiValue;
ControlID piv_HiValueID ={ 'Ppiv', 426 };

// Peak Detection Controls
ControlRef piv_PeakDetection;
ControlID piv_PeakDetectionID ={ 'Ppiv', 427 };

// PIV Parameter Controls
ControlRef piv_FFTSize;
ControlID piv_FFTSizeID ={ 'Ppiv', 428 };
ControlRef piv_FFTInc;
ControlID piv_FFTIncID ={ 'Ppiv', 429 };
ControlRef piv_FFTMode;
ControlID piv_FFTModeID ={ 'Ppiv', 430 };

// Velocity and Length Controls
ControlRef piv_Pixel;
ControlID piv_PixelID ={ 'Ppiv', 431 };
ControlRef piv_Delay;
ControlID piv_DelayID ={ 'Ppiv', 432 };

// PIV Plotting Controls
ControlRef piv_Plot;
ControlID piv_PlotID ={ 'Ppiv', 433 };
ControlRef piv_Label;
ControlID piv_LabelID ={ 'Ppiv', 434 };
ControlRef piv_ArrowHeads;
ControlID piv_ArrowHeadsID ={ 'Ppiv', 435 };
ControlRef piv_Scale;
ControlID piv_ScaleID ={ 'Ppiv', 436 };
ControlRef piv_DirOn;
ControlID piv_DirOnID ={ 'Ppiv', 437 };
ControlRef piv_Direction;
ControlID piv_DirectionID ={ 'Ppiv', 438 };

// Conditional PIV Processing Controls
ControlRef piv_CondPiv;
ControlID piv_CondPivID ={ 'Ppiv', 439 };
ControlRef piv_Mask;
ControlID piv_MaskID ={ 'Ppiv', 440 };
ControlRef piv_MaskBtn;
ControlID piv_MaskBtnID ={ 'Ppiv', 441 };
ControlRef piv_MaskMode;
ControlID piv_MaskModeID ={ 'Ppiv', 442 };

// Shift values
ControlRef piv_ShiftX;
ControlID piv_ShiftXID ={ 'Ppiv', 444 };
ControlRef piv_ShiftY;
ControlID piv_ShiftYID ={ 'Ppiv', 445 };
// Preferences Dialog event handler
pascal OSStatus piv_prefs_EventHandler (EventHandlerCallRef myHandler, EventRef event, void *userData)
{
    OSStatus 		result = eventNotHandledErr;
    HICommand	command;
    Boolean		stopModalLoop = FALSE;
   

    // Get the HI Command
    GetEventParameter (event, kEventParamDirectObject, typeHICommand, NULL,
                       sizeof (HICommand), NULL, &command);
    // Look for commands


    switch (command.commandID)
    {
        case 'DONE':
            get_PIV_Values();
            stopModalLoop = TRUE;
            result = noErr;
            break;
        case 'CNCL':
            stopModalLoop = TRUE;
            result = noErr;
            break;
    }
    // Stop the modal loop.
    if (stopModalLoop)
    {
        QuitAppModalLoopForWindow((WindowRef)userData);

    }
    //Return how we handled the event.
    return result;
}

int set_PIV_Values()
{
    CFStringRef text;
    OSStatus err;

    GetControlByID( piv_prefs_window, &piv_PreprocessID, &piv_Preprocess );
    GetControlByID( piv_prefs_window, &piv_MacroID, &piv_Macro );
    GetControlByID( piv_prefs_window, &piv_PreBrowseID, &piv_PreBrowse );

    GetControlByID( piv_prefs_window, &piv_AutoClipID, &piv_AutoClip );
    GetControlByID( piv_prefs_window, &piv_LoValueID, &piv_LoValue );
    GetControlByID( piv_prefs_window, &piv_HiValueID, &piv_HiValue );

    GetControlByID( piv_prefs_window, &piv_PeakDetectionID, &piv_PeakDetection );

    GetControlByID( piv_prefs_window, &piv_FFTSizeID, &piv_FFTSize );
    GetControlByID( piv_prefs_window, &piv_FFTIncID, &piv_FFTInc );
    GetControlByID( piv_prefs_window, &piv_FFTModeID, &piv_FFTMode );
    GetControlByID( piv_prefs_window, &piv_ShiftXID, &piv_ShiftX );
    GetControlByID( piv_prefs_window, &piv_ShiftYID, &piv_ShiftY );

    GetControlByID( piv_prefs_window, &piv_PixelID, &piv_Pixel );
    GetControlByID( piv_prefs_window, &piv_DelayID, &piv_Delay );
    
    GetControlByID( piv_prefs_window, &piv_PlotID, &piv_Plot );
    GetControlByID( piv_prefs_window, &piv_LabelID, &piv_Label );
    GetControlByID( piv_prefs_window, &piv_ArrowHeadsID, &piv_ArrowHeads );
    GetControlByID( piv_prefs_window, &piv_ScaleID, &piv_Scale );
    GetControlByID( piv_prefs_window, &piv_DirOnID, &piv_DirOn );
    GetControlByID( piv_prefs_window, &piv_DirectionID, &piv_Direction );

    GetControlByID( piv_prefs_window, &piv_CondPivID, &piv_CondPiv );
    GetControlByID( piv_prefs_window, &piv_MaskID, &piv_Mask );
    GetControlByID( piv_prefs_window, &piv_MaskBtnID, &piv_MaskBtn );
    GetControlByID( piv_prefs_window, &piv_MaskModeID, &piv_MaskMode );

    // Preprocessing Values
    SetControl32BitValue(piv_Preprocess,preprocesspiv);
    SET_CONTROL_SVALUE(PreMacro,piv_MacroID,piv_Macro);
    SetControl32BitValue(piv_AutoClip,autoclip);
    SET_CONTROL_FVALUE(loclip,piv_LoValueID,piv_LoValue);
    SET_CONTROL_FVALUE(hiclip,piv_HiValueID,piv_HiValue);

    // Peak Detection Values
    SetControl32BitValue(piv_PeakDetection,peakmethod+1);
    // PIV Values
    SET_CONTROL_VALUE(boxinc,piv_FFTIncID,piv_FFTInc);
    SET_CONTROL_VALUE(fftsize,piv_FFTSizeID,piv_FFTSize);
    SetControl32BitValue(piv_FFTMode,2);
    SET_CONTROL_VALUE(shiftx,piv_ShiftXID,piv_ShiftX);
    SET_CONTROL_VALUE(shifty,piv_ShiftYID,piv_ShiftY);

    // Velocity and Length Values
    SET_CONTROL_FVALUE(pivpixel,piv_PixelID,piv_Pixel);
    SET_CONTROL_FVALUE(pivdelay,piv_DelayID,piv_Delay);

    // Plotting Values
    SetControl32BitValue(piv_Plot,plotduringpiv);
    SetControl32BitValue(piv_Label,labelplot);
    SetControl32BitValue(piv_ArrowHeads,plotarrowheads);
    SET_CONTROL_FVALUE(pivscale,piv_ScaleID,piv_Scale);
    SetControl32BitValue(piv_Direction,plot_dir_pref+1);

    // Conditional PIV Values
    SetControl32BitValue(piv_CondPiv,c_flag);
    SetControl32BitValue(piv_MaskMode,1);
    
    return err;
}

int get_PIV_Values()
{
    CFStringRef text;
    int temp;
    Size actualSize;

    preprocesspiv = GetControl32BitValue(piv_Preprocess);
    /*GetControlData( piv_Macro, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
    PreMacro = text;
    CFRelease( text ); //*/
    autoclip = GetControl32BitValue(piv_AutoClip);
    GetControlData( piv_LoValue, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
    loclip = CFStringGetDoubleValue( text );
    CFRelease( text ); //*/
    GetControlData( piv_HiValue, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
    hiclip = CFStringGetDoubleValue( text );
    CFRelease( text ); //*/

    peakmethod = GetControl32BitValue(piv_PeakDetection);
    peakmethod--;

     GetControlData( piv_FFTSize, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     temp = CFStringGetIntValue( text );
     CFRelease( text );
     GetControlData( piv_FFTInc, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     boxinc = CFStringGetIntValue( text );
     CFRelease( text );  //*/
     GetControlData( piv_ShiftX, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     shiftx = CFStringGetIntValue( text );
     CFRelease( text );
     GetControlData( piv_ShiftY, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     shifty = CFStringGetIntValue( text );
     CFRelease( text );  //*/

     GetControlData( piv_Pixel, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     pivpixel = CFStringGetDoubleValue( text );
     CFRelease( text ); //*/
     GetControlData( piv_Delay, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     pivdelay = CFStringGetDoubleValue( text );
     CFRelease( text ); //*/

     plotduringpiv = GetControl32BitValue(piv_Plot);
     labelplot = GetControl32BitValue(piv_Label);
     plotarrowheads = GetControl32BitValue(piv_ArrowHeads);
     GetControlData( piv_Scale, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     pivscale = CFStringGetDoubleValue( text );
     CFRelease( text ); //*/
    plot_dir_pref = GetControl32BitValue(piv_Direction);
    plot_dir_pref--;

     c_flag = GetControl32BitValue(piv_CondPiv);

     // read in results before leaving -- might want to check if all is ok before actually leaving
     if(is_power_2(temp)){
         fftsize = temp;
         return 0;
     } else {
         beep();
         return -1;
     }
}


/*****************************************************************************
			N U L L F N  - null-function used for code development
*****************************************************************************/
int nullfn()
{
    IBNibRef nibRef;

    EventTypeSpec 	dialogSpec = {kEventClassCommand, kEventCommandProcess };
    EventHandlerUPP	dialogUPP;

    OSStatus err = eventNotHandledErr;

    // Get the window from the nib and show it
    err = CreateNibReference( CFSTR("oma"), &nibRef );
    err = CreateWindowFromNib( nibRef, CFSTR("PIV_prefs"), &piv_prefs_window );
    DisposeNibReference( nibRef );
    //ShowWindow(piv_prefs_window );

    // Install our event handler
    dialogUPP =  NewEventHandlerUPP (piv_prefs_EventHandler);
    err = InstallWindowEventHandler (piv_prefs_window, dialogUPP, 1, &dialogSpec, (void *) piv_prefs_window, NULL);

    // set all of the pane values
    set_PIV_Values();

    ShowWindow(piv_prefs_window );

    // Run modally
    RunAppModalLoopForWindow(piv_prefs_window);

    HideWindow(piv_prefs_window);
    DisposeWindow(piv_prefs_window);
    DisposeEventHandlerUPP(dialogUPP);
    return err;
} /* nullfn */
//__________________________________________________________________
int set_PIV2_Pane_Values()
{
    CFStringRef text;
    OSStatus err;

    GetControlByID( oma_prefs_window, &piv_PreprocessID, &piv_Preprocess );
    GetControlByID( oma_prefs_window, &piv_MacroID, &piv_Macro );
    GetControlByID( oma_prefs_window, &piv_PreBrowseID, &piv_PreBrowse );

    GetControlByID( oma_prefs_window, &piv_AutoClipID, &piv_AutoClip );
    GetControlByID( oma_prefs_window, &piv_LoValueID, &piv_LoValue );
    GetControlByID( oma_prefs_window, &piv_HiValueID, &piv_HiValue );

    GetControlByID( oma_prefs_window, &piv_PeakDetectionID, &piv_PeakDetection );

    GetControlByID( oma_prefs_window, &piv_FFTSizeID, &piv_FFTSize );
    GetControlByID( oma_prefs_window, &piv_FFTIncID, &piv_FFTInc );
    GetControlByID( oma_prefs_window, &piv_FFTModeID, &piv_FFTMode );
    GetControlByID( oma_prefs_window, &piv_ShiftXID, &piv_ShiftX );
    GetControlByID( oma_prefs_window, &piv_ShiftYID, &piv_ShiftY );

    GetControlByID( oma_prefs_window, &piv_PixelID, &piv_Pixel );
    GetControlByID( oma_prefs_window, &piv_DelayID, &piv_Delay );
    
    GetControlByID( oma_prefs_window, &piv_PlotID, &piv_Plot );
    GetControlByID( oma_prefs_window, &piv_LabelID, &piv_Label );
    GetControlByID( oma_prefs_window, &piv_ArrowHeadsID, &piv_ArrowHeads );
    GetControlByID( oma_prefs_window, &piv_ScaleID, &piv_Scale );
    GetControlByID( oma_prefs_window, &piv_DirOnID, &piv_DirOn );
    GetControlByID( oma_prefs_window, &piv_DirectionID, &piv_Direction );

    GetControlByID( oma_prefs_window, &piv_CondPivID, &piv_CondPiv );
    GetControlByID( oma_prefs_window, &piv_MaskID, &piv_Mask );
    GetControlByID( oma_prefs_window, &piv_MaskBtnID, &piv_MaskBtn );
    GetControlByID( oma_prefs_window, &piv_MaskModeID, &piv_MaskMode );
    
    GetControlByID( oma_prefs_window, &piv_alt_ControlID, &piv_alt_Field );

    // Preprocessing Values
    SetControl32BitValue(piv_Preprocess,preprocesspiv);
    SET_CONTROL_SVALUE(PreMacro,piv_MacroID,piv_Macro);
    SetControl32BitValue(piv_AutoClip,autoclip);
    SET_CONTROL_FVALUE(loclip,piv_LoValueID,piv_LoValue);
    SET_CONTROL_FVALUE(hiclip,piv_HiValueID,piv_HiValue);

    // Peak Detection Values
    SetControl32BitValue(piv_PeakDetection,peakmethod+1);
    
    // PIVALT Values
    SetControl32BitValue(piv_alt_Field,alter_vectors+1);
    
    // PIV Values
    SET_CONTROL_VALUE(boxinc,piv_FFTIncID,piv_FFTInc);
    SET_CONTROL_VALUE(fftsize,piv_FFTSizeID,piv_FFTSize);
    SetControl32BitValue(piv_FFTMode,2);
    SET_CONTROL_VALUE(shiftx,piv_ShiftXID,piv_ShiftX);
    SET_CONTROL_VALUE(shifty,piv_ShiftYID,piv_ShiftY);

    // Velocity and Length Values
    SET_CONTROL_FVALUE(pivpixel,piv_PixelID,piv_Pixel);
    SET_CONTROL_FVALUE(pivdelay,piv_DelayID,piv_Delay);

    // Plotting Values
    SetControl32BitValue(piv_Plot,plotduringpiv);
    SetControl32BitValue(piv_Label,labelplot);
    SetControl32BitValue(piv_ArrowHeads,plotarrowheads);
    SET_CONTROL_FVALUE(pivscale,piv_ScaleID,piv_Scale);
    SetControl32BitValue(piv_Direction,plot_dir_pref+1);

    // Conditional PIV Values
    SetControl32BitValue(piv_CondPiv,c_flag);
    SetControl32BitValue(piv_MaskMode,1);
    
    return err;
}

int remember_PIV2_Pane_Values()
{
    CFStringRef text;
    int temp;
    Size actualSize;

    preprocesspiv = GetControl32BitValue(piv_Preprocess);
    autoclip = GetControl32BitValue(piv_AutoClip);
    GetControlData( piv_LoValue, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
    loclip = CFStringGetDoubleValue( text );
    CFRelease( text ); //*/
    GetControlData( piv_HiValue, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
    hiclip = CFStringGetDoubleValue( text );
    CFRelease( text ); //*/

    peakmethod = GetControl32BitValue(piv_PeakDetection);
    peakmethod--;

     GetControlData( piv_FFTSize, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     temp = CFStringGetIntValue( text );
     CFRelease( text );
     GetControlData( piv_FFTInc, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     boxinc = CFStringGetIntValue( text );
     CFRelease( text );  //*/
     GetControlData( piv_ShiftX, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     shiftx = CFStringGetIntValue( text );
     CFRelease( text );
     GetControlData( piv_ShiftY, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     shifty = CFStringGetIntValue( text );
     CFRelease( text );  //*/

     GetControlData( piv_Pixel, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     pivpixel = CFStringGetDoubleValue( text );
     CFRelease( text ); //*/
     GetControlData( piv_Delay, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     pivdelay = CFStringGetDoubleValue( text );
     CFRelease( text ); //*/

     plotduringpiv = GetControl32BitValue(piv_Plot);
     labelplot = GetControl32BitValue(piv_Label);
     plotarrowheads = GetControl32BitValue(piv_ArrowHeads);
     GetControlData( piv_Scale, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
     pivscale = CFStringGetDoubleValue( text );
     CFRelease( text ); //*/
     plot_dir_pref = GetControl32BitValue(piv_Direction);
     plot_dir_pref--;
    
     alter_vectors = GetControl32BitValue(piv_alt_Field);
     alter_vectors--;


     c_flag = GetControl32BitValue(piv_CondPiv);

     // read in results before leaving -- might want to check if all is ok before actually leaving
     if(is_power_2(temp)){
         fftsize = temp;
         return 0;
     } else {
         beep();
         return -1;
     }
}

//_______________________________
void setprefix()
{
    do_preference_tab_number(PREFIX_TAB);
}

void setcontour()
{
    do_preference_tab_number(CONTOUR_TAB);
}

void setsurface()
{
    do_preference_tab_number(SURFACE_TAB);
}

void sethist()
{
    do_preference_tab_number(HISTOGRAM_TAB);
}

void setplotone()
{
    do_preference_tab_number(INTEGRATED_TAB);
}


int do_preference_tab_number(int tab_no)
{

        IBNibRef nibRef;
        
        EventTypeSpec 	dialogSpec = {kEventClassCommand, kEventCommandProcess };
        EventHandlerUPP	dialogUPP;
       
        OSStatus err = eventNotHandledErr;    
        
        // Get the window from the nib and show it
        err = CreateNibReference( CFSTR("oma"), &nibRef );
        err = CreateWindowFromNib( nibRef, CFSTR("OMA_prefs"), &oma_prefs_window );
        DisposeNibReference( nibRef );
       
        // Install our event handler
        dialogUPP =  NewEventHandlerUPP (oma_prefs_EventHandler);
        err = InstallWindowEventHandler (oma_prefs_window, dialogUPP, 1, &dialogSpec, (void *) oma_prefs_window, NULL);

        GetControlByID( oma_prefs_window, &tab_ControlID, &tab_Field );
        lastTabIndex = tab_no;
        SetControlValue(tab_Field,lastTabIndex);

        // set all of the pane values
        //set_PIV_Pane_Values();
        set_INTEGRATED_Pane_Values();
        set_CONTOUR_Pane_Values();
        set_HISTOGRAM_Pane_Values();
        set_SURFACE_Pane_Values();
        set_PREFIX_Pane_Values();
        set_PIV2_Pane_Values();
        
        SelectItemOfTabControl(tab_Field);
        
        ShowWindow(oma_prefs_window );

        // Run modally
        RunAppModalLoopForWindow(oma_prefs_window);
        
        HideWindow(oma_prefs_window);
        DisposeWindow(oma_prefs_window);
        DisposeEventHandlerUPP(dialogUPP);
        return err;
        
}


int setup_status()
{

	//extern int macval;

    //extern Rect cbrect;
    //extern CGrafPtr colorbarptr;			// the off screen port with colorbar info 

    //CGrafPtr 		savePort;
    //GDHandle		saveDevice;
    //GetGWorld(&savePort,&saveDevice);
    //save_state();
	
    update_status();
    printxyzstuff(0,0,-1);
    //update_status();
    //update_status();
    
    // draw the color bar 
    //CopyBits((BitMap*) *(colorbarptr->portPixMap),&colorminmaxdlog->portBits,&((*(colorbarptr->portPixMap))->bounds),&r,0,0); 
    //SetPortWindowPort(Status_window);
    /*CopyBits( (BitMap*) *GetPortPixMap(colorbarptr),
                                            GetPortBitMapForCopyBits(GetWindowPort(Status_window)),
                                            &cbrect,&cbarRect,0,0);
	*/
    //SetGWorld(savePort,saveDevice);
	//restore_state();
    return 0;
}


// ----------------------------------------------------------------------
//
// Status Window event handler
//
// ----------------------------------------------------------------------
pascal OSStatus Status_EventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
    OSStatus 		result = eventNotHandledErr;
    HICommand		command;
    EventRef MyEvent;
    Point mousePt;
    HIPoint loc;
    HIViewRef outview;
    UInt32 event_info;
    //CGrafPtr 		savePort;
    //GDHandle		saveDevice;
	OSErr err;
    extern DATAWORD	cmin,cmax;
	static int cmnd_done = 0;
	void do_rgb_mult(float x, float y, float z);
	extern short newwindowflag;	
	short save_flag=newwindowflag;
	int drgbq();

    
    event_info = GetEventClass (event);
    //printf(" event info %d\n",event_info);
    
    if(event_info == kEventClassCommand){
    
        // Get the HI Command
        GetEventParameter (event, kEventParamDirectObject, typeHICommand, NULL, 
                                sizeof (HICommand), NULL, &command);
        // Look for commands
		//result = noErr;
        switch (command.commandID)
        {
            case 'DCMX':
				cmnd_done++;
				if(cmnd_done > 1) break;
                cmax -= cminmaxinc/100.0*(max - min);
                update_status();
                do_autoupdate();
                result = noErr;
                break;
            case 'ICMX':
				cmnd_done++;
				if(cmnd_done > 1) break;
                cmax += cminmaxinc/100.0*(max - min);
				//printf("%d\n",cminmaxinc);
                update_status();
                do_autoupdate();
                result = noErr;
                break;
            case 'DCMN':
				cmnd_done++;
				if(cmnd_done > 1) break;
                cmin -= cminmaxinc/100.0*(max - min);
                update_status();
                do_autoupdate();
                result = noErr;
                break;
            case 'ICMN':
				cmnd_done++;
				if(cmnd_done > 1) break;
                cmin += cminmaxinc/100.0*(max - min);
                update_status();
                do_autoupdate();
                result = noErr;
                break;
            case 'PLUS':
                docalcs = false;
                showselection = false;
                showruler = false;
                plotline = false;
                toolselected = 0;
                update_status();
                printxyzstuff(0,0,-1);
                result = noErr;
                break;
            case 'RULE':
                toolselected = 3;
                showselection = false;
                showruler = true;
                docalcs = true;
                plotline = false;
                update_status();
                printxyzstuff(0,0,-1);
                result = noErr;
                break;
            case 'PLOT':
                toolselected = 4;    
                showselection = false;
                showruler = true;
                docalcs = true;	
                plotline = true;
                update_status();
                printxyzstuff(0,0,-1);
                result = noErr;
                break;
            case 'RECT':
                toolselected = 1;    
                docalcs = false;
                showselection = true;
                showruler = false;
                plotline = false;
                update_status();
                printxyzstuff(0,0,-1);
                result = noErr;
                break;
            case 'CBOX':
                toolselected = 2;    
                docalcs = true;
                showselection = true;
                showruler = false;
                plotline = false;
                update_status();
                printxyzstuff(0,0,-1);
                result = noErr;
                break;
            case 'SCAL':
                if(autoscale)
                    autoscale = false;
                else
                    autoscale = true;
                do_autoupdate();
                result = noErr;
                break;
            case 'UPDA':
                if(autoupdate)
                    autoupdate = false;
                else
                    autoupdate = true;
                do_autoupdate();
                result = noErr;
                break;
/*            case 'MRGB':
				do_rgb_mult(r_scale , g_scale , b_scale);
				r_scale = g_scale = b_scale = 1.0;
				update_status();
                result = noErr;
                break;
            case 'RRGB':
				r_scale = g_scale = b_scale = 1.0;
				newwindowflag=0;
				drgbq();
				newwindowflag = save_flag;				
				update_status();
                result = noErr;
                break;
 */
            case 'swin':
                // This is the select window event that comes through in a document window.
                // In order to make it so that the command is done on the initial click
                // (rather than requiring a select click and then a click to make the command work)
                // this creates a mouse event and sends it using HIViewClick
                // that will then get back to the appropriate part of this handler to do the command
                // This requires the HIPoint coordinates, which is done clumsily here -- there must be a better way
                // printf("swin\n");
				cmnd_done = 0;
                //GetGWorld(&savePort,&saveDevice);	
				save_state();
                SetPortWindowPort( Status_window);   

                err = CreateEvent (NULL,kEventClassMouse, kEventMouseDown,GetCurrentEventTime(), kEventAttributeNone,&MyEvent);
                err = SetEventParameter( MyEvent, kEventParamWindowRef, typeWindowRef, sizeof( WindowRef ), &Status_window);
                GetMouse(&mousePt);
                //SetGWorld(savePort,saveDevice);
				restore_state();
                err = SetEventParameter( MyEvent, kEventParamMouseLocation, typeQDPoint, sizeof( Point ), &mousePt);
                loc.x = mousePt.h;
                loc.y = mousePt.v+20;       // I'm guessing this is the thickness of the top bar
                err = SetEventParameter( MyEvent, kEventParamWindowMouseLocation, typeHIPoint, sizeof( HIPoint ), &loc);
                err = HIViewGetViewForMouseEvent(HIViewGetRoot(Status_window),MyEvent,&outview);
                err = HIViewClick(outview,MyEvent);
				// something to be sure this window is visible
				BringToFront(Status_window);
				//WindowRef the_wind = GetUserFocusWindow();
				//ActivateWindow(Status_window,true);
				//SetUserFocusWindow(the_wind);
				//ShowWindow(Status_window);
				//
				result = noErr;
                break;
        }
        
    } else if(event_info ==  kEventClassControl) {   
		SysBeep(1);	
		printf("drop\n");						// this doesn't seem to work for getting an event when something is dropped onto the status window
		result = noErr;
	} else {                               // only command and update events done here currently
        assigncolor1();                     // need to redraw colorbar
		result = noErr;
		
    }
	//Return how we handled the event.
	//ActivateWindow(Status_window,false);
    return result;
}

// ----------------------------------------------------------------------
//
// RGB_Tools Window event handler
//
// ----------------------------------------------------------------------
pascal OSStatus RGB_Tools_EventHandler(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
    OSStatus 		result = eventNotHandledErr;
    HICommand		command;
    EventRef MyEvent;
    Point mousePt;
    HIPoint loc;
    HIViewRef outview;
    UInt32 event_info;
    //CGrafPtr 		savePort;
    //GDHandle		saveDevice;
	OSErr err;
    extern DATAWORD	cmin,cmax;
	static int cmnd_done = 0;
	void do_rgb_mult(float x, float y, float z);
	extern short newwindowflag;	
	short save_flag=newwindowflag;
	int drgbq();
    void gamma_set(float,float,float);
    
    event_info = GetEventClass (event);
    //printf(" event info %d\n",event_info);
    
    if(event_info == kEventClassCommand){
		
        // Get the HI Command
        GetEventParameter (event, kEventParamDirectObject, typeHICommand, NULL, 
						   sizeof (HICommand), NULL, &command);
        // Look for commands
		//result = noErr;
        switch (command.commandID)
        {
            case 'MRGB':
				do_rgb_mult(r_scale , g_scale , b_scale);
				r_scale = g_scale = b_scale = 1.0;
				update_status();
                result = noErr;
                break;
            case 'RRGB':
				r_scale = g_scale = b_scale = 1.0;
				newwindowflag=0;
				drgbq();
				newwindowflag = save_flag;				
				update_status();
                result = noErr;
                break;
            case 'MGAM':
				gamma_set(r_gamma , g_gamma , b_gamma);
				r_gamma = g_gamma = b_gamma = 1.0;
				update_status();
                result = noErr;
                break;
            case 'RSET':
				r_gamma = g_gamma = b_gamma = 1.0;
				newwindowflag=0;
				drgbq();
				newwindowflag = save_flag;
				update_status();
                result = noErr;
                break;

            case 'swin':
                // This is the select window event that comes through in a document window.
                // In order to make it so that the command is done on the initial click
                // (rather than requiring a select click and then a click to make the command work)
                // this creates a mouse event and sends it using HIViewClick
                // that will then get back to the appropriate part of this handler to do the command
                // This requires the HIPoint coordinates, which is done clumsily here -- there must be a better way
                // printf("swin\n");
				cmnd_done = 0;
                //GetGWorld(&savePort,&saveDevice);	
				save_state();
                SetPortWindowPort( RGB_Tools_window);   
				
                err = CreateEvent (NULL,kEventClassMouse, kEventMouseDown,GetCurrentEventTime(), kEventAttributeNone,&MyEvent);
                err = SetEventParameter( MyEvent, kEventParamWindowRef, typeWindowRef, sizeof( WindowRef ), &RGB_Tools_window);
                GetMouse(&mousePt);
                //SetGWorld(savePort,saveDevice);
				restore_state();
                err = SetEventParameter( MyEvent, kEventParamMouseLocation, typeQDPoint, sizeof( Point ), &mousePt);
                loc.x = mousePt.h;
                loc.y = mousePt.v+20;       // I'm guessing this is the thickness of the top bar
                err = SetEventParameter( MyEvent, kEventParamWindowMouseLocation, typeHIPoint, sizeof( HIPoint ), &loc);
                err = HIViewGetViewForMouseEvent(HIViewGetRoot(RGB_Tools_window),MyEvent,&outview);
                err = HIViewClick(outview,MyEvent);
				// something to be sure this window is visible
				BringToFront(RGB_Tools_window);
				//
				result = noErr;
                break;
        }
        
    }
    return result;
}

int do_autoupdate()
{
    short savenewwindowstatus,save_scale_status;
    extern short newwindowflag;
	extern int gwnum;
	int drgbq(),dquartz();
    
    if(autoupdate) {
		savenewwindowstatus = newwindowflag;
		newwindowflag = false;
		save_scale_status = autoscale;
		autoscale = false;
		if(oma_wind[gwnum-1].windowtype == QRGBMAP)
			drgbq(0,0);
		else if(oma_wind[gwnum-1].windowtype == BITMAP)
			displa(0,0);				// display without changing window name 
		else
			dquartz(0,0);
		newwindowflag = savenewwindowstatus;
		autoscale = save_scale_status;
    }
    return 0;
}


int nib_status_window()
{
        //extern int macval;
        IBNibRef nibRef;
        
        EventHandlerUPP	dialogUPP;
        int i;
        
        static EventTypeSpec    eventTypes[6];
        
        CFStringRef text;
        void scroll_process_nib();
        ControlActionUPP	ctlUPP = NewControlActionUPP((ControlActionProcPtr) scroll_process_nib );
	/*
		void r_scroll_process_nib();
		ControlActionUPP	r_ctlUPP = NewControlActionUPP((ControlActionProcPtr) r_scroll_process_nib );

		void g_scroll_process_nib();
		ControlActionUPP	g_ctlUPP = NewControlActionUPP((ControlActionProcPtr) g_scroll_process_nib );

		void b_scroll_process_nib();
		ControlActionUPP	b_ctlUPP = NewControlActionUPP((ControlActionProcPtr) b_scroll_process_nib );
*/
        OSStatus err = eventNotHandledErr;  
		
		#define NUM_TO_HANDLE 2	
        
        eventTypes[0].eventClass = kEventClassCommand;
        eventTypes[0].eventKind  = kEventCommandProcess;
        
        eventTypes[1].eventClass = kEventClassWindow;
    eventTypes[1].eventKind  = kEventWindowFocusContent;
/*        eventTypes[2].eventClass =  kEventClassControl;			// not how it's done
        eventTypes[2].eventKind  = kEventControlDragReceive;

        eventTypes[3].eventClass =  kEventClassControl;
        eventTypes[3].eventKind  = kEventControlDragEnter;

        eventTypes[4].eventClass =  kEventClassControl;
        eventTypes[4].eventKind  = kEventControlDragLeave;

        eventTypes[5].eventClass =  kEventClassControl;
        eventTypes[5].eventKind  = kEventControlDragWithin;

*/
		
        /*
        eventTypes[1].eventClass = kEventClassWindow;
        eventTypes[1].eventKind  = kEventWindowUpdate;

        eventTypes[2].eventClass = kEventClassMouse;
        eventTypes[2].eventKind  = kEventMouseDown;
        eventTypes[3].eventClass = kEventClassMouse;
        eventTypes[3].eventKind  = kEventMouseUp;
        
         eventTypes[1].eventKind  = kEventMouseDragged;
        */
        
        
        
        // Get the window from the nib and show it
        err = CreateNibReference( CFSTR("oma"), &nibRef );
        err = CreateWindowFromNib( nibRef, CFSTR("Status"), &Status_window );
        DisposeNibReference( nibRef );
        
        // Install our event handler
        
        dialogUPP =  NewEventHandlerUPP(Status_EventHandler);
        err = InstallWindowEventHandler(Status_window, dialogUPP, NUM_TO_HANDLE,(const EventTypeSpec*) &eventTypes, (void *) Status_window, NULL);

        GetControlByID( Status_window, &status_cmin_ControlID, &status_cmin_Field );
        GetControlByID( Status_window, &status_cmax_ControlID, &status_cmax_Field );
        GetControlByID( Status_window, &status_dcmn_ControlID, &status_dcmn_Field );
        GetControlByID( Status_window, &status_icmn_ControlID, &status_icmn_Field );
        GetControlByID( Status_window, &status_dcmx_ControlID, &status_dcmx_Field );
        GetControlByID( Status_window, &status_icmx_ControlID, &status_icmx_Field );
        GetControlByID( Status_window, &status_autoscale_ControlID, &status_autoscale_Field );
        GetControlByID( Status_window, &status_autoupdate_ControlID, &status_autoupdate_Field );
        
        GetControlByID( Status_window, &status_t1_ControlID, &status_t1_Field );
        GetControlByID( Status_window, &status_t2_ControlID, &status_t2_Field );
        GetControlByID( Status_window, &status_t3_ControlID, &status_t3_Field );
        GetControlByID( Status_window, &status_t4_ControlID, &status_t4_Field );
        GetControlByID( Status_window, &status_t5_ControlID, &status_t5_Field );
        GetControlByID( Status_window, &status_t6_ControlID, &status_t6_Field ); 
        GetControlByID( Status_window, &status_t7_ControlID, &status_t7_Field ); 
        
        GetControlByID( Status_window, &status_cmni_ControlID, &status_cmni_Field ); 
	/*
		GetControlByID( Status_window, &status_rsca_ControlID, &status_rsca_Field );
		GetControlByID( Status_window, &status_gsca_ControlID, &status_gsca_Field );
		GetControlByID( Status_window, &status_bsca_ControlID, &status_bsca_Field );
	*/	
		GetControlByID( Status_window, &status_cbar_ControlID, &status_cbar_Field ); 
        
        GetControlByID( Status_window, &status_cmnmxinc_ControlID, &status_cmnmxinc_Field );
	/*
		GetControlByID( Status_window, &status_rtxt_ControlID, &status_rtxt_Field );
		GetControlByID( Status_window, &status_gtxt_ControlID, &status_gtxt_Field );
		GetControlByID( Status_window, &status_btxt_ControlID, &status_btxt_Field );
	
		GetControlByID( Status_window, &status_rgbm_ControlID, &status_rgbm_Field );
		GetControlByID( Status_window, &status_rrgb_ControlID, &status_rrgb_Field );
   */     
        SetControlAction (status_cmni_Field,ctlUPP);
	/*
		SetControlAction (status_rsca_Field,r_ctlUPP);
		SetControlAction (status_gsca_Field,g_ctlUPP);
		SetControlAction (status_bsca_Field,b_ctlUPP);
     */          
        for(i=0; i<NUMTOOLS; i++){
            status_tool_ControlID[i].signature=kstatus_signature;
            status_tool_ControlID[i].id= kstatus_tool+i;
            GetControlByID( Status_window, &status_tool_ControlID[i], &status_tool_Field[i] );
        }    
                    
        ShowWindow(Status_window);
        
        status_window_inited = true;
        
        sprintf(txt,"Min/Max Increment: %d %%",cminmaxinc);
        SET_CONTROL_SVALUE( txt, status_cmnmxinc_ControlID, status_cmnmxinc_Field );
	
	/* this is done in update_status()
		sprintf(txt,"Red: %.2f \n",r_scale);
		SET_CONTROL_SVALUE( txt, status_rtxt_ControlID, status_rtxt_Field );
		sprintf(txt,"Green: %.2f \n",g_scale);
		SET_CONTROL_SVALUE( txt, status_gtxt_ControlID, status_gtxt_Field );
		sprintf(txt,"Blue: %.2f \n",b_scale);
		SET_CONTROL_SVALUE( txt, status_btxt_ControlID, status_btxt_Field );
	 */
        
        printxyzstuff(0,0,-1);

	update_status();

	assigncolor1();

        return 0;
}

int nib_RGB_Tools_window()
{
	//extern int macval;
	IBNibRef nibRef;
	
	EventHandlerUPP	dialogUPP;
	
	
	static EventTypeSpec    eventTypes[6];
	
	
	//void scroll_process_nib();
	//ControlActionUPP	ctlUPP = NewControlActionUPP((ControlActionProcPtr) scroll_process_nib );
	
	void r_scroll_process_nib();
	ControlActionUPP	r_ctlUPP = NewControlActionUPP((ControlActionProcPtr) r_scroll_process_nib );
	
	void g_scroll_process_nib();
	ControlActionUPP	g_ctlUPP = NewControlActionUPP((ControlActionProcPtr) g_scroll_process_nib );
	
	void b_scroll_process_nib();
	ControlActionUPP	b_ctlUPP = NewControlActionUPP((ControlActionProcPtr) b_scroll_process_nib );
	
	void rgam_scroll_process_nib();
	ControlActionUPP	rgam_ctlUPP = NewControlActionUPP((ControlActionProcPtr) rgam_scroll_process_nib );
	
	void ggam_scroll_process_nib();
	ControlActionUPP	ggam_ctlUPP = NewControlActionUPP((ControlActionProcPtr) ggam_scroll_process_nib );
	
	void bgam_scroll_process_nib();
	ControlActionUPP	bgam_ctlUPP = NewControlActionUPP((ControlActionProcPtr) bgam_scroll_process_nib );

	OSStatus err = eventNotHandledErr;
	
#define NUM_TO_HANDLE 2	
	
	eventTypes[0].eventClass = kEventClassCommand;
	eventTypes[0].eventKind  = kEventCommandProcess;
	
	eventTypes[1].eventClass = kEventClassWindow;
	eventTypes[1].eventKind  = kEventWindowFocusContent;
	
	
	// Get the window from the nib and show it
	err = CreateNibReference( CFSTR("oma"), &nibRef );
	err = CreateWindowFromNib( nibRef, CFSTR("RGB_Tools"), &RGB_Tools_window );
	DisposeNibReference( nibRef );
	
	// Install our event handler
	
	dialogUPP =  NewEventHandlerUPP(RGB_Tools_EventHandler);
	err = InstallWindowEventHandler(RGB_Tools_window, dialogUPP, NUM_TO_HANDLE,(const EventTypeSpec*) &eventTypes, (void *) RGB_Tools_window, NULL);

	GetControlByID( RGB_Tools_window, &status_rsca_ControlID, &status_rsca_Field );
	GetControlByID( RGB_Tools_window, &status_gsca_ControlID, &status_gsca_Field );
	GetControlByID( RGB_Tools_window, &status_bsca_ControlID, &status_bsca_Field );
	
	
	GetControlByID( RGB_Tools_window, &status_rtxt_ControlID, &status_rtxt_Field );
	GetControlByID( RGB_Tools_window, &status_gtxt_ControlID, &status_gtxt_Field );
	GetControlByID( RGB_Tools_window, &status_btxt_ControlID, &status_btxt_Field );
	
	GetControlByID( RGB_Tools_window, &status_rgbm_ControlID, &status_rgbm_Field );
	GetControlByID( RGB_Tools_window, &status_rrgb_ControlID, &status_rrgb_Field );

    GetControlByID( RGB_Tools_window, &status_rgam_ControlID, &status_rgam_Field );
	GetControlByID( RGB_Tools_window, &status_ggam_ControlID, &status_ggam_Field );
	GetControlByID( RGB_Tools_window, &status_bgam_ControlID, &status_bgam_Field );
	
	
	GetControlByID( RGB_Tools_window, &status_rgamtxt_ControlID, &status_rgamtxt_Field );
	GetControlByID( RGB_Tools_window, &status_ggamtxt_ControlID, &status_ggamtxt_Field );
	GetControlByID( RGB_Tools_window, &status_bgamtxt_ControlID, &status_bgamtxt_Field );
	
	GetControlByID( RGB_Tools_window, &status_rgbgam_ControlID, &status_rgbgam_Field );
	GetControlByID( RGB_Tools_window, &status_rrgbgam_ControlID, &status_rrgbgam_Field );

	SetControlAction (status_rsca_Field,r_ctlUPP);
	SetControlAction (status_gsca_Field,g_ctlUPP);
	SetControlAction (status_bsca_Field,b_ctlUPP);

	SetControlAction (status_rgam_Field,rgam_ctlUPP);
	SetControlAction (status_ggam_Field,ggam_ctlUPP);
	SetControlAction (status_bgam_Field,bgam_ctlUPP);

	ShowWindow(RGB_Tools_window);
	
	update_status();
	
	return 0;
}

/*
int printcmin_cmax_nib()
{
	extern DATAWORD	cmin,cmax;
	//CGrafPtr savePort;
	//GDHandle saveDevice;
	CFStringRef text;
	OSErr err;
	//GetGWorld(&savePort,&saveDevice);
	save_state();
	SetPortWindowPort(Status_window);

        
	if(cmin == cmax) 
		cmin -= 1;
	sprintf(txt, DATAFMT,cmin);
	SET_CONTROL_SVALUE( txt, status_cmin_ControlID, status_cmin_Field );	
        
	sprintf(txt, DATAFMT,cmax);
        
	SET_CONTROL_SVALUE( txt, status_cmax_ControlID, status_cmax_Field );
	SetControl32BitValue(status_autoscale_Field,autoscale);
	SetControl32BitValue(status_autoupdate_Field,autoupdate);

	//SetGWorld(savePort,saveDevice);
	restore_state();
	return 0;
}
*/


#ifdef FLOAT
#define DATAXYZ "x: %d  y: %d  z: %g"
#else
#define DATAXYZ "x: %d  y: %d  z: %d"
#endif

int printxyzstuff_nib(int nc,int nt,short line)
{
	
	static int nnt0 = 0;
	static int nnc0 = 0;
	static int nnt1 = 0;
	static int nnc1 = -1;
	extern int last_x_val,last_y_val;
	
	extern int report_spectral_info;
	extern float nitrogen_pixel,rayleigh_pixel,laser_wavelength;
	float lambda, shift,nm_per_pix;
#define N2_RAMAN  2331
	
	last_x_val = nc;
	last_y_val = nt;
	
	//CGrafPtr savePort;
	//GDHandle saveDevice;
	CFStringRef text;
	OSErr err;
	//GetGWorld(&savePort,&saveDevice);
	save_state();
	SetPortWindowPort(Status_window);
	
	if(line < 0) {		// this is the refresh  case 
		sprintf(txt,DATAXYZ,nnc0,nnt0,idat(nnt0,nnc0));
		SET_CONTROL_SVALUE( txt, status_t1_ControlID, status_t1_Field );
		sprintf(txt," ");
		SET_CONTROL_SVALUE( txt, status_t2_ControlID, status_t2_Field );    // erase second line
		
		if( nnc1 >=0 && report_spectral_info && toolselected == 0) {		// print wavelength data
			nm_per_pix = 1e7/(1/laser_wavelength*1e7-N2_RAMAN);  // raman wavelength
			nm_per_pix = (nm_per_pix - laser_wavelength)/(nitrogen_pixel - rayleigh_pixel);
			lambda = laser_wavelength + (nc - rayleigh_pixel)*nm_per_pix;
			shift = 1e7/laser_wavelength - 1e7/lambda;
			sprintf(txt,"Wavelength: %.2f    Shift: %.0f",lambda,shift);
			SET_CONTROL_SVALUE( txt, status_t2_ControlID, status_t2_Field );
		}
		
		if( nnc1 >=0 && toolselected != 0) {	// indicates there are two lines to be drawn (tool 0 is cross hair)
			sprintf(txt,DATAXYZ,nnc1,nnt1,idat(nnt1,nnc1)); 
			SET_CONTROL_SVALUE( txt, status_t2_ControlID, status_t2_Field );
		}	
		//SetGWorld(savePort,saveDevice);
		restore_state();
		return 0;
	}
	if(line != 0) {
		nnt1 = nt;
		nnc1 = nc;
		sprintf(txt,DATAXYZ,nc,nt,idat(nt,nc)); 
		SET_CONTROL_SVALUE( txt, status_t2_ControlID, status_t2_Field );
	} else {
		nnt0 = nt;
		nnc0 = nc;
		nnc1 = -1;
		sprintf(txt,DATAXYZ,nc,nt,idat(nt,nc));
		SET_CONTROL_SVALUE( txt, status_t1_ControlID, status_t1_Field );
		
		if( report_spectral_info && toolselected == 0) {		// print wavelength data
			nm_per_pix = 1e7/(1/laser_wavelength*1e7-N2_RAMAN);  // raman wavelength
			nm_per_pix = (nm_per_pix - laser_wavelength)/(nitrogen_pixel - rayleigh_pixel);
			lambda = laser_wavelength + (nc - rayleigh_pixel)*nm_per_pix;
			shift = 1e7/laser_wavelength - 1e7/lambda;
			sprintf(txt,"Wavelength: %.2f    Shift: %.0f",lambda,shift); 
			SET_CONTROL_SVALUE( txt, status_t2_ControlID, status_t2_Field );
		}
		
	}
	//SetGWorld(savePort,saveDevice);
	restore_state();
	return 0;
}

int pal_res_num[NUMPAL] = {134,129,130,131,132,128,133,135};		// these are the resource numbers for palettes 0 - 7

int setcolors_nib()
{
	OSErr err;
	extern int thepalette;
	ControlButtonContentInfo myInfo;
	Size actualSize;
	err = GetControlData(status_cbar_Field, kControlContentPictRes,kControlImageWellContentTag,sizeof(ControlButtonContentInfo), &myInfo , &actualSize );
	myInfo.u.resID = pal_res_num[thepalette];
	err = SetControlData(status_cbar_Field, kControlContentPictRes,kControlImageWellContentTag,actualSize, &myInfo );
	// need to mark this as needing an update needsupdate
	DrawControls (Status_window);
	return err;
}

void scroll_process_nib()
{
    CFStringRef text;
	OSErr err;
    cminmaxinc = GetControlValue(status_cmni_Field);
    sprintf(txt,"Min/Max Increment: %d %%",cminmaxinc);
    SET_CONTROL_SVALUE( txt, status_cmnmxinc_ControlID, status_cmnmxinc_Field );

}

void r_scroll_process_nib()
{
    CFStringRef text;
	OSErr err;
	int i;
	extern short newwindowflag;	
	short save_flag=newwindowflag;
	int drgbq();
	
	
    i = GetControlValue(status_rsca_Field);
	r_scale = pow(CSF,i/100.);
    sprintf(txt,"Red: %.2f \n",r_scale);
    SET_CONTROL_SVALUE( txt, status_rtxt_ControlID, status_rtxt_Field );
	
	newwindowflag=0;
	drgbq();
	newwindowflag = save_flag;
	
	
}

void g_scroll_process_nib()
{
    CFStringRef text;
	OSErr err;
    int i;
	extern short newwindowflag;	
	short save_flag=newwindowflag;
	int drgbq();
	
    i = GetControlValue(status_gsca_Field);
	g_scale = pow(CSF,i/100.);
    sprintf(txt,"Green: %.2f \n",g_scale);
    SET_CONTROL_SVALUE( txt, status_gtxt_ControlID, status_gtxt_Field );
	
	newwindowflag=0;
	drgbq();
	newwindowflag = save_flag;
	
}

void b_scroll_process_nib()
{
    CFStringRef text;
	OSErr err;
    int i;
	extern short newwindowflag;	
	short save_flag=newwindowflag;
	int drgbq();
	
    i = GetControlValue(status_bsca_Field);
	b_scale = pow(CSF,i/100.);
    sprintf(txt,"Blue: %.2f \n",b_scale);
    SET_CONTROL_SVALUE( txt, status_btxt_ControlID, status_btxt_Field );
	
	newwindowflag=0;
	drgbq();
	newwindowflag = save_flag;
	
}

void rgam_scroll_process_nib()
{
    CFStringRef text;
	OSErr err;
	int i;
	extern short newwindowflag;
	short save_flag=newwindowflag;
	int drgbq();
	
	
    i = GetControlValue(status_rgam_Field);
	r_gamma = pow(CSF,i/100.);
    sprintf(txt,"Red Gamma: %.2f \n",r_gamma);
    SET_CONTROL_SVALUE( txt, status_rtxt_ControlID, status_rgamtxt_Field );
	
	newwindowflag=0;
	drgbq();
	newwindowflag = save_flag;
	
	
}

void ggam_scroll_process_nib()
{
    CFStringRef text;
	OSErr err;
    int i;
	extern short newwindowflag;
	short save_flag=newwindowflag;
	int drgbq();
	
    i = GetControlValue(status_ggam_Field);
	g_gamma = pow(CSF,i/100.);
    sprintf(txt,"Green Gamma: %.2f \n",g_gamma);
    SET_CONTROL_SVALUE( txt, status_gtxt_ControlID, status_ggamtxt_Field );
	
	newwindowflag=0;
	drgbq();
	newwindowflag = save_flag;
	
}

void bgam_scroll_process_nib()
{
    CFStringRef text;
	OSErr err;
    int i;
	extern short newwindowflag;
	short save_flag=newwindowflag;
	int drgbq();
	
    i = GetControlValue(status_bgam_Field);
	b_gamma = pow(CSF,i/100.);
    sprintf(txt,"Blue Gamma: %.2f \n",b_gamma);
    SET_CONTROL_SVALUE( txt, status_btxt_ControlID, status_bgamtxt_Field );
	
	newwindowflag=0;
	drgbq();
	newwindowflag = save_flag;
	
}

int update_status()
{
    extern int pause_flag;
    extern char pause_string[];
	
	extern int macval;
	
	extern DATAWORD	cmin,cmax;

	
    OSErr err;
	CFStringRef text;
	int i;
	
    //CGrafPtr savePort;
    //GDHandle saveDevice;	
    //GetGWorld(&savePort,&saveDevice);
	save_state();
    SetPortWindowPort(Status_window);
	
// update_pause
    if(  pause_flag != 0) {
            SET_CONTROL_SVALUE(pause_string, status_t7_ControlID, status_t7_Field );
    } else {
            SET_CONTROL_SVALUE(" ", status_t7_ControlID, status_t7_Field );
    
    }

// printmacronum
	sprintf(txt,"Macro #:  %d",macval);
	SET_CONTROL_SVALUE( txt, status_t6_ControlID, status_t6_Field );

// drawtools
	for(i=0; i<NUMTOOLS; i++) {
		if( i == toolselected) {
			SetControlValue(status_tool_Field[i],1);	
		} else {
			SetControlValue(status_tool_Field[i],0);
		}
	}

// printparams
	sprintf(txt,"Cols: %4d Rows: %4d         ",header[NCHAN],header[NTRAK]);
	SET_CONTROL_SVALUE( txt, status_t3_ControlID, status_t3_Field );
	sprintf(txt,"X0:   %4d Y0:   %4d         ",header[NX0],header[NY0]);
	SET_CONTROL_SVALUE( txt, status_t4_ControlID, status_t4_Field );
	sprintf(txt,"DX:   %4d DY:   %4d         ",header[NDX],header[NDY]);
	SET_CONTROL_SVALUE( txt, status_t5_ControlID, status_t5_Field );


// printcmin_cmax
	if(cmin == cmax) 
		cmin -= 1;
	sprintf(txt, DATAFMT,cmin);
	SET_CONTROL_SVALUE( txt, status_cmin_ControlID, status_cmin_Field );	
        
	sprintf(txt, DATAFMT,cmax);
        
	SET_CONTROL_SVALUE( txt, status_cmax_ControlID, status_cmax_Field );
	SetControl32BitValue(status_autoscale_Field,autoscale);
	SetControl32BitValue(status_autoupdate_Field,autoupdate);
	
	
// RGB_Tools
	SetPortWindowPort(RGB_Tools_window);
	
	i = 100*log(r_scale)/log(CSF);
	SetControl32BitValue(status_rsca_Field,i);
	i = 100*log(g_scale)/log(CSF);
	SetControl32BitValue(status_gsca_Field,i);
	i = 100*log(b_scale)/log(CSF);
	SetControl32BitValue(status_bsca_Field,i);
    
    i = 100*log(r_gamma)/log(CSF);
	SetControl32BitValue(status_rgam_Field,i);
	i = 100*log(g_gamma)/log(CSF);
	SetControl32BitValue(status_ggam_Field,i);
	i = 100*log(b_gamma)/log(CSF);
	SetControl32BitValue(status_bgam_Field,i);


	sprintf(txt,"Red: %.2f \n",r_scale);
	SET_CONTROL_SVALUE( txt, status_rtxt_ControlID, status_rtxt_Field );
	sprintf(txt,"Green: %.2f \n",g_scale);
	SET_CONTROL_SVALUE( txt, status_gtxt_ControlID, status_gtxt_Field );
	sprintf(txt,"Blue: %.2f \n",b_scale);
	SET_CONTROL_SVALUE( txt, status_btxt_ControlID, status_btxt_Field );

    sprintf(txt,"Red Gamma: %.2f \n",r_gamma);
	SET_CONTROL_SVALUE( txt, status_rgamtxt_ControlID, status_rgamtxt_Field );
	sprintf(txt,"Green Gamma: %.2f \n",g_gamma);
	SET_CONTROL_SVALUE( txt, status_ggamtxt_ControlID, status_ggamtxt_Field );
	sprintf(txt,"Blue Gamma: %.2f \n",b_gamma);
	SET_CONTROL_SVALUE( txt, status_bgamtxt_ControlID, status_bgamtxt_Field );

	SetPortWindowPort(Status_window);

    //SetGWorld(savePort,saveDevice);
	restore_state();
    return 0;
}


