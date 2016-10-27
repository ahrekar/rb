/*
   Program to record polarization.
   RasPi connected to USB 1208LS.
   Target energy: USB1208LS Analog out Ch1 controls HP3617A. See pg 31 my lab book
   PMT Counts: data received from CTR in USB1208
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

#include "pmd.h"
#include "fileTools.h"
#include "polarizationAnalysisTools.h"
#ifndef DEFINITIONS_H
#define DEFINITIONS_H
	#include "mathTools.h"
#endif

int processFile(char* backgroundFileName, char* comments);
int processFileWithBackground(char* analysisFileName, char* backgroundFileName, char* dataFile, int dataPointsPerRevolution, int revolutions, int normalizeWithCurrent);
int getCommentLineFromFile(char* inputFile, char* returnString);

int calculateFourierCoefficients(char* fileName, int dataPointsPerRevolution, int Revolutions, int normalizeWithCurrent, float* fcCosRet, float* fcCosErrRet, float* fcSinRet, float* fcSinErrRet, float* avgCurrentReturn, float* avgCurrentStdDevReturn);
float calculateOneSumTerm(char trigFunc, float intensity, float i,int k);
float calculateOneSumTermError(char trigFunc, int posOrNeg, float intensity,float intensityErr, float i, float iErr, int k);

int calculateStokesFromFC(float* fcCos, float* fcCosErr, float* fcSin, float* fcSinErr, float* stokesReturn, float* stokesErrReturn);
float calculateStokes(int i, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4);
float calculateStokesErr(int i, int signOfError, float alpha, float beta, float delta, float* fcCos, float* fcCosErr, float* fcSin, float* fcSinErr);

int writeDataSummaryToFile(char* rawDataFileName, char* analysisFileName, char* backgroundFileName, 
							int normalizeWithCurrent, char* comments,
							float* fcCos, float* fcCosErr, float* fcSin, float* fcSinErr,
							float* stokesParameters, float* spError,
							float avgCurrent, float avgCurrentStdDev);

int plotStokesParameters(char* analysisFileName);
int plotDataFit(char* analysisFileName,float* fcCos, float* fcSin);

int printOutFC(float* fcCos, float* fcCosErr,float* fcSin, float* fcSinErr);
int printOutSP(float* sp, float* spError);
int printOutFloatArray(float* array, int n);
int printOutFloatArrayWithError(float* array, float* error, int n);

int calculateFourierCoefficients(char* fileName, int dataPointsPerRevolution, int revolutions, int normalizeWithCurrent, float* fcCosRet, float* fcCosErrRet, float* fcSinRet, float* fcSinErrRet, float* avgCurrentReturn, float* avgCurrentStdDevReturn){	
	// Begin File setup
	int totalDatapoints=dataPointsPerRevolution*revolutions;
	float currentSum, currentErrSum;
	float* currentValues = calloc(totalDatapoints,sizeof(float));
	FILE* data = fopen(fileName,"r");
	if (!data) {
		printf("Unable to open file %s\n",fileName);
		exit(1);
	}
	// End File setup
	int k;
	for (k=0; k < totalDatapoints/2; k++){
		fcCosRet[k]=0;
		fcSinRet[k]=0;
		fcCosErrRet[POS+k]=0;
		fcCosErrRet[NEG+k]=0;
		fcSinErrRet[POS+k]=0;
		fcSinErrRet[NEG+k]=0;
	}
	// TODO: implement the FFT version of this. 
	int i;
	char trash[1024]; 	// We need to skip several lines in the file that aren't data
					 	// This is a buffer to accomplish that.
	trash[0]='#'; 	// This is a quickly thrown together hack to avoid having an fgets statement 
				 	// outside of the while loop.
	float initialCurrent=0;	
	currentSum=0;
	currentErrSum=0;
	for (i=0; i< totalDatapoints; i++){
		int steps, counts;
		float current, currentErr;
		float angle;
		int j=0;
		while(trash[0]=='#'){ // Skip over all lines that have a # at the beginning. 
			
			fgets(trash,1024,data);
			j++;
		}
		//printf("Lines skipped=%d\n",j);
		trash[0]='a';
		fscanf(data,"%d\t%d\t%f\t%f\t%f\n",&steps,&counts,&current,&currentErr,&angle);
		currentSum+=current;
		currentErrSum+=currentErr;
		currentValues[i]=current;
		if(i==0)
			initialCurrent=current;

		float intensity;
		float intensityErr;
		for (k=0; k < totalDatapoints/2; k++){
			intensity=counts;
			intensityErr=sqrt((float)counts);
			if(normalizeWithCurrent==1){
				intensity=intensity/fabs(current/initialCurrent);
				intensityErr=counts/fabs(current/initialCurrent)*sqrt(pu2(counts,sqrt(counts))+pu2(current,currentErr));
			}

			fcCosRet[k] += calculateOneSumTerm('c',intensity, (float)i, k);
			fcSinRet[k] += calculateOneSumTerm('s',intensity, (float)i, k);

			fcCosErrRet[POS+k] += pow(calculateOneSumTermError('c',POS,intensity,intensityErr,(float)i,DSTEP,k),2);
			fcCosErrRet[NEG+k] += pow(calculateOneSumTermError('c',NEG,intensity,intensityErr,(float)i,DSTEP,k),2);
			fcSinErrRet[POS+k] += pow(calculateOneSumTermError('s',POS,intensity,intensityErr,(float)i,DSTEP,k),2);
			fcSinErrRet[NEG+k] += pow(calculateOneSumTermError('s',NEG,intensity,intensityErr,(float)i,DSTEP,k),2);
		}
		//printf("%f\t%f\n",fcErrReturn[COS+POS+0],fcErrReturn[COS+POS+1]);
	}

	*avgCurrentReturn = currentSum/totalDatapoints;
	*avgCurrentStdDevReturn = stdDeviation(currentValues,totalDatapoints);

	for (k=0; k < totalDatapoints/2; k++){
		fcCosErrRet[POS+k]=sqrt(fcCosErrRet[POS+k]);
		fcCosErrRet[NEG+k]=sqrt(fcCosErrRet[NEG+k]);
		fcSinErrRet[POS+k]=sqrt(fcSinErrRet[POS+k]);
		fcSinErrRet[NEG+k]=sqrt(fcSinErrRet[NEG+k]);
	}
	//printf("Lines Read: %d\n",i);
	fclose(data);
	return 0;
}

float calculateOneSumTerm(char trigFunc, float intensity, float i,int k){
	int totalDatapoints=DATAPOINTS;
	int d0_L=0;	// d0_L represents two delta functions. See Berry for
				// more info.
	if(k==0){
		d0_L=1;
	}else if(k==totalDatapoints/2-1 && DATAPOINTS%2==0){
		d0_L=1;
	}
	else{
		d0_L=0;
	}
	if (trigFunc=='c')
		return 2 * intensity * cos(k*((2*PI*REVOLUTIONS)*i/totalDatapoints))/(totalDatapoints*(1+d0_L));
	else
		return 2 * intensity * sin(k*((2*PI*REVOLUTIONS)*i/totalDatapoints))/(totalDatapoints*(1+d0_L));
}

float calculateOneSumTermError(char trigFunc, int posOrNeg, float intensity,float intensityErr, float i, float iErr, int k){
	if (posOrNeg==NEG){
		intensityErr=-intensityErr;	
		iErr=-iErr;	
	}
	float sI=calculateOneSumTerm(trigFunc,intensity+intensityErr,i,k)-calculateOneSumTerm(trigFunc,intensity,i,k);
	float sStep=calculateOneSumTerm(trigFunc,intensity,i+iErr,k)-calculateOneSumTerm(trigFunc,intensity,i,k);

	return sqrt(pow(sI,2)+pow(sStep,2));
}

int calculateStokesFromFC(float* fcCos, float* fcCosErr, float* fcSin, float* fcSinErr, float* stokesReturn, float* stokesErrorReturn){
	float delta=2*PI*(DELTA)/360.0;
	float alpha=2*PI*(ALPHA)/360.0;
	float beta_0=2*PI*(BETA)/360.0;
	float c0=fcCos[0];
	float c2=fcCos[2];
	float c4=fcCos[4];
	float s2=fcSin[2];
	float s4=fcSin[4];
	int pos=0;
	int neg=NUMSTOKES;
	int i;
	for(i=0;i<NUMSTOKES;i++){
		stokesReturn[i]=calculateStokes(i,alpha,beta_0,delta,c0,c2,c4,s2,s4);
		stokesErrorReturn[pos+i]=calculateStokesErr(i,pos,alpha,beta_0,delta,fcCos,fcCosErr,fcSin,fcSinErr);
		stokesErrorReturn[neg+i]=calculateStokesErr(i,neg,alpha,beta_0,delta,fcCos,fcCosErr,fcSin,fcSinErr);
	}	
	return 0;
}

float calculateStokes(int i, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4){
	if(i==0)
		return c0-(1+cos(delta))/(1-cos(delta))*(c4*cos(4*alpha+4*beta)+s4*sin(4*alpha+4*beta));
	else if(i==1)
		return 2.0/(1-cos(delta))*(c4*cos(2*alpha+4*beta)+s4*sin(2*alpha+4*beta)) / \
			calculateStokes(0,alpha, beta, delta, c0, c2, c4, s2, s4);
	else if(i==2)
		return 2.0/(1-cos(delta))*(s4*cos(2*alpha+4*beta)-c4*sin(2*alpha+4*beta)) / \
			calculateStokes(0,alpha, beta, delta, c0, c2, c4, s2, s4);
	else{
		float p3c2=c2/sin(2*alpha+2*beta);
		float p3s2=-s2/cos(2*alpha+2*beta);
		float p3=p3c2;
		int sign=1;
		if(p3s2>0 && p3c2>0)
			sign = 1;
		else if(p3s2<0 && p3c2<0)
			sign = -1;
		else 
			p3=(fabs(c2)>fabs(s2))?p3c2:p3s2;
			if(p3>0)
				sign=1;
			else
				sign=-1;
			

		return sign * sqrt(pow(c2,2)+pow(s2,2))/pow(sin(delta),2) / \
			calculateStokes(0,alpha, beta, delta, c0, c2, c4, s2, s4);
	}
}

float calculateStokesErr(int i, int signOfError, float alpha, float beta, float delta, float* fcCos, float* fcCosErr, float* fcSin, float* fcSinErr){
	float c0=fcCos[0];
	float c2=fcCos[2];
	float c4=fcCos[4];
	float s2=fcSin[2];
	float s4=fcSin[4];
	int numVars = 8;
	float* temp = calloc(numVars,sizeof(float));
	float totalError=0;
	int sgn=1;
	int sign=POS;
	float dDelta=2*PI*(DDELTA)/360.0;
	float dAlpha=2*PI*(DALPHA)/360.0;
	float dBeta=2*PI*(DBETA)/360.0;

	if(signOfError == NUMSTOKES){
		sign=NEG;
		sgn=-1;
	}

	float k;
	      k=calculateStokes(i, alpha , beta, delta, c0, c2, c4, s2, s4);
	temp[0]=calculateStokes(i,   alpha+sgn*(dAlpha)     , beta, delta, c0, c2, c4, s2, s4)-k;
	temp[1]=calculateStokes(i,alpha, beta  +  sgn*dBeta       , delta, c0, c2, c4, s2, s4)-k;
	temp[2]=calculateStokes(i,alpha, beta,     delta+sgn*dDelta      , c0, c2, c4, s2, s4)-k;
	temp[3]=calculateStokes(i,alpha, beta, delta, c0+fcCosErr[sign+0], c2, c4, s2, s4)-k;
	temp[4]=calculateStokes(i,alpha, beta, delta, c0, c2+fcCosErr[sign+2], c4, s2, s4)-k;
	temp[5]=calculateStokes(i,alpha, beta, delta, c0, c2, c4+fcCosErr[sign+4], s2, s4)-k;
	temp[6]=calculateStokes(i,alpha, beta, delta, c0, c2, c4, s2+fcSinErr[sign+2], s4)-k;
	temp[7]=calculateStokes(i,alpha, beta, delta, c0, c2, c4, s2, s4+fcSinErr[sign+4])-k;

	int j;

	for(j=0;j<numVars;j++)
		totalError+=pow(temp[j],2);
	
	free(temp);

	return sqrt(totalError);
}

int printOutFC(float* fcCos, float* fcCosErr,float* fcSin, float* fcSinErr){
	printf("Cos Coefficients:\n");
	int numCoefficients = 10;
	int i;
	for(i=0;i<numCoefficients;i++){
		printf("%s %d:\t%10.3f\t%10.3f\t%10.3f\n","Cos",i,fcCos[i],fcCosErr[POS+i],fcCosErr[NEG+i]);
	}
	printf("Sin Coefficients:\n");
	for(i=0;i<numCoefficients;i++){
		printf("%s %d:\t%10.3f\t%10.3f\t%10.3f\n","Sin",i,fcSin[i],fcSinErr[POS+i],fcSinErr[NEG+i]);
	}
	return 0;
}

int printOutSP(float* sp, float* spError){
	printf("Stokes Parameters:\n");
	printf("(M/I)^2 + (C/I)^2: %f\n", pow(sp[1],2) + pow(sp[2],2));
	printf("  I:\t%10.3f\t%10.3f\t%10.3f\n",sp[0],spError[0+0],spError[4+0]);
	printf("M/I:\t%10.3f\t%10.3f\t%10.3f\n",sp[1],spError[0+1],spError[4+1]);
	printf("C/I:\t%10.3f\t%10.3f\t%10.3f\n",sp[2],spError[0+2],spError[4+2]);
	printf("S/I:\t%10.3f\t%10.3f\t%10.3f\n",sp[3],spError[0+3],spError[4+3]);
	return 0;
}

int printOutFloatArray(float* array, int n){
	int i;
	for(i=0;i<n;i++){
		printf("Element[%d]:%f\n",i,array[i]);
	}
	return 0;
}

int printOutFloatArrayWithError(float* array, float* errorArray, int n){
	int i;
	for(i=0;i<n;i++){
		printf("Element[%d]:%f\tError:%f\n",i,array[i],errorArray[i]);
	}
	return 0;
}

int getCommentLineFromFile(char* inputFile, char* returnString){
	FILE* data = fopen(inputFile,"r");
	char* pointer;
	if (!data) {
		printf("Unable to open file %s\n",inputFile);
		exit(1);
	}
	do{
		fgets(returnString,1024,data);
	} while(strncmp(returnString,"#Comments",9));
	pointer = strtok(returnString,"\t");
	pointer = strtok(NULL,"\t");
	strcpy(returnString,pointer);
	returnString[strcspn(returnString,"\n")]=0;

	return 0;
}

int processFileWithBackground(char* analysisFileName, char* backgroundFileName, char* dataFileName, int dataPointsPerRevolution, int revolutions, int normalizeWithCurrent){
	char comments[1024];

	printf("Data Points Per Revolution=%d\nRevolutions=%d\n",dataPointsPerRevolution,revolutions);
	int totalDatapoints = dataPointsPerRevolution * revolutions;
	float* fcCos = malloc(totalDatapoints/2*sizeof(float));
	float* fcSin = malloc(totalDatapoints/2*sizeof(float));
	float* fcCosErr = malloc(totalDatapoints*sizeof(float)); 	// We need twice as many datapoints for the 
	float* fcSinErr = malloc(totalDatapoints*sizeof(float)); 	// error because I'm doing the upper error and   														
																// lower error separately.      
																
	float avgCurrent;
	float avgCurrentStdDev;

	// Find fourier coefficients from raw data.
	printf("Calculating Fourier Coefficients for raw Data...\n");
	calculateFourierCoefficients(dataFileName,dataPointsPerRevolution,revolutions,normalizeWithCurrent,fcCos,fcCosErr,fcSin,fcSinErr,&avgCurrent,&avgCurrentStdDev);

	printf("Plotting Data Fit...\n");
	plotDataFit(analysisFileName,fcCos,fcSin);

	//printf("====Raw Data Fourier Coefficients====\n");
	printf("\n");

	// Calculate fourier coefficients from BG data, if provided, and
	// remove background from data
	if(strcmp(backgroundFileName,"NONE") != 0){
		float* fcCosBg = malloc(totalDatapoints/2*sizeof(float));
		float* fcSinBg = malloc(totalDatapoints/2*sizeof(float));
		float* fcCosBgErr = malloc(totalDatapoints*sizeof(float));
		float* fcSinBgErr = malloc(totalDatapoints*sizeof(float));
		float avgCurrentBg;
		float avgCurrentBgStdDev;
		printf("Calculating Fourier Coefficients for background...\n");
		calculateFourierCoefficients(backgroundFileName,dataPointsPerRevolution,revolutions,normalizeWithCurrent,fcCosBg,fcCosBgErr,fcSinBg,fcSinBgErr,&avgCurrentBg,&avgCurrentBgStdDev);

		//printf("====Background Fourier Coefficients====\n");
		printf("\n");

		int k;
		for (k=0; k < totalDatapoints/2; k++){
			fcCos[k]-=fcCosBg[k];
			fcSin[k]-=fcSinBg[k];
			fcCosErr[POS+k]=sqrt(pow(fcCosErr[POS+k],2)+pow(fcCosBgErr[POS+k],2));
			fcCosErr[NEG+k]=sqrt(pow(fcCosErr[NEG+k],2)+pow(fcCosBgErr[NEG+k],2));
			fcSinErr[POS+k]=sqrt(pow(fcSinErr[POS+k],2)+pow(fcSinBgErr[POS+k],2));
			fcSinErr[NEG+k]=sqrt(pow(fcSinErr[NEG+k],2)+pow(fcSinBgErr[NEG+k],2));
		}

		//printf("====Signal Fourier Coefficients====\n");
		printf("\n");

		free(fcCosBg);
		free(fcSinBg);
		free(fcCosBgErr);
		free(fcSinBgErr);
	}
	// Calculate Stokes Parameters from Fourier Coefficients.
	float* stokesParameters = malloc(NUMSTOKES*sizeof(float));
	float* spErr = malloc(NUMSTOKES*2*sizeof(float));
	printf("Calculating Stokes Parameters from Fourier Coefficients...\n");
	calculateStokesFromFC(fcCos,fcCosErr,fcSin,fcSinErr,stokesParameters,spErr);

	printf("====Stokes Parameters====\n");
	printOutSP(stokesParameters,spErr);
	printf("\n");

	getCommentLineFromFile(dataFileName,comments);

	printf("Writing analysis to file...\n");
	writeDataSummaryToFile(analysisFileName,backgroundFileName,dataFileName,
							normalizeWithCurrent,comments,
							fcCos,fcCosErr,fcSin,fcSinErr,
							stokesParameters,spErr,
							avgCurrent,avgCurrentStdDev);

	printf("Generating graph of Stokes parameters...\n");
	plotStokesParameters(analysisFileName);

	free(fcCos);
	free(fcSin);
	free(fcCosErr);
	free(fcSinErr);
	free(stokesParameters);
	free(spErr);
	return 0;
}

int writeDataSummaryFileHeader(char* analysisFileName, char* backgroundFileName, char* dataFileName, char* comments){
	FILE *file;
	file=fopen(analysisFileName,"w");
	if (!file) {
		printf("Unable to open file: %s\n", analysisFileName);
		exit(1);
	}
	fprintf(file,"#File\t%s\n",analysisFileName);
	fprintf(file,"#DataFile\t%s\n",dataFileName);
	fprintf(file,"#BackgroundFile\t%s\n",backgroundFileName);
	fprintf(file,"#Comments\t%s\n",comments);
	fprintf(file,"#ALPHA\t%f\t%f\n",ALPHA,DALPHA);
	fprintf(file,"#BETA\t%f\t%f\n",BETA,DBETA);
	fprintf(file,"#DELTA\t%f\t%f\n",DELTA,DDELTA);

	fflush(file);
	fclose(file);
	return 0;
}

int writeDataSummaryFileColumnHeadings(char* analysisFileName){
	FILE *file;
	file=fopen(analysisFileName,"a");
	if (!file) {
		printf("Unable to open file: %s\n", analysisFileName);
		exit(1);
	}
	fprintf(file,"fileName\tprocessedFile\tbackgroundFile\tcomments\t");
	fprintf(file,"alpha\td_alpha\tbeta_0\td_beta_0\tdelta\td_delta\t");
	fprintf(file,"normCurr\t");
	fprintf(file,"p0\tp0ErrUp\tp0ErrDown\tp1\tp1ErrUp\tp1ErrDown\tp2\tp2ErrUp\tp2ErrDown\tp3\tp3ErrUp\tp3ErrDown\t");
	fprintf(file,"c0\tc0ErrUp\tc0ErrDown\tc4\tc4ErrUp\tc4ErrDown\ts4\ts4ErrUp\ts4ErrDown\ts2\ts2ErrUp\ts2ErrDown\t");
	fprintf(file,"c2\tc2ErrUp\tc2ErrDown\tAvg.Current\tAvg.CurrentStdDev\n");

	fflush(file);
	fclose(file);
	return 0;
}

int writeDataSummaryFileEntry(char* analysisFileName, char* backgroundFileName, char* dataFileName, 
							int normalizeWithCurrent, char* comments,
							float* fcCos, float* fcCosErr, float* fcSin, float* fcSinErr,
							float* stokesParameters, float* spErr,
							float avgCurrent, float avgCurrentStdDev){
	FILE *file;
	file=fopen(analysisFileName,"a");
	if (!file) {
		printf("Unable to open file: %s\n", analysisFileName);
		exit(1);
	}
	fprintf(file,"%s\t%s\t%s\t%s\t",analysisFileName,dataFileName,backgroundFileName,comments);
	fprintf(file,"%f\t%f\t%f\t%f\t%f\t%f\t",ALPHA,DALPHA,BETA,DBETA,DELTA,DDELTA);
	fprintf(file,"%d\t",normalizeWithCurrent);
	fprintf(file,"%f\t%f\t%f\t",stokesParameters[0],spErr[0+0],spErr[NUMSTOKES+0]);
	fprintf(file,"%f\t%f\t%f\t",stokesParameters[1],spErr[0+1],spErr[NUMSTOKES+1]);
	fprintf(file,"%f\t%f\t%f\t",stokesParameters[2],spErr[0+2],spErr[NUMSTOKES+2]);
	fprintf(file,"%f\t%f\t%f\t",stokesParameters[3],spErr[0+3],spErr[NUMSTOKES+3]);
	fprintf(file,"%f\t%f\t%f\t",fcCos[0],fcCosErr[POS+0],fcCosErr[NEG+0]);
	fprintf(file,"%f\t%f\t%f\t",fcCos[4],fcCosErr[POS+4],fcCosErr[NEG+4]);
	fprintf(file,"%f\t%f\t%f\t",fcSin[4],fcSinErr[POS+4],fcSinErr[NEG+4]);
	fprintf(file,"%f\t%f\t%f\t",fcSin[2],fcSinErr[POS+2],fcSinErr[NEG+2]);
	fprintf(file,"%f\t%f\t%f\t",fcCos[2],fcCosErr[POS+2],fcCosErr[NEG+2]);
	fprintf(file,"%f\t%f\n",avgCurrent,avgCurrentStdDev);

	fflush(file);
	fclose(file);
	return 0;
}

int writeDataSummaryToFile(char* analysisFileName, char* backgroundFileName, char* dataFileName, 
							int normalizeWithCurrent, char* comments,
							float* fcCos, float* fcCosErr, float* fcSin, float* fcSinErr,
							float* stokesParameters, float* spErr,
							float avgCurrent, float avgCurrentStdDev){
	char* underscoreLocation;
	int fileExists;
	char dailySummaryFileName[1024];
	char fileName[2][1024];

	strcpy(dailySummaryFileName,analysisFileName);
	underscoreLocation=strstr(dailySummaryFileName,"_");
	strcpy(underscoreLocation,".dat");
	fileExists=access(dailySummaryFileName,F_OK);

	writeDataSummaryFileHeader(analysisFileName,backgroundFileName,dataFileName,comments);

	strcpy(fileName[0],analysisFileName);
	strcpy(fileName[1],dailySummaryFileName);

	int i;
	for(i=0;i<2;i++){
		// Print out the header information
		if(i==0 || (i==1 && fileExists == -1)){ // If it's the datafile OR if its the daily file and the file didn't exist before. 
			writeDataSummaryFileColumnHeadings(fileName[i]);
		}

		writeDataSummaryFileEntry(fileName[i], backgroundFileName, dataFileName, 
									normalizeWithCurrent, comments, 
									fcCos, fcCosErr, fcSin, fcSinErr,
									stokesParameters, spErr,
									avgCurrent, avgCurrentStdDev);
	}

	return 0;
}

int plotDataFit(char* analysisDataFileName, float* fcCos, float* fcSin){
	char rawDataFileNameBase[1024];
	char* extensionStart;
	strcpy(rawDataFileNameBase,analysisDataFileName);
	extensionStart=strstr(rawDataFileNameBase,"analysis.dat");
	strcpy(extensionStart,"");

	FILE* gnuplot;
	char buffer[1024];
	// Create rough graphs of data.
	gnuplot = popen("gnuplot","w"); 
	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb size 158,72 enhanced\n");
		fprintf(gnuplot, "set output\n");			
		
		sprintf(buffer, "set title '%s'\n", rawDataFileNameBase);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Angle'\n");			
		fprintf(gnuplot, "set xtics (\"0\" 0,\"0.5{/Symbol p}\" pi/2,\"{/Symbol p}\" pi,\"1.5{/Symbol p}\" 1.5*pi,\"2{/Symbol p}\" 2*pi)\n");
		//Begin plot command
		fprintf(gnuplot, "set multiplot layout 2,1\n");
		fprintf(gnuplot, "set ylabel 'Counts'\n");			
		sprintf(buffer, "plot '%s.dat' using ($1/1200*2*pi):2, %f+%f*cos(2*x)+%f*cos(4*x)+%f*sin(2*x)+%f*sin(4*x)\n",rawDataFileNameBase,fcCos[0],fcCos[2],fcCos[4],fcSin[2],fcSin[4]);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "set ylabel 'Current'\n");			
		sprintf(buffer, "plot '%s.dat' using ($1/1200*2*pi):(-$3)\n",rawDataFileNameBase);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset multiplot\n");
		//End plot command
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png enhanced\n");
		sprintf(buffer, "set output '%s.png'\n", rawDataFileNameBase);
		fprintf(gnuplot, buffer);
		// SAME PLOT COMMANDS GO HERE
		fprintf(gnuplot, "set multiplot layout 2,1\n");
		fprintf(gnuplot, "set ylabel 'Counts'\n");			
		sprintf(buffer, "plot '%s.dat' using ($1/1200*2*pi):2, %f+%f*cos(2*x)+%f*cos(4*x)+%f*sin(2*x)+%f*sin(4*x)\n",rawDataFileNameBase,fcCos[0],fcCos[2],fcCos[4],fcSin[2],fcSin[4]);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "set ylabel 'Current'\n");			
		sprintf(buffer, "plot '%s.dat' using ($1/1200*2*pi):(-$3)\n",rawDataFileNameBase);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset multiplot\n");
	}
	return pclose(gnuplot);
}

int plotStokesParameters(char* analysisFileName){
	FILE* gnuplot;
	char buffer[1024];
	// Create rough graphs of data.
	gnuplot = popen("gnuplot","w"); 

	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb size 158,32\n");
		fprintf(gnuplot, "set output\n");			
		
		sprintf(buffer, "set title '%s'\n", analysisFileName);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Relative Stokes Parameter'\n");			
		fprintf(gnuplot, "set ylabel 'Value'\n");			
		fprintf(gnuplot, "set yrange [-.05:.05]\n");			
		fprintf(gnuplot, "set xrange [0:4]\n");			
		sprintf(buffer, "plot '%s' using (1):15:($15+$16):($15-$17) w yerror,'%s' using (2):18:($18+$19):($18-$20) w yerror,'%s' using (3):21:($21+$22):($21-$23) w yerror\n",analysisFileName,analysisFileName,analysisFileName);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s.png'\n", analysisFileName);
		fprintf(gnuplot, buffer);
		// SAME PLOT COMMANDS GO HERE
		sprintf(buffer, "plot '%s' using (1):15:($15+$16):($15-$17) w yerror,'%s' using (2):18:($18+$19):($18-$20) w yerror,'%s' using (3):21:($21+$22):($21-$23) w yerror\n",analysisFileName,analysisFileName,analysisFileName);
		fprintf(gnuplot, buffer);
	}
	return pclose(gnuplot);
}
