/*
 ATTEN.c -- Photometric Image Processing and Display
 Copyright (C) 2003  by the Developers of OMA
 *
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
#include "custom.h"
#include "SDK.h"

// The following is necessary redefinition (from omadialogsx.h) in order to prevent the
// ZeroLink linker from crapping itself when  SET_CONTROL_FVALUE is called
// otherwise OMA will exit with SIGABRT (6).
#define SET_CONTROL_FVALUE(theValue,theID,theField) 	\
text = CFStringCreateWithFormat( NULL, NULL, CFSTR("%g"), theValue );\
err = SetControlData( theField, 0,kControlEditTextCFStringTag,sizeof(CFStringRef), &text );\
CFRelease( text );\
DrawOneControl( theField );

#define SET_CONTROL_SVALUE(theValue,theID,theField) 	\
text = CFStringCreateWithFormat( NULL, NULL, CFSTR("%s"), theValue );\
err = SetControlData( theField,0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text );\
CFRelease( text );\
DrawOneControl( theField );

// Declare Window name and Control ID's for Window elements (atten_param_window)
WindowRef atten_param_window;
ControlRef att_Mode;  ControlID att_ModeID ={ 'Patt', 301 };
ControlRef att_VirtualX;  ControlID att_VirtualXID ={ 'Patt', 303 };
ControlRef att_VirtualY;  ControlID att_VirtualYID ={ 'Patt', 304 };
ControlRef att_Scale;  ControlID att_ScaleID ={ 'Patt', 306 };
ControlRef att_Diameter;  ControlID att_DiameterID ={ 'Patt', 307 };
ControlRef att_Res;  ControlID att_ResID ={ 'Patt', 308 };
ControlRef att_Thick;  ControlID att_ThickID ={ 'Patt', 309 };



// LOCAL VARIABLES for the Attenuation Functions

int Attenuation_Parameters_Set = 1;
float x_loc = -10;
float y_loc = 50;
int UseVirtual = 1;
float Att_Particle_Diameter = 10;
float Att_IPResolution = 10;
float Att_LSThickness = 10;
float Att_Scaling = 1;


//***************************************************************************************//
//**    set_Atten_Values -                                                                                              **//
//**                 Poke values into the dialog box (atten_param_window) when first drawn **//
//**                 Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  **//
//***************************************************************************************//
int set_Atten_Values(){
	CFStringRef text;
	OSStatus err;
	
	// Get a handle to the various window elements
	GetControlByID( atten_param_window, &att_ModeID, &att_Mode );
	GetControlByID( atten_param_window, &att_VirtualXID, &att_VirtualX );
	GetControlByID( atten_param_window, &att_VirtualYID, &att_VirtualY );
	GetControlByID( atten_param_window, &att_ScaleID, &att_Scale );
	GetControlByID( atten_param_window, &att_DiameterID, &att_Diameter );
	GetControlByID( atten_param_window, &att_ResID, &att_Res );
	GetControlByID( atten_param_window, &att_ThickID, &att_Thick );
	
	//Click the default value of the Radio Buttons controlling MODE
	SetControl32BitValue(att_Mode,UseVirtual+1);
	
	//Write the initial values into the window (x_loc, y_loc)
	SET_CONTROL_FVALUE(x_loc,att_VirtualXID,att_VirtualX);
	SET_CONTROL_FVALUE(y_loc,att_VirtualYID,att_VirtualY);
	
	//Write the initial values into the window (scaling constant)
	SET_CONTROL_FVALUE(Att_Scaling,att_ScaleID,att_Scale);
	//Write the initial values into the window (diameter)
	SET_CONTROL_FVALUE(Att_Particle_Diameter,att_DiameterID,att_Diameter);
	//Write the initial values into the window (resolutions)
	SET_CONTROL_FVALUE(Att_IPResolution,att_ResID,att_Res);
	SET_CONTROL_FVALUE(Att_LSThickness,att_ThickID,att_Thick);
	
	return err;
}


//***************************************************************************************//
//**    get_Atten_Values -                                                                                              **//
//**                 Poke values into the dialog box (atten_param_window) when first drawn **//
//**                 Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  **//
//***************************************************************************************//
int get_Atten_Values(){
	CFStringRef text;
	Size actualSize;
	
	// Read the new Mode (whether collimated or uncollimated)
	UseVirtual = GetControl32BitValue(att_Mode);
	UseVirtual--;
	
	// Read out the values of the dialog boxes as doubles into floats (x_loc, y_loc)
	GetControlData( att_VirtualX, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
	x_loc = CFStringGetDoubleValue( text );
	CFRelease( text );
	GetControlData( att_VirtualY, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
	y_loc = CFStringGetDoubleValue( text );
	CFRelease( text );
	
	// Read out the values of the dialog boxes as doubles into floats (scaling constant)
	GetControlData( att_Scale, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
	Att_Scaling = CFStringGetDoubleValue( text );
	CFRelease( text );
	
	// Read out the values of the dialog boxes as doubles into floats (diameter)
	GetControlData( att_Diameter, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
	Att_Particle_Diameter = CFStringGetDoubleValue( text );
	CFRelease( text );	
	
	// Read out the values of the dialog boxes as doubles into floats (resolutions)
	GetControlData( att_Res, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
	Att_IPResolution = CFStringGetDoubleValue( text );
	CFRelease( text );
	GetControlData( att_Thick, 0, kControlEditTextCFStringTag, sizeof(CFStringRef), &text, &actualSize );
	Att_LSThickness = CFStringGetDoubleValue( text );
	CFRelease( text );
	
	return OMA_OK;
}

//***************************************************************************************//
//**    PK_Atten_EventHandler -                                                                                     **//
//**                 Handle the Modal dialog controls for the atten_param_window               *//
//**                 Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  *//
//***************************************************************************************//
pascal OSStatus PK_Atten_EventHandler (EventHandlerCallRef myHandler, EventRef event, void *userData)
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
            get_Atten_Values();
            stopModalLoop = TRUE;
            result = noErr;
			Attenuation_Parameters_Set = 1;
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


//***************************************************************************************//
//**    PK_Build_Atten_Dialog -                                                                                     **//
//**                 Build an instance of the Atten_Values dialog in the OMA NIB file             *//
//**                 Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  *//
//***************************************************************************************//
int PK_Build_Atten_Dialog(){
	IBNibRef nibRef;
	
	
    EventTypeSpec 	dialogSpec = {kEventClassCommand, kEventCommandProcess };
    EventHandlerUPP	dialogUPP;
	
    OSStatus err = eventNotHandledErr;
	
    // Get the window from the nib and show it
    err = CreateNibReference( CFSTR("oma"), &nibRef );
    err = CreateWindowFromNib( nibRef, CFSTR("Atten_values"), &atten_param_window );
    DisposeNibReference( nibRef );
	
    // Install our event handler
    dialogUPP =  NewEventHandlerUPP (PK_Atten_EventHandler);
    err = InstallWindowEventHandler (atten_param_window, dialogUPP, 1, &dialogSpec, (void *) atten_param_window, NULL);
	
    // set all of the pane values
    set_Atten_Values();
	
    ShowWindow(atten_param_window );
	
    // Run modally
    RunAppModalLoopForWindow(atten_param_window);
	
    HideWindow(atten_param_window);
    DisposeWindow(atten_param_window);
    DisposeEventHandlerUPP(dialogUPP);	
	
	return OMA_OK;
}

//***************************************************************// 
//**    A T T V A L - Insert the necessary values for ATTENU. Open dialog if there are no arguments *//
//**                         Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  *//
//***************************************************************//
int
attval (int n, int index)	
{	
	int narg;
	int Err;
	
	// Read in Comand line arguments
	// We want to be able to call this function froma macro without needed to user enter any variables.
	// If arguments are specified, then take the values from command line...
	// ... Otherwise, open a GUI dialog (NIB) to ask for the values 
	narg = sscanf(&cmnd[index],"%f %f", &x_loc, &y_loc);
	
	if (narg ==0) {
		Err = PK_Build_Atten_Dialog();
		return Err;
	}
	
	if(narg != 2) {
		x_loc = 0.0;
		y_loc = 0.0;
		printf("Incomplete arguments - We will be taking assumed values\n");
	}
	Attenuation_Parameters_Set = 1;
	return OMA_OK;
	
}



//***************************************************************//
//**    A T T E N U- Corrections for Attenuation due to dense seeding of particles *//
//**                         Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  *//
//***************************************************************//
int attenu (int n, int index)	
{	
	int	nc, nt;
	Image Result;
	Image Absorbed;
	Image Admitted;
	
	
	float x_dist, y_dist;
	float x_step, y_step;
	float costheta, sintheta;
	DATAWORD Hypot;
	DATAWORD sum_on_path;
	DATAWORD Phi;
	DATAWORD Mu;
	DATAWORD N_p;
	int dist;
	int Err;	
	int i;    // loop counters
	
	if (!Attenuation_Parameters_Set){
		beep();
		printf("Must Set parameters [use ATTVAL] for this function before it can be used\n");
		return OMA_ARGS;
	}
	
	//Check that we can handle the location of the Virtual Origin...
	if ((x_loc > 0)||(y_loc > header[NTRAK])){
		beep();
		printf("Sorry! For now I can only handle a VO on the LHS of the image\n Use ROTATE to help me out, eh?\n");
		return OMA_RANGE;
	}
	
	// Create a New image to hold the result and other intermediaries
	if((Err = SDK_Image_Create(&Result,header[NCHAN],header[NTRAK] ))!=OMA_OK){ 
		beep(); nomemory(); return Err;
	}
	if((Err = SDK_Image_Create(&Absorbed,header[NCHAN],header[NTRAK] ))!=OMA_OK){ 
		beep(); nomemory(); return Err;
	}
	if((Err = SDK_Image_Create(&Admitted,header[NCHAN],header[NTRAK] ))!=OMA_OK){ 
		beep(); nomemory(); return Err;
	}
	// As a test of some new functions lets write the original array into the image Absorb and then use 
	// SDK_Image_PixelInterp()
	if((Err = SDK_Image_CopyBuffer(&Absorbed))!=OMA_OK){ 
		beep(); printf("We fucked up\n"); return Err;
	}
	
	// For each pixel in original image.... loop (NB: We are looping column-wise)
	for(nc=0; nc<header[NCHAN] ; nc++) {
		for(nt=0; nt< header[NTRAK]; nt++) {
			
			if (UseVirtual) {
				// Step 1: Estimate the \PHI (Admission) from precalculated ¬µ (Absorption)
				//     Step 1.1: Build a loop to trace along laser path but STOP at edge of image to save time
				x_dist = x_loc - nc;
				y_dist = y_loc - nt;
				Hypot = (DATAWORD) sqrt(x_dist*x_dist + y_dist*y_dist);
				dist = (int) Hypot;
				sum_on_path = 0;				
				//     Step 1.2: Integrate along the path keeping a total of the Absoptions
				for (i = 1; i<dist; i++) {
					costheta = x_dist/Hypot;
					sintheta = y_dist/Hypot;
					x_step = (float) (nc + i*costheta);
					y_step = (float) (nt + i*sintheta);
					if ((x_step >=0)&&(y_step>=0)&&(x_step< header[NCHAN])&&(y_step <header[NTRAK])){
						sum_on_path += SDK_Image_InterpPixel(&Absorbed,x_step, y_step);
					}
				}				
				//     Step 1.3: Calculate \PHI
				Phi = 1 - sum_on_path;
				if (Phi<0) Phi = 0;
				if((Err = SDK_Image_SetPixel(&Admitted, nc, nt, Phi))!=OMA_OK) return Err;
				
				// Step 2: Use \PHI (Admission) and scaling constant to calculate N_p (Result)
				if (Phi!=0) N_p = PKGetPixel(nc,nt) / Att_Scaling / Phi;
				else  N_p = 0;
				if((Err = SDK_Image_SetPixel(&Result, nc, nt, N_p))!=OMA_OK) return Err;
				
				// Step 3: Use N_p (Result) and size variables to get next part of ¬µ(i,j) (Absorption)
				if (N_p == 0) {
					Mu = 0;
				} else {
					Mu = 1- pow( (1-(pi*Att_Particle_Diameter*Att_Particle_Diameter) / (4*Att_IPResolution*Att_LSThickness)),N_p);
				} 
				
				if((Err = SDK_Image_SetPixel(&Absorbed, nc, nt, Mu))!=OMA_OK) return Err;
				
			} else {
				beep();
				printf("Sorry. For now we are only handling the case of a virtual origin\n");
				SDK_Image_Free(&Result); SDK_Image_Free(&Absorbed); SDK_Image_Free(&Admitted);
				return OMA_MISC;
			}
		}
	}
	SDK_Image_2Buffer(&Absorbed);
	SDK_Image_Free(&Result);
	SDK_Image_Free(&Absorbed);
	SDK_Image_Free(&Admitted);
	SDK_Image_Refresh();
	return OMA_OK;
}
//***************************************************************//
//**    A T T P R T - Print out the variables used in ATTENU to the Command Window  *//
//**                         (used to check that the dialog is working correctly)  **//
//**                         Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  *//
//***************************************************************//
int attprt ()	
{	
	printf("ATTENU will be called with the following parameters:\n");
	printf("Virtual Origin is at (%f, %f)\n", x_loc, y_loc);
	printf("Particle Diameter is %f\n", Att_Particle_Diameter);
	printf("In plane resolution is %f\n", Att_IPResolution);
	printf("Laser sheet thickness is %f\n", Att_LSThickness);
	printf("Scale factor is %f\n", Att_Scaling);
	printf("Are we using Virtual Origin?  %d\n", UseVirtual);
	
	return OMA_OK;
}

