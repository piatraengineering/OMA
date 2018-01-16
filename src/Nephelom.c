/*
 NEPHELOM.c -- Photometric Image Processing and Display
 Copyright (C) 2003  by the Developers of OMA
 *
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 
 */
#include "impdefs.h"
#include "custom.h"


// The following is necessary redefinition (from omadialogsx.h) in order to prevent the
// ZeroLink linker from crapping itself when  SET_CONTROL_FVALUE is called
// otherwise OMA will exit with SIGABRT (6).
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

// Declare Window name and Control ID's for Window elements (neph_window)
WindowRef neph_window;

ControlRef N_Nozz;  ControlID N_NozzID ={ 'Pnef', 100 };
ControlRef Nn_FCN;  ControlID Nn_FCNID ={ 'Pnef', 101 };
ControlRef Nn_SFPJ;  ControlID Nn_SFPJID ={ 'Pnef', 102 };

ControlRef N_Particles;  ControlID N_ParticlesID ={ 'Pnef', 200 };
ControlRef Np_5070s;  ControlID Np_5070sID ={ 'Pnef', 201 };
ControlRef Np_6048s;  ControlID Np_6048sID ={ 'Pnef', 202 };
ControlRef Np_ballAH;  ControlID Np_ballAHID ={ 'Pnef', 203 };
ControlRef Np_Talc;  ControlID Np_TalcID ={ 'Pnef', 204 };

ControlRef N_Laser;  ControlID N_LaserID ={ 'Pnef', 300 };
ControlRef Nl_Horiz;  ControlID Nl_HorizID ={ 'Pnef', 301 };
ControlRef Nl_Vert;  ControlID Nl_VertID ={ 'Pnef', 302 };
ControlRef Nl_Dual;  ControlID Nl_DualID ={ 'Pnef', 303 };

ControlRef N_Qa;  ControlID N_QaID ={ 'Pnef', 400 };
ControlRef Na_L;  ControlID Na_LID ={ 'Pnef', 401 };
ControlRef Na_M;  ControlID Na_MID ={ 'Pnef', 402 };
ControlRef Na_H;  ControlID Na_HID ={ 'Pnef', 403 };

ControlRef N_Qc;  ControlID N_QcID ={ 'Pnef', 500 };
ControlRef Nc_0000;  ControlID Nc_0000ID ={ 'Pnef', 501 };
ControlRef Nc_0102;  ControlID Nc_0102ID ={ 'Pnef', 502 };
ControlRef Nc_0205;  ControlID Nc_0205ID ={ 'Pnef', 503 };
ControlRef Nc_0255;  ControlID Nc_0255ID ={ 'Pnef', 504 };
ControlRef Nc_0467;  ControlID Nc_0467ID ={ 'Pnef', 505 };
ControlRef Nc_0540;  ControlID Nc_0540ID ={ 'Pnef', 506 };
ControlRef Nc_0730;  ControlID Nc_0730ID ={ 'Pnef', 507 };
ControlRef Nc_0995;  ControlID Nc_0995ID ={ 'Pnef', 508 };
ControlRef Nc_1350;  ControlID Nc_1350ID ={ 'Pnef', 509 };

ControlRef N_mp;  ControlID N_mpID ={ 'Pnef', 600 };
ControlRef Nm_Min;  ControlID Nm_MinID ={ 'Pnef', 601 };
ControlRef Nm_Max;  ControlID Nm_MaxID ={ 'Pnef', 602 };

ControlRef N_Macro;  ControlID N_MacroID ={ 'Pnef', 700 };

// LOCAL VARIABLES for the Nephelometry Processing Functions

int Use_Nozzle = 1;
int Use_Particles = 0;
int Use_Laser = 0;
int Use_Qa = 0;
int Use_Qc = 0;
int Use_Mp = 0;

int Do_N_FCN = 0;
int Do_N_SFPJ = 0;
int Do_P_5070 = 0;
int Do_P_6048 = 0;
int Do_P_ballAH = 0;
int Do_P_Talc = 0;
int Do_L_H = 0;
int Do_L_V = 0;
int Do_L_D = 0;
int Do_Qa_L = 0;
int Do_Qa_M = 0;
int Do_Qa_H = 0;
int Do_Qc_0000 = 0;
int Do_Qc_0102 = 0;
int Do_Qc_0205 = 0;
int Do_Qc_0255 = 0;
int Do_Qc_0467 = 0;
int Do_Qc_0540 = 0;
int Do_Qc_0730 = 0;
int Do_Qc_0995 = 0;
int Do_Qc_1350 = 0;

char nephmacrobuff[] = "Process1";
float mpminvalue= 0.00;
float mpmaxvalue= 30.00;

int FCN_AH_H_M_0205_0p40, FCN_AH_H_M_0205_17p2, FCN_AH_H_M_0205_1p20, FCN_AH_H_M_0205_2p80, FCN_AH_H_M_0205_4p20, FCN_AH_H_M_0205_5p00, FCN_AH_V_M_0205_0p60, FCN_AH_V_M_0205_1p00, FCN_AH_V_M_0205_30p0, FCN_AH_V_M_0205_3p10, FCN_AH_V_M_0205_4p70, FCN_AH_V_M_0205_5p40, FCN_HH_H_M_0205_0p44, FCN_HH_H_M_0205_2p10, FCN_HH_H_M_0205_3p10, FCN_HH_H_M_0205_4p60, FCN_HH_H_M_0205_8p70, FCN_HH_V_M_0205_0p72, FCN_HH_V_M_0205_2p60, FCN_HH_V_M_0205_2p90, FCN_HH_V_M_0205_3p20, FCN_HH_V_M_0205_5p00, FCN_HH_V_M_0205_5p30, FCN_HH_V_M_0205_5p70, FCN_HH_V_M_0205_6p40, FCN_HH_V_M_0205_7p20, FCN_HL_H_M_0205_0p50, FCN_HL_H_M_0205_0p70, FCN_HL_H_M_0205_0p95, FCN_HL_H_M_0205_1p30, FCN_HL_H_M_0205_1p50, FCN_HL_H_M_0205_1p51, FCN_HL_H_M_0205_1p70, FCN_HL_H_M_0205_2p10, FCN_HL_H_M_0205_3p20, FCN_HL_H_M_0205_3p90, FCN_HL_H_M_0205_8p20, FCN_HL_V_M_0205_0p90, FCN_HL_V_M_0205_1p60, FCN_HL_V_M_0205_3p10, FCN_HL_V_M_0205_4p00, FCN_TP_H_M_0205_10p0, FCN_TP_H_M_0205_2p10, FCN_TP_H_M_0205_5p10, FCN_TP_V_M_0205_10p4, FCN_TP_V_M_0205_2p00, FCN_TP_V_M_0205_4p30;

int SFPJ_AH_D_H_0000_10p7, SFPJ_AH_D_H_0000_4p00, SFPJ_AH_D_H_0000_8p70, SFPJ_AH_D_H_0995_8p00, SFPJ_AH_D_H_1350_16p0, SFPJ_AH_D_H_1350_2p40, SFPJ_AH_D_H_1350_9p00, SFPJ_HH_D_H_0000_2p20, SFPJ_HH_D_H_0000_2p40, SFPJ_HH_D_H_0000_2p60, SFPJ_HH_D_H_0102_2p80, SFPJ_HH_D_H_0255_3p20, SFPJ_HH_D_H_0255_4p80, SFPJ_HH_D_H_0467_3p30, SFPJ_HH_D_H_0540_0p49, SFPJ_HH_D_H_0540_0p70, SFPJ_HH_D_H_0540_0p75, SFPJ_HH_D_H_0540_1p20, SFPJ_HH_D_H_0540_1p50, SFPJ_HH_D_H_0540_2p10, SFPJ_HH_D_H_0540_3p20, SFPJ_HH_D_H_0540_5p50, SFPJ_HH_D_H_0540_8p00, SFPJ_HH_D_H_0730_3p40, SFPJ_HH_D_H_0995_3p50, SFPJ_HH_D_H_1350_0p90, SFPJ_HH_D_H_1350_10p0, SFPJ_HH_D_H_1350_2p00, SFPJ_HH_D_H_1350_2p80, SFPJ_HH_D_H_1350_3p20, SFPJ_HH_D_H_1350_3p30, SFPJ_HH_D_H_1350_4p90, SFPJ_HH_D_L_0730_4p40, SFPJ_HH_D_L_0730_9p60, SFPJ_TP_D_H_0000_2p50, SFPJ_TP_D_H_0000_7p40, SFPJ_TP_D_H_0540_10p9, SFPJ_TP_D_H_0540_7p00, SFPJ_TP_D_H_0995_12p0, SFPJ_TP_D_H_1350_4p00, SFPJ_TP_D_H_1350_8p00;

//***************************************************************************************//
//**    set_Neph_Toggles -                                                                                              **//
//**                 Poke values into the dialog box (neph_window) when first drawn **//
//***************************************************************************************//
int set_Neph_Toggles(){
	CFStringRef text;
	OSStatus err;
	
	// Get a handle to the various window elements
	GetControlByID( neph_window, &N_NozzID, &N_Nozz ); SetControl32BitValue(N_Nozz,Use_Nozzle);
	GetControlByID( neph_window, &Nn_FCNID, &Nn_FCN ); SetControl32BitValue(Nn_FCN,Do_N_FCN);
	GetControlByID( neph_window, &Nn_SFPJID, &Nn_SFPJ ); SetControl32BitValue(Nn_SFPJ,Do_N_SFPJ);
	
	GetControlByID( neph_window, &N_ParticlesID, &N_Particles ); SetControl32BitValue(N_Particles,Use_Particles);
	GetControlByID( neph_window, &Np_5070sID, &Np_5070s ); SetControl32BitValue(Np_5070s,Do_P_5070);
	GetControlByID( neph_window, &Np_6048sID, &Np_6048s ); SetControl32BitValue(Np_6048s,Do_P_6048);
	GetControlByID( neph_window, &Np_ballAHID, &Np_ballAH ); SetControl32BitValue(Np_ballAH,Do_P_ballAH);
	GetControlByID( neph_window, &Np_TalcID, &Np_Talc ); SetControl32BitValue(Np_Talc,Do_P_Talc);
	
	GetControlByID( neph_window, &N_LaserID, &N_Laser ); SetControl32BitValue(N_Laser,Use_Laser);
	GetControlByID( neph_window, &Nl_HorizID, &Nl_Horiz ); SetControl32BitValue(Nl_Horiz,Do_L_H);
	GetControlByID( neph_window, &Nl_VertID, &Nl_Vert ); SetControl32BitValue(Nl_Vert,Do_L_V);
	GetControlByID( neph_window, &Nl_DualID, &Nl_Dual ); SetControl32BitValue(Nl_Dual,Do_L_D);
	
	GetControlByID( neph_window, &N_QaID, &N_Qa ); SetControl32BitValue(N_Qa,Use_Qa);
	GetControlByID( neph_window, &Na_LID, &Na_L ); SetControl32BitValue(Na_L,Do_Qa_L);
	GetControlByID( neph_window, &Na_MID, &Na_M ); SetControl32BitValue(Na_M,Do_Qa_M);
	GetControlByID( neph_window, &Na_HID, &Na_H ); SetControl32BitValue(Na_H,Do_Qa_H);
	
	GetControlByID( neph_window, &N_QcID, &N_Qc ); SetControl32BitValue(N_Qc,Use_Qc);
	GetControlByID( neph_window, &Nc_0000ID, &Nc_0000 ); SetControl32BitValue(Nc_0000,Do_Qc_0000);
	GetControlByID( neph_window, &Nc_0102ID, &Nc_0102 ); SetControl32BitValue(Nc_0102,Do_Qc_0102);
	GetControlByID( neph_window, &Nc_0205ID, &Nc_0205 ); SetControl32BitValue(Nc_0205,Do_Qc_0205);
	GetControlByID( neph_window, &Nc_0255ID, &Nc_0255 ); SetControl32BitValue(Nc_0255,Do_Qc_0255);
	GetControlByID( neph_window, &Nc_0467ID, &Nc_0467 ); SetControl32BitValue(Nc_0467,Do_Qc_0467);
	GetControlByID( neph_window, &Nc_0540ID, &Nc_0540 ); SetControl32BitValue(Nc_0540,Do_Qc_0540);
	GetControlByID( neph_window, &Nc_0730ID, &Nc_0730 ); SetControl32BitValue(Nc_0730,Do_Qc_0730);
	GetControlByID( neph_window, &Nc_0995ID, &Nc_0995 ); SetControl32BitValue(Nc_0995,Do_Qc_0995);
	GetControlByID( neph_window, &Nc_1350ID, &Nc_1350 ); SetControl32BitValue(Nc_1350,Do_Qc_1350);
	
	GetControlByID( neph_window, &N_mpID, &N_mp ); SetControl32BitValue(N_mp,Use_Mp);
	GetControlByID( neph_window, &Nm_MinID, &Nm_Min );SET_CONTROL_FVALUE(mpminvalue,Nm_MinID, Nm_Min);
	GetControlByID( neph_window, &Nm_MaxID, &Nm_Max );SET_CONTROL_FVALUE(mpmaxvalue,Nm_MaxID, Nm_Max);
	
	GetControlByID( neph_window, &N_MacroID, &N_Macro ); SET_CONTROL_SVALUE(nephmacrobuff, N_MacroID, N_Macro);
	
	return err;
}

//***************************************************************************************//
//**    get_Neph_Toggles -                                                                                              **//
//**                 Poke values into the dialog box (atten_param_window) when first drawn **//
//**                 Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  **//
//***************************************************************************************//
int get_Neph_Toggles(){
	CFStringRef text;
	Size actualSize;
	
	Use_Nozzle = GetControl32BitValue(  N_Nozz    );
	Use_Particles = GetControl32BitValue(  N_Particles    );
	Use_Laser = GetControl32BitValue(  N_Laser    );
	Use_Qa = GetControl32BitValue(  N_Qa    );
	Use_Qc = GetControl32BitValue(  N_Qc    );
	Use_Mp = GetControl32BitValue(  N_mp    );
	
	Do_N_FCN = GetControl32BitValue(  Nn_FCN    );
	Do_N_SFPJ = GetControl32BitValue(  Nn_SFPJ    );
	Do_P_5070 = GetControl32BitValue(  Np_5070s    );
	Do_P_6048 = GetControl32BitValue(  Np_6048s    );
	Do_P_ballAH = GetControl32BitValue(  Np_ballAH    );
	Do_P_Talc = GetControl32BitValue(  Np_Talc    );
	Do_L_H = GetControl32BitValue(  Nl_Horiz    );
	Do_L_V = GetControl32BitValue(  Nl_Vert    );
	Do_L_D = GetControl32BitValue(  Nl_Dual    );
	Do_Qa_L = GetControl32BitValue(  Na_L    );
	Do_Qa_M = GetControl32BitValue(  Na_M    );
	Do_Qa_H = GetControl32BitValue(  Na_H    );
	Do_Qc_0000 = GetControl32BitValue(  Nc_0000    );
	Do_Qc_0102 = GetControl32BitValue(  Nc_0102    );
	Do_Qc_0205 = GetControl32BitValue(  Nc_0205    );
	Do_Qc_0255 = GetControl32BitValue(  Nc_0255    );
	Do_Qc_0467 = GetControl32BitValue(  Nc_0467    );
	Do_Qc_0540 = GetControl32BitValue(  Nc_0540    );
	Do_Qc_0730 = GetControl32BitValue(  Nc_0730    );
	Do_Qc_0995 = GetControl32BitValue(  Nc_0995    );
	Do_Qc_1350 = GetControl32BitValue(  Nc_1350    );
	
	GetControlData( Nm_Min, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
	mpminvalue = CFStringGetDoubleValue( text );
	CFRelease( text );
	GetControlData( Nm_Max, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
	mpmaxvalue = CFStringGetDoubleValue( text );
	CFRelease( text );
	GetControlData( N_Macro, 0, kControlEditTextCFStringTag,sizeof(CFStringRef), &text, &actualSize );
	CFStringGetCString(text,nephmacrobuff,CHPERLN, kCFStringEncodingMacRoman);
	CFRelease( text );
	
	return OMA_OK;
}



//***************************************************************//
//**    N E P H I N I T - Initialise the Booleans for whether to process or not  *//
//**                         (used to check that the dialog is working correctly)  **//
//**                         Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  *//
//***************************************************************//
int neph_init (int init_val)	
{	
	FCN_AH_H_M_0205_0p40 = init_val; 	FCN_AH_H_M_0205_17p2 = init_val; 	FCN_AH_H_M_0205_1p20 = init_val; 	FCN_AH_H_M_0205_2p80 = init_val; 	FCN_AH_H_M_0205_4p20 = init_val; 	FCN_AH_H_M_0205_5p00 = init_val; 	FCN_AH_V_M_0205_0p60 = init_val; 	FCN_AH_V_M_0205_1p00 = init_val; 	FCN_AH_V_M_0205_30p0 = init_val; 	FCN_AH_V_M_0205_3p10 = init_val; 	FCN_AH_V_M_0205_4p70 = init_val; 	FCN_AH_V_M_0205_5p40 = init_val; 	FCN_HH_H_M_0205_0p44 = init_val; 	FCN_HH_H_M_0205_2p10 = init_val; 	FCN_HH_H_M_0205_3p10 = init_val; 	FCN_HH_H_M_0205_4p60 = init_val; 	FCN_HH_H_M_0205_8p70 = init_val; 	FCN_HH_V_M_0205_0p72 = init_val; 	FCN_HH_V_M_0205_2p60 = init_val; 	FCN_HH_V_M_0205_2p90 = init_val; 	FCN_HH_V_M_0205_3p20 = init_val; 	FCN_HH_V_M_0205_5p00 = init_val; 	FCN_HH_V_M_0205_5p30 = init_val; 	FCN_HH_V_M_0205_5p70 = init_val; 	FCN_HH_V_M_0205_6p40 = init_val; 	FCN_HH_V_M_0205_7p20 = init_val; 	FCN_HL_H_M_0205_0p50 = init_val; 	FCN_HL_H_M_0205_0p70 = init_val; 	FCN_HL_H_M_0205_0p95 = init_val; 	FCN_HL_H_M_0205_1p30 = init_val; 	FCN_HL_H_M_0205_1p50 = init_val; 	FCN_HL_H_M_0205_1p51 = init_val; 	FCN_HL_H_M_0205_1p70 = init_val; 	FCN_HL_H_M_0205_2p10 = init_val; 	FCN_HL_H_M_0205_3p20 = init_val; 	FCN_HL_H_M_0205_3p90 = init_val; 	FCN_HL_H_M_0205_8p20 = init_val; 	FCN_HL_V_M_0205_0p90 = init_val; 	FCN_HL_V_M_0205_1p60 = init_val; 	FCN_HL_V_M_0205_3p10 = init_val; 	FCN_HL_V_M_0205_4p00 = init_val; 	FCN_TP_H_M_0205_10p0 = init_val; 	FCN_TP_H_M_0205_2p10 = init_val; 	FCN_TP_H_M_0205_5p10 = init_val; 	FCN_TP_V_M_0205_10p4 = init_val; 	FCN_TP_V_M_0205_2p00 = init_val; 	FCN_TP_V_M_0205_4p30 = init_val;
	SFPJ_AH_D_H_0000_10p7 = init_val; 	SFPJ_AH_D_H_0000_4p00 = init_val; 	SFPJ_AH_D_H_0000_8p70 = init_val; 	SFPJ_AH_D_H_0995_8p00 = init_val; 	SFPJ_AH_D_H_1350_16p0 = init_val; 	SFPJ_AH_D_H_1350_2p40 = init_val; 	SFPJ_AH_D_H_1350_9p00 = init_val; 	SFPJ_HH_D_H_0000_2p20 = init_val; 	SFPJ_HH_D_H_0000_2p40 = init_val; 	SFPJ_HH_D_H_0000_2p60 = init_val; 	SFPJ_HH_D_H_0102_2p80 = init_val; 	SFPJ_HH_D_H_0255_3p20 = init_val; 	SFPJ_HH_D_H_0255_4p80 = init_val; 	SFPJ_HH_D_H_0467_3p30 = init_val; 	SFPJ_HH_D_H_0540_0p49 = init_val; 	SFPJ_HH_D_H_0540_0p70 = init_val; 	SFPJ_HH_D_H_0540_0p75 = init_val; 	SFPJ_HH_D_H_0540_1p20 = init_val; 	SFPJ_HH_D_H_0540_1p50 = init_val; 	SFPJ_HH_D_H_0540_2p10 = init_val; 	SFPJ_HH_D_H_0540_3p20 = init_val; 	SFPJ_HH_D_H_0540_5p50 = init_val; 	SFPJ_HH_D_H_0540_8p00 = init_val; 	SFPJ_HH_D_H_0730_3p40 = init_val; 	SFPJ_HH_D_H_0995_3p50 = init_val; 	SFPJ_HH_D_H_1350_0p90 = init_val; 	SFPJ_HH_D_H_1350_10p0 = init_val; 	SFPJ_HH_D_H_1350_2p00 = init_val; 	SFPJ_HH_D_H_1350_2p80 = init_val; 	SFPJ_HH_D_H_1350_3p20 = init_val; 	SFPJ_HH_D_H_1350_3p30 = init_val; 	SFPJ_HH_D_H_1350_4p90 = init_val; 	SFPJ_HH_D_L_0730_4p40 = init_val; 	SFPJ_HH_D_L_0730_9p60 = init_val; 	SFPJ_TP_D_H_0000_2p50 = init_val; 	SFPJ_TP_D_H_0000_7p40 = init_val; 
	SFPJ_TP_D_H_0540_10p9 = init_val; 	SFPJ_TP_D_H_0540_7p00 = init_val; 	SFPJ_TP_D_H_0995_12p0 = init_val;
	SFPJ_TP_D_H_1350_4p00 = init_val; 	SFPJ_TP_D_H_1350_8p00 = init_val;
	
	return OMA_OK;
}

//***************************************************************//
//**    N E P H SET NOZZLE - Initialise the Booleans for whether to process or not  *//
//**                         (used to check that the dialog is working correctly)  **//
//**                         Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  *//
//***************************************************************//
int neph_set_nozzle()
{
	if (!Do_N_FCN){
		FCN_AH_H_M_0205_0p40 = 0; FCN_AH_H_M_0205_17p2 = 0; FCN_AH_H_M_0205_1p20 = 0; FCN_AH_H_M_0205_2p80 = 0;
		FCN_AH_H_M_0205_4p20 = 0; FCN_AH_H_M_0205_5p00 = 0; FCN_AH_V_M_0205_0p60 = 0; FCN_AH_V_M_0205_1p00 = 0;
		FCN_AH_V_M_0205_30p0 = 0; FCN_AH_V_M_0205_3p10 = 0; FCN_AH_V_M_0205_4p70 = 0; FCN_AH_V_M_0205_5p40 = 0;
		FCN_HH_H_M_0205_0p44 = 0; FCN_HH_H_M_0205_2p10 = 0; FCN_HH_H_M_0205_3p10 = 0; FCN_HH_H_M_0205_4p60 = 0;
		FCN_HH_H_M_0205_8p70 = 0; FCN_HH_V_M_0205_0p72 = 0; FCN_HH_V_M_0205_2p60 = 0; FCN_HH_V_M_0205_2p90 = 0;
		FCN_HH_V_M_0205_3p20 = 0; FCN_HH_V_M_0205_5p00 = 0; FCN_HH_V_M_0205_5p30 = 0; FCN_HH_V_M_0205_5p70 = 0;
		FCN_HH_V_M_0205_6p40 = 0; FCN_HH_V_M_0205_7p20 = 0; FCN_HL_H_M_0205_0p50 = 0; FCN_HL_H_M_0205_0p70 = 0;
		FCN_HL_H_M_0205_0p95 = 0; FCN_HL_H_M_0205_1p30 = 0; FCN_HL_H_M_0205_1p50 = 0; FCN_HL_H_M_0205_1p51 = 0;
		FCN_HL_H_M_0205_1p70 = 0; FCN_HL_H_M_0205_2p10 = 0; FCN_HL_H_M_0205_3p20 = 0; FCN_HL_H_M_0205_3p90 = 0;
		FCN_HL_H_M_0205_8p20 = 0; FCN_HL_V_M_0205_0p90 = 0; FCN_HL_V_M_0205_1p60 = 0; FCN_HL_V_M_0205_3p10 = 0;
		FCN_HL_V_M_0205_4p00 = 0; FCN_TP_H_M_0205_10p0 = 0; FCN_TP_H_M_0205_2p10 = 0; FCN_TP_H_M_0205_5p10 = 0;
		FCN_TP_V_M_0205_10p4 = 0; FCN_TP_V_M_0205_2p00 = 0; FCN_TP_V_M_0205_4p30 = 0;
	}
	if (!Do_N_SFPJ){
		SFPJ_AH_D_H_0000_10p7 = 0; 	SFPJ_AH_D_H_0000_4p00 = 0; 	SFPJ_AH_D_H_0000_8p70 = 0; 	SFPJ_AH_D_H_0995_8p00 = 0;
		SFPJ_AH_D_H_1350_16p0 = 0; 	SFPJ_AH_D_H_1350_2p40 = 0; 	SFPJ_AH_D_H_1350_9p00 = 0; 	SFPJ_HH_D_H_0000_2p20 = 0;
		SFPJ_HH_D_H_0000_2p40 = 0; 	SFPJ_HH_D_H_0000_2p60 = 0; 	SFPJ_HH_D_H_0102_2p80 = 0; 	SFPJ_HH_D_H_0255_3p20 = 0;
		SFPJ_HH_D_H_0255_4p80 = 0; 	SFPJ_HH_D_H_0467_3p30 = 0; 	SFPJ_HH_D_H_0540_0p49 = 0; 	SFPJ_HH_D_H_0540_0p70 = 0;
		SFPJ_HH_D_H_0540_0p75 = 0; 	SFPJ_HH_D_H_0540_1p20 = 0; 	SFPJ_HH_D_H_0540_1p50 = 0; 	SFPJ_HH_D_H_0540_2p10 = 0;
		SFPJ_HH_D_H_0540_3p20 = 0; 	SFPJ_HH_D_H_0540_5p50 = 0; 	SFPJ_HH_D_H_0540_8p00 = 0; 	SFPJ_HH_D_H_0730_3p40 = 0;
		SFPJ_HH_D_H_0995_3p50 = 0; 	SFPJ_HH_D_H_1350_0p90 = 0; 	SFPJ_HH_D_H_1350_10p0 = 0; 	SFPJ_HH_D_H_1350_2p00 = 0;
		SFPJ_HH_D_H_1350_2p80 = 0; 	SFPJ_HH_D_H_1350_3p20 = 0; 	SFPJ_HH_D_H_1350_3p30 = 0; 	SFPJ_HH_D_H_1350_4p90 = 0;
		SFPJ_HH_D_L_0730_4p40 = 0; 	SFPJ_HH_D_L_0730_9p60 = 0; 	SFPJ_TP_D_H_0000_2p50 = 0; 	SFPJ_TP_D_H_0000_7p40 = 0; 	
		SFPJ_TP_D_H_0540_10p9 = 0; 	SFPJ_TP_D_H_0540_7p00 = 0; 	SFPJ_TP_D_H_0995_12p0 = 0; 	SFPJ_TP_D_H_1350_4p00 = 0;
		SFPJ_TP_D_H_1350_8p00 = 0;
	}
	return OMA_OK;
}
int neph_set_particles()
{
	if (!Do_P_5070){
		FCN_HH_H_M_0205_0p44 = 0; FCN_HH_H_M_0205_2p10 = 0; FCN_HH_H_M_0205_3p10 = 0; FCN_HH_H_M_0205_4p60 = 0;
		FCN_HH_H_M_0205_8p70 = 0; FCN_HH_V_M_0205_0p72 = 0; FCN_HH_V_M_0205_2p60 = 0; FCN_HH_V_M_0205_2p90 = 0;
		FCN_HH_V_M_0205_3p20 = 0; FCN_HH_V_M_0205_5p00 = 0; FCN_HH_V_M_0205_5p30 = 0; FCN_HH_V_M_0205_5p70 = 0;
		FCN_HH_V_M_0205_6p40 = 0; FCN_HH_V_M_0205_7p20 = 0; SFPJ_HH_D_H_0000_2p20 = 0;
		SFPJ_HH_D_H_0000_2p40 = 0; 	SFPJ_HH_D_H_0000_2p60 = 0; 	SFPJ_HH_D_H_0102_2p80 = 0; 	SFPJ_HH_D_H_0255_3p20 = 0;
		SFPJ_HH_D_H_0255_4p80 = 0; 	SFPJ_HH_D_H_0467_3p30 = 0; 	SFPJ_HH_D_H_0540_0p49 = 0; 	SFPJ_HH_D_H_0540_0p70 = 0;
		SFPJ_HH_D_H_0540_0p75 = 0; 	SFPJ_HH_D_H_0540_1p20 = 0; 	SFPJ_HH_D_H_0540_1p50 = 0; 	SFPJ_HH_D_H_0540_2p10 = 0;
		SFPJ_HH_D_H_0540_3p20 = 0; 	SFPJ_HH_D_H_0540_5p50 = 0; 	SFPJ_HH_D_H_0540_8p00 = 0; 	SFPJ_HH_D_H_0730_3p40 = 0;
		SFPJ_HH_D_H_0995_3p50 = 0; 	SFPJ_HH_D_H_1350_0p90 = 0; 	SFPJ_HH_D_H_1350_10p0 = 0; 	SFPJ_HH_D_H_1350_2p00 = 0;
		SFPJ_HH_D_H_1350_2p80 = 0; 	SFPJ_HH_D_H_1350_3p20 = 0; 	SFPJ_HH_D_H_1350_3p30 = 0; 	SFPJ_HH_D_H_1350_4p90 = 0;
		SFPJ_HH_D_L_0730_4p40 = 0; 	SFPJ_HH_D_L_0730_9p60 = 0; 	
	}
	if (!Do_P_6048){
		FCN_HL_H_M_0205_0p50 = 0; FCN_HL_H_M_0205_0p70 = 0; FCN_HL_H_M_0205_0p95 = 0; FCN_HL_H_M_0205_1p30 = 0; 
		FCN_HL_H_M_0205_1p50 = 0; FCN_HL_H_M_0205_1p51 = 0; FCN_HL_H_M_0205_1p70 = 0; FCN_HL_H_M_0205_2p10 = 0;
		FCN_HL_H_M_0205_3p20 = 0; FCN_HL_H_M_0205_3p90 = 0; FCN_HL_H_M_0205_8p20 = 0; FCN_HL_V_M_0205_0p90 = 0; 
		FCN_HL_V_M_0205_1p60 = 0; FCN_HL_V_M_0205_3p10 = 0; FCN_HL_V_M_0205_4p00 = 0;
	}
	if (!Do_P_ballAH){
		FCN_AH_H_M_0205_0p40 = 0; FCN_AH_H_M_0205_17p2 = 0; FCN_AH_H_M_0205_1p20 = 0; FCN_AH_H_M_0205_2p80 = 0;
		FCN_AH_H_M_0205_4p20 = 0; FCN_AH_H_M_0205_5p00 = 0; FCN_AH_V_M_0205_0p60 = 0; FCN_AH_V_M_0205_1p00 = 0;
		FCN_AH_V_M_0205_30p0 = 0; FCN_AH_V_M_0205_3p10 = 0; FCN_AH_V_M_0205_4p70 = 0; FCN_AH_V_M_0205_5p40 = 0;
		SFPJ_AH_D_H_0000_10p7 = 0; 	SFPJ_AH_D_H_0000_4p00 = 0; 	SFPJ_AH_D_H_0000_8p70 = 0; 	SFPJ_AH_D_H_0995_8p00 = 0;
		SFPJ_AH_D_H_1350_16p0 = 0; 	SFPJ_AH_D_H_1350_2p40 = 0; 	SFPJ_AH_D_H_1350_9p00 = 0; 
	}
	if (!Do_P_Talc){
		FCN_TP_H_M_0205_10p0 = 0; FCN_TP_H_M_0205_2p10 = 0; FCN_TP_H_M_0205_5p10 = 0; FCN_TP_V_M_0205_10p4 = 0;
		FCN_TP_V_M_0205_2p00 = 0; FCN_TP_V_M_0205_4p30 = 0; SFPJ_TP_D_H_0000_2p50 = 0; SFPJ_TP_D_H_0000_7p40 = 0; 
		SFPJ_TP_D_H_0540_10p9 = 0; SFPJ_TP_D_H_0540_7p00 = 0; SFPJ_TP_D_H_0995_12p0 = 0; 	SFPJ_TP_D_H_1350_4p00 = 0;
		SFPJ_TP_D_H_1350_8p00 = 0;
	}
	return OMA_OK;
}
int neph_set_laser()
{
	if (!Do_L_H){
		FCN_AH_H_M_0205_0p40 = 0; 	FCN_AH_H_M_0205_17p2 = 0; 	FCN_AH_H_M_0205_1p20 = 0; 	FCN_AH_H_M_0205_2p80 = 0;
		FCN_AH_H_M_0205_4p20 = 0; 	FCN_AH_H_M_0205_5p00 = 0; 	FCN_HH_H_M_0205_0p44 = 0; 	FCN_HH_H_M_0205_2p10 = 0;
		FCN_HH_H_M_0205_3p10 = 0; 	FCN_HH_H_M_0205_4p60 = 0; 	FCN_HH_H_M_0205_8p70 = 0; 	FCN_HL_H_M_0205_0p50 = 0;
		FCN_HL_H_M_0205_0p70 = 0; 	FCN_HL_H_M_0205_0p95 = 0; 	FCN_HL_H_M_0205_1p30 = 0; 	FCN_HL_H_M_0205_1p50 = 0;
		FCN_HL_H_M_0205_1p51 = 0; 	FCN_HL_H_M_0205_1p70 = 0; 	FCN_HL_H_M_0205_2p10 = 0; 	FCN_HL_H_M_0205_3p20 = 0;
		FCN_HL_H_M_0205_3p90 = 0; 	FCN_HL_H_M_0205_8p20 = 0; 	 FCN_TP_H_M_0205_10p0 = 0; 	FCN_TP_H_M_0205_2p10 = 0;
		FCN_TP_H_M_0205_5p10 = 0; 	
		
	}
	if (!Do_L_V){
		FCN_TP_V_M_0205_10p4 = 0; 	FCN_TP_V_M_0205_2p00 = 0; 	FCN_TP_V_M_0205_4p30 = 0;
		FCN_HL_V_M_0205_0p90 = 0; 	FCN_HL_V_M_0205_1p60 = 0; 	FCN_HL_V_M_0205_3p10 = 0; 	FCN_HL_V_M_0205_4p00 = 0;
		FCN_HH_V_M_0205_0p72 = 0; 	FCN_HH_V_M_0205_2p60 = 0; 	FCN_HH_V_M_0205_2p90 = 0; 	FCN_HH_V_M_0205_3p20 = 0;
		FCN_HH_V_M_0205_5p00 = 0; 	FCN_HH_V_M_0205_5p30 = 0; 	FCN_HH_V_M_0205_5p70 = 0; 	FCN_HH_V_M_0205_6p40 = 0;
		FCN_HH_V_M_0205_7p20 = 0; 	FCN_AH_V_M_0205_0p60 = 0; 	FCN_AH_V_M_0205_1p00 = 0; 	FCN_AH_V_M_0205_30p0 = 0;
		FCN_AH_V_M_0205_3p10 = 0; 	FCN_AH_V_M_0205_4p70 = 0; 	FCN_AH_V_M_0205_5p40 = 0; 
	}
	if(!Do_L_D){
		SFPJ_AH_D_H_0000_10p7 = 0; 	SFPJ_AH_D_H_0000_4p00 = 0; 	SFPJ_AH_D_H_0000_8p70 = 0; 	SFPJ_AH_D_H_0995_8p00 = 0;
		SFPJ_AH_D_H_1350_16p0 = 0; 	SFPJ_AH_D_H_1350_2p40 = 0; 	SFPJ_AH_D_H_1350_9p00 = 0; 	SFPJ_HH_D_H_0000_2p20 = 0;
		SFPJ_HH_D_H_0000_2p40 = 0; 	SFPJ_HH_D_H_0000_2p60 = 0; 	SFPJ_HH_D_H_0102_2p80 = 0; 	SFPJ_HH_D_H_0255_3p20 = 0;
		SFPJ_HH_D_H_0255_4p80 = 0; 	SFPJ_HH_D_H_0467_3p30 = 0; 	SFPJ_HH_D_H_0540_0p49 = 0; 	SFPJ_HH_D_H_0540_0p70 = 0;
		SFPJ_HH_D_H_0540_0p75 = 0; 	SFPJ_HH_D_H_0540_1p20 = 0; 	SFPJ_HH_D_H_0540_1p50 = 0; 	SFPJ_HH_D_H_0540_2p10 = 0;
		SFPJ_HH_D_H_0540_3p20 = 0; 	SFPJ_HH_D_H_0540_5p50 = 0; 	SFPJ_HH_D_H_0540_8p00 = 0; 	SFPJ_HH_D_H_0730_3p40 = 0;
		SFPJ_HH_D_H_0995_3p50 = 0; 	SFPJ_HH_D_H_1350_0p90 = 0; 	SFPJ_HH_D_H_1350_10p0 = 0; 	SFPJ_HH_D_H_1350_2p00 = 0;
		SFPJ_HH_D_H_1350_2p80 = 0; 	SFPJ_HH_D_H_1350_3p20 = 0; 	SFPJ_HH_D_H_1350_3p30 = 0; 	SFPJ_HH_D_H_1350_4p90 = 0;
		SFPJ_HH_D_L_0730_4p40 = 0; 	SFPJ_HH_D_L_0730_9p60 = 0; 	SFPJ_TP_D_H_0000_2p50 = 0; 	SFPJ_TP_D_H_0000_7p40 = 0; 
		SFPJ_TP_D_H_0540_10p9 = 0; 	SFPJ_TP_D_H_0540_7p00 = 0; 	SFPJ_TP_D_H_0995_12p0 = 0;
		SFPJ_TP_D_H_1350_4p00 = 0; 	SFPJ_TP_D_H_1350_8p00 = 0;
	}
	return OMA_OK;
}
int neph_set_qa()
{
	if(!Do_Qa_L){
		SFPJ_HH_D_L_0730_4p40 = 0; 	SFPJ_HH_D_L_0730_9p60 = 0; 
	}
	if(!Do_Qa_M){
		FCN_AH_H_M_0205_0p40 = 0; 	FCN_AH_H_M_0205_17p2 = 0; 	FCN_AH_H_M_0205_1p20 = 0; 	FCN_AH_H_M_0205_2p80 = 0;
		FCN_AH_H_M_0205_4p20 = 0; 	FCN_AH_H_M_0205_5p00 = 0; 	FCN_AH_V_M_0205_0p60 = 0; 	FCN_AH_V_M_0205_1p00 = 0;
		FCN_AH_V_M_0205_30p0 = 0; 	FCN_AH_V_M_0205_3p10 = 0; 	FCN_AH_V_M_0205_4p70 = 0; 	FCN_AH_V_M_0205_5p40 = 0;
		FCN_HH_H_M_0205_0p44 = 0; 	FCN_HH_H_M_0205_2p10 = 0; 	FCN_HH_H_M_0205_3p10 = 0; 	FCN_HH_H_M_0205_4p60 = 0;
		FCN_HH_H_M_0205_8p70 = 0; 	FCN_HH_V_M_0205_0p72 = 0; 	FCN_HH_V_M_0205_2p60 = 0; 	FCN_HH_V_M_0205_2p90 = 0;
		FCN_HH_V_M_0205_3p20 = 0; 	FCN_HH_V_M_0205_5p00 = 0; 	FCN_HH_V_M_0205_5p30 = 0; 	FCN_HH_V_M_0205_5p70 = 0;
		FCN_HH_V_M_0205_6p40 = 0; 	FCN_HH_V_M_0205_7p20 = 0; 	FCN_HL_H_M_0205_0p50 = 0; 	FCN_HL_H_M_0205_0p70 = 0;
		FCN_HL_H_M_0205_0p95 = 0; 	FCN_HL_H_M_0205_1p30 = 0; 	FCN_HL_H_M_0205_1p50 = 0; 	FCN_HL_H_M_0205_1p51 = 0;
		FCN_HL_H_M_0205_1p70 = 0; 	FCN_HL_H_M_0205_2p10 = 0; 	FCN_HL_H_M_0205_3p20 = 0; 	FCN_HL_H_M_0205_3p90 = 0;
		FCN_HL_H_M_0205_8p20 = 0; 	FCN_HL_V_M_0205_0p90 = 0; 	FCN_HL_V_M_0205_1p60 = 0; 	FCN_HL_V_M_0205_3p10 = 0;
		FCN_HL_V_M_0205_4p00 = 0; 	FCN_TP_H_M_0205_10p0 = 0; 	FCN_TP_H_M_0205_2p10 = 0; 	FCN_TP_H_M_0205_5p10 = 0;
		FCN_TP_V_M_0205_10p4 = 0; 	FCN_TP_V_M_0205_2p00 = 0; 	FCN_TP_V_M_0205_4p30 = 0;
	}
	if(!Do_Qa_H){
		SFPJ_AH_D_H_0000_10p7 = 0; 	SFPJ_AH_D_H_0000_4p00 = 0; 	SFPJ_AH_D_H_0000_8p70 = 0; 	SFPJ_AH_D_H_0995_8p00 = 0;
		SFPJ_AH_D_H_1350_16p0 = 0; 	SFPJ_AH_D_H_1350_2p40 = 0; 	SFPJ_AH_D_H_1350_9p00 = 0; 	SFPJ_HH_D_H_0000_2p20 = 0;
		SFPJ_HH_D_H_0000_2p40 = 0; 	SFPJ_HH_D_H_0000_2p60 = 0; 	SFPJ_HH_D_H_0102_2p80 = 0; 	SFPJ_HH_D_H_0255_3p20 = 0;
		SFPJ_HH_D_H_0255_4p80 = 0; 	SFPJ_HH_D_H_0467_3p30 = 0; 	SFPJ_HH_D_H_0540_0p49 = 0; 	SFPJ_HH_D_H_0540_0p70 = 0;
		SFPJ_HH_D_H_0540_0p75 = 0; 	SFPJ_HH_D_H_0540_1p20 = 0; 	SFPJ_HH_D_H_0540_1p50 = 0; 	SFPJ_HH_D_H_0540_2p10 = 0;
		SFPJ_HH_D_H_0540_3p20 = 0; 	SFPJ_HH_D_H_0540_5p50 = 0; 	SFPJ_HH_D_H_0540_8p00 = 0; 	SFPJ_HH_D_H_0730_3p40 = 0;
		SFPJ_HH_D_H_0995_3p50 = 0; 	SFPJ_HH_D_H_1350_0p90 = 0; 	SFPJ_HH_D_H_1350_10p0 = 0; 	SFPJ_HH_D_H_1350_2p00 = 0;
		SFPJ_HH_D_H_1350_2p80 = 0; 	SFPJ_HH_D_H_1350_3p20 = 0; 	SFPJ_HH_D_H_1350_3p30 = 0; 	SFPJ_HH_D_H_1350_4p90 = 0;
		SFPJ_TP_D_H_0000_2p50 = 0; 	SFPJ_TP_D_H_0000_7p40 = 0; 	SFPJ_TP_D_H_0540_10p9 = 0; 	SFPJ_TP_D_H_0540_7p00 = 0;
		SFPJ_TP_D_H_0995_12p0 = 0; 	SFPJ_TP_D_H_1350_4p00 = 0; 	SFPJ_TP_D_H_1350_8p00 = 0;
	}
	return OMA_OK;
}
int neph_set_qc()
{
	if(!Do_Qc_0000){
		SFPJ_AH_D_H_0000_10p7 = 0; 	SFPJ_AH_D_H_0000_4p00 = 0; 	SFPJ_AH_D_H_0000_8p70 = 0; 	SFPJ_HH_D_H_0000_2p20 = 0;
		SFPJ_HH_D_H_0000_2p40 = 0; 	SFPJ_HH_D_H_0000_2p60 = 0;  	SFPJ_TP_D_H_0000_2p50 = 0; 	SFPJ_TP_D_H_0000_7p40 = 0; 
	}
	if(!Do_Qc_0102){
		SFPJ_HH_D_H_0102_2p80 = 0;
	}
	if(!Do_Qc_0205){
		FCN_AH_H_M_0205_0p40 = 0; 	FCN_AH_H_M_0205_17p2 = 0; 	FCN_AH_H_M_0205_1p20 = 0; 	FCN_AH_H_M_0205_2p80 = 0;
		FCN_AH_H_M_0205_4p20 = 0; 	FCN_AH_H_M_0205_5p00 = 0; 	FCN_AH_V_M_0205_0p60 = 0; 	FCN_AH_V_M_0205_1p00 = 0;
		FCN_AH_V_M_0205_30p0 = 0; 	FCN_AH_V_M_0205_3p10 = 0; 	FCN_AH_V_M_0205_4p70 = 0; 	FCN_AH_V_M_0205_5p40 = 0;
		FCN_HH_H_M_0205_0p44 = 0; 	FCN_HH_H_M_0205_2p10 = 0; 	FCN_HH_H_M_0205_3p10 = 0; 	FCN_HH_H_M_0205_4p60 = 0;
		FCN_HH_H_M_0205_8p70 = 0; 	FCN_HH_V_M_0205_0p72 = 0; 	FCN_HH_V_M_0205_2p60 = 0; 	FCN_HH_V_M_0205_2p90 = 0;
		FCN_HH_V_M_0205_3p20 = 0; 	FCN_HH_V_M_0205_5p00 = 0; 	FCN_HH_V_M_0205_5p30 = 0; 	FCN_HH_V_M_0205_5p70 = 0;
		FCN_HH_V_M_0205_6p40 = 0; 	FCN_HH_V_M_0205_7p20 = 0; 	FCN_HL_H_M_0205_0p50 = 0; 	FCN_HL_H_M_0205_0p70 = 0;
		FCN_HL_H_M_0205_0p95 = 0; 	FCN_HL_H_M_0205_1p30 = 0; 	FCN_HL_H_M_0205_1p50 = 0; 	FCN_HL_H_M_0205_1p51 = 0;
		FCN_HL_H_M_0205_1p70 = 0; 	FCN_HL_H_M_0205_2p10 = 0; 	FCN_HL_H_M_0205_3p20 = 0; 	FCN_HL_H_M_0205_3p90 = 0;
		FCN_HL_H_M_0205_8p20 = 0; 	FCN_HL_V_M_0205_0p90 = 0; 	FCN_HL_V_M_0205_1p60 = 0; 	FCN_HL_V_M_0205_3p10 = 0;
		FCN_HL_V_M_0205_4p00 = 0; 	FCN_TP_H_M_0205_10p0 = 0; 	FCN_TP_H_M_0205_2p10 = 0; 	FCN_TP_H_M_0205_5p10 = 0;
		FCN_TP_V_M_0205_10p4 = 0; 	FCN_TP_V_M_0205_2p00 = 0; 	FCN_TP_V_M_0205_4p30 = 0;
	}
	if(!Do_Qc_0255){
		SFPJ_HH_D_H_0255_3p20 = 0;
		SFPJ_HH_D_H_0255_4p80 = 0;
	}
	if(!Do_Qc_0467){
		SFPJ_HH_D_H_0467_3p30 = 0; 	
	}
	if(!Do_Qc_0540){
		SFPJ_TP_D_H_0540_10p9 = 0; 	SFPJ_TP_D_H_0540_7p00 = 0; 	SFPJ_HH_D_H_0540_0p49 = 0; 	SFPJ_HH_D_H_0540_0p70 = 0;
		SFPJ_HH_D_H_0540_0p75 = 0; 	SFPJ_HH_D_H_0540_1p20 = 0; 	SFPJ_HH_D_H_0540_1p50 = 0; 	SFPJ_HH_D_H_0540_2p10 = 0;
		SFPJ_HH_D_H_0540_3p20 = 0; 	SFPJ_HH_D_H_0540_5p50 = 0; 	SFPJ_HH_D_H_0540_8p00 = 0;
	}
	if(!Do_Qc_0730){
		SFPJ_HH_D_H_0730_3p40 = 0; 	SFPJ_HH_D_L_0730_4p40 = 0; 	SFPJ_HH_D_L_0730_9p60 = 0;
	}
	if(!Do_Qc_0995){
		SFPJ_AH_D_H_0995_8p00 = 0;	SFPJ_TP_D_H_0995_12p0 = 0;	SFPJ_HH_D_H_0995_3p50 = 0; 
	}
	if(!Do_Qc_1350){
		SFPJ_TP_D_H_1350_4p00 = 0; 	SFPJ_TP_D_H_1350_8p00 = 0; 	SFPJ_AH_D_H_1350_16p0 = 0; 	SFPJ_AH_D_H_1350_2p40 = 0;
		SFPJ_AH_D_H_1350_9p00 = 0; 	SFPJ_HH_D_H_1350_0p90 = 0; 	SFPJ_HH_D_H_1350_10p0 = 0; 	SFPJ_HH_D_H_1350_2p00 = 0;
		SFPJ_HH_D_H_1350_2p80 = 0; 	SFPJ_HH_D_H_1350_3p20 = 0; 	SFPJ_HH_D_H_1350_3p30 = 0; 	SFPJ_HH_D_H_1350_4p90 = 0;
	}
	return OMA_OK;
}

int Not_In_Range(float pointval){
	float temp;
	//check first that min is lower and max is uppoer bounds
	if (mpminvalue>mpmaxvalue){
		temp = mpminvalue;
		mpminvalue = mpmaxvalue;
		mpmaxvalue = temp;
	}
	
	if ((pointval >mpmaxvalue)||(pointval<mpminvalue)) return 1;
	else return 0;
}

int neph_set_Mass()
{
	if (Not_In_Range( 0.4 )) FCN_AH_H_M_0205_0p40=0;
	if (Not_In_Range( 0.44 )) FCN_HH_H_M_0205_0p44=0;
	if (Not_In_Range( 0.49 )) SFPJ_HH_D_H_0540_0p49=0;
	if (Not_In_Range( 0.5 )) FCN_HL_H_M_0205_0p50=0;
	if (Not_In_Range( 0.6 )) FCN_AH_V_M_0205_0p60=0;
	if (Not_In_Range( 0.7 )) FCN_HL_H_M_0205_0p70=0;
	if (Not_In_Range( 0.7 )) SFPJ_HH_D_H_0540_0p70=0;
	if (Not_In_Range( 0.72 )) FCN_HH_V_M_0205_0p72=0;
	if (Not_In_Range( 0.75 )) SFPJ_HH_D_H_0540_0p75=0;
	if (Not_In_Range( 0.9 )) FCN_HL_V_M_0205_0p90=0;
	if (Not_In_Range( 0.9 )) SFPJ_HH_D_H_1350_0p90=0;
	if (Not_In_Range( 0.95 )) FCN_HL_H_M_0205_0p95=0;
	if (Not_In_Range( 1 )) FCN_AH_V_M_0205_1p00=0;
	if (Not_In_Range( 1.2 )) FCN_AH_H_M_0205_1p20=0;
	if (Not_In_Range( 1.2 )) SFPJ_HH_D_H_0540_1p20=0;
	if (Not_In_Range( 1.3 )) FCN_HL_H_M_0205_1p30=0;
	if (Not_In_Range( 1.5 )) FCN_HL_H_M_0205_1p50=0;
	if (Not_In_Range( 1.5 )) SFPJ_HH_D_H_0540_1p50=0;
	if (Not_In_Range( 1.51 )) FCN_HL_H_M_0205_1p51=0;
	if (Not_In_Range( 1.6 )) FCN_HL_V_M_0205_1p60=0;
	if (Not_In_Range( 1.7 )) FCN_HL_H_M_0205_1p70=0;
	if (Not_In_Range( 2 )) FCN_TP_V_M_0205_2p00=0;
	if (Not_In_Range( 2 )) SFPJ_HH_D_H_1350_2p00=0;
	if (Not_In_Range( 2.1 )) FCN_HH_H_M_0205_2p10=0;
	if (Not_In_Range( 2.1 )) FCN_HL_H_M_0205_2p10=0;
	if (Not_In_Range( 2.1 )) FCN_TP_H_M_0205_2p10=0;
	if (Not_In_Range( 2.1 )) SFPJ_HH_D_H_0540_2p10=0;
	if (Not_In_Range( 2.2 )) SFPJ_HH_D_H_0000_2p20=0;
	if (Not_In_Range( 2.4 )) SFPJ_AH_D_H_1350_2p40=0;
	if (Not_In_Range( 2.4 )) SFPJ_HH_D_H_0000_2p40=0;
	if (Not_In_Range( 2.5 )) SFPJ_TP_D_H_0000_2p50=0;
	if (Not_In_Range( 2.6 )) FCN_HH_V_M_0205_2p60=0;
	if (Not_In_Range( 2.6 )) SFPJ_HH_D_H_0000_2p60=0;
	if (Not_In_Range( 2.8 )) FCN_AH_H_M_0205_2p80=0;
	if (Not_In_Range( 2.8 )) SFPJ_HH_D_H_0102_2p80=0;
	if (Not_In_Range( 2.8 )) SFPJ_HH_D_H_1350_2p80=0;
	if (Not_In_Range( 2.9 )) FCN_HH_V_M_0205_2p90=0;
	if (Not_In_Range( 3.1 )) FCN_AH_V_M_0205_3p10=0;
	if (Not_In_Range( 3.1 )) FCN_HH_H_M_0205_3p10=0;
	if (Not_In_Range( 3.1 )) FCN_HL_V_M_0205_3p10=0;
	if (Not_In_Range( 3.2 )) FCN_HH_V_M_0205_3p20=0;
	if (Not_In_Range( 3.2 )) FCN_HL_H_M_0205_3p20=0;
	if (Not_In_Range( 3.2 )) SFPJ_HH_D_H_0255_3p20=0;
	if (Not_In_Range( 3.2 )) SFPJ_HH_D_H_0540_3p20=0;
	if (Not_In_Range( 3.2 )) SFPJ_HH_D_H_1350_3p20=0;
	if (Not_In_Range( 3.3 )) SFPJ_HH_D_H_0467_3p30=0;
	if (Not_In_Range( 3.3 )) SFPJ_HH_D_H_1350_3p30=0;
	if (Not_In_Range( 3.4 )) SFPJ_HH_D_H_0730_3p40=0;
	if (Not_In_Range( 3.5 )) SFPJ_HH_D_H_0995_3p50=0;
	if (Not_In_Range( 3.9 )) FCN_HL_H_M_0205_3p90=0;
	if (Not_In_Range( 4 )) FCN_HL_V_M_0205_4p00=0;
	if (Not_In_Range( 4 )) SFPJ_AH_D_H_0000_4p00=0;
	if (Not_In_Range( 4 )) SFPJ_TP_D_H_1350_4p00=0;
	if (Not_In_Range( 4.2 )) FCN_AH_H_M_0205_4p20=0;
	if (Not_In_Range( 4.3 )) FCN_TP_V_M_0205_4p30=0;
	if (Not_In_Range( 4.4 )) SFPJ_HH_D_L_0730_4p40=0;
	if (Not_In_Range( 4.6 )) FCN_HH_H_M_0205_4p60=0;
	if (Not_In_Range( 4.7 )) FCN_AH_V_M_0205_4p70=0;
	if (Not_In_Range( 4.8 )) SFPJ_HH_D_H_0255_4p80=0;
	if (Not_In_Range( 4.9 )) SFPJ_HH_D_H_1350_4p90=0;
	if (Not_In_Range( 5 )) FCN_AH_H_M_0205_5p00=0;
	if (Not_In_Range( 5 )) FCN_HH_V_M_0205_5p00=0;
	if (Not_In_Range( 5.1 )) FCN_TP_H_M_0205_5p10=0;
	if (Not_In_Range( 5.3 )) FCN_HH_V_M_0205_5p30=0;
	if (Not_In_Range( 5.4 )) FCN_AH_V_M_0205_5p40=0;
	if (Not_In_Range( 5.5 )) SFPJ_HH_D_H_0540_5p50=0;
	if (Not_In_Range( 5.7 )) FCN_HH_V_M_0205_5p70=0;
	if (Not_In_Range( 6.4 )) FCN_HH_V_M_0205_6p40=0;
	if (Not_In_Range( 7 )) SFPJ_TP_D_H_0540_7p00=0;
	if (Not_In_Range( 7.2 )) FCN_HH_V_M_0205_7p20=0;
	if (Not_In_Range( 7.4 )) SFPJ_TP_D_H_0000_7p40=0;
	if (Not_In_Range( 8 )) SFPJ_AH_D_H_0995_8p00=0;
	if (Not_In_Range( 8 )) SFPJ_HH_D_H_0540_8p00=0;
	if (Not_In_Range( 8 )) SFPJ_TP_D_H_1350_8p00=0;
	if (Not_In_Range( 8.2 )) FCN_HL_H_M_0205_8p20=0;
	if (Not_In_Range( 8.7 )) FCN_HH_H_M_0205_8p70=0;
	if (Not_In_Range( 8.7 )) SFPJ_AH_D_H_0000_8p70=0;
	if (Not_In_Range( 9 )) SFPJ_AH_D_H_1350_9p00=0;
	if (Not_In_Range( 9.6 )) SFPJ_HH_D_L_0730_9p60=0;
	if (Not_In_Range( 10 )) FCN_TP_H_M_0205_10p0=0;
	if (Not_In_Range( 10 )) SFPJ_HH_D_H_1350_10p0=0;
	if (Not_In_Range( 10.4 )) FCN_TP_V_M_0205_10p4=0;
	if (Not_In_Range( 10.7 )) SFPJ_AH_D_H_0000_10p7=0;
	if (Not_In_Range( 10.9 )) SFPJ_TP_D_H_0540_10p9=0;
	if (Not_In_Range( 12 )) SFPJ_TP_D_H_0995_12p0=0;
	if (Not_In_Range( 16 )) SFPJ_AH_D_H_1350_16p0=0;
	if (Not_In_Range( 17.2 )) FCN_AH_H_M_0205_17p2=0;
	if (Not_In_Range( 30 )) FCN_AH_V_M_0205_30p0=0;
	return OMA_OK;
}
int neph_count()
{
	int total_to_do = 0;
	total_to_do = FCN_AH_H_M_0205_0p40 +	FCN_AH_H_M_0205_17p2 +	FCN_AH_H_M_0205_1p20 +	FCN_AH_H_M_0205_2p80 +	FCN_AH_H_M_0205_4p20 +	FCN_AH_H_M_0205_5p00 +	FCN_AH_V_M_0205_0p60 +	FCN_AH_V_M_0205_1p00 +	FCN_AH_V_M_0205_30p0 +	FCN_AH_V_M_0205_3p10 +	FCN_AH_V_M_0205_4p70 +	FCN_AH_V_M_0205_5p40 +	FCN_HH_H_M_0205_0p44 +	FCN_HH_H_M_0205_2p10 +	FCN_HH_H_M_0205_3p10 +	FCN_HH_H_M_0205_4p60 +	FCN_HH_H_M_0205_8p70 +	FCN_HH_V_M_0205_0p72 +	FCN_HH_V_M_0205_2p60 +	FCN_HH_V_M_0205_2p90 +	FCN_HH_V_M_0205_3p20 +	FCN_HH_V_M_0205_5p00 +	FCN_HH_V_M_0205_5p30 +	FCN_HH_V_M_0205_5p70 +	FCN_HH_V_M_0205_6p40 +	FCN_HH_V_M_0205_7p20 +	FCN_HL_H_M_0205_0p50 +	FCN_HL_H_M_0205_0p70 +	FCN_HL_H_M_0205_0p95 +	FCN_HL_H_M_0205_1p30 +	FCN_HL_H_M_0205_1p50 +	FCN_HL_H_M_0205_1p51 +	FCN_HL_H_M_0205_1p70 +	FCN_HL_H_M_0205_2p10 +	FCN_HL_H_M_0205_3p20 +	FCN_HL_H_M_0205_3p90 +	FCN_HL_H_M_0205_8p20 +	FCN_HL_V_M_0205_0p90 +	FCN_HL_V_M_0205_1p60 +	FCN_HL_V_M_0205_3p10 +	FCN_HL_V_M_0205_4p00 +	FCN_TP_H_M_0205_10p0 +	FCN_TP_H_M_0205_2p10 +	FCN_TP_H_M_0205_5p10 +	FCN_TP_V_M_0205_10p4 +	FCN_TP_V_M_0205_2p00 +	FCN_TP_V_M_0205_4p30 +
		SFPJ_AH_D_H_0000_10p7 +	SFPJ_AH_D_H_0000_4p00 +	SFPJ_AH_D_H_0000_8p70 +	SFPJ_AH_D_H_0995_8p00 +	SFPJ_AH_D_H_1350_16p0 +	SFPJ_AH_D_H_1350_2p40 +	SFPJ_AH_D_H_1350_9p00 +	SFPJ_HH_D_H_0000_2p20 +	SFPJ_HH_D_H_0000_2p40 +	SFPJ_HH_D_H_0000_2p60 +	SFPJ_HH_D_H_0102_2p80 +	SFPJ_HH_D_H_0255_3p20 +	SFPJ_HH_D_H_0255_4p80 +	SFPJ_HH_D_H_0467_3p30 +	SFPJ_HH_D_H_0540_0p49 +	SFPJ_HH_D_H_0540_0p70 +	SFPJ_HH_D_H_0540_0p75 +	SFPJ_HH_D_H_0540_1p20 +	SFPJ_HH_D_H_0540_1p50 +	SFPJ_HH_D_H_0540_2p10 +	SFPJ_HH_D_H_0540_3p20 +	SFPJ_HH_D_H_0540_5p50 +	SFPJ_HH_D_H_0540_8p00 +	SFPJ_HH_D_H_0730_3p40 +	SFPJ_HH_D_H_0995_3p50 +	SFPJ_HH_D_H_1350_0p90 +	SFPJ_HH_D_H_1350_10p0 +	SFPJ_HH_D_H_1350_2p00 +	SFPJ_HH_D_H_1350_2p80 +	SFPJ_HH_D_H_1350_3p20 +	SFPJ_HH_D_H_1350_3p30 +	SFPJ_HH_D_H_1350_4p90 +	SFPJ_HH_D_L_0730_4p40 +	SFPJ_HH_D_L_0730_9p60 +	SFPJ_TP_D_H_0000_2p50 +	SFPJ_TP_D_H_0000_7p40 +
		SFPJ_TP_D_H_0540_10p9 +	SFPJ_TP_D_H_0540_7p00 +	SFPJ_TP_D_H_0995_12p0 +
		SFPJ_TP_D_H_1350_4p00 +	SFPJ_TP_D_H_1350_8p00;
	
	printf("We are doing %d sets from a total of 88\n", total_to_do);
	return OMA_OK;
}
int neph_build_control()
{
	FILE 	*fp;
	
	// OPen a file and write to it in stages
	strcpy(scratch, "NCM130206");
	fp = fopen(fullname(&scratch,MACROS_DATA),"w");
	if( fp == NULL) {
		// If the file can't be created then EXIT with an Error
		beep();
		printf("Can't create the control Macro structure\n");
		return OMA_FILE;
	}	
	
	// Start writing the sets we want to process into the overall CONTROL macro
	if (FCN_AH_H_M_0205_0p40){
		fprintf(fp,"folder = \"FCN/ballAH/Horizontal/Qa979/Qc0205/m0p40/\"\n");
		fprintf(fp,"name = \"FN-AH-H-M-0205-0p40\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_AH_H_M_0205_17p2){
		fprintf(fp,"folder = \"FCN/ballAH/Horizontal/Qa979/Qc0205/m17p2/\"\n");
		fprintf(fp,"name = \"FN-AH-H-M-0205-17p2\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_AH_H_M_0205_1p20){
		fprintf(fp,"folder = \"FCN/ballAH/Horizontal/Qa979/Qc0205/m1p20/\"\n");
		fprintf(fp,"name = \"FN-AH-H-M-0205-1p20\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if(FCN_AH_H_M_0205_2p80){
		fprintf(fp,"folder = \"FCN/ballAH/Horizontal/Qa979/Qc0205/m2p80/\"\n");
		fprintf(fp,"name = \"FN-AH-H-M-0205-2p80\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_AH_H_M_0205_4p20){
		fprintf(fp,"folder = \"FCN/ballAH/Horizontal/Qa979/Qc0205/m4p20/\"\n");
		fprintf(fp,"name = \"FN-AH-H-M-0205-4p20\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_AH_H_M_0205_5p00){		
		fprintf(fp,"folder = \"FCN/ballAH/Horizontal/Qa979/Qc0205/m5p00/\"\n");
		fprintf(fp,"name = \"FN-AH-H-M-0205-5p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_AH_V_M_0205_0p60){
		fprintf(fp,"folder = \"FCN/ballAH/Vertical/Qa979/Qc0205/m0p60/\"\n");
		fprintf(fp,"name = \"FN-AH-V-M-0205-0p60\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_AH_V_M_0205_1p00){
		fprintf(fp,"folder = \"FCN/ballAH/Vertical/Qa979/Qc0205/m1p00/\"\n");
		fprintf(fp,"name = \"FN-AH-V-M-0205-1p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (FCN_AH_V_M_0205_30p0){
		fprintf(fp,"folder = \"FCN/ballAH/Vertical/Qa979/Qc0205/m30p0/\"\n");
		fprintf(fp,"name = \"FN-AH-V-M-0205-30p0\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_AH_V_M_0205_3p10){
		fprintf(fp,"folder = \"FCN/ballAH/Vertical/Qa979/Qc0205/m3p10/\"\n");
		fprintf(fp,"name = \"FN-AH-V-M-0205-3p10\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (FCN_AH_V_M_0205_4p70){
		fprintf(fp,"folder = \"FCN/ballAH/Vertical/Qa979/Qc0205/m4p70/\"\n");
		fprintf(fp,"name = \"FN-AH-V-M-0205-4p70\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (FCN_AH_V_M_0205_5p40){
		fprintf(fp,"folder = \"FCN/ballAH/Vertical/Qa979/Qc0205/m5p40/\"\n");
		fprintf(fp,"name = \"FN-AH-V-M-0205-5p40\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (FCN_HH_H_M_0205_0p44){
		fprintf(fp,"folder = \"FCN/5070s/Horizontal/Qa979/Qc0205/m0p44/\"\n");
		fprintf(fp,"name = \"FN-HH-H-M-0205-0p44\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_H_M_0205_2p10){
		fprintf(fp,"folder = \"FCN/5070s/Horizontal/Qa979/Qc0205/m2p10/\"\n");
		fprintf(fp,"name = \"FN-HH-H-M-0205-2p10\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_H_M_0205_3p10){		
		fprintf(fp,"folder = \"FCN/5070s/Horizontal/Qa979/Qc0205/m3p10/\"\n");
		fprintf(fp,"name = \"FN-HH-H-M-0205-3p10\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_H_M_0205_4p60){
		fprintf(fp,"folder = \"FCN/5070s/Horizontal/Qa979/Qc0205/m4p60/\"\n");
		fprintf(fp,"name = \"FN-HH-H-M-0205-4p60\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_H_M_0205_8p70){
		fprintf(fp,"folder = \"FCN/5070s/Horizontal/Qa979/Qc0205/m8p70/\"\n");
		fprintf(fp,"name = \"FN-HH-H-M-0205-8p70\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_V_M_0205_0p72){
		fprintf(fp,"folder = \"FCN/5070s/Vertical/Qa979/Qc0205/m0p72/\"\n");
		fprintf(fp,"name = \"FN-HH-V-M-0205-0p72\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_V_M_0205_2p60){
		fprintf(fp,"folder = \"FCN/5070s/Vertical/Qa979/Qc0205/m2p60/\"\n");
		fprintf(fp,"name = \"FN-HH-V-M-0205-2p60\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_V_M_0205_2p90){
		fprintf(fp,"folder = \"FCN/5070s/Vertical/Qa979/Qc0205/m2p90/\"\n");
		fprintf(fp,"name = \"FN-HH-V-M-0205-2p90\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_V_M_0205_3p20){
		fprintf(fp,"folder = \"FCN/5070s/Vertical/Qa979/Qc0205/m3p20/\"\n");
		fprintf(fp,"name = \"FN-HH-V-M-0205-3p20\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_V_M_0205_5p00){
		fprintf(fp,"folder = \"FCN/5070s/Vertical/Qa979/Qc0205/m5p00/\"\n");
		fprintf(fp,"name = \"FN-HH-V-M-0205-5p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_V_M_0205_5p30){
		fprintf(fp,"folder = \"FCN/5070s/Vertical/Qa979/Qc0205/m5p30/\"\n");
		fprintf(fp,"name = \"FN-HH-V-M-0205-5p30\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_V_M_0205_5p70){
		fprintf(fp,"folder = \"FCN/5070s/Vertical/Qa979/Qc0205/m5p70/\"\n");
		fprintf(fp,"name = \"FN-HH-V-M-0205-5p70\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_V_M_0205_6p40){
		fprintf(fp,"folder = \"FCN/5070s/Vertical/Qa979/Qc0205/m6p40/\"\n");
		fprintf(fp,"name = \"FN-HH-V-M-0205-6p40\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HH_V_M_0205_7p20){
		fprintf(fp,"folder = \"FCN/5070s/Vertical/Qa979/Qc0205/m7p20/\"\n");
		fprintf(fp,"name = \"FN-HH-V-M-0205-7p20\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_H_M_0205_0p50){
		fprintf(fp,"folder = \"FCN/6048s/Horizontal/Qa979/Qc0205/m0p50/\"\n");
		fprintf(fp,"name = \"FN-HL-H-M-0205-0p50\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_H_M_0205_0p70){
		fprintf(fp,"folder = \"FCN/6048s/Horizontal/Qa979/Qc0205/m0p70/\"\n");
		fprintf(fp,"name = \"FN-HL-H-M-0205-0p70\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_H_M_0205_0p95){
		fprintf(fp,"folder = \"FCN/6048s/Horizontal/Qa979/Qc0205/m0p95/\"\n");
		fprintf(fp,"name = \"FN-HL-H-M-0205-0p95\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_H_M_0205_1p30){
		fprintf(fp,"folder = \"FCN/6048s/Horizontal/Qa979/Qc0205/m1p30/\"\n");
		fprintf(fp,"name = \"FN-HL-H-M-0205-1p30\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_H_M_0205_1p50){
		fprintf(fp,"folder = \"FCN/6048s/Horizontal/Qa979/Qc0205/m1p50/\"\n");
		fprintf(fp,"name = \"FN-HL-H-M-0205-1p50\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_H_M_0205_1p51){
		fprintf(fp,"folder = \"FCN/6048s/Horizontal/Qa979/Qc0205/m1p51/\"\n");
		fprintf(fp,"name = \"FN-HL-H-M-0205-1p51\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_H_M_0205_1p70){
		fprintf(fp,"folder = \"FCN/6048s/Horizontal/Qa979/Qc0205/m1p70/\"\n");
		fprintf(fp,"name = \"FN-HL-H-M-0205-1p70\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (FCN_HL_H_M_0205_2p10){
		fprintf(fp,"folder = \"FCN/6048s/Horizontal/Qa979/Qc0205/m2p10/\"\n");
		fprintf(fp,"name = \"FN-HL-H-M-0205-2p10\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_H_M_0205_3p20){
		fprintf(fp,"folder = \"FCN/6048s/Horizontal/Qa979/Qc0205/m3p20/\"\n");
		fprintf(fp,"name = \"FN-HL-H-M-0205-3p20\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_H_M_0205_3p90 ){
		fprintf(fp,"folder = \"FCN/6048s/Horizontal/Qa979/Qc0205/m3p90/\"\n");
		fprintf(fp,"name = \"FN-HL-H-M-0205-3p90\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_H_M_0205_8p20 ){
		fprintf(fp,"folder = \"FCN/6048s/Horizontal/Qa979/Qc0205/m8p20/\"\n");
		fprintf(fp,"name = \"FN-HL-H-M-0205-8p20\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_V_M_0205_0p90 ){
		fprintf(fp,"folder = \"FCN/6048s/Vertical/Qa979/Qc0205/m0p90/\"\n");
		fprintf(fp,"name = \"FN-HL-V-M-0205-0p90\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (FCN_HL_V_M_0205_1p60 ){
		fprintf(fp,"folder = \"FCN/6048s/Vertical/Qa979/Qc0205/m1p60/\"\n");
		fprintf(fp,"name = \"FN-HL-V-M-0205-1p60\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_V_M_0205_3p10 ){
		fprintf(fp,"folder = \"FCN/6048s/Vertical/Qa979/Qc0205/m3p10/\"\n");
		fprintf(fp,"name = \"FN-HL-V-M-0205-3p10\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_HL_V_M_0205_4p00 ){
		fprintf(fp,"folder = \"FCN/6048s/Vertical/Qa979/Qc0205/m4p00/\"\n");
		fprintf(fp,"name = \"FN-HL-V-M-0205-4p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_TP_H_M_0205_10p0 ){
		fprintf(fp,"folder = \"FCN/Talc/Horizontal/Qa979/Qc0205/m10p0/\"\n");
		fprintf(fp,"name = \"FN-TP-H-M-0205-10p0\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_TP_H_M_0205_2p10 ){
		fprintf(fp,"folder = \"FCN/Talc/Horizontal/Qa979/Qc0205/m2p10/\"\n");
		fprintf(fp,"name = \"FN-TP-H-M-0205-2p10\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (FCN_TP_H_M_0205_5p10 ){
		fprintf(fp,"folder = \"FCN/Talc/Horizontal/Qa979/Qc0205/m5p10/\"\n");
		fprintf(fp,"name = \"FN-TP-H-M-0205-5p10\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_TP_V_M_0205_10p4 ){
		fprintf(fp,"folder = \"FCN/Talc/Vertical/Qa979/Qc0205/m10p4/\"\n");
		fprintf(fp,"name = \"FN-TP-V-M-0205-10p4\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (FCN_TP_V_M_0205_2p00 ){		
		fprintf(fp,"folder = \"FCN/Talc/Vertical/Qa979/Qc0205/m2p00/\"\n");
		fprintf(fp,"name = \"FN-TP-V-M-0205-2p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (FCN_TP_V_M_0205_4p30 ){
		fprintf(fp,"folder = \"FCN/Talc/Vertical/Qa979/Qc0205/m4p30/\"\n");
		fprintf(fp,"name = \"FN-TP-V-M-0205-4p30\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_AH_D_H_0000_10p7 ){
		fprintf(fp,"folder = \"SFPJ/ballAH/Dual/Qa1100/Qc0000/m10p7/\"\n");
		fprintf(fp,"name = \"PJ-AH-D-H-0000-10p7\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_AH_D_H_0000_4p00 ){
		fprintf(fp,"folder = \"SFPJ/ballAH/Dual/Qa1100/Qc0000/m4p00/\"\n");
		fprintf(fp,"name = \"PJ-AH-D-H-0000-4p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_AH_D_H_0000_8p70 ){
		fprintf(fp,"folder = \"SFPJ/ballAH/Dual/Qa1100/Qc0000/m8p70/\"\n");
		fprintf(fp,"name = \"PJ-AH-D-H-0000-8p70\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_AH_D_H_0995_8p00 ){
		fprintf(fp,"folder = \"SFPJ/ballAH/Dual/Qa1100/Qc0995/m8p00/\"\n");
		fprintf(fp,"name = \"PJ-AH-D-H-0995-8p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_AH_D_H_1350_16p0 ){		
		fprintf(fp,"folder = \"SFPJ/ballAH/Dual/Qa1100/Qc1350/m16p0/\"\n");
		fprintf(fp,"name = \"PJ-AH-D-H-1350-16p0\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_AH_D_H_1350_2p40 ){
		fprintf(fp,"folder = \"SFPJ/ballAH/Dual/Qa1100/Qc1350/m2p40/\"\n");
		fprintf(fp,"name = \"PJ-AH-D-H-1350-2p40\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_AH_D_H_1350_9p00 ){
		fprintf(fp,"folder = \"SFPJ/ballAH/Dual/Qa1100/Qc1350/m9p00/\"\n");
		fprintf(fp,"name = \"PJ-AH-D-H-1350-9p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_0000_2p20 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0000/m2p20/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0000-2p20\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_0000_2p40 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0000/m2p40/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0000-2p40\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_0000_2p60 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0000/m2p60/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0000-2p60\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_0102_2p80 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0102/m2p80/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0102-2p80\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_0255_3p20 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0255/m3p20/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0255-3p20\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_0255_4p80 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0255/m4p80/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0255-4p80\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_0467_3p30 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0467/m3p30/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0467-3p30\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_0540_0p49 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0540/m0p49/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0540-0p49\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_0540_0p70 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0540/m0p70/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0540-0p70\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_0540_0p75 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0540/m0p75/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0540-0p75\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_0540_1p20 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0540/m1p20/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0540-1p20\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_0540_1p50 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0540/m1p50/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0540-1p50\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_0540_2p10 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0540/m2p10/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0540-2p10\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_0540_3p20 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0540/m3p20/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0540-3p20\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_0540_5p50 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0540/m5p50/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0540-5p50\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_0540_8p00 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0540/m8p00/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0540-8p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_0730_3p40 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0730/m3p40/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0730-3p40\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_0995_3p50 ){		
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc0995/m3p50/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-0995-3p50\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_1350_0p90 ){		
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc1350/m0p90/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-1350-0p90\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_1350_10p0 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc1350/m10p0/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-1350-10p0\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_1350_2p00 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc1350/m2p00/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-1350-2p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_1350_2p80 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc1350/m2p80/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-1350-2p80\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_H_1350_3p20 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc1350/m3p20/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-1350-3p20\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_1350_3p30 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc1350/m3p30/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-1350-3p30\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_H_1350_4p90 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa1100/Qc1350/m4p90/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-H-1350-4p90\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_HH_D_L_0730_4p40 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa550/Qc0730/m4p40/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-L-0730-4p40\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_HH_D_L_0730_9p60 ){
		fprintf(fp,"folder = \"SFPJ/5070s/Dual/Qa550/Qc0730/m9p60/\"\n");
		fprintf(fp,"name = \"PJ-HH-D-L-0730-9p60\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_TP_D_H_0000_2p50 ){
		fprintf(fp,"folder = \"SFPJ/Talc/Dual/Qa1100/Qc0000/m2p50/\"\n");
		fprintf(fp,"name = \"PJ-TP-D-H-0000-2p50\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_TP_D_H_0000_7p40 ){
		fprintf(fp,"folder = \"SFPJ/Talc/Dual/Qa1100/Qc0000/m7p40/\"\n");
		fprintf(fp,"name = \"PJ-TP-D-H-0000-7p40\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_TP_D_H_0540_10p9 ){	
		fprintf(fp,"folder = \"SFPJ/Talc/Dual/Qa1100/Qc0540/m10p9/\"\n");
		fprintf(fp,"name = \"PJ-TP-D-H-0540-10p9\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_TP_D_H_0540_7p00 ){
		fprintf(fp,"folder = \"SFPJ/Talc/Dual/Qa1100/Qc0540/m7p00/\"\n");
		fprintf(fp,"name = \"PJ-TP-D-H-0540-7p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_TP_D_H_0995_12p0 ){
		fprintf(fp,"folder = \"SFPJ/Talc/Dual/Qa1100/Qc0995/m12p0/\"\n");
		fprintf(fp,"name = \"PJ-TP-D-H-0995-12p0\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	} 
	if (SFPJ_TP_D_H_1350_4p00 ){
		fprintf(fp,"folder = \"SFPJ/Talc/Dual/Qa1100/Qc1350/m4p00/\"\n");
		fprintf(fp,"name = \"PJ-TP-D-H-1350-4p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	if (SFPJ_TP_D_H_1350_8p00){
		fprintf(fp,"folder = \"SFPJ/Talc/Dual/Qa1100/Qc1350/m8p00/\"\n");
		fprintf(fp,"name = \"PJ-TP-D-H-1350-8p00\"\n");
		fprintf(fp, "EXE %s\n", nephmacrobuff);
	}
	// Close the file and flush the system then leave
	fclose(fp);
	settext(&scratch);
	fileflush(&scratch);
	
	return OMA_OK;
}

int neph_reckon()
{
	extern int execut_a_macfile();
	
	neph_init(1);
	if (Use_Nozzle) neph_set_nozzle();
	if (Use_Particles) neph_set_particles();
	if (Use_Laser) neph_set_laser();
	if (Use_Qa) neph_set_qa();
	if (Use_Qc) neph_set_qc();
	if (Use_Mp) neph_set_Mass();
	neph_count();
	neph_build_control();
	strcpy(scratch, "NCM130206");
	execut_a_macfile(scratch);
	return OMA_OK;
}
//***************************************************************************************//
//**    PK_Neph_EventHandler -                                                                                     **//
//**                 Handle the Modal dialog controls for the atten_param_window               *//
//***************************************************************************************//
pascal OSStatus PK_Neph_EventHandler (EventHandlerCallRef myHandler, EventRef event, void *userData)
{
    OSStatus 		result = eventNotHandledErr;
    HICommand	command;
    Boolean		stopModalLoop = FALSE;
	
    // Get the HI Command
    GetEventParameter (event, kEventParamDirectObject, typeHICommand, NULL,
                       sizeof (HICommand), NULL, &command);
    // Look for commands
	
	
    switch (command.commandID)
    {
        case 'DONE':
            get_Neph_Toggles();
            stopModalLoop = TRUE;
            result = noErr;
			neph_reckon();
            break;
        case 'CNCL':
            stopModalLoop = TRUE;
            result = noErr;
            break;
    }
    // Stop the modal loop.
    if (stopModalLoop)
    {
        QuitAppModalLoopForWindow((WindowRef)userData);
		
    }
    //Return how we handled the event.
    return result;
}

//***************************************************************************************//
//**    PK_Build_Neph_Dialog -                                                                                     **//
//**                 Build an instance of the Atten_Values dialog in the OMA NIB file             *//
//**                 Kalt, P.A.M. & Birzer C.H., (2005) Proceedings of ACLDFMC-4, p73-76.  *//
//***************************************************************************************//
int PK_Build_Neph_Dialog(){
	IBNibRef nibRef;
	
	
    EventTypeSpec 	dialogSpec = {kEventClassCommand, kEventCommandProcess };
    EventHandlerUPP	dialogUPP;
	
    OSStatus err = eventNotHandledErr;
	
    // Get the window from the nib and show it
    err = CreateNibReference( CFSTR("oma"), &nibRef );
    err = CreateWindowFromNib( nibRef, CFSTR("Neph_prefs"), &neph_window );
    DisposeNibReference( nibRef );
	
    // Install our event handler
    dialogUPP =  NewEventHandlerUPP (PK_Neph_EventHandler);
    err = InstallWindowEventHandler (neph_window, dialogUPP, 1, &dialogSpec, (void *) neph_window, NULL);
	
    // set all of the pane values
    set_Neph_Toggles();
	
    ShowWindow(neph_window );
	
    // Run modally
    RunAppModalLoopForWindow(neph_window);
	
    HideWindow(neph_window);
    DisposeWindow(neph_window);
    DisposeEventHandlerUPP(dialogUPP);	
	
	return OMA_OK;
}


//***************************************************************// 
//**   NEPHELOMETRY - Open neph_prefs dialog                        *//
//***************************************************************//
int
nephelometry (int n, int index)	
{	
	
	int Err;
	Err = PK_Build_Neph_Dialog();
	
	return Err;	
}
