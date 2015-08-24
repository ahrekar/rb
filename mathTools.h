#include <math.h>
float stdDeviation(float* value, int numValues);

float stdDeviation(float* value, int numValues){
	float stdDev, sum, avg;

	// First Calculate the Average value
	sum = 0.0;
	int i;
	for(i=0; i < numValues;i++){ 
		sum += value[i];
	}
	avg = sum / (float) numValues;

	// Then calculate the Standard Deviation
	sum = 0.0;
	for(i=0; i < numValues;i++){
		sum += pow(avg-value[i],2);
	}
	stdDev = sqrt(sum/(numValues-1));

	return stdDev;
}
