

/***********************************************************************************
 Created:	17:9:2002
 FileName: 	hdrloader.h
 Author:		Igor Kravtchenko
 
 Info:		Load HDR image and convert to a set of float32 RGB triplet.
 ************************************************************************************/

#define true 1
#define false 0

typedef struct  {
	int width, height;
	// each pixel takes 3 float32, each component can be of any value...
	float *cols;
} HDRLoaderResult;


int loadHDR(const char*, HDRLoaderResult*);


