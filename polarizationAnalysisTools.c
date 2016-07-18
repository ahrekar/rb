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
int processFileWithBackground(char* analysisFileName, char* backgroundFileName, char* dataFile, int dataPointsPerRevolution, int revolutions, char* comments);

int calculateFourierCoefficients(char* fileName, int dataPointsPerRevolution, int Revolutions, float* fcReturn, float* fcErrReturn, float* avgCurrentReturn, float* avgCurrentStdDevReturn);
float calculateOneSumTerm(int trigFunc, float intensity, float i,int k);
float calculateOneSumTermError(int trigFunc, int posOrNeg, float intensity,float intensityErr, float i, float iErr, int k);

int calculateStokesFromFC(float* fourierCoefficients, float* fcErr, float* stokesReturn, float* stokesErrReturn);
float calculateStokes(int i, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4);
float calculateStokesErr(int i, int signOfError, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4, float* fcErrors);

int writeDataSummaryToFile(char* rawDataFileName, char* analysisFileName, char* backgroundFileName, 
							char* comments,
							float* fourierCoefficients, float* fcError, 
							float* stokesParameters, float* spError,
							float avgCurrent, float avgCurrentStdDev);

int printOutFC(float* fourierCoefficients, float* fcErr);
int printOutSP(float* sp, float* spError);
int printOutFloatArray(float* array, int n);
int printOutFloatArrayWithError(float* array, float* error, int n);

int calculateFourierCoefficients(char* fileName, int dataPointsPerRevolution, int revolutions, float* fcReturn,float* fcErrReturn, float* avgCurrentReturn, float* avgCurrentStdDevReturn){	
	// normalizeWithCurrent should be set to 1 if it is desired
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
		fcReturn[COS+k]=0;
		fcReturn[SIN+k]=0;
		fcErrReturn[COS+POS+k]=0;
		fcErrReturn[COS+NEG+k]=0;
		fcErrReturn[SIN+POS+k]=0;
		fcErrReturn[SIN+NEG+k]=0;
	}
	// TODO: implement the FFT version of this. 
	int i;
	char trash[1024]; 	// We need to skip several lines in the file that aren't data
					 	// This is a buffer to accomplish that.
	trash[0]='#'; 	// This is a quickly thrown together hack to avoid having an fgets statement 
				 	// outside of the while loop.
	
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
		//fscanf(data,"%d\t%d\t%f\t%f\n",&steps,&counts,&current,&currentErr);
		//fscanf(data,"%d,%d,%f\n",&steps,&counts,&current); 	// Because I think I might want to revert 
																// to non-error calculations 
																// quickly at some point
																// I'm going to keep it hanging 
																// around commented out for a while.
		//printf("%d,%d,%f,%f\n",steps,counts,current,currentErr);
		//
		currentSum+=current;
		currentErrSum+=currentErr;
		currentValues[i]=current;

		float intensity;
		float intensityErr;
		for (k=0; k < totalDatapoints/2; k++){
			intensity=counts;
			intensityErr=sqrt((float)counts);
			if(NORMCURR==1){
				intensity=intensity/fabs(current);
				intensityErr=counts/fabs(current)*sqrt(pu2(counts,sqrt(counts))+pu2(current,currentErr));
			}

			fcReturn[COS+k] += calculateOneSumTerm(COS,intensity, (float)i, k);
			fcReturn[SIN+k] += calculateOneSumTerm(SIN,intensity, (float)i, k);

			fcErrReturn[COS+POS+k] += pow(calculateOneSumTermError(COS,POS,intensity,intensityErr,(float)i,DSTEP,k),2);
			fcErrReturn[COS+NEG+k] += pow(calculateOneSumTermError(COS,NEG,intensity,intensityErr,(float)i,DSTEP,k),2);
			fcErrReturn[SIN+POS+k] += pow(calculateOneSumTermError(SIN,POS,intensity,intensityErr,(float)i,DSTEP,k),2);
			fcErrReturn[SIN+NEG+k] += pow(calculateOneSumTermError(SIN,NEG,intensity,intensityErr,(float)i,DSTEP,k),2);
		}
		//printf("%f\t%f\n",fcErrReturn[COS+POS+0],fcErrReturn[COS+POS+1]);
	}

	*avgCurrentReturn = currentSum/totalDatapoints;
	*avgCurrentStdDevReturn = stdDeviation(currentValues,totalDatapoints);

	for (k=0; k < totalDatapoints/2; k++){
		fcErrReturn[COS+POS+k]=sqrt(fcErrReturn[COS+POS+k]);
		fcErrReturn[COS+NEG+k]=sqrt(fcErrReturn[COS+NEG+k]);
		fcErrReturn[SIN+POS+k]=sqrt(fcErrReturn[SIN+POS+k]);
		fcErrReturn[SIN+NEG+k]=sqrt(fcErrReturn[SIN+NEG+k]);
	}
	//printf("Lines Read: %d\n",i);
	fclose(data);
	return 0;
}

float calculateOneSumTerm(int trigFunc, float intensity, float i,int k){
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
	if (trigFunc==COS)
		return 2 * intensity * cos(k*((2*PI*REVOLUTIONS)*i/totalDatapoints))/(totalDatapoints*(1+d0_L));
	else
		return 2 * intensity * sin(k*((2*PI*REVOLUTIONS)*i/totalDatapoints))/(totalDatapoints*(1+d0_L));
}

float calculateOneSumTermError(int trigFunc, int posOrNeg, float intensity,float intensityErr, float i, float iErr, int k){
	if (posOrNeg==NEG){
		intensityErr=-intensityErr;	
		iErr=-iErr;	
	}
	float sI=calculateOneSumTerm(trigFunc,intensity+intensityErr,i,k)-calculateOneSumTerm(trigFunc,intensity,i,k);
	float sStep=calculateOneSumTerm(trigFunc,intensity,i+iErr,k)-calculateOneSumTerm(trigFunc,intensity,i,k);

	return sqrt(pow(sI,2)+pow(sStep,2));
}

int calculateStokesFromFC(float* fourierCoefficients, float* fcErrors, float* stokesReturn, float* stokesErrorReturn){
	float delta=2*PI*(DELTA)/360.0;
	float alpha=2*PI*(ALPHA)/360.0;
	float beta_0=2*PI*(BETA)/360.0;
	float c0=fourierCoefficients[COS+0];
	float c2=fourierCoefficients[COS+2];
	float c4=fourierCoefficients[COS+4];
	float s2=fourierCoefficients[SIN+2];
	float s4=fourierCoefficients[SIN+4];
	int pos=0;
	int neg=NUMSTOKES;
	int i;
	for(i=0;i<NUMSTOKES;i++){
		stokesReturn[i]=calculateStokes(i,alpha,beta_0,delta,c0,c2,c4,s2,s4);
		stokesErrorReturn[pos+i]=calculateStokesErr(i,pos,alpha,beta_0,delta,c0,c2,c4,s2,s4,fcErrors);
		stokesErrorReturn[neg+i]=calculateStokesErr(i,neg,alpha,beta_0,delta,c0,c2,c4,s2,s4,fcErrors);
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

float calculateStokesErr(int i, int signOfError, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4, float* fcErrors){
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
	temp[3]=calculateStokes(i,alpha, beta, delta, c0+fcErrors[COS+sign+0], c2, c4, s2, s4)-k;
	temp[4]=calculateStokes(i,alpha, beta, delta, c0, c2+fcErrors[COS+sign+2], c4, s2, s4)-k;
	temp[5]=calculateStokes(i,alpha, beta, delta, c0, c2, c4+fcErrors[COS+sign+4], s2, s4)-k;
	temp[6]=calculateStokes(i,alpha, beta, delta, c0, c2, c4, s2+fcErrors[SIN+sign+2], s4)-k;
	temp[7]=calculateStokes(i,alpha, beta, delta, c0, c2, c4, s2, s4+fcErrors[SIN+sign+4])-k;

	int j;
	for(j=0;j<numVars;j++)
		totalError+=pow(temp[j],2);
	
	free(temp);

	return sqrt(totalError);
}

int printOutFC(float* fourierCoefficients, float* fcError){
	printf("Cos Coefficients:\n");
	int numCoefficients = 10;
	int i;
	for(i=0;i<numCoefficients;i++){
		printf("%s %d:\t%10.3f\t%10.3f\t%10.3f\n","Cos",i,fourierCoefficients[COS+i],fcError[COS+POS+i],fcError[COS+NEG+i]);
	}
	printf("Sin Coefficients:\n");
	for(i=0;i<numCoefficients;i++){
		printf("%s %d:\t%10.3f\t%10.3f\t%10.3f\n","Sin",i,fourierCoefficients[SIN+i],fcError[SIN+POS+i],fcError[SIN+NEG+i]);
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

int processFileWithBackground(char* analysisFileName, char* backgroundFileName, char* dataFileName, int dataPointsPerRevolution, int revolutions, char* comments){

	int totalDatapoints = dataPointsPerRevolution * revolutions;
	float* fourierCoefficients = malloc(totalDatapoints*sizeof(float));
	float* fcErr = malloc(totalDatapoints*2*sizeof(float)); 	// We need twice as many datapoints for the 
														// error because I'm doing the upper error and
														// lower error separately. I'll access the upper
														// and lower error in a similarly convoluted way
														// to how I access the fourier coefficients.
	float avgCurrent;
	float avgCurrentStdDev;

	// Find fourier coefficients from raw data.
	calculateFourierCoefficients(dataFileName,dataPointsPerRevolution,revolutions,fourierCoefficients,fcErr,&avgCurrent,&avgCurrentStdDev);

	printf("====Raw Data Fourier Coefficients====\n");
	printOutFC(fourierCoefficients,fcErr);
	printf("\n");

	// Calculate fourier coefficients from BG data, if provided, and
	// remove background from data
	if(!strcmp(backgroundFileName,"NONE")){
		
		float* fcBg = malloc(totalDatapoints*sizeof(float));
		float* fcBgErr = malloc(totalDatapoints*2*sizeof(float));
		float avgCurrentBg;
		float avgCurrentBgStdDev;
		calculateFourierCoefficients(backgroundFileName,dataPointsPerRevolution,revolutions,fcBg,fcBgErr,&avgCurrentBg,&avgCurrentBgStdDev);

		printf("====Background Fourier Coefficients====\n");
		printOutFC(fcBg,fcBgErr);
		printf("\n");

		int k;
		for (k=0; k < totalDatapoints; k++){
			fourierCoefficients[k]-=fcBg[k];
			fcErr[POS+k]=sqrt(pow(fcErr[POS+k],2)+pow(fcBgErr[POS+k],2));
			fcErr[NEG+k]=sqrt(pow(fcErr[NEG+k],2)+pow(fcBgErr[NEG+k],2));
		}

		printf("====Signal Fourier Coefficients====\n");
		printOutFC(fourierCoefficients,fcErr);
		printf("\n");

		free(fcBg);
		free(fcBgErr);
	}
	// Calculate Stokes Parameters from Fourier Coefficients.
	float* stokesParameters = malloc(NUMSTOKES*sizeof(float));
	float* spErr = malloc(NUMSTOKES*2*sizeof(float));
	calculateStokesFromFC(fourierCoefficients,fcErr,stokesParameters,spErr);

	printf("====Stokes Parameters====\n");
	printOutSP(stokesParameters,spErr);
	printf("\n");

	writeDataSummaryToFile(analysisFileName,backgroundFileName,dataFileName,
							comments,
							fourierCoefficients,fcErr,
							stokesParameters,spErr,
							avgCurrent,avgCurrentStdDev);

	free(fourierCoefficients);
	free(fcErr);
	free(stokesParameters);
	free(spErr);
	return 0;
}

int writeDataSummaryToFile(char* analysisFileName, char* backgroundFileName, char* dataFileName, 
							char* comments,
							float* fourierCoefficients, float* fcErr, 
							float* stokesParameters, float* spErr,
							float avgCurrent, float avgCurrentStdDev){
	FILE* dataSummary;
	// Record the results along with the raw data in a file.
	dataSummary=fopen(analysisFileName,"w");
	if (!dataSummary) {
		printf("Unable to open file: %s\n", analysisFileName);
		exit(1);
	}
	
	fprintf(dataSummary,"#File\t%s\n",analysisFileName);
	fprintf(dataSummary,"#DataFile\t%s\n",dataFileName);
	fprintf(dataSummary,"#BackgroundFile\t%s\n",backgroundFileName);
	fprintf(dataSummary,"#Comments\t%s\n",comments);
	fprintf(dataSummary,"#ALPHA\t%f\t%f\n",ALPHA,DALPHA);
	fprintf(dataSummary,"#BETA\t%f\t%f\n",BETA,DBETA);
	fprintf(dataSummary,"#DELTA\t%f\t%f\n",DELTA,DDELTA);
	fprintf(dataSummary,"#Current Adj. Intensity\t%s\n",(NORMCURR==1)?"Yes":"No");
	fprintf(dataSummary,"# SOS p1 & p2\t%f\n",pow(stokesParameters[1],2)+pow(stokesParameters[2],2));
	// Print out the header information
	fprintf(dataSummary,"p0\tp0ErrUp\tp0ErrDown\tp1\tp1ErrUp\tp1ErrDown\tp2\tp2ErrUp\tp2ErrDown\tp3\tp3ErrUp\tp3ErrDown\t");
	fprintf(dataSummary,"c0\tc0ErrUp\tc0ErrDown\tc4\tc4ErrUp\tc4ErrDown\ts4\ts4ErrUp\ts4ErrDown\ts2\ts2ErrUp\ts2ErrDown\t");
	fprintf(dataSummary,"c2\tc2ErrUp\tc2ErrDown\tAvg.Current\tAvg.CurrentStdDev\n");
	// Print out the data
	fprintf(dataSummary,"%f\t%f\t%f\t",stokesParameters[0],spErr[0+0],spErr[NUMSTOKES+0]);
	fprintf(dataSummary,"%f\t%f\t%f\t",stokesParameters[1],spErr[0+1],spErr[NUMSTOKES+1]);
	fprintf(dataSummary,"%f\t%f\t%f\t",stokesParameters[2],spErr[0+2],spErr[NUMSTOKES+2]);
	fprintf(dataSummary,"%f\t%f\t%f\t",stokesParameters[3],spErr[0+3],spErr[NUMSTOKES+3]);
	fprintf(dataSummary,"%f\t%f\t%f\t",fourierCoefficients[COS+0],fcErr[POS+COS+0],fcErr[NEG+COS+0]);
	fprintf(dataSummary,"%f\t%f\t%f\t",fourierCoefficients[COS+4],fcErr[POS+COS+4],fcErr[NEG+COS+4]);
	fprintf(dataSummary,"%f\t%f\t%f\t",fourierCoefficients[SIN+4],fcErr[POS+SIN+4],fcErr[NEG+SIN+4]);
	fprintf(dataSummary,"%f\t%f\t%f\t",fourierCoefficients[SIN+2],fcErr[POS+SIN+2],fcErr[NEG+SIN+2]);
	fprintf(dataSummary,"%f\t%f\t%f\t",fourierCoefficients[COS+2],fcErr[POS+COS+2],fcErr[NEG+COS+2]);
	fprintf(dataSummary,"%f\t%f\n",avgCurrent,avgCurrentStdDev);
	
	fflush(dataSummary);
	fclose(dataSummary);

	return 0;
}
