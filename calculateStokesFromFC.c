/** This program will be used to calculate Stokes Parameters from fourier coefficients when a small 
 * number of them need to be computed, i.e. if I am computing the average polarization from two runs.
 *
 * Input, the 5 relevant fourier coefficients.
 * Output, the 4 relative Stokes Parameters
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "polarizationAnalysisTools.h"
#define NUMFOURIER 5

float calculateStokesErr2(int i, int signOfError, float alpha, float beta, float delta, float* fc, float* fcErr);

int main(int argc, char** argv){
	float delta=2*PI*(DELTA)/360.0;
	float alpha=2*PI*(ALPHA)/360.0;
	float beta_0=2*PI*(BETA)/360.0;
	float* stokesParameters = calloc(NUMSTOKES,sizeof(float));
	float* spErr = calloc(NUMSTOKES*2,sizeof(float));
	float* fc = calloc(NUMFOURIER,sizeof(float)); //The fourier coefficients
	float* fcErr = calloc(NUMFOURIER*2,sizeof(float));
	int i;
	if(argc!=6 && argc!=18){
		// The number of arguments is not correct, help the silly
		// user with a usage statement.
		printf("Usages:\n\t./calculateStokesFromFC <float c0> <float c4> <float s4> <float s2> <float c2>\n");
		printf("\t./calculateStokesFromFC <float c0> <float c0upErr> <float c0downErr> <float c4> <float c4upErr> <float c4downErr> <float s4> <float s4upErr> <float s4downErr> <float s2> <float s2upErr> <float s2downErr> <float c2> <float c2upErr> <float c2downErr>\n");
		return 1;
	} else if (argc==6){
		/**
		for(i=0;i<NUMFOURIER;i++){
			fc[i]=atof(argv[1+i]);
		}
		**/
		fc[0]=atof(argv[1+0]);
		fc[1]=atof(argv[1+4]);
		fc[2]=atof(argv[1+1]);
		fc[3]=atof(argv[1+3]);
		fc[4]=atof(argv[1+2]);
		for(i=0;i<NUMSTOKES;i++){
			stokesParameters[i]=calculateStokes(i,alpha,beta_0,delta,fc[0],fc[1],fc[2],fc[3],fc[4]);
		}
		for(i=0;i<NUMSTOKES*2;i++){
			spErr[i]=0;
		}
	} else if (argc==16){
		int pos=0;
		int neg=NUMSTOKES;
		/**
		for(i=0;i<NUMFOURIER;i++){
			fc[i]=atof(argv[1+i*3]);
			fcErr[i]=atof(argv[1+i*3+1]);
			fcErr[i+NUMFOURIER]=atof(argv[1+i*3+2]);
		}
		**/
		fc[0]=atof(argv[1+0*3]);
		fc[1]=atof(argv[1+4*3]);
		fc[2]=atof(argv[1+1*3]);
		fc[3]=atof(argv[1+3*3]);
		fc[4]=atof(argv[1+2*3]);
		for(i=0;i<2;i++){
			fcErr[0+i*NUMFOURIER]=atof(argv[1+0*3+1+i]); // 1 to skip the program name, x*3 to skip other args and errors, +1 to skip value, i for
													 		//pos or negative errors.
			fcErr[1+i*NUMFOURIER]=atof(argv[1+4*3+1+i]);
			fcErr[2+i*NUMFOURIER]=atof(argv[1+2*3+1+i]);
			fcErr[3+i*NUMFOURIER]=atof(argv[1+3*3+1+i]);
			fcErr[4+i*NUMFOURIER]=atof(argv[1+1*3+1+i]);
		}
		for(i=0;i<NUMSTOKES;i++){
			stokesParameters[i]=calculateStokes(i,alpha,beta_0,delta,fc[0],fc[1],fc[2],fc[3],fc[4]);
			spErr[pos+i]=calculateStokesErr2(i,pos,alpha,beta_0,delta,fc,fcErr);
			spErr[neg+i]=calculateStokesErr2(i,neg,alpha,beta_0,delta,fc,fcErr);
		}
	}
	printOutSP(stokesParameters,spErr);
	return 0;
}

float calculateStokesErr2(int i, int signOfError, float alpha, float beta, float delta, float* fc, float* fcErr){
	int numVars = 8;
	float* temp = calloc(numVars,sizeof(float));
	float totalError=0;
	int sgn=1;
	float dDelta=2*PI*(DDELTA)/360.0;
	float dAlpha=2*PI*(DALPHA)/360.0;
	float dBeta=2*PI*(DBETA)/360.0;

	if(signOfError == NUMSTOKES){
		sgn=-1;
	}

	float k;
	      k=calculateStokes(i,alpha, beta, delta, fc[0], fc[1], fc[2], fc[3], fc[4]);
	temp[0]=calculateStokes(i,   alpha+sgn*(dAlpha)     , beta, delta, fc[0], fc[1], fc[2], fc[3], fc[4])-k;
	temp[1]=calculateStokes(i,alpha, beta  +  sgn*dBeta       , delta, fc[0], fc[1], fc[2], fc[3], fc[4])-k;
	temp[2]=calculateStokes(i,alpha, beta,     delta+sgn*dDelta      , fc[0], fc[1], fc[2], fc[3], fc[4])-k;
	temp[3]=calculateStokes(i,alpha, beta, delta, fc[0]+fcErr[signOfError+0], fc[1], fc[2], fc[3], fc[4])-k;
	temp[4]=calculateStokes(i,alpha, beta, delta, fc[0], fc[1]+fcErr[signOfError+1], fc[2], fc[3], fc[4])-k;
	temp[5]=calculateStokes(i,alpha, beta, delta, fc[0], fc[1], fc[2]+fcErr[signOfError+2], fc[3], fc[4])-k;
	temp[6]=calculateStokes(i,alpha, beta, delta, fc[0], fc[1], fc[2], fc[3]+fcErr[signOfError+3], fc[4])-k;
	temp[7]=calculateStokes(i,alpha, beta, delta, fc[0], fc[1], fc[2], fc[3], fc[4]+fcErr[signOfError+4])-k;

	int j;
	for(j=0;j<numVars;j++)
		totalError+=pow(temp[j],2);
	
	free(temp);

	return sqrt(totalError);
}
