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
#include	"impdefs.h"
#include "oma_quartz.h"
/*
 * Resource ID constants.
 */
#define starID			133
#define nu200ID			134
#define st6ID			135
#define SensiCamID		136



/* define starMenu */
#define	biasCommand			1
#define sclearCommand		2
#define darkCommand			3
#define exposeCommand		4
#define focusCommand		5
#define trigCommand			6
#define setstarCommand		8

/* Within nu200Menu */
#define	nubiasCommand		1
#define nuclearCommand		2
#define nudarkCommand		3
#define nuexposeCommand		4
#define nufocusCommand		5
#define nugainCommand		6
#define nutregCommand		7
#define nutrigCommand		8

/* Within st6Menu */
#define	st6darkCommand		1
#define st6exposeCommand	2
#define st6focusCommand		3
#define st6settingsCommand	5

/* Within SensiCamMenu */
#define	SensiCamStatusCommand		1
#define SensiCamexposeCommand	2
#define SensiCamfocusCommand		3
#define SensiCamsettingsCommand	5

#define menuCount		 5

/*
 * HIWORD and LOWORD macros, for readability.
 */
#define HIWORD(aLong)		(((aLong) >> 16) & 0xFFFF)
#define LOWORD(aLong)		((aLong) & 0xFFFF)

extern	MenuHandle		MyMenus[]; 		/* The menu handles */
extern	Boolean			answerback;		/* tells if a menu item is sending text to main OMA */
extern	Boolean			printall;		/* the flag for "priority only" printing */
		
extern Boolean			loshowbox;		/* whether or not to show line out boxes */
extern	short			pastelines;		/*?*/
//extern 	GWorldPtr	myGWorldPtrs[]; // offscreen maps

extern TXNObject object;

//static int IDlist[] = {starID,nu200ID,st6ID,SensiCamID};

int  DoAboutBox(),import_file();
 
void redoMenus(int menunumber)
{
	/*
	extern		MenuHandle	MyMenus[];

	MyMenus[menunumber] = GetMenu(IDlist[menunumber]);
	
	InsertMenu(MyMenus[menunumber], 0);
	*/
	DrawMenuBar();

}


int keypct(int n, int index)
{
	contour(nil);
	return 0;
}

int keyinteg(int n, int index)
{
	plotone(nil);
	return 0;
}


//___________________

OSStatus DoHIMenuCommand(HICommand* thecommand)
{

    long				tlength;		// the length of cut text     
    short				tmp,numlzs;
    WindowPtr			front_window;
    
    int					wnum;
    //PicHandle 			thepict;
    //Rect				theRect;
    //ScrapRef			scrap;
    
    ScrapRef the_scrap;
    char scrapdata[CHPERLN];

    OSStatus	anErr;
	OSStatus result = eventNotHandledErr;
	
	OSStatus AddWindowContentToPasteboardAsPDF( PasteboardRef , int);
	int plotchans_q(),histogram_q(),plot_both(),exitx();
	PasteboardRef GetPasteboard(void);
	
	//extern OMA_Window oma_wind[];
	
    //extern Rect copyRect[];
    //extern WindowPtr 	gwind[];		// an array of graphics window pointers 
    extern WindowRef Status_window,RGB_Tools_window;
    extern  WindowPtr	Ctwind;					// the text window
    extern int  gwnum, typing_pointer;
    
    extern void 		showAboutMeDialog();
    extern void             set_st6();
	extern char* gets_string;
	extern char contents_path[];

    
	int *ptri;
	char* string;
	int savpdf(int n, int index);
	FSRef MyFSRef;
	char oldname[CHPERLN];

	string = gets_string;
	ptri = &typing_pointer;
    switch (thecommand->commandID) 
    {
                case 'abou':
                    DoAboutBox();
                    return noErr;

                case 'help':
					getcwd(oldname,256);
					chdir(contents_path);
					anErr = FSPathMakeRef((unsigned char*)HELPURL,&MyFSRef,NULL);
                    anErr = LSOpenFSRef(&MyFSRef,NULL);
					chdir(oldname);
                    return anErr;

                case 'site':
                    anErr = LSOpenCFURLRef(CFURLCreateWithString(NULL,
							CFStringCreateWithCString(NULL,"http://oma-x.org/index.html",kCFStringEncodingMacRoman),NULL),NULL);
                    return anErr;
                    
                case 'quit':
                    exitx();
                    ExitToShell();
                    //DoneFlag = true;			// Request exit
                    return noErr;

                case 'ODAT':
                    getdatafile();
                    return noErr;
                case 'SDAT':
                    savedatafile();
                    return noErr;
                case 'SPCT':
                    //putfile(nil);
					savpdf(0,0);
                    return noErr;
                case 'OPCT':
                    import_file();
                    return noErr;
                case 'SPRE':
                    setprefix();
                    return noErr;
                case 'SAVS':
                    saveprefs(nil);
                    return noErr;
                case 'GETS':
                    loadprefs(nil);
                    return noErr;
                case 'CLOS':
                    front_window = FrontWindow();
                    if(front_window == Ctwind || front_window == Status_window || front_window == RGB_Tools_window){
                        SysBeep(1);
                        break;	
                    }
                    closewindow(activegwnum(front_window),0);
                    return noErr;
                case 'SREC':
                    saverectangle(nil);
                    return noErr;
                case 'LPAL':
                    getpalettefile(nil);
                    return noErr;
                case 'copy':
                    if( Ctwind != FrontWindow() ) {
                            
                        if( (wnum = activegwnum( FrontWindow())) >= gwnum) 
                            break;
						anErr = AddWindowContentToPasteboardAsPDF( GetPasteboard(), wnum);
                        //picture copying code from PICTFile.c -- seems to need a long pause to work properly
                        //SetPortWindowPort(oma_wind[wnum].gwind);

                        //assigncolor1();			// clean up after the dialog boxes  ??? why this ???
/*                            
                        GetWindowPortBounds(oma_wind[wnum].gwind, &theRect);
                        ClipRect(&theRect); 
                        thepict = OpenPicture(&theRect);
                        LockPixels( GetGWorldPixMap(myGWorldPtrs[wnum]));
                        CopyBits( (BitMap*) *GetPortPixMap(myGWorldPtrs[wnum]),GetPortBitMapForCopyBits(GetWindowPort(oma_wind[wnum].gwind)),
                                    &copyRect[wnum],&theRect,0,0);
*/
                        //ClosePicture();
						/*
                        do
                            {
                            //if (anErr) break;
                            anErr = ClearCurrentScrap ( );
                            if (anErr) break;
                            anErr = GetCurrentScrap (&scrap);
                            MoveHHi ((Handle) thepict);
                            anErr = MemError ( );
                            if (anErr) break;
                            HLock ((Handle) thepict);
                            anErr = MemError ( );
                            if (anErr) break;
                            anErr = PutScrapFlavor (scrap, 'PICT', 0, GetHandleSize((Handle) thepict), *thepict);
                            if (anErr) break;
                            }
                            while (false);

                        KillPicture(thepict);
                        */    
                    }
                    else {
                            TXNCopy(object);  
                    }
                    return noErr;
                case 'past':					 
                            
                    anErr = GetCurrentScrap (&the_scrap);
                    //printf("%d Err\n",anErr);
                    tlength = CHPERLN;
                    anErr = GetScrapFlavorData (the_scrap,(ScrapFlavorType) 'TEXT',&tlength, scrapdata );

                    TXNPaste(object);
                    
                    //printf("%d Err %d length, %s string\n",anErr,tlength,scrapdata);

                    if (anErr == noErr) {									
                        if(tlength + *ptri >= CHPERLN)
                            tlength = CHPERLN -1 - *ptri; 	// make sure that this is not too long 
                
                        numlzs = 0;				// the number of leading zeros 
                        for(tmp=0; tmp < tlength; tmp++) {	// strip away any leading 0 
                            if( scrapdata[tmp] == 0)
                                numlzs++;
                        }
                        //	printf("%d \n",numlzs);
                
                        BlockMove(&scrapdata[numlzs],&string[*ptri],tlength-numlzs);
                
                        pastelines = 0;	// ?
                        for(tmp=*ptri; tmp < *ptri+tlength-numlzs; tmp++){
                        //	printf("%d \n",string[tmp]);
                            if(string[tmp] == '\n') {
                                string[tmp] = 0;
                                pastelines++;
                                answerback = true;		// inform the event loop of input 
         
                            }
                        }
                        *ptri += tlength;
                    }
                    return noErr;

                case 'LPRT':
                    //CheckMenuItem(MyMenus[editMenu], limitprintCommand, printall);
                    printall = !printall;
                    return noErr;
				case 'PCTR':
                    contour(nil);
                    return noErr;
                case 'SCTR':
                    setcontour();
                    return noErr;
                case 'PCOL':
					plotchans_q(1);
					return noErr;
                case 'PROW':
                    plotchans_q(0);
                    return noErr;
                case 'PBOT':
                    plot_both();
                    return noErr;
                case 'PSUR':
                    surface("Surface Plot");
                    return noErr;
                case 'SSUR':
                    setsurface();
                    return noErr;
                case 'SHIS':
                    sethist();
                    return noErr;
                case 'PHIS':
                    histogram_q(nil);
                    return noErr;
                case 'SINT':
                    setplotone();
                    return noErr;
                case'PINT':
                    plotone(nil);
                    return noErr;
                case 'TEMP':
                    oma_template();
                    return noErr;
                case 'SBOX':
                    loshowbox = !loshowbox;
                    //CheckMenuItem(MyMenus[lineoutMenu], theItem, loshowbox);
                    return noErr;
            /*            
                default:
                    break;
            }
            break;
#ifdef SensiCam
        case SensiCamID:
            switch (theItem) {
                case SensiCamStatusCommand:
                    SensiCamStatusDialog();
                    break;
                case SensiCamexposeCommand:
                    printf("Not Implemented.\n");
                    break;
                case SensiCamfocusCommand:
                    printf("Not Implemented.\n");
                    break;
                case SensiCamsettingsCommand:
                    SensiCamSettingsDialog();
                    break;
                        
                default:
                    break;
            }
            break;

#endif
		*/
        default:
                break;

    } //endsw theMenu
	
    HiliteMenu(0);	// unhighlight what MenuSelect (or MenuKey) hilited 
	return result;
}




