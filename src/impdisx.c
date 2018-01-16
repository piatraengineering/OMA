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
/***
Display for the IMP program
Information from the IMP program:
	Data Pointer
	Nchan
	Ntracks
	color min
	color max
	pixel size
	plot type
***/

#define ENTER	3
#define DEL 	8
#define RET	13
#define PGUP	11
#define PGDN	12
#define PAUS	0
#define RARROW	29
#define LARROW	28
#define UARROW	30
#define DARROW	31
#define PERIOD	46
#define FK15	0

#include	"impdefs.h"
#include "oma_quartz.h"


#define kAboutBox	200	/* Dialog resource for About box */

//CGrafPtr qdtheportptr;

/*******************************************************************************/



#define SETRECT(rectp, _left, _top, _right, _bottom)	\
	(rectp)->left = (_left), (rectp)->top = (_top), 	\
	(rectp)->right = (_right), (rectp)->bottom = (_bottom)

/*
 * HIWORD and LOWORD macros, for readability.
 */
#define HIWORD(aLong)		(((aLong) >> 16) & 0xFFFF)
#define LOWORD(aLong)		((aLong) & 0xFFFF)

#define	GWX0		20		/* the x screen location of the first graphics window */
#define GWY0		44		/* the y screen location of the first graphics window */
#define GWDX		20		/* the y offset of the origin of subsequent windows */
#define GWDY		20		/* the y offset of the origin of subsequent windows */
#define windowID	128

/*******************************************************************************/

//extern _DataInit();

//MenuHandle		MyMenus[maxmenuCount]; 	/* The menu handles */
MenuBarHandle		myMenuBar;
Boolean 		DoneFlag;				/* Becomes TRUE when File/Quit chosen */
Boolean			answerback = false;		/* tells if a menu item is sending text to main OMA */
Boolean			multilines = false;		/* tells if a multiple line paste command is being processed */
Boolean			printall = true;		/* the flag for "priority only" printing */
Boolean			from_noprint = false;	/* flag that says don't print from within macro */

/******************************************************/
extern		OMA_Window	oma_wind[NGWINDS+1];

Rect 		mrect;
float           opaque_value = 0.75;

char application_path[CHPERLN];		// this is the path to the directory that the program is running from
char contents_path[CHPERLN];		// this is the path to the Contents directory
//GDHandle 	themaxdevice;
GDHandle 	mainDevice;		// the main device
GDHandle 	orgdevice;
int			ncolor;					/* the number of colors on the screen */
short		thedepth = 8;			// from the days when there might be less than 8 pits per pixel
//short		themaxdepth;				/* the maximum number of bits per pixel */
DATAWORD	crange;					// the range of colors defined by the curret cmin and cmax


PatHandle mypalhand = 0;		/* stuff to be used by palette manager when there are more */
CTabPtr myctptr;					/* than 8 bits per pixel */
CTabHandle mycthand;

/*********************************************************/
 
Rect 	r = {440,400,480,550};	/* the rectangle for color graphics windows */

RGBColor thecolor[258],pixcolor;
RGBColor filecolor[258];        // Three files containing palette files
RGBColor filecolor2[258];
RGBColor filecolor3[258];
RGBColor RGBWhite = {0xFFFF,0xFFFF,0xFFFF};
RGBColor RGBBlack = {0,0,0};

char	colorbar_values[256];

ColorSpec csa[256]; /* 6.0 */			/* the color spec array -- for getting palettes right */

WindowPtr		Ctwind;					/* Carbon the text window */

TXNOffset tostart = 0, toend = 0;
Handle textHandle;

int             gwnum;					// the graphics window counter
short			orgx = GWX0;
short			orgy = GWY0;			// the starting locations for the first window
short			swidth,sheight;
int             thepalette = BGRBOW;	// specifies the color (or gray) palette to be used 

Rect                    cbrect;
Rect                    screenRect;
Rect                    txRect = {300,50,600,640};
Point                   mousePt;
EventRecord             myEvent;

WindowPtr	whichWindow;
int erase_from_command = 0;

// some globals for the selection of a rectangle in a window to be saved as a subarray 

WindowPtr	windowwithabox;		
extern Point substart,subend;	// these two points define a data subarray that can be saved 
int             thereisabox = 0;
Rect		selectrect;
Point 		lastpoint;
DATAWORD	idat(int,int);
int             showselection = false;
int             docalcs = false;

int command_is_ready = 0;
char* gets_string;
int typing_pointer=0;

/* globals used to determine the fonts used for the command and status windows */

int 		c_font,s_font;		

/* globals for the ruler and line plot*/

int             showruler = false;
int             plotline = false;
int             ruler_scale_defined = 0;
char		unit_text[UNIT_NAME_LENGTH];
float		ruler_scale = 1.0;



/* global for stopping routines with "cmd ." */

int stopstatus = 0;
	
//char		itable[256];				/* the table of pixmap values for each color index */

short		pastelines;		/*?*/
short		startupvolume;

extern TWOBYTE	header[];
extern int numboxes;
extern Boolean loshowbox;
extern Rect cbarRect;		// the rectangle on the colorbar status window (defined in omadialogsx

extern WindowRef Status_window, RGB_Tools_window;
extern int drawing_in_open_window;

//extern RCMarker row_marker[];
//extern RCMarker col_marker[];

/* globals associated with zoom window */

short zwidth,zheight;
int is_zoom_window = 0;
int is_zoom_rgb_window = 0;

extern int alter_vectors,is_fft_window,is_auto_window;

//___________________________________________________________


void Initialize();	// function prototypes 
void MakeWindow(void);
void DoAboutBox(void);
void DrawWindow(WindowPtr window);
static OSErr QuitAppleEventHandler(const AppleEvent *appleEvt, AppleEvent* reply, UInt32 refcon);

int printf(),gtiff();

Boolean		gQuitFlag;	/* global */


TXNObject		object = NULL;
extern Boolean is_special;

// Somehow 10.5 doesn't seem to define this
#ifndef kTXNDefaultFontName
#define kTXNDefaultFontName 0
#endif


void InitMLTE(void)
{
    return;
}


EventTypeSpec    oma_eventTypes[]= {
									{kEventClassMouse,kEventMouseDown},
									{kEventClassWindow,kEventWindowDrawPart},
									//{kEventClassMenu,kEventMenuBeginTracking},
									{ kEventClassCommand, kEventCommandProcess },
									{kEventClassKeyboard,kEventRawKeyDown}};

int dinit()
{
	
	int i;

	short err;
 	char oldname[CHPERLN]; 
		
	//SInt16 v_ref_num; 
	//SInt32 dir_ID;
	//FSRef newfsref;
		
	FSSpec MyFSSpec;
	FSRef MyFSRef, parent;
	
	IBNibRef 		nibRef;
	pascal OSStatus oma_EventHandler (EventHandlerCallRef myHandler, EventRef event, void *userData);
	int nib_status_window();
	int nib_RGB_Tools_window();
	
	extern char  application_path[];
	extern int swap_bytes;
	extern int is_big_endian;	// this tells the byte ordering of this machine
								// big endian is PowerPC et al
								// little endian is intel et al
	void set_byte_ordering_value();
	
	CFByteOrder the_order = CFByteOrderGetCurrent();
	if(the_order == CFByteOrderBigEndian){
				// powerPC ordering
		swap_bytes = 0;
		is_big_endian = 1;
	} else {
				// intel ordering
		swap_bytes = 1;
		is_big_endian = 0;
	}
	set_byte_ordering_value();			// set the appropriate value in the trailer

#ifdef MacOSXBundle

	// This sets the deault volume to the folder containing the application
	// if it is in a Bundle (means 4 levels up from executable file)

	CFBundleRef main_bundle = CFBundleGetMainBundle();
	CFURLRef the_executable = CFBundleCopyExecutableURL(main_bundle);
	CFStringGetCString(CFURLCopyFileSystemPath(the_executable,kCFURLPOSIXPathStyle),oldname,256,kCFStringEncodingMacRoman);
	FSPathMakeRef((unsigned char*)oldname,&MyFSRef,NULL);		// now have FSRefto the executable
	
	//err = HGetVol((unsigned char*)oldname,&v_ref_num,&dir_ID);	
	//err = FSMakeFSSpec(v_ref_num,dir_ID,(unsigned char*)"",&MyFSSpec);
	//err = FSpMakeFSRef(&MyFSSpec,&MyFSRef);		// now have FSRef
	err = FSGetCatalogInfo(&MyFSRef,0,NULL,NULL,NULL,&parent);		// parent is MacOS folder
	err = FSGetCatalogInfo(&parent,0,NULL,NULL,NULL,&MyFSRef);		// Mysref is Contents folder
	FSRefMakePath(&MyFSRef,(unsigned char*)contents_path,255);
	err = FSGetCatalogInfo(&MyFSRef,0,NULL,NULL,&MyFSSpec,&parent);	// 
	err = FSGetCatalogInfo(&parent,0,NULL,NULL,NULL,&MyFSRef);
	parent = MyFSRef;
	//err = HSetVol(NULL,MyFSSpec.vRefNum,MyFSSpec.parID);	
#endif

#ifdef DO_MACH_O
        FSRefMakePath(&parent,(unsigned char*)application_path,255);
        //chdir(oldname); 
		chdir(application_path);
#endif                
	//CFBundleRef main_bundle = CFBundleGetMainBundle();
	//CFURLRef the_executable = CFBundleCopyExecutableURL(main_bundle);
	//CFIndex thenum;
	//CFURLGetByteRangeForComponent(the_executable,kCFURLComponentPath,&thenum);
	//CFURLGetBytes(the_executable,oldname,thenum);
	//CFStringRef ex_path = CFURLCopyFileSystemPath(the_executable,kCFURLPOSIXPathStyle);
	//CFStringGetCString(CFURLCopyFileSystemPath(the_executable,kCFURLPOSIXPathStyle),oldname,256,kCFStringEncodingMacRoman);
	//kCFURLComponentPath

	Initialize();

	//MakeMenu();
	err = CreateNibReference(CFSTR("oma"), &nibRef);
	err = CreateMenuBarFromNib(nibRef, CFSTR("MenuBar"),&myMenuBar);
	//err = SetMenuBarFromNib(nibRef, CFSTR("MenuBar"));
	DisposeNibReference(nibRef);
	//EnableAllMenuItems (myMenuBar);
	SetMenuBar (myMenuBar);
	DrawMenuBar();

	//qdtheportptr = GetQDGlobalsThePort ();
	//screenRect = (**mainDevice).gdRect;
	GetAvailableWindowPositioningBounds (mainDevice,&screenRect);
	
	//SetRect(&dragRect, 4, 20 + 4, screenRect.right-4, screenRect.bottom-4);


	//---------------------------------------------------------------
	//themaxdevice = GetMaxDevice(&screenRect);		// get the device with the deepest screen 
        //themaxdepth = (*((*themaxdevice)->gdPMap))->pixelSize;
	//if( themaxdepth > 8) {
		//thedepth = 8;
		//mycthand = (CTabHandle) NewHandle(sizeof(ColorTable)+(256*sizeof(ColorSpec)));
		//myctptr = *mycthand;
	//} else {
		//thedepth = themaxdepth;
	//}		
	ncolor = (1 << thedepth) - 2;						// the number of colors 
        if(ncolor == 0) {ncolor = 254; }					// one-bit per pixel case 
	//---------------------------------------------------------------

	chdir(contents_path);
	loadprefs(SETTINGSFILE);	// load the default values
	
		 
	nib_status_window();         // the coordinate and color mapping window
	nib_RGB_Tools_window();		// RGB_Tools window
	MakeWindow();
	getpalettefile(PALETTEFILE2); // the default palette in a file
	for(i=0; i<=(ncolor+1); i++) 
		filecolor2[i] = thecolor[i] ;
	getpalettefile(PALETTEFILE3); // the default palette in a file
	for(i=0; i<=(ncolor+1); i++) 
		filecolor3[i] = thecolor[i] ;
	getpalettefile(PALETTEFILE); // the default palette in a file
	
	chdir(application_path);
       
	// reposition windows to the lower left of the screen
	GetWindowPortBounds(Ctwind,&r);
	MoveWindow(Ctwind,-r.left,screenRect.bottom-txRect.bottom-4,false);
	
	GetWindowPortBounds(Status_window,&r);
	MoveWindow(Status_window,-r.left+txRect.right+10,screenRect.bottom-txRect.bottom-4,false);
	
	GetWindowPortBounds(RGB_Tools_window,&r);
	MoveWindow(RGB_Tools_window,-r.left+txRect.right+240+2*10,screenRect.bottom-r.bottom-4,false);

	TXNUpdate(object);
	//printf("%d Bits per Pixel.\n",themaxdepth);
	
	for(i=0; i<NGWINDS+1; i++){
		 oma_wind[i].row_marker.window = -1;
		 oma_wind[i].col_marker.window = -1;
	}

	assigncolor1();		// set up the default palette 
	setup_status();		
	//printf("%s\n",oldname);
	//printf("%s\n",contents_path);
		
	return 0;
}


// oma event handler
pascal OSStatus oma_EventHandler (EventHandlerCallRef myHandler, EventRef event, void *userData)
{
    OSStatus 		result = eventNotHandledErr,err = 0;
	short	part;
    //Boolean	hit;
    
	
    int	gw,tmp;
	UInt32 event_kind,event_class,modifiers;
	EventMouseButton button;
	EventRecord omaEvent;
	HICommand		command;
	
    Point showcoords(short horiz,short vert,short line,int window_number);
	Point orig_mousePt;
    WindowPtr	myWindow;
	WindowPtr	theWindow;
    
	char ch;

    extern int pause_flag;
    extern int macflag,exflag;
    
	
	extern int selected_hist_index,hist_index;
    extern char cmnd_history[];
	
	OSStatus DoHIMenuCommand(HICommand* thecommand);
	
	int macro(),update_pause(),pprintf();

	
#ifdef SensiCam
    extern int sensi_focus;													
#endif 

	err = ConvertEventRefToEventRecord(event,&omaEvent);
	//if(err) SysBeep(1);
	
	event_class = GetEventClass(event);
	event_kind = GetEventKind(event);
	
	theWindow = FrontWindow();
	// don't handle events from dialogs -- only the main windows
	if(theWindow != Status_window && theWindow != Ctwind && theWindow != RGB_Tools_window){
		for (gw = 0; gw<gwnum; gw++){
			if(theWindow == oma_wind[gw].gwind) break;
		}
		if(gw == gwnum)
			return result;
	}

	switch(event_class) {
		case kEventClassAppleEvent:
			//if( event_kind == kAEOpenDocuments || 1){
				result = AEProcessAppleEvent( &omaEvent );
				printf("apple event kind %d %d result %d\n",event_kind,omaEvent.what,result);
			//}
			return result;
		case kEventClassCommand:
			GetEventParameter( event, kEventParamDirectObject, typeHICommand, NULL,
				sizeof( HICommand ), NULL, &command );
			err = DoHIMenuCommand(&command);
			return err;
		case kEventClassWindow:
			// could do another switch on the kind, but have only one for now
            if( event_kind == kEventWindowFocusContent) DrawWindow((WindowPtr) theWindow);
            return noErr;
		case kEventClassMouse:
			err = GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(typeQDPoint),NULL,&orig_mousePt);
			err = GetEventParameter(event,kEventParamMouseButton,typeMouseButton,NULL,sizeof(typeMouseButton),NULL,&button);
			err = GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(typeUInt32),NULL,&modifiers);
			if(theWindow == Ctwind){
                TXNFocus(object, true);
			}
            part = FindWindow(orig_mousePt, &myWindow);
            whichWindow = myWindow;
            tmp = activegwnum(whichWindow);
            switch (part)
            {
                case inMenuBar:  // process a moused menu command 
                    //DoMenuCommand(MenuSelect(orig_mousePt),gets_string,&typing_pointer);
                    MenuSelect(orig_mousePt);
					return noErr;
                    break;

                case inSysWindow:
                    break;

                case inContent:
                    if (myWindow != FrontWindow()) {
                        SelectWindow(myWindow);
                        //break;
                    } else {
                        if (whichWindow == Ctwind) {
                            TXNClick(object, &omaEvent);
                        }
                        //SetCursor(&arrow_cursor);
						SetThemeCursor( kThemeArrowCursor );
                        //checkscroll();
                    }

                    if(whichWindow == Ctwind || whichWindow == Status_window || whichWindow == RGB_Tools_window)
                        break;	// don't track coordinates outside graphics windows

                    break;
            case inDrag:	// pass screenBits.bounds 
				// the command window still needs this -- probably nee to re-do that then
                //GetRegionBounds(GetGrayRgn(), &tempRect);
                DragWindow(myWindow, orig_mousePt, &screenRect);
                break;
                
            case inGrow:
                SetPortWindowPort(myWindow);
                if( myWindow == Ctwind) {
                        TXNGrowWindow(object, &omaEvent);	
                        TXNUpdate(object);
                        break;
                }					
                break;
                
            case inGoAway:
            
                for(tmp = 0;tmp < gwnum;tmp++) {
                    if(oma_wind[tmp].gwind == myWindow) {
                            closewindow(tmp,0);
                            break;
                    }
                }
                
                if(Ctwind == myWindow) {
                    DisposeWindow(myWindow);
                    ExitToShell();
                }
                
                break;
                
            case inZoomIn:
            case inZoomOut:
                if( myWindow == Ctwind) {
                        TXNZoomWindow(object, part);
                        TXNUpdate(object);	
                        break;
                }
				/*					
                hit = TrackBox(myWindow, orig_mousePt, part);
                if (hit) {
                    SetPort(GetWindowPort(myWindow));   // window must be current port
                    EraseRect(GetWindowPortBounds(myWindow, &tempRect));   // inval/erase because of ZoomWindow bug
                    ZoomWindow(myWindow, part, true);
                    InvalWindowRect(myWindow, GetWindowPortBounds(myWindow, &tempRect));	
                }
				*/
                break;
            }
            break;
			return noErr;
			//SysBeep(1);
			break;
		case kEventClassKeyboard:
			err = GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,NULL,1,NULL,&ch);
			err = GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,4,NULL,&modifiers);

			if(pause_flag != 0) {
				pause_flag = 0;
				update_status();
				if ((modifiers & cmdKey) && ch == '.' ){ 
					if (macflag !=0 || exflag !=0){
						stopmacro();
					}
				}
				if (modifiers & cmdKey) 
					pause_flag = -1;
						
				return noErr;
			}
			
			if (modifiers & cmdKey){
				if(ch == '.') {
					if (macflag !=0 ) {
						stopmacro();
					} else {
						stopstatus++;		// count up cmd periods 
					}
					return noErr;
				}
				//printf("%c %d %d\n",ch, HiWord(MenuKey(ch)), LoWord(MenuKey(ch)));
				//DoMenuCommand(MenuKey(ch),gets_string,&typing_pointer);
				return noErr;
			}

			switch(ch) {
				case FK15:
					ch = (char) 0x0D;
					
					TXNSetData(object,kTXNTextData,(void *)&ch, 1, kTXNEndOffset, kTXNEndOffset);
					macro(0,0);
					//ShowCursor();   /* force the cursor to be shown */
					typing_pointer=0;
					return(0);
				case ENTER:
					ch = (char) 0x0D;
				case RET:
					
					TXNSetData(object,kTXNTextData,(void *)&ch, 1, kTXNEndOffset, kTXNEndOffset);
					gets_string[typing_pointer] = 0;
					//ShowCursor();	/* force the cursor to be shown */
					typing_pointer=0;
					command_is_ready = 1;
					return noErr;
					
				case DEL:
																					
					TXNGetSelection(object,&tostart,&toend);
					TXNSetSelection (object,tostart-1,toend);
					TXNClear(object);
					
					typing_pointer--;
					if(typing_pointer < 0) typing_pointer = 0;
					break;
				case PGDN:
					
					break;
				case PGUP:
					
					break;
				case UARROW:
					if(selected_hist_index > 0) {
						selected_hist_index-=2;
						while(selected_hist_index >= 0 && cmnd_history[selected_hist_index] !=0) {
								selected_hist_index--;
						}
						selected_hist_index++;
						strcpy(gets_string,&cmnd_history[selected_hist_index]);
						
						TXNGetSelection(object,&tostart,&toend);
						TXNSetSelection (object,tostart-typing_pointer,toend);
						TXNClear(object);

						typing_pointer = strlen(gets_string);
						printf("%s",gets_string);
					}
					break;
				case DARROW:
					if(selected_hist_index < hist_index) {
							
						while(cmnd_history[selected_hist_index] !=0) {
							selected_hist_index++;
						}
						selected_hist_index++;
						if(selected_hist_index < hist_index){
							strcpy(gets_string,&cmnd_history[selected_hist_index]);
					
							TXNGetSelection(object,&tostart,&toend);
							TXNSetSelection (object,tostart-typing_pointer,toend);
							TXNClear(object);

							typing_pointer = strlen(gets_string);
							printf("%s",gets_string);
						}
					}
					break;
				case RARROW:
				case LARROW:
					
					TXNSetData(object,kTXNTextData,(void *)&ch, 1, kTXNEndOffset, kTXNEndOffset);
					break;
				default:
					gets_string[typing_pointer++] = ch;
							
					TXNSetData(object,kTXNTextData,(void *)&ch, 1, kTXNEndOffset, kTXNEndOffset);		
			}
			return noErr;
	}
	//SysBeep(1);
	return result;
}

static pascal OSErr
oma_receiveDropHandler(WindowRef inWindow, void* unused, DragRef inDragRef)
{	
	
	OSErr		result = noErr,err;
	OSErr open_oma_given_fsspec( FSSpec event_open_FSSpec);
	
	FlavorType	flavor;
	DragItemRef	itemRef;
	UInt16		flavorCount;
	HFSFlavor	drag_file;
	Size		mysize;
	int i;

 	char oldname[CHPERLN]; 	
	getcwd(oldname,CHPERLN);
	
	err = GetDragItemReferenceNumber( inDragRef, 1, &itemRef );
	err = CountDragItemFlavors( inDragRef, itemRef, &flavorCount );
	for(i = 1; i<= flavorCount; i++) {
		err = GetFlavorType( inDragRef, itemRef, i, &flavor );
		if( flavor == kDragFlavorTypeHFS ){
			err = GetFlavorData (inDragRef ,itemRef, kDragFlavorTypeHFS, &drag_file, &mysize, 0);
			err = open_oma_given_fsspec(drag_file.fileSpec);
			if(err != noErr)
				result = eventNotHandledErr;
			
		 } 
	}
	
	chdir(oldname);
	return result;
}

void Initialize()	/* Initialize some managers */
{
    OSErr	err;
	//char *pData;
	
	OSErr oma_prefs_CommandHandler(AppleEvent * theAppleEvent,AppleEvent * reply,SInt32 handlerRefcon);
	//static OSErr oma_open_CommandHandler(AppleEvent * theAppleEvent,AppleEvent * reply,SInt32 handlerRefcon);
    OSErr oma_open_CommandHandler(AppleEvent * theAppleEvent,AppleEvent * reply,SInt32 handlerRefcon);
	   
    //InitCursor();
    //ibeamHdl = GetCursor(iBeamCursor);		/* Grab this for use later */
    //plusHdl = GetCursor(crossCursor);
    //GetQDGlobalsArrow(&arrow_cursor);
    
    mainDevice = GetMainDevice();
	//mainDevice = DMGetFirstScreenDevice (true);	both of these are deprecated
    
    InitMLTE();

    err = AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, 
			NewAEEventHandlerUPP((AEEventHandlerProcPtr)QuitAppleEventHandler), 0, false );
    if (err != noErr)
        ExitToShell();
	
	// enable open of file by double click
	EnableMenuCommand(NULL, kHICommandOpen);
	AEInstallEventHandler((AEEventClass)kCoreEventClass, kAEOpenDocuments, 
		NewAEEventHandlerUPP((AEEventHandlerProcPtr)oma_open_CommandHandler), 0, false );
	
	// enable the Preferences menu and put in a handler
	EnableMenuCommand(NULL, kHICommandPreferences);
	AEInstallEventHandler(kCoreEventClass, kAEShowPreferences, 
		NewAEEventHandlerUPP((AEEventHandlerProcPtr)oma_prefs_CommandHandler), 0, false );			

	InstallApplicationEventHandler( NewEventHandlerUPP(oma_EventHandler), GetEventTypeCount(oma_eventTypes), oma_eventTypes, 0, NULL );
	
	err = InstallReceiveHandler(NewDragReceiveHandlerUPP(oma_receiveDropHandler), NULL, NULL);

}

OSErr QuitAppleEventHandler( const AppleEvent *appleEvt, AppleEvent* reply, UInt32 refcon )
{
    gQuitFlag =  true;
    return noErr;
}



OSErr open_oma_given_fsspec( FSSpec event_open_FSSpec){
	
	
	OSErr  theErr;
 	char fullpath[256]; 	
	FInfo finfo,*finfo_ptr;
	int i,num_col=1;
	char *getcwd(); 
	int read_jpeg(),gmacro(),dcraw_glue(),read_fit(),getpivfromfinder();
	
	//FSRef parent;
	FSRef MyFSRef;
	extern char   txt[];
	//extern char curname[256];
	extern int have_max;
	extern char    cmnd[];  	/* the command buffer */
	extern char	   lastname[];
	extern Variable user_variables[];
	extern float r_scale,g_scale,b_scale;
	
	int dquartz(int n,int index),pivplt(int,int), drgbq(int,int),dmnmx_q();

	FSpMakeFSRef(&event_open_FSSpec,&MyFSRef);		// now have FSRef for the selected file
	FSRefMakePath(&MyFSRef,(unsigned char*)fullpath,255); // this is the path
	printf("%s\nOMA>",fullpath); // this is the file name
	
	// given an FSRef, this stuff gets the finder info and the file name
	FSCatalogInfo myInfo;
	HFSUniStr255 myName;
	theErr = FSGetCatalogInfo(&MyFSRef,kFSCatInfoFinderInfo,&myInfo,&myName,NULL,NULL);
	finfo_ptr = (FInfo*) &myInfo.finderInfo;
	finfo = *finfo_ptr;
	//CFStringRef	myCFnameRef = CFStringCreateWithCharacters(NULL,myName.unicode,myName.length);
	
	// use the full path name for everything
	strcpy(txt,fullpath);	//
	/*
	CFStringGetCString(myCFnameRef,txt,256,kCFStringEncodingMacRoman);
	printf("%s\n",txt);
	*/
	// get the volume name without the file name
	/*
	FSGetCatalogInfo(&MyFSRef,0,NULL,NULL,NULL,&parent);
	FSRefMakePath(&parent,(unsigned char*)curname,255);
	chdir(curname); 
	*/

	// see what kind of file this is and jump to appropriate routine 
	i = strlen(txt)-4;
	if( finfo.fdType == 'OPRF' ){
		printf("Prefs loaded from %s\n",fullpath);
		loadprefs(fullpath);
		printf("OMA>");
	} else if( finfo.fdType == 'ODTA' || memcmp(&txt[i],".dat",4) == 0 ){   // open ODTA or .dat files
		if( finfo.fdType == 'TEXT'){
			//beep();
			//printf("Finder-based open of text files not currently supported.\n");
			getpivfromfinder(fullpath);
			printf("OMA>");
			pivplt(0,0);
		} else {
			getfile(0,-1);
			printf("OMA>");
			dquartz(0,0);
            dmnmx_q();
		}
	} else if( memcmp(&txt[i],".jpg",4) == 0 || memcmp(&txt[i],".JPG",4) == 0 ){   // open jpeg files
		strcpy(lastname,txt);
		read_jpeg(txt,-1);
		printf("OMA>");
		r_scale=g_scale=b_scale= 1.0;
		update_status();
		drgbq(0,0);
        dmnmx_q();
	} else if( memcmp(&txt[i],".mac",4) == 0 || memcmp(&txt[i],".MAC",4) == 0 ){   // open macro files
		// txt[i] = 0;		// get rid of the extension -- the gmacro routine will add it on
		strcpy(cmnd,txt);
		strcpy(lastname,txt);	// remember this name too

		gmacro(0,-1);	// this is the full name of the macro including its path
		printf("OMA>");
	} else if( memcmp(&txt[i],".nef",4) == 0 || memcmp(&txt[i],".NEF",4) == 0 ||
			   memcmp(&txt[i],".raw",4) == 0 || memcmp(&txt[i],".RAW",4) == 0 ||
			   memcmp(&txt[i],".rw2",4) == 0 || memcmp(&txt[i],".RW2",4) == 0 ||
			   memcmp(&txt[i],".dng",4) == 0 || memcmp(&txt[i],".DNG",4) == 0){   // open nikon raw files and digital negative files
		strcpy(lastname,txt);
		
		if( (num_col = dcraw_glue(txt,-1)) < OMA_OK){
			printf("OMA>");
			return -1;
		}
		have_max = 0;
		maxx();
		update_status();
		printf("OMA>");
		if(num_col == 3){
			r_scale=g_scale=b_scale= 1.0;
			update_status();
			drgbq(0,0);
            dmnmx_q();
		}else{
			dquartz(0,0);
            dmnmx_q();
		}
		
	} else if( memcmp(&txt[i],".fts",4) == 0 || memcmp(&txt[i],".FTS",4) == 0 ||
			   memcmp(&txt[i],".fit",4) == 0 || memcmp(&txt[i],".FIT",4) == 0){   // open fits files
		strcpy(lastname,txt);
		read_fit(txt);
		printf("OMA>");
		dquartz(0,0);
        dmnmx_q();
	} else if( memcmp(&txt[i],".tif",4) == 0 || memcmp(&txt[i],".TIF",4) == 0 ||
			   memcmp(&txt[i-1],".tiff",5) == 0 || memcmp(&txt[i-1],".TIFF",5) == 0){   // open fits files
		strcpy(lastname,txt);
		strcpy(cmnd,txt);
		gtiff(0,0);
		printf("OMA>");
		if (user_variables[0].ivalue == 3){
			r_scale=g_scale=b_scale= 1.0;
			update_status();
			drgbq(0,0);
            dmnmx_q();
		} else {
			dquartz(0,0);
            dmnmx_q();
		}
	}else {
		beep();
		printf("File Type Not Supported.\n");
		printf("OMA>");
		return -1;
	}
	return noErr;

}



// "open" command handler
// get here from finder when files are dropped onto icon

//static OSErr oma_open_CommandHandler(AppleEvent * theAppleEvent,AppleEvent * reply,SInt32 handlerRefcon)
OSErr oma_open_CommandHandler(AppleEvent * theAppleEvent,AppleEvent * reply,SInt32 handlerRefcon)
{
	FSSpec event_open_FSSpec;
	Size actualSize;
	OSErr  theErr;
 	char oldname[CHPERLN]; 	
	char *getcwd(); 
	
	extern char   txt[];
	//extern char curname[256];
	extern int have_max;
	extern char    cmnd[];  	/* the command buffer */
	extern char	   lastname[];

	getcwd(oldname,256);
	if ( (theErr = AEGetParamPtr(theAppleEvent, keyDirectObject, typeFSRef, NULL,
								  &event_open_FSSpec, sizeof( FSSpec ), &actualSize ))  != noErr ) {
		beep();
		printf("Error getting a data or settings file from the Finder.\n");
		return(theErr);
	} else {
	
		theErr = open_oma_given_fsspec(event_open_FSSpec);
	
	}
	
	chdir(oldname);

	return(theErr);
}


void MakeWindow()	// Put up a window
{
    
    //SetRect(&txRect,50,300,640,600);

    Ctwind = NewCWindow(nil, &txRect, "\pOMA Commands", true, zoomDocProc, (WindowPtr) -1, true, 0);

    if (Ctwind != NULL) 
    {
        OSStatus		status = noErr;
        TXNFrameID		frameID	= 0;
        WindowPtr		paramWindow = NULL;
        TXNFrameOptions	frameOptions;
        
        frameOptions = kTXNShowWindowMask; // ShowWindow not needed with kTXNShowWindowMask
        frameOptions |= kTXNWantHScrollBarMask | kTXNWantVScrollBarMask | kTXNDrawGrowIconMask;
        
        paramWindow = Ctwind;

		/*
        status = TXNNewObject(	NULL, 
                                paramWindow, 
                                nil, // iFrame will be set to the window bounds rect
                                frameOptions,
                                kTXNTextEditStyleFrameType,
                                kTXNTextensionFile,
                                kTXNSystemDefaultEncoding,
                                &object,
                                &frameID, 
                                0);
		*/
		status = TXNCreateObject(NULL,frameOptions,&object);
		status = TXNAttachObjectToWindowRef(object,paramWindow);
                                                        
        if (status == noErr) {		
            if (object != NULL) {
                // sets the state of the scrollbars so they are drwan correctly
                //status = TXNActivate(object, frameID, kScrollBarsSyncWithFocus);
				status = TXNSetScrollbarState(object,kScrollBarsSyncWithFocus);
                if (status != noErr)
                    beep();
                status = SetWindowProperty(Ctwind,'GRIT','tFrm',sizeof(TXNFrameID),&frameID);
                status = SetWindowProperty(Ctwind,'GRIT','tObj',sizeof(TXNObject),&object);
            }
        }	
    } else {
        DisposeWindow(Ctwind);
        Ctwind = NULL;
        //AlertUser(eNoWindow);		// and tell user
        beep();
    }      
    SetPortWindowPort(Ctwind);  // set port to new window 
    GetWindowPortBounds(Ctwind,&txRect);
    InsetRect(&txRect, 4, 0);
}

//___________________
char* omagets(char* string)
{
    Boolean	gotEvent;
    EventRecord	event;
	//EventRef eventRef;
    char tmptxt[CHPERLN];
    int frommain = true;
	uint32 sleeptime;
	extern int macflag;

        
    gQuitFlag = false;
    
	if(string == nil) {
		gets_string = tmptxt;	// a junk place for typing events during macro execution
		frommain = false;
	} else {
		gets_string = string;
	}

    do
    {
		if(command_is_ready) {
			command_is_ready = 0;
			return(0);
		}
		
		if(macflag)
			sleeptime = 0;
		else 
			sleeptime = 1;
		gotEvent = WaitNextEvent(everyEvent,&event,sleeptime,nil);	// the above should optimize this without making oma a pig
		
		if (gotEvent) {
			if(event.what == kHighLevelEvent){
				AEProcessAppleEvent(&event);
			}
		 } else {
			if( !frommain) return(0);
		 }
		 
    } while (!gQuitFlag);
    
    ExitToShell();	
    return 0;				
}

void DrawWindow(WindowPtr window)
{
    
    BeginUpdate(window);

    if (window == Ctwind) {
        TXNUpdate(object);
    }

    DrawControls(window);
    DrawGrowIcon(window);
    EndUpdate(window);
    
}

Point showcoords(short horiz,short vert,short line,int window_number)
{
    int nc,nt;
    Point answer;
    float x,y;
	int printxyzstuff();
    
	CGRect dstRect;
	HIViewRef   contentView;
	HIViewFindByID(HIViewGetRoot(FrontWindow()), kHIViewWindowContentID, &contentView);
	HIViewGetBounds(contentView, &dstRect);
    x =  horiz * (header[NCHAN]-1);
    x /= dstRect.size.width;
	if(oma_wind[window_number].windowtype == QRGBMAP)
		y =  vert * (header[NTRAK]/3-1);
	else
		y =  vert * (header[NTRAK]-1);
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

int erasebox()
{
/*
    extern Rect selectrect;
    extern int gwnum,activegwnum();

    
    if( activegwnum(windowwithabox) < gwnum ) {
            SetPortWindowPort(windowwithabox);
            PenMode(srcXor);
            selectrect.bottom = lastpoint.v;
            selectrect.right = lastpoint.h;
            if( thereisabox == -1) {
                    MoveTo(selectrect.right,selectrect.bottom);
                    LineTo(selectrect.left,selectrect.top);
            } else
                    FrameRect(&selectrect);
            PenMode(srcCopy);
    }
*/
    thereisabox = 0;
    return 0;
}

int activegwnum(WindowPtr wind)		// return the index of the window passed as an argument
{
	int	tmp;
	
	for(tmp=0;tmp<gwnum;tmp++) {
		if(oma_wind[tmp].gwind == wind) {
//			printf("Window %d -- %d\n",tmp,wind); 
			return(tmp);
		}
		//printf("Windowx %d -- %d  %d\n",tmp,wind,gwind[tmp]); 
  	}
  	return 0;
}

void DoAboutBox(void)
{
        (void) NoteAlert(kAboutBox, nil);  // simple alert dialog box

}

/* ************** */

int setfinfo()
{
	return 0;
}
int getfinfo()
{
	return 0;
}
int checkevents()
{
	gets(nil);
	return 0;
}
int beep()
{
    extern int stop_on_error,macflag,exflag;
	SysBeep(1);
	is_special = true;
    if(stop_on_error && (macflag || exflag))
        stopmacro();
	return 0;
}
int nomemory()
{
	return 0;
}
int setarrow()
{
	return 0;
}

/*
*/
/*
*/
Boolean center_position = false;

int chooseposn(short nchan, short ntrack, short pixsiz)
{

	Rect portRect;
	
	portRect = screenRect;
	float nsx=1,nsy=1,im_scale=1;
	
	swidth = nchan*pixsiz;
	if(pixsiz < 0 )
		swidth = nchan / abs(pixsiz);
	sheight = ntrack*pixsiz;
	if(pixsiz < 0 )
		sheight = ntrack / abs(pixsiz);
	
	// if picture is too big for screen, shrink the rectangle
	if(swidth+GWX0 > portRect.right-portRect.left || sheight+GWY0+GWDY > portRect.bottom-portRect.top){
		nsx = (swidth);
		nsx /= (portRect.right-portRect.left-GWX0);
		nsy = (sheight);
		nsy /= (portRect.bottom-portRect.top-GWY0-GWDY);
		if(nsx > nsy){
			swidth = swidth/nsx;
			sheight = sheight/nsx;
			im_scale = nsx;
		} else {
			swidth = swidth/nsy;
			sheight = sheight/nsy;
			im_scale = nsy;
		}
	}		
	if(im_scale > 1.0) printf("Image size reduced by a factor of %f.\n",im_scale);
	if(gwnum == 0) {
		orgx = GWX0;
		orgy = GWY0; }
	else {
		
		if( swidth+orgx > portRect.right){
			orgx = GWX0;
			orgy += sheight+TBTHICK;
		}
	}
	
	
	if( sheight+orgy > portRect.bottom) {
		orgy = GWY0 + GWDY;
	}
	if(gwnum >= NGWINDS){
		SetPortWindowPort( oma_wind[gwnum].gwind);
		closewindow(0,0);
	}
	if( center_position ) {
		orgx = (portRect.right - swidth)/2;
		orgy = (portRect.bottom - sheight)/2;
	}
	
	//printf("%d %d %d %d \n",orgx, orgy, swidth+orgx, sheight+orgy);
	//SetRect(&r, orgx, orgy, swidth+orgx, sheight+orgy);
	
	r.left = orgx;
	r.top = orgy;
	r.right = swidth+orgx;
	r.bottom = sheight+orgy;

	
	
	return 0;
}

int openwindow(short swidth,short sheight,char* name,short windowtype)
{
  return(false);
}


int closewindow(int n, int from_event)					// close the nth graphics window 
{
//	extern int alter_vectors,is_fft_window,is_auto_window;
//	extern int lineongraph;
	
	int i;
	
	erase_from_command = !from_event;
	
	if(abs(oma_wind[n].windowtype) == BITMAP) {
		
		if(oma_wind[n].row_marker.window != -1) {		// this window has an open row plot
			//closewindow(oma_wind[n].row_marker.window);	// get rid of it
			oma_wind[oma_wind[n].row_marker.window].row_marker.window = -1;	// get rid of coupling
		}
		
		if(oma_wind[n].col_marker.window != -1) {		// this window has an open column plot
			//closewindow(oma_wind[n].col_marker.window);	// get rid of it
			oma_wind[oma_wind[n].col_marker.window].col_marker.window = -1;	// get rid of coupling
		}
	}
	if(abs(oma_wind[n].windowtype) == LINEGRAPH) {
  		
  		if(oma_wind[n].row_marker.window != -1) {		// this is a row plot -- modify the data window marker accordingly
  			//if(oma_wind[oma_wind[n].row_marker.window].row_marker.lineongraph && oma_wind[n].windowtype == LINEGRAPH) {
			//	beep();
			//	printf("obsolete linegraphics window\n");
			//}
  			oma_wind[oma_wind[n].row_marker.window].row_marker.window = -1;
  			oma_wind[n].row_marker.window = -1;			//
  		}
  		if(oma_wind[n].col_marker.window != -1) {		// this is a row plot -- modify the data window marker accordingly
  			//if(oma_wind[oma_wind[n].col_marker.window].col_marker.lineongraph && oma_wind[n].windowtype == LINEGRAPH) {
			//	beep();
			//	printf("obsolete linegraphics window\n");
			//}
  			oma_wind[oma_wind[n].col_marker.window].col_marker.window = -1;
  			oma_wind[n].col_marker.window = -1;			//

  		}
	}
	
	if(!from_event) {
		DisposeWindow(oma_wind[n].gwind);	// use this since toolbox initially allocated storage 
		if(oma_wind[n].windowtype == QLINEGRAPH) {
			CGDataConsumerRelease(oma_wind[n].consumer);		// this also releases the pdfData, via MyCFDataRelease
		}
	}
	

	if( abs(oma_wind[n].windowtype) == ZOOM )
		is_zoom_window = 0;
	if( abs(oma_wind[n].windowtype) == FFT )
		is_fft_window = 0;
	if( abs(oma_wind[n].windowtype) == AUTO )
		is_auto_window = 0;
	if( oma_wind[n].windowtype == QRGBZOOM )
		is_zoom_rgb_window = 0;

	oma_wind[n].window_rgb_data = NULL;	// this pointer is no longer any good
	oma_wind[n].pdfData = NULL;	// this pointer is no longer any good
	oma_wind[n].consumer = NULL;	// this pointer is no longer any good
    // free the memory associated with any labels
    for(i=0; i< oma_wind[n].num_annotations; i++){ 
        switch (oma_wind[n].annotations[i].type) {
            case WINDOW_LABEL:
                free(oma_wind[n].annotations[i].label);
                oma_wind[n].annotations[i].label = NULL;
                break;
            default:
                break;
        } 
    }
    oma_wind[n].num_annotations = 0;
	
	gwnum--;
	if(gwnum <= 0) 
		gwnum = 0;
	
	if(oma_wind[gwnum].windowtype != QROWCOL){
		oma_wind[gwnum].row_marker.window=oma_wind[gwnum].col_marker.window = -1;
	}
	for(i=0; i<n; i++){
		if(oma_wind[i].row_marker.window != -1) {	// this window has an active row plot
			 if(oma_wind[i].row_marker.window > n)
				oma_wind[i].row_marker.window -= 1;	//  gets demoted by 1
		}
		if(oma_wind[i].col_marker.window != -1) {	// this window has an active row plot
			if(oma_wind[i].col_marker.window > n)
				oma_wind[i].col_marker.window -= 1;	//  gets demoted by 1
		}
	}
		
	while( n < gwnum) {
		// QuickDraw only parts
		//ourcmhandle[n] = ourcmhandle[n+1];
		//myGWorldPtrs[n] = myGWorldPtrs[n+1];
		//copyRect[n] = copyRect[n+1];

		if(oma_wind[n+1].row_marker.window != -1) {	// this window has an active row plot
			if(oma_wind[n+1].row_marker.window >= n)
				oma_wind[n+1].row_marker.window -= 1;	// everything gets demoted by 1
		}
		if(oma_wind[n+1].col_marker.window != -1) {	// this window has an active row plot
			if(oma_wind[n+1].col_marker.window >= n)
				oma_wind[n+1].col_marker.window -= 1;	// everything gets demoted by 1
		}

		oma_wind[n] = oma_wind[n+1];
		
		n++;
	}
	if( gwnum == 0)
		thereisabox = false;
	return 0;
}

/* ************* */


int assigncolor1()
{
	unsigned int i, constant,thrd;
	int setcolors_nib();
	extern ColorSpec csa[256];
	extern RGBColor color[256][8];
        
	extern WindowRef Status_window;
	//OSErr	err;
	
	if(thedepth < 4) return -1;
	
	switch(thepalette) {
		case FROMAFILE2:
                        for(i=0; i<=(ncolor+1); i++) 
				thecolor[i] = filecolor2[i];
                        thecolor[254] = thecolor[253];
			break;			
		case FROMAFILE3:
                        for(i=0; i<=(ncolor+1); i++) 
				thecolor[i] = filecolor3[i];
                        thecolor[254] = thecolor[253];
			break;			
		case FROMAFILE:
                        for(i=0; i<=(ncolor+1); i++) 
				thecolor[i] = filecolor[i];
                        thecolor[254] = thecolor[253];
			break;			
		case GRAYMAP:
			for(i=0; i<=(ncolor+1); i++) 
				thecolor[i].red = thecolor[i].green = thecolor[i].blue = i*65536/(1 << thedepth);
			break;
		case REDMAP:
			for(i=0; i<=(NCOLOR+1); i++) { 
				thecolor[i].red = i*65536/(1 << thedepth);
				thecolor[i].green = thecolor[i].blue = 0; }
			break;
		case BLUEMAP:
			for(i=0; i<=(NCOLOR+1); i++) {
				thecolor[i].blue = i*65536/(1 << thedepth);
				thecolor[i].red = thecolor[i].green = 0; }
			break;
		case GREENMAP:
			for(i=0; i<=(NCOLOR+1); i++) {
				thecolor[i].green = i*65536/(1 << thedepth);
				thecolor[i].red = thecolor[i].blue = 0; }
			break;
		case BGRBOW:
		default:
			thrd = (1 << thedepth)/3;
			constant = 65536/thrd;
			for (i=0; i<thrd; i++) {
				thecolor[i].blue = i*constant;
				thecolor[i].red = thecolor[i].green = 0;
				thecolor[i+thrd].blue = thrd*constant - i*constant;
				thecolor[i+thrd].green = i*constant;
				thecolor[i+thrd].red = 0;
				thecolor[i+thrd*2].red = i*constant;
				thecolor[i+thrd*2].green = thrd*constant - i*constant;
				thecolor[i+thrd*2].blue = 0;
			}
	} /* end of palette switch */
	//printf("start\n");
	for(i=0; i<256; i++) {
		csa[i].value = i;
		csa[i].rgb = thecolor[i];
		//printf("%d %d %d\n",thecolor[i].red/256,thecolor[i].green/256,thecolor[i].blue/256);
		color[i][thepalette]= csa[i].rgb;
	}
	//if( themaxdepth >8 ) {
		csa[255].rgb = RGBBlack;	/* be sure color 255 is black */ 
		csa[0].rgb = RGBWhite;
	//} else {
	//	csa[ncolor].rgb = RGBBlack;	/* be sure color 255 is black */ 
	//	csa[0].rgb = RGBWhite;
	//}
	// display of the color palette in the status window relies on setcolors()
	 setcolors_nib();
	return 0;
}

/* ************* */
