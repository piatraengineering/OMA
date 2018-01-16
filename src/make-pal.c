#include <stdio.h>
#include <fcntl.h>
#define PMODE 0666 

/* 
Make a binary palette file from a text file with R G B values
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
	
	printf("Text file:\n");
	scanf("%s",filename);
	printf("Pal file:\n");
	scanf("%s",outfile);


	if ((fp = fopen(filename,"r")) == NULL) {
		printf(" File Not Found.\n");
		return -1;
	}

	for(i=0; i<256; i++) {
		fscanf(fp,"%d %d %d",&red[i],&green[i],&blue[i]);
		printf("%d %d %d\n",red[i],green[i],blue[i]);
		r[i]=red[i];
		g[i]=green[i];
		b[i]=blue[i];
	}
	fclose(fp);
	
	if ((fp = fopen(outfile,"w")) == NULL) {
		printf(" File Not Found.\n");
		return -1;
	}

	for(i=0; i<256; i++) {
		fprintf(fp,"%c",r[i]);
	}
	for(i=0; i<256; i++) {
		fprintf(fp,"%c",g[i]);
	}
	for(i=0; i<256; i++) {
		fprintf(fp,"%c",b[i]);
	}
	
	fclose(fp);

}