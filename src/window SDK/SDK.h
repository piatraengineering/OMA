/*
 *  SDK.h
Header file for the OMA SOFTWARE DEVELOPERS KIT
 by Peter Kalt. April 2006
 *
 */
 
 #define GAUSSIAN	0
 #define PARABOLA	1
 #define CENTROID	2

// Function Prototypes for the PTK Programmers toolkit for OMA
int                PKCycleVector(), PKSetNthVectorX(), PKSetNthVectorY(), PKSetNthVectorZ();
int                PKSetVectorX(), PKSetVectorY(), PKSetVectorZ(), PKSetVector(), PKSetPixel(), PKSetNthPeak();
float             PKGetNthVectorX(), PKGetNthVectorY(),  PKGetNthVectorZ(), PKGetNthPeak(), PKGetVectorX();
float             PKGetVectorY(), PKGetVectorZ();
DATAWORD PKGetPixel(),  PKInterpPixel(),  PKGetTempPixel();

// Function Prototypes for VECTOR functions
int               SDK_Vector_SetX(), SDK_Vector_SetY(), SDK_Vector_SetZ(), SDK_Vector_SetPeak();
float            SDK_Vector_GetX(), SDK_Vector_GetY(), SDK_Vector_GetZ(), SDK_Vector_GetPeak();
float            SDK_Vector_Magnitude();

// Function Prototypes for IMAGE functions
int               SDK_Image_SetPixel(), SDK_Image_Create(), SDK_Image_CopyBuffer();
int               SDK_Image_Free(), SDK_Image_2Buffer(), SDK_Image_2Temp();
int               SDK_Image_Refresh(), SDK_Image_GetMaxPixelX(), SDK_Image_GetMaxPixelY();
int               SDK_Image_GetMinPixelX(), SDK_Image_GetMinPixelY(), SDK_Image_CopyTemp();
int               SDK_Image_Clip(), SDK_Image_Clop(), SDK_Image_XCorr(), SDK_Image_CopyImage();
int               SDK_Image_3PtKill();
float            SDK_Image_3PtEstimatorX(), SDK_Image_3PtEstimatorY();
DATAWORD SDK_Image_GetPixel(), SDK_Image_InterpPixel();
DATAWORD SDK_Image_GetMinPixel(), SDK_Image_GetMaxPixel();
DATAWORD SDK_Image_GetFractionValue();

// Function Prototypes for FIELD functions
int               SDK_Field_SetVector(), SDK_Field_GetVector(), SDK_Field_Create();
int               SDK_Field_CopyBuffer(), SDK_Field_Free(), SDK_Field_2Buffer(), SDK_Field_2NthPeak();

// Function Prototypes for WINDOW functions
int               SDK_Window_Create(), SDK_Window_Release(), SDK_Window_SetRGB();
int               SDK_Window_Update(), SDK_Window_DrawLine(), SDK_Window_CopyBuffer();

// Crap that should be re-written
Vector PKInterpVector();
Vector PKPixel2Vector();
Vector PKGetVector();