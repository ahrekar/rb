/*
   Program to analyze Faraday rotation data.
   This first iteration will just take a file
   of our current faraday rotation as input and
   then output a number density. It will accomplish
   this by using a fitting routine implemented in C
   and taken from the web. Search for "C Program for
   fitting curves" to find the relevant .pdf.
   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define maxnpts 50      /* Maximum data pairs - increase if desired */
#define NUMITERATIONS 500
#define NUMITERATIONSERR 1000
/*Change nterms to the number of parameters to be fit in your equation*/
/***********************************************************/ 
#define nterms 3       /* Number of parameters to be fit */
/***********************************************************/

/*Nonlinear Least Squares Curve Fitting Program*/

/*Marquardt algorithm from P.R. Bevington,"Data Reduction and Error 
  Analysis for the Physical Sciences," McGraw-Hill, 1969; Adapted by 
  Wayne Weimer & David Harris. Jackknife error algorithm of M.S. Caceci, 
  Anal. Chem. 1989, 6/, 2324. Translated to ANSI C by Douglas Harris & 
  Tim Seufert 7-94 */
int param, iteration, nloops, n, cycle, nfree;
int npts;                                 /* Number of data pairs */
long double x[maxnpts],y[maxnpts],sigmay[maxnpts]; /*x,y,y uncertainty*/
long double weight[maxnpts];                   /* Weighting factor */
long double yfit[maxnpts];                /* Calculated values of y */
long double a[nterms], sigmaa[nterms], b[nterms]; /* a[i]=c[i] params */ 
long double beta[nterms], c[nterms]; /* To be fit by program */ 
long double finala[nterms], lastsigmaa[nterms];
long double alpha[nterms][nterms], arry[nterms][nterms];
long double aug[nterms] [nterms*2];        /* For matrix inversion */
long double deriv[maxnpts] [nterms]; /* Derivatives */ 
long double flambda; /*Proportion of gradient search(=0.001 at start)*/ 
long double chisq; /* Variance of residuals in curve fit */ 
long double chisql, fchisq, sy;
char errorchoice;
char fileName[1024];
FILE *fp;

void readdata(char* fileName),            unweightedinput(void); 
void weightedinput(void),       chisquare(void);
void calcderivative(void),      matrixinvert(void);
void curvefit(int npoints),     display(void);
void uncertainties(void),       jackknifedata(char *fileName, int k);
void print_data(void);
int fitToWu(char* fileName);

int main (int argc, char **argv)
{
	strcpy(fileName,argv[1]);
	fitToWu(fileName);
	return 0;
}


int fitToWu(char* fileName){/* The main routine */
	int i;
	errorchoice='2';
	readdata(fileName);
	a[0]=1;
	a[1]=1;
	a[2]=1;
	flambda = 0.001; iteration = 0; cycle = 0; 
	for(i=0; i < NUMITERATIONS; i++){
		iteration++;
		curvefit(npts);
	}
	uncertainties(); 
	return 0;
}

/*******************************/
long double func(int i)     /* The function you are fitting*/
	/*******************************/
{
	int loop;     long double value;
	long double alpha;
	if (param==1) {
		for (loop=0; loop<nterms; loop++)   c[loop] = b[loop];
	} else {
		for (loop=0; loop<nterms; loop++)   c[loop] = a[loop];
	}

	alpha = 10;

	/********************************************/ 
	/*Enter the function to be fit:*/
	/********************************************/
	//value = c[2]/pow(x[i],4) + c[1]/pow(x[i],2) + c[0]; //Wu Equation.

	value = c[0] + c[1]*x[i] + c[2]*pow(x[i],2);  // Test Function
	/* x[i] is the independent variable */
	/* Values of c[n], c[n-1], c[0] are determined by least squares */ 
	/* nterms must be set to equal n+1 */
	/* Example of another function: value = c[2]*x[i]*x[i]+c[1]*x[i]+c[0]*/
	return (value);
}


void readdata(char* fileName)
{
	int n = 0;
	fp = fopen(fileName, "r");
	if (fp == NULL) {
		printf("Fatal error: could not open file %s\n", fileName); 
		exit(1);
	}

	char trash[1024];
	do{
		fgets(trash,1024,fp);
	} while (trash[0]=='#');

	for(n=0; !feof(fp); n++) {
		fscanf(fp,"%Lf\t%Lf\t%Lf\n",&x[n],&y[n],&sigmay[n]);
		if (errorchoice == '1')  sigmay[n] = 1.0;
	}
	fclose(fp);
	npts = n - 1;
}

void chisquare(void) /* Sum of squares of differences between */
	/*         measured and calculated y values */
{
	int i;
	fchisq = 0;
	for (i=0; i<npts; i++) {
		weight[i] = 1 / (sigmay[i] * sigmay[i]);
		fchisq += weight[i] * ( y[i] - yfit[i] ) * ( y[i] - yfit[i] );
	}
	fchisq /= nfree;
}

void calcderivative(void)                                                   /* Numerical derivative */
{
	int i, m;                    long double atemp, delta;
	for (m=0; m<nterms; m++) {
		atemp = a[m]; delta = fabs( a[m] / 100000 ); a[m] = atemp + delta; 
		for (i=0; i<npts; i++) deriv[i][m] = ( func(i) - yfit[i] ) / delta; 
		a[m] = atemp;
	}
}



void matrixinvert(void) /* Inverts the matrix arry[]                                                                                     */
	/*A method called pivoting reduces rounding error*/
{     
	int i, j, k, ik[nterms], jk[nterms];         
	long double rsave, amax;
	for (k=0; k<nterms; k++) { 
		amax =0.0;
		for (i=k; i < nterms; i++) { 
			for (j=k; j < nterms; j++) {
				if (abs(amax) <= abs(arry[i] [j])) {
					amax = arry[i][j]; 
					ik[k] = i;    jk[k] = j;
				}
			}
		}
		i = ik[k]; 
		if (i>k) {
			for (j=0; j < nterms; j++) {
				rsave = arry[k] [j];
				arry[k] [j] = arry[i] [j];
				arry[i] [j] = -1 * rsave;
			}
		}
		j = ik[k]; 
		if (j>k) {
			for (i=0; i < nterms; i++) {
				rsave = arry [i] [k];
				arry[i] [k] = arry[i] [j];                                        arry[i] [j] = -1 * rsave;
			}
		}
		for (i=0; i<nterms; i++) {
			if (i!=k){
				arry[i] [k] = -1 * arry[i] [k] / amax;
			}
		}
		for (i=0; i<nterms; i++) {
			for (j=0; j<nterms; j++) {
				if (j!=k && i!=k) {
					arry[i] [j] = arry[i] [j] + arry[i] [k] * arry[k][j];
				}
			}
		}
		for (j=0; j<nterms; j++) {
			if (j!=k) {
				arry[k] [j] = arry[k][j]/amax;
			}
		}
		arry[k] [k] = 1/amax;
	}
	for (k=nterms-1; k>-1; k--){
		j = ik[k];
		if (j>k){
			for (i=0; i<nterms; i++) { 
				rsave = arry[i] [k];
				arry[i] [k] = -1 * arry[i] [j];  arry[i] [j] = rsave; 
			}
		}
		i = jk[k];
		if (1>k) {
			for (j=0; j<nterms; j++) { 
				rsave = arry[k] [j];
				arry[k] [j] = -1 * arry[i] [j];  arry[i] [j] = rsave;
			}
		}
	}
}

void curvefit(int npoints)             /* Curve fitting algorthim */
{
	int i, j, k;
	nfree = npoints - nterms;
	for (j=0; j<nterms; j++) {
		b[j] = beta[j] = 0;
		for (k=0; k<=j; k++)    alpha[j] [k] = 0;
	}
	param=0;
	for (i=0; i<npoints; i++)    yfit[i] = func(i);
	chisquare();
	chisql = fchisq;
	calcderivative();
	for (i=0; i<npoints; i++) {
		for (j=0; j<nterms; j++) {
			beta[j] += weight[i] * ( y[i] - yfit[i] ) * deriv[i] [j]; 
			for (k=0; k<=j; k++)
				alpha[j] [k] += (weight[i] * deriv[i] [j] * deriv[i] [k]);
		}
	}
	for (j=0; j<nterms; j++) {
		for (k=0; k<=j; k++)   alpha[k][j] = alpha[j] [k];
	}
	nloops = 0; 
	do {
		param = 1;
		for (j=0; j<nterms; j++) {
			for (k=0; k<nterms; k++)
				arry[j] [k] = alpha[j] [k] / sqrt( alpha[j][j] * alpha[k] [k] );
			arry[j] [j] = flambda + 1;
		}
		matrixinvert();
		for (j=0; j<nterms; j++) { 
			b [j] = a [j] ;
			for (k=0; k<nterms; k++)
				b[j] += beta [k] * arry[j][k] / sqrt ( alpha[j] [j] * alpha[k][k]);
		}
		for (i=0; i<npoints; i++) yfit[i] = func(i);
		chisquare();
		if ( (chisql - fchisq) < 0 ) flambda *= 10;
		nloops++;
	}  while ( fchisq > chisql );
	for (j=0; j<nterms; j++) {
		a[j] = b[j];
		sigmaa[j] = sqrt( arry[j] [j] / alpha[j] [j] );
	}
	flambda /= 10;
}

void printResults(void)           /* Prints result of curvefit */
{
	int i;
	printf("\n%d\t", iteration); 
	for (i=0; i<nterms; i++) {
		printf("%- #12.8Lf\t", a[i]);
		finala[i] = a[i];
	}
	//Sum of squares of residuals
	printf("%- #12.8Lf\n", fchisq * nfree); 
	sy = sqrt(fchisq);
}

void uncertainties(void) /*Calculates uncertainties by removing one*/
	/*data point and recalculating parameters*/
{
	int i,k;   long double ajack[nterms][maxnpts], avajack[nterms];

	cycle++;
	for (i=0; i<npts; i++) {
		jackknifedata(fileName, i);
		for (k=0; k<=NUMITERATIONSERR; k++) curvefit(npts-1);
		for (k=0; k<nterms; k++) ajack[k] [i] = a[k];
	}
	for (k=0; k<nterms; k++) avajack[k] = 0; // Zero the jack values.
	for (k=0; k<nterms; k++) {
		for (i=0; i<npts; i++) avajack[k] += ajack[k][i]; // Sum the values from the matrix
		avajack[k] = avajack[k]/npts; // Divide by num points to get average.
	}
	for (k=0; k<nterms; k++) sigmaa[k] = 0; // Zero the sigma values.
	for (k=0; k<nterms; k++) {
		for (i=0; i<npts; i++)
			sigmaa[k] += (ajack[k][i]-avajack[k])*(ajack[k][i]-avajack[k]); 
		sigmaa [k] = sqrt ( (npts-1) * sigmaa[k]/npts );
		printf("%- 12.8Lf\t%- 12.8Lf\t",a[k],sigmaa[k]); 
	}
	printf("%- #12.8Lf\n", fchisq * nfree); 
	printf("\n");
}

void jackknifedata(char *fileName, int k)
	/* Removes one data point */
{
	int n = 0;
	fp = fopen(fileName, "rb"); 

	// First skip over the comment lines
	char trash[1024];
	do{
		fgets(trash,1024,fp);
	} while (trash[0]=='#');
	while (!feof(fp)){
		fscanf(fp,"%Lf\t%Lf\t%Lf\n",&x[n],&y[n],&sigmay[n]);
		if (errorchoice == '1')  sigmay[n] = 1.0;
		weight [n] = 1/(sigmay[n]*sigmay[n]);
		n++;
	}
/**
	for(n=0; !feof(fp); n++) {
		fscanf(fp,"%Lf\t%Lf\t%Lf\n",&x[n],&y[n],&sigmay[n]);
		if (errorchoice == '1')  sigmay[n] = 1.0;
	}
	**/
	npts = n-1; 
	fclose(fp); 
	for (n=0; n<(npts-1); n++) {
		if (n>=k) {
			x[n] = x[n+1];  y[n] = y[n+1]; weight[n] = weight[n+1];
		}
	}
}
