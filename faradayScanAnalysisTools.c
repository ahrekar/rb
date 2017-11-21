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
#include "fileTools.h" 

#define PI 3.14159265358979
#define NUMSTEPS 350	
#define STEPSIZE 25
#define STEPSPERREV 350.0
#define WAITTIME 2

#define BUFSIZE 1024

int plotData(char* fileName);
int plotRawData(char* fileName);
float calculateBdotL(float mag1Volt, float mag2Volt);
int recordNumberDensity(char* fileName);
int analyzeData(char* fileName, int runs, int revolutions, int dataPointsPerRevolution);
int	readInData(char* fileName,int totalDatapoints, int numAouts, int* aouts, float* wavelength, int* steps, float* intensity, float* inensityErr,int* homeFlag);
float calculateOneSumTerm(int trigFunc, int dataPointsPerRevolution,int revolutions,float intensity, float i,int k);
float calculateOneSumTermError(int trigFunc, int posOrNeg,int dataPointsPerRevolution, int revolutions, float intensity,float intensityErr, float i, float iErr, int k);
int fourierAnalysis(int dataPointsPerRevolution, int revolutions, int* steps, float* intensity, float* intensityErr, float* fourierCoefficients, float* fcErr);
float calculateAngleError(float c2, float c2Err, float s2, float s2Err);

// Okay, so gnuplot's fitting function isn't behaving like I want it to
// (not giving me "reasonable" answers), so I'm hacking together
// a quick automated data fitting scheme. It involves using Wolfram,
// so it's going to be kind of messy.
int calculateNumberDensity(char* fileName, int leftDataExclude, int rightDataExclude){
	float probeOffset,magnet1Volt,magnet2Volt;
	float aoutConv,aoutInt;
	float bDotL;
	int i=0;
	char buffer[BUFSIZE];
	//char script[BUFSIZE]="\\home\\pi\\RbControl\\shortGetFit.wl";
	FILE *wolfram;
	//wolfram = popen("wolfram","w"); 
	wolfram = popen("wolfram >> /dev/null","w"); 

	getCommentLineFromFile(fileName,"#ProbeOffset:",buffer);
	probeOffset=atof(buffer);
	getCommentLineFromFile(fileName,"#Mag1Voltage:",buffer);
	magnet1Volt=atof(buffer);
	getCommentLineFromFile(fileName,"#Mag2Voltage:",buffer);
	magnet2Volt=atof(buffer);

	aoutConv=9e-6*pow(probeOffset,2)+.0012*probeOffset-.0651;
	aoutInt=.6516*probeOffset-22.851;
	bDotL=calculateBdotL(magnet1Volt,magnet2Volt);


	if (wolfram != NULL){
		sprintf(buffer, "faradayData=Import[\"%s\",\"Data\"]\n", fileName);
		fprintf(wolfram, buffer);

		//sprintf(buffer, "Get[\"%s\"]",script);
		//Removes lines from the file (This one gets rid of the comments)
		//fprintf(wolfram, "faradayData=Delete[faradayData,{{1},{2},{3},{4},{5},{6},{7},{8}}]\n");			
		for(i=0;i<13;i++){
			fprintf(wolfram, "faradayData=Delete[faradayData,{{1}}];\n");			
			
		}
		//Removes unneccesary columns from the file
		for(i=0;i<7;i++){
			fprintf(wolfram, "faradayData=Drop[faradayData,None,{2}]\n");
		}
		fprintf(wolfram, "faradayData=Drop[faradayData,None,{3}]\n");
		fprintf(wolfram, "faradayData=Drop[faradayData,None,{3}]\n");
		//Removes lines from the file (This one gets rid of the lower frequency)
		for(i=0;i<leftDataExclude;i++){
			fprintf(wolfram, "faradayData=Delete[faradayData,Dimensions[faradayData][[1]]]\n");
		}
		//Removes lines from the file (This one gets rid of the higher frequency)
		for(i=0;i<rightDataExclude;i++){
			fprintf(wolfram, "faradayData=Delete[faradayData,{{1}}]\n");
		}
		fprintf(wolfram, "c=2.9979*^10\n");
		fprintf(wolfram, "re=2.8179*^-13\n");
		fprintf(wolfram, "fge=0.34231\n");
		fprintf(wolfram, "k=4/3\n");
		fprintf(wolfram, "Mb=9.2740*^-24\n");
		fprintf(wolfram, "BdotL=%f\n",bDotL);
		fprintf(wolfram, "h=6.6261*^-34\n");
		fprintf(wolfram, "vo=3.77107*^14\n");
		fprintf(wolfram, "pi=3.14159265\n");
		fprintf(wolfram, "aoutConv=%f\n",aoutConv);
		fprintf(wolfram, "aoutIntercept=%f\n",aoutInt);
		fprintf(wolfram, "const=c*re*fge*k*Mb*BdotL/(4*pi*h*vo)\n");
		fprintf(wolfram, "model=a+b*const*(vo+(aoutConv*detune+aoutIntercept)*1*^9)/((aoutConv*detune+aoutIntercept)*1*^9)^2+d*(vo+(aoutConv*detune+aoutIntercept))^5/((aoutConv*detune+aoutIntercept))^4\n");
		fprintf(wolfram, "modelResult=NonlinearModelFit[faradayData,model,{a,b,d},detune]\n");
		fprintf(wolfram, "initialAngle=Replace[a,modelResult[\"BestFitParameters\"][[1]]]\n");
		fprintf(wolfram, "detuneSquare=Replace[b,modelResult[\"BestFitParameters\"][[2]]]\n");
		fprintf(wolfram, "detuneFourth=Replace[d,modelResult[\"BestFitParameters\"][[3]]]\n");
		fprintf(wolfram, "initialAngle>>fitParams.txt\n");
		fprintf(wolfram, "detuneSquare>>>fitParams.txt\n");
		fprintf(wolfram, "detuneFourth>>>fitParams.txt\n");
		//fprintf(wolfram, "modelResult[\"ParameterErrors\"]>>>fitParams.txt\n");
	}
	return pclose(wolfram);
}

int recordNumberDensity(char* fileName){
	char analysisFileName[1024];
	strcpy(analysisFileName,fileName);
	char* extensionStart=strstr(analysisFileName,"RotationAnalysis.dat");
	strcpy(extensionStart,"DensityAnalysis.dat");
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
	char fileNameBase[1024];
	char* extension;
	strcpy(fileNameBase,fileName);
	extension = strstr(fileNameBase,".dat");
	strcpy(extension,"");

	FILE *gnuplot;
	gnuplot = popen("gnuplot","w"); 

    int aoutColumnNumber=1,angleColumnNumber=10;
	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb size 100,28\n");
		fprintf(gnuplot, "set output\n");			

		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Aout (Detuning)'\n");			
		fprintf(gnuplot, "set ylabel 'Theta'\n");			
		fprintf(gnuplot, "set xrange [0:1024] reverse\n");			
		sprintf(buffer, "plot '%s' using %d:%d:($%d+$%d):($%d+$%d) with errorbars\n",fileName,aoutColumnNumber,angleColumnNumber,angleColumnNumber,angleColumnNumber+1,angleColumnNumber,angleColumnNumber+2);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s.png'\n", fileNameBase);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using %d:%d:($%d+$%d):($%d+$%d) with errorbars\n",fileName,aoutColumnNumber,angleColumnNumber,angleColumnNumber,angleColumnNumber+1,angleColumnNumber,angleColumnNumber+2);
		fprintf(gnuplot, buffer);
	}
	return pclose(gnuplot);
}

int plotRawData(char* fileName){
	char buffer[BUFSIZE];
	char fileNameBase[1024];
	char* extension;
	strcpy(fileNameBase,fileName);
	extension = strstr(fileNameBase,".dat");
	strcpy(extension,"");

	FILE *gnuplot;
	gnuplot = popen("gnuplot","w"); 

    int stepColumnNumber=1;
    int intensityColumnNumber=4;
	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb size 100,28\n");
		fprintf(gnuplot, "set output\n");			

		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Angle (Step #)'\n");			
		fprintf(gnuplot, "set ylabel 'Intensity'\n");			
		sprintf(buffer, "plot '%s' using %d:%d\n",fileName,stepColumnNumber,intensityColumnNumber);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s.png'\n", fileNameBase);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using %d:%d\n",fileName,stepColumnNumber,intensityColumnNumber);
		fprintf(gnuplot, buffer);
	}
	return pclose(gnuplot);
}

int analyzeData(char* fileName, int runs, int revolutions, int dataPointsPerRevolution){
	char fileNameCopy[1024];
    char comments[1024];
    int fileExists=0;
	int totalDatapointsPerRun=dataPointsPerRevolution*revolutions;
	int cos=0;
	int sin=dataPointsPerRevolution/2;
	int pos=0;
	int neg=dataPointsPerRevolution;

	char* extensionStart;

	int numAouts= runs; 

	int* aouts=calloc(totalDatapointsPerRun*numAouts,sizeof(float)); 
	int* homeFlag=calloc(totalDatapointsPerRun*numAouts,sizeof(float)); 
	float* wavelength=calloc(totalDatapointsPerRun*numAouts,sizeof(float)); 
	float* intensity=calloc(totalDatapointsPerRun*numAouts,sizeof(float));
	float* intensityErr=calloc(totalDatapointsPerRun*numAouts,sizeof(float));
	int* steps=calloc(totalDatapointsPerRun*numAouts,sizeof(float)); 

	printf("Reading in data...");
	readInData(fileName,totalDatapointsPerRun*numAouts,numAouts,aouts,wavelength,steps,intensity,intensityErr,homeFlag);
	printf("Data read in. ");

	strcpy(fileNameCopy,fileName);
	extensionStart=strstr(fileNameCopy,".dat");
	strcpy(extensionStart,"RotationAnalysis.dat");

	FILE* analysis = fopen(fileNameCopy,"w");
	if (!analysis) {
		printf("Unable to open file %s\n",fileNameCopy);
		exit(1);
	}

	strcpy(fileNameCopy,fileName);
	extensionStart=strstr(fileNameCopy,"_");
	strcpy(extensionStart,".dat");
	fileExists=access(fileNameCopy,F_OK);

	FILE* daily = fopen(fileNameCopy,"a");
	if (!daily) {
		printf("Unable to open file %s\n",fileNameCopy);
		exit(1);
	}

	getCommentLineFromFile(fileName,"#Comments:",comments);

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

	fprintf(analysis,"Aout\tWavelength\tc0\ts2\ts2Err+\ts2Err-\tc2\tc2Err+\tc2Err-\tangle\tangleErr+\tangleErr-\thomeFlag\n");
    if(fileExists==-1)
        fprintf(daily,"Filename\tComments\tAout\tWavelength\tc0\ts2\ts2Err+\ts2Err-\tc2\tc2Err+\tc2Err-\tangle\tangleErr+\tangleErr-\thomeFlag\n");

	float* fourierCoefficients = calloc(totalDatapointsPerRun,sizeof(float));
	float* fcErr = calloc(totalDatapointsPerRun*2,sizeof(float));
	int i;
	float c0,s2,c2,angle,angleErrUp,angleErrDown,linearPart,linearPercent;

	printf("Beginning Data Analysis...\n");
	for(i=0;i<numAouts;i++){
		fourierAnalysis(dataPointsPerRevolution,revolutions,&steps[totalDatapointsPerRun*i],&intensity[totalDatapointsPerRun*i],&intensityErr[totalDatapointsPerRun*i],fourierCoefficients,fcErr);
		c0=fourierCoefficients[cos+0];
		s2=fourierCoefficients[sin+2];
		c2=fourierCoefficients[cos+2];
        linearPart=sqrt(pow(s2,2)+pow(c2,2));
        linearPercent=linearPart/c0;
		angle = 0.5*atan2(c2,s2);
		angleErrUp=calculateAngleError(c2,fcErr[pos+cos+2],s2,fcErr[pos+sin+2]);
		angleErrDown=calculateAngleError(c2,-fcErr[neg+cos+2],s2,-fcErr[neg+sin+2]);
		angle = angle*180.0/PI;
		angleErrUp = angleErrUp*180.0/PI;
		angleErrDown = angleErrDown*180.0/PI;
		printf("c0 = %f\tlinearPercent= %f\ts2 = %f\ts2Err = %f\tc2 = %f\tc2Err = %f\tangle = %f (%f)\n",c0,linearPercent,s2,fcErr[sin+pos+2],c2,fcErr[cos+pos+2],angle,angleErrUp);
		fprintf(analysis,"%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",aouts[totalDatapointsPerRun*i],wavelength[totalDatapointsPerRun*i],c0,s2,fcErr[sin+pos+2],fcErr[sin+neg+2],c2,fcErr[cos+pos+2],fcErr[cos+neg+2],angle,angleErrUp,angleErrDown,homeFlag[totalDatapointsPerRun*i]);
		fprintf(daily,"%s\t%s\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",fileName,comments,aouts[totalDatapointsPerRun*i],wavelength[totalDatapointsPerRun*i],c0,s2,fcErr[sin+pos+2],fcErr[sin+neg+2],c2,fcErr[cos+pos+2],fcErr[cos+neg+2],angle,angleErrUp,angleErrDown,homeFlag[totalDatapointsPerRun*i]);
	}
	free(aouts);
	free(intensity);
	free(intensityErr);
	free(steps);
	fclose(analysis);

	return 0;
}

int readInData(char* fileName,int totalDatapoints, int numAouts, int* aouts, float* wavelength, int* steps, float* intensity, float* intensityErr,int* homeFlag){
	FILE* data = fopen(fileName,"r");
    int dataPointsPerAout=totalDatapoints/numAouts;
    float discard;

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

	int i,j=0;
    for(j=0;j< numAouts; j++){
        fscanf(data,"\n\n#AOUT:%d(%f)\n",&aouts[j*dataPointsPerAout],&wavelength[j*dataPointsPerAout]);
        //printf("The aout is %d/%d and the code is %d and j is %d\n",aouts[j*dataPointsPerAout],numAouts,code,j);
        for (i=0; i < dataPointsPerAout; i++){
            fscanf(data,"%d\t%f\t%f\t%f\t%f\t%f\t%f\n",&steps[j*dataPointsPerAout+i],&discard,&discard,&intensity[j*dataPointsPerAout+i],&intensityErr[j*dataPointsPerAout+i],&discard,&discard);
            aouts[j*dataPointsPerAout+i]=aouts[j*dataPointsPerAout];
            wavelength[j*dataPointsPerAout+i]=wavelength[j*dataPointsPerAout];
            //fscanf(data,"%d\t%f\t%d\t%f\t%f\t%d\n",&aouts[i],&wavelength[i],&steps[i],&intensity[i],&intensityErr[i],&homeFlag[i]);
        }
    }
    /*
        code=fscanf(data,"\n\n#AOUT:%d(%f)\n",&aouts[j*dataPointsPerAout],&wavelength[j*dataPointsPerAout]);
        printf("The aout is %d/%d and the code is %d and j is %d\n",aouts[j*dataPointsPerAout],numAouts,code,j);
        for (i=0; i < dataPointsPerAout; i++){
            code=fscanf(data,"%d\t%f\t%f\t%f\t%f\n",&steps[j*dataPointsPerAout+i],&intensity[j*dataPointsPerAout+i],&intensityErr[j*dataPointsPerAout+i],&discard,&discard);
            printf("code %d\n",code);
            aouts[j*dataPointsPerAout+i]=aouts[j*dataPointsPerAout];
            wavelength[j*dataPointsPerAout+i]=wavelength[j*dataPointsPerAout];
        }
        j++;
        fscanf(data,"\n\n#AOUT:%d(%f)\n",&aouts[j*dataPointsPerAout],&wavelength[j*dataPointsPerAout]);
        printf("The aout is %d/%d and the code is %d and j is %d\n",aouts[j*dataPointsPerAout],numAouts,code,j);

     */

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


/** 
 * Returns the integrated magnetic field across the target cell in units of Tesla
 *
 * Inputs: The voltage across the first magnet, the voltage across the second magnet
 *
 * Ouput: The value of B dot L
 * */
float calculateBdotL(float mag1Volt, float mag2Volt){
	float numberOfMagnets=2;
	float quadDep[2]={0.085,0.153};
	float linDep[2]={-1.907,1.866};
	float offset[2]={17.24,15.594};
	float volt[2]={mag1Volt,mag2Volt};
	float voltToCurrConv[2] = {0.0504,0.0488};
	float voltToCurrOffset[2] = {-0.0113,-0.0069};
	float current[2];
	
	float totalQuadDep,totalLinDep,totalOffset;
	float targetCellEnd=2.794; // Measured in cm
	int i;

	for (i=0;i<numberOfMagnets;i++)
		current[i]=voltToCurrConv[i]*volt[i]+voltToCurrOffset[i];

	totalQuadDep=current[0]*quadDep[0]+current[1]*quadDep[1];
	totalLinDep=current[0]*linDep[0]+current[1]*linDep[1];
	totalOffset=current[0]*offset[0]+current[1]*offset[1];

	// We divide each of the above variables by the next higher order of their dependence,
	// these will be the coefficients after integration
	totalQuadDep=totalQuadDep/3;
	totalLinDep=totalLinDep/2;
	totalOffset=totalOffset/1;

	// Then we evaluate the integral at the end  points, since I set the front edge of the
	// target cell to zero, this means just evaluating at the end of the targetCell
	return (totalQuadDep*pow(targetCellEnd,3) + totalLinDep*pow(targetCellEnd,2) + totalOffset*targetCellEnd)/1e4;
}
