/*
OMAX -- Photometric Image Processing and Display
Copyright (C) 2003  by the Developers of OMA

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
#include "SDK.h"

int printf(),pprintf(),kill_vector(),testMain();

#define READMODE O_RDONLY  /* Read file mode. */
/* ********** */

/* Add custom commands to this module. */

/* ********** */


DATAWORD idat(int,int);

/* Declare the routine name in the list below. */


int amask();
int angrad();

int average();
int abelclean();
int abelprep();
int abelrect();

#ifdef ATTENUATE
int attenu();
int attval();
int attprt();
int attscale();
int attipr();
int attlst();
int attdia();
#endif
int abelinv();

int binsiz();
int binbyt();

int cosmo();
int ctrlst();
int count();

int dist2n();

int findpeak();
int fuel();
int fmax_horizontal();
int fmax_vertical();
int fold();

int gnoise();
int getfun();

int get_background();
int gradient_pk();
int getangle();
int getangle2();

int horiso();
int horiso_fill();
int isedge();

int jfcalc();
int jfconst();
int jfstripe();

int kwabel();
int kill_box();

int lookup();
int line();

int makpsz();
int mcalx();
int mconst();

int nan2zero();
int noise();

int parse();
int population();
int pivabs();
int pivbiggest();
int pivcplus();
int pivcminus();
int pivcmul();
int pivcdiv();
int pivdot();
int pivfilter();
int pivflv();
int pivflh();
int pivmaskmul();
int pivmaskdiv();
int pivmax();
int pivmin();
int pivrot();
int pivx2y();
int pivy2x();
int pdfit();
int polarn();
int pkcorr();
int piv_universal_outlier_detection();

int rycalc();

int ramp();
int round_up();
int roundoff();
int ramcalibrate();

int sauter();
int shift();
int scalc();
int scalc1();
//int sccorrect();
int sinkpk();
int smear();
int solget();
int solgf();
int solgb();
int solmak();
int solmul();
int solsavef();
int solspecies();
int square_column_fill();
int stripe();

int tecplot();
int tester();
int test2();
int trace();
int trace2();
int tracpt();
int trpdf();
int tracor();
int trial();
int tsmooth();

int singrd();
int stream();
int strtim();
int swapix();
int shotnoise();

int vertedge();
//int ttask();

#ifdef LAB_JACK
int labjak();
int waithi();
int dout();
#endif


/* Add the names of custom commands to the list below.  The commands should be
	in alphabetical order for the first letter of the command.  Subsequent letters
	in the command name need not be alphabetized. All commands must be 6 letters.
	For shorter commands, fill with spaces. Use all caps. When running the program 
	it doesnÇt matter, but in this list, you must use all caps. 
*/

ComDef    my_commands[] = {
{{"AMASK          "}, amask},
{{"ANGRAD         "}, angrad},
{{"AVERAGE        "}, average},
{{"ABELCLEAN      "}, abelclean},
{{"ABELPREP       "}, abelprep},
{{"ABELRECT       "}, abelrect},
#ifdef ATTENUATE
{{"ATTENUATION    "}, attenu},
{{"ATTDIAMETER    "}, attdia},
{{"ATTIPRESOLUTION"}, attipr},
{{"ATTLSTHICKNESS "}, attlst},
{{"ATTVALUES      "}, attval},
{{"ATTPRINT       "}, attprt},
{{"ATTSCALE       "}, attscale},
#endif
{{"ABELINV        "}, abelinv},
	
{{"COSMOOTH       "}, cosmo},
{{"COUNT          "}, count},
	
{{"CTRLST         "}, ctrlst},
{{"DIST2N         "}, dist2n},
#ifdef LAB_JACK
{{"DOUT           "}, dout},
#endif
{{"FINDPEAK       "}, findpeak},
{{"FMAXHORIZ      "}, fmax_horizontal},
{{"FMAXVERT       "}, fmax_vertical},
{{"FUEL           "}, fuel},
{{"FOLD           "}, fold},
    
{{"GNOISE         "}, gnoise},
{{"GETFUNCT       "}, getfun},
{{"GETBKG         "}, get_background},
{{"GRADPK         "}, gradient_pk},
{{"GETANGLE       "}, getangle},
{{"GETANGLE2      "}, getangle2},
{{"HORISO         "}, horiso},
{{"HORFILL        "}, horiso_fill},
{{"ISEDGE         "}, isedge},
{{"JFCALC         "}, jfcalc},
{{"JFCONS         "}, jfconst},
{{"JFSTRIPE       "}, jfstripe},
{{"KWABEL         "}, kwabel},
{{"KILBOX         "}, kill_box},
{{"LOOKUP         "}, lookup},
{{"LINE           "},line},
#ifdef LAB_JACK
{{"LABJAK         "}, labjak},
#endif
{{"MAKPSZ         "},makpsz},
{{"MCALX          "}, mcalx},
{{"MCONST         "}, mconst},
{{"NAN2ZERO       "}, nan2zero},	
{{"NOISE          "}, noise},
{{"PARSE          "},  parse},
{{"POPULN         "}, population},
{{"PIVABS         "}, pivabs},
{{"PIVBIG         "}, pivbiggest},
{{"PIVDOT         "}, pivdot},
{{"PIVFLH         "}, pivflh},
{{"PIVFLV         "}, pivflv},
{{"PIVFTR         "}, pivfilter},
{{"PIVMAX         "}, pivmax},
{{"PIVMIN         "}, pivmin},
{{"PIVM*          "}, pivmaskmul},
{{"PIVM/          "}, pivmaskdiv},
{{"PIVROT         "}, pivrot},
{{"PIVX2Y         "}, pivx2y},
{{"PIVY2X         "}, pivy2x},
{{"PIVC+          "}, pivcplus},
{{"PIVC-          "}, pivcminus},
{{"PIVC*          "}, pivcmul},
{{"PIVC/          "}, pivcdiv},
{{"PIVUODETECT    "}, piv_universal_outlier_detection},
{{"PDFIT          "}, pdfit},
{{"POLARN         "}, polarn},

{{"RYCALC         "}, rycalc},
{{"RAMP           "},ramp},
{{"RNDOFF         "}, roundoff},
{{"RNDUP          "}, round_up},
{{"RAMCALIBRATE   "}, ramcalibrate},

{{"SAUTER         "},     sauter},
{{"SHIFT          "},        shift},
{{"SCALC          "}, scalc},
{{"SCALC1         "}, scalc1},
//"SCCORR         "}, sccorrect,
{{"SMEAR          "}, smear},
{{"SOLGET         "}, solget},
{{"SOLGF          "}, solgf},
{{"SOLGB          "}, solgb},
{{"SOLMAKE        "}, solmak},
{{"SOLMULT        "}, solmul},
{{"SOLSAVEF       "}, solsavef},
{{"SOLSPECIES     "}, solspecies},
{{"STRIPE         "}, stripe},
{{"SINGRD         "},singrd},
{{"SINKPK         "},sinkpk},
{{"STREAM         "},stream},
{{"STRTIM         "},strtim},
{{"SWAPIX         "}, swapix},
{{"SHOTNOISE      "}, shotnoise},
{{"TECPLT         "}, tecplot},
{{"TESTER         "}, pkcorr},
#ifdef Mac
{{"TEST2          "}, test2},
#endif
//"TTASK          "}, ttask,
{{"TRACE          "}, trace},
{{"TRACE2         "},trace2},
{{"TRACOR         "},tracor},
{{"TRIAL          "}, trial},
{{"TRACPT         "},tracpt},
{{"TRPDF          "},trpdf},
{{"TSMOOTH        "},tsmooth},
{{"VERTEDGE       "},vertedge},
#ifdef LAB_JACK
{{"WAITHI         "},waithi},
#endif

{{{0,0,0,0,0,0,0}},0}};
		

/* Now the code for the routines */
/*----------------------------------------------------------------*/
#ifdef LAB_JACK
int
labjak(n,index)	/*    test labjack */
int n,index;	
{
	testMain();
	return 0;
}

#endif
/*----------------------------------------------------------------*/
/*
 RAMCALIBRATE laser_wavelength rayleigh_pixel nitrogen_pixel
 
 Provide calibration data for identifying features in a Raman spectrum
 Arguments are:
	laser_wavelength
	rayleigh_pixel -> the x pixel location of the peak of the Rayleigh line. This can be float if you want to be accurate.
	nitrogen_pixel -> the x pixel location of the peak of the N2 Raman line. This can be float too.
	The horizontal direction is assumed to correspond to the wavelength axis.
	If rotation is needed, do that first for accurate results.
 
 After this command is given, dragging the + cursor tool will read out the wavelength (nm) and shift (cm-1) as well as x and y pixel locations.
 To undefine the constants and stop showing wavelength and shift, give the command without arguments.
	 
*/

float nitrogen_pixel,rayleigh_pixel,laser_wavelength;
int report_spectral_info = 0;

int
ramcalibrate(int n, int index)
{
	int i,narg = 0;
	
	// Check to see if there were arguments
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			narg = sscanf(&cmnd[index],"%f %f %f",&laser_wavelength,&rayleigh_pixel,&nitrogen_pixel);
			break;
		}
	}
	if(narg == 0) report_spectral_info = 0;
	else if(narg == 3) {
		report_spectral_info = 1;
	}
	return 0;


}
/*----------------------------------------------------------------*/
int
findpeak(int n, int index)	/*    locates peak within window designated (Center, +, -) */
{
	long int i;
    int center=0; int start=0; int end=0; int peak_loc=0;
	float peak=0, sum=0;
	int side_pix = 2;		// get COM from this many pixels on either side of the peak
	extern Variable user_variables[];
	
	if (n <= 0) {
		printf("FINDP requires estimate of peak location and the + and - limits to\n");
		printf("locate the peak: i.e. findp 179 -5 +5\n");
		beep();
		return -1;
	}
	
	/* Check to see if there were arguments */
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d %d",&center,&start,&end);
			break;
		}
	}
	peak = center+start;
	for (i=center+start; i<=center+end; i++){
		if (idat(0,peak_loc) < idat(0,i)) {
			peak_loc = i;
		}
	}
	
	for(i= peak_loc-side_pix; i<= peak_loc+side_pix; i++){
		sum = sum + idat(0,i);
		peak = peak + i*idat(0,i);
	}
	peak /= sum;
	pprintf("peak:\t%f\tlocation:\t%.2f\n",idat(0,peak_loc),peak);
	user_variables[0].fvalue = idat(0,peak_loc);
	user_variables[0].is_float = 1;
	user_variables[1].fvalue = peak;
	user_variables[1].is_float = 1;
	
	return 0;

}

/*----------------------------------------------------------------*/
int
scalc(int n,int index)	/*   Spectra Calc - locates peak and centers on it of findpeak != 0 */
{
	float com,ave,rms;		/* centroid coordinates,average, and rms */
	long int tmp,icount,i,datval;
	int center,start, end,peak, findpeak;
	int sf;
	
	extern char unit_text[];
	extern int ruler_scale_defined;
	extern float ruler_scale;
	
	if (n <= 0) {
		printf("SCALC requires estimate of peak location, the + and - limits and whether to:\n");
		printf("locate the peak (1, default) or not (0) : i.e. 1dcalc 179 -5 +5 1\n");
		beep();
		return -1;
	}
	
	/* Check to see if there were arguments */
	findpeak = 1;
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d %d %d",&center,&start,&end,&findpeak);
			break;
		}
	}
	if (findpeak != 0){
		peak = center+start;
		for (i=center+start; i<=center+end; i++){
			if (idat(0,peak) < idat(0,i)) {
				peak = i;
			}
		}
	}else{
		peak = center;
	}
	
	sf = trailer[SFACTR];
	tmp = icount = 0;
	com = ave = rms = 0.0;
	for(i=peak+start; i<=peak+end; i++) {
			datval = sf*idat(0,i);
			tmp += datval;					/* average */
			com += i * datval;				/* center of mass */
			rms += datval*datval;			/* rms */
			icount++;						/* number of points */
	}
	com /= tmp;
	ave = (float)tmp/(float)icount;
	rms = rms/(float)icount - ave*ave;	
	rms = sqrt(rms);
	pprintf("Sum:\t%d\tAve:\t%g\trms\t%g\t# Pts:\t",tmp,ave,rms);
	pprintf("%d\tpeak:\t%d",icount,peak);
	if( ruler_scale_defined ) {
		com /= ruler_scale;
	}
	pprintf("\tCOM:\t%.2f",com);
	if( ruler_scale_defined && unit_text[0] )
		pprintf("\t%s\n",unit_text);
	else
		pprintf("\n");
	return 0;
}

/*----------------------------------------------------------------*/
int
scalc1(int n, int index)	/*   Spectra Calc - locates peak and centers on it of findpeak != 0 */
{
	float com,ave,rms;		/* centroid coordinates,average, and rms */
	int tmp,icount,i,datval;
	int center,start, end,peak, refpeak, left,right;
	int sf;
	
	extern char unit_text[];
	extern int ruler_scale_defined;
	extern float ruler_scale;
	
	if (n <= 0) {
		printf("SCALC requires estimate of peak location, the + and - limits and a:\n");
		printf("reference peak : i.e. scalc 203 -5 +5 203\n");
		beep();
		return -1;
	}
	
	/* Check to see if there were arguments */
	refpeak = 0;
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d %d %d",&center,&start,&end,&refpeak);
			break;
		}
	}
	if (refpeak != 0){
		peak = refpeak+start;
		left = peak;
		right = refpeak+end;
	}else{
		peak = center+start;
		left = peak;
		right = center+end;
	}
	
	for (i=left; i<=right; i++){
		if (idat(0,peak) < idat(0,i)) {
			peak = i;
		}
	}
	printf("ref %d,center%d,peak %d\n",refpeak,center,peak);
	if (refpeak != 0) {
		peak = center - refpeak + peak;
	}
	sf = trailer[SFACTR];
	tmp = icount = 0;
	com = ave = rms = 0.0;
	for(i=peak+start; i<=peak+end; i++) {
			datval = sf*idat(0,i);
			tmp += datval;					/* average */
			com += i * datval;				/* center of mass */
			rms += datval*datval;			/* rms */
			icount++;						/* number of points */
	}
	com /= tmp;
	ave = (float)tmp/(float)icount;
	rms = rms/(float)icount - ave*ave;	
	rms = sqrt(rms);
	pprintf("Sum:\t%d\tAve:\t%.2f\trms\t%.2f\t# Pts:\t",tmp,ave,rms);
	pprintf("%d\tpeak:\t%d",icount,peak);
	if( ruler_scale_defined ) {
		com /= ruler_scale;
	}
	pprintf("\tCOM:\t%.2f",com);
	if( ruler_scale_defined && unit_text[0] )
		pprintf("\t%s\n",unit_text);
	else
		pprintf("\n");
	return 0;
}

/*
Command removed 10/2004

sccorrect(n)

int n;

{
   	extern TWOBYTE header[];
    extern TWOBYTE trailer[];
    extern DATAWORD *datpt,*backdat,min,max;
	extern unsigned int backsize;
	extern int		maxint,npts,doffset;
	extern float scalarvalue;
	
	DATAWORD *datptcopy,*bcopy;
	int x,nc;
	float sc;
	
	if(backdat == 0) {
		beep();
		printf("Must Define Background.\n");
		return -1;
	}
	if ((header[NCHAN]*header[NTRAK]+MAXDOFFSET)*DATABYTES != backsize) {
		beep();
		printf("Background Size Conflicts With Data.\n");
		return -1;
	}
	if(n==0) n=1;
	datptcopy = datpt + doffset;
	bcopy = backdat + doffset;
		
	sc=n/scalarvalue;
	
	for(nc=doffset; nc<doffset + header[NCHAN] * header[NTRAK]; nc++) {
		x = ( *(datptcopy) - *(bcopy++)) * sc;
		*(datptcopy++) = x;
	}
	
    maxx();
	
	setarrow();
	return 0;	

}
*/
/*________________________________________________________________________*/
/* ************* 
Jonathan Frank's fcalc routines -- take into account functional form of aT, W and Cp

commands are	 OLD	 NEW
				fcalc	jfcalc
				fconst	jfcons

  ********** */
#define GET_F	1
#define GET_T	2
#define GET_ITERATION	3
#define GET_ERROR 4
#define GET_WMIX 5
#define GET_YF 6
#define GET_AT 7

#define CPFIT 6
#define RAXFIT 8
#define FMAPFIT 9
#define WMIXFIT 5

float *RaX_a,*Cp_a,*Fmap_a,Q,RCor,*Wmix_a;
float Cp(float),RaX(float),Fmap(float);

extern int fcalc_case;
extern float Fcal,W1,W2,Rc1,Rc2,T1,T2,Yf1,Wfu,Xfu,Err,Tad,FlRich,Tcal;
extern int have_constants;
extern int MaxIt,start_over;

int
jfcalc(int n,int index)			/* Using the data in the current buffer as rayleigh input and
						   the specified file as the fluorescence input, create the conserved
						   scalar image */
{
	extern float *RaX_a,*Cp_a,*Fmap_a;
	int i,j,nt,nc;
	DATAWORD *raypoint,*flpoint=0;
	unsigned int  raysize;
	TWOBYTE	rayxsize,rayysize;	
	float f0,aT,Wmix,x,aT1,aT2,fold=0,D2,denom,fluor,ray,T,Yf=0;
	float r,aTH,aTC,WmixC,WmixH,Cpmix,CpH,CpC;
	float f = 0.0;
	int notgreat = 0;
	
	// for third scalar
	DATAWORD *npoint=0;
	int havethird = 0;
	float D3,Yna,denom2;
	float Ynf=0.0;
	
	if(have_constants == 0) {
		beep();
		printf(" Load Constants First (use FCONST filename).\n");
		return -1;
	}
	
	/* copy current data into the rayleigh array */
	
	raysize = header[NCHAN]*header[NTRAK];
	rayxsize = header[NCHAN];
	rayysize = header[NTRAK];
		
	raypoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(raypoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++)
		*(raypoint + i) = *(datpt+i+doffset);
	
	/* Get the fuel image */
	
	for(i=0; cmnd[index] != ' ' && cmnd[index] != EOL; txt[i++] = cmnd[index++] ) ;		// treat space as filename delimitor here
	txt[i] = 0;
	index++;
			
	fullname(txt,GET_DATA);
	getfile (0,-1);			/* get the file */
	/* keylimit(-2);			 reset printing  to previous mode*/

	if( (rayxsize != header[NCHAN]) || (rayysize != header[NTRAK]) ) {
			beep();
			printf("Image sizes are different. \n");
			goto cleanup;
	}
	
	/* copy current data into the fuel array */
	
		
	flpoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(flpoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++)
		*(flpoint + i) = *(datpt+i+doffset);
		
		
	/* Now try to Get the N2 Raman image */
	
	for(i=0; cmnd[index] != EOL; txt[i++] = cmnd[index++] ) ;
	txt[i] = 0;
	npoint = 0;
	fullname(txt,GET_DATA);
	if ( getfile (0,-1) != -1) {
		printf("Three scalar.\n");
		
		if( (rayxsize != header[NCHAN]) || (rayysize != header[NTRAK]) ) {
			beep();
			printf("Image sizes are different. \n");
			goto cleanup;
		}
		/* copy current data into third scalar array */
		npoint = (DATAWORD*) malloc(raysize*DATABYTES);

		if(npoint == 0) {
			nomemory();
			goto cleanup;
		}

		for(i=0; i<raysize; i++)
			*(npoint + i) = *(datpt+i+doffset);
			
		havethird = 1;
	}
	
	/* now do the calcs for each point */
	
	i = 0;
	aT2 = T2*Rc2;
	aT1 = T1*Rc1;
	Wmix = W2;
	aT = aT2;
	T = T2;
	
	
	Yna = 0.767; // Nitrogen mass fraction in air
	denom2 = Ynf - Yna;
	D3 = (Tcal * Fcal) / (28.0 * 0.79);	/* N2 Raman calibration - assume calibrations
											done at same temp., and hardwire XN2 in air*/
	
	denom = Yf1 + (Cp(1.000) * T1 - Cp(0.000) * T2) / Q;
	D2 = (Tcal * Fcal) / (Wfu * Xfu);	/* fluorescence (fuel) calibration */
	
	
	for( nt = 0; nt<header[NTRAK]; nt++) {
		for( nc = 0; nc<header[NCHAN]; nc++) {
		
			fluor = *(flpoint+i);
			ray = *(raypoint+i);
			
			if( start_over ) { /* start from the initial condition each time */
				Wmix = W2;
				T = T2;
				aT = aT2;
				f0 = 0.000;
			} else {
				f0 = f;
			}

			f = fluor / D2 / Wmix * aT / ray + (Cp(f0) * aT / ray - Cp(0.000) * T2) / Q;
			f /= denom;
				
			for(j=0; j<MaxIt; j++) {
				
				fold = f;
				
				WmixC = 1./ (f/W1 + (1.0-f)/W2);
				x = f * WmixC/W1;					// x is fuel mole fraction

				aTC = (x) * aT1 + (1-x) * aT2;
				
				//-T = aTC/ray;
				T = aT/ray;

				r = (T-T2)/(Tad-T2);  /* reactedness */
				if(r>1.0) r= 1.0;
				if(r<0.0) r= 0.0;				

				aTH = aT2 * RaX(f);
				aT = (1-r)*aTC + r*aTH;
				
				/*
				if(aT/ray > Tad)
					aT = Tad * ray;		// force temp below Tad
				*/

				// WmixH = 28.8135 - 7.93551*f + 3.67441*f*f; Note: this was for a 1/3 ch4/air
				
				WmixH = Wmix_a[0] + Wmix_a[1]*f + Wmix_a[2]*f*f + Wmix_a[3]*f*f*f+ Wmix_a[4]*f*f*f*f;
				Wmix = (1-r)*WmixC + r*WmixH;
				
				
				if ( fluor < FlRich ) {  /* if on the lean side */ 
					/* aT = aT * (1.0 - 0.18*(T - T2)/(Tad - T2)); */
				}
												
				Yf = fluor / D2 / Wmix * aT / ray;
				
				Yf = Yf * (1 + RCor * r * r);	// correction for fuel loss
				if(Yf>1.0) Yf= 1.0;
				if(Yf<0.0) Yf= 0.0;				
				
/*				tau = (T - T1)/(Tad - T1);
				if (f>0.254 && T>360) {
					Yf = 0.00000253667 + 0.00437021*tau + -0.00277642*tau*tau + -0.00165348*tau*tau*tau + Yf*Yf1*(1.0-tau); 
					if (Yf<0.0)
						Yf = 0.0;
					else
						Yf = sqrt(Yf);
				}
*/			
/*				if (f>=0.420 && T>360) {
					Yf += 0.000108297 + 0.0121285*tau + 0.0404846*tau*tau - 0.0317741*tau*tau*tau; 
				}
				if (f<0.420 && f>=.294) {
					Yf +=  -78.2219 + 340.802*tau + -556.675*tau*tau + 404.154*tau*tau*tau - 110.046*tau*tau*tau*tau; 
				}
				if (f<0.294 && f>=.254) {
					Yf +=  -222.579 + 682.878*tau + -698.562*tau*tau + 238.274*tau*tau*tau; 
				}

				if (Yf<0.0)
					Yf = 0.0;
*/					
				CpH = Cp(fold);
				CpC = f*Cp(1.00) + (1-f)*Cp(0.00);
				Cpmix = (1-r)*CpC + r*CpH;
				
				
				f = Yf + (Cpmix * aT / ray - Cp(0.000) * T2) / Q;
				f /= denom;
				if(f>1.0) f= 1.0;
				if(f<0.0) f= 0.0;				
								
				if( fabs((f - fold)/f) < Err ) break;
			}

			f = Fmap(f); /* Remap f to correct for fuel dropout and constant cp (if used above) */
			
			if( fabs((f - fold)/f) > Err ) notgreat++;
			
			switch(fcalc_case) {
			case GET_T:	
				*(datpt+(i++)+doffset) = aT / ray;
				break;
			case GET_ERROR:
				*(datpt+(i++)+doffset) = (f - fold)/f *10000;
				break;
			case GET_ITERATION:
				*(datpt+(i++)+doffset) = j;
				break;
			case GET_WMIX:
				*(datpt+(i++)+doffset) = Wmix * 10;
				break;
			case GET_YF:
				*(datpt+(i++)+doffset) = Yf * 1000.0;
				break;
			case GET_AT:
				*(datpt+(i++)+doffset) = aT /295.0;
				break;
			default:
			case GET_F:
				*(datpt+(i++)+doffset) = f*1000.0 ;
				break;
			}

		}
	}


cleanup:

	free(RaX_a);
	free(Cp_a);
	free(Fmap_a);
	if(raypoint!=0) {
		free(raypoint);	
		raypoint = 0;
	}
	if(flpoint!=0) {
		free(flpoint);	
		flpoint = 0;
	}
	if(npoint!=0) {
		free(npoint);	
		npoint = 0;
	}
	have_max = 0;
	pprintf(" %d questionable points.\n",notgreat);
	return 0;
}

// -----------------
int
rycalc(int n,int index)			/* Using the data in the current buffer as rayleigh input and
						   the specified file as the fluorescence input, create the conserved
						   scalar image */
{
	extern float *RaX_a,*Cp_a,*Fmap_a;
	int i,j,nt,nc;
	DATAWORD *raypoint,*flpoint=0;
	unsigned int  raysize;
	TWOBYTE	rayxsize,rayysize;	
	float f0,aT,Wmix,x,aT1,aT2,fold=0,D2,denom,fluor,ray,T,Yf=0;
	float r,aTH,aTC,WmixC,WmixH,Cpmix,CpH,CpC;
	float f = 0.0;
	float rayf,raymin;
	float Yfmin,rmin,close;
	int notgreat = 0;
	
	
	float tabfun(int n, float x);
	int richside=0;

    extern char    cmnd[],*fullname();
	
	if(have_constants == 0) {
		beep();
		printf(" Load Constants First (use FCONST filename).\n");
		return -1;
	}
	
	/* copy current data into the rayleigh array */
	
	raysize = header[NCHAN]*header[NTRAK];
	rayxsize = header[NCHAN];
	rayysize = header[NTRAK];
		
	raypoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(raypoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++)
		*(raypoint + i) = *(datpt+i+doffset);
	
	/* Get the fuel image */
	
	for(i=0; cmnd[index] != ' ' && cmnd[index] != EOL; txt[i++] = cmnd[index++] ) ;		// treat space as filename delimitor here
	txt[i] = 0;
	index++;
			
	fullname(txt,GET_DATA);
	getfile (0,-1);			/* get the file */
	/* keylimit(-2);			// reset printing  to previous mode*/

	if( (rayxsize != header[NCHAN]) || (rayysize != header[NTRAK]) ) {
			beep();
			printf("Image sizes are different. \n");
			goto cleanup;
	}
	
	/* copy current data into the fuel array */
	
		
	flpoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(flpoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++)
		*(flpoint + i) = *(datpt+i+doffset);
		
		
	/* now do the calcs for each point */
	
	i = 0;
	aT2 = T2*Rc2;
	aT1 = T1*Rc1;
	Wmix = W2;
	aT = aT2;
	T = T2;
		
	denom = Yf1 + (Cp(1.000) * T1 - Cp(0.000) * T2) / Q;
	D2 = (Tcal * Fcal) / (Wfu * Xfu);	/* fluorescence (fuel) calibration */
	
	raymin = Fmap_a[1];	// calcs say this is minimum rayleigh
	Yfmin = Fmap_a[3];
	rmin = Fmap_a[4];		
	close = Fmap_a[5];
	
	for( nt = 0; nt<header[NTRAK]; nt++) {
		richside = Fmap_a[2];				// specifies whether we start rich or lean on each row
		for( nc = 0; nc<header[NCHAN]; nc++) {
		
			fluor = *(flpoint+i);
			ray = *(raypoint+i);
			
			if( start_over ) { /* start from the initial condition each time */
				Wmix = W2;
				T = T2;
				aT = aT2;
				f0 = 0.000;
			} else {
				f0 = f;
			}

			f = tabfun(richside,ray/Rc2);	
			for(j=0; j<MaxIt; j++) {
				
				fold = f;
				
				WmixC = 1./ (f/W1 + (1.0-f)/W2);
				x = f * WmixC/W1;					// x is fuel mole fraction

				aTC = (x) * aT1 + (1-x) * aT2;
				
				//-T = aTC/ray;
				T = aT/ray;

				r = (T-T2)/(Tad-T2);  /* reactedness */
				if(r>1.0) r= 1.0;
				if(r<0.0) r= 0.0;				

				aTH = aT2 * RaX(f);
				aT = (1-r)*aTC + r*aTH;
				
				WmixH = Wmix_a[0] + Wmix_a[1]*f + Wmix_a[2]*f*f + Wmix_a[3]*f*f*f+ Wmix_a[4]*f*f*f*f;
				Wmix = (1-r)*WmixC + r*WmixH;				
																
				Yf = fluor / D2 / Wmix * aT / ray;
				
				Yf = Yf * (1 + RCor * r * r);	// correction for fuel loss
				if(Yf>1.0) Yf= 1.0;
				if(Yf<0.0) Yf= 0.0;				
									
				CpH = Cp(fold);
				CpC = f*Cp(1.00) + (1-f)*Cp(0.00);
				Cpmix = (1-r)*CpC + r*CpH;				
				
				f = Yf + (Cpmix * aT / ray - Cp(0.000) * T2) / Q;
				f /= denom;
				if(f>1.0) f= 1.0;
				if(f<0.0) f= 0.0;	
				
				rayf = ray/Rc2;		// fraction of air rayleigh signal

				if(richside == 1){
					// have we crossed over to the lean side?
					if( Yf < Yfmin && r > rmin && fabs(rayf-raymin) < close)
					//if( Yf < Yfmin)
						richside = 0;
				
				} else {
					// have we crossed over to the rich side?
					if( Yf > Yfmin && r < rmin && fabs(rayf-raymin) < close)
					//if( Yf > Yfmin)
						richside = 1;		
				}
				
				f = tabfun(richside,rayf);
			
				if( fabs((f - fold)/f) < Err ) break;
			}

			//f = Fmap(f); /* Remap f to correct for fuel dropout and constant cp (if used above) */
			
			if( fabs((f - fold)/f) > Err ) notgreat++;
			
			//printf("%f %f %f f fr rayf\n",f,fr,rayf);
			
			//f = fr;
			
			switch(fcalc_case) {
			case GET_T:	
				*(datpt+(i++)+doffset) = aT / ray;
				break;
			case GET_ERROR:
				*(datpt+(i++)+doffset) = (f - fold)/f *10000;
				break;
			case GET_ITERATION:
				*(datpt+(i++)+doffset) = j;
				break;
			case GET_WMIX:
				*(datpt+(i++)+doffset) = Wmix * 10;
				break;
			case GET_YF:
				*(datpt+(i++)+doffset) = Yf * 1000.0;
				break;
			case GET_AT:
				*(datpt+(i++)+doffset) = aT /295.0;
				break;
			default:
			case GET_F:
				*(datpt+(i++)+doffset) = f*1000.0 ;
				break;
			}

		}
	}


cleanup:

	free(RaX_a);
	free(Cp_a);
	free(Fmap_a);
	if(raypoint!=0) {
		free(raypoint);	
		raypoint = 0;
	}
	if(flpoint!=0) {
		free(flpoint);	
		flpoint = 0;
	}
	have_max = 0;
	pprintf(" %d questionable points.\n",notgreat);
	return 0;
}


float Cp( float f)

{
	float cp = 0.00;
	int i;
	
	for( i=0; i< CPFIT; i++)
		cp += Cp_a[i] * pow(f,(float)i);
		
	return (cp);
	
}

float RaX( float f)

{
	float rax,cutoff;
	
	cutoff = RaX_a[0];
	if (f > cutoff) {
		rax = RaX_a[3]*f*f + RaX_a[2]*f + RaX_a[1];
	} else {
		rax = RaX_a[7]*f*f*f + RaX_a[6]*f*f + RaX_a[5]*f + RaX_a[4];
	}
	
	return (rax);
	
}

float Fmap(float f)

{
	float cutoff,x;
	float fnew = 0.00;
	int i;
	
	cutoff = Fmap_a[0];
	if (f > cutoff) {
		for( i=1; i< 5; i++){
			x = (float)(i - 1);
			fnew += Fmap_a[i] * pow(f,x);
		}
	} else {
		for( i=5; i< FMAPFIT; i++){
			x = (float)(i - 5);
			fnew += Fmap_a[i] * pow(f,x);
		}
	}
	
	return (fnew);
	
}


/* ********** */

int
jfconst(int n,int index)			/* Read in constants to be used for FCALC */	
{
	extern char cmnd[],*fullname();
	extern float *RaX_a,*Cp_a,*Fmap_a,*Wmix_a;
	int notfound = 0;
	unsigned int fild;
	int gtfloat(unsigned int,float *),i;
	float tmp;
	
	RaX_a = (float *)malloc(RAXFIT*4);
	Cp_a = (float *)malloc(CPFIT*4);
	Fmap_a = (float *)malloc(FMAPFIT*4);
	Wmix_a = (float *)malloc(WMIXFIT*4);
	
	if(RaX_a == 0 || Cp_a == 0 || Fmap_a == 0 || Wmix_a == 0) {
		nomemory();
		if(RaX_a != 0) free(RaX_a);
		if(Cp_a != 0) free(Cp_a);
		if(Fmap_a != 0) free(Fmap_a);
		if(Wmix_a != 0) free(Wmix_a);
		return -1;
	}


	
	fild = open(fullname(&cmnd[index],GET_DATA),READMODE);


	if( fild != -1) {
		if( gtfloat(fild,&Fcal) != 1) notfound = 1;
		if( gtfloat(fild,&Tcal) != 1) notfound = 1;
		if( gtfloat(fild,&Rc1) != 1) notfound = 1;
		if( gtfloat(fild,&Rc2) != 1) notfound = 1;
		if( gtfloat(fild,&T1) != 1) notfound = 1;
		if( gtfloat(fild,&T2) != 1) notfound = 1;
		if( gtfloat(fild,&W1) != 1) notfound = 1;
		if( gtfloat(fild,&W2) != 1) notfound = 1;
		if( gtfloat(fild,&Q) != 1) notfound = 1;
		for( i=0; i< CPFIT; i++) if( gtfloat(fild,&Cp_a[i]) != 1) notfound = 1;
		for( i=0; i< RAXFIT; i++) if( gtfloat(fild,&RaX_a[i]) != 1) notfound = 1;
		for( i=0; i< FMAPFIT; i++) if( gtfloat(fild,&Fmap_a[i]) != 1) notfound = 1;
		for( i=0; i< WMIXFIT; i++) if( gtfloat(fild,&Wmix_a[i]) != 1) notfound = 1;
		if( gtfloat(fild,&Yf1) != 1) notfound = 1;
		if( gtfloat(fild,&Wfu) != 1) notfound = 1;
		if( gtfloat(fild,&Xfu) != 1) notfound = 1;
		if( gtfloat(fild,&Tad) != 1) notfound = 1;
		if( gtfloat(fild,&FlRich) != 1) notfound = 1;
		if( gtfloat(fild,&RCor) != 1) notfound = 1;
		if( gtfloat(fild,&Err) != 1) notfound = 1;
		if( gtfloat(fild,&tmp) != 1) notfound = 1;
		MaxIt = tmp;
		if( gtfloat(fild,&tmp) != 1) notfound = 1;
		start_over = tmp;
		
    	close(fild);

		if(notfound == 1) {
			beep();
			printf(" Data Format Problem.\n");
			return -1; 
		}
		
		
		printf(" Fluorescence Calibration: %.2f\n",Fcal);
		printf(" Calibration Temp: %.2f\n",Tcal);
		printf(" Rayleigh counts for fuel at T2: %.2f\n",Rc1);
		printf(" Rayleigh counts for air at T1: %.2f\n",Rc2);
		printf(" Temperature of Fuel: %.2f\n",T1);
		printf(" Temperature of Air: %.2f\n",T2);
		printf(" Adiabatic Flame Temperature: %.2f\n",Tad);
		
		printf(" mole weight of fuel stream: %.2f\n",W1);
		printf(" mole weight of air stream: %.2f\n",W2);
		
		printf(" Lower Calorific Value: %.2f\n",Q);
		for( i=0; i< CPFIT; i++) printf(" Cp:a[%1.0d] %.3e\n",i,Cp_a[i]);
		for( i=0; i< WMIXFIT; i++) printf(" Wmix:a[%1.0d] %.3e\n",i,Wmix_a[i]);
		for( i=0; i< RAXFIT; i++) printf(" RaX:a[%1.0d] %.3e\n",i,RaX_a[i]);
		for( i=0; i< FMAPFIT; i++) printf(" Fmap:a[%1.0d] %.3e\n",i,Fmap_a[i]);
		printf(" Y(fu,1): %.2f\n",Yf1);
		printf(" Mole weight of pure fuel: %.2f\n",Wfu);
		
		printf(" Mole fraction of fuel in fluor calib: %.2f\n",Xfu);
		printf(" Rich if fluor is greater than: %.2f\n",FlRich);
		printf(" Fuel loss correction factor %.2f\n",RCor);
		printf(" Max acceptable deviation in f: %.2f\n",Err);

		printf(" Max number of iterations: %d\n",MaxIt);

		have_constants = 1;
		return 0;

	}
	else {
		beep();
		printf(" File Not Found.\n");
		return -1;
	}
	
}

int gtfloat(unsigned int fd,float *x)

{
	int i = 0;
	char j;
	extern char txt[];

	for( read(fd,&j,1); j != '\n' && i < 255; read(fd,&j,1)){
		//printf("%c ",j);
		txt[i++] = j;	
	}

	txt[i] = 0;
	i = sscanf(txt,"%f",x);
	//printf("%d %f\n",i,*x);
	return (i);
	
}
/* ********** */
/*
 GETFUN n filename
	Read in a tabulated function to be used by tabfun routine
	n is function number to be defined
	filename is file containing y=f(x) data pairs, preceeded by number of table entries
	Assumptions for tabulated functions:
	File format:
	first number is integer with the number of pairs to follow
	pairs are float with x followed by f(x) on the same line
	x values are ordered smallest to largest
	y = f(x) is single valued
 
 
 */


float* xptr[10] = {0*10};		// have a maximum of 10 functions
float* yptr[10] = {0*10};
int funsize[10] = {0*10};		// the number of elements in each function

int
getfun(int n,int index)	
{
	int	i,j;
	int ferror = 0;
	FILE *fp;

    	
	if( n<0 || n>9){
		beep();
		printf("Functions must be numbered 0-9.\n");
		return -1;
	}
	while(cmnd[index] != ' ' && cmnd[index] != EOL) index++;
	
	fp = fopen(fullname(&cmnd[++index],GET_DATA),"r");
	if( fp != NULL) {
	
		fscanf(fp,"%d",&i);		// the number of data pairs in the function
		
		if(xptr[n] !=0) free(xptr[n]);
		if(yptr[n] !=0) free(yptr[n]);
		
		xptr[n] = (float*) malloc(i*sizeof(float));
		yptr[n] = (float*) malloc(i*sizeof(float));
		funsize[n] = i;
		if(yptr[n] == 0 || xptr[n]==0) {
			beep();
			printf(" Not enough memory.\n");
			return -1;
		}
		
		
		printf("Will read %d x-y pairs.\n",i);
		
		j=0;
        while ( j < i ) {
                if( fscanf(fp,"%f %f",(xptr[n]+j),(yptr[n]+j)) != 2)
                	ferror = 1;
                //printf("%f %f\n",*(xptr[n]+j),*(yptr[n]+j));
                j++;
		}
		
    	fclose(fp);
		if(ferror == 1){
			beep();
			printf("Data Error.\n");
			if(xptr[n] !=0) free(xptr[n]);
			if(yptr[n] !=0) free(yptr[n]);
			return -1;
		}
		if(*xptr[n] >= *(xptr[n]+1) ){
			beep();
			printf("Incorrect Data Ordering.\n");
			if(xptr[n] !=0) free(xptr[n]);
			if(yptr[n] !=0) free(yptr[n]);
			return -1;
		}
		
		printf("x range is from %g to %g.\n",*(xptr[n]),*(xptr[n]+funsize[n]-1));
		printf("y range is from %g to %g.\n",*(yptr[n]),*(yptr[n]+funsize[n]-1));
		
	}
	else {
		beep();
		printf("Could not open file: %s\n",&cmnd[index]);
		return -1;
	}
	return 0;
}

float tabfun(int n, float x)	// n is the function number, x is the desired x value
{
	/* Assumptions for tabulated functions:
		File format:
		first number is integer with the number of pairs to follow
		pairs are float with x followed by f(x) on the same line
		x values are ordered smallest to largest
		y = f(x) is single valued
	*/

	int i=0;
	float x0,x1,y0,y1,y;
	
	if( x <= *(xptr[n]) )
		return ( *(yptr[n]) );	// less than min x, returns f(min x)
	if( x >= *(xptr[n]+funsize[n]-1) )
		return ( *(yptr[n]+funsize[n]-1) ); // > max x, returns f(max x)
		
	while(  x >= *(xptr[n]+i) )
		i++;
		
		
	x0 = *(xptr[n]+i);
	x1 = *(xptr[n]+i+1);
	y0 = *(yptr[n]+i);
	y1 = *(yptr[n]+i+1);
	y = y0 + (y1-y0)/(x1-x0)*(x-x0);
	y = y0;								// What?
	return(y);
	
}
		 
/* ************* 
End of fcalc routines -- take into account functional form of aT, W and Cp
   ********** */
   
/* ********** */
/*
 LOOKUP

Redefine the current image using the lookup table specified by a tabulated function
Use GETFUN n filename before using this command
Ignore scale factors for now
*/
int 
lookup(int  n)			
{
    float x,tabfun(int, float);
    int i;
    /*
	int oldsf,newsf;

	// Work out (positive) scale factors 
	oldsf = trailer[SFACTR];
	newsf = (max*oldsf+n)/maxint;
	i = (min*oldsf+n)/maxint;
	if (newsf < 0 ) newsf = -newsf;
	if (i < 0 ) i = -i;
	if (i > newsf) newsf = i;
	newsf++;
	*/
	if(xptr[n] == 0) {
		beep();
		printf("Function %d is not currently defined.\n",n);
		return -1;
	}
     i = 0;
     while ( i < npts) {
        	x = *(datpt+i+doffset); 
            *(datpt+i+doffset) = tabfun(n,x);
            i++;
    }
	have_max = 0;
    maxx();
    return 0;			
}


/* ********** */
   
   
   
/* ************************************************* */
int
cosmo(int n,int index)		/* Contour- smoothing: command is "cosmo dx dy cnt name"
					where cnt is the nominal 1/2 width of contour interval (in counts)
					and name is the name of file to be smoothed. Read in Rayleigh
					file into buffer first. */
					
{
	extern char cmnd[];
	extern unsigned int fd; 
    extern DATAWORD *datpt;
    extern char    *fullname();

	DATAWORD *datp,*datp2;
	int dx,dy,i,j,k,size,nt,nc,count,dxs,dys,cnt,w,area,ray,ray0,raym,rayp,dray,wmin,wmax,option;
	float r,sum,*condram=0;
	int jhi = 0,jlo = 0;
	DATAWORD *raypoint,*rampoint=0,*pray=0;
	unsigned int  raysize;
	TWOBYTE	rayxsize,rayysize;	
    dx=0;dy=0;cnt=0;
	option = 1;
	printf("%d pointer \n",datpt);
	
	if(option == 0) pprintf("\n no fancy interval adjustment\n");
	if(option == 1) pprintf("\n use ray data to adjust contour limits \n");
		
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;  	/* make a bit bigger for file reads */

	datp2 = datp = malloc(size);
	if(datp == 0) {
		nomemory();
		return -1;
	}
	
	/* copy current data into the rayleigh array */
	
	raysize = header[NCHAN]*header[NTRAK];
	rayxsize = header[NCHAN];
	rayysize = header[NTRAK];
		
	raypoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(raypoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++)
		*(raypoint + i) = *(datpt+i+doffset);
							
	/* Get the Raman image */	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d %d %s",&dx,&dy,&cnt,txt);
			break;
		}
	}

	area = dx * dy; // area around the point to be averaged over


	dxs = -dx/2;
	dys = -dy/2;
	if( dx & 0x1)
		dx = dx/2+1;
	else
		dx /= 2;
	if( dy & 0x1)
		dy = dy/2+1;
	else
		dy /= 2;
		

	fullname(txt,GET_DATA);
	printf("Raman file used: %s\n",txt);
	getfile (0,-1);			/* get the file */
	
	if(fd == -1){
		goto cleanup;
	} 

	
	if( (rayxsize != header[NCHAN]) || (rayysize != header[NTRAK]) ) {
			beep();
			printf("Image sizes are different. \n");
			goto cleanup;											  
	}
	
	
	/* copy current data into the Raman array */
	rampoint = (DATAWORD*) malloc(raysize*DATABYTES);
	if(rampoint == 0) {
		nomemory();
		goto cleanup;
	}


	pray   =  (DATAWORD*) malloc(50*cnt*DATABYTES); 
	condram = (float*) malloc(50*cnt*sizeof(float));

	for(i=0; i<raysize; i++) {
	  *(rampoint + i) = *(datpt+i+doffset);
	}

	
	
	for(nc=0; nc<doffset; nc++) 
		*(datp++) = *(datpt+nc);	/* copy the CCD header */

	for(nt=0; nt<rayysize; nt++) {

		
		
		for(nc=0; nc<rayxsize;nc++){
		
			sum = 0.0;  
			count = 0;

			ray0 = raypoint[ nt * rayxsize + nc ];
			w = cnt / sqrt(2000./ray0 );
			wmin = 0;	 wmax = 2 * w;	 raym = ray0 - w;	 rayp = ray0 + w;			
			
			
			for(j=0; j<2*w+1; j++) { 
				pray[j] = 0; 
				condram[j] = 0.0; 
			}


			for(i=dxs; i<dx; i++) { /* get pdf of data in contour interval */
				for(j=dys; j<dy; j++) {
					k = (nt + j)*rayxsize + nc + i;
					ray = raypoint[k];
					if( ray >= raym && ray <= rayp){ 
						if( (nt+j) < rayysize && (nc+i) < rayxsize){
							if((nt+j) >= 0 && (nc+i) >= 0){   
							 	dray = ray - raym;
							 	if(dray < 50 * cnt){
							 		pray[dray] += 1 ;
								 	condram[dray] += rampoint[k];
								} else {
									beep();
								} 
							}
						}
					}
				}
			}

/* arrange contour interval so that rayleigh average over interval is close to rayl. pt in center */
			if(option == 1){ 
				j = 2 * w + 1;
				for (i = 0; i < j; i++)	{ 
					count += pray[i] ;
					sum += pray[i] * (raym + i);
				}

				r =0.999 * (float)ray0;
				if((sum / (float)count) < r ) {
					for (i = 0; i < w; i++)	{
						sum -=  pray[i] * (raym + i);
			 			count -= pray[i];
						if( sum  >= r * (float)count){
							wmin = i; 
							 goto done;				   
						} 
					}	
				}
		
				r = 1.001 * (float)ray0;
				if((sum / (float)count) > r) {
					for (i = 2*w; i >w; i--) {
						sum -=  pray[i] * (raym + i); 
						count -=  pray[i];
						if( sum  <= r * (float)count ){
							wmax = i;
							goto done;					
						}
					}	
				}
			}   /* end of option 1 */
	

done:						
			sum = 0.;  
			count = 0;
			if(( w - wmin)< (0.2 * w)) {wmin = w - 0.2 * w;  jlo++;} 
			if(( wmax - w)< (0.2 * w)) {wmax = w + 0.2 * w;  jhi++;}   

			for (i = wmin; i < wmax+1; i++) {
				count += pray[i] ;
				sum += condram[i];
			}
			   						   
			*(datp++) =  sum/(float)count + 0.5;

		}
	}
	free(datpt);
	printf("%d pointer \n",datpt);
	datpt = datp2;

cleanup:
	if(condram!=0) {free(condram);	condram = 0;} 
	if(pray!=0) {free(pray);	pray = 0;}
	if(raypoint!=0) {free(raypoint);	raypoint = 0;}
	if(rampoint!=0) {free(rampoint);	rampoint = 0;}

	
	have_max = 0;
	setarrow();	
	return 0;

}  /* end fn */


int
cosreg(int n,int index)		/* Contour-smoothing region: command is "cosreg dx dy cnt name"
					where cnt is the nominal 1/2 width of contour interval (in counts)
					and name is the name of file to be smoothed. Read in Rayleigh
					file into buffer first. */
{
	extern unsigned int fd; 

	DATAWORD *datp,*datp2;
	int dx,dy,i,j,k,size,nt,nc,count,dxs,dys,cnt,w,area,ray,ray0,raym,rayp,dray,wmin,wmax,option;
	float r,sum,*condram=0;
	int jhi = 0,jlo = 0;
	DATAWORD *raypoint,*rampoint=0,*pray=0;
	unsigned int  raysize;
	TWOBYTE	rayxsize,rayysize;	

    
    dx=0; dy=0; cnt=0;
	option = 1;
	
	if(option == 0) pprintf("\n no fancy interval adjustment\n");
	if(option == 1) pprintf("\n use ray data to adjust contour limits \n");
		
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;  	/* make a bit bigger for file reads */

	datp2 = datp = malloc(size);
	if(datp == 0) {
		nomemory();
		return -1;
	}
	
	/* get ready to copy current data into the rayleigh array */
	
	raysize = header[NCHAN]*header[NTRAK];
	rayxsize = header[NCHAN];
	rayysize = header[NTRAK];
		
	raypoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(raypoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++) // move data into rayleigh array here
		*(raypoint + i) = *(datpt+i+doffset);
							
	/* Get the arguments */	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d %d %s",&dx,&dy,&cnt,txt);
			break;
		}
	}

	// Setup arguments related to size of region
	area = dx * dy;
	dxs = -dx/2;
	dys = -dy/2;
	if( dx & 0x1)
		dx = dx/2+1;
	else
		dx /= 2;
	if( dy & 0x1)
		dy = dy/2+1;
	else
		dy /= 2;
		

	fullname(txt,GET_DATA);
	printf("Raman file used: %s\n",txt);
	getfile (0,-1);			/* get the Raman file */
	
	if(fd == -1){
		goto cleanup;
	} 

	
	if( (rayxsize != header[NCHAN]) || (rayysize != header[NTRAK]) ) {
			beep();
			printf("Image sizes are different. \n");
			goto cleanup;											  
	}
	
	
	/* copy current data into the Raman array */
	rampoint = (DATAWORD*) malloc(raysize*DATABYTES);
	if(rampoint == 0) {
		nomemory();
		goto cleanup;
	}

	// arrays 
	pray   =  (DATAWORD*) malloc(10*cnt*DATABYTES); 
	condram = (float*) malloc(10*cnt*sizeof(float));

	for(i=0; i<raysize; i++) { 
	  *(rampoint + i) = *(datpt+i+doffset);
	} // now have the raman image copy

	
	
	for(nc=0; nc<doffset; nc++) 
		*(datp++) = *(datpt+nc);	/* copy the CCD header */

	for(nt=0; nt<rayysize; nt++) {	// start of the image smooth loop; pixel considered is nc,nt

		
		
		for(nc=0; nc<rayxsize;nc++){
		
			sum = 0.0;  
			count = 0;

			ray0 = raypoint[ nt * rayxsize + nc ]; // the rayleigh value at this pixel
			w = cnt / (sqrt) (2000./ray0 );
			wmin = 0;	 wmax = 2 * w;	 raym = ray0 - w;	 rayp = ray0 + w;			
			
			
			for(j=0; j<2*w+1; j++) { 
				pray[j] = 0; 
				condram[j] = 0.0; 
			}


			for(i=dxs; i<dx; i++) { // get pdf of data in contour interval 
				for(j=dys; j<dy; j++) {
					k = (nt + j)*rayxsize + nc + i;
					ray = raypoint[k]; // ray is the rayleigh value at this pixel 
					if( ray >= raym && ray <= rayp){ 
						if( (nt+j) < rayysize && (nc+i) < rayxsize){
							if((nt+j) >= 0 && (nc+i) >= 0){ 
								// if we get here, the rayleigh point is within the contour  
							 	dray = ray - raym;
							 	++ pray[dray] ;
								 condram[dray] += rampoint[k];
							}
						}
					}
				}
			}

/* arrange contour interval so that rayleigh average over interval is close to rayl. pt in center */
			if(option == 1){ 
				j = 2 * w + 1;
				for (i = 0; i < j; i++)	{ 
					count += pray[i] ;
					sum += pray[i] * (raym + i);
				}
				
				/* sum/count is the average rayleigh signal within the contour */
				
				r =0.999 * (float)ray0;
				if((sum / (float)count) < r ) {
					for (i = 0; i < w; i++)	{
						sum -=  pray[i] * (raym + i);
			 			count -= pray[i];
						if( sum  >= r * (float)count){
							wmin = i; 
							 goto done;				   
						} 
					}	
				}
		
				r = 1.001 * (float)ray0;
				if((sum / (float)count) > r) {
					for (i = 2*w; i >w; i--) {
						sum -=  pray[i] * (raym + i); 
						count -=  pray[i];
						if( sum  <= r * (float)count ){
							wmax = i;
							goto done;					
						}
					}	
				}
			}   /* end of option 1 */
	

done:						
			sum = 0.;  
			count = 0;
			if(( w - wmin)< (0.2 * w)) {wmin = w - 0.2 * w;  jlo++;} 
			if(( wmax - w)< (0.2 * w)) {wmax = w + 0.2 * w;  jhi++;}   

			for (i = wmin; i < wmax+1; i++) {
				count += pray[i] ;
				sum += condram[i];
			}
			   						   
			*(datp++) =  sum/(float)count + 0.5;

		}
	}
	free(datpt);
	datpt = datp2;

cleanup:
	if(condram!=0) {free(condram);	condram = 0;} 
	if(pray!=0) {free(pray);	pray = 0;}
	if(raypoint!=0) {free(raypoint);	raypoint = 0;}
	if(rampoint!=0) {free(rampoint);	rampoint = 0;}

	
	have_max = 0;
	setarrow();
	return 0;	

}  /* end fn */

/* ************************* */
// noise generating functions---------


int noise(int n,int index)		/*	NOISE mean rms seed
									uniform deviate generator with using built-in random-number
									generator */
{
    extern DATAWORD *datpt;
	extern int	doffset;
	unsigned int seed=0;
	float x;
    float mean=0; float rms=0;
	int i,nt,nc;
	DATAWORD *datp;
	
	/* Get the arguments */	
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%f %f %d",&mean,&rms,&seed);
			break;
		}
	}
	printf(" Mean: %.2f, rms: %.2f, seed %d\n",mean,rms,seed);

	srand(seed);
	rms *= 1.75;

	datp = datpt+doffset;
	for(nt=0; nt<header[NTRAK]; nt++) {
		
		for(nc=0; nc<header[NCHAN];nc++){
			x = rand();
			x = (x/RAND_MAX*2.0 -1.0)*rms; 
			*(datp++) = mean + x;
		}
	}
	trailer[SFACTR] = 1;
	have_max = 0;
	return 0;
}

// gaussian deviate generator.  source: "Numerical Recipes in C", W.H. Press
float ranfGauss()
{	
	static int iset = 0;
	static float gset;
	float fac, rsq, v1, v2, test;
	
	if (iset==0) {
		do {
			v1 = (float)rand()/(float)(RAND_MAX);
			v2 = (float)rand()/(float)(RAND_MAX);
			v1 = 2.0*v1-1.0;
			v2 = 2.0*v2-1.0;
			rsq = v1*v1+v2*v2;
		} while (rsq>=1.0 || rsq==0.0);
		
		fac = sqrt(-2.0*log(rsq)/rsq);
		gset = v1*fac;
		iset = 1;
		test = v2*fac;
		return v2*fac;
		
	} else {
		iset = 0;
		test = gset;
		return gset;
		}
	}

// gaussian noise

int gnoise(int n,int index)		/*	GNOISE mean rms seed
									gaussian deviate generator using built-in random-number
									generator followed by Box-Muller transform */
{
    extern DATAWORD *datpt;
	extern int	doffset;
	unsigned int seed=0;
	float x;
    float mean=0; float rms=0;
	int i,nt,nc;
	DATAWORD xdat;
	DATAWORD *datp;
	
	// Get the arguments
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%f %f %d",&mean,&rms,&seed);
			break;
		}
	}
	printf(" Mean: %.2f, rms: %.2f, seed %d\n",mean,rms,seed);
	
	srand(seed);

	datp = datpt+doffset;
	for(nt=0; nt<header[NTRAK]; nt++) {
		
		for(nc=0; nc<header[NCHAN];nc++){
			x = ranfGauss();
			xdat = (x*rms+mean);
			*(datp++) = xdat;
		}
	}
	trailer[SFACTR] = 1;
	have_max = 0;
	return 0;
}

// shot noise

int shotnoise(int n,int index)		/*	SHOTNOISE seed
                                 treat the data as counts and add random root N noise to the data */
{
    extern DATAWORD *datpt;
	extern int	doffset;
	unsigned int seed=0;
	float x;
	int i,nt,nc;
	DATAWORD *datp;
	
	// Get the arguments
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') {
			sscanf(&cmnd[index],"%d",&seed);
			break;
		}
	}
	printf(" Seed %d\n",seed);
	
	srand(seed);
    
	datp = datpt+doffset;
	for(nt=0; nt<header[NTRAK]; nt++) {
		
		for(nc=0; nc<header[NCHAN];nc++){
			x = idat(nt,nc);
            x += ((float)rand()/(float)RAND_MAX-.5)*sqrtf(x);
			*(datp++) = x;
		}
	}
	trailer[SFACTR] = 1;
	have_max = 0;
	return 0;
}

/*----------------------------------------------------------------*/

/* ************************* */

/* For reading in Mitch's calculated solutions and turning them into OMA format images 

	Solutions are in binary form
	Solutions have all data
	Species are represented as mass fractions
*/

/* Variables related to the computed solution */

#define NINT 4
#define MAX_NSP 60
#define MAX_NPTS 121*121

int have_solution = 0;				/* indicates a solution file has been read in */
float sol_x0,sol_x1,sol_y0,sol_y1;	/* the bounds of the solution read in */
int sol_nx,sol_ny,sol_nsp;			/* the number of x and y grid lines in solution and species */
int spnum = 4;						/* the species number to get */
int mole_flag = 0;					/* calculate mole fraction or leave as mass fraction?	*/
float multiplier=1.0;				/* scale up by this amount */
double *sol;						/* the solution array -- has one quantity in it, e.g., T or CH4 */
double *sol_x;						/* the array of x values of the solution grid */
double *sol_y;						/* the array of y values of the solution grid */
double smin,smax;	

int soot_species = 20;
/*
float weight[45] = { 
		  //	Ethylene Mechanism with soot -- 45 species
28.0542	, //	1	C2H4
31.9988	, //	2	O2
28.0134	, //	3	N2
18.0153	, //	4	H2O
44.01	, //	5	CO2
1.00797	, //	6	H
15.9994	, //	7	O
17.0074	, //	8	OH
33.0068	, //	9	HO2
2.01594	, //	10	H2
28.0106	, //	11	CO
34.0147	, //	12	H2O2
29.0185	, //	13	HCO
30.0265	, //	14	CH2O
15.0351	, //	15	CH3
14.0271	, //	16	CH2
30.0701	, //	17	C2H6
29.0622	, //	18	C2H5
16.043	, //	19	CH4
27.0462	, //	20	C2H3
26.0382	, //	21	C2H2
42.0376	, //	22	CH2CO
41.0297	, //	23	HCCO
25.0303	, //	24	C2H
78.1147	, //	25	C6H6
77.1068	, //	26	PHENYL
77.1068	, //	27	L-C6H5
66.1036	, //	28	C-C5H6
65.0956	, //	29	CYC5H5
40.0653	, //	30	C3H4
39.0574	, //	31	C3H3
38.0494	, //	32	C3H2
41.0733	, //	33	C3H5
42.0813	, //	34	C3H6
43.0892	, //	35	I-C3H7
44.0972	, //	36	C3H8
50.0605	, //	37	C4H2
54.0924	, //	38	C4H6
53.0845	, //	39	NC4H5
51.0685	, //	40	C4H3U
14.0271	, //	41	CH2(S)
42.0376	, //	42	HCCOH
44.0536	, //	43	CH3CHO
40.0653	, //	44	ALLENE
41.0733	  //	45	CH3CHCH 
};
#define SOOT_SPECIES 0
*/

/*
float weight[52] = {  
		  //	CH* OH* in GRI mechanism -- 52 species
16.043	, //	1	CH4
31.9988	, //	2	O2
18.0153	, //	3	H2O
44.01	, //	4	CO2
1.00797	, //	5	H
2.01594	, //	6	H2
28.0106	, //	7	CO
15.9994	, //	8	O
17.0074	, //	9	OH
33.0068	, //	10	HO2
34.0147	, //	11	H2O2
29.0185	, //	12	HCO
30.0265	, //	13	CH2O
15.0351	, //	14	CH3
31.0345	, //	15	CH3O
28.0134	, //	16	N2
14.0271	, //	17	CH2
13.0191	, //	18	CH
30.0701	, //	19	C2H6
29.0622	, //	20	C2H5
28.0542	, //	21	C2H4
27.0462	, //	22	C2H3
26.0382	, //	23	C2H2
42.0376	, //	24	CH2CO
41.0297	, //	25	HCCO
25.0303	, //	26	C2H
46.0055	, //	27	NO2
30.0061	, //	28	NO
30.0293	, //	29	N2H2
29.0214	, //	30	NNH
31.0141	, //	31	HNO
17.0306	, //	32	NH3
16.0226	, //	33	NH2
15.0147	, //	34	NH
44.0128	, //	35	N2O
14.0067	, //	36	N
27.0258	, //	37	HCN
42.0173	, //	38	NCO
43.0252	, //	39	HOCN
43.0252	, //	40	HNCO
43.0252	, //	41	HCNO
26.0179	, //	42	CN
52.0357	, //	43	C2N2
14.0271	, //	44	CH2(S)
12.0112	, //	45	C
31.0345	, //	46	CH2OH
32.0424	, //	47	CH3OH
42.0376	, //	48	HCCOH
28.0338	, //	49	H2CN
41.0325	, //	50	HCNN
13.0191	, //	51	CH*
17.0074	 //	52	OH*
};
#define SOOT_SPECIES 0
*/

/*
float weight[16] = {  
		  //	16 species mechanism 
16.043	, //	1	CH4
31.9988	, //	2	O2
18.0153	, //	3	H2O
44.01	, //	4	CO2
1.00797	, //	5	H
2.01594	, //	6	H2
28.0106	, //	7	CO
15.9994	, //	8	O
17.0074	, //	9	OH
33.0068	, //	10	HO2
34.0147	, //	11	H2O2
29.0185	, //	12	HCO
30.0265	, //	13	CH2O
15.0351	, //	14	CH3
31.0345	, //	15	CH3O
28.0134	  //	16	N2
};
#define SOOT_SPECIES 0
*/

/*
float weight[31] = {  
		  //	31 species mechanism 
16.043	, //	1	CH4
31.9988	, //	2	O2
18.0153	, //	3	H2O
44.01	, //	4	CO2
1.00797	, //	5	H
2.01594	, //	6	H2
28.0106	, //	7	CO
15.9994	, //	8	O
17.0074	, //	9	OH
33.0068	, //	10	HO2
34.0147	, //	11	H2O2
29.0185	, //	12	HCO
30.0265	, //	13	CH2O
15.0351	, //	14	CH3
31.0345	, //	15	CH3O
28.0134	, //	16	N2
14.0271	, //	17	CH2
13.0191	, //	18	CH
30.0701	, //	19	C2H6
29.0622	, //	20	C2H5
28.0542	, //	21	C2H4
27.0462	, //	22	C2H3
26.0382	, //	23	C2H2
42.0376	, //	24	CH2CO
41.0297	, //	25	HCCO
25.0303	, //	26	C2H
14.0271	, //	27	CH2(S)
12.0112	, //	28	C
31.0345	, //	29	CH2OH
32.0424	, //	30	CH3OH
42.0376	 //	31	HCCOH
};
#define SOOT_SPECIES 0
*/

float weight[84] = {
		  //	Ethylene Mechanism with soot and NO chemistry -- 66 + 18(NO) = 84 species
28.0542	, //	1	C2H4
31.9988	, //	2	O2
28.0134	, //	3	N2
18.0153	, //	4	H2O
44.01	, //	5	CO2
1.00797	, //	6	H
15.9994	, //	7	O
17.0074	, //	8	OH
33.0068	, //	9	HO2
2.01594	, //	10	H2
28.0106	, //	11	CO
34.0147	, //	12	H2O2
29.0185	, //	13	HCO
30.0265	, //	14	CH2O
15.0351	, //	15	CH3
14.0271	, //	16	CH2
30.0701	, //	17	C2H6
29.0622	, //	18	C2H5
16.043	, //	19	CH4
27.0462	, //	20	C2H3
26.0382	, //	21	C2H2
42.0376	, //	22	CH2CO
41.0297	, //	23	HCCO
25.0303	, //	24	C2H
78.1147	, //	25	C6H6 --A1
77.1068	, //	26	PHENYL --A2
77.1068	, //	27	L-C6H5
66.1036	, //	28	C-C5H6
65.0956	, //	29	CYC5H5
40.0653	, //	30	C3H4
39.0574	, //	31	C3H3
38.0494	, //	32	C3H2
41.0733	, //	33	C3H5
42.0813	, //	34	C3H6
43.0892	, //	35	I-C3H7
44.0972	, //	36	C3H8
50.0605	, //	37	C4H2
54.0924	, //	38	C4H6
53.0845	, //	39	NC4H5
51.0685	, //	40	C4H3U
14.0271	, //	41	CH2(S)
42.0376	, //	42	HCCOH
44.0536	, //	43	CH3CHO
40.0653	, //	44	ALLENE
41.0733	, //	45	CH3CHCH
31.0400	, //	46	CH3O
13.0200	, //	47	CH
43.1000	, //	48	C3H7(N)
56.1200	, //	49	C4H81
74.0800	, //	50	C6H2
52.0800	, //	51	C4H4
43.0500	, //	52	CH2CHO
43.0500	, //	53	CH2OH
32.0500	, //	54	CH3OH
41.0800	, //	55	CH3CCH2
66.0600	, //	56	H2C4O
51.0700	, //	57	C4H3(L)
53.0900	, //	58	C4H5(L)
54.1000	, //	59	C4H612 ???
75.0900	, //	60	C6H3
76.1000	, //	61	C6H4(C)
93.1100	, //	62	C6H5O
94.1200	, //	63	C6H5OH
80.0900	, //	64	C5H4O
81.1000	, //	65	C5H4OH
39.9800	, //	66	AR
12.0100	, //	67	C
14.0100	, //	68	N
15.0200	, //	69	NH
16.0300	, //	70	NH2
17.0400	, //	71	NH3
29.0300	, //	72	NNH
30.0100	, //	73	NO
46.0100	, //	74	NO2
44.0200	, //	75	N2O
31.0200	, //	76	HNO
26.0200	, //	77	CN
27.0300	, //	78	HCN
28.0400	, //	79	H2CN
41.0400	, //	80	HCNN
43.0300	, //	81	HCNO
43.0300	, //	82	HOCN
43.0300	, //	83	HNCO
42.0200	  //	84	NCO
};
//#define SOOT_SPECIES 20


int
solget(int n, int index)
{
	unsigned int fd;
	int num[4],i,j,k,ind;
	double ** fsol;		/* the full solution */
	double ** dmatrix(long, long , long , long );
	void free_dmatrix(double **, long , long , long , long);
	double sum;
	void swap_bytes_routine(char* co, int num, int nb);
	extern int is_big_endian;
	
	if((fd = open(fullname(&cmnd[index],GET_DATA),READMODE)) == -1) {
		printf("File Not Found: %s\n",&cmnd[index]);
		beep();
		return -1;
	}
	read(fd,(char*)num,NINT*4);	/* read in NINT integers */
	
	if(!is_big_endian) swap_bytes_routine((char*) num, NINT*4, 4);
	
	sol_nsp = num[1];
	sol_nx = num[2];
	sol_ny = num[3];
        //sol_nsp = spnum;    // only need to go as far as the one we are after
        
	
	printf("Solution for %d variables is on %d x %d grid.\n",sol_nsp,sol_nx,sol_ny);
	
	sol_x = (double *)malloc((sol_nx+1)*8);
	sol_y = (double *)malloc((sol_ny+1)*8);
	
	read(fd,(char*)sol_x,(sol_nx+1)*8);	/* read the x array */
	
	if(!is_big_endian) swap_bytes_routine((char*) sol_x, (sol_nx+1)*8, 8);
	
	read(fd,(char*)sol_y,(sol_ny+1)*8);	/* read the y array */
	
	if(!is_big_endian) swap_bytes_routine((char*) sol_y, (sol_ny+1)*8, 8);
	
	sol_x0=sol_x[1];
	sol_x1=sol_x[sol_nx];
	sol_y0=sol_y[1];
	sol_y1=sol_y[sol_ny];
        /*
        for(i=1; i< sol_nx+1; i++){
            if(sol_x0 > sol_x[i])
                sol_x0=sol_x[i];
            if(sol_x1 < sol_x[i])
                sol_x1=sol_x[i];
        }
        for(i=1; i< sol_ny+1; i++){
            if(sol_y0 > sol_y[i])
                sol_y0=sol_y[i];
            if(sol_y1 < sol_y[i])
                sol_y1=sol_y[i];
        }
	*/
	printf("X range is from %f to %f\n",sol_x0,sol_x1);
	printf("Y range is from %f to %f\n",sol_y0,sol_y1);
	
	if(sol!=0) {free(sol); sol = 0;}		
	sol = (double *)malloc((sol_nx+1)*8 * (sol_ny+1)*8);
	if(sol == 0) {
		nomemory();
		return -1;
	}
	
	fsol = dmatrix(0,sol_nsp+1,0,(sol_nx+1)*(sol_ny+1));
	
	if(fsol==0) return 0;
	
	for(j=1; j<= sol_ny; j++) {
		ind = (sol_nx+1)*(j-1);
		for(k=1; k<= sol_nsp; k++) {
			read(fd,(char*)&fsol[k][ind],(sol_nx+1)*8);	
			if(!is_big_endian) swap_bytes_routine((char*) &fsol[k][ind],(sol_nx+1)*8, 8);
		}
	}
	close(fd);
	
	if(mole_flag != 0) {
		printf("Calculating mole fraction.\n");
		sol_nsp -= soot_species;
		for(j=1; j<=sol_ny; j++) {
			ind = (sol_nx+1)*(j-1);
			for(i=1; i<=sol_nx; i++) {
				sum=0.0;
				for(k=1; k<=sol_nsp-4; k++) {
					sum += fsol[k+4][ind+i]/weight[k-1];
				}
				for(k=1; k<=sol_nsp-4; k++) {
					fsol[k+4][ind+i] = fsol[k+4][ind+i]/weight[k-1]/sum;
				}
			}
		}
	}
	
	smin = 1e10;
	smax = -1e10;
	i=0;
	for(j=1; j<= sol_ny; j++) {
		ind = (sol_nx+1)*(j-1);
		for(k=1; k<= sol_nx; k++) {
			*(sol+i) = fsol[spnum][k+ind];	
			if(*(sol+i) < smin) smin = *(sol+i);
			if(*(sol+i) > smax) smax = *(sol+i);			
			i++;
		}
	}
	printf("Min %e\nMax %e\n",smin,smax);
	
	free_dmatrix(fsol,0,sol_nsp+1,0,(sol_nx+1)*(sol_ny+1));
	have_solution = 1;
	return 0;
}
/* ************************* */
int
solspecies(int n, int index)		// read species mass fractions in from a formatted text file
									// SOLSPECIES filename
// file format:
// species
// soot species
// molecular weight of species 1
// molecular weight of species 2
// etc.
{
	FILE *fp, *fopen();
	int i,j;
	int num_species;
		
	if ((fp = fopen(fullname(&cmnd[index],GET_DATA),"r")) == NULL) {
		beep();
		printf(" File Not Found.\n");
		return -1;
	}
	fscanf(fp,"%d",&num_species);
	fscanf(fp,"%d",&soot_species);
	printf("Reading masses for %d species.\nSoot_species set to %d\n",num_species,soot_species);
	for(i=0; i<num_species; i++){
		weight[i]=0;	// just to be sure that we don't get fooled by left-overs in the default definition
		j = fscanf(fp,"%f",&weight[i]);
		if(j != 1){
			beep();
			printf("Error Reading species %d.\n",i);
			return -1;
		}else {
			printf("Species %d\tWeight:\t%f\n",i+1,weight[i]);
		}
	}
	fclose(fp);
	return 0;
}

/* ************************* */
int
solgf(int n, int index)				/* read in a string of formatted numbers for a single variable */
									/* assumes solget has been read in first */
									/* use solmak to get the image */
									/* SOLGF filename */
{
	FILE *fp, *fopen();
	float x;
	int i;
	
	if(!have_solution){
		beep();
		printf("Must read solution first first\n"); 
		return -1;
	}
	
	if ((fp = fopen(fullname(&cmnd[index],GET_DATA),"r")) == NULL) {
		beep();
		printf(" File Not Found.\n");
		return -1;
	}
	
	
	
	smin = 1e10;
	smax = -1e10;
	
	for(i=0; i<sol_ny*sol_nx; i++) {
		fscanf(fp,"%f",&x);
		*(sol+i) = x;
		if(*(sol+i) < smin) smin = *(sol+i);
		if(*(sol+i) > smax) smax = *(sol+i);			

	}
	fclose(fp);
	printf("Min %e\nMax %e\n",smin,smax);
	return 0;
	
}
	
/* ************************* */
/*
SOLGB filename
	read in binary numbers for a single variable 
	assumes solget has been read in first 
	use solmak to get the image 
*/

int
solgb(int n, int index)				
{
	unsigned int fd;
	
	int i,j,k;
	float x[3];
	
	if(!have_solution){
		beep();
		printf("Must read solution first first\n"); 
		return -1;
	}
	
	if((fd = open(fullname(&cmnd[index],GET_DATA),READMODE)) == -1) {
		printf("File Not Found: %s\n",&cmnd[index]);
		beep();
		return -1;
	}
	//read(fd,(char*) sol,(sol_nx*sol_ny)*8);	
	i=0;
	for(j=1; j<= sol_ny; j++) {
		//ind = (sol_nx+1)*(j-1);
		for(k=0; k< sol_nx; k++) {
			read(fd,(char*)x,sizeof(float)*3);
			*(sol+i) = x[2];
			i++;
		}
	}
	close(fd);
	
	smin = 1e10;
	smax = -1e10;
	
	for(i=0; i<sol_ny*sol_nx; i++) {
		if(*(sol+i) < smin) smin = *(sol+i);
		if(*(sol+i) > smax) smax = *(sol+i);			

	}
	
	printf("Min %e\nMax %e\n",smin,smax);
	return 0;
	
}
/* ************************* */

/* write the solution for the variable specified using SOLMUL as formatted text files 
		SOLSAVEF filename
		The files created will be
		filename_X			a 1-D array of X values of the grid
		filename_Y			a 1-D array of Y values of the grid
		filename			a 2-D array of the values, size is (#x by #y)
		.
		.
		Usage would be something like this:
		SOLMUL 4 1 0
		SOLGET binfile
		SOLSAVEF filename
*/
		
int
solsavef(int n, int index)
{
	int nt,nc;
	
	char name[CHPERLN];

	
	FILE *fp;
    extern char     cmnd[],*fullname();
	
	n = 0;
	
	strcpy(name,&cmnd[index]);
	fp = fopen(fullname(name,SAVE_DATA),"w");
	
	if( fp != NULL) {
		for(nt=0; nt<sol_ny; nt++){
			for(nc=0; nc<sol_nx; nc++){
				fprintf(fp,"%g\t",*(sol+(nc)+(nt)*sol_nx));
			}
			fprintf(fp,"\n");
		}
    	fclose(fp);
	
		settext(name);
		fileflush(name);	// for updating directory 
	} else {
		beep();
		return -1;
	}
	
	strcpy(name,&cmnd[index]);
	strcat(name,"_X");
	fp = fopen(fullname(name,SAVE_DATA),"w");
	
	if( fp != NULL) {
		for(nt=1; nt<=sol_nx; nt++){
			fprintf(fp,"%g\n",sol_x[nt]);
		}
    	fclose(fp);
		settext(name);
		fileflush(name);	// for updating directory 
	} else {
		beep();
		return -1;
	}
	
	strcpy(name,&cmnd[index]);
	strcat(name,"_Y");
	fp = fopen(fullname(name,SAVE_DATA),"w");
	
	if( fp != NULL) {
		for(nt=1; nt<=sol_ny; nt++){
			fprintf(fp,"%g\n",sol_y[nt]);
		}
    	fclose(fp);
		settext(name);
		fileflush(name);	// for updating directory 
	} else {
		beep();
		return -1;
	}
	return 0;
	
}
	
/* ************************* */

/* make an image from the solution read in 
		SOLMAK x0 y0 x1 y1
		x0 y0 x1 y1 are floating point numbers that specify the sub rectangle of the solution
		the current ruler determines the resolution
*/
		
int
solmak(int n, int index)
{
	int narg,nt,nc,i,j;
	float x0,x1,y0,y1;
	double x,y;
	DATAWORD *datp2;
	DATAWORD interpolate_sol();
	
	extern Variable user_variables[];	

	extern int ruler_scale_defined;
	extern float ruler_scale;
	
	if(!ruler_scale_defined){
		beep();
		printf("Must define ruler first\n"); 
		return -1;
	}
	
	if(!have_solution){
		beep();
		printf("Must read solution first first\n"); 
		return -1;
	}
	
	
	narg = sscanf(&cmnd[index],"%f %f %f %f",&x0,&y0,&x1,&y1);

	if(narg != 4) {
		beep();
		printf("Need 4 Arguments.\n"); 
		return -1;
	}
	
	if( x0<sol_x0 || x1>sol_x1 || y0<sol_y0 || y1>sol_y1) {
		printf("Region does not match solution.\n");
		beep();
		return -1;
	}
	
	nt = header[NTRAK];
	nc = header[NCHAN];
	
	header[NCHAN] = (x1-x0)*ruler_scale+1;	/* the number of x pixels in new image */
	header[NTRAK] = (y1-y0)*ruler_scale+1;	/* the number of y pixels in new image */
	npts = header[NTRAK]*header[NCHAN];

	if ( checkpar() == 1) {
			beep();
			printf("Not enough memory for %d x %d image.\n", header[NCHAN],header[NTRAK]);
            header[NCHAN] = nc;
	        header[NTRAK] = nt;
			npts = nc*nt;
	}
	printf("%d x %d\n",header[NCHAN],header[NTRAK]);
	#ifdef FLOAT
	#else
	while(smax*multiplier < 1000.0) {
		multiplier *= 10.0;
	}
	#endif
	pprintf("Multiplier of variable\t%d\t%.1e\n",spnum,multiplier);
	
	datp2 = datpt+ doffset;
	y = y0;
	for(nt=0; nt<header[NTRAK]; nt++) {
		x = x0;
		for(j=1; sol_y[j]<y; j++){};/* j is index at y location in solution */
		for (nc=0; nc<header[NCHAN]; nc++) {
			for(i=1; sol_x[i]<x; i++){};			/* i is index at x location in solution */
			*(datp2++) = interpolate_sol(x,y,i,j);
			x += 1.0/ruler_scale;
		}
		y += 1.0/ruler_scale;
		
	}
		
	have_max = 0;
	trailer[SFACTR] = 1;
	maxx();
	
	// return values available as variables
	user_variables[0].fvalue = multiplier;
	user_variables[0].is_float = 1;
	
	setarrow();
	return 0;
}

DATAWORD interpolate_sol(double x,double y,int i,int j)
{
	DATAWORD value;
	double z,xf,yf;
	double z1,dx,dy,z2,z3,z4;
	//double hy,hx;
	
	i--;
	if(i == 0) i=1;
	j--;
	if(j == 0) j=1;
	
	dx = sol_x[i+1] - sol_x[i];
	dy = sol_y[j+1] - sol_y[j];
	
	xf = x - sol_x[i];	// the fraction part
	yf = y - sol_y[j];
	
	xf /= dx;
	yf /= dy;
	
	//		z3-----------z4
	//		|             |
	//		|             |
	//		|             |
	//		|             |
	//		|             |
	//      z1-----------z2
	
	
	
	z1 = *(sol+(i)+(j)*sol_nx);
	z2 = *(sol+(i+1)+(j)*sol_nx);
	z3 = *(sol+(i)+(j+1)*sol_nx);
	z4 = *(sol+(i+1)+(j+1)*sol_nx);
	/*
	if(dx > dy) {
		if( yf <= xf) {
			// lower right triangle 
			z = xf*(z2-z1) + yf*(z4-z2) + z1;
		}
		else {
			// upper left triangle 
			z = xf*(z4-z3) + yf*(z3-z1) + z1;
		}
	} else {
		if( yf <= (1-xf) ) {
			// lower left triangle 
			z = xf*(z2-z1) + yf*(z3-z1) + z1;
		}
		else {
			// upper right triangle 
			z = (1-xf)*(z3-z4) + (1-yf)*(z2-z4) + z4;
		}
	}
	*/
	// Bilinear Interpolation
	double a00,a10,a01,a11;
	a00 = z1;
	a10 = z2 - z1;
	a01 = z3 - z1;
	a11 = z1 - z2 - z3 + z4;
	z = a00 + a10*xf + a01*yf + a11*xf*yf;
	
	
	
	/*
	hy = *(sol+(i)+(j+1)*sol_nx);
	hx = *(sol+(i+1)+(j)*sol_nx);
	z = xf*(hx-z1) + yf*(hy-z1) + z1;
	*/
	value = z*multiplier;
	
	return(value);
}

/* ************************* */

/* specify the species_number, multiplier, and mole_fraction_flag to be used in SOLMAK 
		SOLMUL snum mult moleflag
*/
	
int
solmul(int n, int index)
{
	int narg;

	narg = sscanf(&cmnd[index],"%d %f %d",&spnum,&multiplier,&mole_flag);

	if(narg != 3) {
		beep();
		printf("Need 3 Arguments.\n"); 
		return -1;
	}
	printf("Species %d\nMultiply by %f\nMole_flag: %d\n",spnum,multiplier,mole_flag);
	return 0;
}

#define NR_END 1
#define FREE_ARG char*

double **dmatrix(long nrl, long nrh, long ncl, long nch)
// allocate a matrix with subscript range m[ nrl-nrh] [ ncl-nch ]
{

	long i, nrow=nrh-nrl+1, ncol=nch-ncl+1;
	double **m;
	size_t arsiz;
	
	// allocate pointers to rows
	
	 m = (double **) malloc((size_t)((nrow+NR_END)*sizeof(double*)));
	 
	 if(!m) {
	 	beep();
	 	printf("allocation failure 1 in matrix()\n");
	 	m=0;
	 	return m;
	 }
	 
	 m += NR_END;
	 m -= nrl;
	 
	 // allocate rows and set pointers to them
	 arsiz = (size_t)((nrow*ncol+NR_END)*sizeof(double));
	 printf("%d size\n",arsiz);
	 
	 m[nrl] = (double *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(double)));
	 if( !m[nrl] ) {
	 	beep();
	 	printf("allocation failure 2 in dmatrix()\n");
	 	free((FREE_ARG) (m+nrl-NR_END));
	 	m = 0; 
	 	return m;
	 }
	 m[nrl] += NR_END;
	 m[nrl] -= ncl;
	 
	 for( i=nrl+1; i<=nrh; i++) 
	 	m[i] = m[i-1] + ncol;
	 	
	 // return pointer to array of pointers to rows
	 
	 return m;
}

void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch)
{
	free((FREE_ARG) (m[nrl]+ncl-NR_END));
	free((FREE_ARG) (m+nrl-NR_END));
}

/* ************************* */
int
fuel(int n,int index)		/* fuel: command is "fuel ctr1 ctr2 name"
					where ctr1 is the fuel contour at which the rayleigh image will be cut to
					form a new fuel image. ctr 2 is a different contour value to determine
					the scaling and offset for this shot. name is the name of the raman image.
					read the rayleigh image file into the buffer first. */
					
{
	extern unsigned int fd; 

	DATAWORD *datp,*datp2;
	int i,j,nc1,nc2,cntr1,cntr2,ramval;

	DATAWORD *raypoint,*rampoint;
	unsigned int  raysize,size;
	TWOBYTE	rayxsize,rayysize;
	
	float c1ave,c2ave,b,a;	
	
	rampoint = raypoint = 0;
		
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;  	/* make a bit bigger for file reads */

	datp2 = datp = malloc(size);
	if(datp == 0) {
		nomemory();
		return -1;
	}
	
	/* copy current data into the rayleigh array */
	
	raysize = header[NCHAN]*header[NTRAK];
	rayxsize = header[NCHAN];
	rayysize = header[NTRAK];
		
	raypoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(raypoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++)
		*(raypoint + i) = *(datpt+i+doffset);
							
	/* Get the Raman image */	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d %s",&cntr1,&cntr2,txt);
			break;
		}
	}

	fullname(txt,GET_DATA);
	printf("Raman file used: %s\n",txt);
	getfile (0,-1);			/* get the file */
	
	if(fd == -1){
		goto cleanup;
	} 

	/* size the same? */
	
	if( (rayxsize != header[NCHAN]) || (rayysize != header[NTRAK]) ) {
			beep();
			printf("Image sizes are different. \n");
			goto cleanup;											  
	}
	
	/* copy current data into the Raman array */
	rampoint = (DATAWORD*) malloc(raysize*DATABYTES);
	if(rampoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++) {
	  *(rampoint + i) = *(datpt+i+doffset);
	}

	
	
	for(j=0; j<doffset; j++) 
		*(datp++) = *(datpt+j);	/* copy the CCD header */


	// first, find the average rayleigh values along the raman contours specified 
	
	c1ave = c2ave = 0.0;
	nc1 = nc2 = 0;
	
	for(i=0; i<raysize; i++) {
		ramval = *(rampoint + i);
	  	if ( ramval  == cntr1){
	  		c1ave += *(raypoint+i);
	  		nc1++;
	  	}
	  	if ( ramval  == cntr2){
	  		c2ave += *(raypoint+i);
	  		nc2++;
	  	}
	  
	}
	if( nc1 == 0 || nc2 ==0) {
		beep();
		printf("Each contour must have at least one datapoint.\n");
		goto cleanup;
	}
	
	c1ave = c1ave/nc1;
	c2ave = c2ave/nc2;
	
	b = (float)(cntr2 - cntr1)/(c2ave - c1ave);
	a = c1ave - (float)cntr1/b;
	
	pprintf("a: %.2f\tb:%.4f\n",a,b);

	for(i=0; i<raysize; i++) {
		ramval = *(rampoint + i);
	  	if ( ramval  > cntr1){
	  		c1ave = (*(raypoint + i) - a) * b;
	  		*(datp++) = c1ave;
	  	} else {
	  		*(datp++) = *(rampoint + i);
	  	}
	  
	}


	free(datpt);
	datpt = datp2;

cleanup:
	if(raypoint!=0) {free(raypoint);	raypoint = 0;}
	if(rampoint!=0) {free(rampoint);	rampoint = 0;}
	have_max = 0;
	setarrow();	
	return 0;
}  /* end fn */


/* ************************* */

/*
ABELCLEAN X
	Try to clean up Abel inverted images.
	Calculate average horizontal gradient magnitude
	If a single line is more than X times greater than the average, replace it with the 
	average of the tracks above and below.
	
*/

int abelclean(int n, int index)
{
	float overfactor = 3.0;
	float ave = 0.,grad,imageave=0;
	float *line_grad;
	int nt,nc,i;
	DATAWORD *pt;
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%f",&overfactor);
			break;
		}
	}
	line_grad = (float*) malloc(header[NTRAK]*sizeof(float));
	
	for(nt=0; nt< header[NTRAK]; nt++){
		ave = 0;
		for(nc=1; nc<header[NCHAN]; nc++) {
			grad = idat(nt,nc) - idat(nt,nc-1);
			ave += grad*grad;
		}
		line_grad[nt] = ave/nc;
		imageave += ave/nc;
	}
	imageave /= nt;
	pt = datpt + doffset;
	
	for(nt=1; nt< header[NTRAK]-1; nt++){
		if(line_grad[nt] > imageave*overfactor){
			for(nc=0; nc<header[NCHAN]; nc++) {
				*(pt+nc +header[NCHAN]*nt) = (idat(nt-1,nc) + idat(nt+1,nc))/2;
			}
		}
	}
	
	free(line_grad);
	have_max = 0;
	maxx();
	return 0;

}
/* ************************* */

/*
GETANGLE
	Calculate the angle that the current image needs to be rotated to align the x center
	of mass with the y axis. The region inside the current rectangle is considered.
*/

int getangle(int n, int index)
{
	Point start,end;
	int x0,y0,sizx,sizy;
	int nt,nc;
	
	double xcom=0.,ave=0.,dmin;
	float sx,sy,sx2,sy2,sxy,slope,r,theta;
	
	DATAWORD datval;
	
	extern DATAWORD min;
	extern Variable user_variables[];
	
	start = substart;
	end = subend;
	
	if(subend.h < substart.h){
		end.h = substart.h;
		start.h = subend.h;
	}
	if(subend.v < substart.v){
		end.v = substart.v;
		start.v = subend.v;
	}

	x0 = start.h;
	y0 = start.v;
	sizx = end.h - start.h +1;
	sizy = end.v - start.v +1;
	
	sx = sy = sx2 = sy2 = sxy = 0.;
	
	dmin = min;
	
	for(nt=y0; nt< sizy+y0; nt++){
		ave = xcom = 0.;
		for(nc=x0; nc<sizx+x0; nc++) {
			datval = idat(nt,nc);		
			ave += datval;					// average 
			xcom += nc * (datval);			// x center of mass -- subtract min
			//rms += datval*datval;			// rms 
		
		}
		// if the entire row is the min value, don't include that point in the calculation
		// probably a good idea to do some clipping before using this routine
		// say something like clipfbot .05
		// set parts below some threshold to 0, so that unimportant parts of the 
		// image don't contribute to the calculation of the angle
		// should make it possible to have a larger rectangle for example
		ave = ave/(float)sizx;
		if(ave != dmin) {		
			xcom /= sizx;
			xcom /= (ave);
			
			sy += xcom;
			sx += nt;
			sxy += xcom*nt;
			sy2 += xcom * xcom;
			sx2 += nt * nt;
			printf("%d\t%g\n",nt,xcom);
		} else {
			sizy -= 1;
		}
	}

	slope = (sizy*sxy - sx*sy)/(sizy*sx2 - sx*sx);
	r = sxy/sqrt(sx2*sy2);
	printf("Slope: %g R: %g Pixels: %d\n",slope,r, sizy);
	theta = atan(slope);
	theta = -theta*180./PI;
	printf("Angle: %f\n",theta);
	
	user_variables[0].fvalue = theta;
	user_variables[0].is_float = 1;
	user_variables[1].fvalue = xcom;
	//user_variables[1].value = xcom;
	user_variables[1].is_float = 1;

	return 0;
}
/* ************************* */

/*
 GETANGLE2 fraction
 Calculate the angle that the current image needs to be rotated to align the x center
 of mass with the y axis. The entire image width is used. The start of the rectangle
 begins where maximum along a horizontal line is greater than "fraction" of the maximum
 and continues until the maximum along a horizontal line falls below "fraction" of the maximum
 */

int getangle2(int n, int index)
{

	int y1,y0,sizx,sizy;
	int nt,nc;
	
	double xcom=0.,ave=0.;
	float sx,sy,sx2,sy2,sxy,slope,r,theta;
	
	DATAWORD datval,max_fraction=0.,max_val = 0.;
	
	extern DATAWORD min,max;
	extern Variable user_variables[];


	nc = sscanf(&cmnd[index],"%f",&max_fraction);
	if (nc < 1){
		beep();
		printf("Command format is GETANGLE2 fraction\n");
		return -1;
	}
	
	max_val = max_fraction*max;
	printf("Data cutoff is at %g\n",max_val);
	printf("max is %g\n",max);
	printf("max_fract is %g\n",max_fraction);
		
	y1 = 0;
	y0 = 0;
	sizx = header[NCHAN];
	sizy = header[NTRAK];
	
	sx = sy = sx2 = sy2 = sxy = 0.;

	for(nt=0; nt< sizy; nt++){
		for(nc=0; nc<sizx; nc++) {
			if(idat(nt,nc) > max_val) break;
		}
		if(nc != sizx) break;
	}
	y0 = nt;

	for(nt=sizy-1; nt>=0; nt--){
		for(nc=0; nc<sizx; nc++) {
			if(idat(nt,nc) > max_val) break;
		}
		if(nc != sizx) break;
	}
	y1 = nt;
	
	printf("Using region between rows %d and %d\n",y0,y1);
	sizy = y1-y0+1;
	
	for(nt=y0; nt<= y1; nt++){
		ave = xcom = 0.;
		for(nc=0; nc<sizx; nc++) {
			datval = idat(nt,nc);		
			ave += datval;					// average 
			xcom += nc * (datval);			// x center of mass -- subtract min
			//rms += datval*datval;			// rms 
			
		}
		// if the entire row is the min value, don't include that point in the calculation
		// probably a good idea to do some clipping before using this routine
		// say something like clipfbot .05
		// set parts below some threshold to 0, so that unimportant parts of the 
		// image don't contribute to the calculation of the angle
		// should make it possible to have a larger rectangle for example
		ave = ave/(float)sizx;
		
		xcom /= sizx;
		xcom /= (ave);
		
		sy += xcom;
		sx += nt;
		sxy += xcom*nt;
		sy2 += xcom * xcom;
		sx2 += nt * nt;
		// printf("%d\t%g\n",nt,xcom);
	}
	
	slope = (sizy*sxy - sx*sy)/(sizy*sx2 - sx*sx);
	r = sxy/sqrt(sx2*sy2);
	printf("Slope: %g R: %g Pixels: %d\n",slope,r, sizy);
	theta = atan(slope);
	theta = -theta*180./PI;
	printf("Angle: %f\n",theta);
	
	user_variables[0].fvalue = theta;
	user_variables[0].is_float = 1;
	user_variables[1].fvalue = xcom;
	//user_variables[1].value = xcom;
	user_variables[1].is_float = 1;
	
	return 0;
}


/* ************************* */

/* 
FOLD fraction new_width
    Fold an image in half vertically. It is assumed to be symmetric about some nearly verticle axis.
    The center of symmetry is found separately for each row, except if there are dark regions at the top and bottom of the image.
    For the dark regions, the average center of the middle part of the image is used as the center of symmetry.
    Dark regions are rows (at the top and bottom) whose values are all less than fraction*image_max. x0,y0 and x1,y1 are coordinates of the top and bottom of the dark region, center is the average center. These are returned in command_return_1-5. 
*/

int fold(int n, int index)
{
    
    int y1,y0,sizx,sizy,new_width,center,x0,x1;
    int nt,nc;
    long int size;
    
    double xcom=0.,ave=0.,ave_xcom=0.;
    
    DATAWORD datval,max_fraction=0.,max_val = 0., *datp;
    
    extern DATAWORD min,max;
    extern Variable user_variables[];
    
    
    nc = sscanf(&cmnd[index],"%f %d",&max_fraction, &new_width);
    if (nc < 2){
        beep();
        printf("Command format is FOLD fraction new_width\n");
        return -1;
    }
    
    size = (new_width * header[NTRAK] + MAXDOFFSET) * DATABYTES;
    size = (size+511)/512*512;	/* make a bit bigger for file reads */
    
    datp = (DATAWORD*) malloc(size);
    if(datp == 0) {
        nomemory();
        return -1;
    }
    datp += doffset;
    
    max_val = max_fraction*max;
    printf("Data cutoff is at %g\n",max_val);
    printf("max is %g\n",max);
    printf("max_fract is %g\n",max_fraction);
    
    y1 = 0;
    y0 = 0;
    sizx = header[NCHAN];
    sizy = header[NTRAK];
    
    for(nt=0; nt< sizy; nt++){
        for(nc=0; nc<sizx; nc++) {
            if(idat(nt,nc) > max_val) break;
        }
        if(nc != sizx) break;
    }
    y0 = nt;
    x0 = nc;
    
    for(nt=sizy-1; nt>=0; nt--){
        for(nc=sizx-1; nc>=0; nc--) {
            if(idat(nt,nc) > max_val) break;
        }
        if(nc != -1) break;
    }
    y1 = nt;
    x1 = nc;
    
    printf("Using region between rows %d and %d\n",y0,y1);
    sizy = y1-y0+1;
    
    for(nt=y0; nt<= y1; nt++){
        ave = xcom = 0.;
        for(nc=0; nc<sizx; nc++) {
            datval = idat(nt,nc);		
            ave += datval;					// average 
            xcom += nc * (datval);			// x center of mass -- subtract min
            
        }
        ave = ave/(float)sizx;
        
        xcom /= sizx;
        xcom /= (ave);
        ave_xcom += xcom;
        center = xcom+.5;
        
        for(nc=0; nc<new_width; nc++) {
            *(datp + nt * new_width +nc)= (idat(nt,center+nc) + idat(nt,center-nc))/2.0;
        }
        
    }
    ave_xcom /= sizy;
    center = ave_xcom + .5;
    
    for(nt=0; nt< y0; nt++){
        for(nc=0; nc<new_width; nc++) {
            *(datp + nt * new_width +nc)= (idat(nt,center+nc) + idat(nt,center-nc))/2.0;
        }
    }
    for(nt=y1+1; nt< header[NTRAK]; nt++){
        for(nc=0; nc<new_width; nc++) {
            *(datp + nt * new_width +nc)= (idat(nt,center+nc) + idat(nt,center-nc))/2.0;
        }
    }
    header[NDX] = header[NDY] = 1;
    header[NX0] = header[NY0] = 0;
    header[NCHAN] = new_width;
    npts = header[NCHAN]*header[NTRAK];
    free(datpt);
    datpt = datp - doffset;
    have_max = 0;
    maxx();
    user_variables[0].fvalue = user_variables[0].ivalue = x0;
    user_variables[0].is_float=0;
    user_variables[1].fvalue = user_variables[1].ivalue = y0;
    user_variables[1].is_float=0;
    user_variables[2].fvalue = user_variables[2].ivalue = x1;
    user_variables[2].is_float=0;
    user_variables[3].fvalue = user_variables[3].ivalue = y1;
    user_variables[3].is_float=0;
    user_variables[4].fvalue = user_variables[4].ivalue = center;
    user_variables[4].is_float=0;
   
    
    return 0;
}


/* ************************* */


/*
ABELPREP clip_value [fill_value]
 Finds the maximum pixel value along horizontal lines in the image. If that 
 maximum is < clip_value, the entire horizontal line is set to fill_value.
 Default for fill_value is 0.
*/


int abelprep(int n, int index){
	float fill_value = 0.0, clip_value = 0.;
	float hmax = 0.;
	int nt,nc=0,i;
	
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			nc = sscanf(&cmnd[index],"%f %f",&clip_value,&fill_value);
			break;
		}
	}
	if (nc < 1){
		beep();
		printf("Command format is ABELPREP clip_value [fill_value]\n");
		return -1;
	} else if (nc == 1) {
		fill_value = 0.0;
	}
	
	for(nt=0; nt < header[NTRAK]; nt++){
		hmax = -1e10 ;
		for(nc=0; nc<header[NCHAN]; nc++) {
			if( idat(nt,nc) > hmax)
				hmax = idat(nt,nc);
		}
		if ( hmax < clip_value){
			for(nc=0; nc<header[NCHAN]; nc++) 
				*(datpt+doffset+nc+nt*header[NCHAN]) = fill_value;
		}
	}
	have_max = 0;
	maxx();
	return 0;
			
				
}

 

/*
 A different version -- sets values lower than  clip_value to fill_value
 The discontinuities make for some funny looking things

int abelprep(int n, int index){
    float fill_value = 0.0, clip_value = 0.;
   
    int nt,nc=0,i;
    
    for ( i = index; cmnd[i] != EOL; i++) {
	if(cmnd[i] == ' ') { 
	    nc = sscanf(&cmnd[index],"%f %f",&clip_value,&fill_value);
	    break;
	}
    }
    if (nc < 1){
	beep();
	printf("Command format is ABELPREP clip_value [fill_value]\n");
	return -1;
    } else if (nc == 1) {
	fill_value = 0.0;
    }
    
    for(nt=0; nt < header[NTRAK]; nt++){
	for(nc=0; nc<header[NCHAN]; nc++) {
	    if( idat(nt,nc) <=  clip_value)
		*(datpt+doffset+nc+nt*header[NCHAN]) = fill_value;
	}
    }
    have_max = 0;
    maxx();
    return 0;
}
*/

/* ************************* */


/*
 ABELRECT rec_width [rec_y0 rec_y1]
 Sets the rectangle (of width rec_width) according to the centroid of the image.
 If rec_y0 and rec_y1 are omitted, they are taken to be the top and bottom of the image
 */


int abelrect(int n, int index){

	float datval;
	int nt,nc=0,icount=0,y0=0,y1,width = 50;
	y1 = header[NTRAK]-1;
	double xcom=0.,ave=0.;	
	
	//extern Point substart,subend;
	extern DATAWORD min;
	
	
	nc = sscanf(&cmnd[index],"%d %d %d",&width,&y0,&y1);
	//printf(" width, y0 y1 %d %d %d\n",width,y0,y1);
	if (nc < 1){
		beep();
		printf("Command format is ABELRECT rec_width [rec_y0 rec_y1]\n");
		return -1;
	}
	printf(" width, y0 y1 narg %d %d %d %d\n",width,y0,y1,nc);
	for(nt=y0; nt <= y1; nt++){
		for(nc=0; nc<header[NCHAN]; nc++) {
			datval = idat(nt,nc);		
			ave += datval;					// average 
			xcom += nc * (datval-min);			// x center of mass -- subtract min
			icount++;						// number of points 
		}
	}
	xcom /= icount;
	ave = ave/(float)icount;
	xcom /= (ave-min);
	
	substart.h = xcom+.5;
	substart.v = y0;
	
	subend.h = substart.h + width;
	subend.v = y1;
	
	return 0;
	
	
}

/* ************************* */

/* ************************************************************ */
/* Slightly different abel inversion, Kevin Walsh,June 26, 1996 */
/* 																*/
/* 			     uses analytic evaluation of integral required  */
/*               for abel inversion, which involves a lot of    */
/*               evaluations of ln(x)             	            */
/*           													*/
/*    n is a scale factor for the image       					*/
/*           													*/
/* ************************************************************ */
int
kwabel(int n)			/* 	calculate abel inversion */
{
	int nc,nt,x,j;
	float di;
	DATAWORD idat(int,int),*datp;
	float sum=0, datsum, xsum, x2sum, xdatsum,frac;
	float top,bot,fx,fnc;
	float coldat[1024];    /* dummy array to hold one row of data during calculation */
	float deriv[1024];
	float area1,area2,norm=0,scale;
	
	datp = datpt+doffset;

    if (n==0) scale=1.0;
	else scale=(float)n;
    	
	/* do reconstruction replacing data */
	for(nt=0; nt<header[NTRAK];nt++) {
		
		/*********** get area under curve of a single row of intigrated data **********/
		area1 = 0.0;
		area2 = 0.0;
		for(nc=0;nc < header[NCHAN]; nc++){
			area1 += (float)(*(datp + nt*header[NCHAN] + nc));
			coldat[nc] = (float)(*(datp + nt*header[NCHAN] + nc));
		}
		area1 *= 2.0;
		area1 -= coldat[0];
		for(x=0;x < header[NCHAN]; x++){
				/*********** use least sqrs approx. with 3 pts. to calc. derivative ********/
				datsum = 0.0;
				xsum = 0.0;
				x2sum = 0.0;
				xdatsum = 0.0;
				if(x == 0){
					di = idat(nt,x+1) - idat(nt,x);	
				 }
				else {
					if(x == header[NCHAN]-1){
						di = idat(nt,x) - idat(nt,x-1);
					       }
					else {
						  for(j=0; j < 3; j++) {
						    	datsum += idat(nt,x-1+j);
						 		xsum += x+j;
								x2sum += (x+j)*(x+j);
								xdatsum += idat(nt,x-1+j)*(x+j);
						      }
						di = (xdatsum - datsum * xsum / 3.0) / ( x2sum - xsum * xsum / 3.0);
					}
				 }
				 deriv[x]=di;
		  }
		/*********** do inversion **********/
		for(nc=1;nc < header[NCHAN]; nc++){
			sum = 0.0;
			for(x=nc;x < header[NCHAN]; x++){		
				fx = (float)x;
				fnc = (float)nc;
				top = fx + sqrt(fx*fx-fnc*fnc);
				bot = fx+1.0+sqrt((fx+1.0)*(fx+1.0)-fnc*fnc);
				frac= top/bot;
				sum += (1.0/PI)*deriv[x]*log(frac);
				 }
			
	

			coldat[nc] = sum;
			area2+=PI*(2.0*(float)nc+1.0)*sum;
			
		}
	  /* At r=0 (nc=0) the integral is undefined - as a solution make f(r=0)=f(r=1) */
	  coldat[0]=coldat[1];
	  area2+=PI*sum;
	  if (area2>0.0) norm=area1/area2;
	  /* Scaling done to assure that photon counts are conserved */
	  
	  for(nc=0;nc < header[NCHAN]; nc++){
		*(datp + nt*header[NCHAN] + nc)	= coldat[nc]*norm*scale; 
		} 
		
	}
	

    
	have_max = 0;
    maxx();
    return 0;
}
/* ************************* */

int operator_is_defined=0;
int abel_row,abel_col;
DATAWORD* abel_datp=0;

/* ABEL
 Calculate the abel inversion using
 // Calculate three-point abel inversion operator Di,j
 // The index i,j start from 0.
 // The formula followed Dasch 1992 (Applied Optics) which contains several typos.
 // One correction is done in function OP1 follow Martin's PhD thesis
 */

int abelinv(int n,int index){
    int row, col, size;
    int i,j,k;
    
    DATAWORD *datp;
    
    // function prototype
    float OP0(int i,int j);
    float OP1(int i,int j);
    float OP_D(int i,int j);
    
    row = header[NTRAK];
    col = header[NCHAN];
    
    // if operator is not defined or not the right size, then calculate a new one for this image size
    if(!(operator_is_defined && abel_row==row && abel_col==col)){
        
        if (abel_datp!=NULL) {
            //printf("Operator has been defined previously. Free it\n");
            free(abel_datp);
        }
        abel_datp = malloc(row*col*sizeof(DATAWORD));
        
        if( abel_datp == 0) {
            nomemory();
            return -1;
        }
        abel_row=row;
        abel_col=col;
        //printf("abel_row is %d", row);
        i=0;
        for (k=0; k<row; k++) {
            for (j=0; j<col; j++) {
                *(abel_datp+i) = OP_D(k,j);
                i++;
            }
        }
        operator_is_defined=1;
    }
    
    size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
    
    size = (size+511)/512*512;    /* make a bit bigger for file reads */
    //printf("%d\n",row);
    
    datp = calloc(size,1);
    if(datp == 0) {
        nomemory();
        return -1;
    }

    datp = datp+doffset;    /* I guess this one make the pointer to skip the first 80 bytes information*/
    
    
    
    for (i=0; i<row; i++) {
        for (k=0; k<col; k++) {
            for (j=0; j<col; j++) {
                *(datp+i*col+k) += *(datpt+doffset+i*col+j)* *(abel_datp+k*col+j);
            }
        }
    }
    
    // copy the 2nd col to the 1st col.
    for (i=0; i<row; i++) {
        *(datp+i*col) = *(datp+i*col+1);
    }
    
    free(datpt);
    datpt = datp-doffset;    /* Needs the 80 bytes thing for buffer*/
    
    have_max=0;
    maxx();
    
    return(0);
}

// functions for the ABEL command
// The definition of operators
float OP0(int i,int j)
{
    float I0=0;
    // Define operator OP0
    if (j<i || (j==i&&i==0))
        I0 = 0;
    else if (j==i&&i!=0)
        I0 = log((pow(pow((2*j+1),2)-4*pow(i,2),0.5) +2*j+1)/(2*j))/(2*PI);
    else if (j>i)
        I0 = log((pow(pow((2*j+1),2)-4*pow(i,2),0.5)+2*j+1)/(pow((pow((2*j-1),2)-4*pow(i,2)),0.5)+2*j-1))/(2*PI);
    return (I0);
}

float OP1(int i,int j)
{
    // Define operator OP1
    float I1=0;
    if (j<i)
    {I1 = 0;}
    else if (j==i)
    {I1 = pow(pow((2*j+1),2)-4*pow(i,2),0.5)/(2*PI)-2*j*OP0(i,j);}
    else if (j>i)
    {I1 = (pow(pow((2*j+1),2)-4*pow(i,2),0.5)-pow(pow((2*j-1),2)-4*pow(i,2),0.5))/(2*PI)-2*j*OP0(i,j);}
    return (I1);
}

float OP_D(int i,int j)
{
    // Calculate three-point abel inversion operator Di,j
    // The index i,j start from 0.
    // The formula followed Dasch 1992 (Applied Optics) which contains several typos.
    // One correction is done in function OP1 follow Martin's PhD thesis
    
    float ID=0;
    if (j<i-1)
    {ID = 0.;}
    else if (j==i-1)
    {ID = OP0(i,j+1)-OP1(i,j+1);}
    else if (j==i)
    {ID = OP0(i,j+1)-OP1(i,j+1)+2*OP1(i,j);}
    else if (j>=i+1)
    {ID = OP0(i,j+1)-OP1(i,j+1)+2*OP1(i,j)-OP0(i,j-1)-OP1(i,j-1);}
    else if (i==0&&j==1)
    {ID = OP0(i,j+1)-OP1(i,j+1)+2*OP1(i,j)-2*OP1(i,j-1);}
    return (ID);
}

/* ************************* */

// start up a task using the time manager
/*
TMTask control_routine;


ttask(int n,int index)
{
	static int installed = 0;
	ProcPtr pnam;
	void do_control();
	//static	int    (*funame)() =  do_control; 
	if( !installed) {
		
		//pnam = (ProcPtr)do_control;
		//control_routine.tmAddr = NewTimerProc(pnam);
		//control_routine.tmWakeUp = 0;
		//control_routine.tmReserved = 0;
		//InsXTime((QElem*)&control_routine);						//{install the task record}
		//PrimeTime((QElem*)&control_routine, 1000);				//{activate the task record}
		//installed = 1;
		
	}
		
}

void do_control()
{
	static int n=0;
	n++;
	PrimeTime((QElem*)&control_routine, 1000);

}
*/
/* ************************* */
/*----------------------------------------------------------------*/


int stripe(n,index)		// stripe correct Rayleigh images
int n,index;	

/* Command:

	STRIPE start_col end_col rotation_angle [ave_value]
	
	This command does the following operations using other internal routines --
	
		1) rotate the existing image by "rotation_angle"
		2) crop the rotated image to be so that no "filled" pixels are present (will be smaller than original)
		3) divide each row of the image by the average of the pixels between start_col and end_col
	
	If ave_value is not specified, the average of the region inside the box will be used
*/ 

{
	extern char cmnd[];
    extern DATAWORD *datpt;
	extern int	doffset;
	extern Point substart,subend;
	
	DATAWORD *datp;
	DATAWORD idat(int,int);
	
	int i,j,k,pxstart,pxend,width,height;
	float angle,ave,row_ave,v;

	ave = 0.0;
	/* Check to see if there was a second argument */
	j=0;
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			j = sscanf(&cmnd[index],"%d %d %f %f",&pxstart,&pxend,&angle,&ave);
			break;
		}
	}
	if (j < 3) {
		beep();
		printf("At least three arguments needed. Command format: \n");
		printf("start_col end_col rotation_angle [ave_value]\n");
		return -1;
	}
	
	printf("Rotate by %.2f degrees\n",angle);
	printf("Start integration at Column %d\n",pxstart);
	printf("End integration at Columnl %d\n",pxend);
	
	if( j == 3 ) {
		printf("Scale by average value.\n");
	} else {
		if (ave <= 0.0) {
			beep();
			printf("Average must be positive.\n");
			return -1;
		}
		printf("Scale by %.1f.\n",ave);
	}
	
	sprintf(cmnd,"%f",angle);
	index = 0;
	
	width = header[NCHAN];
	height = header[NTRAK];
	
	rotate(1,index);
	
	i = header[NCHAN] - width;
	j = header[NTRAK] - height;
	
	substart.h = i;
	substart.v = j;
	subend.h = header[NCHAN]-1-i;
	subend.v = header[NTRAK]-1-j;

	
	index = 0;
	strcpy(cmnd,"OMAtemp");

	saverectangle(fullname(cmnd,GET_DATA));
	setdata(cmnd);
	fileflush(cmnd);	/* for updating directory */
	
	
	// Read in the rotated image
	index = 0;
	strcpy(cmnd,"OMAtemp");
	
	getfile(0,index);

	datp = datpt + doffset;
	
	if( pxstart < 0 ) 
		pxstart = 0;			
	if( pxend > header[NCHAN]) 
		pxend = header[NCHAN];

	k=0;
	
	if(ave == 0.0) {
		for(i = 0; i<header[NTRAK]; i++){
			for(j=pxstart; j<= pxend; j++){
				ave += idat(i,j);
			}
		}
		ave = ave/(pxend - pxstart +1)/header[NTRAK];
	//printf("%2f is ave\n",ave);
	}

	for(i = 0; i<header[NTRAK]; i++){
		row_ave = 0.0;
		for(j=pxstart; j<= pxend; j++){
			row_ave += idat(i,j);
		}
		row_ave = row_ave/(pxend - pxstart +1);
		//printf("%2f is ave\n",row_ave);
	
		for(j=0; j< header[NCHAN]; j++){
			v = row_ave;
			*(datp++) = v;
		}

	}
	// Save them stripe file
	
	have_max = 0;
	
	index = 0;
	strcpy(cmnd,"OMAstripe");

	savefile(0,index);
	setdata(cmnd);
	fileflush(cmnd);	/* for updating directory */

	// Read in the rotated image
	index = 0;
	strcpy(cmnd,"OMAtemp");
	
	getfile(0,index);

	datp = datpt + doffset;
	
	if( pxstart < 0 ) 
		pxstart = 0;			
	if( pxend > header[NCHAN]) 
		pxend = header[NCHAN];

	k=0;
	
	if(ave == 0.0) {
		for(i = 0; i<header[NTRAK]; i++){
			for(j=pxstart; j<= pxend; j++){
				ave += idat(i,j);
			}
		}
		ave = ave/(pxend - pxstart +1)/header[NTRAK];
	//printf("%2f is ave\n",ave);
	}

	for(i = 0; i<header[NTRAK]; i++){
		row_ave = 0.0;
		for(j=pxstart; j<= pxend; j++){
			row_ave += idat(i,j);
		}
		row_ave = row_ave/(pxend - pxstart +1);
		//printf("%2f is ave\n",row_ave);
	
		for(j=0; j< header[NCHAN]; j++){
			v = *(datp)*ave/row_ave;
			*(datp++) = v;
		}

	}
	have_max = 0;
	maxx();
	return 0;

}

// *****************

//
int 
mcalx(int n,int index)			/* Using the data in the current buffer as rayleigh input and
						   the specified file as the fuel input, create the conserved
						   scalar image */
//
//				Functions to be defined by user {use 'getfun n filename'}:
//				n =		0 : Cp - specific heat [cp.dat]
//						1 : RaX - Rayleigh parameter [rax.dat]
//						2 : W - mixture molecular weight [mw.dat]
//						3 : reac - reactness vs. f from counterflow code
//
{
	float cp_h(float),rax_h(float),mw_h(float);
	int i,j,nt,nc;
	DATAWORD *raypoint,*flpoint=0;
	unsigned int  raysize;
	TWOBYTE	rayxsize,rayysize;	
	float f0,aT,Wmix,x,aT1,aT2,fold=0,D2,denom,fluor,ray,T,Yf=0;
	float r=0,aTH,aTC,WmixC,WmixH,Cpmix,CpH,CpC;
	float f = 0.0, raman = 0.0;
	int notgreat = 0;
	
	float reac_h(float);
	
    extern char    cmnd[],*fullname();
  	float tabfun(int,float);

	if(have_constants == 0) {
		beep();
		printf(" Load Constants First (use FCONST filename).\n");
		return -1;
	}
	
	/* copy current data into the rayleigh array */
	
	raysize = header[NCHAN]*header[NTRAK];
	rayxsize = header[NCHAN];
	rayysize = header[NTRAK];
		
	raypoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(raypoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++) 
		*(raypoint + i) = *(datpt+i+doffset);
	
	/* Get the fuel image */
	
	for(i=0; cmnd[index] != ' ' && cmnd[index] != EOL; txt[i++] = cmnd[index++] ) ;		// treat space as filename delimitor here
	txt[i] = 0;
	index++;
			
	fullname(txt,GET_DATA);
	getfile (0,-1);			/* get the file */
	/* keylimit(-2);			 reset printing to previous mode*/

	if( (rayxsize != header[NCHAN]) || (rayysize != header[NTRAK]) ) {
			beep();
			printf("Image sizes are different. \n");
			goto cleanup;
	}
	
	/* copy current data into the fuel array */
	
		
	flpoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(flpoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++)
		*(flpoint + i) = *(datpt+i+doffset);
		
			
	/* now do the calcs for each point */
	
	i = 0;
	aT2 = T2*Rc2;
	aT1 = T1*Rc1;
	Wmix = W2;
	aT = aT2;
	T = T2;
	
	
	denom = Yf1 + (cp_h(1.000) * T1 - cp_h(0.000) * T2) / Q;
	D2 = (Tcal * Fcal) / (Wfu * Xfu);	/* fluorescence (fuel) calibration */
	
	
	for( nt = 0; nt<header[NTRAK]; nt++) {
		
		for( nc = 0; nc<header[NCHAN]; nc++) {
		
			fluor = raman = *(flpoint+i);
			ray = *(raypoint+i);
			
			
			if( start_over ) { /* start from the initial condition each time */
				Wmix = W2;
				T = T2;
				aT = aT2;
				f0 = 0.000;
			} else {
				f0 = f;
			}

			f = fluor / D2 / Wmix * aT / ray + (cp_h(f0) * aT / ray - cp_h(0.000) * T2) / Q;
			f /= denom;

		//	fake = 0.2142*ray-70;
			
		//	if((fabs(fluor-fake)/fake > 0.10) && fluor>50) fluor = (fake + ((fluor-fake)/(10.0*fake))); // check for scale problems
				
			for(j=0; j<MaxIt; j++) {
				
				fold = f;
				
				WmixC = 1./ (f/W1 + (1.0-f)/W2);
				x = f * WmixC/W1;					// x is fuel mole fraction

				aTC = (x) * aT1 + (1-x) * aT2;
				
				T = aTC/ray;

				r = (T-T2)/(Tad-T2);  /* reactedness */
				
				/*
				if(r>1.0) {
					r = 1.0;//reac_h(f);
				//	rerr = abs(minray - ray);
				//	ray +=sqrt(rerr); 
					}
				if(r<0.0) r= 0.0;
				if(r<reac_h(f)) ray -=1.0;			
			//	Define Mixedness parameter:	M=1 : pure mixing problem
			//								M=0 : pure reacting limit
			//  Constrain M to lie within logical limits 0..1 (for my sanity)
			//	reac_h : functional form comes from calculations
				M = reac_h(f) - r;
				if(M>1.0) M=1.0;
//				if(M<0.0) M=0.0;

*/

/*


// Stoichiometric
				if(fabs(f-FlRich)<0.2) {
					if(M>0.1) {
						ray -= 1.;
						if(r>=0.95) fluor+=0.5;
					}
				//	fluor += 0.1;
					}

// Fuel Rich #1
				if((f-FlRich)>0.2 && f<0.75) {
					if(M<0.0) {
						ray += 1.0; 
						fluor -= 0.0; }
					if(M>0.1) {
						ray -=2.0;
						fluor += 1.0; }
					}
// Fuel Rich #2
				if((f)>0.75) {
					if(M<0.0) {
						ray -= 0.5; 
						//  fluor -= 0.0;
					}
					if(M>0.1) {
						fluor +=1.0;
						//  fluor += 1.0; 
					}
				}
// Fuel Lean
				if((f-FlRich)<-0.2) {
					if(M>0.1) {
						ray -= 0.5; }
					}
*/
/*
 				if(M<0.0) M=0.0;
				if(fluor<0) fluor = 0.0;
//				if(fluor/Fcal > 0.25) fluor = 0.25*Fcal;

*/
				aTH = aT2 * rax_h(f);
				//aT = M*aTC + (1-M)*aTH;	// Temperature redefine based on M
				aT = (1-r)*aTC + r*aTH;

				T = aT/ray;				//
				r = (T-T2)/(Tad-T2);	/* reactedness */
				if(r>1.0) r= 1.0;
				if(r<0.0) r= 0.0;

				WmixH = mw_h(f);
				//Wmix = M*WmixC + (1-M)*WmixH;
				Wmix = (1-r)*WmixC + r*WmixH;
																
				Yf = fluor / D2 / Wmix * aT / ray;
				
//				Yf = Yf * (1 + RCor * r * r);	// correction for fuel loss
				if(Yf>1.0) Yf= 1.0;
				if(Yf<0.0) Yf= 0.0;				
				
				CpH = cp_h(f);
				CpC = f*cp_h(1.00) + (1-f)*cp_h(0.00);
				//Cpmix = M*CpC + (1-M)*CpH;
				Cpmix = (1-r)*CpC + r*CpH;
				
				f = Yf + (Cpmix * aT / ray - cp_h(0.000) * T2) / Q;
				f /= denom;
				
				//if(f>1.0) f= 1.0;
				//if(f<0.0) f= 0.0;				
								
				if( fabs((f - fold)) < Err ) break;
			}

		//	f = Fmap(f); /* Remap f to correct for fuel dropout and constant cp (if used above) */
			
		//	if( fabs((f - fold)/f) > Err ) notgreat++;  // Old way
			if( fabs((f - fold)) > Err ) notgreat++;	// New way - absolute error
			
			switch(fcalc_case) {
			case GET_T:	
				*(datpt+(i++)+doffset) = aT / ray;
				break;
			case GET_ERROR:
				*(datpt+(i++)+doffset) = (f - fold)/f *10000;
				break;
			case GET_ITERATION:
				*(datpt+(i++)+doffset) = r*1000.0;
				break;
			case GET_WMIX:
				*(datpt+(i++)+doffset) = Wmix * 10;
				break;
			case GET_YF:
				*(datpt+(i++)+doffset) = Yf * 1000.0;
				break;
			case GET_AT:
				*(datpt+(i++)+doffset) = aT /295.0;
				break;
			default:
			case GET_F:
				*(datpt+(i++)+doffset) = f*1000.0 ;
				break;
			}

		}
	}

cleanup:

	if(raypoint!=0) {
		free(raypoint);	
		raypoint = 0;
	}
	if(flpoint!=0) {
		free(flpoint);	
		flpoint = 0;
	}
	have_max = 0;
	setarrow();
	pprintf(" %d questionable points.\n",notgreat);
	return 0;
}
/* ********** */
float cp_h( float f)
{
	float cp = 0.00;
	
	cp = tabfun(0,f);
		
	return (cp);
}

float rax_h( float f)
{
	float rax = 1.00;
	
	rax = tabfun(1,f);
	
	return (rax);
	
}

float mw_h( float f)
{
	float mw = 0.00;
	
	mw = tabfun(2,f);
		
	return (mw);
}

float reac_h( float f)
{
	float rr = 0.00;
	
	rr = tabfun(3,f);
		
	return (rr);
}
// ***************
int 
mconst(int n,int index)			/* Read in constants to be used for MCALX */	
{
	extern char cmnd[],*fullname();
	int notfound = 0;
	
	FILE *fild;
	int getfloat(FILE *fp,float *x);
	float tmp;
	
	//fild = open(fullname(&cmnd[index],GET_DATA),READMODE);
	fild = fopen(fullname(&cmnd[index],GET_DATA),"r");


	if( fild != NULL) {
		if( getfloat(fild,&Fcal) != 1) notfound = 1;
		if( getfloat(fild,&Tcal) != 1) notfound = 2;
		if( getfloat(fild,&Rc1) != 1) notfound = 3;
		if( getfloat(fild,&Rc2) != 1) notfound = 4;
		if( getfloat(fild,&T1) != 1) notfound = 5;
		if( getfloat(fild,&T2) != 1) notfound = 6;
		if( getfloat(fild,&W1) != 1) notfound = 7;
		if( getfloat(fild,&W2) != 1) notfound = 8;
		if( getfloat(fild,&Q) != 1) notfound = 9;
		if( getfloat(fild,&Yf1) != 1) notfound = 10;
		if( getfloat(fild,&Wfu) != 1) notfound = 11;
		if( getfloat(fild,&Xfu) != 1) notfound = 12;
		if( getfloat(fild,&Tad) != 1) notfound = 13;
		if( getfloat(fild,&FlRich) != 1) notfound = 14;
		if( getfloat(fild,&RCor) != 1) notfound = 15;
		if( getfloat(fild,&Err) != 1) notfound = 16;
		if( getfloat(fild,&tmp) != 1) notfound = 17;
		MaxIt = tmp;
		if( getfloat(fild,&tmp) != 1) notfound = 18;
		start_over = tmp;
		
    	fclose(fild);

		if(notfound != 0) {
			beep();
			printf(" Data Format Problem. Argument %d\n",notfound);
			return -1; 
		}
		
		have_constants = 1;
		return 0;

	}
	else {
		beep();
		printf(" File Not Found.\n");
		return -1;
	}
	
}
// ***************
int
jfstripe(int n,int index)
				//  get a stripe response for the image -- j. frank algorithm 
				// JFSTRIP horiz_offset width
{
	DATAWORD *datp,*datp2;
	DATAWORD idat(int,int);
	
 	int h,w,wb,offset,offset2,i,size,intlength;
 	float sum,localmean;
    offset = 0;
    offset2=0;
	/* Check to see if there was a second argument */
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d",&offset,&offset2);
			break;
		}
	}
	
	printf("Vertical offset: %d\nHorizontal integration: %d\n",offset,offset2);

	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */

	datp2 = datp = malloc(size);
	if(datp == 0) {
		nomemory();
		return -1;
	}

      /* 
      h = index for height or row number
	  w = index for width or column number
 	  wb = index for integration along rows
	  offset = row offset above and below stripe used to determine local mean
	  offset2 =column offset used for horizontal integration limits
			(integration is from wb-offset2 to wb+offset2)
	  data = data without stripe correction
	  datastripe = stripe response image
 	  */
    for ( h = 0; h < offset; h++){ 
	  	for ( w = 0; w < header[NCHAN]; w++) { 
	  	*(datp+h*header[NCHAN]+w+doffset) = 1000;
	  	}
	 }

    for ( h = header[NTRAK]-offset; h < header[NTRAK]; h++){ 
	  	for ( w = 0; w < header[NCHAN]; w++) { 
	  	*(datp+h*header[NCHAN]+w+doffset) = 1000;
	  	}
	 }
 	  

    for ( h = offset; h < header[NTRAK]-offset; h++){ // h is the height or row number
	  	for ( w = 0; w < header[NCHAN]; w++) { //-- NO CHECKING THAT WE ARE IN BOUNDS YET --
	    	intlength = 0;
	      	sum = 0.0;
	      	// do integration 
	      	for ( wb = w-offset2; wb<w+offset2+1; wb++) {
		  		//if ((data[(h-offset)*width+wb] != 0.0) && (data[(h+offset)*width+wb] != 0.0)) {
		      	if ( wb>0 && wb<header[NCHAN]) {
		      		localmean = (idat(h-offset,wb) + idat(h+offset,wb) )/2.0;
		      		//(data[(h-offset)*width+wb] + data[(h+offset)*width+wb])/2.0;
		      		sum += 1000.0*idat(h,wb)/localmean;
		      		intlength++;
		    	}
			}
	      	if (intlength > 0) {
		  		//datastripe[h*width+w] = sum/(float)intlength;
		  		*(datp+h*header[NCHAN]+w+doffset) = sum/(float)intlength;
			} else {
		  		//datastripe[h*width+w] = 1000.0;
		  		*(datp+h*header[NCHAN]+w+doffset) = 1000;
			}
	    }

	}
	free(datpt);
	datpt = datp2;
	have_max = 0;
	maxx();
	return 0;

}	

/* ************************* */

// AMASK value

/*	make a "mask" corresponding to air in Rayleigh flame images

	set new image value to 1 under the following conditions:
		image value > value
		all image values along the current row have satisfied the above condition as well
	otherwise, set the image value to 0
	
	This assumes the cold air part of the immage is on the left hand side
	
	
*/
int
amask(int n,int index)
{
	extern int	doffset;
	
	int nt,nc;
	DATAWORD *datp,idat(int,int),value;
	void bfill(int x, int y, DATAWORD bvalue, DATAWORD nvalue);
	
	value = n;
	
	datp = datpt+doffset;
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN];nc++){
			if( idat(nt,nc) > value) {
				*(datp + nc + nt*header[NCHAN]) = -1;
			} else {
					break;
			}
		}
		
		for(nc=0; nc<header[NCHAN];nc++){
			if( idat(nt,nc) == -1 ) {
				bfill(nc,nt-1,value,-1);
			} else {
					break;
			}
		}

	}	
	for(nt=header[NTRAK]-2; nt >= 0; nt--) {
		
		for(nc=0; nc<header[NCHAN];nc++){
			if( idat(nt,nc) == -1 ) {
				bfill(nc,nt+1,value,-1);
			} else {
					break;
			}
		}

	}	


	have_max = 0;
	maxx();
	return 0;

}


/*----------------------------------------------------------------*/

// boundary fill algorithm from foley and van dam

void bfill(int x, int y, DATAWORD bvalue, DATAWORD nvalue)
{
	DATAWORD rp;
	
	if(y<0) return;
	if(x<0) return;
	if(x>header[NCHAN]) return;
	if(y>header[NTRAK]) return;
	
	rp = idat(y,x);
	if( rp > bvalue && rp > nvalue) {
		*(datpt+doffset+header[NCHAN]*y+x) = nvalue ;
		bfill(x,y-1,bvalue,nvalue);
		bfill(x,y+1,bvalue,nvalue);
		bfill(x-1,y,bvalue,nvalue);
		bfill(x+1,y,bvalue,nvalue);
	
	}
} 


/* ************************* */


int
singrd(int n,int index)				/* draw grid from sin function */
{
	int nc,nt;
	int i, dum=0;
	float rsize=0;
	DATAWORD *datp;
	float radius,dist,x,y;
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%f %d",&rsize,&dum);\
			break;
		}
	}
	datp = datpt+doffset;
	printf("rsize = %f\n",rsize);
	radius = rsize;
	printf("radius = %f\n",radius);
	if (radius <= 0 || radius > header[NCHAN] || radius > header[NTRAK]) radius = 20.0;
	printf("radius = %f\n",radius);
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			x = radius - fmod(nc,radius*2);
			y = radius - fmod(nt,radius*2);
			dist = sqrt(y*y + x*x);
			if (dist < radius){
				*(datp++) = 10000.0*cos(dist/(.67*radius)*PI/2);
			}else{
				*(datp++) = 10000.0*cos(dist/(.67*radius)*PI/2);
				// *(datp++) = 0;
			}
		}
	}
	trailer[SFACTR] = 1;
	have_max = 0;
    maxx();
    return 0;
}


/* --------------------------- */
int
ramp(int n,int index)				/* fill current image with a ramp from 0 to the number of number of channels */
{
	extern DATAWORD *datpt;
	int nc,nt;
	DATAWORD *datp;
	
	datp = datpt+doffset;
	//printf("rsize = %f\n",rsize);
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
				*(datp++) = nc;
		}
	}
	trailer[SFACTR] = 1;
	have_max = 0;
    maxx();
    return 0;
}

/* --------------------------- */
int 
line(int n,int index)				/* set all pixels in the specified row of the current image to 1 */
{
	int nc;
	DATAWORD *datp;
	
	datp = datpt+doffset;
	
	for(nc=0;nc < header[NCHAN]; nc++){
			*(datp+n*header[NCHAN]+nc) = 1;
	}

	have_max = 0;
    maxx();
    return 0;
}
/* --------------------------- */

#define NUM_CTRS 200
int
ctrlst(int n,int index)				// treat the current image as a set of connected contours
									// make lists of the x-y pixels that define the contour
									// goes clockwise
									// nonzero values are on the contour
									// after the routine is done, the original image should all be zeros
									// number of closed contours varies depending on the image
									//
									// CTR [num_pts]
									//   num_pts is the number of points on either side
{
	int nc,nt,h,v,vlast,hlast,i,j;
	int np;
	DATAWORD idat(int,int);
	DATAWORD *datp;
	float get_ang_1( Point*,int );
	float get_ang_2( Point*,int );
	float dist(Point*,Point*);
	
	Point *ctrpts[NUM_CTRS];
	int npts_ctr[NUM_CTRS],num_ctrs=0;
	float angle;
	
	datp = datpt+doffset;
	
	if(n < 1) n = 1;
	
	for(nt=1; nt<header[NTRAK]-1;nt++) {
		for(nc=1;nc < header[NCHAN]-1; nc++){
			if(idat(nt,nc)!= 0) {
				npts_ctr[num_ctrs] = 0;
				// have the first point on a contour
				// allocate space for points
				ctrpts[num_ctrs] = (Point*) malloc(50000 * sizeof(Point));
				hlast = h = (ctrpts[num_ctrs]+npts_ctr[num_ctrs])->h = nc;
				vlast = v = (ctrpts[num_ctrs]+npts_ctr[num_ctrs])->v = nt;
				npts_ctr[num_ctrs] += 1;
				// look for more points
				while(1){
					
					// look N
					if(idat(v-1,h) != 0) {
						v = v-1;
						//ctrpts[npts_ctr].h = h;
						//ctrpts[npts_ctr++].v = v;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->h = h;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->v = v;
						npts_ctr[num_ctrs] += 1;
						*(datp + h + v*header[NCHAN]) = 0;	// mark this point found
						goto check;						
					}
					// look E
					if(idat(v,h+1) != 0) {
						h = h+1;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->h = h;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->v = v;
						npts_ctr[num_ctrs] += 1;
						*(datp + h + v*header[NCHAN]) = 0;	// mark this point found
						goto check;						
					}
					// look S
					if(idat(v+1,h) != 0) {
						v = v+1;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->h = h;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->v = v;
						npts_ctr[num_ctrs] += 1;
						*(datp + h + v*header[NCHAN]) = 0;	// mark this point found
						goto check;						
					}
					// look W
					if(idat(v,h-1) != 0) {
						h = h-1;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->h = h;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->v = v;
						npts_ctr[num_ctrs] += 1;
						*(datp + h + v*header[NCHAN]) = 0;	// mark this point found
						goto check;					
					}
					// look NE
					if(idat(v-1,h+1) != 0) {
						v = v-1;
						h = h+1;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->h = h;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->v = v;
						npts_ctr[num_ctrs] += 1;
						*(datp + h + v*header[NCHAN]) = 0;	// mark this point found
						goto check;						
					}
					// look SE
					if(idat(v+1,h+1) != 0) {
						v = v+1;
						h = h+1;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->h = h;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->v = v;
						npts_ctr[num_ctrs] += 1;
						*(datp + h + v*header[NCHAN]) = 0;	// mark this point found
						goto check;						
					}
					// look SW
					if(idat(v+1,h-1) != 0) {
						v = v+1;
						h = h-1;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->h = h;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->v = v;
						npts_ctr[num_ctrs] += 1;
						*(datp + h + v*header[NCHAN]) = 0;	// mark this point found
						goto check;					
					}
					// look NW
					if(idat(v-1,h-1) != 0) {
						v = v-1;
						h = h-1;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->h = h;
						(ctrpts[num_ctrs]+npts_ctr[num_ctrs])->v = v;
						npts_ctr[num_ctrs] += 1;
						*(datp + h + v*header[NCHAN]) = 0;	// mark this point found
						goto check;						
					}

check:				if( h == ctrpts[num_ctrs]->h && v == ctrpts[num_ctrs]->v)
						break;
					if( h == hlast && v == vlast){
						printf("Can't complete -- ");
						break;
					
					}
					
					hlast = h;
					vlast = v;
				
				}
				
				printf("%d points",npts_ctr[num_ctrs]);
				// check to see if this can be added to another contour
				for(i=0; i<num_ctrs; i++){
					// start points match?
					if( dist(ctrpts[i],ctrpts[num_ctrs]) <= 1.5){
						printf("- %d & %d start pts- ",num_ctrs,i);
					}
					// end points match?
					if( dist(ctrpts[i]+npts_ctr[i]-1,ctrpts[num_ctrs]+npts_ctr[num_ctrs]-1) <= 1.5){
						printf("- %d & %d end pts- ",num_ctrs,i);
					}
					// start-end points match?
					if(dist(ctrpts[i]+npts_ctr[i]-1,ctrpts[num_ctrs]) <= 1.5){
						printf("- %d & %d start-end pts- ",num_ctrs,i);
					}
					// end-start points match?
					if( dist(ctrpts[i],ctrpts[num_ctrs]+npts_ctr[num_ctrs]-1) <= 1.5){
						printf("- %d & %d end-start pts- ",num_ctrs,i);
						// add the points in the found contour to this one
						for(j=0; j< npts_ctr[i];j++){
							*(ctrpts[num_ctrs]+npts_ctr[num_ctrs]) = *(ctrpts[i]+j);
							npts_ctr[num_ctrs] += 1;
						}
						// free the old representation of the found contour
						free(ctrpts[i]);
						ctrpts[i] = ctrpts[num_ctrs];
						npts_ctr[i] = npts_ctr[num_ctrs];
						num_ctrs--;
						break;
					}
				}
				/*
				if(npts_ctr[num_ctrs] <= 3){
					free(ctrpts[num_ctrs]);
					printf(" -- contour rejected\n");
				} else {
					num_ctrs++;
					printf("\n");
					
				}
				*/
				num_ctrs++;
				printf("\n");
			}
		}
	}
	// now have the contours
	printf("%d Contours found\n",num_ctrs);
	
	// set background of image to -1
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			*(datp + nc + nt*header[NCHAN]) = -1;
		}
	}

	
	for(nc = 0; nc < num_ctrs; nc++){				// process each contour
		for( np = n; np < npts_ctr[nc]-n; np++) {		// process the points in the contour
			// calculate the angle at each point
			
			angle = get_ang_1(ctrpts[nc]+np,n);
			//printf("%f\n",angle);
			/*
			angle = get_ang_2(ctrpts[nc]+np,n);
			printf("%f\n",angle);
			*/
			
			angle *= 180.0/PI;			// in degrees
			angle += 90;
			// set the pixel value to the angle
			h = (ctrpts[nc]+np)->h;
			v = (ctrpts[nc]+np)->v;
			*(datp + h + v*header[NCHAN]) = angle;
			// label the point with its contour number
			*(datp + h + v*header[NCHAN]) = nc;
			//printf("%f\n",angle);
			
		}
		// now fill in the missing points
		
		// check for connected contour
		/*
		if( dist(ctrpts[nc],ctrpts[nc]+npts_ctr[nc]-1) <= 1.5) {	// closed contour
			printf("Contour %d is closed\n",nc+1);
			// need t0 fill in the missing points
			for(i=0; i<2*n ; i++){
				// create a set of continuous points at the end of the contour list
				*(ctrpts[nc]+npts_ctr[nc]+i) = *(ctrpts[nc]+npts_ctr[nc]-2*n+i);
				*(ctrpts[nc]+npts_ctr[nc]+i+2*n+1) = *(ctrpts[nc]+i+1);
			}
			*(ctrpts[nc]+npts_ctr[nc]+2*n) = *(ctrpts[nc]); // 
			for(i=0; i<n ; i++){
				angle = get_ang_1(ctrpts[nc]+npts_ctr[nc]+i+2*n,n);
				angle *= 180.0/PI;			// in degrees
				angle += 90;
				// set the pixel value to the angle
				h = (ctrpts[nc]+i)->h;
				v = (ctrpts[nc]+i)->v;
				*(datp + h + v*header[NCHAN]) = angle;

				angle = get_ang_1(ctrpts[nc]+npts_ctr[nc]+i+n,n);
				angle *= 180.0/PI;			// in degrees
				angle += 90;
				// set the pixel value to the angle
				h = (ctrpts[nc]+npts_ctr[nc]-n +i)->h;
				v = (ctrpts[nc]+npts_ctr[nc]-n +i)->v;
				*(datp + h + v*header[NCHAN]) = angle;
			}


		} else {
			printf("Contour %d is open\n",nc+1);
		}
		*/
		free(ctrpts[nc]);
	}
	
	have_max = 0;
    maxx();
    return 0;
}

float dist(Point* p1,Point* p2)
{
	float d;
	
	d = sqrt( (p1->h - p2->h)*(p1->h - p2->h) + (p1->v - p2->v)*(p1->v - p2->v));
	return d;
}

// Solve for the angle by averaging n points on either side of the point considered and getting the slope

float get_ang_1( Point* pts,int n)	// get the slope form n points on either side of the point
{
	float x0,y0,x1,y1,angle,slope;
	int i;
	
	
	x0 = y0 = x1 = y1 = 0.0;
	
	for(i=1; i<=n; i++){
		x0 += (pts-i)->h;
		y0 += (pts-i)->v;
		
		x1 += (pts+i)->h;
		y1 += (pts+i)->v;
	}

		if (x1 == x0) {
					angle = PI/2.0;
		} else {
			slope = (y1-y0)/(x1-x0);
			angle = atan(slope);
		}
	return angle;
	
}

// Solve for the angle by a least squares fit to the n points on either side of the main point

float get_ang_2( Point* pts,int n)	// get the slope from n points on either side of the point
{
	float angle,slope;
	int i;
	float sumxx,sumx,sumy,sumyy,sumxy;
	
	
	sumxx=sumx=sumy=sumyy=sumxy=0.0;
	
	for(i= -n; i<= n; i++){
		sumxx += (pts+i)->h*(pts+i)->h;
		sumyy += (pts+i)->v*(pts+i)->v;
		sumxy += (pts+i)->v*(pts+i)->h;
		sumx += (pts+i)->h;
		sumy += (pts+i)->v;

	}
	
	n = 2*n+1;
	
	//M = (N*SXY - SX*SY)/(N*SX2 - SX**2.0)
	//GRAD=(DTAPTS*(SUMXY)-(SUMXI*SUMYI))/(DTAPTS*SUMX2-SUMXI**2)
	
	if ((n*sumxx - sumx*sumx) == 0.0) {
		angle = PI/2.0;
	} else {
		slope = (n*sumxy - sumx*sumy)/(n*sumxx - sumx*sumx);
		angle = atan(slope);
	}
	
	return angle;
	
}

/* --------------------------- */

int trace2(int n, int index)			// ray trace across contour lines
										// TRACE y0 angle index2
{
	int ix,iy;
	int narg,ncross;
	float x,y,rindex1,rindex2,theta,theta_min,theta_max,ti;
	float theta1,theta2,int_ang,xc,yc;
	DATAWORD *datp,idat(int,int);
		
	datp = datpt+doffset;
	
	// start at left hand side of image
	ix = 0;
	iy = n;		// pixel location
	
	x = ix;		// ray location	
	y = iy+0.5;	// start at the center of the pixel
	theta = 0.0;
	rindex1 = 1.0;
	rindex2 = 1.3;
	ncross = 0;
	xc=yc=0.0;
	
	narg = sscanf(&cmnd[index],"%d %f %f",&iy,&theta, &rindex2);

	// propogate across the entire image
	while(ix < header[NCHAN] && iy > 0 && iy < header[NTRAK]){
		// apply snells law if there is an interface
		if(idat(iy,ix) > -1 && idat(iy,ix) < 181) {
			if(  sqrt((xc-x)*(xc-x) + (yc-y)*(yc-y)) > 2.5){
				
				int_ang = idat(iy,ix)*PI/180.0;		// angles in radians
				
				
					if( theta <= int_ang ){
						theta1 = int_ang-theta;
						if(theta1 > PI/2.0) {
							theta1 = PI - theta1;
							theta2 = asin(rindex1/rindex2*sin(theta1));
							theta = theta2 + int_ang - PI;
						}else {
							theta2 = asin(rindex1/rindex2*sin(theta1));
							theta =  (int_ang - theta2);
						}
					} else {
						theta1 = theta-int_ang;
						if(theta1 > PI/2.0) {
							theta1 = PI - theta1;
							theta2 = asin(rindex1/rindex2*sin(theta1));
							theta = theta2 -int_ang;
						}else {
							theta2 = asin(rindex1/rindex2*sin(theta1));
							theta = theta2-int_ang;
						}	
					}
				ncross++;
				
				// propagate 2 pixels normal to the interface to avoid double counting
				//ix += 2.0*abs(cos(int_ang));
				//x = ix;
				//y += 2.0*sin(int_ang);
				//y = iy+.5;
				
				//if(ncross&1 == 0) theta = -theta;
				
				ti = rindex1;
				rindex1 = rindex2;
				rindex2 = ti;
				xc = x;
				yc = y;
			}
			
		}
	
		*(datp + ix + iy*header[NCHAN]) = 200;	// fill in this pixel
		if(x == (float)ix){		// start from LHS of pixel
			theta_max = atan( (float)(iy+1) - y );
			theta_min = atan( (float)(iy) - y );
			if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
				ix++;
				x = ix;
				y = y + tan(theta);
			} else if(theta > theta_max) {			// this will go to pixel above
				x = x + ((float)(iy+1) - y)/tan(theta);
				iy++;
				y = iy;
			} else {								// this goes to pixel below
				x = x + ((float)(iy) - y)/tan(theta);
				iy--;
				y = iy+1;
			}	
		} else {									// start from top or bottom of pixel
			theta_max = atan( 1.0/(1.0 - x + (float)ix));
			theta_min = -theta_max;
			if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
				y = y + tan(theta)*(1.0 - x + (float)ix);
				ix++;
				x = ix;
			} else if(theta > 0.0) {				// this will go to pixel above
				x = x + 1.0/tan(theta);
				iy++;
				y = iy;			
			} else {								// this goes to pixel below
				x = x - 1.0/tan(theta);
				iy--;
				y = iy+1;
			}
		}
	}
	printf("%d Contours crossed\n",ncross);
	return 0;
}

/* --------------------------- */

int trace(int n, int index)			// ray trace across the image
									// continuous image interpreted as local index of refraction
									// index = 1.0 + signal/scale_factor
									// TRACE y0 angle scale_factor
									// y0 is the starting location of the ray
									// if y0 < 0, use y0 as an increment and trace entire image
									//		starting at the first row of pixels
									// angle is the initial propogation direction (in radians, 0 is horizontal)
									
{
	int ix,iy,nt,nc,ixo,iyo;
	int narg,size,entire_image=0;
	double x,y,rindex1,rindex2,theta,theta_min,theta_max,dy,y_start,y_end;
	double theta1,theta2,int_ang,nx,ny,a,factor,dx=1.0,dyp=0.0;
	float t,h,f;
	DATAWORD *datp,idat(int,int),*datp2;
	
	
	narg = sscanf(&cmnd[index],"%f %f %f",&h,&t,&f);
	if(narg != 3){
		beep();
		printf("Need 3 args\n");
		return -1;
	}
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */

	datp2 = malloc(size);
	if(datp2 == 0) {
		nomemory();
		return -1;
	}
	datp = datp2+doffset; 				// set image to 0
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			*(datp++) = 0;
		}
	}
	
	datp = datp2+doffset;

	theta = t;
	y_start = h;
	dy = 0.0;
	if(h < 0.0){
		dy = -h;
		//y_start = .01;
		y_start = dy/2.0+.0001;
		entire_image = 1;
	}
	y_end = header[NTRAK]-1;
	factor = f;
	printf("%f %f %f\n",y_start,theta,factor);
	
	do {
		
		// start at left hand side of image
		ix = 1;
		x = ix;		// ray location	
		y = y_start;
		iy = y;	
		theta = t;	
		
		// propogate across the entire image
		while(ix < header[NCHAN]-1 && iy >= 0 && iy < header[NTRAK]-1){
			// apply snells law at each pixel
			// get the gradient
			nx = idat(iy,ix-1) - idat(iy,ix+1);
			ny = idat(iy-1,ix) - idat(iy+1,ix);
			
			if (ny == 0.0) 
				int_ang = PI/2.0;
			else
				int_ang = atan(nx/ny);
			
			int_ang = PI/2.0 - int_ang;
			
			rindex2 = 1.0+idat(iy,ix)/factor;
			rindex1 = 1.0+idat(iy,ix)/factor + cos(theta)*nx/2.0/factor + sin(theta)*ny/2.0/factor;	
			//rindex2 = idat(iy,ix)/factor;
			//rindex1 = idat(iy,ix)/factor + cos(theta)*nx/2.0/factor + sin(theta)*ny/2.0/factor;	
			
			//printf("%f %f\n",rindex1, rindex2);	
			
					
						if( theta <= int_ang ){
							theta1 = int_ang-theta;
							if(theta1 > PI/2.0) {
								theta1 = PI - theta1;
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								theta = theta2 + int_ang - PI;
							}else {
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								theta =  (int_ang - theta2);
							}
						} else {
							theta1 = theta-int_ang;
							if(theta1 > PI/2.0) {
								theta1 = PI - theta1;
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								//theta = PI - (theta2 -int_ang);
								//theta = theta2 + int_ang - PI;
								theta = theta2 -int_ang;
								beep();
							}else {
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								theta = theta2+int_ang;
							}	
						}
			
		
			//*(datp + ix + iy*header[NCHAN]) = int_ang*180.0/PI;	// fill in this pixel
			//*(datp + ix + iy*header[NCHAN]) += 200*dx;	// fill in this pixel
			ixo = ix;
			iyo = iy;
			if(x == (float)ix){		// start from LHS of pixel
				theta_max = atan( (float)(iy+1) - y );
				theta_min = atan( (float)(iy) - y );
				if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
					ix++;
					dx = 1.0;
					x = ix;
					dyp = tan(theta);
					y = y + dyp;
				} else if(theta > theta_max) {			// this will go to pixel above
					dx = ((float)(iy+1) - y)/tan(theta);
					x = x + dx;
					iy++;
					dyp=iy-y;
					y = iy;
				} else {								// this goes to pixel below
					dx = ((float)(iy) - y)/tan(theta);
					x = x + dx;
					dyp=iy-y;
					iy--;
					y = iy+1;
				}	
			} else {									// start from top or bottom of pixel
				theta_max = atan( 1.0/(1.0 - x + (float)ix));
				theta_min = -theta_max;
				if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
					dyp = tan(theta)*(1.0 - x + (float)ix);
					y = y + dyp;
					ix++;
					dx = ix - x;
					x = ix;
				} else if(theta > 0.0) {				// this will go to pixel above
					dx = 1.0/tan(theta);
					x = x + dx;
					iy++;
					dyp=iy-y;
					y = iy;			
				} else {								// this goes to pixel below
					dx = - 1.0/tan(theta);
					x = x + dx;
					dyp=iy-y;
					iy--;
					y = iy+1;
				}
			}
			//if(dx<0.0)beep();
			*(datp + ixo + iyo*header[NCHAN]) += 200*sqrt(dx*dx + dyp*dyp);	// fill in this pixel proportionally
			// *(datp + ixo + iyo*header[NCHAN]) += 200*dx;	// fill in this pixel proportionally
		}
	y_start += dy;
	} while (entire_image == 1 && y_start <	y_end);
	
	free(datpt);
	datpt = datp2;
	
	have_max = 0;
    maxx();
	return 0;
	
}
/* --------------------------- */

int tracor(int n, int index)		// ray trace across the image
									// continuous image interpreted as local index of refraction
									// index = 1.0 + signal/scale_factor
									// TRACOR dy angle scale_factor
									// dy is an increment and trace entire image
									//		starting at the first row of pixels
									// angle is the initial propogation direction (in radians, 0 is horizontal)
									
{
	int ix,iy=0,nt,nc,ixo,iyo,numrays,raynum,i,j,smooth;
	int narg,size;
	double x,y,rindex1,rindex2,theta,theta_min,theta_max,dy,y_start,y_end;
	double theta1,theta2,int_ang,nx,ny,a,factor,dx=1.0,dyp=0.0,xgoal=0;
	double* yval;
	double old_next_col_pix, sheet_intensity, newpix, intensity=5000,sigy=2/1.75;
	double sum,norm=0.;
	double* angle;
	float t,h,f;
	DATAWORD *datp,idat(int,int),*datp2;
	
	
	narg = sscanf(&cmnd[index],"%f %f %f %d",&h,&t,&f,&smooth);
	if(narg != 4){
		beep();
		printf("Need 4 args\n");
		return -1;
	}
	
	if (smooth/2 == smooth/2.0) {
		printf("The smoothing parameter must be odd. May I suggest 5?\n");
		beep();
		return -1;
	}

	smooth = (smooth-1)/2;
	sigy = smooth/1.75;
	
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */

	datp2 = (DATAWORD*)malloc(size);
	if(datp2 == 0) {
		nomemory();
		return -1;
	}
	datp = datp2+doffset; 				// set image to 0
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			*(datp++) = 0;
		}
	}
	
	datp = datp2+doffset;

	theta = t;
	y_start = h;
	if(h < 0.0) h = -h;
	dy = h;
	//y_start = .01;
	intensity = 200.0/dy;
	
	y_start = dy/2.0+.0001;

	y_end = header[NTRAK]-1;
	numrays= (y_end - y_start)/dy;
	
	factor = f;
	printf("%f %f %f\n",y_start,theta,factor);
	
	yval = (double*)  malloc(numrays*sizeof(double));
	if(yval == 0) {
		nomemory();
		return -1;
	}
	angle = (double*)  malloc(numrays*sizeof(double));
	if(angle == 0) {
		nomemory();
		return -1;
	}
	for(i=0; i<numrays;i++){
		*(angle+i) = theta;
		*(yval+i) = y_start +i*dy;
	}
	for(j=-smooth; j<=smooth; j++) {
		norm += exp(-j*j/(sigy*sigy)/2.);
	}

	ix = 0;
	for (ix = 1; ix < header[NCHAN]-1; ix++){	//FINISH WHEN RHS OF IMAGE IS REACHED
		
		// start at left hand side of image
		ixo = ix;
		x = ix;		// ray location	
		xgoal = x+1.0;
		for(raynum = 0;	raynum<numrays; raynum++){
			y = *(yval+raynum);
			iy = y;	
			theta = *(angle+raynum);
			x = ix;		
			// propogate across a single pixel
			
			while(x < xgoal && iy >= 0 && iy < header[NTRAK]-1){
				// apply snells law at each pixel
				// get the gradient
				nx = idat(iy,ix-1) - idat(iy,ix+1);
				ny = idat(iy-1,ix) - idat(iy+1,ix);
				
				if (ny == 0.0) 
					int_ang = PI/2.0;
				else
					int_ang = atan(nx/ny);
				
				int_ang = PI/2.0 - int_ang;
				
				rindex2 = 1.0+idat(iy,ix)/factor;
				rindex1 = 1.0+idat(iy,ix)/factor + cos(theta)*nx/2.0/factor + sin(theta)*ny/2.0/factor;	
				//rindex2 = idat(iy,ix)/factor;
				//rindex1 = idat(iy,ix)/factor + cos(theta)*nx/2.0/factor + sin(theta)*ny/2.0/factor;	
				
				//printf("%f %f\n",rindex1, rindex2);	
				
						
							if( theta <= int_ang ){
								theta1 = int_ang-theta;
								if(theta1 > PI/2.0) {
									theta1 = PI - theta1;
									a = rindex1/rindex2*sin(theta1);
									if(a > 1.0) a = 1.0;
									if(a< -1.0) a = -1.0;
									theta2 = asin(a);
									theta = theta2 + int_ang - PI;
								}else {
									a = rindex1/rindex2*sin(theta1);
									if(a > 1.0) a = 1.0;
									if(a< -1.0) a = -1.0;
									theta2 = asin(a);
									theta =  (int_ang - theta2);
								}
							} else {
								theta1 = theta-int_ang;
								if(theta1 > PI/2.0) {
									theta1 = PI - theta1;
									a = rindex1/rindex2*sin(theta1);
									if(a > 1.0) a = 1.0;
									if(a< -1.0) a = -1.0;
									theta2 = asin(a);
									//theta = PI - (theta2 -int_ang);
									//theta = theta2 + int_ang - PI;
									theta = theta2 -int_ang;
									beep();
								}else {
									a = rindex1/rindex2*sin(theta1);
									if(a > 1.0) a = 1.0;
									if(a< -1.0) a = -1.0;
									theta2 = asin(a);
									theta = theta2+int_ang;
								}	
							}
				
			
				//*(datp + ix + iy*header[NCHAN]) = int_ang*180.0/PI;	// fill in this pixel
				//*(datp + ix + iy*header[NCHAN]) += 200*dx;	// fill in this pixel
				ixo = ix;
				iyo = iy;
				if(x == (float)ix){		// start from LHS of pixel
					theta_max = atan( (float)(iy+1) - y );
					theta_min = atan( (float)(iy) - y );
					if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
						//ix++;
						dx = 1.0;
						x += dx;
						dyp = tan(theta);
						y = y + dyp;
					} else if(theta > theta_max) {			// this will go to pixel above
						dx = ((float)(iy+1) - y)/tan(theta);
						x = x + dx;
						iy++;
						dyp=iy-y;
						y = iy;
					} else {								// this goes to pixel below
						dx = ((float)(iy) - y)/tan(theta);
						x = x + dx;
						dyp=iy-y;
						iy--;
						y = iy+1;
					}	
				} else {									// start from top or bottom of pixel
					theta_max = atan( 1.0/(1.0 - x + (float)ix));
					theta_min = -theta_max;
					if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
						dyp = tan(theta)*(1.0 - x + (float)ix);
						y = y + dyp;
						//ix++;
						dx = ix+1 - x;
						x = ix+1;
					} else if(theta > 0.0) {				// this will go to pixel above
						dx = 1.0/tan(theta);
						x = x + dx;
						iy++;
						dyp=iy-y;
						y = iy;			
					} else {								// this goes to pixel below
						dx = - 1.0/tan(theta);
						x = x + dx;
						dyp=iy-y;
						iy--;
						y = iy+1;
					}
				}
				//if(dx<0.0)beep();
				*(datp + ix + iy*header[NCHAN]) += 200*sqrt(dx*dx + dyp*dyp);	// fill in this pixel proportionally
				// *(datp + ixo + iyo*header[NCHAN]) += 200*dx;	// fill in this pixel proportionally
				
				*(angle+raynum) = theta;	// save the location of this ray for next time
				if(y < 0.0) y = 0.0;
				if(y > header[NTRAK]-1.0) y= header[NTRAK]-1.0;
				*(yval+raynum) = y;

			}	//end of 1 ray crossing a pixel
		}	// end of loop on all rays
		// now do corrections one column ahead
		
		// first, smooth this column 1x5 using Gaussian smoothing
		
		for(i=smooth; i<header[NTRAK]-smooth; i++){
			sum = 0;
			for(j=-smooth; j<=smooth; j++) {
				sum += *(datp2+doffset+(i+j)*header[NCHAN]+ix)*exp(-j*j/(sigy*sigy)/2.);
			}

			*(datp2+doffset+i*header[NCHAN]+ix) = sum/norm+0.5;			
		}
		
		for(i=1; i<header[NTRAK]-1; i++){		
			if((ix+2)< header[NCHAN]){
				old_next_col_pix = *(datpt+doffset+i*header[NCHAN]+ix+2);	
				sheet_intensity = *(datp2+doffset+i*header[NCHAN]+ix);
				newpix= intensity * old_next_col_pix/sheet_intensity;
				*(datpt+doffset+i*header[NCHAN]+ix+2) = newpix;
				/*
				old_next_col_pix = *(datpt+doffset+i*header[NCHAN]+ix);	
				sheet_intensity = *(datp2+doffset+i*header[NCHAN]+ix);
				newpix= intensity * old_next_col_pix/sheet_intensity;
				*(datpt+doffset+i*header[NCHAN]+ix) = newpix;
				*/
			}
		}
		
	} 
	
	free(datpt);
	free(angle);
	free(yval);
	datpt = datp2;
	
	have_max = 0;
    maxx();
	return 0;
	
}
/* --------------------------- */


int trpdf(int n, int index)			// ray trace across the image
									// continuous image interpreted as local index of refraction
									// index = 1.0 + signal/scale_factor
									// TRPDF pix_inc angle scale_factor pix_offset num_bins bin_width
									// pix_inc an increment between successive traces
									// pix_offset is how far away to stay from the top and bottom of the image
									// angle is the initial propogation direction (in radians, 0 is horizontal)
									// num_bins is the number of bins in the returned histogram
									// bin_width is the bin width
									
{
	int ix,iy,nc,ixo,iyo,num_bins,pdf_index;
	int narg,size,entire_image=0;
	double x,y,rindex1,rindex2,theta,theta_min,theta_max,dy,y_start,y_end,delta_y;
	double theta1,theta2,int_ang,nx,ny,a,factor,dx=1.0,bin_width,pix_offset;
	float t,h,f,bw,po;
	DATAWORD *datp,idat(int,int),*datp2;
		
	narg = sscanf(&cmnd[index],"%f %f %f %f %d %f",&h,&t,&f,&po,&num_bins,&bw);
	if(narg != 6){
		beep();
		printf("Need 6 args: pix_inc angle scale_factor pix_offset num_bins bin_width\n");
		return -1;
	}
	theta = t;
	if(!(num_bins & 1))num_bins++;	//make sure number of bins is odd so things will be symmetric

	dy = h;
	bin_width = bw;
	pix_offset = po;
	//y_start = .01;
	y_start = dy/2.0+pix_offset;
	entire_image = 1;

	y_end = header[NTRAK]-1-pix_offset;
	factor = f;
	printf("%f %f %f %d %f\n",y_start,theta,factor,num_bins,bin_width);
	printf("%d %f\n",num_bins,bin_width);
	
	
	size = (num_bins + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */

	datp2 = malloc(size);
	if(datp2 == 0) {
		nomemory();
		return -1;
	}
	datp = datp2+doffset; 				// set image to 0
	for(nc=0; nc < num_bins; nc++){
		*(datp++) = 0;
	}
	datp = datp2+doffset;

	do {
		
		// start at left hand side of image
		ix = 1;
		x = ix;		// ray location	
		y = y_start;
		iy = y;	
		theta = t;	
		
		// propogate across the entire image
		while(ix < header[NCHAN]-1 && iy >= 0 && iy < header[NTRAK]-1){
			// apply snells law at each pixel
			// get the gradient
			nx = idat(iy,ix-1) - idat(iy,ix+1);
			ny = idat(iy-1,ix) - idat(iy+1,ix);
			
			if (ny == 0.0) 
				int_ang = PI/2.0;
			else
				int_ang = atan(nx/ny);
			
			int_ang = PI/2.0 - int_ang;
			
			rindex2 = 1.0+idat(iy,ix)/factor;
			rindex1 = 1.0+idat(iy,ix)/factor + cos(theta)*nx/2.0/factor + sin(theta)*ny/2.0/factor;	
			//rindex2 = idat(iy,ix)/factor;
			//rindex1 = idat(iy,ix)/factor + cos(theta)*nx/2.0/factor + sin(theta)*ny/2.0/factor;	
			
			//printf("%f %f\n",rindex1, rindex2);	
			
					
						if( theta <= int_ang ){
							theta1 = int_ang-theta;
							if(theta1 > PI/2.0) {
								theta1 = PI - theta1;
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								theta = theta2 + int_ang - PI;
							}else {
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								theta =  (int_ang - theta2);
							}
						} else {
							theta1 = theta-int_ang;
							if(theta1 > PI/2.0) {
								theta1 = PI - theta1;
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								//theta = PI - (theta2 -int_ang);
								//theta = theta2 + int_ang - PI;
								theta = theta2 -int_ang;
								beep();
							}else {
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								theta = theta2+int_ang;
							}	
						}
			
		
			//*(datp + ix + iy*header[NCHAN]) = int_ang*180.0/PI;	// fill in this pixel
			//*(datp + ix + iy*header[NCHAN]) += 200*dx;	// fill in this pixel
			ixo = ix;
			iyo = iy;
			if(x == (float)ix){		// start from LHS of pixel
				theta_max = atan( (float)(iy+1) - y );
				theta_min = atan( (float)(iy) - y );
				if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
					ix++;
					dx = 1.0;
					x = ix;
					y = y + tan(theta);
				} else if(theta > theta_max) {			// this will go to pixel above
					dx = ((float)(iy+1) - y)/tan(theta);
					x = x + dx;
					iy++;
					y = iy;
				} else {								// this goes to pixel below
					dx = ((float)(iy) - y)/tan(theta);
					x = x + dx;
					iy--;
					y = iy+1;
				}	
			} else {									// start from top or bottom of pixel
				theta_max = atan( 1.0/(1.0 - x + (float)ix));
				theta_min = -theta_max;
				if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
					y = y + tan(theta)*(1.0 - x + (float)ix);
					ix++;
					dx = ix - x;
					x = ix;
				} else if(theta > 0.0) {				// this will go to pixel above
					dx = 1.0/tan(theta);
					x = x + dx;
					iy++;
					y = iy;			
				} else {								// this goes to pixel below
					dx = - 1.0/tan(theta);
					x = x + dx;
					iy--;
					y = iy+1;
				}
			}
			//*(datp + ixo + iyo*header[NCHAN]) += 200*dx;	// fill in this pixel proportionally
		}
	delta_y = y_start-y+(header[NCHAN]-2)*atan(t);
	
	
	if(delta_y >= 0.0){
		pdf_index = 1+(delta_y - bin_width/2)/bin_width;
		pdf_index += num_bins/2;
		if(pdf_index >= num_bins) pdf_index=num_bins-1;
	} else {
		delta_y = -delta_y;
		pdf_index = 1+(delta_y - bin_width/2)/bin_width;
		pdf_index = num_bins/2 - pdf_index;
		if(pdf_index < 0) pdf_index=0;
	}
	*(datp + pdf_index) += 1;
	//printf("%f %d\n",delta_y,pdf_index);
	y_start += dy;
	} while (entire_image == 1 && y_start <	y_end);
	
	free(datpt);
	datpt = datp2;
	
	header[NTRAK]=1;
	header[NCHAN] = num_bins;
	header[NX0] =  header[NY0] = 0;
	header[NDX] =  header[NDY] = 1;
	trailer[SFACTR] = 1;
	npts = num_bins;
	
	have_max = 0;
    maxx();
	printparams();
	return 0;
	
}
/* --------------------------- */

int tracpt(int n, int index)		// ray trace from a point in the center of the image
									// continuous image interpreted as local index of refraction
									// index = 1.0 + signal/scale_factor
									// TRACPT 1/f# dtheta scale_factor									
{
	int ix,iy,nt,nc,ixo,iyo;
	int narg,size;
	int focusing=0;
	double x,y,rindex1,rindex2,theta,theta_min,theta_max,theta_mx,theta0,y_start,dtheta;
	double theta1,theta2,int_ang,nx,ny,a,factor,dx=1.0;
	float fn,f,d;
	DATAWORD *datp,idat(int,int),*datp2;
	
	
	narg = sscanf(&cmnd[index],"%f %f %f",&fn,&d,&f);
	if(narg != 3){
		beep();
		printf("Need 3 args\n");
		return -1;
	}
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */
	
	datp2 = malloc(size);
	if(datp2 == 0) {
		nomemory();
		return -1;
	}
	datp = datp2+doffset; 				// set image to 0
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			*(datp++) = 0;
		}
	}
	
	datp = datp2+doffset;

	dtheta = d;
	factor = f;
	//printf("%f %f %f\n",y_start,dtheta,factor);
	theta_mx = atan(fn);
	y_start = header[NTRAK]/2+0.5001;
	for(theta0 = -theta_mx; theta0 < theta_mx; theta0 += dtheta){
		
		// start at left hand side of image
		ix = 1;
		focusing = 0;
		x = ix;		// ray location	
		y = y_start;
		iy = y;	
		theta = theta0;
		// loop on different angles
		while(ix < header[NCHAN]-1 && iy > 1 && iy < header[NTRAK]-1){
			// need a one statement perfect lens when ix is half way across the image
			if( ix == header[NCHAN]/2 - 30 && focusing == 0){
				theta = -theta;
				focusing = 1;
			}
			// apply snells law at each pixel
			// get the gradient
			nx = idat(iy,ix-1) - idat(iy,ix+1);
			ny = idat(iy-1,ix) - idat(iy+1,ix);
			
			if (ny == 0.0) 
				int_ang = PI/2.0;
			else
				int_ang = atan(nx/ny);
			
			int_ang = PI/2.0 - int_ang;
			
			rindex2 = 1.0+idat(iy,ix)/factor;
			rindex1 = 1.0+idat(iy,ix)/factor + cos(theta)*nx/2.0/factor + sin(theta)*ny/2.0/factor;	
			//rindex2 = idat(iy,ix)/factor;
			//rindex1 = idat(iy,ix)/factor + cos(theta)*nx/2.0/factor + sin(theta)*ny/2.0/factor;	
			
			//printf("%f %f\n",rindex1, rindex2);	
			
					
						if( theta <= int_ang ){
							theta1 = int_ang-theta;
							if(theta1 > PI/2.0) {
								theta1 = PI - theta1;
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								theta = theta2 + int_ang - PI;
							}else {
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								theta =  (int_ang - theta2);
							}
						} else {
							theta1 = theta-int_ang;
							if(theta1 > PI/2.0) {
								theta1 = PI - theta1;
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								//theta = PI - (theta2 -int_ang);
								//theta = theta2 + int_ang - PI;
								theta = theta2 -int_ang;
								beep();
							}else {
								a = rindex1/rindex2*sin(theta1);
								if(a > 1.0) a = 1.0;
								if(a< -1.0) a = -1.0;
								theta2 = asin(a);
								theta = theta2+int_ang;
							}	
						}
			
		
			//*(datp + ix + iy*header[NCHAN]) = int_ang*180.0/PI;	// fill in this pixel
			//*(datp + ix + iy*header[NCHAN]) += 1;	// fill in this pixel
			ixo = ix;
			iyo = iy;			
			if(x == (float)ix){		// start from LHS of pixel
				theta_max = atan( (float)(iy+1) - y );
				theta_min = atan( (float)(iy) - y );
				if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
					ix++;
					dx = 1.0;
					x = ix;
					y = y + tan(theta);
				} else if(theta > theta_max) {			// this will go to pixel above
					dx = ((float)(iy+1) - y)/tan(theta);
					x = x + dx;
					iy++;
					y = iy;
				} else {								// this goes to pixel below
					dx = ((float)(iy) - y)/tan(theta);
					x = x + dx;
					iy--;
					y = iy+1;
				}	
			} else {									// start from top or bottom of pixel
				theta_max = atan( 1.0/(1.0 - x + (float)ix));
				theta_min = -theta_max;
				if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
					y = y + tan(theta)*(1.0 - x + (float)ix);
					ix++;
					dx = ix - x;
					x = ix;
				} else if(theta > 0.0) {				// this will go to pixel above
					dx = 1.0/tan(theta);
					x = x + dx;
					iy++;
					y = iy;			
				} else {								// this goes to pixel below
					dx = - 1.0/tan(theta);
					x = x + dx;
					iy--;
					y = iy+1;
				}
			}
			*(datp + ixo + iyo*header[NCHAN]) += 1*dx+0.5;	// fill in this pixel proportionally
		}
	}
	
	free(datpt);
	datpt = datp2;
	
	have_max = 0;
    maxx();
	return 0;
	
}
/* --------------------------- */

int angrad(int n, int index)			// replace the image with an image that represents the angle of the
										// gradient at each point
{
	int ix,iy,nt,nc;
	int size;
	
	double int_ang,nx,ny;

	DATAWORD *datp,idat(int,int),*datp2;
	
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */

	datp2 = malloc(size);
	if(datp2 == 0) {
		nomemory();
		return -1;
	}
	datp = datp2+doffset; 				// set image to 0
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			
			iy = nt;
			ix = nc;
			nx = idat(iy,ix-1) - idat(iy,ix+1);
			ny = idat(iy-1,ix) - idat(iy+1,ix);
		
			if (ny == 0.0) 
				int_ang = PI/2.0;
			else
				int_ang = atan(nx/ny);
				
				
			int_ang = PI/2.0 - int_ang;
		
			
			
			if(ny <= 0 ){
				int_ang += PI;
				if (ny == 0 && nx > 0) int_ang -= PI;
			}
			
		
			*(datp++) = int_ang*180.0/PI;
		}
	}
	
	datp = datp2+doffset;
	trailer[SFACTR]=1;
	
	
	free(datpt);
	datpt = datp2;
	
	have_max = 0;
    maxx();
	
	return 0;
	
}
/* --------------------------- */

int stream(int n, int index)			// calculate a streamline across an image
                                                // STREAM x0 y0 scale_factor
                                                // x0 y0 is the starting location of the ray
                                                // the two components of the velocity field are needed
                                                // the y velocity is stored with SBACK
                                                // the x velocity field is in the current image buffer
                                                // it is assumed that the major velocity component is in the x direction
									
{
	int ix,iy,nt,nc,count = 0;
	int narg,size;
	double x,y,distance,theta,theta_min,theta_max,vx,vy;
	double dx,x0,y0,time,factor,v;
	float sx,sy,f;
	DATAWORD *datp,idat(int,int),*datp2,*datp3;
        
        extern DATAWORD *backdat;		/* the data pointer for backgrounds */
        extern unsigned int backsize;

	
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	/* make a bit bigger for file reads */

	datp2 = malloc(size);
	if(datp2 == 0) {
		nomemory();
		return -1;
	}
	datp = datp2+doffset; 				// set image to -1
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			*(datp++) = 0;
		}
	}
	
	datp = datp2+doffset;
        datp3 = backdat + doffset;
	
	narg = sscanf(&cmnd[index],"%f %f %f",&sx,&sy,&f);
	if(narg != 3){
		beep();
		printf("Need 3 args\n");
		return -1;
	}
	x = sx;
	y = sy;
	factor = f;
        distance = 0.0;
        time = 0.0;
	printf("%f %f %f\n",x,y,factor);
        if(backdat == 0) {
            beep();
            printf("Must load second image using SBACK first.\n");
            return -1;
        }
        if( (header[NCHAN]*header[NTRAK]+MAXDOFFSET)*DATABYTES != backsize) {
            beep();
            printf("Image Sizes Conflict.\n");
            return -2;
        }

	
	// start at specified pixel
	ix = x;
	iy = y;		
	
	// propogate across the entire image
	while(ix < header[NCHAN]-1 && iy > 1 && iy < header[NTRAK]-1&& count++ < header[NCHAN]*3){
            x0 = x;
            y0 = y;
            vx = idat(iy,ix);
            vy = *(datp3 + ix + iy*header[NCHAN]);	/* y velocity */
            v = sqrt(vx*vx+vy*vy);
            theta = atan(vy/vx);
            
            *(datp + ix + iy*header[NCHAN]) = time;	// fill in this pixel
            
            if(x == (float)ix){		// start from LHS of pixel
                    theta_max = atan( (float)(iy+1) - y );
                    theta_min = atan( (float)(iy) - y );
                    if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
                            ix++;
                            x = ix;
                            y = y + tan(theta);
                    } else if(theta > theta_max) {			// this will go to pixel above
                            x = x + ((float)(iy+1) - y)/tan(theta);
                            iy++;
                            y = iy;
                    } else {								// this goes to pixel below
                            x = x + ((float)(iy) - y)/tan(theta);
                            iy--;
                            y = iy+1;
                    }	
            } else {									// start from top or bottom of pixel
                    theta_max = atan( 1.0/(1.0 - x + (float)ix));
                    theta_min = -theta_max;
                    if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
                            y = y + tan(theta)*(1.0 - x + (float)ix);
                            ix++;
                            x = ix;
                    } else if(theta > 0.0) {				// this will go to pixel above
                            x = x + 1.0/tan(theta);
                            iy++;
                            y = iy;			
                    } else {								// this goes to pixel below
                            x = x - 1.0/tan(theta);
                            iy--;
                            y = iy+1;
                    }
            }
            dx = sqrt( (x-x0)*(x-x0) + (y-y0)*(y-y0) );
            distance += dx;
            time += dx/v*factor;
	}
	
	
	free(datpt);
	datpt = datp2;
	
	have_max = 0;
        maxx();
        printf(" Distance and Time:\t%f\t%f\n", distance,time);
	return 0;
	
}
/* --------------------------- */
/* --------------------------- */

int strtim(int n, int index)			// calculate an image made up of elapsed time
                                                // along streamlines
                                                // STRTIM x0 dy scale_factor
                                                // x0 is the starting x point for all rays
                                                // dy is increment between rays
                                                // the two components of the velocity field are needed
                                                // the y velocity is stored with SBACK
                                                // the x velocity field is in the current image buffer
                                                // it is assumed that the major velocity component is in the x direction									
{
	int ix,iy,nt,nc,count = 0;
	int narg,size;
	double x,y=0,distance=0,theta,theta_min,theta_max,vx,vy;
	double dx,x0,y0,time=0,factor,v;
        double dy,ymax,loopy;
	float sx,sy,f;
	DATAWORD *datp,idat(int,int),*datp2,*datp3,itime;
        
        extern DATAWORD *backdat;		// the data pointer for backgrounds 
        extern unsigned int backsize;

	
	size = (header[NCHAN] * header[NTRAK] + MAXDOFFSET) * DATABYTES;
	size = (size+511)/512*512;	// make a bit bigger for file reads 

	datp2 = malloc(size);
	if(datp2 == 0) {
		nomemory();
		return -1;
	}
	datp = datp2+doffset; 				// set image to -1
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			*(datp++) = -1;
		}
	}
	
	datp = datp2+doffset;
        datp3 = backdat + doffset;
	
	narg = sscanf(&cmnd[index],"%f %f %f",&sx,&sy,&f);
	if(narg != 3){
		beep();
		printf("Need 3 args: x0 dy scale_factor\n");
		return -1;
	}
	x = sx;
	dy = sy;
	factor = f;
	printf("%f %f %f\n",x,y,factor);
        if(backdat == 0) {
            beep();
            printf("Must load second image using SBACK first.\n");
            return -1;
        }
        if( (header[NCHAN]*header[NTRAK]+MAXDOFFSET)*DATABYTES != backsize) {
            beep();
            printf("Image Sizes Conflict.\n");
            return -2;
        }
        ymax = header[NTRAK]-1;
	for(loopy = 0.1; loopy< ymax; loopy+= dy){
            // start at specified pixel
            x = sx;
            y = loopy;
            ix = x;
            iy = y;		
            distance = 0.0;
            time = 0.0;
            count = 0;
            //printf("%d %d\n",ix,iy);
            // propogate across the entire image
            while(ix < header[NCHAN]-1 && iy > 1 && iy < header[NTRAK]-1&& count++ < header[NCHAN]*3){
                x0 = x;
                y0 = y;
                vx = idat(iy,ix);
                vy = *(datp3 + ix + iy*header[NCHAN]);	/* y velocity */
                v = sqrt(vx*vx+vy*vy);
                theta = atan(vy/vx);
                
                itime = time;
                if(*(datp + ix + iy*header[NCHAN]) == -1 || itime < *(datp + ix + iy*header[NCHAN]) ){  // remember the minimum time
                    *(datp + ix + iy*header[NCHAN]) = itime;	// fill in this pixel
                }
                
                if(x == (float)ix){		// start from LHS of pixel
                        theta_max = atan( (float)(iy+1) - y );
                        theta_min = atan( (float)(iy) - y );
                        if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
                                ix++;
                                x = ix;
                                y = y + tan(theta);
                        } else if(theta > theta_max) {			// this will go to pixel above
                                x = x + ((float)(iy+1) - y)/tan(theta);
                                iy++;
                                y = iy;
                        } else {								// this goes to pixel below
                                x = x + ((float)(iy) - y)/tan(theta);
                                iy--;
                                y = iy+1;
                        }	
                } else {									// start from top or bottom of pixel
                        theta_max = atan( 1.0/(1.0 - x + (float)ix));
                        theta_min = -theta_max;
                        if( theta <= theta_max && theta >= theta_min){	// this will make it to the RHS of the pixel
                                y = y + tan(theta)*(1.0 - x + (float)ix);
                                ix++;
                                x = ix;
                        } else if(theta > 0.0) {				// this will go to pixel above
                                x = x + 1.0/tan(theta);
                                iy++;
                                y = iy;			
                        } else {								// this goes to pixel below
                                x = x - 1.0/tan(theta);
                                iy--;
                                y = iy+1;
                        }
                }
                dx = sqrt( (x-x0)*(x-x0) + (y-y0)*(y-y0) );
                distance += dx;
                time += dx/v*factor;
            }
	}
	
	free(datpt);
	datpt = datp2;
	
	have_max = 0;
        maxx();
        printf(" Distance and Time:\t%f\t%f\n", distance,time);
	return 0;
	
}
/* --------------------------- */

//***************************************************************//
//**    ROUNDUP - Round the DATAWORD values UP to the nearest integer value   *//
//***************************************************************//
int
round_up ()
{	
	int nc,nt;
	Image Im_p;
	
	if(SDK_Image_Create(&Im_p,header[NCHAN],header[NTRAK])!=OMA_OK){
		beep();
		printf("SDK_Image_Create returned an error\n");
		return OMA_MEMORY;
	}
	
	for(nt=0; nt<header[NTRAK]; nt++) {
		for (nc =0; nc<header[NCHAN]; nc++){
			SDK_Image_SetPixel(&Im_p, nc, nt, ceil(PKGetPixel(nc,nt)));
		}
	}
	SDK_Image_2Buffer(&Im_p);
	SDK_Image_Free(&Im_p);
	SDK_Image_Refresh();
	return OMA_OK;
}

//***************************************************************//
//**    ROUNDOFF - Round the DATAWORD values Down to the nearest integer value   *//
//***************************************************************//
int
roundoff ()	
{	
	int nc,nt;
	Image Im_p;
	
	if(SDK_Image_Create(&Im_p,header[NCHAN],header[NTRAK])!=OMA_OK){
		beep();
		printf("SDK_Image_Create returned an error\n");
		return OMA_MEMORY;
	}
	
	for(nt=0; nt<header[NTRAK]; nt++) {
		for (nc =0; nc<header[NCHAN]; nc++){
			SDK_Image_SetPixel(&Im_p, nc, nt, floor(PKGetPixel(nc,nt)));
		}
	}
	SDK_Image_2Buffer(&Im_p);
	SDK_Image_Free(&Im_p);
	SDK_Image_Refresh();
	return OMA_OK;
}

#define LINELEN 1024


//**********************************************************************//
//*** PARSE <filename> - Reads in a TECPLOT vector file from         *//
//***                                 PIVView and converts to a OMA PIV field      *//
//***                                     P. Kalt (2006)                                          *//
//**********************************************************************//
int parse(int n,int index)
{
	
	float  xpos, ypos, ucomp, vcomp;
	int flag;
	int nc, nt;
	FILE         *fp;
	Field        myField;
	Vector     tVect;
	int            FieldSizeX, FieldSizeY;
	short        comment = TRUE;
	
	// Check that we can open the file and then do it....
	fp = fopen(fullname(&cmnd[index],GET_DATA),"r");
	
	if (fp == NULL) {
		beep();
		printf("I can't find the file. I am leaving. Sorry\n");
		return OMA_FILE;
	}
		
	// Skip past the Comments
	while (comment) {
		if (fgets(scratch, LINELEN, fp)==NULL) return OMA_MISC;
		if (strncmp(scratch, "#", 1)==0)  comment = TRUE;
		else                                            comment = FALSE;
	}
	if (fgets(scratch, LINELEN, fp)==NULL) return OMA_MISC;
	
	comment = TRUE;
	while (comment){
		if ( strncmp(scratch,"VARIABLES",strlen("VARIABLES")) ==0) comment = FALSE;
	}
	
	// Read the meaty bits
	fread(scratch,strlen("ZONE I="),1,fp);
	fscanf( fp,"%d",&FieldSizeX);
	fread(scratch,strlen(", J="),1,fp);
	fscanf( fp,"%d",&FieldSizeY);
	fread(scratch,strlen(", F=Point\n"),1,fp);
	
	// Make Assignments for PIV data file
	printf("Vector field is %d by %d\n", FieldSizeX, FieldSizeY);
	
	// Find the File TITLE
	// Find the Variables
	// Find the size of the field (ZONE)
	
	// We know the size....Let's get a Field ready to hold the results...
	if (SDK_Field_Create(&myField, FieldSizeX, FieldSizeY)) return OMA_MEMORY;
	
	// Start to parse the data, line by line
	if (fgets(scratch, LINELEN, fp)==NULL) return OMA_MISC;
	for (nt=0; nt<FieldSizeY; nt++) {
		for (nc=0;nc<FieldSizeX; nc++) {
			//  Read actual data from TecPLot file
			if (fgets(scratch, LINELEN, fp)==NULL) return OMA_MISC;
			sscanf( scratch,"%f %f %f %f %d",&xpos, &ypos, &ucomp, &vcomp, &flag);
			//printf("%f %f\n", ucomp, vcomp);
			SDK_Vector_SetX(&tVect, ucomp);
			SDK_Vector_SetY(&tVect, vcomp);
			SDK_Vector_SetZ(&tVect, (float)0.0);
			SDK_Vector_SetPeak(&tVect, (float) flag);
			SDK_Field_SetVector(&myField, nc, nt, &tVect);
		}
	}
	
	// Skip header information
	// Pack up and go
	fclose(fp);
	SDK_Field_2Buffer(&myField);
	SDK_Field_Free(&myField);
	return OMA_OK;
}


//********************************************************
//*** SAUTER <filename> <Factor> 
//***		 - Do a division of LIF and MIE images using floating
//***                point arithmetic instead of integer arithmetic.
//***    <filename> name of file to act as the denominator
//***    <Factor> scales the resulting image by this amount
//***         P. Kalt (2004)
//********************************************************
int
sauter(int n, int index)
{
	
      char				tfil[128], tempfil[128];
	
	int i,j,k,ibuff;
	TWOBYTE *pointer;	/* point to things in the header */	
	int filesf;		/* Scale factor for file data */
	int ramsf;		/* Scale factor for data in RAM */
	int newsf = 0;		/* Scale factor for result in RAM */
	int mathlen;
	
	int fild;    
	
	int				narg;		// Loop counters...
	int				Factor=1;				// Page number for Multiple Images in 1 TIFF file
	
	
	/* Process the command line arguments */
	for (i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] != EOL) {
			narg = sscanf(&cmnd[index],"%s %d",tfil, &Factor);
			if (narg == 1) Factor = 1;
			break;
		}
	}
	strcpy(tempfil,tfil);
	printf("Scaling factor for division is %d\n",Factor);
	
	/* Open the file and read the header */
	if((fild = open(fullname(tfil,GET_DATA),READMODE)) == -1) {
		beep();
		printf("File Not Found.\n");
		return OMA_FILE;
	}
	
	read(fild,(char*)headcopy,256*2);
	
	pointer = (TWOBYTE*)headcopy;	
	filesf = *(pointer + (HEADLEN + COMLEN)/2 + SFACTR);
	if (filesf == 0) filesf = 1;
	ramsf = trailer[SFACTR];
	printf(" File Scale Factor: %d %d\n",filesf,ramsf);
	
	printf (" %d points expected. %d chans and %d tracks.\n",
			npts,*(pointer+NCHAN),*(pointer+NTRAK));	
	if ( npts != *(pointer + NCHAN) * *(pointer + NTRAK) ) {
		beep();
		printf(" Files are not the same length.\n");
		close(fild);
		return OMA_RANGE;
	}
	
	/* First pass to get scale factor */
	i = k = doffset;
	
	while( i < npts+doffset ) {
		
		mathlen = read(fild,(char*)mathbuf,DATABYTES*MATHLEN);
		mathlen /= DATABYTES;
		
		
				
		for(j=k; j < mathlen; j++){
			if(mathbuf[j] == 0) mathbuf[j] = 1;
			ibuff = *(datpt+i)*ramsf;
			ibuff=( ibuff/mathbuf[j]/filesf)/maxint;
			if (ibuff != 0) {
				/*    	printf("dat,mb,fsf,dsf,ibuff: %d %d %d %d %d\n",*(datpt+i)*ramsf, */
				/*	mathbuf[j],filesf,ramsf,ibuff);					  */
				if (ibuff<0) ibuff= -ibuff;
				if (ibuff>newsf) newsf=ibuff;
			}
			i++;
		}
		k = 0;
	}
	
	newsf++;
	close(fild);
	
	/* Second and final pass */
	if((fild = open(fullname(tempfil,GET_DATA),READMODE)) == -1) {
		beep();
		printf("File Reopen Not Found.\n");
		return OMA_FILE;
	}
	
	read(fild,(char*)headcopy,256*2);	/* Skip past header */
	i = k = doffset;
	
	while( i < npts+doffset ) {
		mathlen = read(fild,(char*)mathbuf,DATABYTES*MATHLEN);
		mathlen /= DATABYTES;
		
		
		for(j=k; j < mathlen; j++){
			if(mathbuf[j] == 0) mathbuf[j] = 1;
			ibuff = (*(datpt+i)*Factor/(mathbuf[j]));
			*(datpt+i)=ibuff;
			i++;
		}
		k = 0;
	}
	
	trailer[SFACTR] = newsf;	/* New scale factor */
	close(fild);
	have_max = 0;
	maxx();
	return OMA_OK;
}


//********************************************************
//*** SHIFT <Factor> 
//***		 - Shift (or morph) the current image by the
//***                current PIV field multiplied by <Factor>.
//***         P. Kalt (2004)
//********************************************************
int
shift(int n,int index) 
{
	int narg;
	int nc,nt;
	float s;
	int Err;
	int width, height;
	int PIVwidth, PIVheight;
	Image myimage;
	Image XSimage;
	
	Vector shift_vec;
	float   new_x, new_y;
	
	// Read in the 'scale factor' from the command line arguments
	narg = sscanf(&cmnd[index],"%f",&s);
	if (narg ==0) s=1.0;
	printf("Scale factor for shift is %f\n", s);
	
	// Create an image to hold the shifted result
	if((Err=SDK_Image_CopyBuffer(&myimage))!=OMA_OK){ 
		beep();
		printf("SDK_Image_CopyBuffer() returned an error\n");
		return OMA_MEMORY; 
	}
	
	// Use the entire original image
	width = header[NCHAN];
	height = header[NTRAK];
	
	// Create an image to hold the shifted result
	if((Err=SDK_Image_Create(&XSimage, width, height))!=OMA_OK){ 
		beep();
		printf("SDK_Image_CopyBuffer() returned an error\n");
		return OMA_MEMORY; 
	}
	
	// Check that there is a valid PIV Field
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}

	// Check the dimensions of the PIV field correspond to the image
	PIVwidth = fftsize + (no_of_velx -1 )*boxinc;
	PIVheight = fftsize + (no_of_vely -1 )*boxinc;
	if ((PIVwidth != width) || (PIVheight != height) ){
		beep();
		printf("Warning %d: PIV Field maps to Image in a strange way\n", OMA_RANGE);
		//return OMA_RANGE;   // Non-fatal (for now)
	}
	
	
	// For each pixel in the image
	for(nt=0; nt<height; nt++) {
		for(nc=0;nc < width; nc++){
			// Firstly, find an interpolated displacement from the PIV field
			shift_vec = PKPixel2Vector(nc,nt);

			// Use this displacement to find a new position in the original image
			new_x = shift_vec.x*s + nc;
			new_y = shift_vec.y*s + nt;

			// Interpolate the value of the pixel at that location
			// Write value into the 'myImage' Image structure
			SDK_Image_SetPixel(&XSimage, nc , nt, (DATAWORD) shift_vec.x);
			SDK_Image_SetPixel(&myimage,nc,nt,PKInterpPixel(new_x, new_y));
			//SDK_Image_SetPixel(&myimage,nc,nt,(DATAWORD) 600);
		}
	}
	
	// Housekeeping and Memory management
	SDK_Image_2Buffer(&myimage);	
	SDK_Image_2Temp(&XSimage, 0);
	SDK_Image_Free(&myimage);	
	SDK_Image_Free(&XSimage);
	SDK_Image_Refresh();
	return OMA_OK;
}
	


//********************************************************
//*** TECPLT <filename>
//***		 - Export the current PIV image in a format 
//***                that can be read by TECPLOT
//********************************************************
int tecplot(int n,int index)
{
	int           ptx,pty;
	FILE         *fp;
	
	// Check for Valid PIV image
	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return OMA_NOEXIST;
	}

	// Open FILE for wrinting values
	strcpy(scratch, fullname(&cmnd[index],GET_DATA));
	fp = fopen(scratch,"w");
	if(fp != NULL) {
		// Write TECPLOT header information
		fprintf(fp,"# Original file name: %s\n", scratch);
		fprintf(fp,"#Software: OMA - Custom command by pkalt\n");
		fprintf(fp,"#GridUnit: Not applicable\n");
		fprintf(fp,"# Velocity Unit: [pixels/delay]\n");
		fprintf(fp,"TITLE = \"OMA Velocity Data\"\n");
		fprintf(fp,"VARIABLES = \"x\", \"y\", \"u\", \"v\"\n");
		fprintf(fp,"ZONE I=%d, J=%d, F=Point\n", no_of_velx, no_of_vely);
		
		// Write PIV information into TECPLOT file
		for(pty = 0; pty < no_of_vely; pty++ ){
			for(ptx = 0; ptx < no_of_velx; ptx++ ){
				fprintf(fp,"%d %d %f %f\n", ptx+1,pty+1,vx(ptx,pty,1),vy(ptx,pty,1));
			}
		}	
		// Pack up and go
		fclose(fp);
	} else {
		beep();
		printf("Unable to create File.\n");
		return -1;		
	}
	return OMA_OK;
}

//********************************************************
//*** KILBOX <Factor>
//***		 - Set Contents of Rectangular sub-region to <factor>
//***         P. Kalt (2004)
//********************************************************
int kill_box(int n,int index)
{
	DATAWORD *mypointer;
	int narg, filval;
	int i, j;
	
	narg = sscanf(&cmnd[index],"%d",&filval);
	if (narg ==0) {
		filval = 0;
		printf("No value entered. Using zero.\n");
	}
	for(i=substart.v; i<= subend.v; i++) {
		mypointer = datpt + doffset + substart.h + i*header[NCHAN];
		for(j=substart.h; j<= subend.h; j++) {
			*mypointer = filval;
			mypointer = mypointer+1;
		}
	}
	SDK_Image_Refresh();
	return OMA_OK;
	
}






  //********************************************************
  //*** G E T B K G
  //***		 - Write the value of the background
  //***		to every pixel in image buffer (so result can
   //***		be used inside a macro). Background is defined as mean + 3 * rms
 //***         P. Kalt (2004)

//*** UPDATED 2006 - We will let this still wreck the buffer for
// compatibility but we will be more clever about passing results 
// out in the command_return_N variables.
//
// command_return_1  Average of non-zero pixels
// command_return_2  Average of ALL pixels
// command_return_3  rms of non-zero pixels
// command_return_4  rms of all pixels
//********************************************************
int
get_background(int n, int index)
{
	
	DATAWORD datval;
	int         nc, nt;
	int	 icount;
	float       rms, ave;
	float       crms, cavg;
	extern Variable user_variables[];	
	// Calculate average of all pixels in the image
	icount = 0;
	ave = 0.0;
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN]; nc++) {
			ave += (float)PKGetPixel(nc,nt);
			icount++;
		}
	}
	ave = ave/(float)icount;
	
	// Calculate average of non-zero pixels in the image
	icount = 0;
	cavg = 0.0;
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN]; nc++) {
			if (PKGetPixel(nc,nt) != 0) {
				cavg += (float)PKGetPixel(nc,nt);
				icount++;
			}
		}
	}
	cavg = cavg/(float)icount;
	
	// Calculate rms of all pixels in the image
	icount = 0;
	rms = 0.0;
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN]; nc++) {
			datval = idat(nt,nc);
				rms += pow((float)PKGetPixel(nc,nt) - ave,2);
				icount++;
		}
	}
	rms = sqrt(rms/(float)(icount-1));

	// Calculate rms of non-zero pixels in the image
	icount = 0;
	crms = 0.0;
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN]; nc++) {
			datval = idat(nt,nc);
			if (PKGetPixel(nc,nt) != 0) {
				crms += pow((float)PKGetPixel(nc,nt) - cavg,2);
				icount++;
			}
		}
	}
	crms = sqrt(crms/(float)(icount-1));
	
	// Third pass to write non-zero average to the data buffer
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			PKSetPixel(nc,nt, (DATAWORD)(cavg + 3* crms));
		}
	}
	SDK_Image_Refresh();

	// return values available as variables
	user_variables[0].fvalue = cavg;
	user_variables[0].is_float = 1;
	user_variables[1].fvalue = crms;
	user_variables[1].is_float = 1;
	user_variables[2].fvalue = ave;
	user_variables[2].is_float = 1;
	user_variables[3].fvalue = rms;
	user_variables[3].is_float = 1;
	
	
	return OMA_OK;	
}

 //********************************************************
 //*** AVERAGE
 //***		 - Write the average value of all non-zero pixels.
 //***               to every pixel in image buffer (so result can
 //***               be used inside a macro).
 //***         P. Kalt (2004)

//*** UPDATED 2006 - We will let this still wreck the buffer for
// compatibility but we will be more clever about passing results 
// out in the command_return_N variables.
//
// command_return_1  Average of non-zero pixels
// command_return_2  Average of ALL pixels
// command_return_3  rms of non-zero pixels
// command_return_4  rms of all pixels
//********************************************************
int
average(int n, int index)
{
	DATAWORD *datp;
	DATAWORD datval;
	int         nc, nt;
	
	int	 icount;
	float       rms, ave;
	float       crms, cavg;
	
	extern Variable user_variables[];	
	// Calculate average of all pixels in the image
	icount = 0;
	ave = 0.0;
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN]; nc++) {
			ave += (float)PKGetPixel(nc,nt);
			icount++;
		}
	}
	ave = ave/(float)icount;
	
	// Calculate average of non-zero pixels in the image
	icount = 0;
	cavg = 0.0;
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN]; nc++) {
			if (PKGetPixel(nc,nt) != 0) {
				cavg += (float)PKGetPixel(nc,nt);
				icount++;
			}
		}
	}
	cavg = cavg/(float)icount;
	
	// Calculate rms of all pixels in the image
	icount = 0;
	rms = 0.0;
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN]; nc++) {
			datval = idat(nt,nc);
			rms += pow((float)PKGetPixel(nc,nt) - ave,2);
			icount++;
		}
	}
	rms = sqrt(rms/(float)(icount-1));
	
	// Calculate rms of non-zero pixels in the image
	icount = 0;
	crms = 0.0;
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN]; nc++) {
			datval = idat(nt,nc);
			if (PKGetPixel(nc,nt) != 0) {
				crms += pow((float)PKGetPixel(nc,nt) - cavg,2);
				icount++;
			}
		}
	}
	crms = sqrt(crms/(float)(icount-1));
	
	/* Second pass to write on-zero average to the data buffer */
	datp = datpt+doffset;
	//printf("rsize = %f\n",rsize);
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			// old tech *(datp++) = (DATAWORD)cavg;
			PKSetPixel(nc,nt, (DATAWORD)(cavg));
		}
	}
	SDK_Image_Refresh();
	
	// return values available as variables
	user_variables[0].fvalue = cavg;
	user_variables[0].is_float = 1;
	user_variables[1].fvalue = crms;
	user_variables[1].is_float = 1;
	user_variables[2].fvalue = ave;
	user_variables[2].is_float = 1;
	user_variables[3].fvalue = rms;
	user_variables[3].is_float = 1;
	
	return OMA_OK;
}


//***************************************************************//
//**    P I V A B S - Absolute value of each PIV component         *//
//***************************************************************//
int
pivabs()	
{	
	int i,j;
	
	for (i=0; i<no_of_velx; i++){
		for(j=0; j<no_of_vely; j++){
			PKSetVectorX (i, j, fabs(PKGetVectorX(i, j)));
			PKSetVectorY (i, j, fabs(PKGetVectorY(i, j)));
		}
	}
	return OMA_OK;
}
//***************************************************************//
//**    P I V X 2 Y - Copies X component to Y component         *//
//***************************************************************//
int
pivx2y()	
{	
	int i,j;
	
	for (i=0; i<no_of_velx; i++){
		for(j=0; j<no_of_vely; j++){
			PKSetVectorY (i, j, PKGetVectorX(i, j));
		}
	}
	return OMA_OK;
}
//***************************************************************//
//**    P I V Y 2 X- Copies Y component to X component         *//
//***************************************************************//
int
pivy2x()	
{	
	int i,j;
	
	for (i=0; i<no_of_velx; i++){
		for(j=0; j<no_of_vely; j++){
			PKSetVectorX (i, j, PKGetVectorY(i, j ));
            }
        }
	return OMA_OK;
}
//***************************************************************//
//**    P I V D O T - Dot product of PIV buffer with PIV in file      *//
//***************************************************************//
int
pivdot(int n, int index)	
{	
	int n_x, n_y;
	float ftemp;
	//Field myfield;
	
	// make sure PIV image is currently loaded
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	
	for( n_y=0; n_y< no_of_vely; n_y++) {
		for(n_x=0; n_x<no_of_velx; n_x++) {
			ftemp = PKGetVectorX(n_x,n_y);
			ftemp *= PKGetVectorY(n_x, n_y);
			PKSetVectorX(n_x, n_y, ftemp);
			PKSetVectorY(n_x, n_y, ftemp);
		}
	}
	return OMA_OK;
}
//***************************************************************//
//**    P I V R O T - Rotate the PIV field clockwise by 90¼           *//
//**               NB: the X& Y components will be swaped              *//
//**               NB: this will work for all peaks (peaks_to_save<6) *//
//***************************************************************//
int
pivrot(int n, int index)	
{	
	Field             PIVpeak1, PIVpeak2, PIVpeak3, PIVpeak4, PIVpeak5;
	Vector          tVect = {0.,0.,0.,0};
	int                 i, nc, nt;
	float              oldX, oldY, oldZ;
	int                 oldPeak;
	
	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}

	// Create new Field structures to handle the rotated image (NB: X&Y dim's swapped)
	if (SDK_Field_Create(&PIVpeak1, no_of_vely, no_of_velx)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak2, no_of_vely, no_of_velx)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak3, no_of_vely, no_of_velx)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak4, no_of_vely, no_of_velx)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak5, no_of_vely, no_of_velx)) return OMA_MEMORY;

	// For each vector ....
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			//... and each peak
			for(i=1;i<=peaks_to_save;i++)  {
				// Get the values from the PIV buffer
				oldX = PKGetNthVectorX(nc,nt,i);
				oldY = PKGetNthVectorY(nc,nt,i);
				oldZ = PKGetNthVectorZ(nc,nt,i);
				oldPeak = PKGetNthPeak(nc,nt,i);
				
				// drop in the approriate value into Vectors (X&Y adjusted)
				SDK_Vector_SetX(&tVect, -oldY);
				SDK_Vector_SetY(&tVect, oldX);
				SDK_Vector_SetZ(&tVect, oldZ);
				SDK_Vector_SetPeak(&tVect, oldPeak);
				
				// Place the Vector into the Field
				if (i==1) SDK_Field_SetVector(&PIVpeak1, no_of_vely-nt-1, nc, &tVect);
				if (i==2) SDK_Field_SetVector(&PIVpeak2, no_of_vely-nt-1, nc, &tVect);
				if (i==3) SDK_Field_SetVector(&PIVpeak3, no_of_vely-nt-1, nc, &tVect);
				if (i==4) SDK_Field_SetVector(&PIVpeak4, no_of_vely-nt-1, nc, &tVect);
				if (i==5) SDK_Field_SetVector(&PIVpeak5, no_of_vely-nt-1, nc, &tVect);				
			} 
		}
	}

	// Copy the Field structures to the OMA PIV buffer
	SDK_Field_2Buffer(&PIVpeak1);
	SDK_Field_2NthPeak(&PIVpeak2, 2);
	SDK_Field_2NthPeak(&PIVpeak3, 3);
	SDK_Field_2NthPeak(&PIVpeak4, 4);
	SDK_Field_2NthPeak(&PIVpeak5, 5);
	
	// Free Field Memory structures
	SDK_Field_Free(&PIVpeak1);
	SDK_Field_Free(&PIVpeak2);
	SDK_Field_Free(&PIVpeak3);
	SDK_Field_Free(&PIVpeak4);
	SDK_Field_Free(&PIVpeak5);
	
	return OMA_OK;
}

//***************************************************************//
//**    P I V F L H - Flip the PIV Horizontally                               *//
//**               NB: this will work for all peaks (peaks_to_save<6) *//
//***************************************************************//
int
pivflh(int n, int index)	
{	
	Field             PIVpeak1, PIVpeak2, PIVpeak3, PIVpeak4, PIVpeak5;
	Vector          tVect = {0.,0.,0.,0};
	int                 i, nc, nt;
	float              oldX, oldY, oldZ;
	int                 oldPeak;
	
	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	
	// Create new Field structures to handle the rotated image
	if (SDK_Field_Create(&PIVpeak1, no_of_velx, no_of_vely)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak2, no_of_velx, no_of_vely)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak3, no_of_velx, no_of_vely)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak4, no_of_velx, no_of_vely)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak5, no_of_velx, no_of_vely)) return OMA_MEMORY;
	
	// For each vector ....
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			//... and each peak
			for(i=1;i<=peaks_to_save;i++)  {
				// Get the values from the PIV buffer
				oldX = PKGetNthVectorX(nc,nt,i);
				oldY = PKGetNthVectorY(nc,nt,i);
				oldZ = PKGetNthVectorZ(nc,nt,i);
				oldPeak = PKGetNthPeak(nc,nt,i);
				
				// drop in the approriate value into Vectors (X&Y adjusted)
				SDK_Vector_SetX(&tVect, -oldX);
				SDK_Vector_SetY(&tVect, oldY);
				SDK_Vector_SetZ(&tVect, oldZ);
				SDK_Vector_SetPeak(&tVect, oldPeak);
				
				// Place the Vector into the Field
				if (i==1) SDK_Field_SetVector(&PIVpeak1, (no_of_velx-1)-nc, nt, &tVect);
				if (i==2) SDK_Field_SetVector(&PIVpeak2,  (no_of_velx-1)-nc, nt, &tVect);
				if (i==3) SDK_Field_SetVector(&PIVpeak3,  (no_of_velx-1)-nc, nt, &tVect);
				if (i==4) SDK_Field_SetVector(&PIVpeak4,  (no_of_velx-1)-nc, nt, &tVect);
				if (i==5) SDK_Field_SetVector(&PIVpeak5,  (no_of_velx-1)-nc, nt, &tVect);				
			} 
		}
	}
	
	// Copy the Field structures to the OMA PIV buffer
	SDK_Field_2Buffer(&PIVpeak1);
	SDK_Field_2NthPeak(&PIVpeak2, 2);
	SDK_Field_2NthPeak(&PIVpeak3, 3);
	SDK_Field_2NthPeak(&PIVpeak4, 4);
	SDK_Field_2NthPeak(&PIVpeak5, 5);
	
	// Free Field Memory structures
	SDK_Field_Free(&PIVpeak1);
	SDK_Field_Free(&PIVpeak2);
	SDK_Field_Free(&PIVpeak3);
	SDK_Field_Free(&PIVpeak4);
	SDK_Field_Free(&PIVpeak5);
	
	return OMA_OK;
}
//***************************************************************//
//**    P I V F L V - Flip the PIV vertically                                     *//
//**               NB: this will work for all peaks (peaks_to_save<6) *//
//***************************************************************//
int
pivflv(int n, int index)	
{	
	Field             PIVpeak1, PIVpeak2, PIVpeak3, PIVpeak4, PIVpeak5;
	Vector          tVect = {0.,0.,0.,0};
	int                 i, nc, nt;
	float              oldX, oldY, oldZ;
	int                 oldPeak;
	
	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	
	// Create new Field structures to handle the rotated image
	if (SDK_Field_Create(&PIVpeak1, no_of_velx, no_of_vely)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak2, no_of_velx, no_of_vely)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak3, no_of_velx, no_of_vely)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak4, no_of_velx, no_of_vely)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak5, no_of_velx, no_of_vely)) return OMA_MEMORY;
	
	// For each vector ....
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			//... and each peak
			for(i=1;i<=peaks_to_save;i++)  {
				// Get the values from the PIV buffer
				oldX = PKGetNthVectorX(nc,nt,i);
				oldY = PKGetNthVectorY(nc,nt,i);
				oldZ = PKGetNthVectorZ(nc,nt,i);
				oldPeak = PKGetNthPeak(nc,nt,i);
				
				// drop in the approriate value into Vectors (X&Y adjusted)
				SDK_Vector_SetX(&tVect, oldX);
				SDK_Vector_SetY(&tVect, -oldY);
				SDK_Vector_SetZ(&tVect, oldZ);
				SDK_Vector_SetPeak(&tVect, oldPeak);
				
				// Place the Vector into the Field
				if (i==1) SDK_Field_SetVector(&PIVpeak1, nc, (no_of_vely-1)-nt, &tVect);
				if (i==2) SDK_Field_SetVector(&PIVpeak2, nc, (no_of_vely-1)-nt, &tVect);
				if (i==3) SDK_Field_SetVector(&PIVpeak3, nc, (no_of_vely-1)-nt, &tVect);
				if (i==4) SDK_Field_SetVector(&PIVpeak4, nc, (no_of_vely-1)-nt, &tVect);
				if (i==5) SDK_Field_SetVector(&PIVpeak5, nc, (no_of_vely-1)-nt, &tVect);				
			} 
		}
	}
	
	// Copy the Field structures to the OMA PIV buffer
	SDK_Field_2Buffer(&PIVpeak1);
	SDK_Field_2NthPeak(&PIVpeak2, 2);
	SDK_Field_2NthPeak(&PIVpeak3, 3);
	SDK_Field_2NthPeak(&PIVpeak4, 4);
	SDK_Field_2NthPeak(&PIVpeak5, 5);
	
	// Free Field Memory structures
	SDK_Field_Free(&PIVpeak1);
	SDK_Field_Free(&PIVpeak2);
	SDK_Field_Free(&PIVpeak3);
	SDK_Field_Free(&PIVpeak4);
	SDK_Field_Free(&PIVpeak5);
	
	return OMA_OK;
}
//***************************************************************//
//**    P I V M A X - Set the shown vector to zero if it is bigger than *//
//**                                                    the specified arguement   *//
//***************************************************************//
int
pivmax(int n, int index)	
{	
	int                 nc, nt;
	float              oldX, oldY, oldZ;
	float              magnitude;
	float              limit;      // Maximum allowed vector magnitude
	int                narg;      // The number of command line arguements
	
	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	
	// Read in command line arguments.
	narg = sscanf(&cmnd[index],"%f",&limit);
	if (narg ==0){
		beep();
		printf("I can't guess a default here! I NEED a maximum vector size\n");
		return OMA_RANGE;
	}
	
	// For each Vector in the buffer get length using PKcommands
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			// Get the values from the PIV buffer
			oldX = PKGetVectorX(nc,nt);
			oldY = PKGetVectorY(nc,nt);
			oldZ = PKGetVectorZ(nc,nt);
			
			// Test the magnitude against limits...
			magnitude = sqrt(oldZ*oldZ + oldX*oldX +oldY*oldY);
			if (magnitude > limit) {
				// ...and zero the top peak if needed
				PKSetVectorX(nc, nt, 0.0);
				PKSetVectorY(nc, nt, 0.0);
				PKSetVectorZ(nc, nt, 0.0);
			}
		}
	}
	
	return OMA_OK;
}
//***************************************************************//
//**    P I V M I N - Set the shown vector to zero if it is SMALLER than *//
//**                                                    the specified arguement   *//
//***************************************************************//
int
pivmin(int n, int index)	
{	
	int                 nc, nt;
	float              oldX, oldY, oldZ;
	float              magnitude;
	float              limit;      // Maximum allowed vector magnitude
	int                narg;      // The number of command line arguements
	
	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	
	// Read in command line arguments.
	narg = sscanf(&cmnd[index],"%f",&limit);
	if (narg ==0){
		beep();
		printf("I can't guess a default here! I NEED a maximum vector size\n");
		return OMA_RANGE;
	}
	
	// For each Vector in the buffer get length using PKcommands
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			// Get the values from the PIV buffer
			oldX = PKGetVectorX(nc,nt);
			oldY = PKGetVectorY(nc,nt);
			oldZ = PKGetVectorZ(nc,nt);
			
			// Test the magnitude against limits...
			magnitude = sqrt(oldZ*oldZ + oldX*oldX +oldY*oldY);
			if (magnitude < limit) {
				// ...and zero the top peak if needed
				PKSetVectorX(nc, nt, 0.0);
				PKSetVectorY(nc, nt, 0.0);
				PKSetVectorZ(nc, nt, 0.0);
			}
		}
	}
	
	return OMA_OK;
}
//************************************************************************************//
//**    P I V C +     Conditional PIV arithmetic. Evaluated on non-zero vectors ONLY   *//
//************************************************************************************//
int
pivcplus(int n, int index)	
{	
	float      dx, dy;
	float              oldX, oldY, oldZ;
	int         nc , nt, narg;
	float      magnitude;
	
	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	
	// Read in command line arguments.
	narg = sscanf(&cmnd[index],"%f %f",&dx, &dy);
	if (narg ==0){  dx=0.0;  dy = 0.0; }
	if (narg ==1)  dy = 0.0;
	
	// For each Vector in the buffer get length using PKcommands
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			// Get the values from the PIV buffer
			oldX = PKGetVectorX(nc,nt);
			oldY = PKGetVectorY(nc,nt);
			oldZ = PKGetVectorZ(nc,nt);
			
			// Test the magnitude against limits...
			magnitude = sqrt(oldZ*oldZ + oldX*oldX +oldY*oldY);
			if (magnitude > 0.0) {
				// ...and do the arithmetic as needed
				PKSetVectorX(nc, nt, oldX + dx);
				PKSetVectorY(nc, nt, oldY + dy);
				PKSetVectorZ(nc, nt, oldZ);
			}
		}
	}
	
	return OMA_OK;
}

//************************************************************************************//
//**    P I V C -     Conditional PIV arithmetic. Evaluated on non-zero vectors ONLY   *//
//************************************************************************************//
int
pivcminus(int n, int index)	
{	
	float      dx, dy;
	float              oldX, oldY, oldZ;
	int         nc , nt, narg;
	float      magnitude;

	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	
	// Read in command line arguments.
	narg = sscanf(&cmnd[index],"%f %f",&dx, &dy);
	if (narg ==0){  dx=0.0;  dy = 0.0; }
	if (narg ==1)  dy = 0.0;
	
	// For each Vector in the buffer get length using PKcommands
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			// Get the values from the PIV buffer
			oldX = PKGetVectorX(nc,nt);
			oldY = PKGetVectorY(nc,nt);
			oldZ = PKGetVectorZ(nc,nt);
			
			// Test the magnitude against limits...
			magnitude = sqrt(oldZ*oldZ + oldX*oldX +oldY*oldY);
			if (magnitude > 0.0) {
				// ...and do the arithmetic as needed
				PKSetVectorX(nc, nt, oldX - dx);
				PKSetVectorY(nc, nt, oldY - dy);
				PKSetVectorZ(nc, nt, oldZ);
			}
		}
	}
	
	return OMA_OK;
}

//************************************************************************************//
//**    P I V C *     Conditional PIV arithmetic. Evaluated on non-zero vectors ONLY   *//
//************************************************************************************//
int
pivcmul(int n, int index)	
{	
	float scale;
	float              oldX, oldY, oldZ;
	int         nc , nt, narg;
	float      magnitude;

	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	
	// Read in command line arguments.
	narg = sscanf(&cmnd[index],"%f",&scale);
	if (narg !=1)  scale=1.0;
	
	// For each Vector in the buffer get length using PKcommands
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			// Get the values from the PIV buffer
			oldX = PKGetVectorX(nc,nt);
			oldY = PKGetVectorY(nc,nt);
			oldZ = PKGetVectorZ(nc,nt);
			
			// Test the magnitude against limits...
			magnitude = sqrt(oldZ*oldZ + oldX*oldX +oldY*oldY);
			if (magnitude > 0.0) {
				// ...and do the arithmetic as needed
				PKSetVectorX(nc, nt, oldX *scale);
				PKSetVectorY(nc, nt, oldY *scale);
				PKSetVectorZ(nc, nt, oldZ *scale);
			}
		}
	}
	
	return OMA_OK;
}

//************************************************************************************//
//**    P I V C /     Conditional PIV arithmetic. Evaluated on non-zero vectors ONLY   *//
//************************************************************************************//
int
pivcdiv(int n, int index)	
{	
	float scale;
	float              oldX, oldY, oldZ;
	int         nc , nt, narg;
	float      magnitude;

	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	
	// Read in command line arguments.
	narg = sscanf(&cmnd[index],"%f",&scale);
	if (narg !=1)  scale=1.0;
	
	// Make sure we are not dividing by zero
	if (scale==0) scale=1;
	
	// For each Vector in the buffer get length using PKcommands
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			// Get the values from the PIV buffer
			oldX = PKGetVectorX(nc,nt);
			oldY = PKGetVectorY(nc,nt);
			oldZ = PKGetVectorZ(nc,nt);
			
			// Test the magnitude against limits...
			magnitude = sqrt(oldZ*oldZ + oldX*oldX +oldY*oldY);
			if (magnitude > 0.0) {
				// ...and do the arithmetic as needed
				PKSetVectorX(nc, nt, oldX/scale);
				PKSetVectorY(nc, nt, oldY/scale);
				PKSetVectorZ(nc, nt, oldZ/scale);
			}
		}
	}
	
	return OMA_OK;
}

//************************************************************************************//
//**    P I V M *     Multiply each vector in a PIV Vector field by corresponding pixel value in buffer    *//
//************************************************************************************//
int
pivmaskmul()	
{	
	float scale;
	float              oldX, oldY, oldZ;
	int         nc , nt;
	DATAWORD value;
	
	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Error %d: Must have valid PIV image first.\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	// Check that the PIV image is the same size as the buffer.
	if( (no_of_velx != header[NCHAN]) || (no_of_vely != header[NTRAK])) {
		beep();
		printf("Error %d: Image in buffer must be the same size as the PIV Field.\n", OMA_RANGE);
		return OMA_RANGE;
	}
	
	// For each Vector in the buffer get length using PKcommands
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			
			// Get the values from the PIV buffer
			oldX = PKGetVectorX(nc,nt);
			oldY = PKGetVectorY(nc,nt);
			oldZ = PKGetVectorZ(nc,nt);
			
			// Get the value of the scalar (ie the pixel value)
			value = PKGetPixel(nc,nt);
			scale = (float) value;
			
			// Do the Maths
			PKSetVectorX(nc, nt, oldX *scale);
			PKSetVectorY(nc, nt, oldY *scale);
			PKSetVectorZ(nc, nt, oldZ *scale);
			
		}
	}
	
	return OMA_OK;
}
//************************************************************************************//
//**    P I V M /     Divide each vector in a PIV Vector field by corresponding pixel value in buffer    *//
//************************************************************************************//
int
pivmaskdiv()	
{	
	float scale;
	float              oldX, oldY, oldZ;
	int         nc , nt;
	DATAWORD value;
	
	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Error %d: Must have valid PIV image first.\n", OMA_NOEXIST);
		return OMA_NOEXIST;
	}
	
	// Check that the PIV image is the same size as the buffer.
	if( (no_of_velx != header[NCHAN]) || (no_of_vely != header[NTRAK])) {
		beep();
		printf("Error %d: Image in buffer must be the same size as the PIV Field.\n", OMA_RANGE);
		return OMA_RANGE;
	}
	
	// For each Vector in the buffer get length using PKcommands
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			
			// Get the values from the PIV buffer
			oldX = PKGetVectorX(nc,nt);
			oldY = PKGetVectorY(nc,nt);
			oldZ = PKGetVectorZ(nc,nt);
			
			// Get the value of the scalar (ie the pixel value)
			value = PKGetPixel(nc,nt);
			scale = (float) value;
			
			if (scale !=0.0) {
				// Do the Maths
				PKSetVectorX(nc, nt, (float) oldX /scale);
				PKSetVectorY(nc, nt, (float) oldY /scale);
				PKSetVectorZ(nc, nt, (float) oldZ /scale);
			} else {
				// you should expect zeros in the place of errors
				PKSetVectorX(nc, nt, (float) 0.0);
				PKSetVectorY(nc, nt, (float) 0.0);
				PKSetVectorZ(nc, nt, (float) 0.0);
			}
		}
	}
	
	return OMA_OK;
}


#define NUMELEMENT 1024
//********************************************************
//*** POPULN <filename>
//***		 - Write the population count of pixel values
//***              (Histogram) as a CSV text file 'filename.csv'.
//***         P. Kalt (2004)
//********************************************************
 int
 population (int n, int index)
 {
     int         nc, nt;
     int         i, rightone;
     int         binsofar;
     FILE         *fp;
     DATAWORD datval;
     DATAWORD valstr[NUMELEMENT]={0};
     int popstr[NUMELEMENT] ={0};
     binsofar = 0;

     //Go through Image and keep track of totals
     for(nt=0; nt<header[NTRAK]; nt++) {
         for(nc=0; nc<header[NCHAN]; nc++) {
			datval = PKGetPixel(nc,nt);
             rightone = -1;
             // Find the right Element in the Data Structure
             for (i=0;i<NUMELEMENT;i++){
                 if (valstr[i] == datval) rightone = i;
             }
             if (rightone >= 0) popstr[rightone]++;
             else {
                 if (binsofar < NUMELEMENT) {
                     valstr[binsofar] = datval;
                     popstr[binsofar] = 1;
                     binsofar++;
                 }
             }
         }
     }
     // Open FILE for wrinting values
     strcpy(scratch, fullname(&cmnd[index], CSV_DATA));
     fp = fopen(scratch,"w");
     if(fp != NULL) {
         // Write POPULATION information
         for(i=0;i<NUMELEMENT;i++)
			#ifdef FLOAT
			fprintf(fp,"%g, %d\n", valstr[i], popstr[i]);
			#else
			fprintf(fp,"%d, %d\n", valstr[i], popstr[i]);
			#endif
         // Pack up and go
         fclose(fp);
         settext(&cmnd[index]);		//* text file type */
         fileflush(&cmnd[index]);	//* for updating directory */
     } else {
         beep();
         printf("Unable to create File.\n");
		return OMA_FILE;
	}
	SDK_Image_Refresh();
	return OMA_OK;
}


//***************************************************************//
//**    P I V F T R - Filter PIV based on local neighborhood mean*//
//***************************************************************//
int
pivfilter(int n, int index)	
{	
	int i,num_neighbors,j,k;
	int narg;
	float totalx, totaly;
	float diffx, diffy;
	float meanx, meany;
	float rmsx, rmsy;
	int *badveclist;

	Field origField, newField;
	float sigma;
	short px, py;
	
	// Check for existance of OMA PIV data
	if(no_of_velx == 0) {
		printf("Must have valid PIV image first.\n");
		beep();
		return OMA_NOEXIST;
	}
	
	// Read in command line arguments. Sigma = variance from local mean
	narg = sscanf(&cmnd[index],"%f",&sigma);
	if (narg ==0) sigma=3.0;
	
	// Transfer the OMA PIV to a Field structure
	if(SDK_Field_CopyBuffer(&origField)!=OMA_OK){
		beep();
		printf("SDK_Field_CopyBuffer() returned an error\n");
		return OMA_MEMORY;
	}
	
	// Create results Field structure
	if(SDK_Field_Create(&newField, no_of_velx, no_of_vely)!=OMA_OK){
		beep();
		printf("SDK_Field_Create() returned an error\n");
		return OMA_MEMORY;
	}
	
	// Memory for a structure to remember bad vectors for deleting at the end
	badveclist = malloc(no_of_velx*no_of_vely*sizeof(int));
	k=0;
	for(py = 0; py < no_of_vely; py++ ){
		for(px = 0; px < no_of_velx; px++ ){
			// Build up register of valid neighbours
			num_neighbors = 0;
			totalx = 0;
			totaly = 0;
			// compare adjacent vectors and discard those beyond limits.
			for( i= -2; i<=2; i++){
				for( j= -2; j<=2; j++){
					//Quick test that the neighbours actually exist.
					if (i==0 && j==0) continue;        // dont process base vector
					if ( px+j >= no_of_velx ) break;	// don't run off the page
					if ( px+j < 0 ) continue;			// first vector in a row
					if ( py+i < 0 || py+i >= no_of_vely ) break;	// top or bottom of page
					
					num_neighbors++;
					totalx += vx(px+j, py+i, 1);
					totaly += vy(px+j, py+i, 1);
				}
			}
			// Work out the average.
			meanx = totalx / (float) num_neighbors;
			meany = totaly / (float) num_neighbors;
			totalx = 0.0;
			totaly = 0.0;				
			// Work out the rms
			for( i= -1; i<=1; i++){
				for( j= -1; j<=1; j++){
	
					//Quick test that the neighbours actually exist.
					if (i==0 && j==0) continue;        // dont process base vector
					if ( px+j >= no_of_velx ) break;	// don't run off the page
					if ( px+j < 0 ) continue;			// first vector in a row
					if ( py+i < 0 || py+i >= no_of_vely ) break;	// top or bottom of page
					
					diffx = vx(px+j, py+i, 1) - meanx;
					diffy = vy(px+j, py+i, 1) - meany;
					diffx *= diffx;
					diffy *=diffy;
					totalx += diffx;
					totaly += diffy;
				}
			}
			rmsx = sqrt(totalx/(num_neighbors-1));
			rmsy = sqrt(totaly/(num_neighbors-1));
			
			// If our centre vector is BAD enough then delete_it
			diffx = fabs(vx(px, py, 1) - meanx) - sigma*rmsx;
			diffy = fabs(vy(px, py, 1) - meany) - sigma*rmsy;
			
			if (diffx > 0 || diffy > 0) {
				*(badveclist + k) = 1;
			} else {
				*(badveclist + k) = 0;
			}
			k++;
		}
	}
	k=0;
	for(py = 0; py < no_of_vely; py++ ){
		for(px = 0; px < no_of_velx; px++ ){
			if (*(badveclist + k)) {
				kill_vector(px,py);
			}
			k++;
		}
	}
	if (badveclist != 0) free(badveclist);
	return OMA_OK;
}


//***************************************************************//
//**    P I V B I G - All components of the max vector in PIV field*//
//***************************************************************//
int
pivbiggest(int n, int index)	
{	
	Field myField;
	short nc, nt;
	float win_x=0, win_y=0,  win_z=0, win_mag;
	float mag;
	// Test for existance
	if(no_of_velx == 0) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	
	// Transfer the OMA PIV to a Field structure
	if(SDK_Field_CopyBuffer(&myField)!=OMA_OK){
		beep();
		printf("SDK_Field_CopyBuffer() returned an error\n");
		return OMA_MEMORY;
	}
	
	// This variable holds the maximum magnitude found so far
	win_mag = 0.0;
	
	for( nt=0; nt< no_of_velx; nt ++) {
		for( nc=0; nc< no_of_vely; nc ++) {
			mag = SDK_Vector_Magnitude(SDK_Field_GetVector(&myField, nc, nt));
			if (mag > win_mag){
				win_mag = mag;
				win_x = SDK_Vector_GetX(SDK_Field_GetVector(&myField, nc, nt));
				win_y = SDK_Vector_GetY(SDK_Field_GetVector(&myField, nc, nt));
				win_z = SDK_Vector_GetZ(SDK_Field_GetVector(&myField, nc, nt));
			}
		}
          }
	printf("max vector = %.2f\t%.2f",win_x,win_y);
	printf("\t%.2f\t%.2f\n",win_z,win_mag);
	return OMA_OK;
}



//***************************************************************//
//**    G R A D P K - Determine gradient from 4 points at a distance n from center pixel*//
//***************************************************************//
int
gradient_pk(int n, int index)	
{	

	int nc,nt;
	Image myimage;
	
	float dx;
	float dx0, dx22p5, dx45, dx67p5;
	float dy0, dy22p5, dy45, dy67p5;
	float dx56p25=0, dx11p25=0, dx33p75=0,dx78p75=0;
	float dy56p25=0, dy11p25=0, dy33p75=0,dy78p75=0;
	
	int step, narg;
	int Err;
	int l, j, k;
	
	narg = sscanf(&cmnd[index],"%d",&step);
	if (narg ==0) {
		step = 1;
		printf("No arguments specified, using 1.\nOMA>");
	}

	if((Err = SDK_Image_Create(&myimage, header[NCHAN], header[NTRAK]))!=OMA_OK){
		beep();
		nomemory();
		return Err; 
	}
	
	for(nt=0; nt<header[NTRAK];nt++) {
		for(nc=0;nc < header[NCHAN]; nc++){
			
			j= (int) step* sin(Pi*22.5/180);
			k= (int) step* sin(Pi*45/180);
			l= (int) step* cos(Pi*22.5/180);
				
			dx0 = (PKGetPixel(nc+step,nt) - PKGetPixel(nc-step,nt))/2;
			dy0 = (PKGetPixel(nc,nt+step) - PKGetPixel(nc,nt-step))/2;
			dx22p5 = (PKGetPixel(nc+j,nt+l) - PKGetPixel(nc-j,nt-l))/2;
			dy22p5 = (PKGetPixel(nc-l,nt+j) - PKGetPixel(nc+l,nt-j))/2;
			dx45 = (PKGetPixel(nc+k,nt+k) - PKGetPixel(nc-k,nt-k))/2;
			dy45 = (PKGetPixel(nc-k,nt+k) - PKGetPixel(nc+k,nt-k))/2;
			dx67p5 = (PKGetPixel(nc+l,nt+j) - PKGetPixel(nc-l,nt-j))/2;
			dy67p5 = (PKGetPixel(nc-j,nt+l) - PKGetPixel(nc+j,nt-l))/2;
	
			dx = (   sqrt(dx0*dx0 + dy0*dy0) 
					 + sqrt(dx45*dx45 + dy45*dy45)
					 + sqrt(dx22p5*dx22p5 + dy22p5*dy22p5)
					 + sqrt(dx67p5*dx67p5 + dy67p5*dy67p5) 
					 + sqrt(dx11p25*dx11p25 + dy11p25*dy11p25) 
					 + sqrt(dx33p75*dx33p75 + dy33p75*dy33p75)
					 + sqrt(dx56p25*dx56p25 + dy56p25*dy56p25)
					 + sqrt(dx78p75*dx78p75 + dy78p75*dy78p75) 
					 )/8;
			
			SDK_Image_SetPixel(&myimage, nc, nt, (DATAWORD) dx );
		}
	}
	SDK_Image_2Buffer(&myimage);
	SDK_Image_Free(&myimage);
	SDK_Image_Refresh();
	return OMA_OK;
}

//********************************************************
//*** SWAPIX <pixval> <newval>
//***		 - Swap every pixel with value pixval to newval
//********************************************************
int
swapix(int n,int index)
{
	int	nc, nt;
	int narg;
	int pixval, newval;
	
	n = 0;
	
	narg = sscanf(&cmnd[index],"%d %d",&pixval,&newval);
	
	if(narg != 2) {
		beep();
		printf("Need 2 Arguments.\n"); 
		return OMA_ARGS;
	}
				
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN]; nc++) {
			if (PKGetPixel (nc,nt) == pixval) PKSetPixel(nc,nt,newval);
		}
	}
	SDK_Image_Refresh();
	return OMA_OK;
}


//********************************************************
//*** DIST2N <n> 
//***		 - Swap every pixel with (int) distance to nearest meighbour with value <n>
//********************************************************
int
dist2n(int n,int index)
{
	int	nc, nt;
	int narg;
	DATAWORD  newval;
	int looplimit;
	int boxsize;
	int FoundN;
	int j,k,l;
	int Err;
	int dx=0,dy=0;
	DATAWORD dist;
	Image myimage;
	
	n = 0;
	// Read in Comand line arguments
	#ifdef FLOAT
	narg = sscanf(&cmnd[index],"%f",&newval);
	#else
	narg = sscanf(&cmnd[index],"%d",&newval);
	#endif
	
	if(narg != 1) {
		newval = 0;
		printf("Using defaults\n");
	}
	
	if((Err = SDK_Image_Create(&myimage,header[NCHAN], header[NTRAK]))!=OMA_OK){ 
		beep();
		nomemory();
		return Err;
	}
		
	// Set the maximum box size
	looplimit = header[NCHAN];
	if (looplimit < header[NTRAK]) looplimit = header[NTRAK];
				
	// For each pixel in original image.... loop
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN]; nc++) {
			
			// Check Itself
			if (PKGetPixel(nc,nt) == newval) {
				dx = dy = 0;
			} else {
				FoundN = 0;
				// Otherwise Loop for each level up to looplimit until an 'newval' is found
				for (boxsize = 1; boxsize < looplimit; boxsize++){
					j= (int) boxsize* sin(Pi*22.5/180);
					k= (int) boxsize* sin(Pi*45/180);
					l= (int) boxsize* sin(Pi*67.5/180);
					if (PKGetPixel(nc, nt+boxsize) == newval) FoundN = 1; //N
					if (PKGetPixel(nc+k, nt+k) == newval)      FoundN = 1; //NE
					if (PKGetPixel(nc+j, nt+l) == newval)        FoundN = 1; //ENE
					if (PKGetPixel(nc+l, nt+j) == newval)    FoundN = 1; //NNE
					
					if (PKGetPixel(nc+boxsize, nt) == newval) FoundN = 1; //E
					if (PKGetPixel(nc+k, nt-k) == newval)      FoundN = 1; //SE
					if (PKGetPixel(nc+j, nt-l) == newval)        FoundN = 1; //ESE
					if (PKGetPixel(nc+l, nt-j) == newval)    FoundN = 1; //SSE
					
					if (PKGetPixel(nc, nt-boxsize) == newval) FoundN = 1; //S
					if (PKGetPixel(nc-k, nt-k) == newval)      FoundN = 1; //SW
					if (PKGetPixel(nc-j, nt-l) == newval)        FoundN = 1; //WSW
					if (PKGetPixel(nc-l, nt-j) == newval)    FoundN = 1; //SSW
					
					if (PKGetPixel(nc, nt-boxsize) == newval) FoundN = 1; //W
					if (PKGetPixel(nc-k, nt+k) == newval)      FoundN = 1; //NW
					if (PKGetPixel(nc-j, nt+l) == newval)        FoundN = 1; //WNW
					if (PKGetPixel(nc-l, nt+j) == newval)    FoundN = 1; //NNW

					if (FoundN==1) {
						dx = 0; 
						dy = boxsize;
						break;
					}
				}
				if (FoundN ==0) {
					dx = dy = 6666;
				} 
			}
			dist =  (DATAWORD) sqrt(dx*dx +dy*dy);
			SDK_Image_SetPixel(&myimage, nc, nt, dist);
		} //for nc
	} //for nt
	SDK_Image_2Buffer(&myimage);
	SDK_Image_Free(&myimage);
	SDK_Image_Refresh();
	return OMA_OK;
}

//***************************************************************//
//**    FMAXH - Find the maximum value in each column and mark that pixel with its original value*//
//**                   Scan lines HORIZONTALLY			        *//
//***************************************************************//
int
fmax_horizontal (int n, int index)	
{	
	
	int nc,nt;
	DATAWORD min_in_row, max_in_row;
	DATAWORD value;

	//For each Row
	for(nt=0; nt<header[NTRAK]; nt++) {
		// First Pass to Find Maximum value
		min_in_row = 0;
		max_in_row = 0;
		// First Pass to Find Maximum value
		for (nc =0; nc <header[NCHAN]; nc++){
			value = PKGetPixel(nc,nt);
			if (value < min_in_row) min_in_row = value;
			if (value > max_in_row) max_in_row = value;
		}
		
		// If legitimate, then scan the row and set values in result image
		if (min_in_row != max_in_row) {
			for (nc=0; nc<header[NCHAN]; nc++){
				if (PKGetPixel(nc,nt) == max_in_row) PKSetPixel(nc, nt, max_in_row);
				else PKSetPixel(nc, nt, 0);
			}
		} 
	}
	SDK_Image_Refresh();
	return OMA_OK;
}

//***************************************************************//
//**    FMAXV - Find the maximum value in each column and mark that pixel as 1   *//
//**                   Scan lines VERTICALLY   			        *//
//***************************************************************//
int
fmax_vertical (int n, int index)	
{	
	int nc,nt;
	DATAWORD min_in_col, max_in_col;
	DATAWORD value;
	
	//For each Column
	for(nc=0; nc<header[NCHAN]; nc++) {
		// First Pass to Find Maximum value
		min_in_col = 0;
		max_in_col = 0;
		// First Pass to Find Maximum value
		for (nt =0; nt <header[NTRAK]; nt++){
			value = PKGetPixel(nc,nt);
			if (value < min_in_col) min_in_col = value;
			if (value > max_in_col) max_in_col = value;
		}
		
		// If legitimate, then scan the row and set values in result image
		if (min_in_col != max_in_col) {
			for (nt=0; nt<header[NTRAK]; nt++){
				if (PKGetPixel(nc,nt) == max_in_col) PKSetPixel(nc, nt, max_in_col);
				else PKSetPixel(nc, nt, 0);
			}
		} 
	}
	SDK_Image_Refresh();
	return OMA_OK;
}

//***************************************************************//
//**    TRIAL - Lets try and use these things			*//
//***************************************************************//
int
trial ()	
{	
	int nc,nt;
	Image myimage;
	
	if(SDK_Image_Create(&myimage, 200, 200
						)!=OMA_OK){
		beep();
		nomemory(); 
		return OMA_MEMORY;
	}
	
	for(nt=0; nt<myimage.height; nt++) {
		for (nc =0; nc<myimage.height; nc++){
			SDK_Image_SetPixel(&myimage, nc, nt, nc);
			if (nc==100&&nt==100)printf("we have value %d\n",SDK_Image_GetPixel(&myimage,nc,nt));
		}
	}
	SDK_Image_2Buffer(&myimage);
	SDK_Image_Free(&myimage);
	SDK_Image_Refresh();
	return OMA_OK;
}
//***************************************************************//
//**    S M E A R - Lets try and use these things			*//
//***************************************************************//
int
smear (int n, int index)	
{	
	int	nc, nt;
	int narg;
	Image myimage;
	int looplimit;
	int Err;
	float dist;
	float newval;
	DATAWORD datholder;
	int box_x, box_y;
	
	n = 0;
	// Read in Comand line arguments
	narg = sscanf(&cmnd[index],"%f",&newval);
	
	if(narg != 1) {
		newval = 0.5;
		printf("Using defaults radius of 0.5 pixels\n");
	}
	
	if((Err = SDK_Image_Create(&myimage,header[NCHAN], header[NTRAK]))!=OMA_OK){ 
		beep();
		nomemory();
		return Err;
	}
	
	// Set the maximum box size
	looplimit = (int)newval;
				
	// For each pixel in original image.... loop
	for(nt=0; nt<header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN]; nc++) {
			
			// Check Itself
			if (PKGetPixel(nc,nt) > 0) {
				// Otherwise Loop for each level up to looplimit until an 'newval' is found
				for (box_x = -looplimit; box_x < looplimit; box_x++){
					for (box_y = -looplimit; box_y < looplimit; box_y++){
						// How far to center pixel?
						dist = sqrt(box_x*box_x + box_y*box_y);
						if (dist<=newval){
							// Add 1 to this looped pixel in Image struct
							datholder = SDK_Image_GetPixel(&myimage, nc+box_x, nt+box_y);
							SDK_Image_SetPixel(&myimage, nc+box_x, nt+box_y, datholder+1);
						}
					}
				} 
			}
		} //for nc
	} //for nt
	SDK_Image_2Buffer(&myimage);
	SDK_Image_Free(&myimage);
	SDK_Image_Refresh();
	return OMA_OK;
}
//***************************************************************//
//**    M A K P S Z - Trims the edges from around an image to    *//
//**                          make it the perfect size for even PIV           *//
//***************************************************************//
int
makpsz (int n, int index)	
{	
	int	nc, nt;
	Image myimage;
	int Err;	
	int cropped_height;
	int cropped_width;
	int current_height;
	int current_width;
	
	int loopn;

	cropped_width = cropped_height = 0;
	current_width = header[NCHAN];
	current_height = header[NTRAK];
	
	if (current_width < fftsize || current_height < fftsize){
		beep();
		printf("It appears that the current image is alread smaller than one fftsize (=%d pixels)\n", fftsize);
		return OMA_OK;
	}
	
	cropped_width += fftsize;
	cropped_height += fftsize;
	current_width -= fftsize;
	current_height -= fftsize;
	
	for (loopn = current_width; loopn > boxinc; loopn -= boxinc){
		cropped_width += boxinc;
	}
	for (loopn = current_height; loopn > boxinc; loopn -= boxinc){
		cropped_height += boxinc;
	}
	
	
	if((Err = SDK_Image_Create(&myimage,cropped_width,cropped_height ))!=OMA_OK){ 
		beep();
		nomemory();
		return Err;
	}
	
				
	// For each pixel in original image.... loop
	for(nt=0; nt<cropped_height; nt++) {
		for(nc=0; nc<cropped_width; nc++) {
			if((Err = SDK_Image_SetPixel(&myimage, nc, nt, PKGetPixel(nc,nt)))!=OMA_OK) return Err;
		}
	}
	SDK_Image_2Buffer(&myimage);
	SDK_Image_Free(&myimage);
	SDK_Image_Refresh();
	return OMA_OK;
}
//***************************************************************//
//**    S I N K P K- Is the pixel likely to be a local minima?          *//
//***************************************************************//
int
sinkpk (int n, int index)	
{	
	int	nc, nt;
	Image myimage;
	float radius;
	int no_of_flags;
	int narg;
	DATAWORD thispixel;
	int Err;	
	int no_found;
	int j, k, l;
	// Read in Comand line arguments
	narg = sscanf(&cmnd[index],"%f %d",&radius, &no_of_flags);
	
	if(narg != 2) {
		radius = 5.0;
		no_of_flags = 8;
		printf("Using defaults: radius of %f pixels - with %d hits\n", radius, no_of_flags);
	}
	
	if((Err = SDK_Image_Create(&myimage,header[NCHAN],header[NTRAK] ))!=OMA_OK){ 
		beep();
		nomemory();
		return Err;
	}
	
				
	// For each pixel in original image.... loop
	for(nt=0; nt< header[NTRAK]; nt++) {
		for(nc=0; nc<header[NCHAN] ; nc++) {
			j= (int) radius* sin(Pi*22.5/180);
			k= (int) radius* sin(Pi*45/180);
			l= (int) radius* cos(Pi*22.5/180);
			
			no_found = 0;
			thispixel = PKGetPixel(nc, nt);
			if (PKGetPixel(nc+radius,nt)>thispixel) no_found++;
			if (PKGetPixel(nc-radius,nt)>thispixel) no_found++;
			if (PKGetPixel(nc,nt+radius)>thispixel) no_found++;
			if (PKGetPixel(nc,nt-radius)>thispixel) no_found++;
			if (PKGetPixel(nc+j,nt+l)>thispixel) no_found++;
			if (PKGetPixel(nc-j,nt-l)>thispixel) no_found++;
			if (PKGetPixel(nc-l,nt+j)>thispixel) no_found++;
			if (PKGetPixel(nc+l,nt-j)>thispixel) no_found++;
			if (PKGetPixel(nc+k,nt+k)>thispixel) no_found++;
			if (PKGetPixel(nc-k,nt-k)>thispixel) no_found++;
			if (PKGetPixel(nc-k,nt+k)>thispixel) no_found++;
			if (PKGetPixel(nc+k,nt-k)>thispixel) no_found++;
			if (PKGetPixel(nc+l,nt+j)>thispixel) no_found++;
			if (PKGetPixel(nc-l,nt-j)>thispixel) no_found++;
			if (PKGetPixel(nc-j,nt+l)>thispixel) no_found++;
			if (PKGetPixel(nc+j,nt-l)>thispixel) no_found++;
			
			if (no_found >= no_of_flags) {
				if((Err = SDK_Image_SetPixel(&myimage, nc, nt, 1))!=OMA_OK) return Err; 
			}else{
				if((Err = SDK_Image_SetPixel(&myimage, nc, nt, 0))!=OMA_OK) return Err;	
			}
		}
	}
	SDK_Image_2Buffer(&myimage);
	SDK_Image_Free(&myimage);
	SDK_Image_Refresh();
	return OMA_OK;
}

//------------
int pdfit(int n,int index)
// use 2-D fit (hardwired for now) to calculate scalar from P and D image
//							Arguments:
//							Depolarized file name
//							Rayleigh-cts in air
//							Depolarized-cts in air
//							Scale factor for new image
//							flag: 1 for calculating T, 2 for calculating [CH4], else: xi
{
	int i,nt,nc;
    int ray0cts=0; int dep0cts=0; int imagescale=0; int Tswitch;
	DATAWORD *raypoint,*deppoint;
	unsigned int  raysize;
	TWOBYTE	rayxsize,rayysize;	
	float dep,ray;
    float rayf, depf;
    float ray0ctsf, dep0ctsf, imagescalef;
	float oneoverT, xi_low, xi_high;
    extern char    cmnd[],*fullname();


	
	/* copy current data into the rayleigh array */
	raysize = header[NCHAN]*header[NTRAK];
	rayxsize = header[NCHAN];
	rayysize = header[NTRAK];
	
	raypoint=deppoint=0;
		
	raypoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(raypoint == 0) {
		nomemory();
		goto cleanup;
	}
	

	for(i=0; i<raysize; i++) 
		*(raypoint + i) = *(datpt+i+doffset);
	
	/* Get the Depolarized image and normalizations*/	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%s %d %d %d %d",txt,&ray0cts,&dep0cts,&imagescale,&Tswitch);
			break;
		}
	}
	
	// the normalizations will be needed as float
	ray0ctsf = (float)(ray0cts);
	dep0ctsf = (float)(dep0cts);
	imagescalef = (float)(imagescale);
	
			
	fullname(txt,GET_DATA);
	getfile (0,-1);			/* get the file */

	if( (rayxsize != header[NCHAN]) || (rayysize != header[NTRAK]) ) {
			beep();
			printf("Image sizes are different. \n");
			goto cleanup;
	}
	
	/* copy current data into the fuel array */
	deppoint = (DATAWORD*) malloc(raysize*DATABYTES);

	if(deppoint == 0) {
		nomemory();
		goto cleanup;
	}

	for(i=0; i<raysize; i++) {
		*(deppoint + i) = *(datpt+i+doffset);
		//test = *(deppoint + i);
	}
		
			
	/* now do the calcs for each point */
	i = 0;
	
	for( nt = 0; nt<header[NTRAK]; nt++) {
		
		for( nc = 0; nc<header[NCHAN]; nc++) {
		
			//	normalize pixel to be 1 in air
			rayf = (float)*(raypoint+i);
			depf = (float)*(deppoint+i);
			
			ray = rayf/ray0ctsf;	
			dep = depf/dep0ctsf;
				
			
			//1/(T/1000K) = a1 + a2*Ir + a3*Id + a4/Ir + a5/Id + a6/(Ir+Id) Ê (fits *very* well) Ê 
			//a=(7.695037554e-02 2.361626067e+00 9.318855620e-01 Ê-8.831991300e-02 Ê -1.704867976e-02 1.818799108e-01) 
			//and then T = 1000K / (1/(T/1000K))
			
			//	[CH4]/(mol/m^3) = a1 + a2*Ir + a3*Id + a4/Ir + a5/Id + a6/(Ir+Id) 
			//a=(-7.916955149e-01 1.067495418e+01 Ê-9.941906818e+00 3.100520406e-01 8.533883695e-02 Ê-6.799942879e-01) 
			
			
			//xi =	  a1 + a2*Ir + a3*Id + a4/Ir + a5/Id + a6/(Ir+Id) 
			//		+ a7*(Ir*Id) + a8/(IR*ID) + a9*Ir^2 + a10*Id^2 + a11/Ir^2 + a12/Id^2 
			//a =	(4.622546534e-01	1.172493583e+00 Ê	-1.752052053e+00 Ê	-9.937429481e-01 Ê -1.305955703e-01		1.909804080e+00		
			//		 3.287928154e-01	-6.497956090e-02 Ê	-3.977468052e-01	3.388764314e-01		7.005207056e-02		1.562641070e-02) 
			//But the xi-images do not always look really nice and smooth
			
			if (Tswitch == 1) {
				oneoverT = 7.695037554e-02 + 2.361626067*ray + 9.318855620e-01*dep - 8.831991300e-02/ray  - 1.704867976e-02/dep + 1.818799108e-01/(ray+dep);
				*(datpt+(i++)+doffset) = (int)(imagescale*(1000/oneoverT));
			}
			
			else if (Tswitch == 2) {
				*(datpt+(i++)+doffset) = (int)(imagescale*(-7.916955149e-01 + 1.067495418e+01*ray - 9.941906818*dep + 3.100520406e-01/ray + 8.533883695e-02/dep - 6.799942879e-01/(ray+dep)));
			}
			
			else {
				// broken up here in lower and higher order to keep things under controll
				xi_low = 4.622546534e-01 + 1.172493583*ray - 1.752052053*dep - 9.937429481e-01/ray  - 1.305955703e-01/dep + 1.909804080/(ray+dep);
				xi_high = 3.287928154e-01*(ray*dep) - 6.497956090e-02/(ray*dep) - 3.977468052e-01*(ray*ray) + 3.388764314e-01*(dep*dep)  + 7.005207056e-02/(ray*ray) + 1.562641070e-02/(dep*dep);
				*(datpt+(i++)+doffset) = (int)(imagescale*(xi_low+xi_high));
			}
		
		}
	}

cleanup:

	if(raypoint!=0) {
		free(raypoint);	
		raypoint = 0;
	}
	if(deppoint!=0) {
		free(deppoint);	
		deppoint = 0;
	}
	have_max = 0;
	setarrow();
	return 0;
}

/*
extern int which_ex_buffer;
extern int exe_line_number[];
extern short executedepth[];
extern short loopdepth;

int execut_a_macfile(char filenamebuf[])
{
	extern int exflag,exptr[],exval[];
	extern char* exbuf[];
	extern char cmnd[],*fullname();
	
	int fd,nread,i;
	
	which_ex_buffer++;  // next buffer
	
	if(exbuf[which_ex_buffer] == 0) 
		exbuf[which_ex_buffer] =  calloc(MBUFLEN,1);
	else
		for(i=0; i< MBUFLEN; i++) *(exbuf[which_ex_buffer]+i)=0;	// clear buffer
	
	fd = open(fullname(filenamebuf,MACROS_DATA),READMODE);
	
	if(fd == -1) {
		beep();
		printf("Execute File '%s' Not Found.\n",filenamebuf);
		//free(exbuf[which_ex_buffer]);
		which_ex_buffer--;
		return -1;
	}
	nread = read(fd,exbuf[which_ex_buffer],MBUFLEN);
	printf("%d Bytes Read.\n",nread);
	
 	// the format of macro files has changed -- now they are formatted text files 
	// previously, they were constant length files containing C strings 
	// this code should read both formats 
	
	for(i=0; i<nread ; i++) {
		if( *(exbuf[which_ex_buffer]+i) == 0x0D || *(exbuf[which_ex_buffer]+i) == 0x0A )
			*(exbuf[which_ex_buffer]+i) = 0x00;		// change CR or LF to null 
	}	
	*(exbuf[which_ex_buffer]+nread) = 0;				// one extra to signify end of buffer 
	*(exbuf[which_ex_buffer]+nread+1) = 0;				// one more in case there was no <cr><lf> at end of macro
	
	for(i=0; i<nread ; i++) {
		if( *(exbuf[which_ex_buffer]+i) == 0x0){  // end of line
			if( *(exbuf[which_ex_buffer]+i+1) == 0x0) { // second 0 means end of execute text
				*(exbuf[which_ex_buffer]+i+1) = ';';	// end each execute command with a null command
				*(exbuf[which_ex_buffer]+i+2) = 0x0;
				*(exbuf[which_ex_buffer]+i+3) = 0x0;
				break;
			}
		}
		
	}
	
	close(fd);
	
	exflag = exval[which_ex_buffer] = 1;
	exptr[which_ex_buffer] = 0;
	exe_line_number[which_ex_buffer] = 0;
	executedepth[which_ex_buffer] = loopdepth;
	return 0;
}
*/


//***************************************************************// 
//**   TESTER - Development testing Function                           *//
//***************************************************************//
int
tester (int n, int index)	
{	
	extern char unit_text[];

	printf("0:header[0] = %d, %d\n", header[0], *(datpt) );
	printf("1:header[NMAX] = %d, %d\n", header[NMAX], *(datpt+ 1) );
	printf("2:header[LMAX] = %d, %d\n", header[LMAX], *(datpt+2) );
	printf("3:header[NMIN] = %d, %d\n", header[NMIN], *(datpt+3) );
	printf("4:header[LMIN] = %d, %d\n", header[LMIN], *(datpt+4) );
	printf("5:header[NFRAM] = %d, %d\n", header[NFRAM], *(datpt+5) );
	printf("6:header[NPTS] = %d, %d\n", header[NPTS], *(datpt+6) );
	printf("7:header[NCHAN] = %d, %d\n", header[NCHAN], *(datpt+7) );
	printf("8:header[NTRAK] = %d, %d\n", header[NTRAK], *(datpt+8) );
	printf("9:header[NTIME] = %d, %d\n", header[NTIME], *(datpt+9) );
	printf("10:header[NPREP] = %d, %d\n", header[NPREP], *(datpt+10) );
	printf("11:header[NX0] = %d, %d\n", header[NX0], *(datpt+11) );
	printf("12:header[NY0] = %d, %d\n", header[NY0], *(datpt+12) );
	printf("13:header[NDX] = %d, %d\n", header[NDX], *(datpt+13) );
	printf("14:header[NDY] = %d, %d\n", header[NDY], *(datpt+14) );
	// What is in the trailer
	printf("0:trailer[FREESP] = %d\n", trailer[FREESP]);
	printf("1:trailer[IDWRDS] = %d\n", trailer[IDWRDS]);
	printf("2:trailer[RULER_CODE] = %d\n", trailer[RULER_CODE]);
	printf("3:trailer[RULER_SCALE] = %d\n", trailer[RULER_SCALE]);
	printf("4:trailer[4] = %d, %d\n", trailer[4]);
	printf("5-12:trailer[RULER_UNITS] = %s\n", strcpy(unit_text, (char*) &trailer[RULER_UNITS]));
	printf("13:trailer[RUNNUM] = %d\n", trailer[RUNNUM]);
	printf("14:trailer[TOMA2] = %d\n", trailer[TOMA2]);
	printf("15:trailer[IS_COLOR] = %d\n", trailer[IS_COLOR]);
	printf("16:trailer[16] = %d\n", trailer[16]);
	printf("17:trailer[SFACTR] = %d\n", trailer[SFACTR]);
	printf("18:trailer[NDATE] = %d\n", trailer[NDATE]);
	printf("19:trailer[19] = %d\n", trailer[19]);
	printf("20:trailer[20] = %d\n", trailer[20]);
	printf("21:trailer[DMODE] = %d\n", trailer[DMODE]);
	printf("22:trailer[NDATW] = %d\n", trailer[NDATW]);
	printf("23:trailer[SAMT] = %d\n", trailer[SAMT]);
	printf("24:trailer[SUBFC] = %d\n", trailer[SUBFC]);
	printf("25:trailer[NREAD] = %d\n", trailer[NREAD]);
	printf("26:trailer[LSYFG] = %d\n", trailer[LSYFG]);
	printf("27:trailer[COLFG] = %d\n", trailer[COLFG]);
	printf("28:trailer[NDISF] = %d\n", trailer[NDISF]);
	printf("29:trailer[NDELY] = %d\n", trailer[NDELY]);
	printf("30:trailer[ACSTAT] = %d\n", trailer[ACSTAT]);
	return OMA_OK;
}
//******************************************************************************// 
//**   POLARN - Custom function by pkalt used for polaristion processing       *//
//******************************************************************************//
int
polarn (int n, int index)	
{	
	int nc,nt;
	Image NumSamp;
	Image Result;
	Image Im_Buff;
	Image T0_Buff;
	int width, height;
	DATAWORD numerator, denominator;
	DATAWORD p_ratio, tempval;
	
	// Copy the denominator  from the file in Temp_Image = 0
	if(SDK_Image_CopyTemp(&T0_Buff, 0)!=OMA_OK){
		beep();
		printf("Need to have the denominator in temp buffer 0\n");
		return OMA_MEMORY;
	}
	
	// Create an image with from the file in Buffer
	if(SDK_Image_CopyBuffer(&Im_Buff)!=OMA_OK){
		beep();
		printf("SDK_Image_CopyBuffer() returned an error\n");
		return OMA_MEMORY;
	}
	// Get the dimensions of this image for the other new ones
	width = header[NCHAN];
	height = header[NTRAK];
	
	// Create new image structures for Result
	if(SDK_Image_Create(&Result, width, height)!=OMA_OK){
		beep();
		printf("SDK_Image_Create() returned an error\n");
		return OMA_MEMORY;
	}
	
	// If there is no existing NumSamp in temp buffer 2 create, otherwise use it
	if(SDK_Image_CopyTemp(&NumSamp, 2)!=OMA_OK){
		if (SDK_Image_Create(&NumSamp, width, height)!=OMA_OK){
			beep();
			printf("SDK_Image_Create() returned an error\n");
			return OMA_MEMORY;			
		}
	}
	
	// For each row and column process each pixel in turn...
	for(nt=0; nt<height; nt++) {
		for (nc =0; nc<width; nc++){
			numerator = SDK_Image_GetPixel(&Im_Buff, nc, nt);
			denominator = SDK_Image_GetPixel(&T0_Buff, nc, nt);
			if (denominator!=0){
				p_ratio = numerator/denominator;
				SDK_Image_SetPixel(&Result, nc, nt, p_ratio);
				tempval = SDK_Image_GetPixel(&NumSamp, nc, nt);
				SDK_Image_SetPixel(&NumSamp, nc, nt, tempval+1);
			} else {
				SDK_Image_SetPixel(&Result, nc, nt, (DATAWORD)1.000000);
			}
		}
	}
	
	// Once done, get ready to leave
	SDK_Image_2Buffer(&Result);
	//SDK_Image_2Temp(&Result, 1);
	SDK_Image_2Temp(&NumSamp, 2);
	
	// Free the old Image data structures
	SDK_Image_Free(&Im_Buff);
	SDK_Image_Free(&Result);
	SDK_Image_Free(&T0_Buff);
	SDK_Image_Free(&NumSamp);
	
	SDK_Image_Refresh();
	return OMA_OK;
}	

#ifdef Mac
//***************************************************************// 
//**   TEST2 - Development testing Function                           *//
//***************************************************************//
 
int
test2 (int n, int index)	
{	

	float x0=0,y0=0,x1=0,y1=0,scale=5;
	
	int ulx,uly,nc,nt;
	int height, width;
	
	
	
	if(no_of_velx==0) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	
	width = (no_of_velx-1)*boxinc+fftsize;
	height = (no_of_vely-1)*boxinc+fftsize;
	
	x1 =  (no_of_velx-1)*boxinc+fftsize;
	y1 = (no_of_vely-1)*boxinc+fftsize;
	
	if(SDK_Window_Create(width, height, "Piatra Rocks!")!=OMA_OK ) {
		beep();
		return OMA_MISC;
	}
	
	uly = ulx = 0;
	
	SDK_Window_SetRGB(0.5,0.0,0.0); //Red
	
	/* get the next subarray of the current image */
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			x0 = nc*boxinc +fftsize/2;
			y0 = nt*boxinc + fftsize/2;
			
			x1 = x0 + scale* PKGetVectorX(nc, nt);
			y1 = y0 + scale* PKGetVectorY(nc, nt);
			
			SDK_Window_DrawLine(x0, y0, x1, y1);
		}
	}
	SDK_Window_DrawLine(0.0, 0.0, x0, y0);
	SDK_Window_DrawLine(50.0, 50.0, 100.0, 50.0);
	SDK_Window_DrawLine(0.0, 0.0, (float) width, (float) height);
	SDK_Window_DrawLine(100.0, 100.0, 50.0, 100.0);
	SDK_Window_DrawLine(50.0, 100.0, 50.0, 50.0);
	
	SDK_Window_Update();
	SDK_Window_Release();

	return OMA_OK;
	
}
#endif

// Function prototypes for a couple of functions called here but actually located elsewhere
int printxyzstuff();
extern int stopstatus;
//****************************************************************************
//     P K C O R R - perform X-corr of image in buffer with in Temp Buffer 1
//      based on XPIV in custom.c
//****************************************************************************/
int pkcorr(int n, int index)
{
	Image          Img1, Img2;
	Image          ROI_1, ROI_2, ROI_XC;
	Field             PIVpeak1, PIVpeak2, PIVpeak3, PIVpeak4, PIVpeak5;
	int                FieldSizeX, FieldSizeY;
	int                Peak_Loc_X, Peak_Loc_Y;    // Holds the position of the local peak pixel
	float             Peak_Strength;
	float             Subpix_X, Subpix_Y;           // Holds the exact position of the peak centroid
	Vector          nullVect = {0,0,0,0};
	Vector          tVect = {99,99,99,22};
	short             nc,nt;
	DATAWORD  ROI_min, ROI_max;
	DATAWORD  temp_pixel;
	DATAWORD	loval, hival;		// The low and high clip values based on PIVSET
	int                 fftheight, fftwidth;	// number of pixels in PIV sub-region
	int		i, j;	                    // loop counters
	int		ulx, uly;	          // upper left corner X and Y
	int		mystopstatus;	// Interupt state
	
	// copy current data into the piv1 array
	if (SDK_Image_CopyBuffer(&Img1)!=OMA_OK) return OMA_MISC;
	if (SDK_Image_CopyTemp(&Img2, 1)!=OMA_OK) return OMA_NOEXIST;
	
	// Check both images are the same size
	if ( (Img1.width != Img2.width ) || ( Img1.height != Img2.height) ) {
		beep();
		printf("Both images need to be the same size\n");
		return OMA_MISC;
	}
	
	// Calculate how many vectors will be found
	FieldSizeX = (Img1.width-fftsize)/boxinc +1;
	FieldSizeY = (Img1.height-fftsize)/boxinc +1;
	pprintf("%d by %d\n",FieldSizeX,FieldSizeY);
	
	// Make a FIELD structure the right size
	if (SDK_Field_Create(&PIVpeak1, FieldSizeX, FieldSizeY)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak2, FieldSizeX, FieldSizeY)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak3, FieldSizeX, FieldSizeY)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak4, FieldSizeX, FieldSizeY)) return OMA_MEMORY;
	if (SDK_Field_Create(&PIVpeak5, FieldSizeX, FieldSizeY)) return OMA_MEMORY;
	
	// Set PIV sub-region dimension variables
	fftheight = fftsize;
	fftwidth = fftsize;
	//FFT_area = fftwidth * fftheight;
	//n2 = fftsize;
		
	// Create some working memory
	if (SDK_Image_Create(&ROI_1, fftwidth, fftheight)!=OMA_OK) return OMA_MEMORY;
	if (SDK_Image_Create(&ROI_2, fftwidth, fftheight)!=OMA_OK) return OMA_MEMORY;
	if (SDK_Image_Create(&ROI_XC, fftwidth, fftheight)!=OMA_OK) return OMA_MEMORY;
	
	// Set up the Interrupt variable
	mystopstatus = stopstatus;

	// Main Loop for each Subregion
	nc = nt = 0;  // These are position in vector field
	for(uly = 0; uly <= header[NTRAK] - fftheight; uly += boxinc){
		for(ulx = 0; ulx <= header[NCHAN] - fftwidth; ulx += boxinc){
			
			// Check for Interrupt status - bug out NOW if we want to stop
			checkstoprequest();
			if (mystopstatus != stopstatus) break;
			
			// Print the location of this box in status window
			printxyzstuff(ulx,uly,0);
			printxyzstuff(ulx+fftsize-1,uly+fftsize-1,1);
			
			// Check that we are still inside Image region after applying shiftx/shifty
			if (ulx+shiftx < 0 || uly+shifty < 0
				|| ulx+shiftx+fftwidth > header[NCHAN] 
				|| uly+shifty+fftheight > header[NTRAK]){
				// In this case we have a problem... set all parts of this vector to 0 and break
				SDK_Field_SetVector(&PIVpeak1, nc, nt, &nullVect);
				SDK_Field_SetVector(&PIVpeak2, nc, nt, &nullVect);
				SDK_Field_SetVector(&PIVpeak3, nc, nt, &nullVect);
				SDK_Field_SetVector(&PIVpeak4, nc, nt, &nullVect);
				SDK_Field_SetVector(&PIVpeak5, nc, nt, &nullVect);
				nc++;
				break;
			}
			
			// Copy subregions into our Working Image structures (fftsize'd) ROI-1&2
			for(i=0; i<fftwidth; i++) {
				for(j=0; j<fftheight; j++) {
					SDK_Image_SetPixel(&ROI_1, i, j, 
									   SDK_Image_GetPixel(&Img1, ulx+i, uly+j) );
					SDK_Image_SetPixel(&ROI_2, i, j, 
									   SDK_Image_GetPixel(&Img2, ulx+i+shiftx, uly+j+shifty) );
				}
			}
			
			// Find the Min/Max values and apply clipping if that is what we need to do
			if (autoclip){
				// ROI_1
				loval = SDK_Image_GetFractionValue(&ROI_1, loclip);
				hival = SDK_Image_GetFractionValue(&ROI_1, hiclip);
				SDK_Image_Clip(&ROI_1, hival);
				SDK_Image_Clop(&ROI_1, loval);
				// ROI_2
				loval = SDK_Image_GetFractionValue(&ROI_2, loclip);
				hival = SDK_Image_GetFractionValue(&ROI_2, hiclip);
				SDK_Image_Clip(&ROI_2, hival);
				SDK_Image_Clop(&ROI_2, loval);
			}
			
			// Cross-correlation between ROI_1 & ROI_2 = ROI_XC
			SDK_Image_XCorr(&ROI_1, &ROI_2, &ROI_XC);
			
			// Scale the result to use more of the dynamic range MAXDATAVALUE
			ROI_max = SDK_Image_GetMaxPixel(&ROI_XC);
			ROI_min = SDK_Image_GetMinPixel(&ROI_XC);
			for(i=0; i<fftwidth; i++) {
				for(j=0; j<fftheight; j++) {
					temp_pixel = (SDK_Image_GetPixel(&ROI_XC, i, j) - ROI_min);
					temp_pixel *= (DATAWORD) (MAXDATAVALUE/(ROI_max - ROI_min));
					SDK_Image_SetPixel(&ROI_XC, i, j, temp_pixel);
				}
			}
			
			
			// .... A    Copy ROI_XC to ROI_1 so we can make changes
			SDK_Image_CopyImage(&ROI_XC, &ROI_1);
			
			// For each peak (in descending order) up until "peaks_to_save"...
			for (i=1; i<peaks_to_save; i++){
				// .... B    Find the location of maximum pixel in the Cross-correlation (ROI_1)
				Peak_Loc_X = SDK_Image_GetMaxPixelX(&ROI_1);
				Peak_Loc_Y = SDK_Image_GetMaxPixelY(&ROI_1);
				Peak_Strength = (float) SDK_Image_GetPixel(&ROI_1, Peak_Loc_X, Peak_Loc_Y);
				// .... C    Find the sub-pixel peak location using estimators (3pt centroid/Gaussian/Parabolic/etc).
				Subpix_X = SDK_Image_3PtEstimatorX(&ROI_1, Peak_Loc_X, Peak_Loc_Y);
				Subpix_Y = SDK_Image_3PtEstimatorY(&ROI_1, Peak_Loc_X, Peak_Loc_Y);
				// .... D    Write the values of this peak into a Vector
				SDK_Vector_SetX(&tVect, (float)(Subpix_X-fftwidth/2));
				SDK_Vector_SetY(&tVect, (float)(Subpix_Y -fftheight/2));
				SDK_Vector_SetZ(&tVect, (float)0.0);
				SDK_Vector_SetPeak(&tVect, Peak_Strength);
				//... and drop it into the right Field
				if (i==1) SDK_Field_SetVector(&PIVpeak1, nc, nt, &tVect);
				if (i==2) SDK_Field_SetVector(&PIVpeak2, nc, nt, &tVect);
				if (i==3) SDK_Field_SetVector(&PIVpeak3, nc, nt, &tVect);
				if (i==4) SDK_Field_SetVector(&PIVpeak4, nc, nt, &tVect);
				if (i==5) SDK_Field_SetVector(&PIVpeak5, nc, nt, &tVect);
				// .... E    Set the 3x3 pixel region around "Peak_Loc" to zero and repeat.
				SDK_Image_3PtKill(&ROI_1, Peak_Loc_X, Peak_Loc_Y);
			}
			nc++;
		}
		nc = 0;
		nt++;
	}
	
	// Copy the Field structures to the OMA PIV buffer
	SDK_Field_2Buffer(&PIVpeak1);
	SDK_Field_2NthPeak(&PIVpeak2, 2);
	SDK_Field_2NthPeak(&PIVpeak3, 3);
	SDK_Field_2NthPeak(&PIVpeak4, 4);
	SDK_Field_2NthPeak(&PIVpeak5, 5);
	
	// Free Field Memory structures
	SDK_Field_Free(&PIVpeak1);
	SDK_Field_Free(&PIVpeak2);
	SDK_Field_Free(&PIVpeak3);
	SDK_Field_Free(&PIVpeak4);
	SDK_Field_Free(&PIVpeak5);
	
	// Free Image Memory structures
	SDK_Image_Free(&ROI_1);
	SDK_Image_Free(&ROI_2);
	SDK_Image_Free(&ROI_XC);
		
	return OMA_OK;
} 

int 
eightPtFill(Image *src, short xloc, short yloc, DATAWORD flag, DATAWORD val)
{
	short Is_Part_of_Object = FALSE;	
	
	if (SDK_Image_GetPixel(src, xloc, yloc)==flag  )
		Is_Part_of_Object = TRUE;
	
	if (Is_Part_of_Object) {
		// Set the pixel to 'val' and recurse for neighbours....
		SDK_Image_SetPixel(src, xloc, yloc,val);
		eightPtFill(src, xloc+1, yloc, flag, val);
		eightPtFill(src, xloc-1, yloc, flag, val);
		eightPtFill(src, xloc, yloc+1, flag, val);
		eightPtFill(src, xloc, yloc-1, flag,val);
		eightPtFill(src, xloc+1, yloc+1,flag, val);
		eightPtFill(src, xloc-1, yloc-1, flag,val);
		eightPtFill(src, xloc+1, yloc-1,flag, val);
		eightPtFill(src, xloc-1, yloc+1,flag, val);
	}
	
	return OMA_OK;
}


//***************************************************************// 
//**   COUNT - Count number of distinct objects above background             *//
//***************************************************************//
int
count (int n, int index)	
{	
	Image original, objects;
	short nc, nt;
	int i;
	short height, width;
	DATAWORD         num_obj=0;
	DATAWORD         marker=-78;
	
	// Create an Image with new dimensions
	if(SDK_Image_CopyBuffer(&original)!=OMA_OK){
		beep();
		printf("SDK_Image_CopyBuffer() returned an error\n");
		return OMA_MEMORY;
	}
	
	width = original.width;
	height = original.height;
	if (SDK_Image_Create(&objects, width, height)!=OMA_OK){
		beep();
		printf("SDK_Image_Create() returned an error\n");
		return OMA_MEMORY;
	}
	
	// For each pixel in the original
	for(nt=0; nt<height; nt++) {
		for (nc =0; nc<width; nc++){
			// ... if pixel is above threshold, ID it as art of object
			if(SDK_Image_GetPixel(&original, nc, nt)>0)
					SDK_Image_SetPixel(&objects, nc, nt, marker);

		}
	}
	
	// For each pixel in object image
	for(nt=0; nt<height; nt++) {
		for (nc =0; nc<width; nc++){
			// If we find a pixel that has -1. ie an object not yet counted
			if (SDK_Image_GetPixel(&objects, nc, nt) == marker){
		//		SDK_Image_SetPixel(&objects, nc , nt, ++num_obj);
				eightPtFill(&objects, nc , nt, marker,++num_obj);
			}
		}
	}	
	
	// Do stuff to each object if you like....
	//printf("OMA has found %d objects\n", (int) num_obj);
	for (i=1; i<=(int)num_obj; i++){
		//printf("[%d]: Area=%f\tCentroid=(%f, %f)\n", (int) num_obj,nc,nt );
	}
	
	// clean up and go
	SDK_Image_2Buffer(&objects);
	SDK_Image_Free(&objects);
	SDK_Image_Free(&original);
	SDK_Image_Refresh();
	return OMA_OK;
}
//***************************************************************// 
//**   NAN2ZERO_PIV - Search all PIV peaks for nan's and convert    *//
//***************************************************************//
int
nan2zero_piv ()	
{	
	short nc, nt;
	// 2. Search PIV Buffer for nan
	// 2.1 Create a Field with new dimensions for each peak
	Field priginal;
	int   which_peak;
	for (which_peak=1; which_peak<peaks_to_save;which_peak++){
		if(SDK_Field_CopyNthPeak(&priginal,which_peak)!=OMA_OK){
			beep();
			printf("SDK_Field_CopyBuffer() returned an error\n");
			return OMA_MEMORY;
		}
		
		// 2.2 Get the Image size for our loop
		short height, width;
		width = priginal.width;
		height = priginal.height;
		
		// 2.3 Loop for each pixel in the original
		int found_nan = FALSE;
		Vector   *tVect;
		for(nt=0; nt<height; nt++) {
			for (nc =0; nc<width; nc++){
				// Get a Vector
				if ((tVect = SDK_Field_GetVector(&priginal, nc, nt)) !=NULL){
					if (isnan(SDK_Vector_GetX(tVect))||isnan(SDK_Vector_GetY(tVect))||
						isnan(SDK_Vector_GetZ(tVect))||isnan(SDK_Vector_GetPeak(tVect))){
						SDK_Vector_SetX(tVect,0.00000);
						SDK_Vector_SetY(tVect,0.00000);
						SDK_Vector_SetZ(tVect,0.00000);
						SDK_Vector_SetPeak(tVect,0.00000);
						found_nan = TRUE;
					}
				}
			}
		}
		
		// 2.4 Swap for actual image only if a nan was found
		if (found_nan) {
			printf("We have found a 'nan' in the PIV peak[%d]\n", which_peak);
			SDK_Field_2NthPeak(&priginal, which_peak);
		}
		
		// 2.5 Free the Pointer for the next iteration
		SDK_Field_Free(&priginal);
	}
	
	// clean up and go
	return OMA_OK;
}
//***************************************************************// 
//**   NAN2ZERO_TEMP - Search all temp buffers for nan's and convert    *//
//***************************************************************//
int
nan2zero_temp (int which_tbuff)	
{	
	short nc, nt;
	// 3. Search Image (TEMP 0-9) Buffers for nan
	// 3.1 Create an Image for each TEMP buffer
	Image briginal;
	if(SDK_Image_CopyTemp(&briginal,which_tbuff)==OMA_OK){
		// 3.2 Get the Image size for our loop
		short height, width;
		width = briginal.width;
		height = briginal.height;
		
		// 3.3 Loop for each pixel in the original
		int found_nan = FALSE;
		for(nt=0; nt<height; nt++) {
			for (nc =0; nc<width; nc++){
				if(isnan(SDK_Image_GetPixel(&briginal, nc, nt))){
					SDK_Image_SetPixel(&briginal, nc, nt, (DATAWORD)0);
					found_nan = TRUE;
				}
			}
		}
		// 3.4 Swap for actual image only if a nan was found
		if (found_nan) {
			printf("We have found a 'nan' in the Temp[%d]\n", which_tbuff);
			SDK_Image_2Temp(&briginal, which_tbuff);
		}
		
		// 3.5 Free the pointer for the next iteration
		SDK_Image_Free(&briginal);
		
	}
	// clean up and go
	return OMA_OK;
}

//***************************************************************// 
//**   NAN2ZERO_IMAGE - Search all buffer for nan's and convert    *//
//***************************************************************//
int
nan2zero_image ()	
{	
	short nc, nt;

	// 1. Search Image (DATA) Buffer for nan
	// 1.1 Create an Image with new dimensions
	Image original;
	if(SDK_Image_CopyBuffer(&original)!=OMA_OK){
		beep();
		printf("SDK_Image_CopyBuffer() returned an error\n");
		return OMA_MEMORY;
	}
	
	// 1.2 Get the Image size for our loop
	short height, width;
	width = original.width;
	height = original.height;
	
	// 1.3 Loop for each pixel in the original
	int found_nan = FALSE;
	for(nt=0; nt<height; nt++) {
		for (nc =0; nc<width; nc++){
			// ... if pixel is nan, then fix it
			if(isnan(SDK_Image_GetPixel(&original, nc, nt))){
				SDK_Image_SetPixel(&original, nc, nt, (DATAWORD)0);
				found_nan = TRUE;
			}
		}
	}
	
	// 1.4 Swap for actual image only if a nan was found
	if (found_nan) {
		printf("We have found a 'nan' in the Image buffer\n");		
		SDK_Image_2Buffer(&original);
	}
	
	// 1.5 Free the pointer at this point, eh?
	SDK_Image_Free(&original);
	
	
	// clean up and go
	return OMA_OK;
}

//***************************************************************// 
//**   NAN2ZE - Search all memory structures for nan's and convert    *//
//***************************************************************//
int
nan2zero (int n, int index)	
{	
	nan2zero_image();
	
	if (no_of_velx !=0) nan2zero_piv();
	
	int   which_tbuff;
	for (which_tbuff=0; which_tbuff<NUM_TEMP_IMAGES; which_tbuff++){
		if(temp_dat[n] != 0){
			nan2zero_temp(which_tbuff);
		}
	}
	
	SDK_Image_Refresh();
	return OMA_OK;
}

//____________COMMAND HANDLE________
//{{"PIVUODETECT    "}, piv_universal_outlier_detection},
//____________CODE FOLLOWS____________

/* This is some code that I got off the net to find medians quickly from a fixed array size.
Needed for PIVUODETECT but generally useful too.
*/
typedef float pixelvalue ;
#define PIX_SORT(a,b) { if ((a)>(b)) PIX_SWAP((a),(b)); }
#define PIX_SWAP(a,b) { pixelvalue temp=(a);(a)=(b);(b)=temp; }
pixelvalue opt_med8(pixelvalue * p)
{
    pixelvalue result;
    PIX_SORT(p[1], p[2]) ; PIX_SORT(p[4], p[5]) ; 
    PIX_SORT(p[0], p[1]) ; PIX_SORT(p[3], p[4]) ; PIX_SORT(p[6], p[7]) ;
    PIX_SORT(p[1], p[2]) ; PIX_SORT(p[4], p[5]) ; 
    PIX_SORT(p[0], p[3]) ; PIX_SORT(p[4], p[7]) ;
    PIX_SORT(p[3], p[6]) ; PIX_SORT(p[1], p[4]) ; PIX_SORT(p[2], p[5]) ;
    PIX_SORT(p[4], p[7]) ; PIX_SORT(p[4], p[2]) ; PIX_SORT(p[6], p[4]) ;
    PIX_SORT(p[4], p[2]) ; result = (p[3]+p[4])/2; return(result);
}
pixelvalue opt_med24(pixelvalue * p)
{
    pixelvalue result;
    PIX_SORT(p[0], p[1]) ;    PIX_SORT(p[3], p[4]) ;   PIX_SORT(p[2], p[4]) ;
    PIX_SORT(p[2], p[3]) ;    PIX_SORT(p[6], p[7]) ;   PIX_SORT(p[5], p[7]) ;
    PIX_SORT(p[5], p[6]) ;    PIX_SORT(p[9], p[10]) ;  PIX_SORT(p[8], p[10]) ;
    PIX_SORT(p[8], p[9]) ;    PIX_SORT(p[12], p[13]) ; PIX_SORT(p[11], p[13]) ;
    PIX_SORT(p[11], p[12]) ; PIX_SORT(p[15], p[16]) ; PIX_SORT(p[14], p[16]) ;
    PIX_SORT(p[14], p[15]) ; PIX_SORT(p[18], p[19]) ; PIX_SORT(p[17], p[19]) ;
    PIX_SORT(p[17], p[18]) ; PIX_SORT(p[21], p[22]) ; PIX_SORT(p[20], p[22]) ;
    PIX_SORT(p[20], p[21]) ; PIX_SORT(p[2], p[5]) ;
    PIX_SORT(p[3], p[6]) ;    PIX_SORT(p[0], p[6]) ;   PIX_SORT(p[0], p[3]) ;
    PIX_SORT(p[4], p[7]) ;    PIX_SORT(p[1], p[7]) ;   PIX_SORT(p[1], p[4]) ;
    PIX_SORT(p[11], p[14]) ; PIX_SORT(p[8], p[14]) ;  PIX_SORT(p[8], p[11]) ;
    PIX_SORT(p[12], p[15]) ; PIX_SORT(p[9], p[15]) ;  PIX_SORT(p[9], p[12]) ;
    PIX_SORT(p[13], p[16]) ; PIX_SORT(p[10], p[16]) ; PIX_SORT(p[10], p[13]) ;
    PIX_SORT(p[20], p[23]) ; PIX_SORT(p[17], p[23]) ; PIX_SORT(p[17], p[20]) ;
    PIX_SORT(p[18], p[21]) ;
    PIX_SORT(p[19], p[22]) ; PIX_SORT(p[8], p[17]) ;  PIX_SORT(p[9], p[18]) ;
    PIX_SORT(p[0], p[18]) ;  PIX_SORT(p[0], p[9]) ;   PIX_SORT(p[10], p[19]) ;
    PIX_SORT(p[1], p[19]) ;  PIX_SORT(p[1], p[10]) ;  PIX_SORT(p[11], p[20]) ;
    PIX_SORT(p[2], p[20]) ;  PIX_SORT(p[2], p[11]) ;  PIX_SORT(p[12], p[21]) ;
    PIX_SORT(p[3], p[21]) ;  PIX_SORT(p[3], p[12]) ;  PIX_SORT(p[13], p[22]) ;
    PIX_SORT(p[4], p[22]) ;  PIX_SORT(p[4], p[13]) ;  PIX_SORT(p[14], p[23]) ;
    PIX_SORT(p[5], p[23]) ;  PIX_SORT(p[5], p[14]) ; 
    PIX_SORT(p[6], p[15]) ;  PIX_SORT(p[7], p[16]) ;
    PIX_SORT(p[7], p[19]) ;  PIX_SORT(p[13], p[21]) ; PIX_SORT(p[15], p[23]) ;
    PIX_SORT(p[7], p[13]) ;  PIX_SORT(p[7], p[15]) ;  PIX_SORT(p[1], p[9]) ;
    PIX_SORT(p[3], p[11]) ;  PIX_SORT(p[5], p[17]) ;  PIX_SORT(p[11], p[17]) ;
    PIX_SORT(p[9], p[17]) ;  PIX_SORT(p[4], p[10]) ;  PIX_SORT(p[6], p[12]) ;
    PIX_SORT(p[7], p[14]) ;  PIX_SORT(p[4], p[6]) ;   PIX_SORT(p[4], p[7]) ;
    PIX_SORT(p[12], p[14]) ; PIX_SORT(p[10], p[14]) ; PIX_SORT(p[6], p[7]) ;
    PIX_SORT(p[10], p[12]) ; PIX_SORT(p[6], p[10]) ;  PIX_SORT(p[6], p[17]) ;
    PIX_SORT(p[12], p[17]) ; PIX_SORT(p[7], p[17]) ;  PIX_SORT(p[7], p[10]) ;
    PIX_SORT(p[12], p[18]) ; PIX_SORT(p[7], p[12]) ;  PIX_SORT(p[10], p[18]) ;
    PIX_SORT(p[12], p[20]) ; PIX_SORT(p[10], p[20]) ; PIX_SORT(p[10], p[12]) ;
    result = (p[11]+p[12])/2; return(result);
}
#undef PIX_SORT
#undef PIX_SWAP

//************************************************************************************//
//**    P I V U O D E T E C T   -   Univeral Outlier Detection by residual median.
//**                             Westerweel & Scarano (2005) Exp. Fluids [39]:1096-1100      *//
//************************************************************************************//
int
piv_universal_outlier_detection(int n, int index)	
{	
	float thresh;
	int radius;
	int i, j, k, nc, nt;
	int narg;
	int Err;
	Image Im_Outlier;
	float Epsilon = 0.1;
	
	// Check that the PIV image exists in the first place.
	if(no_of_velx == 0 ) {
		beep();
		printf("Must have valid PIV image first.\n");
		return OMA_NOEXIST;
	}
	// Read in command line arguments.
	narg = sscanf(&cmnd[index],"%f %d %f",&thresh, &radius, &Epsilon );
	if (narg < 3) Epsilon=0.1;
	if (narg < 2) radius=1;
	if (narg ==0) thresh = 2.0;
	
	// Limit the value of the neighbourhood radius 1=3x3;2=5x5
	if ((radius !=1) && (radius !=2))	radius = 1;
	
	// Create an IMAGE to hold outliers
	if((Err = SDK_Image_Create(&Im_Outlier, no_of_velx, no_of_vely)) != OMA_OK){
		beep();
		nomemory();
		return Err;
	}

	// For each velocity component
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			// Find the centre value
			float U, V, W;
			U = PKGetVectorX(nc,nt);
			V = PKGetVectorY(nc,nt);
			W =PKGetVectorZ(nc,nt);

			// Find median of neighbourhood
			float NeighMed_U,NeighMed_V,NeighMed_W;
			if (radius ==1) {
				float Median3x3_U[8];
				float Median3x3_V[8];
				float Median3x3_W[8];
				k=0;
				for (i=-radius; i<=radius; i++){
					for (j=-radius; j<=radius; j++){
						if ((i==0) && (j==0)) break;
						Median3x3_U[k] = PKGetVectorX(nc+i, nt+j);
						Median3x3_V[k] = PKGetVectorY(nc+i, nt+j);
						Median3x3_W[k] = PKGetVectorZ(nc+i, nt+j);
						k++;
					}
				}
				NeighMed_U = opt_med8(Median3x3_U);
				NeighMed_V = opt_med8(Median3x3_V);
				NeighMed_W = opt_med8(Median3x3_W);
			} else {
				float Median5x5_U[24];
				float Median5x5_V[24];
				float Median5x5_W[24];
				k=0;
				for (i=-radius; i<=radius; i++){
					for (j=-radius; j<=radius; j++){
						if ((i==0) && (j==0)) break;
						Median5x5_U[k] = PKGetVectorX(nc+i, nt+j);
						Median5x5_V[k] = PKGetVectorY(nc+i, nt+j);
						Median5x5_W[k] = PKGetVectorZ(nc+i, nt+j);
						k++;
					}
				}
				NeighMed_U = opt_med24(Median5x5_U);
				NeighMed_V = opt_med24(Median5x5_V);
				NeighMed_W = opt_med24(Median5x5_W);			
			}
	
			// Find the Median RESIDUAL r_i = U_i-Um
			float Resid_U, Resid_V, Resid_W;
			if (radius ==1) {
				float Median3x3_U[8];
				float Median3x3_V[8];
				float Median3x3_W[8];
				k=0;
				for (i=-radius; i<=radius; i++){
					for (j=-radius; j<=radius; j++){
						if ((i==0) && (j==0)) break;
						Median3x3_U[k] = fabs(PKGetVectorX(nc+i, nt+j) - NeighMed_U);
						Median3x3_V[k] = fabs(PKGetVectorY(nc+i, nt+j) - NeighMed_V);
						Median3x3_W[k] = fabs(PKGetVectorZ(nc+i, nt+j) - NeighMed_W);
						k++;
					}
				}
				Resid_U = opt_med8(Median3x3_U);
				Resid_V = opt_med8(Median3x3_V);
				Resid_W = opt_med8(Median3x3_W);
			} else {
				float Median5x5_U[24];
				float Median5x5_V[24];
				float Median5x5_W[24];
				k = 0;
				for (i=-radius; i<=radius; i++){
					for (j=-radius; j<=radius; j++){
						if ((i==0) && (j==0)) break;	
						Median5x5_U[k] = fabs(PKGetVectorX(nc+i, nt+j) - NeighMed_U);
						Median5x5_V[k] = fabs(PKGetVectorY(nc+i, nt+j) - NeighMed_V);
						Median5x5_W[k] = fabs(PKGetVectorZ(nc+i, nt+j) - NeighMed_W);
						k++;
					}
				}
				Resid_U = opt_med24(Median5x5_U);
				Resid_V = opt_med24(Median5x5_V);
				Resid_W = opt_med24(Median5x5_W);			
			}
			
			// If residual is too large then mark the vector as OUTLIER
			float NormFluct , r_U, r_V, r_W;
			r_U = fabs(U-NeighMed_U)/(Resid_U+Epsilon);
			r_V = fabs(V-NeighMed_V)/(Resid_V+Epsilon);
			r_W = fabs(W-NeighMed_W)/(Resid_W+Epsilon);
			NormFluct = sqrt(r_U * r_U + r_V * r_V + r_W * r_W);
			
			if (NormFluct > thresh) SDK_Image_SetPixel(&Im_Outlier, nc, nt, (DATAWORD)1.0);  
		}
	}
	
	// Go through and KILL the bad vectors
	for (nt=0; nt<no_of_vely; nt++){
		for (nc=0; nc<no_of_velx; nc++){
			if (SDK_Image_GetPixel(&Im_Outlier, nc, nt) == (DATAWORD)1.0){
				PKSetVectorX(nc, nt, 0.000);
				PKSetVectorY(nc, nt, 0.000);
				PKSetVectorZ(nc, nt, 0.000);
			}
		}
	}
	
	// Clean up and GO
	SDK_Image_Free(&Im_Outlier);
	SDK_Image_Refresh();
	return OMA_OK;
}

//***************************************************************//
//**    TSMOOTH - Smooth radius based on x_dim = Temp[0], y_dim = Temp[1]   *//
//***************************************************************//
int tsmooth()
{
    Image Im_Result;
    Image Im_dimX;
    Image Im_dimY;
    
    int nc, nt;
    int i,j,count;
    float sum;
    int dx, dy, dxs, dys;
    int height, width;
    
    // Allocate space for the result Image
    height = header[NTRAK];
    width = header[NCHAN];
    if(SDK_Image_Create(&Im_Result,header[NCHAN],header[NTRAK])!=OMA_OK){
        beep();
        printf("SDK_Image_Create returned an error\n");
        return OMA_MEMORY;
    }
    
    // Get Image Copy of the Temp Buffers
    if(SDK_Image_CopyTemp(&Im_dimX, 0)!=OMA_OK){
        beep();
        printf("Are you sure that Temp[0] holds the X smoothing diameter?\n");
        return OMA_MEMORY;
    }
    if(SDK_Image_CopyTemp(&Im_dimY, 1)!=OMA_OK){
        beep();
        printf("Are you sure that Temp[1] holds the Y smoothing diameter?\n");
        return OMA_MEMORY;
    }
    
    // Check everything for appropraite sizes
    if ((Im_dimX.width != width)||(Im_dimX.height!=height)||(Im_dimY.width != width)||(Im_dimY.height!=height)) {
        beep();
        printf("Please make sure that the Temporary buffers T[0] and T[1] are the same size as the image.\n");
        return OMA_RANGE;
    }
    
    // Loop through Original image and do appropriate smoothing
    for(nt=0; nt<header[NTRAK]; nt++) {
        for(nc=0; nc<header[NCHAN];nc++){
            // set counters to zero for new pixel
            sum = 0;
            count = 0;
            
            // Work out local smoothing size by reading from Temp buffers
            dx = floor(SDK_Image_GetPixel(&Im_dimX, nc, nt));
            dy = floor(SDK_Image_GetPixel(&Im_dimY, nc, nt));
            dxs = -dx/2;
            dys = -dy/2;
            if( dx & 0x1)   dx = dx/2+1;
            else		dx /= 2;
            if( dy & 0x1)	dy = dy/2+1;
            else		dy /= 2;
            
            // Calculate local average (unweighted rectangular smoothing)
            for(i=dxs; i<dx; i++) {
                for(j=dys; j<dy; j++) {
                    count++;
                    sum += PKGetPixel(nc+i,nt+j);
                }
            }
            SDK_Image_SetPixel(&Im_Result, nc, nt, (DATAWORD) sum/count);
        }
    }
    // Free up memory and GO
    SDK_Image_2Buffer(&Im_Result);
    SDK_Image_Free(&Im_Result);
    SDK_Image_Free(&Im_dimX);
    SDK_Image_Free(&Im_dimY);
    SDK_Image_Refresh();
    return OMA_OK;
}


//***************************************************************//
//**    HORISO - a custom command by PIATRA ENGINEERING   *//
//***************************************************************//
int horiso(int n, int index)
{
    Image Im_Result;
    Image Im_Triangles;
    
    float X_eye, Y_eye, Z_eye, Azimuth;
    float Bay_H, Bay_W;
    
    Vector V_Gravity;
    Vector V_Surface;
    Vector P_Eye;
    Vector P_TopLeftLouvre;
    Vector P_TopRightLouvre;
    Vector P_BottomLeftLouvre;
    Vector P_BottomRightLouvre;
    Vector V_Left;
    
    float Px, Py, Pz; // the eye at window centroid
    float Tx, Ty, Tz; // triangle vertex
    float Sx, Sy, Sz; // surface normal to louvre/window
    float Lx, Ly, Lz; // (Top) Left corner of louvre
    float Rx, Ry, Rz; // (Top) Right corner of louvre
    float Bx, By, Bz; // Bottom (Left) corner of louvre
    float Ix, Iy, Iz; // intercept
    float Tparam;
    float TparX, TparZ;
    
    float X1, Y1, X2, Y2, X3, Y3;
    Vector T_P1, T_P2, T_P3;
    
    int ignore, accept; // kind of a boolean flag
    int nt;
    int narg;
    int verbose = 1;
    int louvre_offset = 1;   // this is an important variable that scales to the drawing units...
    
    // Read in command line arguments
    narg = sscanf(&cmnd[index],"%f %f %f %f %f %f",&X_eye, &Y_eye, &Z_eye, &Azimuth, &Bay_H, &Bay_W );
    if (narg < 5) {Bay_H = 1000; Bay_W=1000;}
    if (narg ==0) {
        beep();
        printf("Please pass the right parameters\n");
        return OMA_RANGE;
    }
    
    // Compute the surface normal and gravity vectors.
    // GRAVITY VECTOR points straight down. Y is map north, X is Map east and Z is elevation.
    SDK_Vector_SetX(&V_Gravity,  0.000);
    SDK_Vector_SetY(&V_Gravity,  0.000);
    SDK_Vector_SetZ(&V_Gravity,  -1.000);
    // SURFACE NORMAL is a horizontal vector with a compass heading (Azimuth)
    SDK_Vector_SetX(&V_Surface, sin(pi*Azimuth/180));
    SDK_Vector_SetY(&V_Surface, cos(pi*Azimuth/180));
    SDK_Vector_SetZ(&V_Surface, 0.000);
    // EYE LOCATION (which represents the centroid of the actual Bay)
    SDK_Vector_SetX(&P_Eye, X_eye);
    SDK_Vector_SetY(&P_Eye, Y_eye);
    SDK_Vector_SetZ(&P_Eye, Z_eye);
    //  IN-PLANE LEFT - a vector pointing in the louvre plane that points left wrt view out window
    SDK_Vector_CrossProduct(&V_Left, &V_Surface, &V_Gravity);
    // TOP LEFT corner = centroid + louver offset* Surface + Left * Bay_W/2 - Bay_H/2*Gravity
    SDK_Vector_SetX(&P_TopLeftLouvre, X_eye+louvre_offset*SDK_Vector_GetX(&V_Surface) +Bay_W/2*SDK_Vector_GetX(&V_Left) -Bay_H/2*SDK_Vector_GetX(&V_Gravity) );
    SDK_Vector_SetY(&P_TopLeftLouvre, Y_eye+louvre_offset*SDK_Vector_GetY(&V_Surface) +Bay_W/2*SDK_Vector_GetY(&V_Left) -Bay_H/2*SDK_Vector_GetY(&V_Gravity) );
    SDK_Vector_SetZ(&P_TopLeftLouvre, Z_eye+louvre_offset*SDK_Vector_GetZ(&V_Surface) +Bay_W/2*SDK_Vector_GetZ(&V_Left) -Bay_H/2*SDK_Vector_GetZ(&V_Gravity) );
    // TOP RIGHT corner = centroid + louver offset* Surface - Left * Bay_W/2 - Bay_H/2*Gravity
    SDK_Vector_SetX(&P_TopRightLouvre, X_eye+louvre_offset*SDK_Vector_GetX(&V_Surface) -Bay_W/2*SDK_Vector_GetX(&V_Left) -Bay_H/2*SDK_Vector_GetX(&V_Gravity) );
    SDK_Vector_SetY(&P_TopRightLouvre, Y_eye+louvre_offset*SDK_Vector_GetY(&V_Surface) -Bay_W/2*SDK_Vector_GetY(&V_Left) -Bay_H/2*SDK_Vector_GetY(&V_Gravity) );
    SDK_Vector_SetZ(&P_TopRightLouvre, Z_eye+louvre_offset*SDK_Vector_GetZ(&V_Surface) -Bay_W/2*SDK_Vector_GetZ(&V_Left) -Bay_H/2*SDK_Vector_GetZ(&V_Gravity) );
    // BOTTOM LEFT corner = centroid + louver offset* Surface + Left * Bay_W/2 + Bay_H/2*Gravity
    SDK_Vector_SetX(&P_BottomLeftLouvre, X_eye+louvre_offset*SDK_Vector_GetX(&V_Surface) +Bay_W/2*SDK_Vector_GetX(&V_Left) +Bay_H/2*SDK_Vector_GetX(&V_Gravity) );
    SDK_Vector_SetY(&P_BottomLeftLouvre, Y_eye+louvre_offset*SDK_Vector_GetY(&V_Surface) +Bay_W/2*SDK_Vector_GetY(&V_Left) +Bay_H/2*SDK_Vector_GetY(&V_Gravity) );
    SDK_Vector_SetZ(&P_BottomLeftLouvre, Z_eye+louvre_offset*SDK_Vector_GetZ(&V_Surface) +Bay_W/2*SDK_Vector_GetZ(&V_Left) +Bay_H/2*SDK_Vector_GetZ(&V_Gravity) );
    // BOTTOM RIGHT corner = centroid + louver offset* Surface - Left * Bay_W/2 + Bay_H/2*Gravity
    SDK_Vector_SetX(&P_BottomRightLouvre, X_eye+louvre_offset*SDK_Vector_GetX(&V_Surface) -Bay_W/2*SDK_Vector_GetX(&V_Left) +Bay_H/2*SDK_Vector_GetX(&V_Gravity) );
    SDK_Vector_SetY(&P_BottomRightLouvre, Y_eye+louvre_offset*SDK_Vector_GetY(&V_Surface) -Bay_W/2*SDK_Vector_GetY(&V_Left) +Bay_H/2*SDK_Vector_GetY(&V_Gravity) );
    SDK_Vector_SetZ(&P_BottomRightLouvre, Z_eye+louvre_offset*SDK_Vector_GetZ(&V_Surface) -Bay_W/2*SDK_Vector_GetZ(&V_Left) +Bay_H/2*SDK_Vector_GetZ(&V_Gravity) );
    
    // Lets spit out the values we have read in and some computed values
    if (verbose){
        printf("The center of window is located at <%f, %f, %f>\n", X_eye, Y_eye, Z_eye);
        printf("The window is %f wide X %f high\n", Bay_W, Bay_H);
        printf("The normal vector from the window is <%f, %f, %f>\n", sin(pi*Azimuth/180), cos(pi*Azimuth/180), 0.000);
        printf("The louvre is located %d away from the centroid\n", louvre_offset);
        printf("The center of louvre is located at <%f, %f, %f>\n", X_eye+louvre_offset*sin(pi*Azimuth/180), Y_eye+louvre_offset*cos(pi*Azimuth/180), Z_eye);
        printf("The Left vector is <%f, %f, %f>\n", SDK_Vector_GetX(&V_Left), SDK_Vector_GetY(&V_Left), SDK_Vector_GetZ(&V_Left));
        printf ("*****************\n");
        printf("TL <%f, %f, %f>\n", SDK_Vector_GetX(&P_TopLeftLouvre), SDK_Vector_GetY(&P_TopLeftLouvre), SDK_Vector_GetZ(&P_TopLeftLouvre));
        printf("TR <%f, %f, %f>\n", SDK_Vector_GetX(&P_TopRightLouvre), SDK_Vector_GetY(&P_TopRightLouvre), SDK_Vector_GetZ(&P_TopRightLouvre));
        printf("BL <%f, %f, %f>\n", SDK_Vector_GetX(&P_BottomLeftLouvre), SDK_Vector_GetY(&P_BottomLeftLouvre), SDK_Vector_GetZ(&P_BottomLeftLouvre));
        printf("BR <%f, %f, %f>\n", SDK_Vector_GetX(&P_BottomRightLouvre), SDK_Vector_GetY(&P_BottomRightLouvre), SDK_Vector_GetZ(&P_BottomRightLouvre));
    }
    
    // Copy the image buffer
    if(SDK_Image_CopyBuffer(&Im_Result)!=OMA_OK){
        beep();
        printf("SDK_Image_Create returned an error\n");
        return OMA_MEMORY;
    }
    
    // Get Image Copy of the Temp Buffers (where the triangle data comes from)
    if(SDK_Image_CopyTemp(&Im_Triangles, 1)!=OMA_OK){
        beep();
        printf("Are you sure that Temp[1] holds the triangles?\n");
        return OMA_MEMORY;
    }
    
    // Check to be sure that the temp image is the right format
    if (Im_Triangles.width != 9) {
        beep();
        printf("Please make sure that the Temporary buffers T[1] holds rows of triangles 3 x [XYZ]\n");
        return OMA_RANGE;
    }
    
    // Loop through Temporary Buffer and extract a triangle for each line...
    for(nt=0; nt<Im_Triangles.height; nt++) {
        SDK_Vector_SetX(&T_P1, SDK_Image_GetPixel(&Im_Triangles, 0, nt));
        SDK_Vector_SetY(&T_P1, SDK_Image_GetPixel(&Im_Triangles, 1, nt));
        SDK_Vector_SetZ(&T_P1, SDK_Image_GetPixel(&Im_Triangles, 2, nt));
        SDK_Vector_SetX(&T_P2, SDK_Image_GetPixel(&Im_Triangles, 3, nt));
        SDK_Vector_SetY(&T_P2, SDK_Image_GetPixel(&Im_Triangles, 4, nt));
        SDK_Vector_SetZ(&T_P2, SDK_Image_GetPixel(&Im_Triangles, 5, nt));
        SDK_Vector_SetX(&T_P3, SDK_Image_GetPixel(&Im_Triangles, 6, nt));
        SDK_Vector_SetY(&T_P3, SDK_Image_GetPixel(&Im_Triangles, 7, nt));
        SDK_Vector_SetZ(&T_P3, SDK_Image_GetPixel(&Im_Triangles, 8, nt));
        
        //reset flags
        ignore = 0; accept=0;
        
        //We will calculate the intesection from Eye to each of 3 triangle points
        Px = X_eye; Py=Y_eye; Pz= Z_eye;
        Sx = SDK_Vector_GetX(&V_Surface); Sy= SDK_Vector_GetY(&V_Surface); Sz= SDK_Vector_GetZ(&V_Surface);
        Lx = SDK_Vector_GetX(&P_TopLeftLouvre); Ly= SDK_Vector_GetY(&P_TopLeftLouvre); Lz= SDK_Vector_GetZ(&P_TopLeftLouvre);
        Rx = SDK_Vector_GetX(&P_TopRightLouvre); Ry= SDK_Vector_GetY(&P_TopRightLouvre); Rz= SDK_Vector_GetZ(&P_TopRightLouvre);
        Bx = SDK_Vector_GetX(&P_BottomLeftLouvre); By= SDK_Vector_GetY(&P_BottomLeftLouvre); Bz= SDK_Vector_GetZ(&P_BottomLeftLouvre);
        // TRIANGLE POINT 1 ********************
        Tx = SDK_Vector_GetX(&T_P1); Ty= SDK_Vector_GetY(&T_P1); Tz= SDK_Vector_GetZ(&T_P1);
        Tparam = (Tx-Px)*Sx +(Ty-Py)*Sy +(Tz-Pz)*Sz;
        if (Tparam == 0) {// In this case (orthognal) we can ignore the triangle
            ignore=1;}
        else Tparam = (Sx*(Lx-Px) + Sy*(Ly-Py) + Sz*(Lz-Pz))/Tparam;
        if( (Tparam>0)&&(Tparam<=1)){
            // a vertex is on the correct side of the louvre so we must consider the whole triangle
            accept=1;
        }
        // Determine the actual intercept point on plane TL, TR, BL on line Eye->T_P1
        Ix = Px + Tparam*(Tx-Px);
        Iy = Py + Tparam*(Ty-Py);
        Iz = Pz + Tparam*(Tz-Pz);
        // We want to map the intercept onto our OMA image plane
        if (Rx==Lx){TparX = (Iy-Ly)/(Ry-Ly);}else{TparX = (Ix-Lx)/(Rx-Lx);} // can use either except when 90 degrees
        TparZ = (Iz-Lz)/(Bz-Lz);
        X1 = Im_Result.width*TparX;
        Y1 = Im_Result.height*TparZ;
        // TRIANGLE POINT 2 ********************
        Tx = SDK_Vector_GetX(&T_P2); Ty= SDK_Vector_GetY(&T_P2); Tz= SDK_Vector_GetZ(&T_P2);
        Tparam = (Tx-Px)*Sx +(Ty-Py)*Sy +(Tz-Pz)*Sz;
        if (Tparam == 0) {// In this case (orthognal) we can ignore the triangle
            ignore=1;}
        else Tparam = (Sx*(Lx-Px) + Sy*(Ly-Py) + Sz*(Lz-Pz))/Tparam;
        if( (Tparam>0)&&(Tparam<=1)){
            // a vertex is on the correct side of the louvre so we must consider the whole triangle
            accept++;
        }
        // Determine the actual intercept point on plane TL, TR, BL on line Eye->T_P1
        Ix = Px + Tparam*(Tx-Px);
        Iy = Py + Tparam*(Ty-Py);
        Iz = Pz + Tparam*(Tz-Pz);
        // We want to map the intercept onto our OMA image plane
        if (Rx==Lx){TparX = (Iy-Ly)/(Ry-Ly);}else{TparX = (Ix-Lx)/(Rx-Lx);}
        TparZ = (Iz-Lz)/(Bz-Lz);    // theses shouldnt div 0 if bay widths are non zero
        X2 = Im_Result.width*TparX;
        Y2 = Im_Result.height*TparZ;
        // TRIANGLE POINT 3 ********************
        Tx = SDK_Vector_GetX(&T_P3); Ty= SDK_Vector_GetY(&T_P3); Tz= SDK_Vector_GetZ(&T_P3);
        Tparam = (Tx-Px)*Sx +(Ty-Py)*Sy +(Tz-Pz)*Sz;
        if (Tparam == 0) {// In this case (orthognal) we can ignore the triangle
            ignore=1;}
        else Tparam = (Sx*(Lx-Px) + Sy*(Ly-Py) + Sz*(Lz-Pz))/Tparam;
        if( (Tparam>0)&&(Tparam<=1)){
            // a vertex is on the correct side of the louvre so we must consider the whole triangle
            accept++;
        }
        // Determine the actual intercept point on plane TL, TR, BL on line Eye->T_P1
        Ix = Px + Tparam*(Tx-Px);
        Iy = Py + Tparam*(Ty-Py);
        Iz = Pz + Tparam*(Tz-Pz);
        // We want to map the intercept onto our OMA image plane
        if (Rx==Lx){TparX = (Iy-Ly)/(Ry-Ly);}else{TparX = (Ix-Lx)/(Rx-Lx);}
        TparZ = (Iz-Lz)/(Bz-Lz);    // theses shouldnt div 0 if bay widths are non zero
        X3 = Im_Result.width*TparX;
        Y3 = Im_Result.height*TparZ;
        //********************************
        if( (accept==3)&&(!ignore)){
            SDK_Image_DrawTriangle(&Im_Result, X1, Y1, X2, Y2, X3, Y3);
            //printf(" [ %f, %f] ", X1, Y1);
            //printf(" [ %f, %f] ", X2, Y2);
            //printf(" [ %f, %f]\n", X3, Y3);
        }
    }
    // Free up memory and GO
    SDK_Image_2Buffer(&Im_Result);
    SDK_Image_Free(&Im_Result);
    SDK_Image_Free(&Im_Triangles);
    SDK_Image_Refresh();
    return OMA_OK;
}

//***************************************************************//
//**    ISEDGE - a custom command by PIATRA ENGINEERING         *//
//**    this command takes a mask image or perimeter            *//
//**    and marks all non zero pixels that have neighbouring zeroes  *//
//**    i.e. on the edge of the mask image                      *//
//***************************************************************//
int isedge()
{
    Image Im_Result;
    Image Im_Original;
    
    int nc, nt;
    int dc, dt;
    int pfin; // PIXELS FOUND IN NEIHGBOURHOOD nonzero pixels (including itself in 3x3 grid)
    
    // Copy the image buffer
    if(SDK_Image_CopyBuffer(&Im_Original)!=OMA_OK){
        beep();
        printf("SDK_Image_CopyBuffer returned an error\n");
        return OMA_MEMORY;
    }
    
    // Create a new image buffer the same size as the input image in memory
    if(SDK_Image_Create(&Im_Result, Im_Original.width, Im_Original.height)!=OMA_OK){
        beep();
        printf("SDK_Image_Create returned an error\n");
        return OMA_MEMORY;
    }
    
    // loop through each pixel in the original image
    for(nt=0; nt<Im_Original.height; nt++) {
        for (nc =0; nc<Im_Original.width; nc++){
            
            // Only non zero pixels are considered
            if (SDK_Image_GetPixel(&Im_Original, nc, nt)!=0){
                
                // Now count the number of zero pixels in surrounding 3x3 box
                pfin = 0;
                for (dc=-1;dc<=1;dc++){
                    for (dt=-1;dt<=1;dt++){
                        if (SDK_Image_GetPixel(&Im_Original, nc+dc, nt+dt)==0) pfin++;
                    }
                }
                
                // If we have the right amount of non zero pixels then mark location as an edge
                //if (pfin == pntc) {
                if (pfin >0) {
                    SDK_Image_SetPixel(&Im_Result, nc, nt, 1);
                } else {
                    SDK_Image_SetPixel(&Im_Result, nc, nt, 0);
                }
                
            }
            
        }
    }
    
    // Free up memory and GO
    SDK_Image_2Buffer(&Im_Result);
    SDK_Image_Free(&Im_Result);
    SDK_Image_Free(&Im_Original);
    SDK_Image_Refresh();
    return OMA_OK;
}

//***************************************************************//
//**    VERTEDGE - a custom command by PIATRA ENGINEERING         *//
//**    this command takes a perimeter created by ISEDGE           *//
//**    and marks all non zero pixels that are part of a vertical edge  *//
//**    i.e. vertical edge (wall) of the building                      *//
//***************************************************************//
int vertedge(int n, int index)
{
    Image Im_Result;
    Image Im_Integ;
    Image Im_Edge;
    Image Im_Original;
    
    int narg;
    short step;
    short failed;
    short leading;
    short trailing;
    int nc, nt;
    
    // Read in command line arguments
    narg = sscanf(&cmnd[index],"%hd",&step);
    if (narg ==0) {
        printf("No argument specified - default step size is 1\n");
        step =1;
    }
    
    // Copy the image buffer
    if(SDK_Image_CopyBuffer(&Im_Original)!=OMA_OK){
        beep();
        printf("SDK_Image_CopyBuffer returned an error\n");
        return OMA_MEMORY;
    }
    
    // Create a new image buffer the same size as the input image in memory
    if(SDK_Image_Create(&Im_Result, Im_Original.width, Im_Original.height)!=OMA_OK){
        beep();
        printf("SDK_Image_Create returned an error\n");
        return OMA_MEMORY;
    }
    
    // Create a new image buffer the same size as the input image in memory
    if(SDK_Image_Create(&Im_Edge, Im_Original.width, Im_Original.height)!=OMA_OK){
        beep();
        printf("SDK_Image_Create returned an error\n");
        return OMA_MEMORY;
    }
    
    // Create a thin single row image to act as the integration of points forming vertical edge
    if(SDK_Image_Create(&Im_Integ, Im_Result.width, 1)!=OMA_OK){
        beep();
        printf("SDK_Image_Create returned an error\n");
        return OMA_MEMORY;
    }
    
    // loop through each pixel in the original image
    for(nt=0; nt<Im_Original.height; nt++) {
        for (nc =0; nc<Im_Original.width; nc++){
            
            // Only non zero pixels are considered
            if (SDK_Image_GetPixel(&Im_Original, nc, nt)!=0){
                
                // Now count the number of zero pixels in surrounding 3x3 box
                int pfin = 0;
                for (int dc=-1;dc<=1;dc++){
                    for (int dt=-1;dt<=1;dt++){
                        if (SDK_Image_GetPixel(&Im_Original, nc+dc, nt+dt)==0) pfin++;
                    }
                }
                
                // If we have the right amount of non zero pixels then mark location as an edge
                //if (pfin == pntc) {
                if (pfin >0) {
                    SDK_Image_SetPixel(&Im_Edge, nc, nt, 1);
                } else {
                    SDK_Image_SetPixel(&Im_Edge, nc, nt, 0);
                }
                
            }
            
        }
    }

    // loop through each pixel in the EDGE image
    for(nt=0; nt<Im_Edge.height; nt++) {
        for (nc =0; nc<Im_Edge.width; nc++){
            
            // Only non zero pixels are considered
            if (SDK_Image_GetPixel(&Im_Edge, nc, nt)!=0){
                
                // Test all pixels above and below within step
                failed=0; leading =0; trailing=0;
                for (int z=-step; z<=step;z++){
                    if (SDK_Image_GetPixel(&Im_Edge, nc, nt+step)==0){
                        failed++; // count the number of zero pixels in the small vertical strip
                    }
                }
                if (!failed){ // In this case we have found a vertical section of edge bigger than +/- step
                    // Going to have to check if edge is leading or trailing edge back in original image
                    if (SDK_Image_GetPixel(&Im_Original, nc+1, nt)==0) trailing = 1;
                    if (SDK_Image_GetPixel(&Im_Original, nc-1, nt)==0) leading = 1;
                    
                    //SDK_Image_SetPixel(&Im_Result, nc, nt, 1);
                    if (trailing && !leading){
                        SDK_Image_SetPixel(&Im_Result, nc, nt, -11);
                        SDK_Image_SetPixel(&Im_Integ, nc, 0, SDK_Image_GetPixel(&Im_Integ, nc, 0)-1);
                    }
                    if (leading && !trailing) {
                        SDK_Image_SetPixel(&Im_Result, nc, nt, 1);
                        SDK_Image_SetPixel(&Im_Integ, nc, 0, SDK_Image_GetPixel(&Im_Integ, nc, 0)+1);
                    }
                    if (leading && trailing) {
                        SDK_Image_SetPixel(&Im_Result, nc, nt, 0.666);
                    }
                }
            }
        }
    }
    
    // Find the first edge from the left edge of the image
    int nvlf=0; // number of vertical lines found'
    int lbox=0, rbox=0; // the left and right of box
    for (nc=0; nc<Im_Integ.width; nc++){
        if (SDK_Image_GetPixel(&Im_Integ, nc, 0)>0){ //leading
            lbox = rbox;
            rbox = nc-1;
            printf("lead box goes from %d to %d \n", lbox, rbox);
            nvlf++;
        } else if (SDK_Image_GetPixel(&Im_Integ, nc, 0)<0){//trailing
            lbox = rbox+1;
            rbox = nc;
            printf("trail box goes from %d to %d \n", lbox, rbox);
            nvlf++;
        }
    }
    printf("there should be %d boxes\n", nvlf);
    
    // Free up memory and GO
    SDK_Image_2Buffer(&Im_Result);
    SDK_Image_Free(&Im_Result);
    SDK_Image_Free(&Im_Original);
    SDK_Image_Refresh();
    return OMA_OK;
}

//***************************************************************//
//**    HORISO_FILL - a custom command handle by PIATRA ENGINEERING   *//
//***************************************************************//
int horiso_fill()
{
    Image Im_Original;
    Image Im_Result;
    
    // Copy the image buffer
    if(SDK_Image_CopyBuffer(&Im_Original)!=OMA_OK){
        beep();
        printf("SDK_Image_CopyBuffer returned an error\n");
        return OMA_MEMORY;
    }
    
    // Create a new image buffer the same size as the input image in memory
    if(SDK_Image_Create(&Im_Result, Im_Original.width, Im_Original.height)!=OMA_OK){
        beep();
        printf("SDK_Image_Create returned an error\n");
        return OMA_MEMORY;
    }

    square_column_fill(&Im_Result, 0, 0, Im_Original.width-1, Im_Original.height-1, 0);
    
    SDK_Image_Free(&Im_Original);
    SDK_Image_Refresh();
    return OMA_OK;
}

//***************************************************************//
//**    SQUARE_COLUMN_FILL - a custom command handle by PIATRA ENGINEERING   *//
//***************************************************************//
int square_column_fill(Image *imptr, int TL, int TR, int BL, int BR, int depth)
{
    return OMA_OK;
}

