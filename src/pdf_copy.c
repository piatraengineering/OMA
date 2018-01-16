
#include "impdefs.h"
#include "oma_quartz.h"

extern OMA_Window	oma_wind[];

#define kGenericRGBProfilePathStr "/System/Library/ColorSync/Profiles/Generic RGB Profile.icc" 

//-------------------------------------------------------------
// The API PasteboardCreate is new on 10.3 systems, but this application is targeted
// at Mac OS X 10.2 and later systems.  By defining in the targets build settings
// MACOSX_DEPLOYMENT_TARGET = 10.2 we are telling the build system to weak link
// API's that aren't avalible on the target system.  We can then
// check PasteboardCreate availibility before calling it by checking
// if PasteboardCreate != NULL as shown in GetPasteboard().
// See AvailabilityMacros.h for more information about weaklinking.
//
// If a paste board has alread been created return it. Otherwise check if
// the API PasteboardCreate can be weaklinked and if so try to create
// one to return.  Returns NULL if the pasteboard can't be created.
PasteboardRef GetPasteboard(void)
{
	static PasteboardRef sPasteboard = NULL;
	
	// If the pasteboard hasn't yet been created, and the API PasteboardCreate
	// is availible then attempt to create it.
	if (sPasteboard == NULL && PasteboardCreate != NULL)
	{
		PasteboardCreate( kPasteboardClipboard, &sPasteboard );
		if (sPasteboard == NULL)
		{
			fprintf(stderr, "PasteboardCreate failed - I wonder why?!\n");
		}
	}
	return sPasteboard;
}


CMProfileRef OpenGenericProfile(void)
{
    static CMProfileRef cachedRGBProfileRef = NULL;
    
    // we only create the profile reference once
    if (cachedRGBProfileRef == NULL)
    {
		OSStatus 			err;
		CMProfileLocation 	loc;
	
		loc.locType = cmPathBasedProfile;
		strcpy(loc.u.pathLoc.path, kGenericRGBProfilePathStr);
	
		err = CMOpenProfile(&cachedRGBProfileRef, &loc);
		
		if (err != noErr)
		{
			cachedRGBProfileRef = NULL;
			// log a message to the console
			fprintf(stderr, "couldn't open generic profile due to error %d\n", (int)err);
		}
    }

    if (cachedRGBProfileRef)
    {
		// clone the profile reference so that the caller has their own reference, not our cached one
		CMCloneProfileRef(cachedRGBProfileRef);   
    }

    return cachedRGBProfileRef;
}


//
//    Return the generic RGB color space. This is a 'get' function and the caller should
//    not release the returned value unless the caller retains it first. Usually callers
//    of this routine will immediately use the returned colorspace with CoreGraphics
//    so they typically do not need to retain it themselves.
    
//    This function creates the generic RGB color space once and hangs onto it so it can
//    return it whenever this function is called.


CGColorSpaceRef GetGenericRGBColorSpace(void)
{
    static CGColorSpaceRef genericRGBColorSpace = NULL;

	if (genericRGBColorSpace == NULL)
	{
		CMProfileRef genericRGBProfile = OpenGenericProfile();
	
		if (genericRGBProfile)
		{
			genericRGBColorSpace = CGColorSpaceCreateWithPlatformColorSpace(genericRGBProfile);
			if (genericRGBColorSpace == NULL)
				fprintf(stderr, "couldn't create the generic RGB color space\n");
			
			// we opened the profile so it is up to us to close it
			CMCloseProfile(genericRGBProfile); 
		}
	}
    return genericRGBColorSpace;
}

//--------------------------------------------------------------------------------------------------
/////////////////////////////// Support for Copy/Paste of PDF Data /////////////////////////////////
//--------------------------------------------------------------------------------------------------

// To create PDF data for the Pasteboard, we need to set up a CFDataConsumer that collects data in a CFMutableDataRef.
// Here are the two required callbacks:

static size_t MyCFDataPutBytes(void* info, const void* buffer, size_t count)
{
	CFDataAppendBytes((CFMutableDataRef)info, buffer, count);
	return count;
}

static void MyCFDataRelease(void* info)
{
	CFRelease((CFMutableDataRef)info);
}


// Draw page 1 of pdfData into the context
static void DrawPDFData(CGContextRef ctx, CFDataRef pdfData)
{
	CGDataProviderRef	provider;
	CGPDFDocumentRef	document;
	CGPDFPageRef		page;
	
	provider = CGDataProviderCreateWithData(NULL, CFDataGetBytePtr(pdfData), CFDataGetLength(pdfData), NULL);
	document = CGPDFDocumentCreateWithProvider(provider);
	
	page = CGPDFDocumentGetPage(document, 1);			// always page 1 only

	CGContextDrawPDFPage(ctx, page);					// Just what it says it does ...

	//CFRelease(document);	// we created it
	//CFRelease(provider);	// we created it
}


//--------------------------------------------------------------------------------------------------


void DrawIntoPDFPage(CGContextRef pdfContext, CGRect pageBounds, int gwnum, UInt32 pageNumber)
{
#pragma unused(pageNumber)
    
	CGColorSpaceRef genericColorSpace = GetGenericRGBColorSpace();
	MTViewData* data;
	CGContextBeginPage(pdfContext, &pageBounds);
	
	// ensure that we are drawing in the correct color space, a calibrated color space
	CGContextSetFillColorSpace(pdfContext, genericColorSpace); 
	CGContextSetStrokeColorSpace(pdfContext, genericColorSpace); 
    
    int i,x,y;
    CGContextRef SetupPDFcontext(int gwnum);
    CGContextRef	lgpdfContext;
    
    if(oma_wind[gwnum].num_annotations>0){
        lgpdfContext = SetupPDFcontext(gwnum);
        
        CGContextSetLineWidth (lgpdfContext, 1);
        CGContextSetRGBStrokeColor (lgpdfContext, 1, 1, 1, 1);
        CGContextSetBlendMode(lgpdfContext,kCGBlendModeDifference);
        CGContextSelectFont (lgpdfContext,"Helvetica",18,kCGEncodingMacRoman);
        CGContextSetTextDrawingMode (lgpdfContext,kCGTextFillStroke);
        CGContextBeginPath (lgpdfContext);

        for(i=0; i< oma_wind[gwnum].num_annotations; i++){ 
            x = oma_wind[gwnum].annotations[i].xpos;
            y = oma_wind[gwnum].annotations[i].ypos;
            switch (oma_wind[gwnum].annotations[i].type) {
                case WINDOW_MARKER:
                    CGContextMoveToPoint( lgpdfContext, x-MARKSIZE, y-MARKSIZE );			
                    CGContextAddLineToPoint( lgpdfContext, x+MARKSIZE, y+MARKSIZE );
                    CGContextMoveToPoint( lgpdfContext, x-MARKSIZE, y+MARKSIZE );			
                    CGContextAddLineToPoint( lgpdfContext, x+MARKSIZE, y-MARKSIZE );
                    break;
                    
                case WINDOW_LABEL:
                    CGContextShowTextAtPoint(lgpdfContext,x,y,
                                             oma_wind[gwnum].annotations[i].label,
                                             strlen(oma_wind[gwnum].annotations[i].label));

                    break;
                    
                default:
                    break;
            } 

        }
        CGContextStrokePath( lgpdfContext );
        CGContextEndPage(lgpdfContext);
        CGContextRelease(lgpdfContext);

    }

	
	if (oma_wind[gwnum].pdfData != NULL)
	{
        // this seems to get called when copying or saving pdf from a plot rows or cols
        // also when ther are annotations from code above

        // first put color image under graphics
        
        data = (MTViewData*) oma_wind[gwnum].view_data_ptr;
		CGContextDrawImage(pdfContext, pageBounds, data->theImage);
        
		DrawPDFData(pdfContext, oma_wind[gwnum].pdfData);
        
	} else {
        
		data = (MTViewData*) oma_wind[gwnum].view_data_ptr;
		CGContextDrawImage(pdfContext, pageBounds, data->theImage);

        
	}
	CGContextEndPage(pdfContext);
}


// Add the windows contents to the Pastboard as PDF.  This function assums that the pasteboard is
// not NULL.
OSStatus AddWindowContentToPasteboardAsPDF( PasteboardRef pasteboard, int gwnum)
{
	OSStatus				err		= noErr;
	//if(oma_wind[gwnum].windowtype == QROWCOL) return -1;	// don't add anything for row/column plots
															// the pdf data hasn't been saved
	CGRect					docRect = CGRectMake(0, 0, oma_wind[gwnum].width, oma_wind[gwnum].height);
	CFDataRef				pdfData = CFDataCreateMutable( kCFAllocatorDefault, 0);
	CGContextRef			pdfContext;
	CGDataConsumerRef		consumer;
	CGDataConsumerCallbacks cfDataCallbacks = { MyCFDataPutBytes, MyCFDataRelease };
	
	// We need to clear the pasteboard of it's current contents so that this application can
	// own it and add it's own data.
	err = PasteboardClear( pasteboard );
	require_noerr( err, PasteboardClear_FAILED );

	consumer = CGDataConsumerCreate((void*)pdfData, &cfDataCallbacks);
	
	// For now (and for demo purposes), just put the whole window drawing as pdf data
	// on the paste board, regardless of what is selected in the window.
	pdfContext = CGPDFContextCreate(consumer, &docRect, NULL);
	require(pdfContext != NULL, CGPDFContextCreate_FAILED);
	
	DrawIntoPDFPage(pdfContext, docRect, gwnum, 1);
	CGContextRelease(pdfContext);   // this finalizes the pdfData
	
	err = PasteboardPutItemFlavor( pasteboard, (PasteboardItemID)1,
						CFSTR("com.adobe.pdf"), pdfData, 0 );
	require_noerr( err, PasteboardPutItemFlavor_FAILED );
	
CGPDFContextCreate_FAILED:
PasteboardPutItemFlavor_FAILED:
	CGDataConsumerRelease(consumer);	// this also releases the pdfData, via MyCFDataRelease

PasteboardClear_FAILED:
	return err;
}

CGContextRef SetupPDFcontext(int gwnum)
{
	//OSStatus				err		= noErr;
	CGRect					docRect = CGRectMake(0, 0, oma_wind[gwnum].width, oma_wind[gwnum].height);
	CFDataRef				pdfData = CFDataCreateMutable( kCFAllocatorDefault, 0);
	CGContextRef			pdfContext;
	CGDataConsumerRef		consumer;
	CGDataConsumerCallbacks cfDataCallbacks = { MyCFDataPutBytes, MyCFDataRelease };
	
	consumer = CGDataConsumerCreate((void*)pdfData, &cfDataCallbacks);
	
	// For now (and for demo purposes), just put the whole window drawing as pdf data
	// on the paste board, regardless of what is selected in the window.
	pdfContext = CGPDFContextCreate(consumer, &docRect, NULL);
	oma_wind[gwnum].pdfData = pdfData;
	oma_wind[gwnum].consumer = consumer;
	// notes on releasing pdfContext and data
	// CGDataConsumerRelease(consumer);	// this also releases the pdfData, via MyCFDataRelease
	
	CGColorSpaceRef genericColorSpace = GetGenericRGBColorSpace();
	CGContextBeginPage(pdfContext, &docRect);
	CGContextSetFillColorSpace(pdfContext, genericColorSpace); 
	CGContextSetStrokeColorSpace(pdfContext, genericColorSpace); 


	return pdfContext;
}



// modified from sample code in the apple quartz programming guide

void MyCreatePDFFile (CGRect pageRect, const char *filename, int gwnum)
{
    CGContextRef pdfContext;
    CFStringRef path;
    CFURLRef url;
 
    path = CFStringCreateWithCString (NULL, filename, kCFStringEncodingUTF8);
    url = CFURLCreateWithFileSystemPath (NULL, path, kCFURLPOSIXPathStyle, 0);
    CFRelease (path);
    pdfContext = CGPDFContextCreateWithURL (url, &pageRect, NULL);
    CFRelease(url);
    //CGContextBeginPage (pdfContext, &pageRect)
    //myDrawContent (pdfContext);
	DrawIntoPDFPage( pdfContext, pageRect, gwnum, 1);
    //CGContextEndPage (pdfContext);
    CGContextRelease (pdfContext);
	
}

