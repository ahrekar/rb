#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <asm/types.h>

#define COMMENTCHAR '#'

int main (int argc, char **argv)
{
	FILE *data, *background, *output;
	char buffer[1024];
	int aout[3];
	float photoCurrent[3],stdDev[3];

	if (argc==4) {
		data=fopen(argv[1],"r");
		if (!data) {
			printf("Unable to open file '");
			printf("%s",argv[1]);
			printf("'.\nPlease check that access is enabled and\n");
			printf("the file name is correct.\n");
			exit(1);
		}
		background=fopen(argv[2],"r");
		if (!background) {
			printf("Unable to open file '");
			printf("%s",argv[2]);
			printf("'.\nPlease check that access is enabled and\n");
			printf("the file name is correct.\n");
			exit(1);
		}
		output=fopen(argv[3],"w");
		if (!output) {
			printf("Unable to open file '");
			printf("%s",argv[3]);
			printf("'.\n");
			exit(1);
		}
	} else {
		printf("Usage:\n$ sudo ./removeBackground <dataFile> <backgroundFile> <outputFile>\n");
		return 0;
	}

	fprintf(output,"#\n");
	fprintf(output,"# This file was created by subtracting:\n");
	fprintf(output,"#    The background file: %s \n", argv[2]);
	fprintf(output,"#    from the data file:  %s \n", argv[1]);
	fprintf(output,"#\n");

	// Copy all the header information to the output file.
	while(fgetc(data) == COMMENTCHAR){ // While the first character is a # (the COMMENTCHAR)
		fgets(buffer,1024,background); // Go to the next line.
		fgets(buffer,1024,data); // Go to the next line.
		fprintf(output,"#");
		fputs(buffer,output);
	}

	// Copy over the header line. Note that if this is from older data files that we have taken
	// (before 8/14/15) this will need to be adjusted because the header line was still
	// commented out at that time.
	fgets(buffer,1024,background); 
	fgets(buffer,1024,data); 
	fputs(buffer,output);

	// Line by line, grab the data and background numbers and manipulate them as needed
	while(fscanf(data,"%i\t%f\t%f\n",aout,photoCurrent,stdDev) != EOF){
		fscanf(background,"%i\t%f\t%f\n",&aout[1],&photoCurrent[1],&stdDev[1]);
		aout[2] = aout[0];
		photoCurrent[2] = photoCurrent[0] - photoCurrent[1];
		stdDev[2] = sqrt(stdDev[0]*stdDev[0] + stdDev[1]*stdDev[1]);
		fprintf(output,"%d\t%f\t%f\n",aout[2],photoCurrent[2],stdDev[2]);
	}

	fclose(data);
	fclose(background);
	fclose(output);

	return 0;
}
