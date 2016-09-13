/*
   Program to record polarization.
   RasPi connected to USB 1208LS.

   FARADAY SCAN


   use Aout 0 to set laser wavelength. see page 98-100
   usage
   $ sudo ./faradayscan <aoutstart> <aoutstop> <deltaaout> <comments_no_spaces>


   2015-12-31
   added error calculations. see page 5 and 6 of "FALL15" lab book
 */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>
#include "mathTools.h" //includes stdDeviation

#define PI 3.14159265358979
#define NUMSTEPS 350	
#define STEPSIZE 25
#define STEPSPERREV 350.0
#define WAITTIME 2

#define BUFSIZE 1024

int plotData(char* fileName);
int calculateNumberDensity(char* fileName);
int recordNumberDensity(char* fileName);
int analyzeData(char* fileName,int dataPtsPerRev, int revolutions, int numAouts);
int	readInData(char* fileName,int totalDatapoints, int numAouts, int* aouts, int* steps, float* intensity, float* inensityErr);
float calculateOneSumTerm(int trigFunc, int dataPointsPerRevolution,int revolutions,float intensity, float i,int k);
float calculateOneSumTermError(int trigFunc, int posOrNeg,int dataPointsPerRevolution, int revolutions, float intensity,float intensityErr, float i, float iErr, int k);
int fourierAnalysis(int dataPointsPerRevolution, int revolutions, int* steps, float* intensity, float* intensityErr, float* fourierCoefficients, float* fcErr);

int main (int argc, char **argv)
{
	char fileName[1024];
	if (argc==2){
		strcpy(fileName,argv[1]);
	} else { 
		printf("usage '~$ sudo ./faradayscan <fileName>'\n");
		return 1;
	}

	int dataPointsPerRev=14;
	int revolutions=1;
	int numAouts=5;
	printf("Processing Data...\n");
	analyzeData(fileName,dataPointsPerRev,revolutions,numAouts);
/*
	plotData(fileName);
	calculateNumberDensity(fileName);
	recordNumberDensity(fileName);
*/
	return 0;
}

// Okay, so gnuplot's fitting function isn't behaving like I want it to
// (not giving me "reasonable" answers), so I'm hacking together
// a quick automated data fitting scheme. It involves using Wolfram,
// so it's going to be kind of messy.
int calculateNumberDensity(char* fileName){
	int i=0;
	char buffer[BUFSIZE];
	//char script[BUFSIZE]="\\home\\pi\\RbControl\\shortGetFit.wl";
	FILE *wolfram;
	wolfram = popen("wolfram >> /dev/null","w"); 

	if (wolfram != NULL){
		sprintf(buffer, "faradayData=Import[\"%s\",\"Data\"]\n", fileName);
		fprintf(wolfram, buffer);

		//sprintf(buffer, "Get[\"%s\"]",script);
		//Removes lines from the file (This one gets rid of the comments)
		//fprintf(wolfram, "faradayData=Delete[faradayData,{{1},{2},{3},{4},{5},{6},{7},{8}}]\n");			
		for(i=0;i<10;i++){
			fprintf(wolfram, "faradayData=Delete[faradayData,{{1}}]\n");			
			
		}
		//Removes unneccesary columns from the file
		for(i=0;i<5;i++){
			fprintf(wolfram, "faradayData=Drop[faradayData,None,{2}]\n");
		}
		fprintf(wolfram, "faradayData=Drop[faradayData,None,{3}]\n");
		//Removes lines from the file (This one gets rid of the data close to resonance)
		//fprintf(wolfram, "faradayData=Delete[faradayData,{{8},{9},{10},{11}}]\n");
		fprintf(wolfram, "l=2.8\n");
		fprintf(wolfram, "c=2.9979*^10\n");
		fprintf(wolfram, "re=2.8179*^-13\n");
		fprintf(wolfram, "fge=0.34231\n");
		fprintf(wolfram, "k=4/3\n");
		fprintf(wolfram, "Mb=9.2740*^-24\n");
		fprintf(wolfram, "B=2.08*^-2\n");
		fprintf(wolfram, "h=6.6261*^-34\n");
		fprintf(wolfram, "vo=3.77107*^14\n");
		fprintf(wolfram, "pi=3.14.159265\n");
		fprintf(wolfram, "aoutConv=-.0266\n");
		fprintf(wolfram, "aoutIntercept=14.961\n");
		fprintf(wolfram, "const=l*c*re*fge*k*Mb*B/(4*pi*h*vo)\n");
		fprintf(wolfram, "model=a+b*const*(vo+(aoutConv*detune+aoutIntercept)*1*^9)/((aoutConv*detune+aoutIntercept)*1*^9)^2+d*(vo+(aoutConv*detune+aoutIntercept))^5/((aoutConv*detune+aoutIntercept))^4\n");
		fprintf(wolfram, "vect=FindFit[faradayData,model,{a,b,d},detune]\n");
		fprintf(wolfram, "Replace[a,vect]>>fitParams.txt\n");
		fprintf(wolfram, "Replace[b,vect]>>>fitParams.txt\n");
		fprintf(wolfram, "Replace[d,vect]>>>fitParams.txt\n");
	}
	return pclose(wolfram);
}

int recordNumberDensity(char* fileName){
	char analysisFileName[1024];
	strcpy(analysisFileName,fileName);
	char* extensionStart=strstr(analysisFileName,".dat");
	strcpy(extensionStart,"analysis.dat");
	float a,b,c;
	int bExp,cExp;

	FILE* data = fopen("fitParams.txt","r");
	if (!data) {
		printf("Unable to open file %s\n",fileName);
		exit(1);
	}
	fscanf(data,"%f\n%f*^%d\n%f*^%d\n",	&a,&b,&bExp,&c,&cExp);
	fclose(data);

	FILE* dataSummary;
	// Record the results along with the raw data in a file.
	dataSummary=fopen(analysisFileName,"w");
	if (!dataSummary) {
		printf("Unable to open file: %s\n", analysisFileName);
		exit(1);
	}
	fprintf(dataSummary,"#File\t%s\n",analysisFileName);
	fprintf(dataSummary,"theta_o\tN\tdetuneFourthTerm\n");
	fprintf(dataSummary,"%2.2E\t%2.2E\t%2.2E\n",a,b*pow(10,bExp),c*pow(10,cExp));
	printf("theta_o\tdetuneSquare\tdetuneFourth\n");
	printf("%2.2E\t%2.2E\t%2.2E\n",a,b*pow(10,bExp),c*pow(10,cExp));
	fclose(dataSummary);

	return 0;
}

int plotData(char* fileName){
	char buffer[BUFSIZE];
	FILE *gnuplot;
	gnuplot = popen("gnuplot","w"); 

	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb size 100,28\n");
		fprintf(gnuplot, "set output\n");			

		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Aout (Detuning)'\n");			
		fprintf(gnuplot, "set ylabel 'Theta'\n");			
		fprintf(gnuplot, "set xrange [0:1024] reverse\n");			
		sprintf(buffer, "plot '%s' using 1:7:8 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s.png'\n", fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:7:8 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
	}
	return pclose(gnuplot);
}

int analyzeData(char* fileName, int dataPointsPerRevolution, int revolutions, int numAouts){
	int totalDatapoints=dataPointsPerRevolution*revolutions;
	int cos=0;
	int sin=dataPointsPerRevolution/2;
	int pos=0;
	int neg=dataPointsPerRevolution;

	char* extensionStart;

	FILE* rawData = fopen(fileName,"r");
	if (!rawData) {
		printf("Unable to open file %s\n",fileName);
		exit(1);
	}
	int* aouts=calloc(totalDatapoints*numAouts,sizeof(float)); 
	float* intensity=calloc(totalDatapoints*numAouts,sizeof(float));
	float* intensityErr=calloc(totalDatapoints*numAouts,sizeof(float));
	int* steps=calloc(totalDatapoints*numAouts,sizeof(float)); 

	printf("Reading in data...\n");
	readInData(fileName,totalDatapoints,numAouts,aouts,steps,intensity,intensityErr);
	printf("Data read in.\n");

	extensionStart=strstr(fileName,".dat");
	strcpy(extensionStart,"Analysis.dat");

	FILE* analysis = fopen(fileName,"w");
	if (!analysis) {
		printf("Unable to open file %s\n",fileName);
		exit(1);
	}

	fprintf(analysis,"Aout\tc0\ts4\td-s4\tc4\td-c4\tangle\tangleError\n");
	float* fourierCoefficients = calloc(totalDatapoints,sizeof(float));
	float* fcErr = calloc(totalDatapoints*2,sizeof(float));
	int i;
	float c0,s4,c4,angle,stderrangle=0;

	printf("Beginning Data Analysis...\n");
	for(i=0;i<numAouts;i++){
		fourierAnalysis(dataPointsPerRevolution,revolutions,&steps[totalDatapoints*i],&intensity[totalDatapoints*i],&intensityErr[totalDatapoints*i],fourierCoefficients,fcErr);
		c0=fourierCoefficients[cos+0];
		s4=fourierCoefficients[sin+4];
		c4=fourierCoefficients[cos+4];
		angle = 0.5*atan2(c4,s4);
		angle = angle*180.0/PI;
		stderrangle=(1/(1+pow(c4/s4,2)))*sqrt(pow(s4,-2))*(sqrt(pow(fcErr[cos+pos],2) + stderrangle*pow(fcErr[sin+pos],2))/2.0);
		stderrangle = stderrangle*180.0/PI;
		printf("c0 = %f\ts4 = %f\tc4 = %f\tangle = %f (%f)\n",c0,s4,c4,angle,stderrangle);
		fprintf(analysis,"%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",aouts[i],c0,s4,fcErr[sin+pos],fcErr[sin+neg],c4,fcErr[cos+pos],fcErr[cos+neg],angle,stderrangle);
	}
	free(aouts);
	free(intensity);
	free(intensityErr);
	free(steps);
	fclose(analysis);

	return 0;
}

int readInData(char* fileName,int totalDatapoints, int numAouts, int* aouts, int* steps, float* intensity, float* intensityErr){
	FILE* data = fopen(fileName,"r");
	char trash[1024];
	if (!data) {
		printf("Unable to open file %s\n",fileName);
		exit(1);
	}

	// Skips over comment lines and the header.
	do{
		fgets(trash,1024,data);
		printf("Skipped a line\n");
	}while(trash[0]=='#');

	int i;
	for (i=0; i< totalDatapoints*numAouts; i++){
		fscanf(data,"%d\t%d\t%f\t%f\n",&aouts[i],&steps[i],&intensity[i],&intensityErr[i]);
		printf("%d\t%d\t%f\t%f\n",aouts[i],steps[i],intensity[i],intensityErr[i]);
	}

	fclose(data);
	return 0;
}

int fourierAnalysis(int dataPointsPerRevolution, int revolutions, int* steps, float* intensity, float* intensityErr, float* fourierCoefficients, float* fcErr){
	float dstep=1.0/350.0;
	int totalDatapoints = dataPointsPerRevolution*revolutions;

	int i;
	int k;
	int cos=0;
	int sin=dataPointsPerRevolution/2;
	int pos=0;
	int neg=dataPointsPerRevolution;
	for (k=0; k < totalDatapoints/2; k++){
		fourierCoefficients[cos+k]=0;
		fourierCoefficients[sin+k]=0;
		fcErr[cos+pos+k]=0;
		fcErr[cos+neg+k]=0;
		fcErr[sin+pos+k]=0;
		fcErr[sin+neg+k]=0;
	}
	for (i=0;i < totalDatapoints;i++){ 
		for (k=0; k < totalDatapoints/2; k++){
			fourierCoefficients[cos+k] += calculateOneSumTerm(cos,dataPointsPerRevolution,revolutions,intensity[i], (float)i, k);
			fourierCoefficients[sin+k] += calculateOneSumTerm(sin,dataPointsPerRevolution,revolutions,intensity[i], (float)i, k);

			fcErr[cos+pos+k] += pow(calculateOneSumTermError(cos,pos,dataPointsPerRevolution,revolutions,intensity[i],intensityErr[i],(float)i,dstep,k),2);
			fcErr[cos+neg+k] += pow(calculateOneSumTermError(cos,neg,dataPointsPerRevolution,revolutions,intensity[i],intensityErr[i],(float)i,dstep,k),2);
			fcErr[sin+pos+k] += pow(calculateOneSumTermError(sin,pos,dataPointsPerRevolution,revolutions,intensity[i],intensityErr[i],(float)i,dstep,k),2);
			fcErr[sin+neg+k] += pow(calculateOneSumTermError(sin,neg,dataPointsPerRevolution,revolutions,intensity[i],intensityErr[i],(float)i,dstep,k),2);
		}
	}
	for (k=0; k < totalDatapoints/2; k++){
		fcErr[cos+pos+k]=sqrt(fcErr[cos+pos+k]);
		fcErr[cos+neg+k]=sqrt(fcErr[cos+neg+k]);
		fcErr[sin+pos+k]=sqrt(fcErr[sin+pos+k]);
		fcErr[sin+neg+k]=sqrt(fcErr[sin+neg+k]);
	}
	return 0;
}

float calculateOneSumTerm(int trigFunc, int dataPointsPerRevolution,int revolutions,float intensity, float i,int k){
	int totalDatapoints = dataPointsPerRevolution*revolutions;
	int cosIndex=0;

	int d0_L=0;	// d0_L represents two delta functions. See Berry for
				// more info.
	if(k==0){
		d0_L=1;
	}else if(k==totalDatapoints/2-1 && totalDatapoints%2==0){
		d0_L=1;
	}
	else{
		d0_L=0;
	}
	if (trigFunc==cosIndex)
		return 2 * intensity * cos(k*((2*PI*revolutions)*i/totalDatapoints))/(totalDatapoints*(1+d0_L));
	else
		return 2 * intensity * sin(k*((2*PI*revolutions)*i/totalDatapoints))/(totalDatapoints*(1+d0_L));
}

float calculateOneSumTermError(int trigFunc, int posOrNeg,int dataPointsPerRevolution, int revolutions, float intensity,float intensityErr, float i, float iErr, int k){
	int pos=0;

	if (posOrNeg!=pos){
		intensityErr=-intensityErr;	
		iErr=-iErr;	
	}
	
	float sI=calculateOneSumTerm(trigFunc,dataPointsPerRevolution,revolutions,intensity+intensityErr,i,k)-calculateOneSumTerm(trigFunc,dataPointsPerRevolution,revolutions,intensity,i,k);
	float sStep=calculateOneSumTerm(trigFunc,dataPointsPerRevolution,revolutions,intensity,i+iErr,k)-calculateOneSumTerm(trigFunc,dataPointsPerRevolution,revolutions,intensity,i,k);

	return sqrt(pow(sI,2)+pow(sStep,2));
}
