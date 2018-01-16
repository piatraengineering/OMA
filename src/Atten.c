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
float x_loc = -1519;
float y_loc = 873;
int UseVirtual = 1;
float Att_Particle_Diameter = 21;
float Att_IPResolution = 225;
float Att_LSThickness = 2000;
float Att_Scaling = 1.5;


//***************************************************************************************//
//**    set_Atten_Values -                                                                                              **//
//**                 Poke values into the dialog box (atten_param_window) when first drawn **//
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
//**                 Peek values into the dialog box (atten_param_window) when first drawn **//
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
//**    A T T S C L - Insert the necessary values for ATTENU. Open dialog if there are no arguments *//
//***************************************************************//
int
attscale (int n, int index)	
{	
	int narg;
	int Err;
	
	// Read in Comand line arguments
	// We want to be able to call this function froma macro without needed to user enter any variables.
	// If arguments are specified, then take the values from command line...
	// ... Otherwise, open a GUI dialog (NIB) to ask for the values 
	narg = sscanf(&cmnd[index],"%f", &Att_Scaling);
	
	if (narg ==0) {
		Err = PK_Build_Atten_Dialog();
		return Err;
	}
	
	Attenuation_Parameters_Set = 1;
	return OMA_OK;
	
}


//***************************************************************//
//**    A T T E N U- Corrections for Attenuation due to dense seeding of particles *//
//**   implemented as Image structures to allow for expansion at a later stage for  *//
//**  non-uniform particle distribution and variations in Ck *//
//** v3.0 */
//***************************************************************//
int attenu (int n, int index)	
{	
	int	nc, nt;
	int Err;	
	
	Image Im_Original;
	Image Im_Result;
	Image Im_Kdiv;
	Image Im_Ktrans;
	Image Im_Ck;
	Image Im_MeanParticleArea;
	Image Im_Admitted;
	Image Im_Np;
	
	float x_dist, y_dist;
	float x_step, y_step;
	float tantheta, costheta, sintheta;
	float Hypot;
	
	DATAWORD DW_old, DW_max;
	DATAWORD Particle_XSectionArea;
	DATAWORD inv_dist;
	
	// Check incoming parameters are OK...
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
	
	// Create a New images to hold the result and other intermediaries____________________________
	if((Err = SDK_Image_CopyBuffer(&Im_Original )) != OMA_OK)	 	                                        { beep(); nomemory(); return Err; }
	if((Err = SDK_Image_Create(&Im_Result,header[NCHAN],header[NTRAK] )) != OMA_OK)                   { beep(); nomemory(); return Err; }
	if((Err = SDK_Image_Create(&Im_Admitted,header[NCHAN],header[NTRAK] )) != OMA_OK)              { beep(); nomemory(); return Err; }
	if((Err = SDK_Image_Create(&Im_Np,header[NCHAN],header[NTRAK] )) != OMA_OK)	                    { beep(); nomemory(); return Err; }
	if((Err = SDK_Image_Create(&Im_Ck,header[NCHAN],header[NTRAK] )) != OMA_OK) 		 	{ beep(); nomemory(); return Err; }
	if((Err = SDK_Image_Create(&Im_Kdiv,header[NCHAN],header[NTRAK] )) != OMA_OK) 		 	{ beep(); nomemory(); return Err; }
	if((Err = SDK_Image_Create(&Im_Ktrans,header[NCHAN],header[NTRAK] )) != OMA_OK) 		{ beep(); nomemory(); return Err; }
	if((Err = SDK_Image_Create(&Im_MeanParticleArea, header[NCHAN],header[NTRAK] )) != OMA_OK) 	{ beep(); nomemory(); return Err; }
	
	// Initialise the Admitted laser power IMAGE ___________________
	for(nc=0; nc<header[NCHAN] ; nc++) {
		for(nt=0; nt< header[NTRAK]; nt++) {
			SDK_Image_SetPixel(&Im_Admitted, nc, nt, (DATAWORD) 1.0000000);
		}
	}


	// Create the K_divergence IMAGE (if there is a V.O.)______________________________
	for(nc=0; nc<header[NCHAN] ; nc++) {
		for(nt=0; nt< header[NTRAK]; nt++) {
			if (UseVirtual) {
				x_dist = x_loc - nc;
				y_dist = y_loc - nt;
				Hypot = sqrt(x_dist*x_dist + y_dist*y_dist);
				inv_dist = (DATAWORD) 2000/Hypot;     // ok. it IS scaled somewhat. Normalised out later.
			} else {
				// If there is no virtual origin the the beam is collimated and this is equal to 1.0 everywhere
				inv_dist = 1.000;
			} 
			SDK_Image_SetPixel(&Im_Kdiv, nc, nt, inv_dist);
		}
	}
	
	// Normalise the K_div IMAGE and fill values into CK and ¹r2 IMAGE
	DW_max = SDK_Image_GetMaxPixel(&Im_Kdiv);
	for(nc=0; nc<header[NCHAN] ; nc++) {
		for(nt=0; nt< header[NTRAK]; nt++) {
			DW_old = SDK_Image_GetPixel(&Im_Kdiv, nc, nt);
			SDK_Image_SetPixel(&Im_Kdiv, nc, nt, DW_old/DW_max);
			// ... and build a Ck image from the user set value (for now)...
			SDK_Image_SetPixel(&Im_Ck, nc, nt, (DATAWORD) Att_Scaling);
			// ... and build an image of varying particle distribution (later), or uniform distribution (for now).
			SDK_Image_SetPixel(&Im_MeanParticleArea, nc, nt, (DATAWORD) pi*Att_Particle_Diameter*Att_Particle_Diameter/4);
		}
	}
	
	// For each pixel in original image.... loop (NB: We are looping column-wise)
	for(nc=0; nc<header[NCHAN] ; nc++) {
		for(nt=0; nt< header[NTRAK]; nt++) {
			if (UseVirtual) {
				// 0: work out distances and trigonometric ratios back to the v.o.
				x_dist = x_loc - nc;
				y_dist = y_loc - nt;
				Hypot = (DATAWORD) sqrt(x_dist*x_dist + y_dist*y_dist);
				costheta = x_dist/Hypot;
				sintheta = y_dist/Hypot;
				tantheta = y_dist/x_dist;
				x_step = (float) (nc - 1);
				y_step = (float) (nt + sintheta);
			} else {
				x_step = (float) (nc - 1);
				y_step = (float) (nt);
			} 
			// 1: Light entering the pixel starts at
			DATAWORD I_in, I_out, I_0;
			I_in = SDK_Image_InterpPixel(&Im_Admitted, x_step, y_step);
			
			// 2: Light entering pixel should be reduced in. prop to dist for divergence
			DATAWORD Div_Ratio;
			Div_Ratio = SDK_Image_GetPixel(&Im_Kdiv, nc, nt)/SDK_Image_InterpPixel(&Im_Kdiv, x_step, y_step);
			I_0 = I_in * Div_Ratio;
			
			// 3: The light in pixel volume reduced by particles
			DATAWORD Signal, Np, Ck;
			Signal = SDK_Image_GetPixel(&Im_Original, nc, nt);
			Particle_XSectionArea = SDK_Image_GetPixel(&Im_MeanParticleArea, nc, nt);
			Ck = SDK_Image_GetPixel(&Im_Ck, nc, nt);
			
			// 3.1: Estimate n_p from signal
			if ((Signal ==0)||(Ck==0)||(Particle_XSectionArea == 0)||(I_0 ==0))
				Np = 0;
			else 
				Np =Signal / Ck / Particle_XSectionArea/ I_0;
			
			if (Np==Np) SDK_Image_SetPixel(&Im_Np, nc, nt, Np);
			
			DATAWORD Ktrans_pixel, PR;
			// Transmitted is (1 - A_particle/A_pixel) to power of Np
			PR =  1 - Particle_XSectionArea/(Att_IPResolution*Att_LSThickness);
			Ktrans_pixel = (DATAWORD) pow(PR, Np);
			SDK_Image_SetPixel(&Im_Ktrans, nc, nt, Ktrans_pixel);
			I_out = I_in * Ktrans_pixel;
			SDK_Image_SetPixel(&Im_Admitted, nc, nt, I_out);
			
			// 4: Do image based arithmetic to correct signal
			DATAWORD Result;
			Result = (DATAWORD) ( (float)Signal / I_in);
			SDK_Image_SetPixel(&Im_Result, nc, nt, Result); 
		}
	}
	
	// Place the result IMAGES into the OMA buffers
	SDK_Image_2Buffer(&Im_Result);
	SDK_Image_2Temp(&Im_Original, 0);
	SDK_Image_2Temp(&Im_Kdiv, 1);
	SDK_Image_2Temp(&Im_Ktrans, 2);
	SDK_Image_2Temp(&Im_Ck, 3);
	SDK_Image_2Temp(&Im_MeanParticleArea, 4);
	SDK_Image_2Temp(&Im_Np, 5);
	SDK_Image_2Temp(&Im_Admitted, 6);
	
	// Clean UP and GO.
	SDK_Image_Free(&Im_Original);
	SDK_Image_Free(&Im_Result);
	SDK_Image_Free(&Im_Kdiv);
	SDK_Image_Free(&Im_Ktrans);
	SDK_Image_Free(&Im_Ck);
	SDK_Image_Free(&Im_MeanParticleArea);
	SDK_Image_Free(&Im_Np);
	SDK_Image_Free(&Im_Admitted);
	SDK_Image_Refresh();
	return OMA_OK;
}

//***************************************************************//
//**    A T T P R T - Print out the variables used in ATTENU to the Command Window  *//
//**                         (used to check that the dialog is working correctly)  **//
//***************************************************************//
int attprt ()	
{	
	pprintf("ATTENU will be called with the following parameters:\n");
	pprintf("Virtual Origin is at (%f, %f)\n", x_loc, y_loc);
	pprintf("Particle Diameter is %f\n", Att_Particle_Diameter);
	pprintf("In plane resolution is %f\n", Att_IPResolution);
	pprintf("Laser sheet thickness is %f\n", Att_LSThickness);
	pprintf("Scale factor is %f\n", Att_Scaling);
	pprintf("Are we using Virtual Origin?  %d\n", UseVirtual);
	
	return OMA_OK;
}
//***************************************************************//
//**    A T T D I A - Set the particle diameter from the command line  **//
//**                         Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  *//
//***************************************************************//
int attdia (int n, int index)	
{	
	int narg;
	int Err;
	
	// Read in Comand line arguments
	// We want to be able to call this function froma macro without needed to user enter any variables.
	// If arguments are specified, then take the values from command line...
	// ... Otherwise, open a GUI dialog (NIB) to ask for the values 
	narg = sscanf(&cmnd[index],"%f", &Att_Particle_Diameter);
	
	if (narg ==0) {
		Err = PK_Build_Atten_Dialog();
		return Err;
	}
	
	Attenuation_Parameters_Set = 1;
	return OMA_OK;
	
}
//***************************************************************//
//**    A T T I P R - Set the In Plane Resolution (IPR) from the command line  **//
//***************************************************************//
int attipr (int n, int index)	
{	
	int narg;
	int Err;
	
	// Read in Comand line arguments
	// We want to be able to call this function froma macro without needed to user enter any variables.
	// If arguments are specified, then take the values from command line...
	// ... Otherwise, open a GUI dialog (NIB) to ask for the values 
	narg = sscanf(&cmnd[index],"%f", &Att_IPResolution);
	
	if (narg ==0) {
		Err = PK_Build_Atten_Dialog();
		return Err;
	}
	
	Attenuation_Parameters_Set = 1;
	return OMA_OK;
	
}
//***************************************************************//
//**    A T T L S T - Set the Laser Sheet Thickness (LST) from the command line  **//
//***************************************************************//
int attlst (int n, int index)	
{	
	int narg;
	int Err;
	
	// Read in Comand line arguments
	// We want to be able to call this function froma macro without needed to user enter any variables.
	// If arguments are specified, then take the values from command line...
	// ... Otherwise, open a GUI dialog (NIB) to ask for the values 
	narg = sscanf(&cmnd[index],"%f", &Att_LSThickness);
	
	if (narg ==0) {
		Err = PK_Build_Atten_Dialog();
		return Err;
	}
	
	Attenuation_Parameters_Set = 1;
	return OMA_OK;
	
}
//***************************************************************//
//**    V O S M O O - Virtually oriented smoothing                     *//
//**                         Kalt, P.A.M.                                                 *//
//***************************************************************//
int vo_smooth (int n, int index)	
{	
	int narg;
	int dy;
	int nc, nt, i;
	int step;
	int num;
	DATAWORD sum;
	Image Im_Original;
	Image Im_Result;
	
	float x_dist, y_dist;
	float x_step, y_step;
	float costheta, sintheta;
	DATAWORD Hypot;
	int Err;

	// Check that we are ready to do VO processing
	if (!Attenuation_Parameters_Set){
		beep();
		printf("Must Set parameters [use ATTVAL] for this function before it can be used\n");
		return OMA_ARGS;
	}
	
	// Create a New image to hold the original data
	if((Err = SDK_Image_CopyBuffer(&Im_Original ))!=OMA_OK){ 
		beep(); nomemory(); return Err;
	}
	
	// Create an Image to hold the result
	if((Err = SDK_Image_Create(&Im_Result,header[NCHAN],header[NTRAK] ))!=OMA_OK){ 
		beep(); nomemory(); return Err;
	}
	
	// Read in command line arguments. dx = length along line toward origin
	narg = sscanf(&cmnd[index],"%d",&dy);
	if (narg !=1) dy = 3;
	step = (int) (dy-1)/2;
	
	// Loop for each pixel in our result
	for(nc=0; nc<header[NCHAN] ; nc++) {
		for(nt=0; nt< header[NTRAK]; nt++) {
			// Work out the direction to the VO
			x_dist = x_loc - nc;
			y_dist = y_loc - nt;
			Hypot = (DATAWORD) sqrt(x_dist*x_dist + y_dist*y_dist);
			costheta = x_dist/Hypot;
			sintheta = y_dist/Hypot;
			sum = num = 0;
			for (i=-step; i<=step; i++){
				// Take as many jumps around our point
				x_step = (float) (nc + i * costheta);
				y_step = (float) (nt + i *sintheta);
				num ++;
				sum += SDK_Image_InterpPixel(&Im_Original, x_step, y_step);
			}
			SDK_Image_SetPixel(&Im_Result, nc, nt, (DATAWORD) sum/num);
		}
	}
	
	// Prepare for departure...
	SDK_Image_2Buffer(&Im_Result);
	SDK_Image_Free(&Im_Original);
	SDK_Image_Free(&Im_Result);
	SDK_Image_Refresh();
	return OMA_OK;
}


DATAWORD
SDK_Image_AvgAlongLine(Image *imptr, short p1x, short p1y, short p2x, short p2y)
{
	float x_dist, y_dist;
	float x_step, y_step;
	float gradient, c_pos;
	DATAWORD sum = 0;
	int i;
	short width;
	
	// Work out values we will need for later
	x_dist = p2x - p1x;
	y_dist = p2y - p1y;
	gradient = y_dist/x_dist;  // rise/run
	width = imptr->width;
	
	// Delete this later
	if (width >100) width = 100;
	
	c_pos = p1y - gradient*p1x;
	// For each point across image
	for (i=0; i<width;i++){
		// find the y-position on line for this x (ie i)
		x_step = i;
		y_step = gradient* i + c_pos;
		sum += SDK_Image_InterpPixel(imptr, x_step, y_step); 
	}
	if (width ==0) return 0;
	else return (DATAWORD) sum;	
}

//***************************************************************//
//**    V O A V G - Virtually oriented averaging                     *//
//**                         Kalt, P.A.M.                                                 *//
//***************************************************************//
int vo_average (int n, int index)	
{	
	Image Im_Original;
	Image Im_Result;

	int nc, nt;
	DATAWORD sum;
	int Err;
	
	// Check that we are ready to do VO processing
	if (!Attenuation_Parameters_Set){
		beep();
		printf("Must Set parameters [use ATTVAL] for this function before it can be used\n");
		return OMA_ARGS;
	}
	
	// Create a New image to hold the original data
	if((Err = SDK_Image_CopyBuffer(&Im_Original ))!=OMA_OK){ 
		beep(); nomemory(); return Err;
	}
	
	// Create an Image to hold the result
	if((Err = SDK_Image_Create(&Im_Result,header[NCHAN],header[NTRAK] ))!=OMA_OK){ 
		beep(); nomemory(); return Err;
	}
	
	// Loop for each pixel in our result
	for(nc=0; nc<header[NCHAN] ; nc++) {
		for(nt=0; nt< header[NTRAK]; nt++) {
			sum = SDK_Image_AvgAlongLine(&Im_Original, nc, nt, x_loc, y_loc);
			SDK_Image_SetPixel(&Im_Result, nc, nt, sum);
		}
	}
		
	// Prepare for departure...
	SDK_Image_2Buffer(&Im_Result);
	SDK_Image_Free(&Im_Original);
	SDK_Image_Free(&Im_Result);
	SDK_Image_Refresh();
	return OMA_OK;
}
