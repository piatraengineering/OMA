/*****************************************************************************/
/*        Copyright (C) Photometrics Ltd. 1992-1997. All rights reserved.    */
/*****************************************************************************/
#ifndef _MASTER_H
#define _MASTER_H
static const char *_master_h_="$Header: /PVCAM/SourceCommon/master.h 8     5/14/02 3:52p Dtrent $";

/******************************** ANSI Types *********************************/
#if defined  __cplusplus        /* BORLAND   C++                             */
  #define PV_C_PLUS_PLUS
#elif defined  __cplusplus__    /* MICROSOFT C++   This allows us to         */
  #define PV_C_PLUS_PLUS          /*   insert the proper compiler flags,       */
#endif                          /*   in PVCAM.H for example, to cope         */
                                /*   properly with C++ definitions.          */

/**************************** Calling Conventions ****************************/
#if defined(WIN32)
  #if defined CDECL_CALL_CONV           /* Use the '_cdecl' calling convention */
    #define PV_DECL __declspec(dllexport) /*  or '__stdcall' calling convention  */
  #else                                 /*  as appropriate.                    */
    #define PV_DECL __declspec(dllexport) __stdcall
  #endif
  #define PV_CDECL _cdecl
#elif defined (LINUX)
  #define PV_DECL
  #define PV_CDECL
  #ifndef NULL
    #define NULL (void *) 0 /* lvr: Whoa! NULL is a reserved identifier. Also this definitiuon is not C++ compatible */
  #endif
#elif defined __mac_os_x
  #ifndef PV_DECL
    //#define PV_DECL __declspec( dllimport) /* Metrowerks CodeWarrior2 and later */
	#define PV_DECL
  #endif
  #ifndef PV_CDECL
    #define PV_CDECL
  #endif
#else
  #error OS Not supported
#endif

/**************************** PVCAM Pointer Types ****************************/
#define PV_PTR_DECL  *
#define PV_BUFP_DECL *

/******************************** PVCAM Types ********************************/
#if defined(WIN32)
  #ifndef __RPCNDR_H__            /* boolean is defined in RPCNDR.H */
    typedef unsigned short boolean;
  #endif
  typedef signed char     int8,   PV_PTR_DECL  int8_ptr;
  typedef unsigned char   uns8,   PV_PTR_DECL  uns8_ptr;
  typedef short int16,            PV_PTR_DECL  int16_ptr;
  typedef unsigned short  uns16,  PV_PTR_DECL  uns16_ptr;
  typedef long            int32,  PV_PTR_DECL  int32_ptr;
  typedef unsigned long   uns32,  PV_PTR_DECL  uns32_ptr;
  typedef double          flt64,  PV_PTR_DECL  flt64_ptr;
  #define BIG_ENDIAN    FALSE /* TRUE for Motorola byte order, FALSE for Intel */
#elif defined(LINUX)
  #if !defined BOOLEAN         /* some vendors, like ISee define their own bool*/
    typedef int           boolean;
    #define BOOLEAN
  #endif
  typedef signed char     int8,   PV_PTR_DECL  int8_ptr;
  typedef unsigned char   uns8,   PV_PTR_DECL  uns8_ptr;
  typedef short int16,            PV_PTR_DECL  int16_ptr;
  typedef unsigned short  uns16,  PV_PTR_DECL  uns16_ptr;
  typedef int             int32,  PV_PTR_DECL  int32_ptr;
  typedef unsigned int    uns32,  PV_PTR_DECL  uns32_ptr;
  typedef double          flt64,  PV_PTR_DECL  flt64_ptr;
  #define BIG_ENDIAN    FALSE /* TRUE for Motorola byte order, FALSE for Intel */
#elif defined __mac_os_x
  #ifndef BOOLEAN
    #define BOOLEAN
    typedef int           boolean;
  #endif
  //typedef signed char        int8;   
  typedef signed char        PV_PTR_DECL  int8_ptr;
  typedef unsigned char      uns8,   PV_PTR_DECL  uns8_ptr;
  //typedef signed short int   int16;
  typedef signed short int   PV_PTR_DECL  int16_ptr;
  typedef unsigned short int uns16,  PV_PTR_DECL  uns16_ptr;
  //typedef signed long int    int32;
  typedef signed long int    PV_PTR_DECL  int32_ptr;
  typedef unsigned long int  uns32,  PV_PTR_DECL  uns32_ptr;
  typedef double             flt64,  PV_PTR_DECL  flt64_ptr;
  /* #define BIG_ENDIAN    TRUE /* TRUE for Motorola byte order, FALSE for Intel */
#else
  #error OS Not supported
#endif

enum { PV_FAIL, PV_OK };

typedef boolean                 PV_PTR_DECL  boolean_ptr;
typedef char                    PV_PTR_DECL  char_ptr;
typedef const char              PV_PTR_DECL  char_const_ptr;
typedef void                    PV_BUFP_DECL void_ptr;
typedef void_ptr                PV_BUFP_DECL void_ptr_ptr;

/****************************** PVCAM Constants ******************************/
#ifndef FALSE
  #define FALSE  PV_FAIL      /* FALSE == 0                                  */
#endif

#ifndef TRUE
  #define TRUE   PV_OK        /* TRUE  == 1                                  */
#endif

#define CAM_NAME_LEN     32 /* Max length of a cam name (includes null term) */

/************************ PVCAM-Specific Definitions *************************/
#define MAX_CAM          16 /* Maximum number of cameras on this system.     */

#endif /* _MASTER_H */
