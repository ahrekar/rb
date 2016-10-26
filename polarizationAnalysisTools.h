/*
   Program to record polarization.
   RasPi connected to USB 1208LS.
   Target energy: USB1208LS Analog out Ch1 controls HP3617A. See pg 31 my lab book
   PMT Counts: data received from CTR in USB1208
*/

#define REVOLUTIONS 2
#define STEPSPERREV 1200
#define DATAPOINTSPERREV 60
#define DATAPOINTS (DATAPOINTSPERREV * REVOLUTIONS)
#define PI 3.14159265358979
#define HPCAL 28.1/960.0
#define NORMCURR 0 	// Set this to 1 to normalize the intensity with the current
#define DWELL 1		// The number of seconds to pause, letting electronics settle
#define ALPHA 3.2	// The constant Alpha (location of transmission axis), measured in degrees.
#define DALPHA 0.0	// The uncertainty in ALPHA
#define BETA -6.95	// The constant Beta_0 (beginning position of QWP relative to positive x axis  ) measured in degrees.
#define DBETA 0.0		// The uncertainty in BETA
#define DELTA 90.0	// The constant Delta (wave plate retardance) in degrees.
#define DDELTA 2.0	// The uncertainty in DELTA
#define DSTEP 0	// The uncertainty in the step size 
#define NUMSTOKES 4
#define POS 0			// Used for my error array positive values are stored first.
#define NEG (DATAPOINTS/2)// Then negative values. 

int calculateFourierCoefficients(char* fileName, int dataPointsPerRevolution, int Revolutions, int normalizeWithCurrent, float* fcCosRet, float* fcCosErrRet, float* fcSinRet, float* fcSinErrRet, float* avgCurrentReturn, float* avgCurrentStdDevReturn);
int calculateStokesFromFC(float* fcCos, float* fcCosErr, float* fcSin, float* fcSinErr, float* stokesReturn, float* stokesErrReturn);
int processFileWithBackground(char* analysisFileName, char* backgroundFileName, char* dataFile, int datapointsPerRevolution, int revolutions, int normalizeWithCurrent);
// Calculates the ith relative stokes parameter.
float calculateStokes(int i, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4);
int writeDataSummaryToFile(char* analysisFileName, char* backgroundFileName,char* rawDataFileName,  
							int normalizeWithCurrent, char* comments,
							float* fcCos, float* fcCosErr, float* fcSin, float* fcSinErr,
							float* stokesParameters, float* spError,
							float avgCurrent, float avgCurrentStdDev);
int writeDataSummaryFileEntry(char* analysisFileName, char* backgroundFileName, char* dataFileName, 
							int normalizeWithCurrent, char* comments,
							float* fcCos, float* fcCosErr, float* fcSin, float* fcSinErr,
							float* stokesParameters, float* spErr,
							float avgCurrent, float avgCurrentStdDev);
int writeDataSummaryFileColumnHeadings(char* analysisFileName);
int writeDataSummaryFileHeader(char* analysisFileName, char* backgroundFileName, char* dataFileName, char* comments);
int printOutSP(float* sp, float* spError);
