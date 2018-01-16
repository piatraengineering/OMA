int echo_color();
{{"ECHOCOLOR      "}, echo_color},

// *********************************************************************/
// **         Functions for colour ECHOCOLOR of text to command window     **/
// *********************************************************************/
#define BLACK 0
#define GREY 7
#define RED 1
#define ORANGE 2
#define YELLOW 3
#define GREEN 4
#define BLUE 5
#define INDIGO 6


// These may not be necessary depending on where you put the following functions
extern TXNOffset tostart, toend ;
extern TXNObject	object;
extern int no_print;


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
	TXNSetSelection (object,tostart-1,toend);
	set_text_color(BLACK);
	
	return OMA_OK;
	
}

int echo_color(int n,int index)
{
	int narg;
	int colflag;
	char arg1[512];
	char arg2[512];
	char *remains;
	
	// Lets keep a copy of the arguments for later
	strcpy(scratch, &cmnd[index]);
	
	// Read in the first argument as a string
	narg = sscanf(&cmnd[index],"%s %s", arg1, arg2);
	
	colflag = BLACK;
	if (!strncmp(arg1, "R", 1) ||  !strncmp(arg1, "r", 1) ) colflag = RED;
	if (!strncmp(arg1, "O", 1) || !strncmp(arg1, "o", 1) ) colflag = ORANGE;
	if (!strncmp(arg1, "Y", 1) || !strncmp(arg1, "y", 1) ) colflag = YELLOW;
	if (!strncmp(arg1, "G", 1) || !strncmp(arg1, "g", 1) ) colflag = GREEN;
	if (!strncmp(arg1, "B", 1) || !strncmp(arg1, "b", 1) ) colflag = BLUE;
	if (!strncmp(arg1, "I", 1) || !strncmp(arg1, "i", 1) ) colflag = INDIGO;
	
	// Print the rest of the line after the first argument
	remains = strchr(scratch, ' ');
	if (narg >= 2)  cprintf(colflag, "%s\n", remains);
	return OMA_OK;
}
