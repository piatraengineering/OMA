/*
 *  SDK.h
 Header file for the OMA SOFTWARE DEVELOPERS KIT
 by Peter Kalt. April 2006
 *
 */

#define GAUSSIAN	0
#define PARABOLA	1
#define CENTROID	3

#define OMA_OK	0
#define OMA_MEMORY	-1			
//#define OMA_FILE             -2
#define OMA_RANGE       -4
#define OMA_NOEXIST       -5
#define OMA_MISC       -6
#define OMA_ARGS    -7

//Data types
typedef struct {
	float x;
	float y;
	float z;
	float peakval;
} Vector;

typedef struct {
	int width;
	int height;
	DATAWORD* ptr;
} Image;

typedef struct {
	int width;
	int height;
	Vector* ptr;
} Field;


// Function Prototypes for the PTK Programmers toolkit for OMA
int	PKCycleVector(int num_x,int num_y);
float	PKGetNthVectorX(int n_x,int n_y, int peak);	
float	PKGetNthVectorY(int num_x,int num_y, int peak);	
float	PKGetNthVectorZ(int num_x,int num_y, int peak);	
float	PKGetVectorX(int num_x,int num_y);	
float	PKGetVectorY(int num_x,int num_y);	
float	PKGetVectorZ(int num_x,int num_y);	
Vector	PKGetVector(int num_x,int num_y);	
int	PKSetNthVectorX(int num_x,int num_y, int peak, float value);	
int	PKSetNthVectorY(int num_x,int num_y, int peak, float value);	
int	PKSetNthVectorZ(int num_x,int num_y, int peak, float value);	
int	PKSetNthPeak(int num_x,int num_y, int peak, float peakstr);
int	PKSetVectorX(int num_x,int num_y, float value);	
int	PKSetVectorY(int num_x,int num_y, float value);	
int	PKSetVectorZ(int num_x,int num_y, float value);	
int	PKSetVector(int num_x,int num_y, Vector myvec);	
DATAWORD PKGetPixel(int xpos, int ypos);
DATAWORD PKGetTempPixel(int xpos, int ypos, int n);
DATAWORD PKInterpPixel(float x, float y);
int PKSetPixel( int xpos, int ypos, DATAWORD value);
float	PKGetNthPeak(int num_x,int num_y, int peak);

// Function Prototypes for VECTOR functions
int	SDK_Vector_SetX (Vector *myVect, float Xval);	
int	SDK_Vector_SetY (Vector *myVect, float Yval);	
int	SDK_Vector_SetZ (Vector *myVect, float Zval);	
int	SDK_Vector_SetPeak (Vector *myVect, float peak);	
int SDK_Vector_CrossProduct (Vector *myVect, Vector *V1, Vector *V2);
float SDK_Vector_DotProduct (Vector *V1, Vector *V2);
float	SDK_Vector_GetX (Vector *myVect);
float	SDK_Vector_GetY (Vector *myVect);	
float	SDK_Vector_GetZ (Vector *myVect);	
float	SDK_Vector_GetPeak (Vector *myVect);	
float	SDK_Vector_Magnitude(Vector *myVect);
int SDK_Vector_Orthogonal(Vector *myVect1, Vector *myVect2);

// Function Prototypes for IMAGE functions
int SDK_Image_SetPixel(Image *ImPointer, short xpos, short ypos, DATAWORD value);
DATAWORD	SDK_Image_GetPixel(Image *imptr, short xpos, short ypos);
DATAWORD SDK_Image_InterpPixel(Image *imptr, float x, float y);
int	SDK_Image_GetMaxPixelX(Image *imptr);
int	SDK_Image_GetMaxPixelY(Image *imptr);
int	SDK_Image_GetMinPixelX(Image *imptr);
int	SDK_Image_GetMinPixelY(Image *imptr);
DATAWORD	SDK_Image_GetMaxPixel(Image *imptr);
DATAWORD	SDK_Image_GetMinPixel(Image *imptr);
int SDK_Image_Norm(Image *imptr);
DATAWORD	SDK_Image_GetFractionValue(Image *imptr, float fraction);
int	SDK_Image_Clip(Image *imptr, DATAWORD limit);
int	SDK_Image_Clop(Image *imptr, DATAWORD limit);
int	SDK_Image_Create (Image *imptr, short width, short height);	
int	SDK_Image_CopyBuffer (Image *imptr);	
int	SDK_Image_CopyImage (Image *imptr, Image *destptr);	
int	SDK_Image_CopyTemp (Image *imptr, int n);	
int	SDK_Image_CopySubregion (Image *imptr, int ulx, int uly, int lrx, int lry);
int	SDK_Image_Overlay (Image *srcptr, Image *destptr, int ulx, int uly);
int	SDK_Image_Free (Image *imptr);	
int	SDK_Image_2Buffer (Image *imptr);	
int	SDK_Image_2Temp (Image *imptr, int n);	
int	SDK_Image_Refresh ();	
int	SDK_Image_XCorr (Image *src1ptr, Image *src2ptr, Image *destptr);	
int	SDK_Image_FFT (Image *src1ptr, Image *destptr);	
float	SDK_Image_3PtEstimatorX (Image *imptr, int xpos, int ypos);	
float	SDK_Image_3PtEstimatorY (Image *imptr, int xpos, int ypos);	
int	SDK_Image_3PtKill (Image *imptr, int xpos, int ypos);
int	SDK_Image_DrawTriangle (Image *imptr, float P1x, float P1y, float P2x, float P2y, float P3x, float P3y);

// Function Prototypes for FIELD functions
Vector*	SDK_Field_GetVector(Field *myfield, short xpos, short ypos);
int SDK_Field_SetVector(Field  *myfield, short xpos, short ypos, Vector *input);
int	SDK_Field_Create (Field *myField, short width, short height);	
int	SDK_Field_CopyBuffer (Field *myField);	
int	SDK_Field_CopyNthPeak (Field *myField, short n);	
int	SDK_Field_Free (Field *target);	
int	SDK_Field_2Buffer (Field *myfield);	
int	SDK_Field_2NthPeak (Field *myfield, short n);	

// Function Prototypes for WINDOW functions
int	SDK_Window_Create (int width, int height, char* name);	
int	SDK_Window_CopyBuffer (char* name);	
int	SDK_Window_Release ();	
int	SDK_Window_SetRGB (float red, float green, float blue);	
int	SDK_Window_Update ();	
int	SDK_Window_DrawLine (float x0, float y0, float x1, float y1);	

// Crap that should be re-written
Vector PKInterpVector(float x, float y);
Vector	PKPixel2Vector(int x,int y);
