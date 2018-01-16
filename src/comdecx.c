/*
 OMAX -- Photometric Image Processing and Display
 Copyright (C) 2006  by the Developers of OMA
 
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

// if this is set, don't print anything
int no_print = 0;

/* Things for loops in macros */

extern char* variable_names;
int var_char_count = 0;
short startval[NESTDEPTH];
short endval[NESTDEPTH];
short stepval[NESTDEPTH];
short lvar[NESTDEPTH];
short lvar_max = 0;
short loopdepth = 0;
short executedepth[EX_NEST_DEPTH] = {0};
short inloop = 0;

int branchto[NESTDEPTH];
int macro_line_number = 0;
int exe_line_number[EX_NEST_DEPTH] = {0};
int macro_stopable = 1;
int int_value = 0;			//%v in a macro is replaced by this value
int inc_value = 1;			//%v is incremented by this amount after use
int last_x_val,last_y_val;	// %x and %y fill in values -- should come from mouse interaction


// Things for IF statements
int if_condition_met = 1;
int ifdepth = 0;
int if_condition[NESTDEPTH];
int error_return = 0;
int which_ex_buffer = -1;

int stop_on_error = 0;

// Things for command history
#define HISTORY_BUFFER_SIZE 10000
char cmnd_history[HISTORY_BUFFER_SIZE];
int hist_index=0;
int selected_hist_index = 0;
int stored_commands=0;

char* fullname();
int update_pause(),vprint(),get_variable_index(),printf(),do_assignment();

ComDef  *clist_ptr;
extern ComDef my_commands[];


int  null();
int  plus();
int  minus();
int  multiply();
int  divide();
int tDstore();
int tDdelete();
int tDyz();
int tDxz();
int tDsavefile();
int	addfile();
int addtmp();
int	accum();
int	accadd();
int	accget();
int	accdelete();
int	accpdf();
int	absol();
int aspectratio();
#ifdef ANALOG_IO
int ain();
int aout();
int ainburst();
#endif
#ifdef ANDOR
int andor();
#endif

int	block();
int blkave();
int binsiz();
int binbyt();
int blockrgb();

int	subfile();
int	mulfile();
int	divfile();
int	subtmp();
int	multmp();
int	divtmp();
int channel();
int	setcminmax();
int	setccd();
int	clip();
int clipbottom();
int clipfbottom();
int clipfr();
int	corect();	
int	createfile();
int	concatfile();
int	closefile();
int	calc_cmd();
int	calcall();
int	compos();
int comtmp();
int crossc();
int croprectangle();
int croprgb();
int cylimage();
int clearbad();
#ifdef GPHOTO
int capture();
//int camexpose();
int camlistsettings();
int camgetsetting();
int camsetsetting();
int close_gphoto_cam();
#endif
int colorflag();
int	displa();
int drgbq();
int disoff();
int     defmac();
int	delay();
int	dx();
int	dy();
//	int	divscalar();	
int	diffx();
int	diffy();
int dzoom_q();

int dfft();
int dautoc();
int dzoomrgb_q();

int dmnmx_q();
int dfilt();
int dcrawarg();
int dquartz();
int div_rgb();
int	doc2rgb();

int	my_erase();
int     exitx();
int	execut();
int echo();
int echo_color();
int exponent();
int	freem();
int	fluct();
int	fwdata();
int	fwdatm();
int	frdata();
int frdatm();
int frame();
int frame2();

int dofft();

int endifcmnd();
int fill_box();
int fill_mask();
int fcalc();
int fccase();
int fconst();
int ftemp();
int fopen_pk();
int fclose_pk();
int fecho_pk();
int fliph();
int flipv();
int findbad();


int getfile();
int	gmacro();
int	gwindo();
int	gimage();
int gpib();
int	gpibdv();
int gpibto();
//	int	getscalar();	
int gradient();
int gradient2();
int	getmatch();
int getsettings();
int	savsettings();
int getfit();
int getpic();
int getrgb();
int getred();
int getgrn();
int getblu();
int gsmooth();
int gsmooth2();
int gtemp();
int getbin();
int getFileNames();
#ifdef GigEthernetCamera
int gige();
#endif

int gamma_fix();
int ghdr();
int     help();

int	incmac();
int	my_invert();
int	integ();	
int	integfill();	
int imacro();
int ifcmnd();
int ifnotexist();
int ifexist();
int ifnotdefined();
int ifdefined();
int image_to_power();


int	keymovie();
int	keyope();
int	keywin();
int	keynsb();
int	keyasb();
int	keyksb();
int	keyrec();
int krrgb();
int	keypct();
int	keysur();
int	keyhis();
int	keyblkandwht();
int	keyinteg();
int	keylimit();
int     list();
int     listmac();
int     logg();
int	linecalc();
int	lineget();
int	linestats();
int	linenorm();
int linssb();
int	ltload();
int	ltsave();
int loop();		
int loopend();
int loopbreak();
int ln();
int ltemp();
int label();

int maxx();
//	int	mulscalar();	
int	maknew();
int	mirror();
int macro();
int	match();

int mask();
int magsqr();
int mult_rgb();
int minus_rgb();
int markpixel();

int	newwin();
int nubus();
int nuparm();
int nettst();
int nextFile();
int	openfl();
int	outpar();

int of2piv();

int	setpixsiz();
int	setpositive();
int	palette();
int	pmacro();
int	prefix();
int	pdfset();
int photomode();
int prflag();

int getpeak();
int piv();
int pivcrs();
int pivsave();
int pivget();
int pivplt();
int pivsiz();
int pivinc();
int pivlst();
int pivset();
int pivalt();
//
int pivdir();
int pivavg();
int pivadd();
int pivsub();
int pivsubc();
int pivmulf();
int pivsqr();
int pivdivf();
int pivplus();
int pivminus();
int pivmul();
int pivdiv();
//
int pivref();
int pivsht();
int pivbad();
int peakim();
int points();
int pairs();
//
int pairls();
//
int pntimg();
int primg();
int prinfo();
int pntmos();
int pairpr();

int power();
int  pdfsiz();
int	pdf();
int imp_pause();
int plus_rgb();
int pixvalue();
int parimage();

int rmacro();
int	rotate();
int rotrgb();
int	receiv();
int	run();
int	rectan();
int ruler();
int rotcrp();
int rgb2grey();
int rgb2red();
int rgb2green();
int rgb2blue();
int readbad();

int savefile();
int	smacro();
int	smooth();
int	setsit();
int	send();
int	synch();
int	suffix();
int	sresp();
int	sback();
//	int	setup();	
int	smean();
int	square();
int	stringmacro();
int save_format();
int square_root();
int set32();
int slideshow();
int say();
int scat();
int swapby();
int subsam();
int stemp();
int savpdf();
int spex();
int satiffscaled();
int sys_command();
int stoponerror();

int	tracks();
int	transfer();
int use_format();
int uprefx();

int vorticity();
int vorticity2();

int     wdata();
int	waithi();
int	setx0();
int	sety0();

int cent();
int chngpix();
int makcirc();
int makring();
int myhist();
int pivrmv();
int pivrnd();
int pivx();
int pivy();
int vecimg();
int vecimx();
int vecimy();
int valmac();
int variab();
int vfloat();
int vint();
int warpar();
int warp();
int writebad();

// Tiff Commands courtesy of Peter Kalt
int satiff();	// saves a tiff file
int gtiff();	// loads a tiff file
int bit8();		// converts to unsigned 8 bit data 0->254

// Added PIV commands courtesy of Peter Kalt
int	makpiv();		// generate a random PIV image
//	int nullfn();		// null function
int pivangle();		// Kill any vector in image that points in wrong direction
int	pivfil();		// Interpolate zero vectors in field from neighbours
int	pivmagnitude();	// Kill any vector in image that differ from neighbourhood average
int	pivmassage();	// Automatically click all good vectors in image PIVALT 3 style
int	xcorr();		// performs cross-correlation on buffer & file images
int 	piv_execute();	// perform batch mode PIV execution
int 	piv_convert();	// Convert to velocity field to m/s

//int subincpp();	// see sample code on how to include c++ code in OMA
int sbig();

// hardware-specific commands
#ifdef GPIB
int conect();
int receiv();
int run();
int send();
int ask();
int gpibdv();
#endif

#ifdef PVCAM
int	pvcam();
#endif

#ifdef SensiCam

int focus();
int sensi();

#endif

#ifdef ST6
int st6();
#endif

#ifdef SERIAL_PORT
int serial();
int serclo();
#endif


ComDef   commands[] =    {
    {{"               "},	null},			
    {{"+              "},	plus},			/*	@imp2.c	*/
    {{"+RGB           "},	plus_rgb},		/*	@imp2.c	*/
    {{"-              "},	minus},			/*	@imp2.c	*/
    {{"-RGB           "},	minus_rgb},		/*	@imp2.c	*/
    {{"*              "},	multiply},		/*	@imp2.c	*/
    {{"*RGB           "},	mult_rgb},		/*	@imp2.c	*/
    {{"/              "},	divide},		/*	@imp2.c	*/
    {{"/RGB           "},	div_rgb},		/*	@imp2.c	*/
    {{";              "},	null},			
    {{"3DSTORE        "},	tDstore},		/*	@imp2.c	*/
    {{"3DDELETE       "},	tDdelete},		/*	@imp2.c	*/
    {{"3DYZ           "},	tDyz},			/*	@imp2.c	*/
    {{"3DXZ           "},	tDxz},			/*	@imp2.c	*/
    {{"3DSAVE         "},	tDsavefile},		/*	@imp2.c	*/
    {{"ADDFILE        "},	addfile},		/*  @imp.c */
    {{"ADDTMP         "},	addtmp},		/*  @imp.c */
    {{"ACCUMULATE     "},	accum},			
    {{"ACADD          "},	accadd},			
    {{"ACGET          "},	accget},			
    {{"ACDELETE       "},	accdelete},	
    {{"ACPDF          "},	accpdf}, 		/* @imp.c */
    {{"ACSPDF         "},	pdfset},			/* @imp.c */
    {{"ABSOLUTE       "},	absol},			/*	@imp2.c	*/	
#ifdef GPIB
    {{"ASK            "},	ask},			/*  @gpib.c */
#endif
    {{"ASPECTRATIO    "},	aspectratio},	/*  @imp2.c */
#ifdef ANALOG_IO
    {{"AINPUT         "},	ain},			/*  @labio.c */
    {{"AOUTPUT        "},	aout},			/*  @labio.c */
    {{"AINBURST       "},	ainburst},		/*  @labio.c */
#endif	
#ifdef ANDOR
    {{"ANDOR          "},	andor},			/*  @camera.c */
#endif
    
    {{"BLOCK          "},	block},			/*	@imp.c	*/
    {{"BLKAVE         "},	blkave},		/*	@imp.c	*/
    {{"BINSIZE        "}, binsiz},			/*	@im2p.c	*/
    {{"BINBYTE        "}, binbyt},			/*	@imp2.c	*/
    {{"BIT8           "},	bit8},			/*	@imp.c	*/
    {{"BLOCKRGB       "},	blockrgb},		/*	@imp.c	*/
    
    {{"CHANEL         "},	channel},		/*	@imp.c	*/
    {{"COLUMN         "},	channel},		/*	@imp.c	*/
    {{"CMINMX         "},	setcminmax},			
    {{"CCD            "},	setccd},	
    {{"CLIP           "},	clip},			/*	@imp2.c	*/
    {{"CLIPBOTTOM     "},	clipbottom},			/*	@imp2.c	*/
    {{"CLIPFRACT      "},	clipfr},			/*	@imp2.c	*/
    {{"CLIPFBOTTOM    "},	clipfbottom},			/*	@imp2.c	*/
    {{"CORECT         "},	corect},			
    {{"CREATE         "},	createfile},			
    {{"CONCATENATE    "},	concatfile},			
    {{"CLOSE          "},	closefile},			
    {{"CALCULATE      "},	calc_cmd},			
    {{"CALCALL        "},	calcall},			
    {{"COMPOSITE      "},	compos},			/*	@imp.c	*/
    {{"COMTMP         "},	comtmp},			/*	@imp.c	*/
    {{"CROP           "},	croprectangle},		/*	@imp2.c	*/
    {{"CROPRGB        "},	croprgb},			/*	@imp2.c	*/
#ifdef GPIB
    {{"CONECT         "},	conect},			/* @gpib.c */
#endif
    {{"CYLIMAGE       "},	cylimage},			/*	@imp2.c	*/
    {{"CLEARBAD       "},	clearbad},			/*	@imp2.c	*/
#ifdef GPHOTO
    {{"CAPTURE        "},	capture},			/*	@gphoto_stuff.c	*/
    //{{"CAMEXPOSE      "},	camexpose},         /*	@gphoto_stuff.c	*/
    {{"CAMLISTSETTINGS"},	camlistsettings},	/*	@gphoto_stuff.c	*/
    {{"CAMGETSETTING  "},	camgetsetting},		/*	@gphoto_stuff.c	*/
    {{"CAMSETSETTING  "},	camsetsetting},		/*	@gphoto_stuff.c	*/
    {{"CAMCLOSE       "},	close_gphoto_cam},		/*	@gphoto_stuff.c	*/
#endif
    {{"COLORFLAG      "},	colorflag},			/* @imp.c */
    
    {{"DISPLAY        "},	dquartz},			/* @impdisq.c */
    {{"DRGB           "},	drgbq},				/* @impdisx.c */
    {{"DISOFF         "},	disoff},			/* @impdrx.c */
    {{"DMACRO         "},	defmac},			
    {{"DIVFILE        "},	divfile},			/* @imp.c */
    {{"DIVTMP         "},	divtmp},			/* @imp.c */
    {{"DX             "},	dx},			
    {{"DY             "},	dy},			
    {{"DIFFX          "},	diffx},			/*	@imp2.c	*/
    {{"DIFFY          "},	diffy},			/*	@imp2.c	*/
    {{"DELAY          "},	delay},			
#ifdef MacOS
    {{"DZOOM          "},	dzoom_q},			/*	@impdisq.c	*/
    {{"DFFT           "},	dfft},			/*	@pivgraph.c	*/
    {{"DAUTOCORR      "},	dautoc},			/*	@pivgraph.c	*/
    {{"DMNMX          "},	dmnmx_q},			/*	@plotchansq.c	*/
    {{"DZRGB          "},	dzoomrgb_q},			/*	@impdisq.c	*/
#endif
    {{"DFILTER        "},	dfilt},			/* @imp2.c */
    {{"DCRAWARG       "},	dcrawarg},			/* @imp.c */
    {{"DOC2RGB        "},	doc2rgb},			/* @imp2.c */
    {{"ERASE          "},	my_erase},			
    {{"EXIT           "},	exitx},			
    {{"EXECUTE        "},	execut},			
    {{"ECHO           "},	echo},			/*	@imp.c	*/
    {{"ECHOCOLOR      "},	echo_color},	/*	@imp.c	*/
    {{"EXP            "},	exponent},		/*	@imp2.c	*/
    {{"ENDIF          "},	endifcmnd},		/*	@comdecx.c */
    
    {{"FREEMEMORY     "},	freem},			/* @imp.c */		
    {{"FLUCTUATION    "},	fluct},			
    {{"FWDATA         "},	fwdata},			/*	@imp2.c	*/
    {{"FWDATMAT       "},	fwdatm},			/*	@imp2.c	*/
    {{"FRDATA         "},	frdata},			/*	@imp2.c	*/
    {{"FRDATMAT       "},	frdatm},			/*	@imp2.c	*/
    {{"FILBOX         "},	fill_box},			/*	@imp2.c	*/
    {{"FILMSK         "},	fill_mask},			/*	@imp2.c	*/
    {{"FCALC          "},	fcalc},				/*	@imp2.c	*/
    {{"FCCASE         "},	fccase},			/*	@imp2.c	*/
    {{"FCONST         "},	fconst},			/*	@imp2.c	*/
    {{"FRAME          "},	frame},				/* 	@imp2.c */
    {{"FRAME2         "},	frame2},			/* 	@imp2.c */
    {{"FLOAT          "},	vfloat},			/*  @comdecx.c */
#ifdef SensiCam
    {{"FOCUS          "},	focus},				/*  @SensiCamx.c */
#endif
    {{"FFT            "},	dofft},				/*  @fft.c */
    {{"FTEMP          "},	ftemp},				/*  @imp.c */
    {{"FOPEN          "},	fopen_pk},			/*  @imp2.c */
    {{"FCLOSE         "},	fclose_pk},			/*  @imp2.c */
    {{"FECHO          "},	fecho_pk},			/*  @imp2.c */
    {{"FLIPHORIZ      "},	fliph},				/*  @imp2.c */
    {{"FLIPVERT       "},	flipv},				/*  @imp2.c */
    {{"FINDBAD        "},	findbad},			/*  @imp2.c */
    
    
    {{"GET            "},	getfile},		/*	@imp.c	*/
    {{"GMACRO         "},	gmacro},
    {{"GETSETTINGS    "},	getsettings},		
#ifdef MacOS
    {{"GWINDO         "},	gwindo},	
#endif
    {{"GRADIENT       "},	gradient},		/*	@imp2.c	*/
    {{"GRAD2          "},	gradient2},		/*	@imp2.c	*/
    {{"GETMATCH       "},	getmatch},		/*	@imp2.c	*/
    {{"GETFIT         "},	getfit},			/*	@imp.c	*/
    {{"GETRED         "},	getred},			/*	@imp.c	*/
    {{"GETGRN         "},	getgrn},			/*	@imp.c	*/
    {{"GETBLUE        "},	getblu},			/*	@imp.c	*/
    {{"GETRGB         "},	getrgb},			/*	@imp.c	*/
    {{"GETPIC         "},	getpic},			/*  @menucommandsx.c */
    {{"GSMOOTH        "},	gsmooth},			/*	@imp.c	*/
    {{"GSMOO2         "},	gsmooth2},			/*	@imp.c	*/
    {{"GTEMP          "},	gtemp},				/*	@imp.c	*/
    {{"GETBINARY      "},   getbin},			/*	@imp2.c	*/
    {{"GETFILENAMES   "},   getFileNames},		/*	@imp.c	*/
    
#ifdef DOTIFF
    {{"GTIFF          "},	gtiff},			/*	@imp.c	*/
#endif
#ifdef GPIB
    {{"GPIBDV         "},	gpibdv},			/* @gpib.c */
#endif
#ifdef GigEthernetCamera
    {{"GIGETHERNETCAM "},	gige},				/* @gige.c */
#endif
    {{"GAMMA          "},	gamma_fix},			/*	@imp2.c	*/
    {{"GHDR           "},	ghdr},              /*	@imp2.c	*/
    {{"HELP           "},	help},			
    {{"INCMACRO       "},	incmac},			
    {{"INVERT         "},	my_invert},	
    {{"INTEGRATE      "},	integ},				/*	imp2.c	*/
    {{"INTFILL        "},	integfill},			/*	imp2.c	*/
    {{"IMACRO         "},	imacro},			/*	@comdec.c	*/
    {{"INTVARIABLE    "},	vint},				/*  @comdecx.c */
    {{"IF             "},	ifcmnd},			/*	@comdecx.c */
    {{"IFNEXS         "},	ifnotexist},		/*	@comdecx.c */
    {{"IFEXST         "},	ifexist},			/*	@comdecx.c */
    {{"IFNDEFINED     "},	ifnotdefined},		/*	@comdecx.c */
    {{"IFDEFINED      "},	ifdefined},			/*	@comdecx.c */
    {{"IMPOWR         "}, image_to_power},		/*  @imp2.c */
    
    
    {{"KRECSV         "},	keyrec},			/*	@imp2.c	*/
    {{"KRRGB          "},	krrgb},			/*	@imp2.c	*/
    {{"KPCTR          "},	keypct},			/*  @domenu.c */
    {{"KINTEGRATE     "},	keyinteg},		/*  @domenu.c */
    {{"KLIMIT         "},	keylimit},		/*	@imp2.c	*/
    {{"LIST           "},	list},			
    {{"LMACRO         "},	listmac},			
    {{"LOG            "},	logg},	
#ifdef MacOS
    {{"LINCALC        "},	linecalc},			
    {{"LINGET         "},	lineget},			
    {{"LINSTATS       "},	linestats},			
    {{"LINORM         "},	linenorm},			
    {{"LINSSB         "},	linssb},			/*	@lineout.c	*/
    {{"LTLOAD         "},	ltload},			
    {{"LTSAVE         "},	ltsave},		
    {{"LABEL          "},	label},				/*	@plotchansq.c	*/
#endif		
    {{"LOOP           "},	loop},			/*	@comdec.c	*/
    {{"LOOPND         "},	loopend},		/*	@comdec.c	*/
    {{"LOOPBREAK      "},	loopbreak},		/*	@comdec.c	*/
    {{"LN             "},	ln},			/*	@imp2.c	*/
    {{"LTEMP          "},	ltemp},			/*	@imp.c	*/
    {{"MAX            "},	maxx},			
    {{"MULFILE        "},	mulfile},			/* @imp.c */
    {{"MULTMP         "},	multmp},			/* @imp.c */
    {{"MAKNEW         "},	maknew},			/*	@imp.c	*/
    {{"MIRROR         "},	mirror},			/*	@imp.c	*/
    {{"MACRO          "},	macro},			/*	@comdec.c	*/
    {{"MATCH          "},	match},			/*	@imp2.c	*/
    {{"MASK           "},	mask},			/*  @fft2.c */
    {{"MAKPIV         "},	makpiv},			/*  @fft2.c */
    {{"MARKPIXEL      "},	markpixel},		/*	@plotchansq.c	*/
    {{"NEWWIN         "},	newwin},			/*  @imp2.c */	
    //		{{"NULLFN         "},	nullfn},			/*  @fft2.c */	
    
#ifdef Network
    {{"NETTST         "},	nettst},			/* @imp_network.c */
#endif
    {{"NEXTFILE        "},	nextFile},			/* @imp.c */
    {{"OPENFLAG       "},	openfl},	
#ifdef PIV
    {{"OF2PIV         "},	of2piv},			/*	@fft.c	*/	
#endif
    {{"PIXSIZE        "},	setpixsiz},			
    {{"POSITVE        "},	setpositive},	/*	@imp2.c	*/
    {{"PALETTE        "},	palette},			
    {{"PMACRO         "},	pmacro},			
    {{"PREFIX         "},	prefix},
#ifdef PVCAM
    {{"PVCAM          "},	pvcam},			/* @pvc_oma.c */
#endif
    {{"PEAK           "},	getpeak},		/*	@fft.c	*/
    {{"PIV            "},	piv},			/*	@fft.c	*/
    {{"PIVCRS         "},	pivcrs},			/*	@fft.c	*/
    {{"PIVSAVE        "},	pivsave},		/*	@fft.c	*/
    {{"PIVGET         "},	pivget},			/*	@fft.c	*/
    {{"PIVPLT         "},	pivplt},			/*	@fft.c	*/
    {{"PIVSIZE        "},	pivsiz},			/*	@fft.c	*/
    {{"PIVINC         "},	pivinc},			/*	@fft.c	*/
    {{"PIVLST         "},	pivlst},			/*	@fft.c	*/
    {{"PIVSET         "},	pivset},			/*	@fft.c	*/
    {{"PIVALTER       "},	pivalt},			/*	@fft.c	*/
    {{"PIVCNV         "},	piv_convert},			/* fft.c */
    {{"PIVEXE         "},	piv_execute},			/* fft.c */
    //
    {{"PIVANG         "},	pivangle},		/*	@fft.c	*/ 
    {{"PIVFILL        "},	pivfil},			/*	@fft2.c	*/ 
    {{"PIVMAG         "},	pivmagnitude},	/*	@fft2.c	*/ 
    {{"PIVMSG         "},	pivmassage},		/*	@fft2.c	*/ 
    //
    {{"PIVDIRECTION   "},	pivdir},			/*	@fft2.c	*/     
    {{"PIVAVG         "},	pivavg},			/*	@fft2.c	*/   
    {{"PIVADD         "},	pivadd},			/*	@fft2.c	*/   
    {{"PIVSUB         "},	pivsub},			/*	@fft2.c	*/   
    {{"PIVSUC         "},	pivsubc},		/*	@fft2.c	*/   
    {{"PIVMULT        "},	pivmulf},		/*	@fft2.c	*/   
    {{"PIVSQRT        "},	pivsqr},			/*	@fft2.c	*/   
    {{"PIVDIVIDE      "},	pivdivf},		/*	@fft2.c	*/   
    {{"PIV+           "},	pivplus},		/*	@fft2.c	*/   
    {{"PIV-           "},	pivminus},  		/*	@fft2.c	*/   
    {{"PIV*           "},	pivmul},			/*	@fft2.c	*/   
    {{"PIV/           "},	pivdiv},			/*	@fft2.c	*/  
    {{"PIVRMV         "},	pivrmv},			/* @fft2.c */
    {{"PIVRND         "},	pivrnd},			/* @fft2.c */
    {{"PIVX           "},	pivx},			/* @fft2.c */
    {{"PIVY           "},	pivy},			/* @fft2.c */ 
    //
    {{"PIVREFORMAT    "}, 	pivref},			/* @fft2.c */
    {{"PIVSHT         "},	pivsht},			/*	@fft.c	*/
    {{"PIVBAD         "},	pivbad},			/*	@fft.c	*/
    {{"PEAKIMAGE      "},	peakim},			/*	@fft2.c	*/
    {{"POINTS         "},	points},			/*	@fft2.c	*/
    {{"PAIRS          "},	pairs},			/*	@fft2.c	*/
    {{"PAIRLS         "},	pairls},			/*	@fft2.c	*/
    {{"PNTIMG         "},	pntimg},			/*	@fft2.c	*/
    {{"PRIMG          "},	primg},			/*	@fft2.c	*/
    {{"PRINFO         "},	prinfo},			/*	@fft2.c	*/
    {{"PNTMOSAIC      "},	pntmos},			/*	@fft2.c	*/
    {{"PAIRPR         "},	pairpr},			/*	@fft2.c	*/
    {{"PIXVALUE       "},	pixvalue},			/*	@imp.c	*/
    {{"PARIMAGE       "},	parimage},			/*	@imp2.c	*/
    
    {{"POWER          "},	power},			/*	@imp2.c	*/
    {{"PDFSIZE        "},	pdfsiz},			/*	@imp.c	*/
    {{"PDFDO          "},	pdf},			/*	@imp.c	*/
    {{"PAUSE          "},	imp_pause},		/*	@comdec.c	*/
    {{"PRFLAG         "},	prflag},		/*	@comdec.c	*/
    
    {{"RMACRO         "},	rmacro},			/*	@comdec.c	*/
    {{"ROW            "},	tracks},			/*	@imp.c	*/
    {{"ROTATE         "},	rotate},			/*	@imp.c	*/
    {{"ROTRGB         "},	rotrgb},			/*	@imp.c	*/
    
    {{"RECTANGLE      "},	rectan},			/*	@imp2.c	*/
    {{"RECTCENTER     "},	cent},			/*  @fft2.c */
    {{"RULER          "},	ruler},			/*	@imp2.c	*/
    {{"ROTCRP         "}, rotcrp},		/*	@imp2.c	*/
    {{"RGB2GREY       "}, rgb2grey},		/*	@imp2.c	*/
    {{"RGB2RED        "}, rgb2red},		/*	@imp2.c	*/
    {{"RGB2GREEN      "}, rgb2green},		/*	@imp2.c	*/
    {{"RGB2BLUE       "}, rgb2blue},		/*	@imp2.c	*/
    {{"READBAD        "}, readbad},			/*	@imp2.c	*/
#ifdef GPIB
    {{"RECEIVE        "},	receiv},			/*  @gpib.c */
    {{"RUN            "},	run},			/*  @gpib.c */
#endif
    {{"SAVE           "},	savefile},
    {{"SAVSETTINGS    "},	savsettings},
#ifdef DOTIFF
    {{"SATIFF         "},	satiff},			/*	@imp.c	*/	
    {{"SATIFFSCALED   "},	satiffscaled},			/*	@imp.c	*/
#endif			
    {{"SAVPDF         "},	savpdf},			/*	@pdf_copy.c	*/	
    {{"SMACRO         "},	smacro},			
    {{"SUBFILE        "},	subfile},			/* @imp.c */
    {{"SUBTMP         "},	subtmp},			/* @imp.c */
    {{"SMOOTH         "},	smooth},			/* @imp.c */
    {{"SIT            "},	setsit},		
    {{"SFORMT         "},	save_format},	/*	@impdr.c	*/					
    {{"SUFFIX         "},	suffix},			
    {{"SRESPONSE      "},	sresp},			
    {{"SBACK          "},	sback},			
    {{"SMEAN          "},	smean},			
    {{"SQUARE         "},	square},			/*	@imp2.c	*/
    {{"STEMP          "},	stemp},			/* @imp.c */
    {{"STRMACRO       "},	stringmacro},		
    {{"SQRT           "},	square_root},	/*	@imp2.c	*/
    {{"SCATTER        "},	scat},			/*	@imp2.c	*/
#ifdef SensiCam
    {{"SENSICAM       "},	sensi},			/*  @SensiCamx.c */
#endif
#ifdef SPEX
    {{"SPEX           "},	spex},			/*  @labio.c */
#endif
    
#ifdef GPIB
    {{"SEND           "},	send},			/*  @gpib.c */
#endif
#ifdef SERIAL_PORT
    {{"SERIAL         "},	serial},			/*  @labio.c */
    {{"SERCLOSE       "},	serclo},			/*  @labio.c */
#endif
    {{"SWAPBYTES      "},	swapby},		/* @imp.c */
    {{"SUBSAMPLE      "},	subsam},		/* @imp.c */
#ifdef SBIG		
    {{"SBIG           "},	sbig},			/* @sbig.c */
#endif		
    {{"SHELL          "},	sys_command},	/* @imp.c */
    {{"STOP_ON_ERROR  "},	stoponerror},   /* @comdecx.c */
    {{"TRACKS         "},	tracks},		
    {{"UFORMT         "},	use_format},	/*	@impdr.c	*/
    {{"UPREFX         "},	uprefx},		/*	@imp.c	*/
    {{"VORTICITY      "},	vorticity},	/*	@fft.c	*/
    {{"VORT2          "},	vorticity2},	/*	@fft.c	*/
    {{"VECIMG         "},	vecimg},		/*  @fft2.c */
    {{"VECIMY         "},	vecimy},		/*  @fft2.c */
    {{"VECIMX         "},	vecimx},		/*  @fft2.c */
    {{"VALMARO        "},	valmac},		/*  @imp.c  */
    {{"VARIABLES      "},	variab},		/*  @comdecx.c */
    {{"WARP           "},	warp},           /*  @imp.c  */
    {{"WARPARAMS      "},	warpar},         /*  @imp.c  */
    {{"WRITEBAD       "},	writebad},         /*  @imp2.c  */
    {{"WDATA          "},	wdata},			/*  @imp.c  */
    {{"X0             "},	setx0},	
    {{"XCORRELATE     "},	xcorr},		/*	@fft2.c	*/ 		
    {{"Y0             "},	sety0},		
    {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},0}};		


Variable user_variables[MAX_VAR] = {{{"command_return_1"},0,0.0,0,{""}},
    {{"command_return_2"},0,0.0,0,{""}},
    {{"command_return_3"},0,0.0,0,{""}},
    {{"command_return_4"},0,0.0,0,{""}},
    {{"command_return_5"},0,0.0,0,{""}},
    {{"command_return_6"},0,0.0,0,{""}},
    {{"command_return_7"},0,0.0,0,{""}},
    {{"command_return_8"},0,0.0,0,{""}},
    {{"command_return_9"},0,0.0,0,{""}},
    {{"command_return_10"},0,0.0,0,{""}}
};
int num_variables = 10;

Expression_Element exp_el[CHPERLN];

int comdec()
{
    int     c,i,cp,clst,j,k; 
    int     chindx = 0;     /* index of first character after command */
    int     sign = 1;       /* sign of integer argument */
    int     ivalue = 0;     /* integer value */
    
    extern char cmnd[];     /* the command buffer */
    extern char* macbuf;    /* the macro buffer */
    extern char* exbuf[];     /* the execute buffer */
    char  txtbuf[CHPERLN];		// temp text buffer
    extern int maccount,macflag,macptr,macval,macincrement;
    extern int exflag,exptr[],exval[];
    extern unsigned char from_noprint;
    
    
    int     (*fnc)();
    
    
    /* --------------------- Code Starts Here ------------------------- */
    if (exflag == 0) which_ex_buffer=-1;    // this was not reset in the stop macro command, so do it now
    
    if (exflag) {
        
        /* Get next line from the execute buffer. */
        
        exptr[which_ex_buffer] = 0;
        for( i=0; i<exe_line_number[which_ex_buffer]; i++) {			// point to the appropriate line number
            while ( *(exbuf[which_ex_buffer] + exptr[which_ex_buffer]) != 0 )
                exptr[which_ex_buffer]++;
            exptr[which_ex_buffer]++;
        }
        
        fill_in_command(cmnd,exbuf[which_ex_buffer]+exptr[which_ex_buffer],exval[which_ex_buffer]);
        /* sprintf(cmnd,exbuf+exptr,exval); */
        
        while ( *(exbuf[which_ex_buffer] + ++exptr[which_ex_buffer]) ) ;	// move past the current command
        
        /* Now have the command, echo it to terminal */
        
        exe_line_number[which_ex_buffer]++;					/* next line unless modified by loop */
        printf (" E-%d-%d: %s\n",which_ex_buffer+1,exe_line_number[which_ex_buffer],cmnd);
        
        /* Now adjust pointers */
        
        if ( *(exbuf[which_ex_buffer] + ++exptr[which_ex_buffer]) == EOL && (executedepth[which_ex_buffer] == loopdepth) ) {
			//if ( *(exbuf + ++exptr) == EOL && inloop == 0 ) {
            exval[which_ex_buffer]++;
            exptr[which_ex_buffer] = 0;
            // get rid of the execute buffer
            //free(exbuf[which_ex_buffer]);
            //printf("%d\n",exbuf[which_ex_buffer]);
            which_ex_buffer--;
            
            if(which_ex_buffer < 0){
                exflag = 0;
                if_condition_met = 1;
            }
        }
        if(macro_stopable)
            checkevents();		/* if the macro is stopable, check for possible events 
                                 (such as cancel macro) before each command */	
        
    } else {
        if (macflag) {
            
            /* Get the appropriate line from the macro buffer. */
            
            macptr = 0;
            for( i=0; i<macro_line_number; i++) {
                while ( *(macbuf + macptr) != 0 )
                    macptr++;
                macptr++;
            }
            
            fill_in_command(cmnd,macbuf+macptr,macval);
            
            /* Now have the command, echo it to terminal */
            
            macro_line_number++;				/* next line unless modified by loop */
            printf (" M%d: %s\n",macro_line_number,cmnd);
            
            while ( *(macbuf + (++macptr) ) ) ;
            
            /* Now adjust macro pointers */
            
            if ( *( macbuf+ ++macptr) == EOL  && inloop == 0) {
                /* if (all commands have been done) */
                update_status();	     		/*  report the macro value used */
                macval += macincrement;	     		/*  increment value */
                macro_line_number = 0;				/* go back to the first line */
                if ( --maccount == 0 ) {     		/* check for done with macros */
                    macflag = 0;
                    macro_line_number = lvar_max = loopdepth = var_char_count = 0;
                    //ifdepth = 0;
                    if_condition_met = 1;
                    if(from_noprint) {
                        keylimit(-2);
                        from_noprint = 0;
                    }
                }
            }
            if(macro_stopable)
                checkevents();		/* if the macro is stopable, check for possible events 
                                     (such as cancel macro) before each command */	
        }
        
        else {
            getlin(txtbuf);   /* go get a line from the terminal */
            fill_in_command(cmnd,txtbuf,macval);
        }
    }
    cp = i = clst = 0;         /* cp is command pointer */
    
    clist_ptr = commands;
    
    // Now have the command text -- save that to history buffer
    
    if(!macflag && !exflag) { // don't buffer stuff going on inside macros and execs
        i = strlen(cmnd);
        if(i>0){
            while( hist_index + i >= HISTORY_BUFFER_SIZE) {
                // buffer full -- get rid of earliest commands
                for(j=0; cmnd_history[j] != 0; j++){};
                j+=2;	// j is the number of characters to be overwritten
                for(k=0; k< hist_index ; k++){
                    if( k+j < HISTORY_BUFFER_SIZE) cmnd_history[k] = cmnd_history[k+j-1];  // overwrite oldest command
                }
                stored_commands--;
                cmnd_history[hist_index - j]=0;
                hist_index = hist_index - j+1;
				
            }
            strcpy(&cmnd_history[hist_index],cmnd);
            cmnd_history[hist_index+i]=0;		// be sure EOL is in there
            hist_index +=  i+1;
            selected_hist_index = hist_index;
            stored_commands++;
            /* 
             // command history diagnostics
             printf("%s\n%d stored; %d index\n",cmnd,stored_commands,hist_index);
             for(i=0; i<hist_index; i++){
             if(cmnd_history[i]!=0) printf("%c",cmnd_history[i]);
             else printf("!");
             }
             printf("\n");
             */
        }
    }
    
    i = 0;
    
    // First, check for an "=" --> means this is an assignment command
    while ( cmnd[i] != EOL  && cmnd[i]!= ';' && cmnd[i]!= '"'){
        if ( cmnd[i++] == '='){
            // exceptions are the ECHO or FECHO commands -- there just leave the = alone
            if( !strncmp(cmnd, "echo", 4) || !strncmp(cmnd, "fecho", 5)) {
                break;
            }
            if(if_condition_met) do_assignment();		// don't do assignments if an if condition is not met
            return 0;
        }
    }
    
    // while not end of command ... 
    i = 0;
    while ( cmnd[i] != EOL  && cmnd[i] != ' ' && cmnd[i]!= ';'){
        if ( toupper(cmnd[i]) !=  clist_ptr[cp].text.name[i] ) {
            cp++;           /* next command */
            i = 0;
            if ( clist_ptr[cp].text.name[i] == EOL ){
                if( clst == 0 ) {
                    clst = 1;
                    clist_ptr = my_commands;
                    cp = 0;
                } else {
                    nosuch();
                    return -1;
                }
            }
        } else {
            i++;
        }
    }
    if (clst == 0 )
        fnc =  commands[cp].fnc;
    else
        fnc = my_commands[cp].fnc;
    
    // next check for an integer argument
    
    if (cmnd[i] != EOL && cmnd[i] != ';') {
        chindx = ++i; // save a pointer to the first 
        //   character after command 
        while ( cmnd[i] != EOL && cmnd[i] != ';'
               && cmnd[i] != ' ') {
            c = cmnd[i++];
            if (c == '+' )
                sign *= 1;
            if (c == '-' )
                sign *= -1;
            if (c >= '0' && c <= '9')
                ivalue = 10 * ivalue + c - '0';
        }
    }
    ivalue *= sign;
    //      printf("%d\n%d\n",ivalue,chindx);       
    
    // Now Execute the Appropriate Command -- unless this is in an IF whose condition is not met
    
    if(if_condition_met ||fnc == endifcmnd || fnc == ifcmnd)
        error_return = (*fnc)(ivalue,chindx);
    
    return 0;
}


int fill_in_command(char* dest,char* source,int val)
{
	int i=0;
	int j=0;
	int k,l,m,n,match,nn,arg_index,numdig = 0;
	int is_arg = 0;
	DateTimeRec datetime;
	float ave_in_rect(),rms_in_rect();
    
	extern char *macstring;
	extern char saveprefixbuf[], getprefixbuf[];
	extern char lastname[],txt[];
	extern DATAWORD max,min;
	extern TWOBYTE trailer[],header[];
	extern int start_oma_time;
	
	int oma_time;
	float nclocks;
	
	// ignore spaces or tabs at the beginning of a command
	while( *(source+i) == ' ' || *(source+i) == '\t') {
		i++;
	}
    
    /*    
     if (!if_condition_met) {    // don't do this command -- just copy ';' to destination
     *(dest+j++)= ';';
     *(dest+j)= '\0';
     return 0;
     }
     */	
	while( *(source+i) != '\0' ) {
        if(*(source+i) == ';') break;
		while( is_arg ){
			is_arg = 0;
			switch( *(source+i++)) {
                case '1':				// allow %3d for example -- include leading zeros
                case '2':				// no 2 digit lengths
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    i--;
                    numdig = *(source+i++) - '0';	// this should be the # of digits in the number
                    //pprintf("number of digits is %d\n",numdig);
                    is_arg = 1;						//	haven't cleared the arg yet -- it better be d
                    break;				
                case 'd':
                    if(numdig != 0) {
                        sprintf(txt,"%d",val);
                        //pprintf("digits: %d  length: %d\n",numdig,strlen(txt));
                        for(nn = strlen(txt); nn < numdig; nn++){
                            *(dest+j++) = '0';
                        }
                        numdig = 0;
                    }
                    sprintf(dest+j,"%d",val);
                    while(*(dest+j)) 
                        j++;
                    
                    break;
                case 's':
                    sprintf(dest+j,"%s",macstring);
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'p':
                    sprintf(dest+j,"%s",saveprefixbuf);
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'q':
                    sprintf(dest+j,"%s",getprefixbuf);
                    while(*(dest+j)) 
                        j++;
                    break;
				case 'f':
                    sprintf(dest+j,"%s",lastname);
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'b':
                    sprintf(dest+j,DATAFMT,max);
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'l':
                    sprintf(dest+j,DATAFMT,min);
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'm':
                    sprintf(dest+j,"%d",trailer[SFACTR]);
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'v':
                    sprintf(dest+j,"%d",int_value);
                    int_value +=inc_value;
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'e':
                    sprintf(dest+j,"%d",error_return);
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'a':
                    sprintf(dest+j,"%g",ave_in_rect());
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'r':
                    sprintf(dest+j,"%g",rms_in_rect());
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'w':
                    sprintf(dest+j,"%d",header[NCHAN]);
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'h':
                    sprintf(dest+j,"%d",header[NTRAK]);
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'x':
                    sprintf(dest+j,"%d",last_x_val);
                    while(*(dest+j)) 
                        j++;
                    break;
                case 'y':
                    sprintf(dest+j,"%d",last_y_val);
                    while(*(dest+j)) 
                        j++;
                    break;
                    
                case 'c':
                    oma_time = TickCount();
                    nclocks = oma_time - start_oma_time;
                    //oma_time /= CLOCKS_PER_SEC;
                    sprintf(dest+j,"%f",nclocks/60.);
                    while(*(dest+j)) 
                        j++;
                    break;
                    
                case 't':
                    GetTime(&datetime);
                    sprintf(dest+j,"%2d/%2d/%2d %2d:%2d:%2d",datetime.month,
                            datetime.day,
                            datetime.year%100,
                            datetime.hour,
                            datetime.minute,
                            datetime.second);
                    while(*(dest+j)) 
                        j++;
                    break;
                    
                default:
                    
                    break;
			}                 	// end of switch 
			// printf("Proc:%c. Next:%c\n",*(source+i-1),*(source+i)); 	
		} // end of is_arg processing
		
		if( *(source+i) == '\0' ) break; // end of command text
		
		if( *(source+i) == '\'' ) {			// a single quote will skip the following character without further interpretation
			i++;							// skip to next character
			*(dest+j++) = *(source+i++);	// just save the character
		}
		
		
		if( *(source+i) == '@' ) {			// this case for loop variable  or other variable
			i++;							// skip to next character 
			k = 0;
			// allow @4varnam -- similar to %4d
			// four digits with leading zeros
			if(*(source+i) > '0' && *(source+i) <='9'){
				// the digit is there
				numdig = *(source+i++) - '0';	// the # of digits in the number
			}
			while( (*(source+i) != ' ') && 
                  (*(source+i) != '\0') &&
                  (*(source+i) != '\\') &&		// Allow specification of end of variable with a '\'
                  (*(source+i) != ')') &&		// variable names can't have these special characters in them
                  (*(source+i) != '+') &&		// plus they could be there in arithmetic statements
                  (*(source+i) != '-') &&
                  (*(source+i) != '*') &&
                  (*(source+i) != '/') &&
                  (*(source+i) != '^') &&
                  (*(source+i) != '\t') &&
                  (*(source+i) != ';') ) {
                txt[k++] = *(source+i++);
			}
			
			txt[k] = '\0';			// txt now has the variable name
			m = 0;					// point to characters in the loop variable table 
			k = 0;					// indicate if a match was found 
			// printf("variable name:%s\n",txt); 
			
			// look through the loop variables to find a match 
			for (n = 0; n<lvar_max; n++) {	
				match = 1;
				for( l = 0; *(variable_names+m) != '\0';l++) {
					// printf("%c",*(variable_names+m)); 
					if( *(variable_names+m)	!= txt[l] )
						match = 0;
					m++;
				}
				m++;
				// printf("\n%d length\n",l); 
				
				if( txt[l] == '\0' && match == 1) {		// if true, we have the match
					k=1;
					//
					if(numdig != 0) {
						sprintf(txt,"%d",startval[n]);
						//pprintf("digits: %d  length: %d\n",numdig,strlen(txt));
						for(nn = strlen(txt); nn < numdig; nn++){
							*(dest+j++) = '0';
						}
						numdig = 0;
					}	
					//
					sprintf(dest+j,"%d",startval[n]);	
					while(*(dest+j)) 
						j++;
					break;	// exit the variable search loop having put in the numeric value
				}
			}
			if( k == 0) {	// no loop variable -- check to see if there is another variable
				arg_index = get_variable_index(txt,0);
				if(arg_index < 0 ) {
                    *(dest+j) = 0; // end the returned command line here
                    if (if_condition_met) {     // only call out errors if we are actually doing this command
                        beep();
                        printf(" No variable to match %s.\n",txt);
                        return arg_index;
                    } else {
                        // this command isn't going to be done 
                        return 0;
                    }
				}
				if(user_variables[arg_index].is_float == -1){   // this is a string variable
					sprintf(dest+j,"%s",user_variables[arg_index].estring);
				} else {
					if(numdig != 0 ) {   
						if(user_variables[arg_index].is_float){
							sprintf(dest+j,"%f",user_variables[arg_index].fvalue);
							//pprintf("digits: %d  length: %d\n",numdig,strlen(txt));
							*(dest+j+numdig) = 0;	// mark end of string after a specified number of digits
						} else {
							sprintf(txt,"%d",user_variables[arg_index].ivalue);
							//pprintf("digits: %d  length: %d\n",numdig,strlen(txt));
							for(nn = strlen(txt); nn < numdig; nn++){
								*(dest+j++) = '0';
							}
							numdig = 0;
						}	
					}
					if(user_variables[arg_index].is_float){
						if( numdig !=0){
							numdig=0;
						}else {
							sprintf(dest+j,"%g",user_variables[arg_index].fvalue);
						}
					} else {
						sprintf(dest+j,"%d",user_variables[arg_index].ivalue);
					}
				}
				while(*(dest+j)) 
					j++;
			}
            
            if(*(source+i) == '\\' ) i++;		// don't actually put the '\' in the command string
            // end of *(source+i) == @ case
		} else {
			if( *(source+i) != '%' ) {
				*(dest+j++) = *(source+i++);	// nothing special, just save the character
			} else {
				is_arg = 1;
				i++;
			}
		}
	}
	*(dest+j) = 0;
	return 0;
}

int treat_as_float = 0;		// for arithmetic on RHS of assignment
// need to decide when to do float to integer conversion and when not to

int do_assignment()
{
	extern char cmnd[];
	
	int is_variable_char(char);
	
	int i,j,varlen;
	int var_index;
	char name[MAX_VAR_LENGTH];
	Expression_Element ex_result,evaluate_string();
	
	// get rid of spaces or tabs in the command -- we just don't need them
	for(i=0; i< strlen(cmnd); i++){
		if(cmnd[i] == '"')  break;  // exception is if there is a string -- leave spaces after first quote
		if(cmnd[i] == ' ' || cmnd[i] == '	'){
			j= i;
			while(cmnd[j] != 0){
				cmnd[j] = cmnd[j+1];
				j++;
			}
			i--;	// maybe we have multiple spaces
		}
	}
	
	// get variable to left of "="
	i = varlen = 0;
	varlen = 0;
	while (cmnd[i] != '=' && varlen < MAX_VAR_LENGTH-1){
		name[varlen++] = cmnd[i++];
	}
	name[varlen] = 0;
    
	// get the index of this variable -- define it if not already there
	var_index = get_variable_index(name,1);
	if(var_index < 0) {
		beep();
		printf("Assignment error variable index:%d\n",var_index);
		return var_index;
	}
	
	// move pointer to the first character beyond the =
	while ( cmnd[i] != '=' ) i++;
	i++;
	
	// Now we are pointing to the RHS of the assignment statement
	//
	ex_result = evaluate_string(&cmnd[i]);	// get the result of the expression
	if( ex_result.op_char == 'e'){
		beep();
		printf("Assignment error -2\n");
		return -2;
	}
	if(ex_result.op_char == 's'){
		strcpy(&user_variables[var_index].estring[0],&ex_result.estring[0]);
		user_variables[var_index].is_float = -1;
	} else {
		if(treat_as_float)
			user_variables[var_index].ivalue = ex_result.fvalue;
		else
			user_variables[var_index].ivalue = ex_result.ivalue;
		user_variables[var_index].fvalue = ex_result.fvalue;
	}
	
	//printf("%d values; %c\n",rhs_vals,exp_el[0].op_char);
	vprint(var_index);
	return 0;
}

int get_variable_index(char* name, int def_flag)
{
	int i,j;
	//printf("lookup %s from %d variables\n",name,num_variables);
	// check to see if this is already defined
	for(i=0; i< num_variables; i++){
		for(j=0; j< strlen(name); j++){
			if( *(name+j) != user_variables[i].vname[j])
				break;
		}
		if( j == strlen(name) && j == strlen(user_variables[i].vname))
			return i;
	}
	
	if( i == num_variables && def_flag == 1){	// add a new variable to the list
		if(num_variables >= MAX_VAR){	// TOO MANY VARIABLES
			return -1;
		}
		for(j=0; j<= strlen(name); j++)
			user_variables[num_variables].vname[j] = *(name+j);
		user_variables[num_variables].ivalue = 0;
		user_variables[num_variables].fvalue = 0;
		user_variables[num_variables].is_float = 0;		// default is integer
		num_variables++;
		return num_variables-1;
	}
	return -2;	// undefined variable
}

int is_variable_char(char ch)
{
	if( ch == '+' || ch == '-' || ch == '*'|| ch == '/' || ch == ')' || ch == '^'
       || ch == '<'|| ch == '>' || ch == 0 )
		return 0;
	return 1;
}


Expression_Element evaluate_string(char* ex_string)
{
	
	int is_variable_char(char);
	
	int i,j,k,varlen,rhs_vals;
	int arg_index,nestdepth;
	char vname[MAX_VAR_LENGTH] = {0};
	Expression_Element ex_result,evaluate();
	/*
     printf("string is %s\n",ex_string);
     for(i=0; ex_string[i] != EOL && ex_string[i] != ';';i++){ 
     printf("%c",ex_string[i]);
     }
     */
	i= 0;
	rhs_vals = 0;
	treat_as_float = 0;
	while(ex_string[i] != EOL && ex_string[i] != ';'){ // While not the end of the command
		// if this is a string
		if( ex_string[i] == '"'){
			j = 0;
			while(ex_string[++i] != '"' && ex_string[i] != EOL){
				ex_result.estring[j++] = ex_string[i];
			}
			ex_result.estring[j] = 0;
			ex_result.op_char = 's';
			return(ex_result);
		}
		// if this is an operator
		if( ex_string[i] == '*' || ex_string[i] == '/' || ex_string[i] == '-' || ex_string[i] == '+' || ex_string[i] == '^'
           || ex_string[i] == ')'|| ex_string[i] == '('|| ex_string[i] == '>'|| ex_string[i] == '<'){
			exp_el[rhs_vals++].op_char = ex_string[i++];
		}
		// if this is a number
		else if( (ex_string[i] >= '0' && ex_string[i] <= '9') || ex_string[i] == '.'){ 
			if(ex_string[i] == '.') treat_as_float = 1;	// if there are numbers with decimal pts, treat the whole expression as float
			j = sscanf(&ex_string[i],"%d",&exp_el[rhs_vals].ivalue);
			j = sscanf(&ex_string[i],"%f",&exp_el[rhs_vals].fvalue);
			if(j != 1){
				beep();
				printf("Assignment error -4\n");
				ex_result.op_char = 'e';
				return(ex_result);	// some problem here
			}
			exp_el[rhs_vals].op_char = 'v';
			i++;
			rhs_vals++;
			while ( (ex_string[i] >= '0' && ex_string[i] <= '9') || ex_string[i]== '.'|| ex_string[i]== 'e'){
				if(ex_string[i] == '.') treat_as_float = 1;	// if there are numbers with decimal pts, treat the whole expression as float
				if(ex_string[i]== 'e'){
					if(ex_string[i+1] == '-' || ex_string[i+1] == '+') i++;	// skip a - or + after e
				}
				i++;	// skip rest of number
			}
		} else {
			// this may be a variable -- get the variable name
			varlen=0;
			while ( is_variable_char((char)ex_string[i]) == 1 && varlen < MAX_VAR_LENGTH-1){
				vname[varlen++] = ex_string[i++];
			}
			vname[varlen] = 0;
			
			arg_index = get_variable_index(vname,0);
			if(arg_index < 0) {
				beep();
				printf("Assignment error -- name: %s\n",vname);
				ex_result.op_char = 'e';
				return(ex_result);	// some problem here
			}
			exp_el[rhs_vals].ivalue = user_variables[arg_index].ivalue;
			exp_el[rhs_vals].fvalue = user_variables[arg_index].fvalue;
			if(user_variables[arg_index].is_float == -1) { // this is a string variable
				strcpy(&exp_el[rhs_vals].estring[0],&user_variables[arg_index].estring[0]); // copy the string too
				exp_el[rhs_vals].op_char = 's';
			} else {
				exp_el[rhs_vals].op_char = 'v';
			}
			rhs_vals++;
			//printf(" variable is %s; expression is %s\n", vname,&ex_string[i]);
		}
	}
	if(rhs_vals == 1 && (exp_el[0].op_char == 'v' || exp_el[0].op_char == 's')){ // simple assignment
		ex_result =  exp_el[0];
		//vprint(var_index);
		return(ex_result);
	}
	nestdepth = 1;
	
	while(nestdepth >0){
		for(i=0; i<rhs_vals; i++){
			if(exp_el[i].op_char == '(') {	// look for matching )
				nestdepth++;
            inside:		for(j=i+1; j<rhs_vals; j++){
                if(exp_el[j].op_char == '(') { // must be nested, start over
                    i = j;
                    nestdepth++;
                    goto inside;
                }
                if(exp_el[j].op_char == ')') { // matched pair, evaluate
                    ex_result = evaluate(i+1,j);
                    if( ex_result.op_char == 'e'){
                        beep();
                        printf("Assignment error -3\n");
                        ex_result.op_char = 'e';
                        return(ex_result);	// some problem here
                    }
                    
                    // now compress the expression
                    exp_el[i] = ex_result;
                    
                    for(k=j+1; k<rhs_vals; k++){
                        exp_el[i+k-j] = exp_el[k];
                    }
                    rhs_vals -= (j-i);
                    break;
                }
            }
				
			}
		}
		nestdepth--;
	}
	
	ex_result = evaluate(0,rhs_vals);
	return(ex_result);
    
}



Expression_Element evaluate(int start, int end)
{
	int i,div_mul_result=0,last_op;
	Expression_Element eres;
	float x,y;
	
	eres.ivalue = 0;
	eres.fvalue = 0.0;
	eres.op_char = 'e';
	
	
	// could begin with a unary operator
	if (exp_el[start].op_char == '-' && exp_el[start+1].op_char == 'v'){
		exp_el[start+1].ivalue = -exp_el[start+1].ivalue;
		exp_el[start+1].fvalue = -exp_el[start+1].fvalue;
		start++;
	}
    
	if (exp_el[start].op_char == '+' && exp_el[start+1].op_char == 'v'){
		start++;
	}
	
	last_op = -2;
	for(i=start+1; i < end; i+= 2){
		if(exp_el[i].op_char == 'v' || exp_el[i+1].op_char != 'v')
			return eres; // error
		if(last_op != i-2)
            div_mul_result = i-1;	// this to take care of 2*3*4/5 case
		if (exp_el[i].op_char == '^'){	
			x = exp_el[div_mul_result].ivalue;
			y = exp_el[i+1].ivalue;
			exp_el[div_mul_result].ivalue = pow(x,y);
			exp_el[div_mul_result].fvalue = pow(exp_el[div_mul_result].fvalue,exp_el[i+1].fvalue);
			last_op = i;
		} else if (exp_el[i].op_char == '*'){			
			exp_el[div_mul_result].ivalue *= exp_el[i+1].ivalue;
			exp_el[div_mul_result].fvalue *= exp_el[i+1].fvalue;
			last_op = i;
		} else if (exp_el[i].op_char == '/'){
			exp_el[div_mul_result].fvalue /= exp_el[i+1].fvalue;
			// What about integer division by zero??
			// Do usual integer arithmetic rules if the divisor is not zero
			// if it is zero, take the value calculated by the floating point operation
			if(exp_el[i+1].ivalue != 0)
				exp_el[div_mul_result].ivalue /= exp_el[i+1].ivalue;
			else
				exp_el[div_mul_result].ivalue = exp_el[div_mul_result].fvalue+0.5;
			
			last_op = i;
		}
	}
	
	if(exp_el[start].op_char == 'v'){
		eres.ivalue = exp_el[start].ivalue;
		eres.fvalue = exp_el[start].fvalue;
	}
	
	for(i=start+1; i < end; i+= 2){
		if(exp_el[i].op_char == 'v' || exp_el[i+1].op_char != 'v')
			return eres; // error
		if(exp_el[i].op_char == '+'){
			eres.ivalue += exp_el[i+1].ivalue;
			eres.fvalue += exp_el[i+1].fvalue;
		} else if (exp_el[i].op_char == '-'){
			eres.ivalue -= exp_el[i+1].ivalue;
			eres.fvalue -= exp_el[i+1].fvalue;
		}
        
		//printf("%c %d\n",eres.op_char,eres.ivalue);
	}	
	
	for(i=start+1; i < end; i+= 2){
		if(exp_el[i].op_char == 'v' || exp_el[i+1].op_char != 'v')
			return eres; // error
		if(exp_el[i].op_char == '<'){
			if(eres.fvalue<exp_el[i+1].fvalue){
				eres.ivalue = 1;
				eres.fvalue = 1.0;
			}else {
				eres.ivalue = 0;
				eres.fvalue = 0.0;
			}
		} else if (exp_el[i].op_char == '>'){
			if(eres.fvalue > exp_el[i+1].fvalue){
				eres.ivalue = 1;
				eres.fvalue = 1.0;
			}else {
				eres.ivalue = 0;
				eres.fvalue = 0.0;
			}
		}
        
		//printf("%c %d\n",eres.op_char,eres.ivalue);
	}	
    
	eres.op_char = 'v';
	return eres; 
	
    
}

int vprint(int index)
{
	if(user_variables[index].is_float > 0)
		printf("%s: %g\n", user_variables[index].vname,user_variables[index].fvalue);
	else if(user_variables[index].is_float == 0)
		printf("%s: %d\n", user_variables[index].vname,user_variables[index].ivalue);
	else
		printf("%s: %s\n", user_variables[index].vname,&user_variables[index].estring[0]);
	return 0;
}

/* ------------------------------------------------------------------------- */
/*									Commands								 */
/* ------------------------------------------------------------------------- */

/* ********** */
int pause_flag = 0;
char pause_string[CHPERLN];
int imp_pause(n,index)

int n,index;

{
    extern char cmnd[];
    
	strcpy(pause_string,&cmnd[index]);
	if(index == 0)
		sprintf(pause_string, "PAUSED");
	// this string will be displayed during pause
	pause_flag =1;
	update_status();
	return 0;
    
}
/* ********** */

int pmacro(int n)
{
    extern int maccount,macflag,macptr,macval;
    extern char* macbuf;
    
    if ( *macbuf == 0){
        beep();
        printf ("No Macro Defined.\n");
        return -1;
    }
    
    macro_line_number = lvar_max = loopdepth = var_char_count = inloop = 0;
    
    macflag = maccount = 1;
    macval = n;
    if (maccount <= 0) 
        maccount = 1;
    macptr = 0;
    return 0;
}
/* ********** */

int macro(int n, int index)
{
	extern unsigned char from_noprint;
	
	keylimit(-1);
	from_noprint = 1;
	rmacro(n,index);
	return 0;
    
}

/* ********** */


int rmacro(int n, int index)
{
    extern int maccount,macflag,macptr,macval,macincrement;
    extern char cmnd[];
    extern char* macbuf;
    
	int i;
	int j = 1;
	
	macro_line_number = lvar_max = loopdepth = var_char_count = inloop = 0;
    
    
    if ( *macbuf == 0){
		beep();
        printf ("No Macro Defined.\n");
        return 1;
    }
	
	macflag = 1;
    macval = 1;
    maccount = n/macincrement;
	if (n !=0) j = n;
    
	/* Check to see if there was a second argument */
	
	for ( i = index; cmnd[i] != EOL; i++) {
		if(cmnd[i] == ' ') { 
			sscanf(&cmnd[index],"%d %d",&i,&j);
			macval = i;
			maccount = (j - i)/macincrement + 1;
			break;
		}
	}
    if (maccount <= 0)  maccount = 1;
    macptr = 0;
	printf("Start Macro: from %d to %d, steps of %d.\n",macval,j,macincrement);
	return 0;
}

/* ********** */

int imacro(int n)			/* set a flag that determines if a macro checks for events during execution */
{
	extern int macro_stopable;
	
	macro_stopable = n;
	return 0;
	
}
/* ********** */
/*
 int stopmacro()
 {
 extern int macflag,macval,exflag;
 extern unsigned char from_noprint;
 if(macflag){
 macflag = 0;
 macro_line_number = lvar_max = loopdepth = var_char_count = inloop = 0;
 ifdepth=0;          // not inside any if condition
 if(from_noprint) {
 keylimit(-2);
 from_noprint = 0;
 }
 printf("Macro Stopped at Value %d.\n",macval);
 }
 if(exflag){
 exflag = 0;
 inloop = 0;
 ifdepth=0;           // not inside any if condition
 which_ex_buffer = -1;
 if_condition_met = 1;
 if(from_noprint) {
 keylimit(-2);
 from_noprint = 0;
 }
 printf("Execute aborted.\n");
 }
 
 return 0;
 
 }
 */
int stopmacro()
{
	extern int macflag,macval,exflag;
	extern unsigned char from_noprint;
    
	if(macflag){
		printf("Macro Stopped at Value %d.\n",macval);
	}
	if(exflag){
		printf("Execute aborted.\n");
	}
    
    macflag = 0;
    macro_line_number = lvar_max = loopdepth = var_char_count = inloop = 0;
    ifdepth=0;          // not inside any if condition
    ;
    exflag = 0;
    inloop = 0;
    ifdepth=0;           // not inside any if condition
    //which_ex_buffer = -1; // resetting this now leads to a crash when stop_on_error is set in certain cases -- ????
    if_condition_met = 1;
    if(from_noprint) {
        keylimit(-2);
        from_noprint = 0;
    }
    
	return 0;
    
}

/* ********** */

#define READMODE O_RDONLY  /* Read file mode. */

int execut(int n, int index)


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
	
	fd = open(fullname(&cmnd[index],MACROS_DATA),READMODE);
    
	if(fd == -1) {
		beep();
		printf("Execute File '%s' Not Found.\n",&cmnd[index]);
		//free(exbuf[which_ex_buffer]);
		which_ex_buffer--;
		return -1;
	}
	nread = read(fd,exbuf[which_ex_buffer],MBUFLEN);
	printf("%d Bytes Read.\n",nread);
	
 	/* the format of macro files has changed -- now they are formatted text files */
	/* previously, they were constant length files containing C strings */
	/* this code should read both formats */
	
	for(i=0; i<nread ; i++) {
		if( *(exbuf[which_ex_buffer]+i) == 0x0D || *(exbuf[which_ex_buffer]+i) == 0x0A )
			*(exbuf[which_ex_buffer]+i) = 0x00;		/* change CR or LF to null */
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


/* ********** */
int ifnotexist(int n, int index)
{
	extern int macflag,exflag;
	extern char cmnd[];     /* the command buffer */
    char tmptxt[CHPERLN];    // scratch space
	int fd,fd2;
	int this_test;
	
	
	if( (macflag == 0) && (exflag == 0)) {
		beep();
		printf("IFNEXS must be within a Macro.\n");
		
		return -1;
	}
    
    strcpy(tmptxt,&cmnd[index]);
	
	fd = open(fullname(&cmnd[index],SAVE_DATA),READMODE);
    fd2 = open(fullname(tmptxt,GET_DATA),READMODE);
	
	if (fd == -1 && fd2 == -1) {
		this_test = 1;
	} else {
		close(fd);
        close(fd2);
		this_test = 0;
	}
	if(ifdepth == 0) if_condition_met = this_test;
	else{	// we're nested -- check to make sure that one above is true
		if(if_condition[ifdepth-1]){
			if_condition_met = this_test;
		}	
	}
	
	if_condition[ifdepth] = this_test;
	ifdepth++;
    if(ifdepth >= NESTDEPTH){
        beep();
        printf("IF buffer overflow.\n");
        return -1;
    }
	printf("if condition: %d; depth %d\n",if_condition_met,ifdepth);
	return 0;
	
}

/* ********** */
int ifexist(int n, int index)
{
	extern int macflag,exflag;
	extern char cmnd[];     /* the command buffer */
    char tmptxt[CHPERLN];    // scratch space
	int fd,fd2;
	int this_test;
	
	
	if( (macflag == 0) && (exflag == 0)) {
		beep();
		printf("IFEXST must be within a Macro.\n");
		
		return -1;
	}
    
    strcpy(tmptxt,&cmnd[index]);
	
	fd = open(fullname(&cmnd[index],SAVE_DATA),READMODE);
    fd2 = open(fullname(tmptxt,GET_DATA),READMODE);
	if (fd == -1 && fd2 == -1) {
		this_test = 0;
	} else {
		close(fd);
        close(fd2);
		this_test = 1;
	}
	if(ifdepth == 0) if_condition_met = this_test;
	else{	// we're nested -- check to make sure that one above is true
		if(if_condition[ifdepth-1]){
			if_condition_met = this_test;
		}	
	}
	
	if_condition[ifdepth] = this_test;
	ifdepth++;
    if(ifdepth >= NESTDEPTH){
        beep();
        printf("IF buffer overflow.\n");
        return -1;
    }
	printf("if condition: %d; depth %d\n",if_condition_met,ifdepth);
	return 0;
}

/* ********** */

int ifnotdefined(int n, int index)	// set flag to use integer value of a variable
{
	extern int macflag,exflag;
	extern char cmnd[];     /* the command buffer */
	int this_test;
	int arg_index;
	
	if( (macflag == 0) && (exflag == 0)) {
		beep();
		printf("IFNDEF must be within a Macro.\n");
		return -1;
	}
	
	// Make the test to see if the argument exists
	arg_index = get_variable_index(&cmnd[index],0);
	if(arg_index < 0) {	// If variable wasn't defined then 
		this_test = 1;    // test returns true
	} else {
		this_test = 0;
	}
	
	// Set the LOOP conditions
	if(ifdepth == 0) if_condition_met = this_test;
	else{	// we're nested -- check to make sure that one above is true
		if(if_condition[ifdepth-1]){
			if_condition_met = this_test;
		}	
	}
	
	if_condition[ifdepth] = this_test;
	ifdepth++;
    if(ifdepth >= NESTDEPTH){
        beep();
        printf("IF buffer overflow.\n");
        return -1;
    }
	printf("if condition: %d; depth %d\n",if_condition_met,ifdepth);
	return 0;
	
}

/* ********** */

int ifdefined(int n, int index)	// set flag to use integer value of a variable
{
	extern int macflag,exflag;
	extern char cmnd[];     /* the command buffer */
	int this_test;
	int arg_index;
	
	if( (macflag == 0) && (exflag == 0)) {
		beep();
		printf("IFNDEF must be within a Macro.\n");
		return -1;
	}
	
	// Make the test to see if the argument exists
	arg_index = get_variable_index(&cmnd[index],0);
	if(arg_index < 0) {	// If variable wasn't defined then 
		this_test = 0;    // test returns true
	} else {
		this_test = 1;
	}
	
	// Set the LOOP conditions
	if(ifdepth == 0) if_condition_met = this_test;
	else{	// we're nested -- check to make sure that one above is true
		if(if_condition[ifdepth-1]){
			if_condition_met = this_test;
		}	
	}
	
	if_condition[ifdepth] = this_test;
	ifdepth++;
    if(ifdepth >= NESTDEPTH){
        beep();
        printf("IF buffer overflow.\n");
        return -1;
    }
	printf("if condition: %d; depth %d\n",if_condition_met,ifdepth);
	return 0;
	
}

/* ********** */

int ifcmnd(int n, int index)
{
	extern int macflag,exflag;
	extern char cmnd[];     /* the command buffer */
	
	Expression_Element evaluate_string(),ex_result;
	int this_test,i,j;
	
	if( (macflag == 0) && (exflag == 0)) {
		beep();
		printf("IF must be within a Macro.\n");
		
		return -1;
	}
	
	// get rid of spaces in the expression -- we just don't need them
	for(i=index; i< strlen(cmnd); i++){
		if(cmnd[i] == ' '){
			j= i;
			while(cmnd[j] != 0){
				cmnd[j] = cmnd[j+1];
				j++;
			}
			i--;	// maybe we have multiple spaces
		}
	}
	
	ex_result = evaluate_string(&cmnd[index]);
	
	if( ex_result.ivalue != 0){	// IF condition is met
		this_test = 1;
	} else {
		this_test = 0;
	}
	if(ifdepth == 0) if_condition_met = this_test;
	else{	// we're nested -- check to make sure that one above is true
		if(if_condition[ifdepth-1]){
			if_condition_met = this_test;
		}	
	}
    
	if_condition[ifdepth] = this_test;
	ifdepth++;
    if(ifdepth >= NESTDEPTH){
        beep();
        printf("IF buffer overflow.\n");
        return -1;
    }
	//printf("if condition: %d; depth %d\n",if_condition_met,ifdepth);
	return 0;
    
}

/* ********** */

int endifcmnd(int n, int index)
{
	if( ifdepth <1){
		beep();
		printf("IF nesting error,\n");
		printf("if condition: %d; depth %d\n",if_condition_met,ifdepth);
		ifdepth = 0;
		if_condition_met = 1;
		return 1;
	}
    
	ifdepth--;
	if(ifdepth == 0)
		if_condition_met = 1;
	else
		if_condition_met = if_condition[ifdepth-1];
	
	//printf("if condition: %d; depth %d\n",if_condition_met,ifdepth);
	return 0;
    
}

/* ********** */

int loop(int n, int index)
{
	int start,end,step,narg,k,l,m,match;
	char vname[COMLEN];
	
	extern int macflag,exflag;
	extern char cmnd[];     /* the command buffer */
	
	if( (macflag == 0) && (exflag == 0)) {
		beep();
		printf("Loops must be within a Macro.\n");
		
		return -1;
	}
	
	narg = sscanf(&cmnd[index],"%s %d %d %d",vname,&start,&end,&step);
	if( narg < 3 ){
		beep();
		printf("Not enough arguments for LOOP.\n");
		return -2;
	}
	if( narg == 3 )
		step = 1;
	
	if( ((start > end) && step > 0) ||
       ((start < end) && step < 0) ||
       (step == 0) ) {
        beep();
        printf("Invalid arguments for LOOP.\n");
        return -3;
	}
	
	
	// look through the loop variables to see if this one already exists 
	k=m=0;
	for (n = 0; n<lvar_max; n++) {	
		match = 1;
		for( l = 0; *(variable_names+m) != '\0';l++) {
			if( *(variable_names+m)	!= vname[l] )
				match = 0;
			m++;
		}
		m++;
		// printf("\n%d length\n",l); 
        
		if( vname[l] == '\0' && match == 1) {	/* if true, we have the match */
			k=1;
			break;
		}
	}
    
	
	if( k == 0) {								// if new name, add it to the list 
		for( n=0; vname[n] != '\0'; n++)
			*(variable_names+n+var_char_count) = vname[n];
		*(variable_names+n+var_char_count) = vname[n];
		var_char_count += n+1;
		n = lvar_max;
		lvar_max++;
	}
	// n is the location in the variable list arrays for this variable 
	
	lvar[loopdepth] = n;
	
	// printf("LOOP ------- loopdepth:%d, variable:%d\n",loopdepth,n); 
	
	endval[n] = end;
	stepval[n] = step;
	startval[n] = start;
	
	if( exflag ) 
		branchto[n] = exe_line_number[which_ex_buffer];	// loop to the line after the loop command 
	else
		branchto[n] = macro_line_number;// loop to the line after the loop command 
    
	loopdepth++;		// the depth of the loop 
    // note that no overflow checking is done -- nesting greater that NESTDEPTH
    // will crash
    
	inloop=1;
	
	/* printf("%s %d %d %d var,start,end,step\n",vname,start,end,step); */
	return 0;
    
}
/* ********** */

int loopbreak()			// break out of the current loop
{
	int i,j;	
	char loopnd_string[7]={"LOOPND"};
	
	extern char* macbuf;    // the macro buffer 
	extern char* exbuf[];    // the macro buffer
	extern int exflag,exptr[],exval[];
	extern int macptr;
	
	
	if(loopdepth <= 0) {
		beep();
		printf("Loop break not in a loop\n");
		return -1;
	}
	
	//printf("%d\n" ,loopnd_string[6]);
	
	if(exflag) {	// loop in execute
		// have to set macro_line_number to point past loopnd
		exptr[which_ex_buffer] = 0;
		for( i=0; i<exe_line_number[which_ex_buffer]; i++) {
			while ( *(exbuf[which_ex_buffer] + exptr[which_ex_buffer]) != 0 )
				exptr[which_ex_buffer]++;
			exptr[which_ex_buffer]++;
		}
		// exptr now points to the next command after the loopbr
		// go through commands in the macro buffer until loopnd is found
		i = j= 0;
		// ignore spaces or tabs at the beginning of a command
		while( *(exbuf[which_ex_buffer]+exptr[which_ex_buffer]+i) == ' ' || *(exbuf[which_ex_buffer]+exptr[which_ex_buffer]+i) == '\t')i++; 
		
		//printf("point to:%s -- line %d\n",macbuf+macptr+i,macro_line_number);
        
		while ( *(exbuf[which_ex_buffer]+exptr[which_ex_buffer]+i) != EOL && *(exbuf[which_ex_buffer]+exptr[which_ex_buffer]+i) != ' ' /*&& *(exbuf[which_ex_buffer]+exptr[which_ex_buffer]+i) != ';'*/){
			if ( toupper(*(exbuf[which_ex_buffer]+exptr[which_ex_buffer]+i)) !=  loopnd_string[j] ) {
				// this is not loopnd -- look at next command 
				i = j= 0;
                
				while ( *(exbuf[which_ex_buffer] + exptr[which_ex_buffer]) != 0 )
					exptr[which_ex_buffer]++;
				exptr[which_ex_buffer]++;
				// ignore spaces or tabs at the beginning of a command
				while( *(exbuf[which_ex_buffer]+exptr[which_ex_buffer]+i) == ' ' || *(exbuf[which_ex_buffer]+exptr[which_ex_buffer]+i) == '\t')i++; 
				//printf("now to:%s -- line_number %d\n",macbuf+macptr+i,macro_line_number);
				exe_line_number[which_ex_buffer]++;
			} else {
				i++;
				j++;
				//printf("%d\n",i);
			}
        }
        
		exe_line_number[which_ex_buffer]++;	// point to one after
		loopdepth--;
		if(loopdepth == 0) {
			inloop = 0;
		}	
		if ( *(exbuf[which_ex_buffer] + ++exptr[which_ex_buffer]) == EOL && (executedepth[which_ex_buffer] == loopdepth) ) {
            //if ( *(exbuf[which_ex_buffer] + ++exptr[which_ex_buffer]) == EOL && inloop == 0 ) {
			exval[which_ex_buffer]++;
			exptr[which_ex_buffer] = 0;
			exflag = 0;
			if_condition_met = 1;
		}	
        
	} else {		// loop in macro
		// have to set macro_line_number to point past loopnd
		macptr = 0;
		for( i=0; i<macro_line_number; i++) {
			while ( *(macbuf + macptr) != 0 )
				macptr++;
			macptr++;
		}
		// macptr now points to the next command
		// go through commands in the macro buffer until loopnd is found
		i = j= 0;
		// ignore spaces or tabs at the beginning of a command
		while( *(macbuf+macptr+i) == ' ' || *(macbuf+macptr+i) == '\t')i++; 
		
		//printf("point to:%s -- line %d\n",macbuf+macptr+i,macro_line_number);
        
		while ( *(macbuf+macptr+i) != EOL && *(macbuf+macptr+i) != ' ' /*&& *(macbuf+macptr+i) != ';'*/){
			if ( toupper(*(macbuf+macptr+i)) !=  loopnd_string[j] ) {
				// next command 
				i = j= 0;
                
				while ( *(macbuf + macptr) != 0 )
					macptr++;
				macptr++;
				// ignore spaces or tabs at the beginning of a command
				while( *(macbuf+macptr+i) == ' ' || *(macbuf+macptr+i) == '\t')i++; 
                
				
				//printf("now to:%s -- line_number %d\n",macbuf+macptr+i,macro_line_number);
				macro_line_number++;
			} else {
				i++;
				j++;
				//printf("%d\n",i);
			}
        }
        
		macro_line_number++;	// point to one after
		loopdepth--;
		if(loopdepth == 0) {
			inloop = 0;
		}	
        
	}
    // presumably this is within an if statement
    // so do the endif bookkeeping
    if( ifdepth >0 ){
        endifcmnd(0,0);
    }
    return 0;
}

/* ********** */

int loopend()
{
	int loopvar_index;
	extern int exflag;
    
	if(loopdepth <= 0) {
		beep();
		printf("Loop Mismatch\n");
		return -1;
	}
	
    /*	if( (macflag == 0) && (exflag == 0) ) {
     beep();
     printf("Loops must be within a Macro.\n");
     
     return -2;
     }
     */
	
	/* loopvar_index = loopdepth-1 + loopvar-loopdepth; */
	loopvar_index = lvar[loopdepth-1];
	
	/*printf("LOOP END -----loopdepth:%d, variable:%d\n",loopdepth-1,loopvar_index);*/
	
	startval[loopvar_index] += stepval[loopvar_index];
	
	if (stepval[loopvar_index ] > 0) {
		if( startval[loopvar_index] <= endval[loopvar_index] ) {
			if( exflag )
				exe_line_number[which_ex_buffer] = branchto[loopvar_index];
			else
				macro_line_number = branchto[loopvar_index];
			return 0;
		}
	} else {
		if( startval[loopvar_index] >= endval[loopvar_index] ) {
			if( exflag )
				exe_line_number[which_ex_buffer] = branchto[loopvar_index];
			else
				macro_line_number = branchto[loopvar_index];
			return 0;
		}
	}
	
	/* end of loop */
    
	loopdepth--;
	if(loopdepth == 0) {
		inloop = 0;
	}
	return 0;
}

/* ********** */

/*
 These commands SAVE and GET a file called OMA Settings from the
 with the directory specified in the SETTINGS FILE part of the file settings menu.
 This was formerly the GRAPHICS FILES menu.
 */
int getsettings(int n,int index)	
{
	extern char txt[];
	extern char cmnd[];
	int err;
	
	if(index == 0){	// no file name was specified, use the default name and get it from the oma application
		strcpy(txt,SETTINGSFILE);
		err = loadprefs (txt);
		return err;
	} else { // otherwise, add the prefix and suffix and use the name specified
		err = loadprefs(fullname(&cmnd[index], SETTINGS_DATA));
		return err;
	}
    
}

/* ********** */

int savsettings(int n,int index)
{
	extern char txt[];
	extern char cmnd[];
	int err;
	
	if(index == 0){	// no file name was specified, use the default name and put it in the oma application
		strcpy(txt,SETTINGSFILE);
		err = saveprefs (txt);
		return err;
	} else { // otherwise, add the prefix and suffix and use the name specified
		err = saveprefs(fullname(&cmnd[index], SETTINGS_DATA));
		return err;
	}
}

/* ********** */

int variab(int n, int index)	// print values of defined variables
{
	int i;
	for(i=0; i<num_variables; i++){
		vprint(i);
	}
	return 0;
    
}

/* ********** */

int vfloat(int n, int index)	// set flag to use floating pt value of a variable
{
	int arg_index;
	extern char cmnd[];
	
	arg_index = get_variable_index(&cmnd[index],0);
	
	if(arg_index < 0) {	// it wasn't defined yet, so define it and set it to 0
		arg_index = get_variable_index(&cmnd[index],1);
		if(arg_index < 0) {	// maybe there was still a problem
            beep();
            printf("Assignment error arg index:%d\n",arg_index);
            return arg_index;
		}
	}
	user_variables[arg_index].is_float=1;
	
	vprint(arg_index);
    
	return 0;
}

/* ********** */

int vint(int n, int index)	// set flag to use integer value of a variable
{
	int arg_index;
	extern char cmnd[];
	
	arg_index = get_variable_index(&cmnd[index],0);
	
	if(arg_index < 0) {	// it wasn't defined yet, so define it and set it to 0
		arg_index = get_variable_index(&cmnd[index],1);
		if(arg_index < 0) {	// maybe there was still a problem
            beep();
            printf("Assignment error arg index:%d\n",arg_index);
            return arg_index;
		}
	}
	user_variables[arg_index].is_float=0;
	
	vprint(arg_index);
	return 0;
    
}
/* ********** */

/* STOP_ON_ERROR flag
 If flag = 1, macro or execute commands will stop on error conditions. If flag = 0
 (default) execution will continue. 
 */

int stoponerror(int n, int index)	
{
	if(n == 0){
		stop_on_error = 0;
        printf("Macros will not stop on error condition.\n");
	}else{
		stop_on_error = 1;
        printf("Macros will stop on error condition.\n");
    }
	return 0;
    
}

/* ********** */

int prflag(int n, int index)	// set flag to use enable/disable printing
{
	if(n == 0)
		no_print=1;
	else
		no_print=0;
    
	return 0;
    
}

//------------------------------------------------------------------------
float ave_in_rect()
/*  Takes current rectangle and returns the average 
 Used to fill in %a value */
{
	int i,j,icount;
	long int sum;
	DATAWORD idat(int,int);
	
	extern Point substart,subend;
	float scalarvalue;
	
	sum = 0;
	icount = 0;
	for (i = substart.v; i<=subend.v; i++){
		for (j = substart.h; j<=subend.h; j++){
			sum += idat(i,j);
			icount++;
		}
	}
	scalarvalue = (float)sum/icount;
	//printf("	%.2f\n",scalarvalue);
	return scalarvalue;
}

float rms_in_rect()
/*  Takes current rectangle and returns the average 
 Used to fill in %a value */
{
	int nt,nc,icount,datval;
	double ave,rms;		
	DATAWORD idat(int,int);
	
	extern Point substart,subend;
	float scalarvalue;
	
	ave = rms = 0.0;
	icount = 0;
	
	for(nt=substart.v; nt<=subend.v; nt++) {
		for(nc=substart.h; nc<=subend.h; nc++) {
			datval = idat(nt,nc);
			ave += datval;					/* average */
			rms += datval*datval;			/* rms */
			icount++;						/* number of points */
		}
	}
	ave = ave/(float)icount;
    
	rms = rms/icount - ave*ave;	
	rms = sqrt(rms);
    
	
	
	scalarvalue = rms;
	//printf("	%.2f\n",scalarvalue);
	return scalarvalue;
}

