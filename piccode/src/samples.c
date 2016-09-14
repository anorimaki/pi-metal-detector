#include "main.h"
#include "samples.h"
#include <math.h>


struct Samples
{
	int16 values[SAMPLES_HISTORY_MAX_SIZE];
	int8 position;
	int32 sum;
	int32 sum_sq;
	int16 mean;
	int8 size_log;
	int8 size;
};

struct Samples samples;


void samples_init( int8 size_log )
{
	samples.size_log = size_log;
	samples.size = 1 << size_log;
	for( int8 i = 0; i<samples.size; ++i )
		samples.values[i] = SAMPLES_UNDEFINED_VALUE;
	samples.position = 0;
	samples.sum = 0;
	samples.sum_sq = 0;
	samples.mean = SAMPLES_UNDEFINED_VALUE;
}

// To avoid overflow in sum_sq,
// value is limited to 8192 for a window size of 64 values
int16 samples_add( int16 value )
{
	int16 pulled_value = samples.values[samples.position];
	samples.values[samples.position] = value;
	
	++samples.position;
	if ( samples.position == samples.size )
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
	
	samples.mean = samples.sum >> samples.size_log; 
	
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
	aux >>= (samples.size_log-2);
	
	aux = samples.sum_sq - aux;
	aux >>= samples.size_log;
	
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

#define EFFICIENCY_SCALE_FACTOR 10
/*
 * Calculates efficiency with the formula: variance / u^2
 * https://en.wikipedia.org/wiki/Efficiency_(statistics)#Estimators_of_u.i.d._variables
 */
int16 samples_efficiency()
{
	if ( samples.mean == SAMPLES_UNDEFINED_VALUE ) {
		return SAMPLES_UNDEFINED_VALUE;
	}
	
	int32 sq_mean = samples.mean * samples.mean;
	int32 var = samples_variance();
	
	var <<= (EFFICIENCY_SCALE_FACTOR/2);
	sq_mean >>= (EFFICIENCY_SCALE_FACTOR/2);
	
	return var / sq_mean;
}
