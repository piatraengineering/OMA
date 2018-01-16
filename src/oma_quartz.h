enum {
	kMTViewSignature	= 'MTVw',
	kCanvasBoundsParam	= 'Boun'
};
#define WINDOW_MARKER 0
#define WINDOW_LABEL 1
#define MAX_ANNOTATIONS 100

typedef struct {
    int type;
    int xpos;
    int ypos;
    char* label;
}WindowAnnotation;

typedef struct {

//	common bits
	WindowPtr 	gwind;
	int			windowtype;
	float   	opaque;
	RCMarker 	row_marker;
	RCMarker 	col_marker;
	
// QD only
//	Rect 		copyRect;
//	GWorldPtr	myGWorldPtrs; 	// offscreen maps
//	CTabHandle 	ourcmhandle;

//	quartz only	
	Ptr 		window_rgb_data;
	Ptr			view_data_ptr;
	int			width;
	int			height;
	CFDataRef	pdfData;			// for some linegraphics to allow copy of window contents as pdf
	CGDataConsumerRef consumer;		// for some linegraphics to allow copy of window contents as pdf
    int         num_annotations;        // the number of annotations
    WindowAnnotation    annotations[MAX_ANNOTATIONS];
    
	
} OMA_Window;

// A pointer to this per-view storage gets passed to the EventHandler as "userData".
struct MTViewData {
    HIViewRef		theView;
    CGImageRef		theImage;
    CGMutablePathRef    thePath;
};
typedef struct MTViewData   MTViewData;
