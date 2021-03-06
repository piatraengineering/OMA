/*
 *  custom.h
 *  oma
 *
 *  Created by Peter Kalt on 7/4/2006.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */


// externally defined DATA structures
extern DATAWORD *datpt;
extern Point substart,subend;	/* these two points define a data subarray */

extern DATAWORD* temp_dat[];
extern TWOBYTE*  temp_header[]; 

// externally defined HEADER structures
extern int 		doffset;
extern TWOBYTE 	header[];
extern TWOBYTE 	trailer[];
extern char             headcopy[];
extern char		comment[];
extern int     	          maxint;         /* the max count CC200 can store */
extern int		npts;		/* number of data points */

// externally defined IMP(lempentation) structures
extern char    cmnd[];  	/* the command buffer */
extern char    scratch[];
extern char    txt[];
extern DATAWORD mathbuf[];
extern int		have_max;
extern char             *fullname();

// externally defined PIV structures
extern int		*peakval;
extern int		fftsize, boxinc;
extern int		no_of_velx, no_of_vely, no_of_vel, peaks_to_save;
extern float	          *xpeaks, *ypeaks;
extern float	          vel(), vx(), vy();
extern int                sum_area, exclude_radius;
extern int                shiftx, shifty;
extern int                plotduringpiv  ;
extern int                preprocesspiv, peakmethod, plotarrowheads;
extern int                plot_dir_pref, labelplot, c_flag, autoclip;
extern float             pivscale ,pivdelay ,pivpixel ;
extern float             hiclip, loclip;


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

int omaprintf();

