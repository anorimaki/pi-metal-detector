#include "main.h"
#include "samples.h"



#define SAMPLES_HISTORY_SIZE_LOG	4
#define SAMPLES_UNDEFINED_VALUE		0xFFFF

struct Samples
{
	int16 values[SAMPLES_HISTORY_SIZE];
	int8 position;
	int32 sum;
};

struct Samples samples;


void samples_init()
{
	for( int8 i = 0; i<SAMPLES_HISTORY_SIZE; ++i )
		samples.values[i] = SAMPLES_UNDEFINED_VALUE;
	samples.position = 0;
	samples.sum = 0;
}


int16 samples_add( int16 value )
{
	int16 pulled_value = samples.values[samples.position];
	samples.values[samples.position] = value;
	
	++samples.position;
	if ( samples.position == SAMPLES_HISTORY_SIZE )
		samples.position = 0;
	
	samples.sum += value;
	
	if ( pulled_value == SAMPLES_UNDEFINED_VALUE )
		return value;		//Buffer not full: Return same added value
		
				//Buffer full: Return mean
	samples.sum -= pulled_value;
	return samples.sum >> SAMPLES_HISTORY_SIZE_LOG; 
}
	
	