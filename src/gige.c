/*
 | ==============================================================================
 | Copyright (C) 2005-2007 Prosilica.  All Rights Reserved.
 |
 | Redistribution of this header file, in original or modified form, without
 | prior written consent of Prosilica is prohibited.
 |
 |==============================================================================
 |
 | This sample code, open the first camera found on the host computer and set it
 | for capturing. It then wait for the user to press a key before enqueuing a
 | frame and saving it to a TIFF file if the capture was successful
 |
 |==============================================================================
 |
 | THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 | WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
 | NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
 | DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 | INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 | OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF
 | LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 | NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 | EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 |
 |==============================================================================
 */

#define _OSX
#define _x86

typedef char bool;

#define false 0
#define true 1
//#define tPvUint32 unsigned int 

#if defined(_LINUX) || defined(_QNX) || defined(_OSX)
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#endif

#include "PvApi.h"
#include "ImageLib.h"

#include "impdefs.h"
#include "oma_quartz.h"

// camera's data
typedef struct 
{
    unsigned long   UID;
    tPvHandle       Handle;
    tPvFrame        Frame[100];
    
} tCamera;

#if defined(_LINUX) || defined(_QNX) || defined(_OSX)
void Sleep(unsigned int time)
{
    struct timespec t,r;
    
    t.tv_sec    = time / 1000;
    t.tv_nsec   = (time % 1000) * 1000000;    
    
    while(nanosleep(&t,&r)==-1)
        t = r;
}
#endif

int GigEinitialized = 0;
tCamera Camera;

// wait for a camera to be plugged
void WaitForCamera()
{   
    int i=0;
    
    printf("waiting for a camera ");
    while(!PvCameraCount() && i++<20)
    {
        printf(".");
        Sleep(250);
    }
    printf("\n");
}

// get the first camera found
bool CameraGet(tCamera* Camera)
{
    tPvUint32 count,connected;
    tPvCameraInfo list;
    
    count = PvCameraList(&list,1,&connected);
    if(count == 1)
    {
        Camera->UID = list.UniqueId;
        printf("got camera %s\n",list.SerialString);
        return true;
    }
    else
        return false;
}

// open the camera
bool CameraSetup(tCamera* Camera)
{
    bool result;
    
    result = PvCameraOpen(Camera->UID,ePvAccessMaster,&(Camera->Handle));
    
    return !result; 
}

// setup and start streaming continuous mode
bool CameraStart_preview(tCamera* Camera, int* time)

{
    extern TWOBYTE 	header[];
    unsigned long FrameSize = 0;
    extern char txt[],cmnd[];
    char pixelformat[256];
    
    // Auto adjust the packet size to max supported by the network, up to a max of 8228.
    // NOTE: In Vista, if the packet size on the network card is set lower than 8228,
    //       this call may break the network card's driver. See release notes.
    //
    PvCaptureAdjustPacketSize(Camera->Handle,8228); //-mbl added this in (was commented out) -- seems OK
    
    // set the camera in capture mode
	if(PvCaptureStart(Camera->Handle)){
		printf("Couldn't start capture.\n");
		return false;
	}
    
    PvAttrEnumGet(Camera->Handle,"SensorType",pixelformat,sizeof(pixelformat),NULL);
    //sprintf(type,"Sensor type: %s\n",pixelformat);
    // set the camera pixel format
    if(strncmp(pixelformat,"Bayer",3) == 0){
        if(PvAttrEnumSet(Camera->Handle,"PixelFormat","Bayer16")){
            printf("Couldn't set format.\n");
            return false;
        }
    } else if (strncmp(pixelformat,"Mono",3) == 0){
        if(PvAttrEnumSet(Camera->Handle,"PixelFormat","Mono16")){
            printf("Couldn't set format.\n");
            return false;
        }  
    }
    
    // set the camera exposure mode
    if(PvAttrEnumSet(Camera->Handle, "ExposureMode","Manual")){
		printf("Couldn't set exposure mode.\n");
		return false;
	}
    
	// Need checking for valid values here
	if(PvAttrUint32Set(Camera->Handle, "Width", header[NCHAN])){
		printf("Couldn't set width.\n");
		return false;
	}
	if(PvAttrUint32Set(Camera->Handle, "Height", header[NTRAK])){
		printf("Couldn't set height.\n");
		return false;
	}
	header[NX0] = header[NX0] & 0xfffe;
	if(PvAttrUint32Set(Camera->Handle, "RegionX", header[NX0])){
		printf("Couldn't set X0.\n");
		return false;
	}
	header[NY0] = header[NY0] & 0xfffe;
	if(PvAttrUint32Set(Camera->Handle, "RegionY", header[NY0])){
		printf("Couldn't set Y0.\n");
		return false;
	}
    
    if(PvAttrUint32Set(Camera->Handle, "ExposureValue", *time)){
        printf("Couldn't set exposure.\n");
        return false;
    }
    
    // how big should the frame buffers be?
    if(PvAttrUint32Get(Camera->Handle,"TotalBytesPerFrame",&FrameSize)) {   // this is affected by width and height
		printf("Couldn't get size.\n");
		return false;
	}
    
    // set the streambyte/second value
    if(PvAttrUint32Set(Camera->Handle,"StreamBytesPerSecond",115000000)){
        printf("Couldn't set streamhold capacity.\n");
        return false;
    }
    
    // set the camera in multiframe acquisition mode
	if(PvAttrEnumSet(Camera->Handle,"AcquisitionMode","Continuous")){
		printf("Couldn't set continuous acquisition mode.\n");
		return false;
	}
    
    // set the trigger mode
    if(PvAttrEnumSet(Camera->Handle,"FrameStartTriggerMode","Freerun")){
        printf("Couldn't set free run mode.\n");
        return false;
    }
    
	// allocate the buffer for the frames we need
    Camera->Frame->Context[0]  = Camera;
    Camera->Frame->ImageBuffer = malloc(FrameSize);
    if(Camera->Frame->ImageBuffer){
        Camera->Frame->ImageBufferSize = FrameSize;
    } else {
        printf("Couldn't allocate space.\n");
        return false;
    }
    
    // IMAGE ACQUISITION
	if(PvCommandRun(Camera->Handle,"AcquisitionStart"))
	{
		// if that fails, we reset the camera to non capture mode
		PvCaptureEnd(Camera->Handle) ;
		printf("Couldn't start acqusition.\n");
		return false;
	}
    
	return true;    
}


// setup and start streaming continuous mode
bool CameraStart_continuous(tCamera* Camera, int* time, int* fcount, int* frate, int* gain, int* trigger, int *triggerDelay, int *binx, int* biny)

{
    extern TWOBYTE 	header[];
    unsigned long FrameSize = 0;
    extern char txt[],cmnd[];
    char pixelformat[256];
    
    // Auto adjust the packet size to max supported by the network, up to a max of 8228.
    // NOTE: In Vista, if the packet size on the network card is set lower than 8228,
    //       this call may break the network card's driver. See release notes.
    //
    PvCaptureAdjustPacketSize(Camera->Handle,8228); //-mbl added this in (was commented out) -- seems OK
    
    // set the camera in capture mode
	if(PvCaptureStart(Camera->Handle)){
		printf("Couldn't start capture.\n");
		return false;
	}
    
    PvAttrEnumGet(Camera->Handle,"SensorType",pixelformat,sizeof(pixelformat),NULL);
    //sprintf(type,"Sensor type: %s\n",pixelformat);
    // set the camera pixel format
    if(strncmp(pixelformat,"Bayer",3) == 0){
        if(PvAttrEnumSet(Camera->Handle,"PixelFormat","Bayer16")){
            printf("Couldn't set format.\n");
            return false;
        }
    } else if (strncmp(pixelformat,"Mono",3) == 0){
        if(PvAttrEnumSet(Camera->Handle,"PixelFormat","Mono16")){
            printf("Couldn't set format.\n");
            return false;
        }
    }
    
    // set the camera exposure mode
    if(PvAttrEnumSet(Camera->Handle, "ExposureMode","Manual")){
		printf("Couldn't set exposure mode.\n");
		return false;
	}
    // set the camera gain mode
    if(PvAttrEnumSet(Camera->Handle, "GainMode","Manual")){
		printf("Couldn't set gain mode.\n");
		return false;
	}
    
    // set the camera gain value (decibel)
    if(PvAttrUint32Set(Camera->Handle, "GainValue", *gain)){
        printf("Couldn't set gain.\n");
        return false;
    }
    
    // set the binning factor
    if(PvAttrUint32Set(Camera->Handle, "BinningX", *binx)){
        printf("Couldn't set horizontal binning.\n");
        return false;
    }
    if(PvAttrUint32Set(Camera->Handle, "BinningY", *biny)){
        printf("Couldn't set vertical binning.\n");
        return false;
    }
    
	// Need checking for valid values here
	if(PvAttrUint32Set(Camera->Handle, "Width", header[NCHAN])){
		printf("Couldn't set width.\n");
		return false;
	}
	if(PvAttrUint32Set(Camera->Handle, "Height", header[NTRAK])){
		printf("Couldn't set height.\n");
		return false;
	}
	header[NX0] = header[NX0] & 0xfffe;
	if(PvAttrUint32Set(Camera->Handle, "RegionX", header[NX0])){
		printf("Couldn't set X0.\n");
		return false;
	}
	header[NY0] = header[NY0] & 0xfffe;
	if(PvAttrUint32Set(Camera->Handle, "RegionY", header[NY0])){
		printf("Couldn't set Y0.\n");
		return false;
	}
    
    if(PvAttrUint32Set(Camera->Handle, "ExposureValue", *time)){
        printf("Couldn't set exposure.\n");
        return false;
    }
    
    // how big should the frame buffers be?
    if(PvAttrUint32Get(Camera->Handle,"TotalBytesPerFrame",&FrameSize)) {   // this is affected by width and height
		printf("Couldn't get size.\n");
		return false;
	}
    
    // set the streambyte/second value
    if(PvAttrUint32Set(Camera->Handle,"StreamBytesPerSecond",115000000)){
        printf("Couldn't set streamhold capacity.\n");
        return false;
    }
    
    // set the camera in continuous acquisition mode
	if(PvAttrEnumSet(Camera->Handle,"AcquisitionMode","Continuous")){
		printf("Couldn't set continuous acquisition mode.\n");
		return false;
	}
    /*
    // set the trigger mode
    if(PvAttrEnumSet(Camera->Handle,"FrameStartTriggerMode","Freerun")){
        printf("Couldn't set free run mode.\n");
        return false;
    }
     */
    
    
    
	// allocate the buffer for the frames we need
    Camera->Frame->Context[0]  = Camera;
    Camera->Frame->ImageBuffer = malloc(FrameSize);
    if(Camera->Frame->ImageBuffer){
        Camera->Frame->ImageBufferSize = FrameSize;
    } else {
        printf("Couldn't allocate space.\n");
        return false;
    }

    
    // **** INTERNAL TRIGGER **** //
    if (*trigger == 0) {
        
        // set the frame rate value
        if(PvAttrFloat32Set(Camera->Handle, "FrameRate",*frate)){
            printf("Couldn't set frame rate.\n");
            return false;
        }
        
        // set the trigger mode
        if(PvAttrEnumSet(Camera->Handle,"FrameStartTriggerMode","FixedRate")){
            printf("Couldn't set fixed-rate mode.\n");
            return false;
        }
        // IMAGE ACQUISITION
        if(PvCommandRun(Camera->Handle,"AcquisitionStart"))
        {
            // if that fails, we reset the camera to non capture mode
            PvCaptureEnd(Camera->Handle) ;
            printf("Couldn't start acqusition.\n");
            return false;
        }

        
    } else if (*trigger == 1){
        
        // FrameStartTriggerMode,"SyncIn2"
        if(PvAttrEnumSet(Camera->Handle,"FrameStartTriggerMode","SyncIn2")){
            printf("Couldn't set trigger mode.\n");
            return false;
        }
        
        // FrameStartTriggerEvent,"EdgeRising"
        if(PvAttrEnumSet(Camera->Handle,"FrameStartTriggerEvent","EdgeRising")){
            printf("Couldn't set edge rising event.\n");
            return false;
        }
        // FrameStartTriggerDelay
        if(PvAttrUint32Set(Camera->Handle,"FrameStartTriggerDelay",*triggerDelay)){
            printf("Couldn't set trigger delay.\n");
            return false;
        }
        
        // AcqStartTriggerMode,"SyncIn2"
        if(PvAttrEnumSet(Camera->Handle,"AcqStartTriggerMode","SyncIn2")){
            printf("Couldn't set acquisition trigger mode.\n");
            return false;
        }
        
        // AcqStartTriggerEvent,"EdgeRising"
        if(PvAttrEnumSet(Camera->Handle,"AcqStartTriggerEvent","LevelHigh")){
            printf("Couldn't set acquisition edgerising event.\n");
            return false;
        }

    }
    
	return true;
}


// setup and start streaming MULTIFRAME MODE
bool CameraStart(tCamera* Camera, int* time, int* fcount, int* frate, int* gain, int* trigger, int *triggerDelay, int *binx, int* biny)

{
    extern TWOBYTE 	header[];
    unsigned long FrameSize = 0;
    unsigned long strcap = 0;
    unsigned long strbyte = 0;
    extern char txt[],cmnd[];
    char pixelformat[256];
    
    // Auto adjust the packet size to max supported by the network, up to a max of 8228.
    // NOTE: In Vista, if the packet size on the network card is set lower than 8228,
    //       this call may break the network card's driver. See release notes.
    //
    PvCaptureAdjustPacketSize(Camera->Handle,8228); //-mbl added this in (was commented out) -- seems OK
    
    // set the camera in capture mode
	if(PvCaptureStart(Camera->Handle)){
		printf("Couldn't start capture.\n");
		return false;
	}
    
    PvAttrEnumGet(Camera->Handle,"SensorType",pixelformat,sizeof(pixelformat),NULL);
    //sprintf(type,"Sensor type: %s\n",pixelformat);
    // set the camera pixel format
    if(strncmp(pixelformat,"Bayer",3) == 0){
        if(PvAttrEnumSet(Camera->Handle,"PixelFormat","Bayer16")){
            printf("Couldn't set format.\n");
            return false;
        }
    } else if (strncmp(pixelformat,"Mono",3) == 0){
        if(PvAttrEnumSet(Camera->Handle,"PixelFormat","Mono16")){
            printf("Couldn't set format.\n");
            return false;
        }  
    }
    
    // set the camera exposure mode
    if(PvAttrEnumSet(Camera->Handle, "ExposureMode","Manual")){
		printf("Couldn't set exposure mode.\n");
		return false;
	}
    
    // set the camera exposure mode
    if(PvAttrEnumSet(Camera->Handle, "GainMode","Manual")){
		printf("Couldn't set gain mode.\n");
		return false;
	}
    
    // set the camera gain value (decibel)
    if(PvAttrUint32Set(Camera->Handle, "GainValue", *gain)){
        printf("Couldn't set gain.\n");
        return false;
    }
    
    // set the binning factor 
    if(PvAttrUint32Set(Camera->Handle, "BinningX", *binx)){
        printf("Couldn't set horizontal binning.\n");
        return false;
    }
    if(PvAttrUint32Set(Camera->Handle, "BinningY", *biny)){
        printf("Couldn't set vertical binning.\n");
        return false;
    }

    // this looks problematic to me  -mbl
    // setting this is up to the user
    // this will mess up the buffer size
    
    //header[NCHAN] = header[NCHAN] / *binx;
    //header[NTRAK] = header[NTRAK] / *biny;
    
	// Need checking for valid values here
	//
	if(PvAttrUint32Set(Camera->Handle, "Width", header[NCHAN])){
		printf("Couldn't set width.\n");
		return false;
	}
	if(PvAttrUint32Set(Camera->Handle, "Height", header[NTRAK])){
		printf("Couldn't set height.\n");
		return false;
	}
	header[NX0] = header[NX0] & 0xfffe;
	if(PvAttrUint32Set(Camera->Handle, "RegionX", header[NX0])){
		printf("Couldn't set X0.\n");
		return false;
	}
	header[NY0] = header[NY0] & 0xfffe;
	if(PvAttrUint32Set(Camera->Handle, "RegionY", header[NY0])){
		printf("Couldn't set Y0.\n");
		return false;
	}
    
    // adjust the framerate if the one selected is to high for the camera
    if(strncmp(pixelformat,"Bayer",3) == 0){
        if(*frate > (-0.0000001572 * pow(header[NTRAK],3) + 0.0003954* pow(header[NTRAK],2) - 0.3615*header[NTRAK] + 153.6)){
            *frate = (-0.0000001572 * pow(header[NTRAK],3) + 0.0003954* pow(header[NTRAK],2) - 0.3615*header[NTRAK] +  153.6)-1; 
            if (header[NTRAK] <= 180){
                if ( *frate >= 99) *frate = 100;
            }
            if (header[NTRAK] == 1024){
                if ( *frate >= 28) *frate = 30;
            }
            printf("Fps set to %d (maximum value given the frame size)\n\n", *frate);
        } 
    } else if (strncmp(pixelformat,"Mono",3) == 0){
        if(*frate > (0.0000000002296 * pow(header[NTRAK],4) - 0.0000006682 * pow(header[NTRAK],3) + 0.0007413 * pow(header[NTRAK],2) - 0.4031 *header[NTRAK] + 120.7)){
            *frate = (0.0000000002296 * pow(header[NTRAK],4) - 0.0000006682 * pow(header[NTRAK],3) + 0.0007413 * pow(header[NTRAK],2) - 0.4031 *header[NTRAK] + 120.7)-1;
            if (header[NTRAK] == 1024){
                if ( *frate >= 19) *frate = 20;
            }
            printf("Fps set to %d (maximum value given the frame size)\n\n", *frate);
        }
    }
    
    if(*time * *frate > 1000000){ 
        *time = 1000000 / *frate;
        printf("The selected exposure is not compatible with the framerate\n");
        printf("Exposure time decreased to %d us\n", *time);
    }
    
    if(PvAttrUint32Set(Camera->Handle, "ExposureValue", *time)){
        printf("Couldn't set exposure.\n");
        return false;
    }
    
    // how big should the frame buffers be?
    if(PvAttrUint32Get(Camera->Handle,"TotalBytesPerFrame",&FrameSize)) {   // this is affected by width and height
		printf("Couldn't get size.\n");
		return false;
	}
    
    // get the streamhold capacity of the camera
    if(PvAttrUint32Get(Camera->Handle,"StreamHoldCapacity",&strcap)){
        printf("Couldn't set streamhold capacity.\n");
        return false;
    }
    
    // set the streambyte/second value
    if(PvAttrUint32Set(Camera->Handle,"StreamBytesPerSecond",115000000)){
        printf("Couldn't set streamhold capacity.\n");
        return false;
    }
    
    if(PvAttrUint32Get(Camera->Handle,"StreamBytesPerSecond",&strbyte)){
        printf("Couldn't get streamhold capacity.\n");
        return false;
    }
    
    // set the number of frame to acquire
    if(PvAttrUint32Set(Camera->Handle,"AcquisitionFrameCount",*fcount)){
		printf("Couldn't set acquisition frame count.\n");
		return false;
	}
    
	// allocate the buffer for the frames we need
    for(int i=0; i< *fcount; i++){
        Camera->Frame[i].Context[0]  = Camera;
        Camera->Frame[i].ImageBuffer = malloc(FrameSize);
        if(Camera->Frame[i].ImageBuffer){
            Camera->Frame[i].ImageBufferSize = FrameSize;
        } else {
            printf("Couldn't allocate space.\n");
            return false;
        }
        // queue frames
        PvCaptureQueueFrame(Camera->Handle,&(Camera->Frame[i]),NULL);
    }
    
    // **** INTERNAL TRIGGER **** //
    if (*trigger == 0) {
        
        // set the frame rate value
        if(PvAttrFloat32Set(Camera->Handle, "FrameRate",*frate)){
            printf("Couldn't set frame rate.\n");
            return false;
        }
        
        // set the trigger mode
        if(PvAttrEnumSet(Camera->Handle,"FrameStartTriggerMode","FixedRate")){
            printf("Couldn't set fixed-rate mode.\n");
            return false;
        }
        
        // set the camera in multiframe acquisition mode
        if(PvAttrEnumSet(Camera->Handle,"AcquisitionMode","MultiFrame")){
            printf("Couldn't set multiframe acquisition mode.\n");
            return false;
        }
        
        // enable the camera to store the images in its memory
        if(PvAttrEnumSet(Camera->Handle,"StreamHoldEnable","On")){
            printf("Couldn't set stream hold.\n");
            return false;
        }
        
        // IMAGE ACQUISITION
        if(PvCommandRun(Camera->Handle,"AcquisitionStart"))
        {
            // if that fails, we reset the camera to non capture mode
            PvCaptureEnd(Camera->Handle) ;
            printf("Couldn't start acqusition.\n");
            return false;
        }
        
        // release the images to the host computer
        if(PvAttrEnumSet(Camera->Handle,"StreamHoldEnable","Off")){
            printf("Couldn't set stream hold.\n");
            return false;
        }
        
    // **** EXTERNAL TRIGGER **** //
    } else if (*trigger == 1){
        
        // FrameStartTriggerMode,"SyncIn2"
        if(PvAttrEnumSet(Camera->Handle,"FrameStartTriggerMode","SyncIn2")){
            printf("Couldn't set trigger mode.\n");
            return false;
        }
        
        // FrameStartTriggerEvent,"EdgeRising"
        if(PvAttrEnumSet(Camera->Handle,"FrameStartTriggerEvent","EdgeRising")){
            printf("Couldn't set edge rising event.\n");
            return false;
        }
        
        // set the camera in multiframe acquisition mode
        if(PvAttrEnumSet(Camera->Handle,"AcquisitionMode","MultiFrame")){
            printf("Couldn't set multiframe acquisition mode.\n");
            return false;
        }
        
        // FrameStartTriggerDelay
        if(PvAttrUint32Set(Camera->Handle,"FrameStartTriggerDelay",*triggerDelay)){
            printf("Couldn't set trigger delay.\n");
            return false;
        }
        
        // AcqStartTriggerMode,"SyncIn2"
        if(PvAttrEnumSet(Camera->Handle,"AcqStartTriggerMode","SyncIn2")){
            printf("Couldn't set acquisition trigger mode.\n");
            return false;
        }
        
        // AcqStartTriggerEvent,"EdgeRising"
        if(PvAttrEnumSet(Camera->Handle,"AcqStartTriggerEvent","LevelHigh")){
            printf("Couldn't set acquisition edgerising event.\n");
            return false;
        }

    }
    
    PvCaptureEnd(Camera->Handle); //should be here, before CameraSnap, otherwise it doesn't work properly
    
	return true;    
}

// snap and save a frame from the camera
void CameraSnap(tCamera* Camera, int* time, int *fcount, int* frate, int* label, int* trigger, int *binx, int *biny, int* sav, char* savestr)
{
    int i,j;
    short *ptr;
    int dquartz();
    DateTimeRec datetime;
    
    extern DATAWORD *datpt;
    extern int doffset;
    extern TWOBYTE 	header[];
    extern char     cmnd[],*fullname();
    extern char     saveprefixbuf[];
    
    unsigned long Tfreq;
    unsigned long T_hi;
    unsigned long T_lo;
    double tempo;
    int nt,nc;
    FILE *fp;
   
    GetTime(&datetime);
    int month = datetime.month;
    int day = datetime.day;
    int year = datetime.year%100;
    int hour = datetime.hour;
    int minute = datetime.minute;
    float second = datetime.second;
    int secondo = second;
    double tempo_pr = 0;
    char strin[256];
    char ora[256];
    
    PvCommandRun(Camera->Handle,"TimeStampReset");
    PvAttrUint32Get(Camera->Handle,"TimeStampFrequency",&Tfreq);
    
    for(j=0; j< *fcount; j++){
        
        if(PvCaptureQueueFrame(Camera->Handle,&(Camera->Frame[j]),NULL)){
            
            while(PvCaptureWaitForFrameDone(Camera->Handle,&(Camera->Frame[j]),PVINFINITE) == ePvErrTimeout)
                printf("still waiting ...\n");
            
            if(Camera->Frame[j].Status == ePvErrSuccess){
                
                ptr = Camera->Frame[j].ImageBuffer;
                
                // move the data
                for(i=0; i< header[NCHAN]*header[NTRAK]; i++){
                    *(datpt+doffset+i) = *(ptr + i);
                }
                dquartz(0,0);
                
                //If the camera crashes, try to remove the timestamp calculation.
                
                PvCommandRun(Camera->Handle,"TimeStampValueLatch");
                PvAttrUint32Get(Camera->Handle,"TimeStampValueHi",&T_hi);
                PvAttrUint32Get(Camera->Handle,"TimeStampValueLo",&T_lo);
                
                tempo = (T_hi*4294967296. + T_lo) / Tfreq;  
                float fratec = 1./(tempo - tempo_pr);
                tempo_pr = tempo;
                
                if(*label == 1){
                    
                    char buffer[100];
                    if (*trigger == 0){
                        sprintf(buffer,"%2d/%2d/%2d  %2d:%2d:%.4f  (Frame %d/%d - %d fps - exp. %d µs) ",
                            month, day, year, hour, minute, (secondo + tempo), j+1, *fcount, *frate, *time);
                    } else if (*trigger == 1){
                        sprintf(buffer,"%2d/%2d/%2d  %2d:%2d:%.4f  (Frame %d/%d - %.1f fps - exp. %d µs) ",
                                month, day, year, hour, minute, (secondo + tempo), j+1, *fcount, fratec, *time);
                    }
                    WindowPtr theActiveWindow;
                    CGContextRef	myContext;
                    OSErr err;
                    char txt[100];
                    theActiveWindow = FrontWindow();
                    extern int gwind_no;
                    extern OMA_Window oma_wind[];
                    
                    gwind_no = activegwnum(theActiveWindow);
                    
                    err = QDBeginCGContext(GetWindowPort(theActiveWindow), &myContext);
                    CGContextBeginPath (myContext);
                    CGContextSelectFont (myContext,"Helvetica",15,kCGEncodingMacRoman);
                    CGContextSetTextDrawingMode (myContext,kCGTextFillStroke);
                    CGContextSetRGBFillColor (myContext, 1, 1, 1, 1);
                    CGContextSetLineWidth (myContext, .3);
                    sprintf(txt,"%s",buffer);
                    CGContextShowTextAtPoint(myContext,10,10,txt,strlen(txt));
                    CGContextFlush(myContext);
                    QDEndCGContext (GetWindowPort(theActiveWindow), &myContext);
                    
                    second = second + 1./ *frate;
                }
                
                if(*sav == 1){
                    strcpy(strin,savestr);
                    sprintf(ora,"_%d_%d_%.4f",hour, minute, (secondo + tempo));
                    strcat(strin,ora);
                    
                    fp = fopen(fullname(strin,SAVE_DATA),"w");
                    if( fp != NULL) {
                        i=0;
                        for(nt=0; nt<header[NTRAK]; nt++){
                            for(nc=0; nc<header[NCHAN]; nc++){
#ifdef FLOAT
                                fprintf(fp,"%gain\t",(*(datpt+i++)));
#else
                                fprintf(fp,"%d\t",(*(datpt+i++)));
#endif
                            }
                            fprintf(fp,"\n");
                        }
                        fclose(fp);
                    }
                }
            }
			//pattern on the GC1380CH is Red Green Green Blue
            else{
                printf("the frame failed to be captured. Status: %d\n",Camera->Frame[j].Status);			
            }
        }
        else{
            printf("failed to queue the frame\n");
        }
    } 
}


void CameraSnap_preview(tCamera* Camera)
{
    int i;
    short *ptr;
    
    extern DATAWORD *datpt;
    extern int doffset;
    extern TWOBYTE 	header[];
    
    if(!PvCaptureQueueFrame(Camera->Handle,&(Camera->Frame[0]),NULL))
    {
        //printf("waiting for the frame to be done ...\n");
        while(PvCaptureWaitForFrameDone(Camera->Handle,&(Camera->Frame[0]),100) == ePvErrTimeout)
            printf("still waiting ...\n");
        if(Camera->Frame[0].Status == ePvErrSuccess)
        {
			ptr = Camera->Frame[0].ImageBuffer;
            
			// move the data
			for(i=0; i< header[NCHAN]*header[NTRAK]; i++){
				*(datpt+doffset+i) = *(ptr+i);
			}
			//pattern on the GC1380CH is Red Green Green Blue
        } else {
            printf("the frame failed to be captured. Status: %d\n",Camera->Frame[0].Status);			
		}
    } 
    else
        printf("failed to queue the frame\n");
}

// stop streaming
void CameraStop(tCamera* Camera)
{
    unsigned long stat_pack_miss = 0;
    unsigned long stat_frame_drop = 0;
    
    if(PvAttrUint32Get(Camera->Handle,"StatPacketsMissed",&stat_pack_miss)){
        printf("Couldn't get stat frame rate.\n");
        return;
    }
	printf("Packets missed: %d.\n",stat_pack_miss);
    
    if(PvAttrUint32Get(Camera->Handle,"StatFramesDropped",&stat_frame_drop)){
        printf("Couldn't get stat frame rate.\n");
        return;
    }
	printf("Frames dropped: %d.\n\n",stat_frame_drop);
    
    PvCaptureQueueClear(Camera->Handle);
    PvCommandRun(Camera->Handle,"AcquisitionStop");
    //PvCaptureEnd(Camera->Handle);  
}

// unsetup the camera

void CameraUnsetup(tCamera* Camera, int *fcount)
{
    //PvCameraClose(Camera->Handle);
    // and free the image buffer of the frame
    for(int i=0;i< *fcount;i++){
        if (Camera->Frame[i].ImageBuffer)
            free( (char*)Camera->Frame[i].ImageBuffer);     // occasional errors: pointer being freed was not allocated
        Camera->Frame[i].ImageBuffer = 0;
    }
}

/* ************************************************************************************ */
/*
 GIGE
 OMA command for the Prosilica GigE camera.
 */

int gige(int n, int index)
{
	extern int have_max;
    extern char txt[],cmnd[];  		/* the command buffer */ 
    extern TWOBYTE 	header[];
    extern short newwindowflag;
    int save_new_status;
    int dquartz();
    
    static int time = 10000;    // default exposure time in us
    static int numFrames = 1;         // default number of frame 
    static int frameRate = 1;           // default framerate in fps
    static int gain = 0;           // default gain in db
    static int bx = 1;          // default horizontal binning factor
    static int by = 1;          // default vertical binning factor
    static int label = 0;
    static int numPreviews = 0;
    static int preview = 0;
    static int trigger = 0;
    static int triggerDelay = 0;
    static int sav = 0;
    unsigned long fsize,scap;
    unsigned long sbyte=0;
    char pixelformat[256];
    char buffer[256];
    static char savestr[256];
    unsigned long pixwidth = 0;
    unsigned long pixheight = 0;
    
    int continuousFrames = 1;
    
    int createfile(int n,int index);
    int closefile();
    int concatfile();
    
    if(!GigEinitialized){
		
		// initialise the Prosilica API
		if(PvInitialize()){
			printf("failed to initialise the API\n");
			return -1;
		}
		
		memset(&Camera,0,sizeof(tCamera));	// just sets this to zero
		
		// wait for a camera to be plugged
		WaitForCamera();
		
		// get a camera from the list
		if(!CameraGet(&Camera)){
			printf("failed to find a camera\n");
			PvUnInitialize();
			return -1;
		}
		
		// setup the camera
		if(!CameraSetup(&Camera)){
			printf("failed to setup the camera\n");
			return -1;
		}
		GigEinitialized = 1;
		
    }
    
    
    if(index == 0) {
		// after initialized, let GIGE command be used to acquire
		strncpy(&cmnd[index],"acq",3);
	}
    
    if( strncmp(&cmnd[index],"exposure",3) == 0){
		sscanf(&cmnd[index],"%s %d",txt, &time);
		if( time <= 10) time = 10;
        if( time > 60000000) time = 60000000;
        printf(" Exposure set to %d us\n",time);
    }
    else if ( strncmp(&cmnd[index],"gain",3) == 0){
        sscanf(&cmnd[index],"%s %d",txt, &gain);
        if( gain <= 0) gain = 0;
        if( gain > 33) gain = 33;
        printf(" Gain value set to %d\n",gain);
    }
    else if ( strncmp(&cmnd[index],"number",3) == 0){
        sscanf(&cmnd[index],"%s %d",txt, &numFrames);
        if( numFrames <= 1) numFrames = 1;
        printf(" Frame number set to %d\n",numFrames);
    }
    else if ( strncmp(&cmnd[index],"rate",3) == 0){
        sscanf(&cmnd[index],"%s %d",txt, &frameRate);
        if( frameRate <= 1) frameRate = 1;
        printf(" Frame rate set to %d fps\n",frameRate);
    }
    // enable/disable the printing of the label over the image
    else if ( strncmp(&cmnd[index],"labon",5) == 0){
        label = 1; 
    }
    else if ( strncmp(&cmnd[index],"laboff",5) == 0){
        label = 0; 
    }
    // set the preview mode
    else if ( strncmp(&cmnd[index],"preview",3) == 0){
        sscanf(&cmnd[index],"%s %d",txt, &numPreviews);
        printf(" Number of frame preview set to %d \n",numPreviews);
        if (trigger == 1){
            printf("Preview not possible in external trigger mode.\n");
        } else { 
            strncpy(&cmnd[index],"acq",3);
            printf(" Wait... \n");
            preview = 1;
            sav = 0;
        }
    }
    // set the binning factor
    else if ( strncmp(&cmnd[index],"binx",4) == 0){
        sscanf(&cmnd[index],"%s %d",txt, &bx);
        if( bx < 1) bx = 1;
        if( bx > 8) bx = 8;
        by = bx;
        printf(" Horizontal (and vertical) binning factor set to %d\n",bx);
    }
    else if ( strncmp(&cmnd[index],"biny",4) == 0){
        sscanf(&cmnd[index],"%s %d",txt, &by);
        if( by < 1) by = 1;
        if( by > 8) by = 8;
        printf(" Vertical binning factor set to %d\n",by);
    }
    // set the trigger mode
    else if ( strncmp(&cmnd[index],"external",3) == 0){
        printf("* External trigger enabled * \n");
        trigger = 1;
    }
    else if ( strncmp(&cmnd[index],"internal",3) == 0){
        printf("* Internal trigger enabled * \n");
        trigger = 0;
    }
    // set the trigger delay [microseconds]
    else if ( strncmp(&cmnd[index],"delay",3) == 0){
        sscanf(&cmnd[index],"%s %d",txt, &triggerDelay);
        if (triggerDelay < 0) triggerDelay = 0;
        printf(" Trigger delay set to %d us. \n",triggerDelay);
    }
    // enable image saving 
    else if ( strncmp(&cmnd[index],"save",3) == 0){
        sscanf(&cmnd[index],"%s %s",txt, savestr);
        printf("* Image saving enabled * \n -> Set the prefix! \n");
        sav = 1;
    }
    else if ( strncmp(&cmnd[index],"endsave",4) == 0){
        printf("* Image saving disabled * \n");
        sav = 0;
    }
    // statistics
    else if ( strncmp(&cmnd[index],"stat",3) == 0){
        printf("\n");
        printf("OMA version: June 2013\n");
        PvAttrEnumGet(Camera.Handle,"SensorType",pixelformat,sizeof(pixelformat),NULL);
        sprintf(buffer,"Sensor type: %s\n",pixelformat);
        printf("\t%s",buffer);
        PvAttrUint32Get(Camera.Handle,"SensorWidth",&pixwidth);
        PvAttrUint32Get(Camera.Handle,"SensorHeight",&pixheight);
        printf("\tSensor dimension: %d x %d pixels \n",pixwidth,pixheight);
        printf("\tExposure time: %d us\n",time);
        printf("\tGain value: %d\n",gain);
        printf("\tFrame number: %d\n",numFrames);
        if (trigger == 0)   printf("\tFrame rate: %d fps.\n",frameRate);
        if (trigger == 1)   printf("\tFrame rate: see external source value.\n");
        printf("\tFrame dimension: %d x %d pixels\n",header[NCHAN],header[NTRAK]);
        printf("\tHorizontal binning: %d\n",bx);
        printf("\tVertical binning: %d\n",by);
        PvAttrUint32Get(Camera.Handle,"StreamHoldCapacity",&scap);
        printf("\tStream hold capacity: %d\n",scap);
        PvAttrUint32Get(Camera.Handle,"TotalBytesPerFrame",&fsize);
        printf("\tFramesize: %d\n",fsize);
        PvAttrUint32Get(Camera.Handle,"StreamBytesPerSecond",&sbyte);
        printf("\tStream bytes per second: %d\n",sbyte);
        if (trigger == 1)   printf("\tTrigger: external\n");
        if (trigger == 1)   printf("\tTrigger delay: %d us\n\n",triggerDelay);
        if (trigger == 0)   printf("\tTrigger: internal\n\n");
    }
    
    save_new_status = newwindowflag;
    
    if ( strncmp(&cmnd[index],"acq",3) == 0){
        
        // continuous acquisition mode
        if(preview){
            
            if(CameraStart_preview(&Camera,&time)){
                for(int i=0; i< numPreviews; i++){
                    // snap now
                    CameraSnap_preview(&Camera);
                    
                    dquartz(0,0);
                    checkevents();
                    newwindowflag = 0;  // if 0 opens the new image in the old window, if 1 it opens it in a new window
                }
                // stop the streaming
                PvCommandRun(Camera.Handle,"AcquisitionStop");
                PvCaptureEnd(Camera.Handle);            
                CameraUnsetup(&Camera, &numFrames);
                
                have_max = 0;
                maxx();
                newwindowflag = save_new_status;
                
                preview = 0;
                return 0;
                
            } else
                printf("Failed to start continuous streaming\n");
            
            // unsetup the camera
            CameraUnsetup(&Camera, &numFrames);
            
            return -1;
            
            // multiframe acquisition mode
        } else {
            
            
            //newwindowflag = 1;    // let the user set this -- don't force it -mbl
            
            // set the camera attributes
            if(CameraStart(&Camera,&time,&numFrames,&frameRate,&gain,&trigger,&triggerDelay,&bx,&by)){
                
                // snap now
                CameraSnap(&Camera,&time,&numFrames,&frameRate,&label,&trigger,&bx,&by,&sav,savestr);
                checkevents();
                
                // stop the streaming
                CameraStop(&Camera);
                CameraUnsetup(&Camera, &numFrames);
                
                have_max = 0;
                maxx();
                newwindowflag = save_new_status;
                
                // reset the binning settings
                header[NCHAN] = header[NCHAN] * bx;
                header[NTRAK] = header[NTRAK] * by;
                
                return 0;
                
            } else
                printf("Failed to start multiframe streaming\n");
            
            // unsetup the camera
            CameraUnsetup(&Camera, &numFrames);
            
            return -1;
            
        } 
    }


    if ( strncmp(&cmnd[index],"sframes",3) == 0){

        DateTimeRec datetime;
        GetTime(&datetime);
        int month = datetime.month;
        int day = datetime.day;
        int year = datetime.year%100;
        int hour = datetime.hour;
        int minute = datetime.minute;
        float second = datetime.second;
        int secondo = second;
        double tempo_pr = 0;
        unsigned long Tfreq;
        unsigned long T_hi;
        unsigned long T_lo;
        double tempo;
        FILE* timeFile = NULL;
        extern char     *fullname();
        char timeFileName[256];
        int sFrames = 1;
        
            
        sscanf(&cmnd[index],"%s %d",txt, &sFrames);
        if( sFrames < 1) sFrames = 1;
        
        strcpy(cmnd, savestr);
        strcpy(timeFileName, savestr);
        strcat(timeFileName, "_times.txt");
        createfile(0,0);
        timeFile = fopen(fullname(timeFileName,RAW_DATA),"w");

        // continuous acquisition mode
        if(CameraStart_continuous(&Camera,&time,&numFrames,&frameRate,&gain,&trigger,&triggerDelay,&bx,&by)){
            PvAttrUint32Get(Camera.Handle,"TimeStampFrequency",&Tfreq);
            for(int i=0; i< sFrames; i++){
                // snap now
                CameraSnap_preview(&Camera);
                
                dquartz(0,0);
                concatfile();
                
                PvCommandRun(Camera.Handle,"TimeStampValueLatch");
                PvAttrUint32Get(Camera.Handle,"TimeStampValueHi",&T_hi);
                PvAttrUint32Get(Camera.Handle,"TimeStampValueLo",&T_lo);
                
                
                tempo = (T_hi*4294967296. + T_lo) / Tfreq;
                float fratec = 1./(tempo - tempo_pr);
                tempo_pr = tempo;
                
                if (trigger == 0){
                    fprintf(timeFile,"%2d/%2d/%2d\t%2d\t%2d\t%.4f\tFrame %d/%d\t%d\tfps\texp.\t%d\tus\n ",
                            month, day, year, hour, minute, (secondo + tempo), i+1, sFrames, frameRate, time);
                } else if (trigger == 1){
                    fprintf(timeFile,"%2d/%2d/%2d\t%2d\t%2d\t%.4f\tFrame %d/%d\t%f\tfps\texp.\t%d\tus\n ",
                            month, day, year, hour, minute, (secondo + tempo), i+1, sFrames, fratec, time);
                }

                
                checkevents();
                
            }
            // stop the streaming
            PvCommandRun(Camera.Handle,"AcquisitionStop");
            PvCaptureEnd(Camera.Handle);
            CameraUnsetup(&Camera, &continuousFrames);
            
            have_max = 0;
            maxx();
            newwindowflag = save_new_status;
            
            closefile();
            fclose(timeFile);
            return 0;
            
        } else
            printf("Failed to start continuous streaming\n");
        
        
        // unsetup the camera
        CameraUnsetup(&Camera, &numFrames);
        
        return -1;
        
        // multiframe acquisition mode

    }
    return -1;
}


