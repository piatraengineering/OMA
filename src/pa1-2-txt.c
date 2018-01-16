#include <stdio.h>
#include <fcntl.h>
#define PMODE 0666 

/* 
Convert a pa1 palette file to a text file
*/

main()
{
	char filename[128];
	char outfile[128];
	FILE *fp, *fopen();
	unsigned int red[256],green[256],blue[256];
	unsigned char r[256],g[256],b[256];
	unsigned int fd;
	int i;
	
	printf("pa1 file:\n");
	scanf("%s",filename);
	printf("Output text file:\n");
	scanf("%s",outfile);


	if ((fp = fopen(filename,"r")) == NULL) {
		printf(" File Not Found.\n");
		return -1;
	}

	for(i=0; i<256; i++) {
		fscanf(fp,"%c",&r[i]);
		red[i] = r[i];
	}
	
	for(i=0; i<256; i++) {
		fscanf(fp,"%c",&g[i]);
		green[i] = g[i];
	}
	
	for(i=0; i<256; i++) {
		fscanf(fp,"%c",&b[i]);
		blue[i] = b[i];
	}
	
	fclose(fp);
	
	if ((fp = fopen(outfile,"w")) == NULL) {
		printf(" File Not Found.\n");
		return -1;
	}

	for(i=0; i<256; i++) {
		fprintf(fp,"%d\t%d\t%d\n",red[i],green[i],blue[i]);
	}
	
	fclose(fp);

}