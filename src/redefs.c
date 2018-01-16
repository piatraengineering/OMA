#ifdef DO_MACH_O
	#include "impdefs.h"
#else
	#include <MacHeadersCarbon.h>
#endif

#define MAXNUMLINES 1000		/* the number of lines that will appear in the command window */

Boolean is_special = false;
extern TXNOffset tostart, toend ;
extern TXNObject	object;
extern int no_print;


int set_text_special(int flag)
{

TXNTypeAttributes	typeAttr;
OSStatus  status;
RGBColor  fontColor = { 0xFFFF, 0, 0 };
RGBColor  regularColor = { 0, 0, 0 };

	if(flag == 1){

				typeAttr.tag = kTXNQDFontColorAttribute;
				typeAttr.size = kTXNQDFontColorAttributeSize;
				typeAttr.data.dataValue = (unsigned long) &fontColor;

				// Set the style attributes
				status = TXNSetTypeAttributes(object, 1, &typeAttr, kTXNUseCurrentSelection, kTXNUseCurrentSelection);
    } else {
				

				typeAttr.tag = kTXNQDFontColorAttribute;
				typeAttr.size = kTXNQDFontColorAttributeSize;
				typeAttr.data.dataValue = (unsigned long)&regularColor;

				// Set the style attributes
				status = TXNSetTypeAttributes(object, 1, &typeAttr, kTXNUseCurrentSelection, kTXNUseCurrentSelection);
				if (status != noErr) SysBeep(1);
    
    }


	return 0;

}


int set_text_color(int flag)
{
	TXNTypeAttributes	typeAttr;
	OSStatus  status;
	
	RGBColor  Col_Black = { 0, 0, 0 };	
	RGBColor  Col_Grey = { 0x3333, 0x3333, 0x3333 };
	RGBColor  Col_Red = { 0xFFFF, 0, 0 };
	RGBColor  Col_Orange = { 0xFFFF, 0x6666, 0x3333 };
	RGBColor  Col_Yellow = { 0xFFFF, 0xFFFF, 0 };
	RGBColor  Col_Green = { 0, 0x9999, 0 };
	RGBColor  Col_Blue = { 0, 0, 0xFFFF };
	RGBColor  Col_Indigo= { 0x3333, 0x3333, 0x9999 };
	
	typeAttr.tag = kTXNQDFontColorAttribute;
	typeAttr.size = kTXNQDFontColorAttributeSize;
	switch(flag) {
		case RED:
			typeAttr.data.dataValue = (unsigned long) &Col_Red;
			break;
		case ORANGE:
			typeAttr.data.dataValue = (unsigned long) &Col_Orange;
			break;
		case YELLOW:
			typeAttr.data.dataValue = (unsigned long) &Col_Yellow;
			break;
		case GREEN:
			typeAttr.data.dataValue = (unsigned long) &Col_Green;
			break;
		case BLUE:
			typeAttr.data.dataValue = (unsigned long) &Col_Blue;
			break;
		case INDIGO:	
			typeAttr.data.dataValue = (unsigned long) &Col_Indigo;
			break;
		case GREY:	
			typeAttr.data.dataValue = (unsigned long) &Col_Grey;
			break;
		default:
		case BLACK:
			typeAttr.data.dataValue = (unsigned long) &Col_Black;
			break;
	}
	status = TXNSetTypeAttributes(object, 1, &typeAttr, kTXNUseCurrentSelection, kTXNUseCurrentSelection);
	return OMA_OK;
}
/* ********** */


int printf(string,a0,a1,a2,a3,a4,a5)
char const string[];
double *a0,*a1,*a2,*a3,*a4,*a5;
{
	char result[512];
	int i;
	
	extern	Boolean	printall;
	//extern TEHandle		TextH;					/* The TextEdit handle */

	if(!printall) return 0;
	if(no_print) return 0;
	sprintf(result,string,a0,a1,a2,a3,a4,a5);
	i = 0;
	while (result[i]) {
		if(result[i] == 10) result[i] = 13;
		i++;
	}	
	
	TXNSetData(object,kTXNTextData,(void *)result, strlen(result), kTXNEndOffset, kTXNEndOffset); 
		if(is_special){
			TXNSetSelection(object,kTXNEndOffset, kTXNEndOffset); 
			TXNGetSelection(object,&tostart,&toend);		
			TXNSetSelection (object,tostart-strlen(result),toend);
			set_text_special(1);
			is_special = false;
			TXNSetSelection (object,tostart,toend);
			set_text_special(0);
		}
	return 0;
	
}

int pprintf( string,a0,a1,a2,a3,a4,a5)		/* priority printing! */
char const string[];
double *a0,*a1,*a2,*a3,*a4,*a5;

{
	char result[512];
	int i;
	
	//extern TEHandle		TextH;					/* The TextEdit handle */

	if(no_print) return 0;

	sprintf(result,string,a0,a1,a2,a3,a4,a5);
	i = 0;
	while (result[i]) {
		if(result[i] == 10) result[i] = 13;
		i++;
	}
	
	TXNSetData(object,kTXNTextData,(void *)result, strlen(result), kTXNEndOffset, kTXNEndOffset); 
		if(is_special){
			TXNSetSelection(object,kTXNEndOffset, kTXNEndOffset); 
			TXNGetSelection(object,&tostart,&toend);		
			TXNSetSelection (object,tostart-strlen(result),toend);
			set_text_special(1);
			is_special = false;
			TXNSetSelection (object,tostart,toend);
			set_text_special(0);
		}

	return 0;
	
}

/* ********** */

int cprintf( col_flag, string,a0,a1,a2,a3,a4,a5)		/* Colour printing */
int col_flag;
char const string[];
double *a0,*a1,*a2,*a3,*a4,*a5;
{
	char result[512];
	int i;
	
	if(no_print) return 0;
	
	sprintf(result,string,a0,a1,a2,a3,a4,a5);
	i = 0;
	while (result[i]) {
		if(result[i] == 10) result[i] = 13;
		i++;
	}
	
	TXNSetData(object,kTXNTextData,(void *)result, strlen(result), kTXNEndOffset, kTXNEndOffset); 
	TXNSetSelection(object,kTXNEndOffset, kTXNEndOffset); 
	TXNGetSelection(object,&tostart,&toend);		
	TXNSetSelection (object,tostart-strlen(result),toend);
	set_text_color(col_flag);
	TXNSetSelection (object,tostart,toend);
	set_text_color(BLACK);
	
	return OMA_OK;
	
}


