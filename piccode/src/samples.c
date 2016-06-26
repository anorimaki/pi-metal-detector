#include "main.h"
#include "samples.h"
#include <math.h>


#define SAMPLES_UNDEFINED_VALUE		0xFFFF

struct Samples
{
	int16 values[SAMPLES_HISTORY_SIZE];
	int8 position;
	int32 sum;
	int32 sum_sq;
	int16 mean;
};

struct Samples samples;


void samples_init()
{
	for( int8 i = 0; i<SAMPLES_HISTORY_SIZE; ++i )
		samples.values[i] = SAMPLES_UNDEFINED_VALUE;
	samples.position = 0;
	samples.sum = 0;
	samples.sum_sq = 0;
}

// To avoid overflow in sum_sq,
// value is limited to 8192 for a window size of 64 values
int16 samples_add( int16 value )
{
	int16 pulled_value = samples.values[samples.position];
	samples.values[samples.position] = value;
	
	++samples.position;
	if ( samples.position == SAMPLES_HISTORY_SIZE )
		samples.position = 0;
	
	samples.sum += value;
	
	int32 value32 = value;
	samples.sum_sq += (value32*value32);
	
	if ( pulled_value == SAMPLES_UNDEFINED_VALUE )
		return value;		//Buffer not full: Return same added value
		
				//Buffer full: Return mean
	samples.sum -= pulled_value;
	
	int32 pulled_value32 = pulled_value;
	samples.sum_sq -= (pulled_value32*pulled_value32);
	
	samples.mean = samples.sum >> SAMPLES_HISTORY_SIZE_LOG; 
	
	return samples.mean;
}
	
/*
 * Calculates variance with the formula: sum(x{i}^2)/N - u^2
 * Where u is the mean: sum(x{i})/N
 */
int32 samples_variance() 
{
	int32 aux = samples.sum >> 1;	//Divide before square to avoid overflows
	aux = (aux*aux); 
	aux >>= (SAMPLES_HISTORY_SIZE_LOG-2);
	
	aux = samples.sum_sq - aux;
	aux >>= SAMPLES_HISTORY_SIZE_LOG;
	
	return aux;
}


int16 samples_std_deviation() 
{
	return sqrt(samples_variance());
}


int16 samples_coeff_variation() 
{
	return samples_std_deviation() / samples.mean;
}

#define EFFICIENCY_SCALE_FACTOR 8
/*
 * Calculates efficiency with the formula: variance / u^2
 * https://en.wikipedia.org/wiki/Efficiency_(statistics)#Estimators_of_u.i.d._variables
 */
int16 samples_efficiency()
{
	int32 sq_mean = samples.mean * samples.mean;
	int32 var = samples_variance();
	
	var <<= (EFFICIENCY_SCALE_FACTOR/2);
	sq_mean >>= (EFFICIENCY_SCALE_FACTOR/2);
	
	return var / sq_mean;
}
