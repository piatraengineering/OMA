#include "impdefs.h"

#define SET_TEXT(theValue,theItem) 	sprintf(txt,"%d",theValue);\
	CopyCStringToPascal(txt,pstring);\
	GetDialogItem(theDialog, theItem, &itemType, &itemHdl, &itemRect);\
	SetDialogItemText(itemHdl, pstring) 

#define SET_VALUE(theValue,theItem)	GetDialogItem(theDialog, theItem, &itemType, &itemHdl, &itemRect);\
	getdialogitemtext(itemHdl,txt);\
	theValue = atoi(txt)

#define SET_CONTROL_VALUE(theValue,theID,theField) 	\
        text = CFStringCreateWithFormat( NULL, NULL, CFSTR("%d"), theValue );\
        err = SetControlData( theField, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text );\
        CFRelease( text );\
        DrawOneControl( theField );

#define SET_CONTROL_FVALUE(theValue,theID,theField) 	\
        text = CFStringCreateWithFormat( NULL, NULL, CFSTR("%g"), theValue );\
        err = SetControlData( theField, 0,kControlEditTextCFStringTag,sizeof(CFStringRef), &text );\
        CFRelease( text );\
        DrawOneControl( theField );


#define SET_CONTROL_SVALUE(theValue,theID,theField) 	\
		text = CFStringCreateWithFormat( NULL, NULL, CFSTR("%s"), theValue );\
		err = SetControlData( theField,0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text );\
        CFRelease( text );\
		DrawOneControl( theField );
    

#define READMODE O_RDONLY  		/* Read file mode. */


/*
 * For the About OMA... DLOG
 */
#define	aboutMeDLOG		128
#define	okButton		1
#define helpButton		2	
#define	authorItem		3               // For setdialogitemtext 
#define	languageItem	4			// For setdialogitemtext 

/*
 * For the No Memory ALRT
 */
#define	nomemoryALRT		130
#define	okButton			1
#define	errorItem			2			/* For setdialogitemtext */

/*
 * For the Line Out Template DLOG
 */

#define	templateDLOG		139
#define	okButton			1
#define lodrawItem			2
#define losumxItem			3
#define losumyItem			4
#define loalignstartItem	5
#define loalignendItem		6
#define lonumboxItem		7
#define loy1Item			14
#define lonumberItem		9
#define lolabelItem			10
#define lox0Item			11
#define loy0Item			12
#define lox1Item			13
#define lonextItem			8
#define losaveItem			15
#define loloadItem			16
#define lopreviousItem		17

/*
 * For the Star1 DLOG
 */

#define	star1DLOG			140
#define	okButton			1
#define starAutoItem		2
#define starTregItem		3
#define starTimeItem		4
#define starGainItem		5

/*
 * For the ST-6 DLOG
 */
#define	st6DLOG				141
#define	okButton			1
#define st6DCSItem			2
#define st6ABGItem			3
#define st6MODEMItem		4
#define st6PRINTERItem		5
#define st6TREGItem			6
#define st6STEMPItem		7
#define st6CTEMPItem		8
#define st6TIMEItem			9
#define st6OFFItem			10

#ifdef SensiCam
/*
 * For the SensiCam DLOG
 */
#include <Sensicam/SensiCamInterface.h> 
//#include "SensiCamInterface.h"
 
#define	SensiCamDLOG		142
#define	okButton                1
#define SensiCamDel1Item	7
#define SensiCamTime1Item	8
#define SensiCamDel2Item	9
#define SensiCamTime2Item	10
#define SensiCamDel3Item	11
#define SensiCamTime3Item	12
#define SensiCamDel4Item	13
#define SensiCamTime4Item	14
#define SensiCamDel5Item	15
#define SensiCamTime5Item	16
#define SensiCamDel6Item	17
#define SensiCamTime6Item	18
#define SensiCamDel7Item	19
#define SensiCamTime7Item	20
#define SensiCamDel8Item	21
#define SensiCamTime8Item	22
#define SensiCamDel9Item	23
#define SensiCamTime9Item	24
#define SensiCamDel10Item	25
#define SensiCamTime10Item	26

#define SensiCamNoneItem	27
#define SensiCamRisingItem	28
#define SensiCamFallingItem	29

#define SensiCamStandardItem	33
#define SensiCamDoubleShortItem	34
#define SensiCamDoubleLongItem	35

#define SensiCamCopyDelayItem	36
#define SensiCamCopyExposureItem	37

#define	SensiCamStatusDLOG		143

#define SensiCamModeItem	2
#define SensiCamTrigItem	3
#define SensiCamTemp1Item	4
#define SensiCamTemp2Item	5

#endif

/************** Set View Dialog (within Set Surface Dialog *******************/

#define XOFF	20
#define YOFF	20
#define XSIZ	345
#define YSIZ	370
#define OFF     20
#define CLOSE	730		// the square root of this is how close the mouse must be to register a hit 
#define TOOLSIZ 18
#define	HCHAR	'_'
#define VCHAR	'|'
#define XCHAR	'+'
#define NULLCHAR ' '
#define CHOFFSET 2

/****************************************************************************/
/****************************************************************************/
// 	Handle nib-based dialogs
/****************************************************************************/
/****************************************************************************/

#define NUMTOOLS	5

#define koma_prefs_ok 128
// PIV controls
#define koma_prefs_piv_siz 129
#define koma_prefs_piv_inc 130
#define koma_prefs_piv_scale 131
#define koma_prefs_piv_plot 132
#define koma_prefs_piv_clip 133
#define koma_prefs_piv_label 134
#define koma_prefs_piv_dir 135
#define koma_prefs_piv_alt 136
// INTEGRATED controls
#define koma_prefs_int_DatMin 141
#define koma_prefs_int_Max 142
#define koma_prefs_int_Min 143
#define koma_prefs_int_StdSize 144
#define koma_prefs_int_Width 145
#define koma_prefs_int_Height 146
#define koma_prefs_int_Int 147
#define koma_prefs_int_IntX 149
// CONTOUR controls
#define koma_prefs_cont_DatMin 91
#define koma_prefs_cont_Max 92
#define koma_prefs_cont_Min 93
#define koma_prefs_cont_StdSize 94
#define koma_prefs_cont_Width 95
#define koma_prefs_cont_Height 96
#define koma_prefs_cont_Color 97
#define koma_prefs_cont_Calc 98
#define koma_prefs_cont_NCtr 100
#define koma_prefs_cont_CValues 101
// HISTOGRAM controls
#define koma_prefs_hist_DatMin 81
#define koma_prefs_hist_Max 82
#define koma_prefs_hist_Min 83
#define koma_prefs_hist_StdSize 84
#define koma_prefs_hist_Width 85
#define koma_prefs_hist_Height 86
#define koma_prefs_hist_Clear 87
// SURFACE controls
#define koma_prefs_surf_Inc 70
#define koma_prefs_surf_DatMin 71
#define koma_prefs_surf_Max 72
#define koma_prefs_surf_Min 73
#define koma_prefs_surf_StdSize 74
#define koma_prefs_surf_Width 75
#define koma_prefs_surf_Height 76
#define koma_prefs_surf_StdPlot 77
#define koma_prefs_surf_White 78
#define koma_prefs_surf_Color 79
// PREFIX controls
#define koma_prefs_prefix_SaveP 201
#define koma_prefs_prefix_GetP 202
#define koma_prefs_prefix_MacP 203
#define koma_prefs_prefix_SetP 204
#define koma_prefs_prefix_SaveS 205
#define koma_prefs_prefix_GetS 206
#define koma_prefs_prefix_MacS 207
#define koma_prefs_prefix_SetS 208
#define koma_prefs_prefix_pal 209

#define koma_prefs_prefix_transparent 217

#define koma_prefs_signature 'Poma'

#define kstatus_signature 'Stat'

#define kstatus_cmin 101
#define kstatus_cmax 102
#define kstatus_dcmn 103
#define kstatus_icmn 104
#define kstatus_dcmx 105
#define kstatus_icmx 106
#define kstatus_autoscale 107
#define kstatus_autoupdate 108

#define kstatus_cmnmxinc 109

#define kstatus_t1 111
#define kstatus_t2 112
#define kstatus_t3 113
#define kstatus_t4 114
#define kstatus_t5 115
#define kstatus_t6 116
#define kstatus_t7 117

#define kstatus_tool 121
#define kstatus_cmni 130

#define kstatus_cbar 135

#define kstatus_rsca 140
#define kstatus_gsca 141
#define kstatus_bsca 142

#define kstatus_rtxt 145
#define kstatus_gtxt 146
#define kstatus_btxt 147

#define kstatus_rgbm 148
#define kstatus_rrgb 149

#define kstatus_rgam 240
#define kstatus_ggam 241
#define kstatus_bgam 242

#define kstatus_rgamtxt 245
#define kstatus_ggamtxt 246
#define kstatus_bgamtxt 247

#define kstatus_rgbgam 248
#define kstatus_rrgbgam 249


#define CONTOUR_TAB 2
#define SURFACE_TAB 3
#define HISTOGRAM_TAB 4
#define INTEGRATED_TAB 5
#define PREFIX_TAB 6

#define		koma_prefs_TAB_ID 	 	50

#define MAXSCROLLVAL	2000
#define MINSCROLLVAL	1

#define kLineout_signature 'Lout'

#define kLineout_OK	100
#define kLineout_total	101
#define kLineout_prev	102
#define kLineout_next	103
#define kLineout_num	104
#define kLineout_label	105
#define kLineout_x0	106
#define kLineout_y0	107
#define kLineout_x1	108
#define kLineout_y1	109
#define kLineout_al_start	110
#define kLineout_al_end	111
#define kLineout_sum_dir	112
#define kLineout_load	113
#define kLineout_save	114
#define kLineout_image	115