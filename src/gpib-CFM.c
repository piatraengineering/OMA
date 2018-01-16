#include "ni488.h"
#include <stdio.h>
#include <CoreFoundation/CFBundle.h>


/*
 *    Here are the user's global variables...
 */
int  ibsta  = 0;
int  iberr  = 0;
int  ibcnt  = 0;
long ibcntl = 0;


/*
 *    This is private data for the language interface only so it is
 *    defined as 'static'.
 */
static CFBundleRef Gpib32Lib = NULL;

/****************************************
 *  Functions that Load and Free GPIB DLL
 ****************************************/

static void * LoadFunction(char * FunctionName)
{
   /*
    *    First check to see if we have a handle to the GPIB DLL.
    */
   if (Gpib32Lib == NULL)  {
      CFBundleRef GpibLibHandle;
      CFStringRef tempStr;
      CFURLRef bundleURL;
      /*
       *    No, we don't have handle so call LoadLibrary.  Save the handle
       *    into a local variable and only set the global 'Gpib32Lib' after
       *    we have everything setup.  This should help in case one process
       *    tries to execute this function from multiple threads at the
       *    same time.
       */

      tempStr = CFStringCreateWithCString(NULL, "/Library/Frameworks/NI488.framework", 0);
      bundleURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, tempStr,   
                                             kCFURLPOSIXPathStyle, true);
      CFRelease( tempStr );
 
      GpibLibHandle = CFBundleCreate(kCFAllocatorDefault, bundleURL);
   
      CFRelease( bundleURL );
   
      if (GpibLibHandle && (CFBundleLoadExecutable(GpibLibHandle)))  {
         Gpib32Lib = GpibLibHandle;
      }
      else {
         /*
          *    The LoadLibrary call failed, return with an error.
          */
         Gpib32Lib = NULL;
         ibsta = ERR;
         iberr = EDVR;
         ibcnt = 0;
         ibcntl = ibcnt;
         return NULL;
      }
   }

   /*
    *    OK, the GPIB library is loaded.  Let's get a pointer to the
    *    requested function.  If the GetFunctionPointerForName call fails,
    *    then return with an error.
    */
   {
      void *FPtr;
      CFStringRef funcStr = CFStringCreateWithCString(NULL, FunctionName,0);
      FPtr = CFBundleGetFunctionPointerForName(Gpib32Lib, funcStr);
      if (FPtr == NULL)  {
         ibsta = ERR;
         iberr = EDVR;
         ibcnt = 0;
         ibcntl = ibcnt;
      }
      CFRelease(funcStr);
      return FPtr;
   }

}  /* end of LoadFunction */


/*******************************************
 ***********  NI 488 FUNCTIONS  ************
 *******************************************/

#define MacroCopyGlobals();               \
   ibsta = (int)ThreadIbsta();            \
   iberr = (int)ThreadIberr();            \
   ibcnt = (int)ThreadIbcnt();            \
   ibcntl = (long)ThreadIbcntl();               

//
// FPtr declarations - kept external to the functions so that they can be set to NULL
// by the language interface variant that has the "expert" function to force the GPIB
// DLL to unload.
//
typedef int (*IBASK_PROC) (int ud, int option, int * v);
static IBASK_PROC IBASK_FPtr = NULL;
typedef int (*IBBNAA_PROC) (int ud, char * udname);
static IBBNAA_PROC IBBNAA_FPtr = NULL;
typedef int (*IBBNAW_PROC) (int ud, char * udname);
static IBBNAW_PROC IBBNAW_FPtr = NULL;
typedef int (*IBCAC_PROC) (int ud, int v);
static IBCAC_PROC IBCAC_FPtr = NULL;
typedef int (*IBCLR_PROC) (int ud);
static IBCLR_PROC IBCLR_FPtr = NULL;
typedef int (*IBCMD_PROC) (int ud, void * buf, long cnt);
static IBCMD_PROC IBCMD_FPtr = NULL;
typedef int (*IBCMDA_PROC) (int ud, void * buf, long cnt);
static IBCMDA_PROC IBCMDA_FPtr = NULL;
typedef int (*IBCONFIG_PROC) (int ud, int option, int v);
static IBCONFIG_PROC IBCONFIG_FPtr = NULL;
typedef int (*IBDEV_PROC) (int ud, int pad, int sad, int tmo, int eot, int eos);
static IBDEV_PROC IBDEV_FPtr = NULL;
typedef int (*IBDIAG_PROC) (int ud, void * buf, long cnt);
static IBDIAG_PROC IBDIAG_FPtr = NULL;
typedef int (*IBDMA_PROC) (int ud, int v);
static IBDMA_PROC IBDMA_FPtr = NULL;
typedef int (*IBEOS_PROC) (int ud, int v);
static IBEOS_PROC IBEOS_FPtr = NULL;
typedef int (*IBEOT_PROC) (int ud, int v);
static IBEOT_PROC IBEOT_FPtr = NULL;

/* ibevent needed for ECAP testing. */
typedef int (*IBEVENT_PROC) (int ud, short * result);
static IBEVENT_PROC IBEVENT_FPtr = NULL;

typedef int (*IBEVENTX_PROC) (int ud, int * result);
static IBEVENTX_PROC IBEVENTX_FPtr = NULL;
typedef int (*IBEXPERT_PROC) (int ud, int option, void * Input, void * Output);
static IBEXPERT_PROC IBEXPERT_FPtr = NULL;
typedef int (*IBFINDA_PROC) (char * udname);
static IBFINDA_PROC IBFINDA_FPtr = NULL;
typedef int (*IBFINDW_PROC) (char * udname);
static IBFINDW_PROC IBFINDW_FPtr = NULL;
typedef int (*IBGTS_PROC) (int ud, int v);
static IBGTS_PROC IBGTS_FPtr = NULL;
typedef int (*IBIST_PROC) (int ud, int v);
static IBIST_PROC IBIST_FPtr = NULL;
typedef int (*IBLCK_PROC) (int ud, int v, unsigned int LockWaitTime, void * Reserved);
static IBLCK_PROC IBLCK_FPtr = NULL;
typedef int (*IBLINES_PROC) (int ud, short * result);
static IBLINES_PROC IBLINES_FPtr = NULL;
typedef int (*IBLN_PROC) (int ud, int pad, int sad, short * listen);
static IBLN_PROC IBLN_FPtr = NULL;
typedef int (*IBLOC_PROC) (int ud);
static IBLOC_PROC IBLOC_FPtr = NULL;
typedef int (*IBNOTIFY_PROC) (int ud, int mask, GpibNotifyCallback_t Callback, void * RefData);
static IBNOTIFY_PROC IBNOTIFY_FPtr = NULL;
typedef int (*IBONL_PROC) (int ud, int v);
static IBONL_PROC IBONL_FPtr = NULL;
typedef int (*IBPAD_PROC) (int ud, int v);
static IBPAD_PROC IBPAD_FPtr = NULL;
typedef int (*IBPCT_PROC) (int ud);
static IBPCT_PROC IBPCT_FPtr = NULL;
typedef int (*IBPOKE_PROC) (int ud, long option, long v);
static IBPOKE_PROC IBPOKE_FPtr = NULL;
typedef int (*IBPPC_PROC) (int ud, int v);
static IBPPC_PROC IBPPC_FPtr = NULL;
typedef int (*IBRD_PROC) (int ud, void * buf, long cnt);
static IBRD_PROC IBRD_FPtr = NULL;
typedef int (*IBRDA_PROC) (int ud, void * buf, long cnt);
static IBRDA_PROC IBRDA_FPtr = NULL;
typedef int (*IBRDFA_PROC) (int ud, char * filename);
static IBRDFA_PROC IBRDFA_FPtr = NULL;
typedef int (*IBRDFW_PROC) (int ud, char * filename);
static IBRDFW_PROC IBRDFW_FPtr = NULL;

/* ibrdkey needed for ECAP testing. */
typedef int (*IBRDKEY_PROC) (int ud, void * buf, int cnt);
static IBRDKEY_PROC IBRDKEY_FPtr = NULL;

typedef int (*IBRPP_PROC) (int ud, char * ppr);
static IBRPP_PROC IBRPP_FPtr = NULL;
typedef int (*IBRSC_PROC) (int ud, int v);
static IBRSC_PROC IBRSC_FPtr = NULL;
typedef int (*IBRSP_PROC) (int ud, char * spr);
static IBRSP_PROC IBRSP_FPtr = NULL;
typedef int (*IBRSV_PROC) (int ud, int v);
static IBRSV_PROC IBRSV_FPtr = NULL;
typedef int (*IBSAD_PROC) (int ud, int v);
static IBSAD_PROC IBSAD_FPtr = NULL;
typedef int (*IBSIC_PROC) (int ud);
static IBSIC_PROC IBSIC_FPtr = NULL;
typedef int (*IBSRE_PROC) (int ud, int v);
static IBSRE_PROC IBSRE_FPtr = NULL;
typedef int (*IBSTOP_PROC) (int ud);
static IBSTOP_PROC IBSTOP_FPtr = NULL;
typedef int (*IBTMO_PROC) (int ud, int v);
static IBTMO_PROC IBTMO_FPtr = NULL;
typedef int (*IBTRG_PROC) (int ud);
static IBTRG_PROC IBTRG_FPtr = NULL;
typedef int (*IBWAIT_PROC) (int ud, int mask);
static IBWAIT_PROC IBWAIT_FPtr = NULL;
typedef int (*IBWRT_PROC) (int ud, void * buf, long cnt);
static IBWRT_PROC IBWRT_FPtr = NULL;
typedef int (*IBWRTA_PROC) (int ud, void * buf, long cnt);
static IBWRTA_PROC IBWRTA_FPtr = NULL;

/* ibwrtkey needed for ECAP testing. */
typedef int (*IBWRTKEY_PROC) (int ud, void * buf, int cnt);
static IBWRTKEY_PROC IBWRTKEY_FPtr = NULL;

typedef int (*IBWRTFA_PROC) (int ud, char * filename);
static IBWRTFA_PROC IBWRTFA_FPtr = NULL;
typedef int (*IBWRTFW_PROC) (int ud, char * filename);
static IBWRTFW_PROC IBWRTFW_FPtr = NULL;
typedef int (*IBLOCKXA_PROC) (int ud, int LockWaitTime, char * LockShareName);
static IBLOCKXA_PROC IBLOCKXA_FPtr = NULL;
typedef int (*IBLOCKXW_PROC) (int ud, int LockWaitTime, char * LockShareName);
static IBLOCKXW_PROC IBLOCKXW_FPtr = NULL;
typedef int (*IBUNLOCKXW_PROC) (int ud);
static IBUNLOCKXW_PROC IBUNLOCKXW_FPtr = NULL;
typedef int (*IBLOCK_PROC) (int ud);
static IBLOCK_PROC IBLOCK_FPtr = NULL;
typedef int (*IBUNLOCK_PROC) (int ud);
static IBUNLOCK_PROC IBUNLOCK_FPtr = NULL;
typedef void (*ALLSPOLL_PROC) (int boardID, Addr4882_t * addrlist, short * results);
static ALLSPOLL_PROC ALLSPOLL_FPtr = NULL;
typedef void (*DEVCLEAR_PROC) (int boardID, Addr4882_t addr);
static DEVCLEAR_PROC DEVCLEAR_FPtr = NULL;
typedef void (*DEVCLEARLIST_PROC) (int boardID, Addr4882_t * addrlist);
static DEVCLEARLIST_PROC DEVCLEARLIST_FPtr = NULL;
typedef void (*ENABLELOCAL_PROC) (int boardID, Addr4882_t * addrlist);
static ENABLELOCAL_PROC ENABLELOCAL_FPtr = NULL;
typedef void (*ENABLEREMOTE_PROC) (int boardID, Addr4882_t * addrlist);
static ENABLEREMOTE_PROC ENABLEREMOTE_FPtr = NULL;
typedef void (*FINDLSTN_PROC) (int boardID, Addr4882_t * addrlist, short * results, int limit);
static FINDLSTN_PROC FINDLSTN_FPtr = NULL;
typedef void (*FINDRQS_PROC) (int boardID, Addr4882_t * addrlist, short * dev_stat);
static FINDRQS_PROC FINDRQS_FPtr = NULL;
typedef void (*PPOLL_PROC) (int boardID, short * result);
static PPOLL_PROC PPOLL_FPtr = NULL;
typedef void (*PPOLLCONFIG_PROC) (int boardID, Addr4882_t addr, int DataLine, int LineSense);
static PPOLLCONFIG_PROC PPOLLCONFIG_FPtr = NULL;
typedef void (*PPOLLUNCONFIG_PROC) (int boardID, Addr4882_t * addrlist);
static PPOLLUNCONFIG_PROC PPOLLUNCONFIG_FPtr = NULL;
typedef void (*PASSCONTROL_PROC) (int boardID, Addr4882_t addr);
static PASSCONTROL_PROC PASSCONTROL_FPtr = NULL;
typedef void (*RCVRESPMSG_PROC) (int boardID, void * buffer, long cnt, int Termination);
static RCVRESPMSG_PROC RCVRESPMSG_FPtr = NULL;
typedef void (*READSTATUSBYTE_PROC) (int boardID, Addr4882_t addr, short * result);
static READSTATUSBYTE_PROC READSTATUSBYTE_FPtr = NULL;
typedef void (*RECEIVE_PROC) (int boardID, Addr4882_t addr, void * buffer, long cnt, int Termination);
static RECEIVE_PROC RECEIVE_FPtr = NULL;
typedef void (*RECEIVESETUP_PROC) (int boardID, Addr4882_t addr);
static RECEIVESETUP_PROC RECEIVESETUP_FPtr = NULL;
typedef void (*RESETSYS_PROC) (int boardID, Addr4882_t * addrlist);
static RESETSYS_PROC RESETSYS_FPtr = NULL;
typedef void (*SEND_PROC) (int boardID, Addr4882_t addr, void * buffer, long cnt, int eot_mode);
static SEND_PROC SEND_FPtr = NULL;
typedef void (*SENDCMDS_PROC) (int boardID, void * buf, long cnt);
static SENDCMDS_PROC SENDCMDS_FPtr = NULL;
typedef void (*SENDDATABYTES_PROC) (int boardID, void * buffer, long cnt, int eot_mode);
static SENDDATABYTES_PROC SENDDATABYTES_FPtr = NULL;
typedef void (*SENDIFC_PROC) (int boardID);
static SENDIFC_PROC SENDIFC_FPtr = NULL;
typedef void (*SENDLLO_PROC) (int boardID);
static SENDLLO_PROC SENDLLO_FPtr = NULL;
typedef void (*SENDLIST_PROC) (int boardID, Addr4882_t * addrlist, void * buffer, long cnt, int eot_mode);
static SENDLIST_PROC SENDLIST_FPtr = NULL;
typedef void (*SENDSETUP_PROC) (int boardID, Addr4882_t * addrlist);
static SENDSETUP_PROC SENDSETUP_FPtr = NULL;
typedef void (*SETRWLS_PROC) (int boardID, Addr4882_t * addrlist);
static SETRWLS_PROC SETRWLS_FPtr = NULL;
typedef void (*TESTSRQ_PROC) (int boardID, short * result);
static TESTSRQ_PROC TESTSRQ_FPtr = NULL;
typedef void (*TESTSYS_PROC) (int boardID, Addr4882_t * addrlist, short * results);
static TESTSYS_PROC TESTSYS_FPtr = NULL;
typedef void (*TRIGGER_PROC) (int boardID, Addr4882_t addr);
static TRIGGER_PROC TRIGGER_FPtr = NULL;
typedef void (*TRIGGERLIST_PROC) (int boardID, Addr4882_t * addrlist);
static TRIGGERLIST_PROC TRIGGERLIST_FPtr = NULL;
typedef void (*WAITSRQ_PROC) (int boardID, short * result);
static WAITSRQ_PROC WAITSRQ_FPtr = NULL;
typedef int  (*THREADIBSTA_PROC) (void);
static THREADIBSTA_PROC THREADIBSTA_FPtr = NULL;
typedef int  (*THREADIBERR_PROC) (void);
static THREADIBERR_PROC THREADIBERR_FPtr = NULL;
typedef int  (*THREADIBCNT_PROC) (void);
static THREADIBCNT_PROC THREADIBCNT_FPtr = NULL;
typedef long (*THREADIBCNTL_PROC) (void);
static THREADIBCNTL_PROC THREADIBCNTL_FPtr = NULL;


int ibask    (int ud, int option, int * v)
{
   int retval;

   if (IBASK_FPtr == NULL)  {
      IBASK_FPtr = (IBASK_PROC)LoadFunction("ibask");
      if (IBASK_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (*IBASK_FPtr) (ud, option, v);
   MacroCopyGlobals();
   return retval;
}
int ibbnaA    (int ud, char * udname)
{
   int retval;

   if (IBBNAA_FPtr == NULL)  {
      IBBNAA_FPtr = (IBBNAA_PROC)LoadFunction("ibbnaA");
      if (IBBNAA_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (*IBBNAA_FPtr) (ud, udname);
   MacroCopyGlobals();
   return retval;
}
int ibbnaW    (int ud, unsigned short *udname)
{
   int retval;

   if (IBBNAW_FPtr == NULL)  {
      IBBNAW_FPtr = (IBBNAW_PROC)LoadFunction("ibbnaW");
      if (IBBNAW_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (*IBBNAW_FPtr) (ud, udname);
   MacroCopyGlobals();
   return retval;
}
int ibcac    (int ud, int v)
{
   int retval;

   if (IBCAC_FPtr == NULL)  {
      IBCAC_FPtr = (IBCAC_PROC)LoadFunction("ibcac");
      if (IBCAC_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBCAC_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibclr    (int ud)
{
   int retval;

   if (IBCLR_FPtr == NULL)  {
      IBCLR_FPtr = (IBCLR_PROC)LoadFunction("ibclr");
      if (IBCLR_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBCLR_FPtr) (ud);
   MacroCopyGlobals();
   return retval;
}
int ibcmd    (int ud, void * buf, long cnt)
{
   int retval;

   if (IBCMD_FPtr == NULL)  {
      IBCMD_FPtr = (IBCMD_PROC)LoadFunction("ibcmd");
      if (IBCMD_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBCMD_FPtr) (ud, buf, cnt);
   MacroCopyGlobals();
   return retval;
}
int ibcmda   (int ud, void * buf, long cnt)
{
   int retval;

   if (IBCMDA_FPtr == NULL)  {
      IBCMDA_FPtr = (IBCMDA_PROC)LoadFunction("ibcmda");
      if (IBCMDA_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBCMDA_FPtr) (ud, buf, cnt);
   MacroCopyGlobals();
   return retval;
}
int ibconfig (int ud, int option, int v)
{
   int retval;

   if (IBCONFIG_FPtr == NULL)  {
      IBCONFIG_FPtr = (IBCONFIG_PROC)LoadFunction("ibconfig");
      if (IBCONFIG_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBCONFIG_FPtr) (ud, option, v);
   MacroCopyGlobals();
   return retval;
}
int ibdev    (int ud, int pad, int sad, int tmo, int eot, int eos)
{
   int retval;

   if (IBDEV_FPtr == NULL)  {
      IBDEV_FPtr = (IBDEV_PROC)LoadFunction("ibdev");
      if (IBDEV_FPtr == NULL)  {
         return -1;
      }
   }
   retval = (*IBDEV_FPtr) (ud, pad, sad, tmo, eot, eos);
   MacroCopyGlobals();
   return retval;
}
int ibdiag   (int ud, void * buf, long cnt)
{
   int retval;

   if (IBDIAG_FPtr == NULL)  {
      IBDIAG_FPtr = (IBDIAG_PROC)LoadFunction("ibdiag");
      if (IBDIAG_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBDIAG_FPtr) (ud, buf, cnt);
   MacroCopyGlobals();
   return retval;
}
int ibdma    (int ud, int v)
{
   int retval;

   if (IBDMA_FPtr == NULL)  {
      IBDMA_FPtr = (IBDMA_PROC)LoadFunction("ibdma");
      if (IBDMA_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBDMA_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibeos    (int ud, int v)
{
   int retval;

   if (IBEOS_FPtr == NULL)  {
      IBEOS_FPtr = (IBEOS_PROC)LoadFunction("ibeos");
      if (IBEOS_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBEOS_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibeot    (int ud, int v)
{
   int retval;

   if (IBEOT_FPtr == NULL)  {
      IBEOT_FPtr = (IBEOT_PROC)LoadFunction("ibeot");
      if (IBEOT_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBEOT_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibexpert (int ud, int option, void * Input, void * Output)
{
   int retval;

   if (IBEXPERT_FPtr == NULL)  {
      IBEXPERT_FPtr = (IBEXPERT_PROC)LoadFunction("ibexpert");
      if (IBEXPERT_FPtr == NULL)  {
         return -1;
      }
   }
   retval = (*IBEXPERT_FPtr) (ud, option, Input, Output);
   MacroCopyGlobals();

   return retval;
}
int ibfindA   (char * udname)
{
   int retval;

   if (IBFINDA_FPtr == NULL)  {
      IBFINDA_FPtr = (IBFINDA_PROC)LoadFunction("ibfindA");
      if (IBFINDA_FPtr == NULL)  {
         return -1;
      }
   }
   retval = (*IBFINDA_FPtr) (udname);
   MacroCopyGlobals();

   return retval;
}
int ibfindW   (unsigned short *udname)
{
   int retval;

   if (IBFINDW_FPtr == NULL)  {
      IBFINDW_FPtr = (IBFINDW_PROC)LoadFunction("ibfindW");
      if (IBFINDW_FPtr == NULL)  {
         return -1;
      }
   }
   retval = (*IBFINDW_FPtr) (udname);
   MacroCopyGlobals();
   return retval;
}
int ibgts    (int ud, int v)
{
   int retval;

   if (IBGTS_FPtr == NULL)  {
      IBGTS_FPtr = (IBGTS_PROC)LoadFunction("ibgts");
      if (IBGTS_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBGTS_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibist    (int ud, int v)
{
   int retval;

   if (IBIST_FPtr == NULL)  {
      IBIST_FPtr = (IBIST_PROC)LoadFunction("ibist");
      if (IBIST_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBIST_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int iblck (int ud, int v, unsigned int LockWaitTime, void * Reserved)
{
   int retval;

   if (IBLCK_FPtr == NULL)  {
      IBLCK_FPtr = (IBLCK_PROC)LoadFunction("iblck");
      if (IBLCK_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBLCK_FPtr) (ud, v, LockWaitTime, Reserved);
   MacroCopyGlobals();
   return retval;
}
int iblines  (int ud, short * result)
{
   int retval;

   if (IBLINES_FPtr == NULL)  {
      IBLINES_FPtr = (IBLINES_PROC)LoadFunction("iblines");
      if (IBLINES_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBLINES_FPtr) (ud, result);
   MacroCopyGlobals();
   return retval;
}
int ibln     (int ud, int pad, int sad, short * listen)
{
   int retval;

   if (IBLN_FPtr == NULL)  {
      IBLN_FPtr = (IBLN_PROC)LoadFunction("ibln");
      if (IBLN_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBLN_FPtr) (ud, pad, sad, listen);
   MacroCopyGlobals();
   return retval;
}
int ibloc    (int ud)
{
   int retval;

   if (IBLOC_FPtr == NULL)  {
      IBLOC_FPtr = (IBLOC_PROC)LoadFunction("ibloc");
      if (IBLOC_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBLOC_FPtr) (ud);
   MacroCopyGlobals();
   return retval;
}
int ibnotify (int ud, int mask, GpibNotifyCallback_t Callback, void * RefData)
{
   int retval;

   if (IBNOTIFY_FPtr == NULL)  {
      IBNOTIFY_FPtr = (IBNOTIFY_PROC)LoadFunction("ibnotify");
      if (IBNOTIFY_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBNOTIFY_FPtr) (ud, mask, Callback, RefData);
   MacroCopyGlobals();
   return retval;
}
int ibonl    (int ud, int v)
{
   int retval;

   if (IBONL_FPtr == NULL)  {
      IBONL_FPtr = (IBONL_PROC)LoadFunction("ibonl");
      if (IBONL_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBONL_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibpad    (int ud, int v)
{
   int retval;

   if (IBPAD_FPtr == NULL)  {
      IBPAD_FPtr = (IBPAD_PROC)LoadFunction("ibpad");
      if (IBPAD_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBPAD_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibpct    (int ud)
{
   int retval;

   if (IBPCT_FPtr == NULL)  {
      IBPCT_FPtr = (IBPCT_PROC)LoadFunction("ibpct");
      if (IBPCT_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBPCT_FPtr) (ud);
   MacroCopyGlobals();
   return retval;
}
int ibpoke   (int ud, long option, long v)
{
   int retval;

   if (IBPOKE_FPtr == NULL)  {
      IBPOKE_FPtr = (IBPOKE_PROC)LoadFunction("ibpoke");
      if (IBPOKE_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBPOKE_FPtr) (ud, option, v);
   MacroCopyGlobals();
   return retval;
}
int ibppc    (int ud, int v)
{
   int retval;

   if (IBPPC_FPtr == NULL)  {
      IBPPC_FPtr = (IBPPC_PROC)LoadFunction("ibppc");
      if (IBPPC_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBPPC_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibrd     (int ud, void * buf, long cnt)
{
   int retval;

   if (IBRD_FPtr == NULL)  {
      IBRD_FPtr = (IBRD_PROC)LoadFunction("ibrd");
      if (IBRD_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBRD_FPtr) (ud, buf, cnt);
   MacroCopyGlobals();
   return retval;
}
int ibrda    (int ud, void * buf, long cnt)
{
   int retval;

   if (IBRDA_FPtr == NULL)  {
      IBRDA_FPtr = (IBRDA_PROC)LoadFunction("ibrda");
      if (IBRDA_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBRDA_FPtr) (ud, buf, cnt);
   MacroCopyGlobals();
   return retval;
}
int ibrdfA    (int ud, char * filename)
{
   int retval;

   if (IBRDFA_FPtr == NULL)  {
      IBRDFA_FPtr = (IBRDFA_PROC)LoadFunction("ibrdfA");
      if (IBRDFA_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBRDFA_FPtr) (ud, filename);
   MacroCopyGlobals();
   return retval;
}
int ibrdfW    (int ud, unsigned short *filename)
{
   int retval;

   if (IBRDFW_FPtr == NULL)  {
      IBRDFW_FPtr = (IBRDFW_PROC)LoadFunction("ibrdfW");
      if (IBRDFW_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBRDFW_FPtr) (ud, filename);
   MacroCopyGlobals();
   return retval;
}
int ibrpp    (int ud, char * ppr)
{
   int retval;

   if (IBRPP_FPtr == NULL)  {
      IBRPP_FPtr = (IBRPP_PROC)LoadFunction("ibrpp");
      if (IBRPP_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBRPP_FPtr) (ud, ppr);
   MacroCopyGlobals();
   return retval;
}
int ibrsc    (int ud, int v)
{
   int retval;

   if (IBRSC_FPtr == NULL)  {
      IBRSC_FPtr = (IBRSC_PROC)LoadFunction("ibrsc");
      if (IBRSC_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBRSC_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibrsp    (int ud, char * spr)
{
   int retval;

   if (IBRSP_FPtr == NULL)  {
      IBRSP_FPtr = (IBRSP_PROC)LoadFunction("ibrsp");
      if (IBRSP_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBRSP_FPtr) (ud, spr);
   MacroCopyGlobals();
   return retval;
}
int ibrsv    (int ud, int v)
{
   int retval;

   if (IBRSV_FPtr == NULL)  {
      IBRSV_FPtr = (IBRSV_PROC)LoadFunction("ibrsv");
      if (IBRSV_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBRSV_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibsad    (int ud, int v)
{
   int retval;

   if (IBSAD_FPtr == NULL)  {
      IBSAD_FPtr = (IBSAD_PROC)LoadFunction("ibsad");
      if (IBSAD_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBSAD_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibsic    (int ud)
{
   int retval;

   if (IBSIC_FPtr == NULL)  {
      IBSIC_FPtr = (IBSIC_PROC)LoadFunction("ibsic");
      if (IBSIC_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBSIC_FPtr) (ud);
   MacroCopyGlobals();

   return retval;
}
int ibsre    (int ud, int v)
{
   int retval;

   if (IBSRE_FPtr == NULL)  {
      IBSRE_FPtr = (IBSRE_PROC)LoadFunction("ibsre");
      if (IBSRE_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBSRE_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibstop   (int ud)
{
   int retval;

   if (IBSTOP_FPtr == NULL)  {
      IBSTOP_FPtr = (IBSTOP_PROC)LoadFunction("ibstop");
      if (IBSTOP_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBSTOP_FPtr) (ud);
   MacroCopyGlobals();
   return retval;
}
int ibtmo    (int ud, int v)
{
   int retval;

   if (IBTMO_FPtr == NULL)  {
      IBTMO_FPtr = (IBTMO_PROC)LoadFunction("ibtmo");
      if (IBTMO_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBTMO_FPtr) (ud, v);
   MacroCopyGlobals();
   return retval;
}
int ibtrg    (int ud)
{
   int retval;

   if (IBTRG_FPtr == NULL)  {
      IBTRG_FPtr = (IBTRG_PROC)LoadFunction("ibtrg");
      if (IBTRG_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBTRG_FPtr) (ud);
   MacroCopyGlobals();
   return retval;
}
int ibwait   (int ud, int mask)
{
   int retval;

   if (IBWAIT_FPtr == NULL)  {
      IBWAIT_FPtr = (IBWAIT_PROC)LoadFunction("ibwait");
      if (IBWAIT_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBWAIT_FPtr) (ud, mask);
   MacroCopyGlobals();
   return retval;
}
int ibwrt    (int ud, void * buf, long cnt)
{
   int retval;

   if (IBWRT_FPtr == NULL)  {
      IBWRT_FPtr = (IBWRT_PROC)LoadFunction("ibwrt");
      if (IBWRT_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBWRT_FPtr) (ud, buf, cnt);
   MacroCopyGlobals();
   return retval;
}
int ibwrta   (int ud, void * buf, long cnt)
{
   int retval;

   if (IBWRTA_FPtr == NULL)  {
      IBWRTA_FPtr = (IBWRTA_PROC)LoadFunction("ibwrta");
      if (IBWRTA_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBWRTA_FPtr) (ud, buf, cnt);
   MacroCopyGlobals();
   return retval;
}
int ibwrtfA   (int ud, char * filename)
{
   int retval;

   if (IBWRTFA_FPtr == NULL)  {
      IBWRTFA_FPtr = (IBWRTFA_PROC)LoadFunction("ibwrtfA");
      if (IBWRTFA_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBWRTFA_FPtr) (ud, filename);
   MacroCopyGlobals();
   return retval;
}
int ibwrtfW   (int ud, unsigned short *filename)
{
   int retval;

   if (IBWRTFW_FPtr == NULL)  {
      IBWRTFW_FPtr = (IBWRTFW_PROC)LoadFunction("ibwrtfW");
      if (IBWRTFW_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBWRTFW_FPtr) (ud, filename);
   MacroCopyGlobals();
   return retval;
}
// GPIB locking functions
int iblockxA   (int ud, int LockWaitTime, char * LockShareName)
{
   int retval;

   if (IBLOCKXA_FPtr == NULL)  {
      IBLOCKXA_FPtr = (IBLOCKXA_PROC)LoadFunction("iblockxA");
      if (IBLOCKXA_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBLOCKXA_FPtr) (ud, LockWaitTime, LockShareName);
   MacroCopyGlobals();
   return retval;
}

int iblockxW   (int ud, int LockWaitTime, unsigned short *LockShareName)
{
   int retval;

   if (IBLOCKXW_FPtr == NULL)  {
      IBLOCKXW_FPtr = (IBLOCKXW_PROC)LoadFunction("iblockxW");
      if (IBLOCKXW_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBLOCKXW_FPtr) (ud, LockWaitTime, LockShareName);
   MacroCopyGlobals();
   return retval;
}

int ibunlockx    (int ud)
{
   int retval;

   if (IBUNLOCKXW_FPtr == NULL)  {
      IBUNLOCKXW_FPtr = (IBUNLOCKXW_PROC)LoadFunction("ibunlockx");
      if (IBUNLOCKXW_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBUNLOCKXW_FPtr) (ud);
   MacroCopyGlobals();
   return retval;
}

// GPIB-ENET locking functions
int iblock   (int ud)
{
   int retval;

   if (IBLOCK_FPtr == NULL)  {
      IBLOCK_FPtr = (IBLOCK_PROC)LoadFunction("iblock");
      if (IBLOCK_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBLOCK_FPtr) (ud);
   MacroCopyGlobals();
   return retval;
}

int ibunlock (int ud)
{
   int retval;

   if (IBUNLOCK_FPtr == NULL)  {
      IBUNLOCK_FPtr = (IBUNLOCK_PROC)LoadFunction("ibunlock");
      if (IBUNLOCK_FPtr == NULL)  {
         return ibsta;
      }
   }
   retval = (int)(*IBUNLOCK_FPtr) (ud);
   MacroCopyGlobals();
   return retval;
}


//  NI-488.2 routines
void AllSpoll      (int boardID, Addr4882_t * addrlist, short * results)
{

   if (ALLSPOLL_FPtr == NULL)  {
      ALLSPOLL_FPtr = (ALLSPOLL_PROC)LoadFunction("AllSpoll");
      if (ALLSPOLL_FPtr == NULL)  {
         return;
      }
   }
   (*ALLSPOLL_FPtr) (boardID, addrlist, results);
   MacroCopyGlobals();
   return;
}
void DevClear      (int boardID, Addr4882_t addr)
{

   if (DEVCLEAR_FPtr == NULL)  {
      DEVCLEAR_FPtr = (DEVCLEAR_PROC)LoadFunction("DevClear");
      if (DEVCLEAR_FPtr == NULL)  {
         return;
      }
   }
   (*DEVCLEAR_FPtr) (boardID, addr);
   MacroCopyGlobals();
   return;
}
void DevClearList  (int boardID, Addr4882_t * addrlist)
{

   if (DEVCLEARLIST_FPtr == NULL)  {
      DEVCLEARLIST_FPtr = (DEVCLEARLIST_PROC)LoadFunction("DevClearList");
      if (DEVCLEARLIST_FPtr == NULL)  {
         return;
      }
   }
   (*DEVCLEARLIST_FPtr) (boardID, addrlist);
   MacroCopyGlobals();
   return;
}
void EnableLocal   (int boardID, Addr4882_t * addrlist)
{

   if (ENABLELOCAL_FPtr == NULL)  {
      ENABLELOCAL_FPtr = (ENABLELOCAL_PROC)LoadFunction("EnableLocal");
      if (ENABLELOCAL_FPtr == NULL)  {
         return;
      }
   }
   (*ENABLELOCAL_FPtr) (boardID, addrlist);
   MacroCopyGlobals();
   return;
}
void EnableRemote  (int boardID, Addr4882_t * addrlist)
{

   if (ENABLEREMOTE_FPtr == NULL)  {
      ENABLEREMOTE_FPtr = (ENABLEREMOTE_PROC)LoadFunction("EnableRemote");
      if (ENABLEREMOTE_FPtr == NULL)  {
         return;
      }
   }
   (*ENABLEREMOTE_FPtr) (boardID, addrlist);
   MacroCopyGlobals();
   return;
}
void FindLstn      (int boardID, Addr4882_t * addrlist, short * results, int limit)
{

   if (FINDLSTN_FPtr == NULL)  {
      FINDLSTN_FPtr = (FINDLSTN_PROC)LoadFunction("FindLstn");
      if (FINDLSTN_FPtr == NULL)  {
         return;
      }
   }
   (*FINDLSTN_FPtr) (boardID, addrlist, results, limit);
   MacroCopyGlobals();
   return;
}
void FindRQS       (int boardID, Addr4882_t * addrlist, short * dev_stat)
{

   if (FINDRQS_FPtr == NULL)  {
      FINDRQS_FPtr = (FINDRQS_PROC)LoadFunction("FindRQS");
      if (FINDRQS_FPtr == NULL)  {
         return;
      }
   }
   (*FINDRQS_FPtr) (boardID, addrlist, dev_stat);
   MacroCopyGlobals();
   return;
}
void PPoll         (int boardID, short * result)
{

   if (PPOLL_FPtr == NULL)  {
      PPOLL_FPtr = (PPOLL_PROC)LoadFunction("PPoll");
      if (PPOLL_FPtr == NULL)  {
         return;
      }
   }
   (*PPOLL_FPtr) (boardID, result);
   MacroCopyGlobals();
   return;
}
void PPollConfig   (int boardID, Addr4882_t addr, int DataLine, int LineSense)
{

   if (PPOLLCONFIG_FPtr == NULL)  {
      PPOLLCONFIG_FPtr = (PPOLLCONFIG_PROC)LoadFunction("PPollConfig");
      if (PPOLLCONFIG_FPtr == NULL)  {
         return;
      }
   }
   (*PPOLLCONFIG_FPtr) (boardID, addr, DataLine, LineSense);
   MacroCopyGlobals();
   return;
}
void PPollUnconfig (int boardID, Addr4882_t * addrlist)
{

   if (PPOLLUNCONFIG_FPtr == NULL)  {
      PPOLLUNCONFIG_FPtr = (PPOLLUNCONFIG_PROC)LoadFunction("PPollUnconfig");
      if (PPOLLUNCONFIG_FPtr == NULL)  {
         return;
      }
   }
   (*PPOLLUNCONFIG_FPtr) (boardID, addrlist);
   MacroCopyGlobals();
   return;
}
void PassControl   (int boardID, Addr4882_t addr)
{

   if (PASSCONTROL_FPtr == NULL)  {
      PASSCONTROL_FPtr = (PASSCONTROL_PROC)LoadFunction("PassControl");
      if (PASSCONTROL_FPtr == NULL)  {
         return;
      }
   }
   (*PASSCONTROL_FPtr) (boardID, addr);
   MacroCopyGlobals();
   return;
}
void RcvRespMsg    (int boardID, void * buffer, long cnt, int Termination)
{

   if (RCVRESPMSG_FPtr == NULL)  {
      RCVRESPMSG_FPtr = (RCVRESPMSG_PROC)LoadFunction("RcvRespMsg");
      if (RCVRESPMSG_FPtr == NULL)  {
         return;
      }
   }
   (*RCVRESPMSG_FPtr) (boardID, buffer, cnt, Termination);
   MacroCopyGlobals();
   return;
}
void ReadStatusByte (int boardID, Addr4882_t addr, short * result)
{

   if (READSTATUSBYTE_FPtr == NULL)  {
      READSTATUSBYTE_FPtr = (READSTATUSBYTE_PROC)LoadFunction("ReadStatusByte");
      if (READSTATUSBYTE_FPtr == NULL)  {
         return;
      }
   }
   (*READSTATUSBYTE_FPtr) (boardID, addr, result);
   MacroCopyGlobals();
   return;
}
void Receive (int boardID, Addr4882_t addr, void * buffer, long cnt, int Termination)
{

   if (RECEIVE_FPtr == NULL)  {
      RECEIVE_FPtr = (RECEIVE_PROC)LoadFunction("Receive");
      if (RECEIVE_FPtr == NULL)  {
         return;
      }
   }
   (*RECEIVE_FPtr) (boardID, addr, buffer, cnt, Termination);
   MacroCopyGlobals();
   return;
}
void ReceiveSetup  (int boardID, Addr4882_t addr)
{

   if (RECEIVESETUP_FPtr == NULL)  {
      RECEIVESETUP_FPtr = (RECEIVESETUP_PROC)LoadFunction("ReceiveSetup");
      if (RECEIVESETUP_FPtr == NULL)  {
         return;
      }
   }
   (*RECEIVESETUP_FPtr) (boardID, addr);
   MacroCopyGlobals();
   return;
}
void ResetSys      (int boardID, Addr4882_t * addrlist)
{

   if (RESETSYS_FPtr == NULL)  {
      RESETSYS_FPtr = (RESETSYS_PROC)LoadFunction("ResetSys");
      if (RESETSYS_FPtr == NULL)  {
         return;
      }
   }
   (*RESETSYS_FPtr) (boardID, addrlist);
   MacroCopyGlobals();
   return;
}
void Send          (int boardID, Addr4882_t addr, void * buffer, long cnt, int eot_mode)
{

   if (SEND_FPtr == NULL)  {
      SEND_FPtr = (SEND_PROC)LoadFunction("Send");
      if (SEND_FPtr == NULL)  {
         return;
      }
   }
   (*SEND_FPtr) (boardID, addr, buffer, cnt, eot_mode);
   MacroCopyGlobals();
   return;
}
void SendCmds      (int boardID, void * buf, long cnt)
{

   if (SENDCMDS_FPtr == NULL)  {
      SENDCMDS_FPtr = (SENDCMDS_PROC)LoadFunction("SendCmds");
      if (SENDCMDS_FPtr == NULL)  {
         return;
      }
   }
   (*SENDCMDS_FPtr) (boardID, buf, cnt);
   MacroCopyGlobals();
   return;
}
void SendDataBytes (int boardID, void * buffer, long cnt, int eot_mode)
{

   if (SENDDATABYTES_FPtr == NULL)  {
      SENDDATABYTES_FPtr = (SENDDATABYTES_PROC)LoadFunction("SendDataBytes");
      if (SENDDATABYTES_FPtr == NULL)  {
         return;
      }
   }
   (*SENDDATABYTES_FPtr) (boardID, buffer, cnt, eot_mode);
   MacroCopyGlobals();
   return;
}
void SendIFC       (int boardID)
{

   if (SENDIFC_FPtr == NULL)  {
      SENDIFC_FPtr = (SENDIFC_PROC)LoadFunction("SendIFC");
      if (SENDIFC_FPtr == NULL)  {
         return;
      }
   }
   (*SENDIFC_FPtr) (boardID);
   MacroCopyGlobals();
   return;
}
void SendLLO       (int boardID)
{

   if (SENDLLO_FPtr == NULL)  {
      SENDLLO_FPtr = (SENDLLO_PROC)LoadFunction("SendLLO");
      if (SENDLLO_FPtr == NULL)  {
         return;
      }
   }
   (*SENDLLO_FPtr) (boardID);
   MacroCopyGlobals();
   return;
}
void SendList      (int boardID, Addr4882_t * addrlist, void * buffer, long cnt, int eot_mode)
{

   if (SENDLIST_FPtr == NULL)  {
      SENDLIST_FPtr = (SENDLIST_PROC)LoadFunction("SendList");
      if (SENDLIST_FPtr == NULL)  {
         return;
      }
   }
   (*SENDLIST_FPtr) (boardID, addrlist, buffer, cnt, eot_mode);
   MacroCopyGlobals();
   return;
}
void SendSetup     (int boardID, Addr4882_t * addrlist)
{

   if (SENDSETUP_FPtr == NULL)  {
      SENDSETUP_FPtr = (SENDSETUP_PROC)LoadFunction("SendSetup");
      if (SENDSETUP_FPtr == NULL)  {
         return;
      }
   }
   (*SENDSETUP_FPtr) (boardID, addrlist);
   MacroCopyGlobals();
   return;
}
void SetRWLS       (int boardID, Addr4882_t * addrlist)
{

   if (SETRWLS_FPtr == NULL)  {
      SETRWLS_FPtr = (SETRWLS_PROC)LoadFunction("SetRWLS");
      if (SETRWLS_FPtr == NULL)  {
         return;
      }
   }
   (*SETRWLS_FPtr) (boardID, addrlist);
   MacroCopyGlobals();
   return;
}
void TestSRQ       (int boardID, short * result)
{

   if (TESTSRQ_FPtr == NULL)  {
      TESTSRQ_FPtr = (TESTSRQ_PROC)LoadFunction("TestSRQ");
      if (TESTSRQ_FPtr == NULL)  {
         return;
      }
   }
   (*TESTSRQ_FPtr) (boardID, result);
   MacroCopyGlobals();
   return;
}
void TestSys       (int boardID, Addr4882_t * addrlist, short * results)
{

   if (TESTSYS_FPtr == NULL)  {
      TESTSYS_FPtr = (TESTSYS_PROC)LoadFunction("TestSys");
      if (TESTSYS_FPtr == NULL)  {
         return;
      }
   }
   (*TESTSYS_FPtr) (boardID, addrlist, results);
   MacroCopyGlobals();
   return;
}
void Trigger       (int boardID, Addr4882_t addr)
{

   if (TRIGGER_FPtr == NULL)  {
      TRIGGER_FPtr = (TRIGGER_PROC)LoadFunction("Trigger");
      if (TRIGGER_FPtr == NULL)  {
         return;
      }
   }
   (*TRIGGER_FPtr) (boardID, addr);
   MacroCopyGlobals();
   return;
}
void TriggerList   (int boardID, Addr4882_t * addrlist)
{

   if (TRIGGERLIST_FPtr == NULL)  {
      TRIGGERLIST_FPtr = (TRIGGERLIST_PROC)LoadFunction("TriggerList");
      if (TRIGGERLIST_FPtr == NULL)  {
         return;
      }
   }
   (*TRIGGERLIST_FPtr) (boardID, addrlist);
   MacroCopyGlobals();
   return;
}
void WaitSRQ       (int boardID, short * result)
{

   if (WAITSRQ_FPtr == NULL)  {
      WAITSRQ_FPtr = (WAITSRQ_PROC)LoadFunction("WaitSRQ");
      if (WAITSRQ_FPtr == NULL)  {
         return;
      }
   }
   (*WAITSRQ_FPtr) (boardID, result);
   MacroCopyGlobals();
   return;
}


//
// Functions which provide access to thread-specific global variables
//
int  ThreadIbsta (void)
{

   if (THREADIBSTA_FPtr == NULL)  {
      THREADIBSTA_FPtr = (THREADIBSTA_PROC)LoadFunction("ThreadIbsta");
      if (THREADIBSTA_FPtr == NULL)  {
         return 0xDDDDDEAD;
      }
   }
   return (*THREADIBSTA_FPtr) ();
}
int  ThreadIberr (void)
{

   if (THREADIBERR_FPtr == NULL)  {
      THREADIBERR_FPtr = (THREADIBERR_PROC)LoadFunction("ThreadIberr");
      if (THREADIBERR_FPtr == NULL)  {
         return 0xDDDDDEAD;
      }
   }
   return (*THREADIBERR_FPtr) ();
}
int  ThreadIbcnt (void)
{

   if (THREADIBCNT_FPtr == NULL)  {
      THREADIBCNT_FPtr = (THREADIBCNT_PROC)LoadFunction("ThreadIbcnt");
      if (THREADIBCNT_FPtr == NULL)  {
         return 0xDDDDDEAD;
      }
   }
   return (*THREADIBCNT_FPtr) ();
}
long ThreadIbcntl (void)
{

   if (THREADIBCNTL_FPtr == NULL)  {
      THREADIBCNTL_FPtr = (THREADIBCNTL_PROC)LoadFunction("ThreadIbcntl");
      if (THREADIBCNTL_FPtr == NULL)  {
         return 0xDDDDDEAD;
      }
   }
   return (*THREADIBCNTL_FPtr) ();
}

static void FreeGpibLibrary (void)
{
   if (Gpib32Lib)  {
      //
      // Set all of the function pointers to NULL.
      //
      IBASK_FPtr = NULL;
      IBBNAW_FPtr = NULL;
      IBCAC_FPtr = NULL;
      IBCLR_FPtr = NULL;
      IBCMD_FPtr = NULL;
      IBCMDA_FPtr = NULL;
      IBCONFIG_FPtr = NULL;
      IBDEV_FPtr = NULL;
      IBDIAG_FPtr = NULL;
      IBDMA_FPtr = NULL;
      IBEOS_FPtr = NULL;
      IBEOT_FPtr = NULL;
      IBEVENTX_FPtr = NULL;
      IBFINDA_FPtr = NULL;
      IBFINDW_FPtr = NULL;
      IBGTS_FPtr = NULL;
      IBIST_FPtr = NULL;
      IBLINES_FPtr = NULL;
      IBLN_FPtr = NULL;
      IBLOC_FPtr = NULL;
      IBNOTIFY_FPtr = NULL;
      IBONL_FPtr = NULL;
      IBPAD_FPtr = NULL;
      IBPCT_FPtr = NULL;
      IBPOKE_FPtr = NULL;
      IBPPC_FPtr = NULL;
      IBRD_FPtr = NULL;
      IBRDA_FPtr = NULL;
      IBRDFA_FPtr = NULL;
      IBRDFW_FPtr = NULL;
      IBRPP_FPtr = NULL;
      IBRSC_FPtr = NULL;
      IBRSP_FPtr = NULL;
      IBRSV_FPtr = NULL;
      IBSAD_FPtr = NULL;
      IBSIC_FPtr = NULL;
      IBSRE_FPtr = NULL;
      IBSTOP_FPtr = NULL;
      IBTMO_FPtr = NULL;
      IBTRG_FPtr = NULL;
      IBWAIT_FPtr = NULL;
      IBWRT_FPtr = NULL;
      IBWRTA_FPtr = NULL;
      IBWRTFA_FPtr = NULL;
      IBWRTFW_FPtr = NULL;
      IBLOCKXA_FPtr = NULL;
      IBLOCKXW_FPtr = NULL;
      IBUNLOCKXW_FPtr = NULL;
      IBLOCK_FPtr = NULL;
      IBUNLOCK_FPtr = NULL;
      ALLSPOLL_FPtr = NULL;
      DEVCLEAR_FPtr = NULL;
      DEVCLEARLIST_FPtr = NULL;
      ENABLELOCAL_FPtr = NULL;
      ENABLEREMOTE_FPtr = NULL;
      FINDLSTN_FPtr = NULL;
      FINDRQS_FPtr = NULL;
      PPOLL_FPtr = NULL;
      PPOLLCONFIG_FPtr = NULL;
      PPOLLUNCONFIG_FPtr = NULL;
      PASSCONTROL_FPtr = NULL;
      RCVRESPMSG_FPtr = NULL;
      READSTATUSBYTE_FPtr = NULL;
      RECEIVE_FPtr = NULL;
      RECEIVESETUP_FPtr = NULL;
      RESETSYS_FPtr = NULL;
      SEND_FPtr = NULL;
      SENDCMDS_FPtr = NULL;
      SENDDATABYTES_FPtr = NULL;
      SENDIFC_FPtr = NULL;
      SENDLLO_FPtr = NULL;
      SENDLIST_FPtr = NULL;
      SENDSETUP_FPtr = NULL;
      SETRWLS_FPtr = NULL;
      TESTSRQ_FPtr = NULL;
      TESTSYS_FPtr = NULL;
      TRIGGER_FPtr = NULL;
      TRIGGERLIST_FPtr = NULL;
      WAITSRQ_FPtr = NULL;
      THREADIBSTA_FPtr = NULL;
      THREADIBERR_FPtr = NULL;
      THREADIBCNT_FPtr = NULL;
      THREADIBCNTL_FPtr = NULL;

      //
      // Free the GPIB32.DLL
      //
      CFBundleUnloadExecutable(Gpib32Lib);
      CFRelease (Gpib32Lib);      
      Gpib32Lib = NULL;
   }
}

