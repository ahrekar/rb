/*Nonlinear Least Squares Curve Fitting Program*/

/*Marquardt algorithm from P.R. Bevington,"Data Reduction and Error 
Analysis for the Physical Sciences," McGraw-Hill, 1969; Adapted by 
Wayne Weimer & David Harris. Jackknife error algorithm of M.S. Caceci, 
Anal. Chem. 1989, 6/, 2324. Translated to ANSI C by Douglas Harris & 
Tim Seufert 7-94 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define maxnpts 50      /* Maximum data pairs - increase if desired */

/*Change nterms to the number of parameters to be fit in your equation*/
/***********************************************************/ 
#define nterms 3       /* Number of parameters to be fit */
/***********************************************************/

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
char filename[20], answer[100];
FILE *fp;

void readdata(void),            unweightedinput(void); 
void weightedinput(void),       chisquare(void);
void calcderivative(void),      matrixinvert(void);
void curvefit(int npoints),     display(void);
void uncertainties(void),       jackknifedata(char *filename, int k);
void print_data(void);

int main(int argc, char** argv){              /* The main routine */
 int i;
 printf("Least Squares Curve Fitting. You must modify the constant\n"); 
 printf("'nterms' and the fuction 'Func' for new problems.\n"); 
 readdata();
 printf("\nEnter initial guesses for parameters:\n"); 
 printf("\t(Note: Parameters cannot be exactly zero.)\n"); 
 for (i=0; i<nterms; i++)
   do {
     printf("Parameter #%d = ", i+1); gets(answer);
   } while ( (a [i] = atof (answer)) -= 0.0 );

 flambda = 0.001;    iteration = 0; cycle = 0;
 do{
   curvefit(npts);
   iteration++; 
   display();
   printf("\n\tAnother iteration (Y/N)? "); gets(answer);
  } while (answer[0] != 'N' && answer[0] != 'n');
  printf("\nDo you want to calculate uncertainty in parameters (Y/N)?"); 
  gets(answer);
  if (answer [0] == 'Y' || answer[0] == 'y') uncertainties(); 
  return 0;
}

                               /*******************************/
long double func(int i)     /* The function you are fitting*/
                               /*******************************/
{
  int loop;     long double value;
  if (param==1) {
    for (loop=0; loop<nterms; loop++)   c[loop] = b[loop];
  } else {
    for (loop=0; loop‹ntelms; loop++)   c[loop] = a[loop];
  }

/********************************************/ 
/*Enter the function to be fit:*/
/********************************************/
  value = c[2]/pow(x[i],4) + c[1]/pow(x[i],2) + c[0]; /*van Deemter Equation*/
    /* x[i] is the independent variable */
    /* Values of c[n], c[n-1], c[0] are determined by least squares */ 
    /* nterms must be set to equal n+1 */
 /* Example of another function: value = c[2]*x[i]*x[i]+c[1]*x[i]+c[0]*/
  return (value);
}


void readdata(void)
{
  int n = 0;
  do {
    printf("\nDo you want to enter x,y values or read them from a 
file?\n");
    printf("\tType E for enter and F for File: "); gets(answer);
    answer[0] = toupper(answer[0]);
  } while (answer[0] != 'E' && answer[0] != 'F');
  if (answer[0]    'F') {
    do {
      printf("\nPlease enter the name of the data file: "); 
      gets(filename);
      printf("\n");
      fp = fopen(filename, "rb");
      if (fp == NULL) {
        printf("Fatal error: could not open file %s\n", filename); 
        exit(1);
	  }

      for(n=0; !feof(fp); n++) {
        fread(&x[n], sizeof(long double), 1, fP); 
        fread(&y[n], sizeof (long double), 1, fP); 
        fread(&sigmay[n], sizeof (long double), 1, fp); 
        if (errorchoice == '1')  sigmay[n] = 1.0;
	  }
      fclose(fp);
      npts = n - 1;
      print_data();
      printf("\nIs this data correct (Y/N)? "); gets(answer);
    } while (answer[0] != 'Y' && answer [0] != 'y');
  } else {
    do {
      printf("\nChoices for error analysis:\n");
      printf("\tl. Let the program weight all points equally\n"); 
      printf("\t2. Enter estimated uncertainty for each point\n\n"); 
      printf("Choose option 1 or 2 now: "); gets(answer);
    } while (answer[0] != '1' && answer[0] != '2');
    errorchoice = answer[0];
    do (
      if (errorchoice == '1')  unweightedinput();
      if (errorchoice == '2')  weightedinput();
      print_data();
      printf("Is this data correct (Y/N)? ");
      gets(answer);
    } while (answer[0] != 'y' && answer[0] != 'Y');
    printf("Enter name of file to save the data in: ");
    gets(filename); 
    fp=fopen(filename, "wb");
    if (fp == NULL) {
      printf("Fatal error: could not open file %-s\n", filename);
      exit(1);

    for(n=0; n<npts; n++) {
      fwrite(&x[n], sizeof(long double), 1, fp); 
      fwrite(&y[n], sizeof(long double), 1, fp); 
      fwrite(&sigmay[n], sizeof(long double), 1, fp);
    }
    fclose(fp);
    printf("Data saved in file %-s\n", filename);
	}
}

void print_data(void)        /* Displays the data entered */
{
  int i;
  for (i=0; i<npts; i++){
    printf("%d\tx = o #12.8Lf\ty =  #12.8Lf\t", i+1, x[i], y[i]);
    printf("Sigmay = %- #12.8Lf\n", sigmay[i]);
    weight[i] = 1 / ( sigmay[i] * sigmay[i] );
  }
	
}





void unweightedinput(void)   /* Enter equal weighted data */
{
  int i, n;
  printf("List the data in the order: x y, with one set on each\n"); 
  printf("line and a space (not a comma) between the numbers.\n"); 
  printf("Type END to end input\n");
  for (n=0; ;n++) {
    gets(answer);
    if (answer[0]=='E' || answer[0]=='e') break;
    x[n] = atof(answer);  i = 0;
    while (answer[i] != ' ' && answer[i] != '\0') i++;
    y[n] = atof(answer+i);  sigmay[n]=1;
  }
  npts = n;
}


void weightedinput(void)           /* Enter unequal weighted data */
{
  int i, n;
  printf("List the data in the order: x y sigmay, with one set on\n"); 
  printf("each line and a space (not a comma) between the numbers.\n");
  printf("Type END to end input\n");
  for (n=0; ;n++) {
	  gets(answer);
	  if (answer[0]=='E' || answer[0]=='e') break;
	  x[n] = atof(answer);    i = 0;
	  while (answer[i] != ' ' && answer[i] != '\0') i++; 
	  y[n] = atof(answer+i);    i++;
	  while (answer[i] != ' ' && answer[i] != '\0') i++; 
	  sigmay[n] = atof(answer+i);
  }
  npts = n;
}

void chisquare(void) /* Sum of squares of differences between */
                                                                 /*         measured and calculated y values */
{
      int i;
      fchisq = 0;
      for (i=0; i<npts; i++) {
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
         alpha[j] [k] +- (weight[i] * deriv[i] [j] * deriv[i] [k]);
	 }
   }
   for (j=0; j<nterms; j++) {
     for (k=0; k<=j; k++)   alpha [k] [j] = aipha[j] [k];
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

void display(void)           /* Prints result of curvefit */
{
  int i;
  printf("\nIteration #%d\n", iteration); 
  for (i=0; i<nterms; i++) {
    printf("A[%3d] = %- #12.8Lf\n", i, a[i]);
    finala[i] = a[i];
  }
  printf("Sum of squares of residuals = %- #12.8Lf", fchisq * nfree); 
  sy = sqrt(fchisq);
}

void uncertainties(void) /*Calculates uncertainties by removing one*/
                            /*data point and recalculating parameters*/
{
  int i,k;   long double ajack[nterms][maxnpts], avajack[nterms];

  do {
    cycle++;
    printf("Calculating uncertainties. Let me think!\n"); 
    for (i=0; i<npts; i++) {
      jackknifedata(filename, i);
      for (k=0; k<=iteration; k++) curvefit(npts-l);
      printf("Now playing with data point %d\n", i+1); 
      for (k=0; k<nterms; k++) ajack[k] [i] = a[k];
	}
    printf("\n\n");
    for (k=0; k<nterms; k++) avajack[k] = 0;
    for (k=0; k<nterms; k++) {
      for (i=0; i<npts; i++) avajack[k] += ajack[k][i];
      avajack[k] = avajack[k]/npts;
	}
    for (k=0; k<nterms; k++) sigmaa[k] = 0;
    for (k=0; k<nterms; k++) {
      for (i=0; i<npts; i++)
        sigmaa[k] += (ajack[k][i]-avajack[k])*(ajack[k] [i]-avajack[k]); 
	  sigmaa [k] = sqrt ( (npts-1) * sigmaa[k]/npts );
	  printf("Parameter[%d] = %- 12.8Lf +/- %- 12.8Lf\n",k, finala[k], 
           sigmaa[k]);
     if (cycle>1)
       printf("\t(Previous uncertainty = %- #12.8Lf)\n\n", 
             lastsigmaa[k]);
     lastsigmaa[k] = sigmaa[k];
	}
   printf("Standard deviation of y = %- #12.8Lf\n", sy); 
   printf("Above result is based %d iterations\n", iteration); 
   iteration += 5;
   printf("Iterations will now be increased to %d"
          " to see if the estimates of \n", iteration);
   printf("uncertainty change. When consecutive cycles give\n"); 
   printf("similar results, it is time to stop.\n");
   printf("\tDo you want to try another cycle now (Y/N)? "); 
   gets(answer);
  } while ( answer[0] == 'y' || answer[0] == 'Y' );
}


void jackknifedata(char *filename, int k)
                                    /* Removes one data point */
{
  int n = 0;
  fp = fopen(filename, "rb");
  while (!feof(fp)){
   fread(&x[n], sizeof(long double), 1, fp); 
   fread(&y[n], sizeof (long double), 1, fp); 
   fread(&sigmay[n], sizeof(long double), 1, fp); 
   if (errorchoice == '1') sigmay[n] = 1.0; 
   weight [n] = 1/(sigmay[n]*sigmay[n]);
   n++;
  }
  npts = n-1; 
  fclose(fp); 
  for (n=0; n<(npts-1); n++) {
    if (n>=k) {
      x[n] = x[n+1];  y[n] = y[n+1]; weight[n] = weight[n+1];
	}
  }
}
