/*
 SOFTWARE DEVELOPERS KIT FOR OMA
 ______________________________________
 OMAX -- Photometric Image Processing and Display
 Copyright (C) 2003  by the Developers of OMA
 
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
#ifdef Mac
#include "oma_quartz.h"
int chooseposn();
int set_window_type();
int openwindow_q();
Ptr Get_rgb_from_image_buffer();
#endif


//****needs prototypes *****************************************************************//
int is_power_2();
int FT2D();
int omaprintf();
//***************************************************************//
//**    PKCycleVector - Cycle through peaks of selected vector (x,y)   *//
//***************************************************************//
int
PKCycleVector(int num_x,int num_y)	
{	
    int			num;
	
    // Calc position in xpeaks/ypeaks arrays
    num = (num_y * no_of_velx) + num_x;
	
    // Get vector based on next peak and plot
    piv_reorder( num+1, 0);
    return OMA_OK;	
}
//***************************************************************//
//**    PKGetNthVectorX - Get X-component of the specified peak (x,y)  *//
//***************************************************************//
float
PKGetNthVectorX(int n_x,int n_y, int peak)	
{	
	int       num;
	float        returnfloat;
		  if(no_of_velx == 0) {
			  beep();
			  pprintf("Error %d: Must have valid PIV image first.\n", OMA_NOEXIST);
			  return OMA_NOEXIST;
		  }
		  
		  if (n_x >no_of_velx) n_x = no_of_velx;
		  if (n_y >no_of_vely) n_y = no_of_vely;
		  if (n_x < 0) n_x = 0;
		  if (n_y < 0) n_y = 0;
		  if ((peak <1)||(peak >= peaks_to_save)) peak = 1;
		  
		  // Find position in xpeaks/ypeaks array
		  num = ( (n_y * no_of_velx) + n_x) * peaks_to_save;
		  returnfloat = xpeaks[num+peak]-xpeaks[num];
		  return (returnfloat);
}
//***************************************************************//
//**    PKGetNthVectorY - Get Y-component of the specified peak (x,y)                  *//
//***************************************************************//
float
PKGetNthVectorY(int num_x,int num_y, int peak)	
{	
	int		num, nump;
	
		  if(no_of_velx == 0) {
			  beep();
			  pprintf("Error %d: Must have valid PIV image first.\n", OMA_NOEXIST);
			  return OMA_NOEXIST;
		  }
		  
		  if (num_x >no_of_velx) num_x = no_of_velx;
		  if (num_y >no_of_vely) num_y = no_of_vely;
		  if (num_x < 0) num_x = 0;
		  if (num_y < 0) num_y = 0;
		  if ((peak <1)||(peak >= peaks_to_save)) peak = 1;
		  
		  // Find position in xpeaks/ypeaks array
		  num = (num_y * no_of_velx) + num_x;
		  nump = num*peaks_to_save;
		  
		  return (ypeaks[nump+peak]-ypeaks[nump]);
}
//***************************************************************//
//**    PKGetNthVectorZ - Get Z-component of the specified peak (x,y)   *//
//***************************************************************//
float
PKGetNthVectorZ(int num_x,int num_y, int peak)	
{	
	return ((float)0);
}
//***************************************************************//
//**    PKGetNthPeak - Get strength of the specified peak (x,y)                  *//
//***************************************************************//
float
PKGetNthPeak(int num_x,int num_y, int peak)	
{	
	int		num, nump;
	
		  if(no_of_velx == 0) {
			  beep();
			  pprintf("Error %d: Must have valid PIV image first.\n", OMA_NOEXIST);
			  return OMA_NOEXIST;
		  }
		  
		  if (num_x >no_of_velx) num_x = no_of_velx;
		  if (num_y >no_of_vely) num_y = no_of_vely;
		  if (num_x < 0) num_x = 0;
		  if (num_y < 0) num_y = 0;
		  if ((peak <1)||(peak >= peaks_to_save)) peak = 1;
		  
		  // Find position in xpeaks/ypeaks array
		  num = (num_y * no_of_velx) + num_x;
		  nump = num*peaks_to_save;
		  
		  return (peakval[nump+peak]);
}
//***************************************************************//
//**    PKGetVectorX - Get X-component of vector (x,y)         *//
//***************************************************************//
float
PKGetVectorX(int num_x,int num_y)	
{	
	return(PKGetNthVectorX(num_x, num_y, 1));
}
//***************************************************************//
//**    PKGetVectorY - Get Y-component of vector (x,y)                  *//
//***************************************************************//
float
PKGetVectorY(int num_x,int num_y)	
{	
	return(PKGetNthVectorY(num_x, num_y, 1));
}
//***************************************************************//
//**    PKGetVectorZ - Get Z-component of vector (x,y)                  *//
//***************************************************************//
float
PKGetVectorZ(int num_x,int num_y)	
{	
	return(PKGetNthVectorZ(num_x, num_y, 1));
}
//***************************************************************//
//**    PKGetVector - Get vector (x,y)         *//
//***************************************************************//
Vector
PKGetVector(int num_x,int num_y)	
{	
	Vector myvec={0.,0.,0.,0.};
	
	myvec.x = PKGetNthVectorX(num_x, num_y, 1);
	myvec.y = PKGetNthVectorY(num_x, num_y, 1);
	myvec.z = PKGetNthVectorZ(num_x, num_y, 1);
	return(myvec);
}
//***************************************************************//
//**    PKSetNthVectorX - Set X-component of the specified peak (x,y)         *//
//***************************************************************//
int
PKSetNthVectorX(int num_x,int num_y, int peak, float value)	
{	
	int		num, nump;
	
		  if(no_of_velx == 0) {
			  beep();
			  pprintf("Error %d: Must have valid PIV image first.\n", OMA_NOEXIST);
			  return OMA_NOEXIST;
		  }
		  
		  //If we have a range problem, just leave
		  if ( (num_x >no_of_velx) || (num_y >no_of_vely) || (num_x < 0) || (num_y < 0) ||(peak <1) || (peak >= peaks_to_save) ) return OMA_RANGE;
		  
		  // Find position in xpeaks/ypeaks array
		  num = (num_y * no_of_velx) + num_x;
		  nump = num*peaks_to_save;
		  xpeaks[nump+peak] = xpeaks[nump] + value;
		  return OMA_OK;
}
//***************************************************************//
//**    PKSetNthVectorY - Set Y-component of the specified peak (x,y)                  *//
//***************************************************************//
int
PKSetNthVectorY(int num_x,int num_y, int peak, float value)	
{	
	int		num, nump;
	
		  if(no_of_velx == 0) {
			  beep();
			  pprintf("Error %d: Must have valid PIV image first.\n", OMA_NOEXIST);
			  return OMA_NOEXIST;
		  }
		  
		  //If we have a range problem, just leave
		  if ( (num_x >no_of_velx) || (num_y >no_of_vely) || (num_x < 0) || (num_y < 0) ||(peak <1) || (peak >= peaks_to_save) ) 
			  return OMA_RANGE;
		  
		  // Find position in xpeaks/ypeaks array
		  num = (num_y * no_of_velx) + num_x;
		  nump = num*peaks_to_save;
		  ypeaks[nump+peak] = ypeaks[nump] + value;
		  return OMA_OK;
}
//***************************************************************//
//**    PKSetNthVectorZ - Set Z-component of the specified peak (x,y)         *//
//***************************************************************//
int
PKSetNthVectorZ(int num_x,int num_y, int peak, float value)	
{	
	int		num, nump;
	
		  if(no_of_velx == 0) {
			  beep();
			  pprintf("Error %d: Must have valid PIV image first.\n", OMA_NOEXIST);
			  return OMA_NOEXIST;
		  }
		  
		  //If we have a range problem, just leave
		  if ( (num_x >no_of_velx) || (num_y >no_of_vely) || (num_x < 0) || (num_y < 0) ||(peak <1) || (peak >= peaks_to_save) ) 
			  return OMA_RANGE;
		  
		  // Find position in xpeaks/ypeaks array
		  num = (num_y * no_of_velx) + num_x;
		  nump = num*peaks_to_save;
		  //zpeaks[nump+peak] = zpeaks[nump] + value;  // One DAY PERHAPS
		  return OMA_OK;
}

//***************************************************************//
//**    PKSetNthPeak - Set strength of the nth specified peak (x,y)                  *//
//***************************************************************//
int
PKSetNthPeak(int num_x,int num_y, int peak, float peakstr)	
{	
	int		num, nump;
	
	// Check we have a PIV file
	if(no_of_velx == 0) {
		beep();
		pprintf("Error %d: Must have valid PIV image first.\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	// Force parameters into appropriate range
	if (num_x >no_of_velx) num_x = no_of_velx;
	if (num_y >no_of_vely) num_y = no_of_vely;
	if (num_x < 0) num_x = 0;
	if (num_y < 0) num_y = 0;
	if ((peak <1)||(peak >= peaks_to_save)) peak = 1;
		  
	// Find position in xpeaks/ypeaks array
	num = (num_y * no_of_velx) + num_x;
	nump = num*peaks_to_save;
		  
	peakval[nump+peak] = peakstr;
	return OMA_OK;
}
//***************************************************************//
//**    PKSetVectorX - Set X-component of vector (x,y)         *//
//***************************************************************//
int
PKSetVectorX(int num_x,int num_y, float value)	
{	
	return(PKSetNthVectorX(num_x, num_y, 1, value));
}
//***************************************************************//
//**    PKSetVectorY - Set Y-component of vector (x,y)                  *//
//***************************************************************//
int
PKSetVectorY(int num_x,int num_y, float value)	
{	
	return(PKSetNthVectorY(num_x, num_y, 1, value));
}
//***************************************************************//
//**    PKSetVectorZ - Set Z-component of vector (x,y)                  *//
//***************************************************************//
int
PKSetVectorZ(int num_x,int num_y, float value)	
{	
	return(PKSetNthVectorZ(num_x, num_y, 1, value));
}
//***************************************************************//
//**    PKSetVector - Set vector (x,y)         *//
//***************************************************************//
int
PKSetVector(int num_x,int num_y, Vector myvec)	
{		
	int XState, YState, ZState;
	XState = PKSetNthVectorX(num_x, num_y, 1, myvec.x) ;
	YState = PKSetNthVectorY(num_x, num_y, 1, myvec.y) ;
	ZState = PKSetNthVectorZ(num_x, num_y, 1, myvec.z) ;
	if (XState == OMA_OK && YState == OMA_OK && ZState==OMA_OK) 
		return OMA_OK;
	else
		return OMA_MISC;
}
//***************************************************************//
//**    PKGetPixel - Returns Pixels value of (x,y) with bounds checking *//
//***************************************************************//
DATAWORD PKGetPixel(int xpos, int ypos)
{
	DATAWORD *pt;
	
	int index;
	
	if (datpt == 0) return OMA_NOEXIST;
	
	if (xpos >= header[NCHAN]) xpos = header[NCHAN]-1;
	if (ypos >= header[NTRAK]) ypos = header[NTRAK]-1;
	if (xpos < 0) xpos = 0;
	if (ypos < 0) ypos = 0;
	
	index = xpos + ypos * header[NCHAN];
	if (index >= header[NCHAN]*header[NTRAK]) return(0);	// check for illegal value passed
	pt = datpt + index + doffset;
	return(*pt);
}
//***************************************************************//
//**    PKGetTempPixel - Returns Pixels value of (x,y) with bounds checking *//
//***************************************************************//
DATAWORD PKGetTempPixel(int xpos, int ypos, int n)
{
	DATAWORD *pt;
	
	int index;
	int t_width, t_height;
	// Check Ranges of Temp Images
	if(n < 0 || n >= NUM_TEMP_IMAGES){
		beep();
		pprintf("Error %d: Illegal temporary image number.\n", OMA_RANGE);
		return OMA_RANGE;
	}
	
	//Check that there is anything there
	if(temp_dat[n] == 0){
		beep();
		pprintf("Error %d: Temporary Image does not exist\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	} 	
	
	// Find properties of the Temporary image
	t_width= (int) *(temp_header[n]+NCHAN);
	t_height= (int) *(temp_header[n]+NTRAK);	

	
	if (xpos >= t_width) xpos = t_width-1;
	if (ypos >= t_height) ypos = t_height-1;
	if (xpos < 0) xpos = 0;
	if (ypos < 0) ypos = 0;
	
	index = xpos + ypos * t_width;
	if (index >= t_width*t_height) return(0);	// check for illegal value passed
	pt = temp_dat[n] + index + doffset;
	return(*pt);
}
//***************************************************************//
//**    PKInterpPixel - Find interpolated pixel value in buffer by fractional (float) values *//
//***************************************************************//
DATAWORD PKInterpPixel(float x, float y)
{
	float xfrac, yfrac;
	
	DATAWORD    cent,east,south, soueast;
	DATAWORD value;
	int xint, yint;
	xint = (int) x;
	yint = (int)y;
	xfrac = x-(float)xint;
	yfrac = y-(float)yint;
	
	cent = PKGetPixel(xint, yint);
	east = PKGetPixel(xint+1, yint);
	south = PKGetPixel(xint, yint+1);
	soueast = PKGetPixel(xint+1, yint+1);
	
	value = xfrac*( east - cent ) + yfrac* (south - cent) + cent + (cent + soueast - south - east) * xfrac * yfrac; 
	return (value);
}	

//***************************************************************//
//**    PKSetPixel - Set Pixels value of (x,y) with bounds checking
//**		xpos       - Pixel x position                                     *//
//**		ypos       - Pixel y position                                     *//
//**		value      - value (DATAWORD) for pixel                  *//
//***************************************************************//
int PKSetPixel( int xpos, int ypos, DATAWORD value)
{
	int index;
	
	// Check pixel position is inbounds
	if (xpos >= header[NCHAN]) xpos = header[NCHAN]-1;
	if (ypos >= header[NTRAK]) ypos = header[NTRAK]-1;
	if (xpos < 0) xpos = 0;
	if (ypos < 0) ypos = 0;
	
	index = xpos + ypos * header[NCHAN];
	// check for illegal value passed
	if (index >= header[NCHAN]*header[NTRAK]){
		return OMA_RANGE;
	}
	*(datpt + index + doffset) = value;
	return OMA_OK;
}
#define STARTSDK	1
//***************************************************************//
//**    SDK_Image_SetPixel - Set Pixels value of (x,y) with bounds checking
//**		Parameters                                                             *//
//**		ImPointer    - Pointer to image structure                *//
//**		xpos       - Pixel x position                                     *//
//**		ypos       - Pixel y position                                     *//
//**		value      - value (DATAWORD) for pixel                  *//
//***************************************************************//
int SDK_Image_SetPixel(Image *ImPointer, short xpos, short ypos, DATAWORD value)
{
	DATAWORD *pt;
	DATAWORD *Pointer;
	int index;
	short width, height;
	
	// Check data structure is non-zero
	if (ImPointer == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	Pointer = ImPointer->ptr;
	width = ImPointer->width;
	height = ImPointer->height;
	
	// If pixel is out of bounds then DO NOT SetPixel
	if (xpos >= width) return OMA_RANGE;
	if (ypos >= height) return OMA_RANGE;
	if (xpos < 0) return OMA_RANGE;
	if (ypos < 0) return OMA_RANGE;
	
	index = xpos + ypos * width;
	
	pt = Pointer + index;
	*pt = value;
	return OMA_OK;
}
//***************************************************************//
//**    SDK_Image_GetPixel - Get Pixels value of (x,y) with from Image  *//
//**		ImPointer    - Pointer to image structure                *//
//**		xpos       - Pixel x position                                     *//
//**		ypos       - Pixel y position                                     *//
//**		value      - value (DATAWORD) for pixel                  *//
//***************************************************************//
DATAWORD
SDK_Image_GetPixel(Image *imptr, short xpos, short ypos)
{
	DATAWORD *pt;
	DATAWORD *Pointer;
	int index;
	short width, height;
	
	// Check data structure is non-zero
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	Pointer = imptr->ptr;
	width = imptr->width;
	height = imptr->height;
	
	// Check pixel position is inbounds
	if (xpos >= width) xpos = width-1;
	if (ypos >= height) ypos = height-1;
	if (xpos < 0) xpos = 0;
	if (ypos < 0) ypos = 0;
	
	index = xpos + ypos * width;
	
	pt = Pointer + index;
	return(*pt);
}
//***************************************************************//
//**    SDK_Image_InterpPixel - Find interpolated pixel value from  **//
//**                                           fractional (float) values in an Image structure *//
//***************************************************************//
DATAWORD SDK_Image_InterpPixel(Image *imptr, float x, float y)
{
	float xfrac, yfrac;
	
	DATAWORD    cent,east,south, soueast;
	DATAWORD value;
	int xint, yint;
	xint = (int) x;
	yint = (int)y;
	xfrac = x-(float)xint;
	yfrac = y-(float)yint;
	
	cent = SDK_Image_GetPixel(imptr, xint, yint);
	east = SDK_Image_GetPixel(imptr, xint+1, yint);
	south = SDK_Image_GetPixel(imptr, xint, yint+1);
	soueast = SDK_Image_GetPixel(imptr, xint+1, yint+1);
	
	value = xfrac*( east - cent ) + yfrac* (south - cent) + cent + (cent + soueast - south - east) * xfrac * yfrac; 
	return (value);
}	
//***************************************************************//
//**    SDK_Image_GetMaxPixelX - Get x position value ofmin/max pixel with from Image  *//
//**		ImPointer    - Pointer to image structure                *//
//***************************************************************//
int
SDK_Image_GetMaxPixelX(Image *imptr)
{
	DATAWORD *Pointer;
	int locmax=0;
	int nc, nt;
	short width, height;
	DATAWORD value, max;
	
	// Check data structure is non-zero
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	Pointer = imptr->ptr;
	width = imptr->width;
	height = imptr->height;
	max = -MAXDATAVALUE;
	for (nt=0;nt<height;nt++){
		for (nc=0;nc<width;nc++){
			value = SDK_Image_GetPixel(imptr, nc, nt);
			if( value >= max){
				max = value;
				locmax = nc;
			} 
		}
	}
	return(locmax);
}
//***************************************************************//
//**    SDK_Image_GetMaxPixelY - Get Y position value ofmin/max pixel with from Image  *//
//**		ImPointer    - Pointer to image structure                *//
//***************************************************************//
int
SDK_Image_GetMaxPixelY(Image *imptr)
{
	DATAWORD *Pointer;
	int locmax=0;
	int nc, nt;
	short width, height;
	DATAWORD value, max;
	
	// Check data structure is non-zero
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	Pointer = imptr->ptr;
	width = imptr->width;
	height = imptr->height;
	max = -MAXDATAVALUE;
	for (nt=0;nt<height;nt++){
		for (nc=0;nc<width;nc++){
			value = SDK_Image_GetPixel(imptr, nc, nt);
			if( value > max){
				max = value;
				locmax = nt;
			} 
		}
	}
	return(locmax);
}
//***************************************************************//
//**    SDK_Image_GetMinPixelX - Get X position value ofmin/max pixel with from Image  *//
//**		ImPointer    - Pointer to image structure                *//
//***************************************************************//
int
SDK_Image_GetMinPixelX(Image *imptr)
{
	DATAWORD *Pointer;
	int locmin=0;
	int nc, nt;
	short width, height;
	DATAWORD value, min;
	
	// Check data structure is non-zero
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	Pointer = imptr->ptr;
	width = imptr->width;
	height = imptr->height;
	min = MAXDATAVALUE;
	for (nt=0;nt<height;nt++){
		for (nc=0;nc<width;nc++){
			value = SDK_Image_GetPixel(imptr, nc, nt);
			if( value < min){
				min = value;
				locmin = nc;
			} 
		}
	}
	return(locmin);
}
//***************************************************************//
//**    SDK_Image_GetMinPixelY - Get Y position value ofmin/max pixel with from Image  *//
//**		ImPointer    - Pointer to image structure                *//
//***************************************************************//
int
SDK_Image_GetMinPixelY(Image *imptr)
{
	DATAWORD *Pointer;
	int locmin=0;
	int nc, nt;
	short width, height;
	DATAWORD value, min;
	
	// Check data structure is non-zero
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	Pointer = imptr->ptr;
	width = imptr->width;
	height = imptr->height;
	min = MAXDATAVALUE;
	for (nt=0;nt<height;nt++){
		for (nc=0;nc<width;nc++){
			value = SDK_Image_GetPixel(imptr, nc, nt);
			if( value < min){
				min = value;
				locmin = nt;
			} 
		}
	}
	return(locmin);
}
//***************************************************************//
//**    SDK_Image_GetMaxPixel - Get value ofmin/max pixel with from Image  *//
//**		ImPointer    - Pointer to image structure                *//
//***************************************************************//
DATAWORD
SDK_Image_GetMaxPixel(Image *imptr)
{
	int xmax, ymax;
	DATAWORD value;
	
	// Check data structure is non-zero
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	xmax = SDK_Image_GetMaxPixelX(imptr);
	ymax = SDK_Image_GetMaxPixelY(imptr);
	value = SDK_Image_GetPixel(imptr, xmax, ymax);
	
	return(value);
}
//***************************************************************//
//**    SDK_Image_GetMinPixelX - Get X position value ofmin/max pixel with from Image  *//
//**		ImPointer    - Pointer to image structure                *//
//***************************************************************//
DATAWORD
SDK_Image_GetMinPixel(Image *imptr)
{
	int xmin, ymin;
	DATAWORD value;
	
	// Check data structure is non-zero
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	xmin = SDK_Image_GetMinPixelX(imptr);
	ymin = SDK_Image_GetMinPixelY(imptr);
	value = SDK_Image_GetPixel(imptr, xmin, ymin);
	
	return(value);
}

//***************************************************************//
//**    SDK_Image_Norm - Normalise the image structure so values range 0 -> 1*//
//***************************************************************//
int
SDK_Image_Norm(Image *imptr)
{
	DATAWORD our_min, our_max;
	short width, height;
	int nc, nt;
	
	// Check data structure is non-zero
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	our_min = SDK_Image_GetMinPixel(imptr);
	our_max = SDK_Image_GetMaxPixel(imptr);
	
	width = imptr->width;
	height = imptr->height;
	
	for (nt=0;nt<height;nt++){
		for (nc=0;nc<width;nc++){
			DATAWORD pixval;
			pixval = SDK_Image_GetPixel(imptr, nc, nt);
			pixval = (pixval - our_min)/ (our_max -our_min);
			SDK_Image_SetPixel(imptr, nc, nt, pixval);
		}
	}
	return OMA_OK;
}

//***************************************************************//
//**    SDK_Image_GetFractionValue - return the value for which 'fraction' [%] of pixels are below *//
//***************************************************************//
DATAWORD
SDK_Image_GetFractionValue(Image *imptr, float fraction)
{
	short nc, nt;
	short  width, height;
	DATAWORD min, max;
	DATAWORD value = 0;
	int   cumulative;
	int   binsize, bin;
	int h_array[HISTOSIZE] = {0};
	
	// Check data structure is non-zero
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	// Prepare the quantities we'll need later
	min = SDK_Image_GetMinPixel(imptr);
	max = SDK_Image_GetMaxPixel(imptr);
	width = imptr->width;
	height = imptr->height;
	
	// Build our copy of a histogram holding bins
	binsize = (max - min)/HISTOSIZE;
	if( binsize < 1) binsize = 1;
	for (nt=0;nt<height;nt++){
		for (nc=0;nc<width;nc++){
			bin = (SDK_Image_GetPixel(imptr, nc, nt) - min)/binsize;
			if (bin <0) bin = 0;
			if (bin > HISTOSIZE-1) bin = HISTOSIZE-1;
			h_array[bin]++;
		}
	}
	
	// Roll through the h_array keeping track of how far into population distribution we are..
	cumulative=0;
	for (nc = 0; nc<HISTOSIZE; nc++){
		cumulative += h_array[nc];
		if (cumulative >= fraction * width *height) {
			value = (DATAWORD) nc*binsize;
			break;	
		}
	}
	return (value);
}
//***************************************************************//
//**    SDK_Image_Clip - Clip value ABOVE a limit*//
//***************************************************************//
int
SDK_Image_Clip(Image *imptr, DATAWORD limit)
{	
	short width, height;
	short nc, nt;
	
	// Check data structure is non-zero
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	width = imptr->width;
	height = imptr->height;
	
	for (nt=0;nt<height;nt++){
		for (nc=0;nc<width;nc++){
			if (SDK_Image_GetPixel(imptr, nc, nt)>limit)
				SDK_Image_SetPixel(imptr, nc, nt, limit);
		}
	}
	return OMA_OK;
}
//***************************************************************//
//**    SDK_Image_Clop - Clip value BELOW a limit*//
//***************************************************************//
int
SDK_Image_Clop(Image *imptr, DATAWORD limit)
{	
	short width, height;
	short nc, nt;
	
	// Check data structure is non-zero
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	width = imptr->width;
	height = imptr->height;
	
	for (nt=0;nt<height;nt++){
		for (nc=0;nc<width;nc++){
			if (SDK_Image_GetPixel(imptr, nc, nt)<limit)
				SDK_Image_SetPixel(imptr, nc, nt, limit);
		}
	}
	return OMA_OK;
}
//***************************************************************//
//**    SDK_Image_Create - Create a new image structure with pointer *//
//***************************************************************//
int
SDK_Image_Create (Image *imptr, short width, short height)	
{
	int size;
	int nc,nt;
	DATAWORD *ptr, *tptr;
	
	// Determine memory size and Handle memory
	size = (width * height);
	ptr = (DATAWORD*) calloc(size,DATABYTES);
	if (ptr == 0){
		return OMA_MEMORY;
	} else {
		imptr->height = height;
		imptr->width = width;
		tptr = ptr;
		// Initialise new structure to zeros.
		for(nt=0;nt<height;nt++){
			for(nc=0;nc<width;nc++){
				*tptr++ = 0;
			}
		}
		imptr->ptr = ptr;
		return OMA_OK;
	}
}
//***************************************************************//
//**    SDK_Image_CopyBuffer - Copy image in OMA image buffer to Image data structure *//
//***************************************************************//
int
SDK_Image_CopyBuffer (Image *imptr)	
{
	int size;
	int nc,nt;
	DATAWORD *ptr;
	
	// Determine memory size and Handle memory
	size = header[NTRAK]*header[NCHAN];
	ptr = (DATAWORD*) calloc(size,DATABYTES);
	if (ptr == 0){
		return OMA_MEMORY;
	} else {
		imptr->height = header[NTRAK];
		imptr->width = header[NCHAN];
		imptr->ptr = ptr;
		for(nt=0;nt<header[NTRAK];nt++){
			for(nc=0;nc<header[NCHAN];nc++){
				SDK_Image_SetPixel(imptr, nc, nt, PKGetPixel(nc,nt));
			}
		}
		return OMA_OK;
	}
}
//***************************************************************//
//**    SDK_Image_CopyImage - Copy one Image data structure to another *//
//***************************************************************//
int
SDK_Image_CopyImage (Image *imptr, Image *destptr)	
{
	short nc,nt;
	int width, height, size;
	DATAWORD *ptr;
	
	// Check data structure is non-zero
	if ((imptr == 0)||(destptr ==0)) {
		beep();
		pprintf("Error %d: Pointer to SRC & DST images must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	// Determine memory size and Handle memory
	width = imptr->width;
	height = imptr->height;
	size = height * width;
	
	ptr = (DATAWORD*) calloc(size,DATABYTES);
	if (ptr == 0){
		return OMA_MEMORY;
	} else {
		destptr->height = height;
		destptr->width = width;
		destptr->ptr = ptr;
		for(nt=0;nt<height;nt++){
			for(nc=0;nc<width;nc++){
				SDK_Image_SetPixel(destptr, nc, nt, SDK_Image_GetPixel(imptr, nc, nt));
			}
		}
	}
	return OMA_OK;
}

//***************************************************************//
//**    SDK_Image_CopyTemp - Copy image in OMA image buffer to Image data structure *//
//***************************************************************//
int
SDK_Image_CopyTemp (Image *imptr, int n)	
{
	int size;
	int nc,nt;
	DATAWORD *ptr, *tptr;
	int t_width, t_height;
	
	// Just check that the tempno is OK
	if(n < 0 || n >= NUM_TEMP_IMAGES){
		beep();
		pprintf("Error %d: Illegal temporary image number.\n", OMA_RANGE);
		return OMA_RANGE;
	}
	// Also check for existence
	if(temp_dat[n] == 0){
		beep();
		pprintf("Error %d: Temporary image %d not defined.\n", OMA_NOEXIST, n);
		return OMA_NOEXIST;
	}
	
	// Determine memory size and Handle memory
	t_width= *(temp_header[n]+NCHAN);
	t_height= *(temp_header[n]+NTRAK);	
	
	size = t_width * t_height;
	ptr = (DATAWORD*) calloc(size,DATABYTES);
	if (ptr == 0){
		return OMA_MEMORY;
	} else {
		imptr->height = t_height;
		imptr->width = t_width;
		tptr = ptr;
		for(nt=0;nt<t_height;nt++){
			for(nc=0;nc<t_width;nc++){
				*tptr++ = PKGetTempPixel(nc,nt,n);
			}
		}
		imptr->ptr = ptr;
		return OMA_OK;
	}
}




//***************************************************************//
//**    SDK_Image_CopySubregion - Copy image in OMA image buffer to Image data structure within bounds*//
//***************************************************************//
int	SDK_Image_CopySubregion (Image *imptr, int ulx, int uly, int lrx, int lry)
{
	int size;
	int nc,nt;
	DATAWORD *ptr;
	
	// Determine memory size and Handle memory
	size = (lrx - ulx) * (lry - uly);
	ptr = (DATAWORD*) calloc(size,DATABYTES);
	if (ptr == 0){
		return OMA_MEMORY;
	} else {
		imptr->height = (lry - uly);
		imptr->width = (lrx - ulx);
		imptr->ptr = ptr;
		for(nt=0;nt<(lry - uly);nt++){
			for(nc=0;nc<(lrx - ulx);nc++){
				SDK_Image_SetPixel(imptr, nc, nt, PKGetPixel(nc+ulx,nt+uly));
			}
		}
		return OMA_OK;
	}
	
}

//***************************************************************//
//**    SDK_Image_Overlay - Overlays data from source into dest at upper left point *//
//***************************************************************//
int	SDK_Image_Overlay (Image *srcptr, Image *destptr, int ulx, int uly)
{
	int nc, nt;
	int rangex, rangey;
	rangex = srcptr->width;
	rangey = srcptr->height;
	
	for(nt=0; nt<rangey; nt++){
		for(nc=0; nc<rangex; nc++){
			DATAWORD DW;
			DW = SDK_Image_GetPixel(srcptr, nc, nt);
			SDK_Image_SetPixel(destptr, nc+ulx, nt+uly, DW);
		}
	}
	return OMA_OK;
	
}


//***************************************************************//
//**    SDK_Image_Free - Release the memory of OMA image struct *//
//***************************************************************//
int
SDK_Image_Free (Image *imptr)	
{
	if (imptr!=0) {
		free(imptr->ptr);
	}
	return OMA_OK;
}
//***************************************************************//
//**    SDK_Image_2Buffer - Move image (in struct) into OMA image buffer *//
//***************************************************************//
int
SDK_Image_2Buffer (Image *imptr)	
{
	int n, size;
	int nc,i,nt;
	DATAWORD *ptr, *tptr;
	
    
	// Transfer image properties
	header[NCHAN] = imptr->width;
	header[NTRAK] = imptr->height;
	npts = header[NCHAN]*header[NTRAK];
	
	// Create an EXACT copy of OMA data buffer including Header+File512Rounding
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET);
	size = (size+511)/512*512;		/* make a bit bigger for file reads */
	ptr = (DATAWORD*) calloc(size,DATABYTES);
	if (ptr == 0){
		return OMA_MEMORY;
	} else {
		tptr = ptr;
		//Copy the CCD header to new image
		for(n=0; n<doffset; n++) 
			*(tptr++) = *(datpt+n);
		i=0;
		// Copy the data from Image structure
		for(nt=0;nt<header[NTRAK];nt++){
			for(nc=0;nc<header[NCHAN];nc++){
				*(ptr + i + doffset) = *(imptr->ptr + i);
				i++;
			}
		}
	}
	free(datpt);
	datpt = ptr;
	return OMA_OK;
}
//***************************************************************//
//**    SDK_Image_2Temp - Move image (in struct) into OMA TEMP buffer #n *//
//***************************************************************//
int
SDK_Image_2Temp (Image *imptr, int n)	
{
	int size;
	int nc,i,nt;
	TWOBYTE headcopy[HEADLEN/2];
	TWOBYTE trailcopy[TRAILEN/2];
	
	// Just check that the tempno is OK
	if(n < 0 || n >= NUM_TEMP_IMAGES){
		beep();
		pprintf("Error %d: Illegal temporary image number.\n", OMA_RANGE);
		return OMA_RANGE;
	}
	
	//size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET)*DATABYTES;
	size = (imptr->width * imptr->height+ MAXDOFFSET);

	// If the pointer to that temp buffer is already taken then free it
	if(temp_dat[n] != 0) free(temp_dat[n]);
	
	// Allocate memory for the data in buffer
	temp_dat[n] = (DATAWORD *)calloc(size, DATABYTES);
	if(temp_dat[n] == 0) {
		nomemory();
		return OMA_MEMORY;
	}
	
	// Allocate Memory for the Temp Data Header (keeping track of the image properties)
	if(temp_header[n] != 0) free(temp_header[n]);
	temp_header[n] = malloc(512);   //PK:Warning: This pointer should prbably be cast to a type
	if(temp_header[n] == 0) {
		nomemory();
		return OMA_MEMORY;
	}
	
	// Copy the data from Image structure 
	i = 0;
	for(nt=0;nt<imptr->height;nt++){
		for(nc=0;nc<imptr->width;nc++){
			*(temp_dat[n] + i + doffset) = *(imptr->ptr + i);
			i++;
		}
	}
	
	//Construct a 'fake' but appropriate header for the TEMP image structure
	for(nc=0; nc<TRAILEN/2; nc++) 
		*(trailcopy+nc) = *(trailer+nc);		
	for(nc = 0;nc<HEADLEN/2;nc++)
		*(headcopy+nc) = *(header+nc);

	trailcopy[SFACTR] = 1;
	headcopy[NMAX] = SDK_Image_GetMaxPixelX(imptr);
	headcopy[LMAX] = SDK_Image_GetMaxPixelY(imptr);
	headcopy[NMIN] = SDK_Image_GetMinPixelX(imptr);
	headcopy[LMIN] = SDK_Image_GetMinPixelY(imptr);
	headcopy[NCHAN] = imptr->width;
	headcopy[NTRAK] = imptr->height;

			  
	//Copy the FAKE header to temp_header
	for(nc=0; nc<HEADLEN/2; nc++) 
		*(temp_header[n]+nc) = *(headcopy+nc);	
	for(nc=0; nc<TRAILEN/2; nc++) 
		*(temp_header[n]+nc+(512-TRAILEN)/2) = *(trailcopy+nc);			
	
	return OMA_OK;
}

//***************************************************************//
//**    SDK_Image_Refresh - Refresh properties of current image             *//
//***************************************************************//
int
SDK_Image_Refresh ()	
{
	have_max = 0;
	maxx();
	return OMA_OK;
}


//***************************************************************************************//
//**    SDK_Image_XCorr - cross correlate and invert src1 & src2, result in dest             *//
//***************************************************************************************//
int
SDK_Image_XCorr (Image *src1ptr, Image *src2ptr, Image *destptr)	
{
	float *real_A,*imag_A;	// buffers for FFT
	float *real_B,*imag_B;	// buffers for FFT
	float  *work1, *work2;       // working storage buffers
	int     i, j, k;                       // Loop counters
	int     n2;	                    // temporary integer variable
	int     temp_float;              // temporary float variable
	int     fftwidth, fftheight;   // size of the source images
	
	// Check that source images exist
	if ((src1ptr == 0)||(src2ptr ==0)||(destptr ==0)) {
		beep();
		pprintf("Error %d: Pointer to SRC & DST images must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	// Check for equality
	fftwidth =  src1ptr->width;
	fftheight = src1ptr->height;
	if ( (fftwidth !=  src2ptr->width) || (fftheight != src2ptr->height) 
						|| (fftwidth !=  destptr->width)
		                                        || (fftheight != destptr->height)  ) {
		beep();
		pprintf("Error %d: The SRC & DST images need to be the same size\n", OMA_MISC);
		return OMA_MISC;
	}

	// Check for appropriate size
	if (!is_power_2(fftwidth) || !is_power_2(fftheight)){
		beep();
		pprintf("Error %d: We are still forcing 2^n requirement until the move to FFTW from FT2D\n", OMA_RANGE);
		return OMA_RANGE;
	}
	
	// Memory for float arrays for the FFT
	if ((real_A = (float *) malloc( fftwidth*fftheight * sizeof(float))) ==0) return OMA_MEMORY;
	if ((imag_A = (float *) malloc( fftwidth*fftheight * sizeof(float)))==0) return OMA_MEMORY;
	if ((real_B = (float *) malloc( fftwidth*fftheight * sizeof(float)))==0) return OMA_MEMORY;
	if ((imag_B = (float *) malloc( fftwidth*fftheight * sizeof(float)))==0) return OMA_MEMORY;
	if ((work1 = (float *) malloc( fftwidth * sizeof(float)))==0) return OMA_MEMORY;
	if ((work2 = (float *) malloc( fftwidth * sizeof(float)))==0) return OMA_MEMORY;
	
	// Transfer data to real and imaginary structures
	k = 0;
	for(i=0; i<fftwidth; i++) {
		for(j=0; j<fftheight; j++) {
			real_A[k]  = (float) SDK_Image_GetPixel(src1ptr, i, j);
			real_B[k]  = (float) SDK_Image_GetPixel(src2ptr, i, j);
			imag_A[k] = 0.0;
			imag_B[k] = 0.0;
			k++;
		}
	}
	
	// Do Fourier transform of each subregion and multiply
	n2 = fftsize;
	FT2D(real_A, imag_A, work1, work2, &n2, &n2);
	FT2D(real_B, imag_B, work1, work2, &n2, &n2);
	
	k = 0;
	for(i=0; i<fftwidth; i++) {
		for(j=0; j<fftheight; j++) {
			if( 1 & (i+j)){
				temp_float = -real_A[k]*real_B[k] - imag_A[k]*imag_B[k];
				imag_A[k] = -real_A[k]*imag_B[k] + real_B[k]*imag_A[k];
				real_A[k] = temp_float;
			} else {
				temp_float = real_A[k]*real_B[k] + imag_A[k]*imag_B[k];
				imag_A[k] = real_A[k]*imag_B[k] - real_B[k]*imag_A[k];
				real_A[k] = temp_float;
			}
			k++;
		}
		
	}
	
	// Inverse the transform (get cross-correlation image)
	n2 = -fftsize;
	FT2D(real_A, imag_A, work1, work2, &n2, &n2);
	
	// Transfer results (magnitude) back to destination Image structure
	k=0;
	for(i=0; i<fftwidth; i++) {
		for(j=0; j<fftheight; j++) {
			temp_float = sqrt(real_A[k]*real_A[k] + imag_A[k]*imag_A[k]);
			SDK_Image_SetPixel(destptr, i, j, (DATAWORD) temp_float);
			k++;
		}
	}
	
	// Free float Memory structures
	free(real_A);
	free(imag_A);
	free(real_B);
	free(imag_B);
	free(work1);
	free(work2);
	
	return OMA_OK;
}
//***************************************************************************************//
//**    SDK_Image_FFT - do the Fourier Transform of the Image                                     *//
//***************************************************************************************//
int
SDK_Image_FFT (Image *src1ptr, Image *destptr)	
{
	float *real_A,*imag_A;	// buffers for FFT
	float  *work1, *work2;       // working storage buffers
	int     i, j, k;                       // Loop counters
	int     n2;	                    // temporary integer variable
	int     temp_float;              // temporary float variable
	int     fftwidth, fftheight;   // size of the source images
	
	// Check that source images exist
	if ((src1ptr == 0)||(destptr ==0)) {
		beep();
		pprintf("Error %d: Pointer to SRC & DST images must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	// Check for equality
	fftwidth =  src1ptr->width;
	fftheight = src1ptr->height;

	// Check for appropriate size
	if (!is_power_2(fftwidth) || !is_power_2(fftheight)){
		beep();
		pprintf("Error %d: We are still forcing 2^n requirement until the move to FFTW from FT2D\n", OMA_RANGE);
		return OMA_RANGE;
	}
	
	// Memory for float arrays for the FFT
	if ((real_A = (float *) malloc( fftwidth*fftheight * sizeof(float))) ==0) return OMA_MEMORY;
	if ((imag_A = (float *) malloc( fftwidth*fftheight * sizeof(float)))==0) return OMA_MEMORY;
	if ((work1 = (float *) malloc( fftwidth * sizeof(float)))==0) return OMA_MEMORY;
	if ((work2 = (float *) malloc( fftwidth * sizeof(float)))==0) return OMA_MEMORY;
	
	// Transfer data to real and imaginary structures
	k = 0;
	for(i=0; i<fftwidth; i++) {
		for(j=0; j<fftheight; j++) {
			real_A[k]  = (float) SDK_Image_GetPixel(src1ptr, i, j);
			imag_A[k] = 0.0;
			k++;
		}
	}
	
	// Do Fourier transform of each subregion and multiply
	n2 = fftsize;
	FT2D(real_A, imag_A, work1, work2, &n2, &n2);

	
	// Transfer results (magnitude) back to destination Image structure
	k=0;
	for(i=0; i<fftwidth; i++) {
		for(j=0; j<fftheight; j++) {
			temp_float = sqrt(real_A[k]*real_A[k] + imag_A[k]*imag_A[k]);
			SDK_Image_SetPixel(destptr, i, j, (DATAWORD) temp_float);
			k++;
		}
	}
	
	// Free float Memory structures
	free(real_A);
	free(imag_A);
	free(work1);
	free(work2);
	
	return OMA_OK;
}

//***************************************************************************************//
//**    SDK_Image_3PtEstimatorX - Estimate sub-pixel peak location and return X position  *//
//***************************************************************************************//
float
SDK_Image_3PtEstimatorX (Image *imptr, int xpos, int ypos)	
{
	float   X;
	DATAWORD   top, bottom, left, right, centre;
	
	// Check that source images exist
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to source image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	centre  = SDK_Image_GetPixel(imptr, xpos, ypos);
	top       = SDK_Image_GetPixel(imptr, xpos, ypos-1);
	bottom = SDK_Image_GetPixel(imptr, xpos, ypos+1);
	left       = SDK_Image_GetPixel(imptr, xpos-1, ypos);
	right     = SDK_Image_GetPixel(imptr, xpos+1, ypos);
	
	switch (peakmethod) {
		case GAUSSIAN:
			X=(float) xpos + (float)((log(left) - log(right))/(2*log(left) - 4*log(centre) + 2*log(right)));
			break;
		case PARABOLA:
			X=(float)xpos +(float)((left-right)/(2*left - 4*centre +2*right));
			break;
		case CENTROID:
		default:
			X=(float)(( (xpos-1)*left+ xpos*centre +(xpos+1)*right  )/(left+centre+right));
			break;
	}
	return X;
}
//***************************************************************************************//
//**    SDK_Image_3PtEstimatorY - Estimate sub-pixel peak location and return Y position  *//
//***************************************************************************************//
float
SDK_Image_3PtEstimatorY (Image *imptr, int xpos, int ypos)	
{
	float   Y;
	DATAWORD   top, bottom, left, right, centre;
	
	// Check that source images exist
	if (imptr == 0) {
		beep();
		pprintf("Error %d: Pointer to source image must be non-NULL\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	centre  = SDK_Image_GetPixel(imptr, xpos, ypos);
	top       = SDK_Image_GetPixel(imptr, xpos, ypos-1);
	bottom = SDK_Image_GetPixel(imptr, xpos, ypos+1);
	left       = SDK_Image_GetPixel(imptr, xpos-1, ypos);
	right     = SDK_Image_GetPixel(imptr, xpos+1, ypos);
	
	switch (peakmethod) {
		case GAUSSIAN:
			Y=(float) ypos+(float) ((log(top)-log(bottom))/(2*log(top) -4*log(centre) +2*log(bottom)));
			break;
		case PARABOLA:
			Y=(float)ypos +(float)((top - bottom)/(2*top - 4*centre +2*bottom));
			break;
		case CENTROID:
		default:
			Y=(float)(( (ypos-1)*top +ypos*centre +(ypos+1)*bottom )/(top+centre+bottom));
			break;
	}
	return Y;
}
//***************************************************************************************//
//**    SDK_Image_3PtKill - Set the 3x3 box centered on (xpos, ypos) to the image minimum  *//
//***************************************************************************************//
int
SDK_Image_3PtKill (Image *imptr, int xpos, int ypos)
{
    int i, j;
    int step =1;
    DATAWORD min_in_Image;
    
    // Check that source images exist
    if (imptr == 0) {
        beep();
        pprintf("Error %d: Pointer to source image must be non-NULL\n", OMA_NOEXIST);
        return OMA_NOEXIST;
    }
    // First find the 'floor' value of the current Image
    min_in_Image = SDK_Image_GetMinPixel(imptr);
    
    // Do a loop that erases around the specified pixel
    for (i=-step; i<=step; i++){
        for(j=-step; j<=step; j++){
            // Bounds checking will be done in SetPixel
            SDK_Image_SetPixel(imptr, xpos+i, ypos+j, min_in_Image);
        }
    }
    
    return OMA_OK;
}
//***************************************************************************************//
//**    SDK_Image_DrawTriangle - pass three points of triangle and fill image within bounds  *//
//***************************************************************************************//
int
SDK_Image_DrawTriangle (Image *imptr, float P1x, float P1y, float P2x, float P2y, float P3x, float P3y)
{
    int i, j, k, l;
    float V1x, V1y; // will eventually hold TOP vertex
    float V2x, V2y; // will eventually hold MIDDLE vertex
    float V3x, V3y; // will eventually hold BOTTOM vertex
    int y1i, y2i, y3i;
    float tempX, tempY;
    
    int IsTopFlat=0;
    int IsBottomFlat = 0;
    
    // Check that source images exist
    if (imptr == 0) {
        beep();
        pprintf("Error %d: Pointer to source image must be non-NULL\n", OMA_NOEXIST);
        return OMA_NOEXIST;
    }
    
    // Now we want to sort the vertices based on y value
    V1x = P1x; V1y = P1y; V2x = P2x; V2y = P2y; V3x = P3x; V3y = P3y;
    if (V2y < V1y){ tempX= V1x; tempY = V1y; V1x = V2x; V1y = V2y; V2x = tempX; V2y = tempY;}
    if (V3y < V1y){ tempX= V1x; tempY = V1y; V1x = V3x; V1y = V3y; V3x = tempX; V3y = tempY;}
    if (V3y < V2y){ tempX= V2x; tempY = V2y; V2x = V3x; V2y = V3y; V3x = tempX; V3y = tempY;}
    
    // Now we find the integer value of these Y positions using ceil()
    y1i = ceil(V1y);
    y2i = ceil(V2y);
    y3i = ceil(V3y);
    
    // We need to examine a coupe of cases:
    // Case 1) y1i==y3i . The triangle has zero effectve hieght. We can just ignore it.
    if (y1i==y3i) { return OMA_OK;}
    
    // Case 2) y1i==y2i. The triangle has a flat top edge.
    if (y1i==y2i) { IsTopFlat=1;}
    
    // Case 3) y2i==y3i. The traingle has a flat bottom edge
    if (y2i==y3i) { IsBottomFlat=1;}

   // Draw top segment when it is appropriate
    if (!IsTopFlat) {
        for ( i=y1i; i<y2i;i++) {
            // Draw a horizontal line from
            // interp V1V2 to interp V1V3
            float t = (i-V1y)/(V3y-V1y);
            float S13 = V1x + t * (V3x-V1x);
            float t2 = (i-V1y)/(V2y-V1y);
            float S12 = V1x + t2 * (V2x-V1x);
            int start, end;
            start = ceil(MIN(S13, S12));
            end = ceil(MAX(S13, S12));
            if (!((start<0)&&(end<0))){
                for ( j=start;j<end; j++){
                    SDK_Image_SetPixel(imptr, j, i, SDK_Image_GetPixel(imptr, j, i) + 1);
                }
            }
        }
    }
    // Draw Bottom segment
    if (!IsBottomFlat){
        for ( k=y2i;k<y3i;k++){
            // Draw a horizontal line from
            // interp V2V3 to interp V1V3
            float t = (k-V1y)/(V3y-V1y);
            float S13 = V1x + t * (V3x-V1x);
            float t2 = (k-V2y)/(V3y-V2y);
            float S23 = V2x + t2 * (V3x-V2x);
            int start, end;
            start = ceil(MIN(S13, S23));
            end = ceil(MAX(S13, S23));
            if (!((start<0)&&(end<0))){
                for (l=start;l<end; l++){
                    SDK_Image_SetPixel(imptr, l, k, SDK_Image_GetPixel(imptr, j, i)+1);
                }
            }
        }
    }
    return OMA_OK;
}

//*********************************************************************//
//**    SDK_Vector_SetX - Set the value of 'X' in a passed Vector          *//
//*********************************************************************//
int
SDK_Vector_SetX (Vector *myVect, float Xval)	
{
	myVect->x = Xval;
	return OMA_OK;
}	
//*********************************************************************//
//**    SDK_Vector_SetY - Set the value of 'Y' in a passed Vector          *//
//*********************************************************************//
int
SDK_Vector_SetY (Vector *myVect, float Yval)	
{
	myVect->y = Yval;
	return OMA_OK;
}	
//*********************************************************************//
//**    SDK_Vector_SetZ - Set the value of 'Z' in a passed Vector          *//
//*********************************************************************//
int
SDK_Vector_SetZ (Vector *myVect, float Zval)	
{
	myVect->z = Zval;
	return OMA_OK;
}	
//*********************************************************************//
//**    SDK_Vector_SetPeak - Set the value of 'peak' in a passed Vector   *//
//*********************************************************************//
int
SDK_Vector_SetPeak (Vector *myVect, float peak)	
{
	myVect->peakval = peak;
	return OMA_OK;
}

//*********************************************************************//
//**    SDK_Vector_GetX - Return the value of 'X' in a passed Vector   *//
//*********************************************************************//
float
SDK_Vector_GetX (Vector *myVect)	
{
	return myVect->x;
}	
//*********************************************************************//
//**    SDK_Vector_GetY - Return the value of 'Y' in a passed Vector   *//
//*********************************************************************//
float
SDK_Vector_GetY (Vector *myVect)	
{
	return myVect->y;
}	
//*********************************************************************//
//**    SDK_Vector_GetZ - Return the value of 'Z' in a passed Vector   *//
//*********************************************************************//
float
SDK_Vector_GetZ (Vector *myVect)	
{
	return myVect->z;
}
//*********************************************************************//
//**    SDK_Vector_DotProduct - Return the dot product of V1.V2       *//
//*********************************************************************//
float
SDK_Vector_DotProduct (Vector *V1, Vector *V2)
{
    float retval;
    retval = (V1->x)*(V2->x) + (V1->y)*(V2->y)+ (V1->z)*(V2->z);
    return retval;
}	
//*********************************************************************//
//**    SDK_Vector_CrossProduct - Set Result Vector variable to V1 x V2   *//
//*********************************************************************//
int
SDK_Vector_CrossProduct (Vector *myVect, Vector *V1, Vector *V2)
{
    float u1, u2, u3;
    float v1, v2, v3;
    u1 = V1->x;
    u2 = V1->y;
    u3 = V1->z;
    v1 = V2->x;
    v2 = V2->y;
    v3 = V2->z;
    
    myVect->x = u2*v3 - u3*v2;
    myVect->y = u3*v1- u1*v3;
    myVect->z = u1*v2 - u1*v1;
    return OMA_OK;
}


//*********************************************************************//
//**    SDK_Vector_GetPeak - Return the value of 'peak' in a passed Vector   *//
//*********************************************************************//
float
SDK_Vector_GetPeak (Vector *myVect)	
{
	return myVect->peakval;
}

//***************************************************************//
//**    SDK_Vector_Magnitude - Pass it a ptr to a Vector and it returns  *//
//**                          the vector magnitude                                   *//
//***************************************************************//
float
SDK_Vector_Magnitude(Vector *myVect)
{
	float retval;
	float xcomp, ycomp, zcomp;
	
	xcomp = myVect->x;
	ycomp = myVect->y;
	zcomp = myVect->z;
	retval = sqrt(xcomp*xcomp + ycomp*ycomp + zcomp*zcomp);
	return(retval);
}
//***************************************************************//
//**    SDK_Vector_Orthogonal - are two vectors orthogonal? returns bool  *//
//***************************************************************//

int SDK_Vector_Orthogonal(Vector *myVect1, Vector *myVect2)
{
    if (SDK_Vector_DotProduct(myVect1, myVect2)==0) return 1;
    else return 0;
}

//***************************************************************//
//**    SDK_Field_GetVector - Get pointer to Vector (x,y) from Field  *//
//**		myfield    - Pointer to Field   structure                *//
//**		xpos       - Pixel x position                                     *//
//**		ypos       - Pixel y position                                     *//
//***************************************************************//
Vector*
SDK_Field_GetVector(Field *myfield, short xpos, short ypos)
{
	Vector *pt;
	Vector *Pointer;
	int index;
	short width, height;
	
	// Check data structure is non-zero
	if (myfield == 0) {
		beep();
		return NULL;
	}
	
	Pointer = myfield->ptr;
	width = myfield->width;
	height = myfield->height;
	
	// Check pixel position is inbounds
	if (xpos >= width) xpos = width-1;
	if (ypos >= height) ypos = height-1;
	if (xpos < 0) xpos = 0;
	if (ypos < 0) ypos = 0;
	
	index = xpos + ypos * width;
	
	pt = Pointer + index;
	return pt;
}

//***************************************************************//
//**    SDK_Field_SetVector - Set Vector at (x,y) in Field             *//
//**		myfield    - Pointer to Field structure		      *//
//**		xpos       - Pixel x position                                       *//
//**		ypos       - Pixel y position                                       *//
//**		input      - input vector for location                         *//
//***************************************************************//
int SDK_Field_SetVector(Field  *myfield, short xpos, short ypos, Vector *input)
{
	Vector *pt;
	
	pt = SDK_Field_GetVector(myfield, xpos, ypos);
	
	pt->x = input->x;
	pt->y = input->y;
	pt->z = input->z;
	pt->peakval = input->peakval;
	
	return OMA_OK;
}

//*********************************************************************//
//**    SDK_Field_Create - Return a pointer to a new PIV Field struct   *//
//*********************************************************************//
int
SDK_Field_Create (Field *myField, short width, short height)	
{
	int size;
	short nc,nt;
	Vector *vptr, *vtemp;
	
	
	// Determine memory size and Handle memory
	size = (width * height);
	vptr = (Vector*) calloc(size,sizeof(Vector));
	if (vptr == 0){
		return OMA_MEMORY;
	} else {
		myField->height = height;
		myField->width = width;
		myField->ptr = vptr;
		// Initialise new structure to zeros.
		for(nt=0;nt<height;nt++){
			for(nc=0;nc<width;nc++){
				// Get a pointer to each Vector in the Field
				if ((vtemp = SDK_Field_GetVector(myField, nc, nt))!=NULL){
					// Initialise the value of that Vector
					SDK_Vector_SetX(vtemp, 0.0000);
					SDK_Vector_SetY(vtemp, 0.0000);
					SDK_Vector_SetZ(vtemp, 0.0000);
					SDK_Vector_SetPeak(vtemp, 0.0000);
				}
			}
		 }
		return OMA_OK;
	}
}
//***************************************************************//
//**    SDK_Field_CopyBuffer - Copy PIV Buffer to a new PIV Field struct *//
//***************************************************************//
int
SDK_Field_CopyBuffer (Field *myField)	
{
	int size;
	int nc,nt;
	Vector *vptr, *vtemp;
	
	if(no_of_velx == 0) {
		beep();
		pprintf("Error %d: Must have valid PIV image first.\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	// Determine memory size and Handle memory
	size = (no_of_velx * no_of_vely);
	vptr = (Vector*) calloc(size,sizeof(Vector));
	if (vptr == 0){
		return OMA_MEMORY;
	} else {
		myField->height = no_of_vely;
		myField->width = no_of_velx;
		myField->ptr = vptr;
		// Initialise new structure to zeros.
		for(nt=0; nt<no_of_vely; nt++){
			for(nc=0; nc<no_of_velx; nc++){
				// Get a pointer to each Vector in the Field
				if ((vtemp = SDK_Field_GetVector(myField, nc, nt))!=NULL){
					// Initialise the value of that Vector
					SDK_Vector_SetX(vtemp, PKGetVectorX(nc, nt));
					SDK_Vector_SetY(vtemp, PKGetVectorY(nc, nt));
					SDK_Vector_SetZ(vtemp, PKGetVectorZ(nc, nt));
					SDK_Vector_SetPeak(vtemp, PKGetNthPeak(nc, nt, 1));
				}
			}
		}
		return OMA_OK;
	}
}
//***************************************************************//
//**    SDK_Field_CopyNthPeak - Copy Nth sub-peak PIV Buffer to a new PIV Field struct *//
//***************************************************************//
int
SDK_Field_CopyNthPeak (Field *myField, short n)	
{
	int size;
	int nc,nt;
	Vector *vptr, *vtemp;
	
	// Check existence of PIV image
	if(no_of_velx == 0) {
		beep();
		pprintf("Error %d: Must have valid PIV image first.\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	// Check the range of peak number
	if ((n<1) || (n>=peaks_to_save)) {
		beep();
		pprintf("Error %d: The peak number specified is out of bounds\n", OMA_RANGE);
		return OMA_RANGE;
	}
	
	// Determine memory size and Handle memory
	size = (no_of_velx * no_of_vely);
	vptr = (Vector*) calloc(size,sizeof(Vector));
	if (vptr == 0){
		return OMA_MEMORY;
	} else {
		myField->height = no_of_vely;
		myField->width = no_of_velx;
		myField->ptr = vptr;
		// Initialise new structure to zeros.
		for(nt=0; nt<no_of_vely; nt++){
			for(nc=0; nc<no_of_velx; nc++){
				// Get a pointer to each Vector in the Field
				if ((vtemp = SDK_Field_GetVector(myField, nc, nt))!=NULL){
					// Initialise the value of that Vector
					SDK_Vector_SetX(vtemp, PKGetNthVectorX(nc, nt, n));
					SDK_Vector_SetY(vtemp, PKGetNthVectorY(nc, nt, n));
					SDK_Vector_SetZ(vtemp, PKGetNthVectorZ(nc, nt, n));
					SDK_Vector_SetPeak(vtemp, PKGetNthPeak(nc, nt, n));
				}
			}
		}
		return OMA_OK;
	}
}

//***************************************************************//
//**    SDK_Field_Free - Release pointer to a new PIV Field struct *//
//***************************************************************//
int
SDK_Field_Free (Field *target)	
{
	if (target!=0) {
		free(target->ptr);
	}
	return OMA_OK;
}
//***************************************************************//
//**    SDK_Field_2Buffer - Move PIV Field struct into OMA PIV buffer *//
//***************************************************************//
int
SDK_Field_2Buffer (Field *myfield)	
{
	
	int n_x,n_y, n_k;
	int i;
	float *xpeaknew, *ypeaknew;
	DATAWORD *peaknew;
	Vector *vtemp;
	
	// Let's check if everything is in order....
	
	// Transfer image properties
	no_of_velx = myfield->width;
	no_of_vely = myfield->height;
	// Leave peaks_to_save alone, and copy the vector that many times into OMA
	
	// Create an EXACT copy of new OMA PIV buffers
	xpeaknew = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * sizeof(float));
	ypeaknew = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * sizeof(float));
	//zpeaknew = (float *)malloc(no_of_velx * no_of_vely * peaks_to_save * sizeof(float));
	peaknew = (DATAWORD*)malloc(no_of_velx * no_of_vely * peaks_to_save * sizeof(int));
	
	if (xpeaknew == 0 || ypeaknew==0||peaknew==0){
		beep();
		pprintf("Error %d: SDK_Field_2Buffer failed to get memory\n", OMA_MEMORY);
		return OMA_MEMORY;
	} else {
		// Copy the data from Field structure
		i=0;
		for(n_y=0; n_y<myfield->height; n_y++){
			for(n_x=0; n_x<myfield->width; n_x++){
				xpeaknew[i] = fftsize/2;
				ypeaknew[i]= fftsize/2;
				peaknew[i]=0;
				i++;
				for(n_k=1; n_k<peaks_to_save; n_k++){
					vtemp = SDK_Field_GetVector(myfield, n_x, n_y);
					xpeaknew[i] = fftsize/2 + vtemp->x;
					ypeaknew[i]= fftsize/2 + vtemp->y;
					peaknew[i]=peaks_to_save-n_k;
					i++;
				}
			}
		}
	}
	// Purge the old OMA PIV memory structures
	free(xpeaks);
	free(ypeaks);
	//free(zpeaks);
	free(peakval);
	
	// OMA structures inherit the newly created pointers
	xpeaks = xpeaknew;
	ypeaks = ypeaknew;
	//zpeaks = zpeaknew;
	peakval = peaknew;
	return OMA_OK;
}

//***************************************************************//
//**    SDK_Field_2NthPeak - Move PIV Field struct into Nth peak of OMA PIV buffer *//
//***************************************************************//
int
SDK_Field_2NthPeak (Field *myfield, short n)	
{
	
	int n_x,n_y;
	Vector *vptr;
	
	// Let's check if everything is in order....
	
	// First, check size.
	if ((no_of_velx != myfield->width)||(no_of_vely != myfield->height)) {
		beep();
		pprintf("Error %d: Field size and existing PIV buffer are not the same size\n", OMA_RANGE);
		return OMA_RANGE;
	}
	
	// Check range
	if ((n <1)||(n >= peaks_to_save)) {
		beep();
		pprintf("Error %d: Your peak number is out of range:%d\n", OMA_RANGE, peaks_to_save);
		return OMA_RANGE;
	}

	
	// set the component of PIV bufffer to the value in the Field struct.
	for(n_y=0; n_y<no_of_vely; n_y++){
		for(n_x=0; n_x<no_of_velx; n_x++){
			vptr = SDK_Field_GetVector(myfield, n_x, n_y);
			PKSetNthVectorX(n_x, n_y, n, vptr->x);
			PKSetNthVectorY(n_x, n_y, n, vptr->y);
			PKSetNthVectorZ(n_x, n_y, n, vptr->z);
			PKSetNthPeak(n_x, n_y, n, vptr->peakval );
		}
	}

	return OMA_OK;
}

#ifdef Mac

extern	int			gwnum;
extern	int			lgheight, lgwidth;
extern short                       orgx, orgy;
extern short                       swidth, sheight;
extern CGContextRef	lgContext;
extern CGContextRef	lgpdfContext;
extern OMA_Window	oma_wind[];
extern	RGBColor	RGBBlack,RGBWhite;
extern	float		xmin,xmax,ymin,ymax,xscalerange,scalex,yscalerange,scaley;

//***************************************************************//
//**    SDK_Window_Create - Initialise a new quartz window for drawing *//
//***************************************************************//
int
SDK_Window_Create (int width, int height, char* name)	
{	
	HIViewRef   contentView;
	CGRect dstRect;
	CGContextRef SetupPDFcontext(int);
	
	if (height < 10) height = 10;
	if (width < 10) width = 10;
	
	lgwidth = scalex = width;
	lgheight = scaley = height;
	
	chooseposn(lgwidth, lgheight, 1);
				
	if(!openwindow_q(lgwidth,lgheight,name,kWindowStandardFloatingAttributes)) 
		return OMA_MISC;
	
	lgpdfContext = SetupPDFcontext(gwnum);
	
	HIViewFindByID(HIViewGetRoot(oma_wind[gwnum].gwind), kHIViewWindowContentID, &contentView);
	HIViewGetBounds(contentView, &dstRect);
	
	QDBeginCGContext(GetWindowPort(oma_wind[gwnum].gwind), &lgContext);
	
	// Setup the default linewidth, font, etc
	CGContextSetLineWidth (lgContext, 1);
	CGContextSetRGBFillColor (lgContext, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor (lgContext, 0, 0, 0, 1);
	CGContextSelectFont (lgContext,"Helvetica",10,kCGEncodingMacRoman);
	CGContextSetTextDrawingMode(lgContext,kCGTextFillStroke);
	CGContextFillRect (lgContext,dstRect);
	CGContextBeginPath (lgContext);	
	
	// Setup the default linewidth, font, etc
	CGContextSetLineWidth (lgpdfContext, 1);
	CGContextSetRGBFillColor (lgpdfContext, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor (lgpdfContext, 0, 0, 0, 1);
	CGContextSelectFont (lgpdfContext,"Helvetica",10,kCGEncodingMacRoman);
	CGContextSetTextDrawingMode(lgpdfContext,kCGTextFillStroke);
	CGContextFillRect (lgpdfContext,dstRect);
	CGContextBeginPath (lgpdfContext);	
	
	//set_window_type(gwnum, QMIXEDTYPE);
	set_window_type(gwnum, QLINEGRAPH);
	
	// Sets some extern variables so that window placement and origin are correct
	scalx(0.0, (float) width);
	scaly(0.0, (float) height);

	return OMA_OK;
}

//***************************************************************//
//**    SDK_Window_CopyBuffer - Initialise a new quartz window  *//
//**                                                      for graphics from buffer *//
//***************************************************************//
int
SDK_Window_CopyBuffer (char* name)	
{	
	//HIViewID    mtViewID = { kMTViewSignature, 0 };
	HIViewRef   contentView;
	CGRect dstRect;
	CGContextRef SetupPDFcontext(int);
	int   width, height;
	
	width = header[NCHAN];
	height = header[NTRAK];
	
	chooseposn(lgwidth, lgheight, 1);
	
	oma_wind[gwnum].window_rgb_data = Get_rgb_from_image_buffer(1);
				
	if(!openwindow_q(lgwidth,lgheight,name,kWindowStandardFloatingAttributes)) 
		return OMA_MISC;
	
	lgpdfContext = SetupPDFcontext(gwnum);
	
	HIViewFindByID(HIViewGetRoot(oma_wind[gwnum].gwind), kHIViewWindowContentID, &contentView);
	HIViewGetBounds(contentView, &dstRect);
	
	QDBeginCGContext(GetWindowPort(oma_wind[gwnum].gwind), &lgContext);
	
	// Setup the default linewidth, font, etc
	CGContextSetLineWidth (lgContext, 1);
	CGContextSetRGBFillColor (lgContext, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor (lgContext, 0, 0, 0, 1);
	CGContextSelectFont (lgContext,"Helvetica",10,kCGEncodingMacRoman);
	CGContextSetTextDrawingMode(lgContext,kCGTextFillStroke);
	CGContextFillRect (lgContext,dstRect);
	CGContextBeginPath (lgContext);	
	
	// Setup the default linewidth, font, etc
	CGContextSetLineWidth (lgpdfContext, 1);
	CGContextSetRGBFillColor (lgpdfContext, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor (lgpdfContext, 0, 0, 0, 1);
	CGContextSelectFont (lgpdfContext,"Helvetica",10,kCGEncodingMacRoman);
	CGContextSetTextDrawingMode(lgpdfContext,kCGTextFillStroke);
	CGContextFillRect (lgpdfContext,dstRect);
	CGContextBeginPath (lgpdfContext);	
	
	//set_window_type(gwnum, QMIXEDTYPE);
	set_window_type(gwnum, QLINEGRAPH);
	scalx(0.0, (float) width);
	scaly(0.0, (float) height);
	
	return OMA_OK;
}

//***************************************************************//
//**    SDK_Window_Release - Release hold on current context *//
//***************************************************************//
int
SDK_Window_Release ()	
{	
	CGContextStrokePath( lgContext );
	CGContextFlush(lgContext);
	QDEndCGContext (GetWindowPort(oma_wind[gwnum].gwind), &lgContext);
	
	CGContextStrokePath( lgpdfContext );
	CGContextEndPage(lgpdfContext);
	CGContextRelease(lgpdfContext);
	
	
	gwnum++;
	orgx += swidth;
	
	return OMA_OK;
}

//***************************************************************//
//**    SDK_Window_SetRGB - Set the current colour *//
//***************************************************************//
int
SDK_Window_SetRGB (float red, float green, float blue)	
{
	
	CGContextSetRGBStrokeColor (lgContext, red, green, blue, 1);
	CGContextSetRGBStrokeColor (lgpdfContext, red, green, blue, 1);
	return OMA_OK;
}

//***************************************************************//
//**    SDK_Window_Update - Refresh/Flush current context *//
//***************************************************************//
int
SDK_Window_Update ()	
{
	CGContextFlush(lgContext);
	CGContextFlush(lgpdfContext);
	return OMA_OK;
}

//***************************************************************//
//**    SDK_Window_DrawLine - Draw a line between two points *//
//***************************************************************//
int
SDK_Window_DrawLine (float x0, float y0, float x1, float y1)	
{	
	//moveto(x0, y0); // pen down
	//lineto(x1,y1);  // link to appropraite point
	
	set(x0, y0);
	dvect(x1, y1);
	
	CGContextStrokePath(lgContext );
	CGContextStrokePath(lgpdfContext );

	return OMA_OK;
}

#endif

//***************************************************************//
//**    PKInterpVector - Find interpolated pixel value from fractional (float) values *//
//***************************************************************//
Vector PKInterpVector(float x, float y)
{
	float xfrac, yfrac;
	Vector new_vec;
	Vector    cent,east,south, soueast;
	int xint, yint;
	
	xint = (int) x;
	yint = (int) y;
	xfrac = x-(float)xint;
	yfrac = y-(float)yint;
	
	cent = PKGetVector(xint, yint);
	east = PKGetVector(xint+1, yint);
	south = PKGetVector(xint, yint+1);
	soueast = PKGetVector(xint+1, yint+1);
	
	new_vec.x = xfrac*( east.x - cent.x ) + yfrac* (south.x - cent.x) + cent.x + (cent.x + soueast.x - south.x - east.x) * xfrac * yfrac; ;
	new_vec.y = xfrac*( east.y - cent.y ) + yfrac* (south.y - cent.y) + cent.y + (cent.y + soueast.y - south.y - east.y) * xfrac * yfrac; ;
	new_vec.z = 0.0;
	new_vec.peakval = 1.614;
	return (new_vec);
}	

//***************************************************************//
//**    PKPixel2Vector - Return a Vector interpolated from the  *//
//**                           surrounding vectors in field  *//
//**                          based on a pixel position in underlying image *//
//***************************************************************//
Vector
PKPixel2Vector(int x,int y)
{
	int				width, height;
	int				ix, iy, px, py;
	float ui,vi,wi;
	Vector         retVec={0.,0.,0.,0.};
	float			xi, yi, xf, yf;
	float			c_u, c_v, e_u, e_v, s_u, s_v, se_u, se_v;
	float			c_w, e_w, s_w, se_w;
	
	// Check for a Valid PIV image
	if (no_of_velx==0)
	{
		beep();
		pprintf("Error %d: Must have a valid PIV image first\n", OMA_NOEXIST);
		return (retVec);
	}
	
	// Keep some important values....
	width=boxinc;
	height=boxinc;
	
	//Find ix and iy [the number of the Vector under the pixel (x,y)]
	xi=((float)x-(float)fftsize/2)/(float)width;
	yi=((float)y-(float)fftsize/2)/(float)height;
	
	if (xi<0.0) xi=0.0;
	if (yi<0.0) yi=0.0;
	
	// Now find location relative to vectors. 
	//This float (xi, yi) is equal to an interger component (ix, iy) plus a fractional float part (xf,yf)
	ix=(int)xi;
	iy=(int)yi;
	
	// The fractional part distance thru the FFT box
	xf= xi-(float)ix;
	yf= yi-(float)iy;
	
	//Check bounds at edges of field
	if  ((ix+1) >= no_of_velx )
	{
		px = ix;
	} else {
		px = ix + 1;
	}
	
	if  ((iy+1) >= no_of_vely )
	{
		py = iy;
	} else {
		py = iy + 1;
	}
	
	// Find the value of surrounding vectors:
	
	//Centre vector (Top-left corner)
	c_u = vx(ix, iy, 1);   if(isnan(c_u)) c_u=0.0;
	c_v = vy(ix, iy, 1);   if(isnan(c_v)) c_v=0.0;
	c_w = 0.0;
	//Eastern Vector
	e_u = vx(px,iy,1);   if(isnan(e_u)) e_u=0.0;
	e_v = vy(px,iy,1);   if(isnan(e_v)) e_v=0.0;
	e_w = 0.0;
	// Southern Vector
	s_u = vx(ix,py,1);   if(isnan(s_u)) s_u=0.0;
	s_v = vy(ix,py,1);   if(isnan(s_v)) s_v=0.0;
	s_w = 0.0;
	// South-Eastern vector
	se_u = vx(px,py,1);   if(isnan(se_u)) se_u=0.0;
	se_v = vy(px,py,1);   if(isnan(se_v)) se_v=0.0;
	se_w = 0.0;
	
	ui = xf*(c_u - e_u) + yf* (c_u -s_u) - c_u + (- c_u - se_u + s_u + e_u) * xf * yf; 
	vi = xf*(c_v - e_v) + yf* (c_v -s_v) - c_v + (- c_v - se_v + s_v + e_v) *  xf * yf; 
	wi = xf*(c_w - e_w) + yf* (c_w -s_w) - c_w + (- c_w - se_w + s_w + e_w) *  xf * yf; 
	
	retVec.x = ui;
	retVec.y = vi;
	retVec.z = wi;
	retVec.peakval = 1.614;
	return (retVec);
}


#define ENDSDK	1	