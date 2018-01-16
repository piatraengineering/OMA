#define DO_MACH_O 1

#define ATTENUATE 1

#define LAB_JACK 1
//#define ANALOG_IO 1
// Labjack U3
// for this, you need to copy the liblabjackusb.dylib to /usr/lib
// if that file is not found in /usr/lib, it will fail to load when oma starts up and oma will quit

//#define LJU3 1


//#define SensiCam 1

#define __PREFIX__

//#define PVCAM 1

#define TEXACO 1
#define GPIB 1
#define VISA 1

// if you want to use commands related to GPHOTO2
#define GPHOTO 1

//#define ST6 1 (and old serial port SBIG camera)

#define SERIAL_PORT 1
//#define SPEX 1

// for newer SBIG detectors
#define SBIG 1