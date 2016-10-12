#include "main.h"
#include "samples.h"
#include <math.h>


struct
{
	int8 size_log;
	int8 size;
	
	int16 values[SAMPLES_HISTORY_MAX_SIZE];
	int8 position;
	int32 sum;
	int16 mean;
	
	int32 sq_values[SAMPLES_HISTORY_MAX_SIZE];
	int8 sq_position;
	int32 sq_sum;
	int1 recalculate_sq_values;
	int32* sq_values_last;
} samples;

#define SQ_VALUES_ADDR(pos) *(int32*)(((int8*)&samples.sq_values) + (pos<<2))

/*
 * About arrays;
 * 1. Index access to int32 array (p. ex. samples.sq_values[i]) 
 * generates a call to @MUL1616 function. Index access to a int16 array does 
 * not generate a mult opetation but it is worse than pointer access. A loop
 * to initialize a int16 array generates 32 instructions with index access
 * and 20 with pointer access.
 * An example about performances: the loop in samples_init for arrays 
 * initialization (size 64) takes 1.332ms with index access and 659us
 * with pointer access.
 * So, this code avoid index arrays access and uses pointer acces.
 *
 * 2. Pointer comparison in loop conditions are avoided too. It is most faster 
 * to compare and increment an int8 than compare a pointer variable (2 bytes).
 */


void samples_init( int8 size_log )
{
	samples.size_log = size_log;
	samples.size = 1 << size_log;
	samples.sq_values_last = &samples.sq_values + samples.size;

	int16* values_ptr = &samples.values;
	int32* sq_values_ptr = &samples.sq_values;
	int8 i;
	for( i=0; i<samples.size; ++i, ++values_ptr, ++sq_values_ptr ) {
		*values_ptr = SAMPLES_UNDEFINED_VALUE;
		*sq_values_ptr = SAMPLES_UNDEFINED_VALUE;
	}
	samples.position = 0;
	samples.sq_position = 0;
	
	samples.sum = 0;
	samples.sq_sum = 0;
	
	samples.mean = SAMPLES_UNDEFINED_VALUE;
	samples.recalculate_sq_values = 0;
}




// To avoid overflow in sum_sq,
// value is limited to 5792 for a maximun window size of 128 values
int16 samples_add( int16 value )
{
	int16* value_ptr = &samples.values + samples.position;
	int16 pulled_value = *value_ptr;
	*value_ptr = value;
	
	samples.sum += value;
	
	++samples.position;
	if ( samples.position == samples.size )
		samples.position = 0;
	
	if ( samples.position == samples.sq_position ) {
		//variance has not been recalculated and all the window has
		//new values to be processed
		samples.recalculate_sq_values = 1;
	}
	
	if ( pulled_value == SAMPLES_UNDEFINED_VALUE )
		return value;		//Buffer not full: Return same added value
	
				//Buffer full: Return mean
	samples.sum -= pulled_value;
	samples.mean = samples.sum >> samples.size_log;
	
	return samples.mean;
}



inline void update_squared_sums() 
{
	if ( samples.recalculate_sq_values ) {
		samples.sq_sum = 0;
		int32* sq_value_ptr = &samples.sq_values;
		int16* value_ptr = &samples.values;
		for( int8 i=0; i<samples.size; ++i, ++value_ptr, ++sq_value_ptr ) {
			int16 value = *value_ptr;
			int32 sq_value = _mul( value, value );
			*sq_value_ptr = sq_value;
			samples.sq_sum += sq_value;
		}
		samples.recalculate_sq_values = 0;
		samples.sq_position = samples.position;
		return;
	}
	
	int32* sq_value_ptr = &samples.sq_values + samples.sq_position;
	int16* value_ptr = &samples.values + samples.sq_position;
	while( samples.position != samples.sq_position ) {
		int32 pulled_value = *sq_value_ptr;
		
		int16 value = *value_ptr;
		int32 sq_value = _mul( value, value );
		*sq_value_ptr = sq_value;
		samples.sq_sum += sq_value;
		
		++samples.sq_position;
		if ( samples.sq_position == samples.size ) {
			samples.sq_position = 0;
			sq_value_ptr = &samples.sq_values;
			value_ptr = &samples.values;
		}
		else {
			++sq_value_ptr;
			++value_ptr;
		}
	
		if ( pulled_value != SAMPLES_UNDEFINED_VALUE )
			samples.sq_sum -= pulled_value;
	}
	
#if 0
	int16* values_ptr = &samples.values;
	int8 i;
	samples.sq_sum = 0;
	for( i=0; i<samples.size; ++i, ++values_ptr ) {
		int16 value16 = *values_ptr;
		if ( value16 == SAMPLES_UNDEFINED_VALUE )
			break;
		
		int32 value = value16;
		int32 sq_value = value*value;
		samples.sq_sum += sq_value;
	}
#endif
}


/*
 * Calculates variance with the formula: sum(x{i}^2)/N - u^2
 * Where u is the mean: sum(x{i})/N
 */
int32 samples_variance() 
{
	int32 sq_mean = _mul( samples.mean, samples.mean );
	
	update_squared_sums();
	int32 aux = samples.sq_sum >> samples.size_log;

	return aux - sq_mean;
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
	
	int32 sq_mean = _mul( samples.mean, samples.mean );
	int32 var = samples_variance();
	
	var <<= (EFFICIENCY_SCALE_FACTOR/2);
	sq_mean >>= (EFFICIENCY_SCALE_FACTOR/2);
	
	return var / sq_mean;
}
