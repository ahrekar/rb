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
int analyzeData(char* fileName,int dataPtsPerRev, int revolutions);
int	readInData(char* fileName,int totalDatapoints, int numAouts, int* aouts, int* steps, float* intensity, float* inensityErr);
float calculateOneSumTerm(int trigFunc, int dataPointsPerRevolution,int revolutions,float intensity, float i,int k);
float calculateOneSumTermError(int trigFunc, int posOrNeg,int dataPointsPerRevolution, int revolutions, float intensity,float intensityErr, float i, float iErr, int k);
int fourierAnalysis(int dataPointsPerRevolution, int revolutions, int* steps, float* intensity, float* intensityErr, float* fourierCoefficients, float* fcErr);
float calculateAngleError(float c2, float c2Err, float s2, float s2Err);
int getNumberOfDataLines(char* fileName);

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
		for(i=0;i<1;i++){
			fprintf(wolfram, "faradayData=Delete[faradayData,{{1}}]\n");			
			
		}
		//Removes unneccesary columns from the file
		for(i=0;i<7;i++){
			fprintf(wolfram, "faradayData=Drop[faradayData,None,{2}]\n");
		}
		fprintf(wolfram, "faradayData=Drop[faradayData,None,{3}]\n");
		fprintf(wolfram, "faradayData=Drop[faradayData,None,{3}]\n");
		//Removes lines from the file (This one gets rid of the data close to resonance)
		//fprintf(wolfram, "faradayData=Delete[faradayData,{{8},{9},{10},{11}}]\n");
		fprintf(wolfram, "c=2.9979*^10\n");
		fprintf(wolfram, "re=2.8179*^-13\n");
		fprintf(wolfram, "fge=0.34231\n");
		fprintf(wolfram, "k=4/3\n");
		fprintf(wolfram, "Mb=9.2740*^-24\n");
		fprintf(wolfram, "BdotL=2.08*^-2\n");
		fprintf(wolfram, "h=6.6261*^-34\n");
		fprintf(wolfram, "vo=3.77107*^14\n");
		fprintf(wolfram, "pi=3.14.159265\n");
		fprintf(wolfram, "aoutConv=-.0266\n");
		fprintf(wolfram, "aoutIntercept=14.961\n");
		fprintf(wolfram, "const=c*re*fge*k*Mb*BdotL/(4*pi*h*vo)\n");
		fprintf(wolfram, "model=a+b*const*(vo+(aoutConv*detune+aoutIntercept)*1*^9)/((aoutConv*detune+aoutIntercept)*1*^9)^2+d*(vo+(aoutConv*detune+aoutIntercept))^5/((aoutConv*detune+aoutIntercept))^4\n");
		fprintf(wolfram, "modelResult=NonlinearModelFit[faradayData,model,{a,b,d},detune]\n");
		fprintf(wolfram, "modelResult['BestFitParameters']>>fitParams.txt\n");
		fprintf(wolfram, "modelResult['BestFitParameters']\n");
		fprintf(wolfram, "modelResult['ParameterErrors']>>>fitParams.txt\n");
		fprintf(wolfram, "modelResult['ParameterErrors']\n");
	}
	return pclose(wolfram);
}

int recordNumberDensity(char* fileName){
	char analysisFileName[1024];
	strcpy(analysisFileName,fileName);
	char* extensionStart=strstr(analysisFileName,".dat");
	strcpy(extensionStart,"Analysis.dat");
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

		fprintf(gnuplot, "set x2tics nomirror\n");
		fprintf(gnuplot, "set xtics nomirror\n");
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Aout (Detuning)'\n");			
		fprintf(gnuplot, "set ylabel 'Theta'\n");			
		fprintf(gnuplot, "set xrange [0:1024] reverse\n");			
		fprintf(gnuplot, "set x2range [1024*(-.022)+18.448:0*(-.022)+18.448]\n");			
		//sprintf(buffer, "plot '%s' using 1:9:($9+$10):($9+$11) with errorbars, '' u ($1*(-.022)+18.448):9\n",fileName);
		sprintf(buffer, "plot '%s' using 1:9:($9+$10):($9+$11) with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s.png'\n", fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:9:($9+$10):($9+$11) with errorbars, '' u ($1*(-.022)+18.448):9\n",fileName);
		fprintf(gnuplot, buffer);
	}
	return pclose(gnuplot);
}

int analyzeData(char* fileName, int dataPointsPerRevolution, int revolutions){
	char fileNameCopy[1024];
	int totalDatapoints=dataPointsPerRevolution*revolutions;
	int cos=0;
	int sin=dataPointsPerRevolution/2;
	int pos=0;
	int neg=dataPointsPerRevolution;

	char* extensionStart;

	int numberOfDataLines = getNumberOfDataLines(fileName);
	int numAouts= numberOfDataLines/(dataPointsPerRevolution*revolutions); 


	int* aouts=calloc(totalDatapoints*numAouts,sizeof(float)); 
	float* intensity=calloc(totalDatapoints*numAouts,sizeof(float));
	float* intensityErr=calloc(totalDatapoints*numAouts,sizeof(float));
	int* steps=calloc(totalDatapoints*numAouts,sizeof(float)); 

	printf("Reading in data...\n");
	readInData(fileName,totalDatapoints,numAouts,aouts,steps,intensity,intensityErr);
	printf("Data read in.\n");

	strcpy(fileNameCopy,fileName);
	extensionStart=strstr(fileNameCopy,".dat");
	strcpy(extensionStart,"Analysis.dat");

	FILE* analysis = fopen(fileNameCopy,"w");
	if (!analysis) {
		printf("Unable to open file %s\n",fileNameCopy);
		exit(1);
	}

	FILE* rawData = fopen(fileName,"r");
	if (!rawData) {
		printf("Unable to open file %s\n",fileName);
		exit(1);
	}

	// Copies over comment lines.
	char line[1024];
	fgets(line,1024,rawData);
	while(line[0]=='#'){
		fprintf(analysis,"%s",line);
		fgets(line,1024,rawData);
	}

	fclose(rawData);

	fprintf(analysis,"Aout\tc0\ts2\ts2Err+\ts2Err-\tc2\tc2Err+\tc2Err-\tangle\tangleErr+\tangleErr-\n");
	float* fourierCoefficients = calloc(totalDatapoints,sizeof(float));
	float* fcErr = calloc(totalDatapoints*2,sizeof(float));
	int i;
	float c0,s2,c2,angle,angleErrUp,angleErrDown;

	printf("Beginning Data Analysis...\n");
	for(i=0;i<numAouts;i++){
		fourierAnalysis(dataPointsPerRevolution,revolutions,&steps[totalDatapoints*i],&intensity[totalDatapoints*i],&intensityErr[totalDatapoints*i],fourierCoefficients,fcErr);
		c0=fourierCoefficients[cos+0];
		s2=fourierCoefficients[sin+2];
		c2=fourierCoefficients[cos+2];
		angle = 0.5*atan2(c2,s2);
		angleErrUp=calculateAngleError(c2,fcErr[pos+cos+2],s2,fcErr[pos+sin+2]);
		angleErrDown=calculateAngleError(c2,-fcErr[neg+cos+2],s2,-fcErr[neg+sin+2]);
		angle = angle*180.0/PI;
		angleErrUp = angleErrUp*180.0/PI;
		angleErrDown = angleErrDown*180.0/PI;
		printf("c0 = %f\ts2 = %f\tc2 = %f\tangle = %f (%f)\n",c0,s2,c2,angle,angleErrUp);
		fprintf(analysis,"%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",aouts[totalDatapoints*i],c0,s2,fcErr[sin+pos+2],fcErr[sin+neg+2],c2,fcErr[cos+pos+2],fcErr[cos+neg+2],angle,angleErrUp,angleErrDown);
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
		//printf("Skipped a line\n");
	}while(trash[0]=='#');

	int i;
	for (i=0; i < totalDatapoints*numAouts; i++){
		fscanf(data,"%d\t%d\t%f\t%f\n",&aouts[i],&steps[i],&intensity[i],&intensityErr[i]);
		//printf("%d\t%d\t%f\t%f\n",aouts[i],steps[i],intensity[i],intensityErr[i]);
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

float calculateAngleError(float c2, float c2Err, float s2, float s2Err){
	float angle;
	float angleErr=0;
	float angleErrCos=0;
	float angleErrSin=0;
	angle = 0.5*atan2(c2,s2);
	angleErrCos=angle-0.5*atan2(c2+c2Err,s2);
	angleErrSin=angle-0.5*atan2(c2,s2+s2Err);
	angleErr=sqrt(pow(angleErrCos,2)+pow(angleErrSin,2));
	return angleErr;
}

int getNumberOfDataLines(char* fileName){
	char trash[1024];
	int numberOfLines=0;
	char* line = NULL;
	size_t len =0;
	ssize_t read;

	FILE* rawData = fopen(fileName,"r");

	if (!rawData) {
		printf("Unable to open file %s\n",fileName);
		exit(1);
	}

	// Skips over comment lines and the header.
	do{
		fgets(trash,1024,rawData);
		//printf("Skipped a line\n");
	}while(trash[0]=='#');

	while((read=getline(&line, &len,rawData)) != -1){
		numberOfLines++;
	}

	fclose(rawData);

	return numberOfLines;
}
