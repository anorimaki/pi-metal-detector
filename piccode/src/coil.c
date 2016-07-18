#include "main.h"
#include "coil.h"
#include "config.h"
#include "samples.h"
#include "mathutil.h"
#include "adconvert.h"
#include "picconfig.h"


struct Coil coil;

#define COIL_HISTORY_PEAK   0
#define COIL_HISTORY_DECAY  1

struct
{
	int1 type;
	int16 value;
} coil_history;

void coil_init()
{
	IDLEN = 0; //Enter in Sleep mode if SLEEP is executed

	output_low(PI_COIL_CTRL_PIN);
	output_drive(PI_COIL_CTRL_PIN);

	output_low(PI_ADC_INDICATOR_PIN);
	output_drive(PI_ADC_INDICATOR_PIN);

	samples_init();
}


//
// Precondition: interrupts sould be disabled for good timing confidence
//
#inline
void coil_pulse()
{
	output_high(PI_COIL_CTRL_PIN);
	delay_us(coil.pulse_length);
	output_low(PI_COIL_CTRL_PIN);
}


void coil_check_samples_history(int1 type, int16 value)
{
	if ((type == coil_history.type) && (value == coil_history.value))
		return;

	coil_history.type = type;
	coil_history.value = value;
	
	samples_init();
}


#inline
int16 coil_read_peak()
{
	return adc_read(PI_COIL_VOLTAGE_CH);
}


int16 coil_peak_ref()
{
	coil_check_samples_history(COIL_HISTORY_PEAK, 0);
						
	disable_interrupts(GLOBAL);
	int16 measure = coil_read_peak();
	enable_interrupts(GLOBAL);

	return samples_add( measure );
}


int16 coil_peak()
{
	coil_check_samples_history(COIL_HISTORY_PEAK, coil.pulse_length);

	//Disable interrupts for good timing accuracy.
	disable_interrupts(GLOBAL);

	coil_pulse();
	delay_us(4);
	int16 measure = coil_read_peak();

	//Enable interrupts to capture read input switches
	enable_interrupts(GLOBAL);

	return samples_add( measure );
}


int16 coil_read_decay( int8 delay )
{
	//Disable interrupts for good timing accuracy.
	disable_interrupts(GLOBAL);

	coil_pulse();

	delay_us(delay);

	int16 measure = adc_read(PI_DECAY_SIGNAL_CH); 

	enable_interrupts(GLOBAL);
	
	return COIL_MAX_ADC_VALUE-measure;
}


int16 coil_custom_sample( int8 delay, int8 n_log2 )
{
	int8 n = 1 << n_log2;
	int16 sum = 0;
	for( int8 i; i<n; ++i ) {
		sum += coil_read_decay( delay );
		delay_ms( COIL_PULSE_PERIOD );
	}
	return sum >> n_log2;
}


int16 coil_custom_sample( int8 delay )
{
	coil_check_samples_history(COIL_HISTORY_DECAY, delay);
	
	int16 measure = coil_read_decay( delay );
	
	return samples_add( measure );
}


int16 coil_sample()
{
	return coil_custom_sample(coil.sample_delay);
}


signed int32 coil_normalize(int16 value, int16 zero, int16 new_max_value)
{
	return math_change_range( value - zero, 
				COIL_MAX_ADC_VALUE - zero, new_max_value );
} 
