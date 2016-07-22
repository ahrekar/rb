/*
   Program to record polarization.
   RasPi connected to USB 1208LS.
   Target energy: USB1208LS Analog out Ch1 controls HP3617A. See pg 31 my lab book
   PMT Counts: data received from CTR in USB1208
*/

#define REVOLUTIONS 2
#define STEPSPERREV 1200
#define DATAPOINTSPERREV 75
#define DATAPOINTS (DATAPOINTSPERREV * REVOLUTIONS)
#define PI 3.14159265358979
#define HPCAL 28.1/960.0
#define NORMCURR 0 	// Set this to 1 to normalize the intensity with the current
#define DWELL 1		// The number of seconds to pause, letting electronics settle
#define ALPHA -43.9	// The constant Alpha (location of transmission axis), measured in degrees.
#define DALPHA 5.0	// The uncertainty in ALPHA
#define BETA 0.0	// The constant Beta_0 (beginning position of QWP relative to positive x axis  ) measured in degrees.
#define DBETA 5.0		// The uncertainty in BETA
#define DELTA 90.0	// The constant Delta (wave plate retardance) in degrees.
#define DDELTA 3.0	// The uncertainty in DELTA
#define DSTEP 0	// The uncertainty in the step size 
#define NUMSTOKES 4
#define COS 0			// Used for my fourier coefficient array. Cos stored first.
#define SIN (DATAPOINTS/2)	// Sin stored second
#define POS 0			// Used for my error array positive values are stored first.
#define NEG (DATAPOINTS)// Then negative values. 

int calculateStokesFromFC(float* fourierCoefficients, float* fcErr, float* stokesReturn, float* stokesErrReturn);
int processFileWithBackground(char* analysisFileName, char* backgroundFileName, char* dataFile, int datapointsPerRevolution, int revolutions, char* comments);
