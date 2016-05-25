#include "main.h"
#include "setup.h"
#include "display.h"
#include "coil.h"
#include "input.h"
#include "samples.h"


#define MAX_PULSE_TIME	500		//In us
#define MIN_PULSE_TIME	10		//in us


#inline
int8 pulse_time_increment( int16 switch_on_time ) {
	int8 ret = switch_on_time >> 6;
	return (ret==0) ? 1 : ret;
}


signed int16 get_increment( int16 current, int16 min, int16 max ) {
	signed int16 increment;
	
	if ( in_switches[SWITCH_INCREMENT].state ) {
		int16 raw_inc = pulse_time_increment( 
								in_switches[SWITCH_INCREMENT].state_time );
		increment = max-current;
		increment = (increment>raw_inc) ? raw_inc : increment;
		in_switches[SWITCH_INCREMENT].state_time = 0;
	}
	
	if ( in_switches[SWITCH_DECREMENT].state ) {
		int16 raw_dec = pulse_time_increment( 
								in_switches[SWITCH_DECREMENT].state_time );
		increment = current-min;
		increment = (increment>raw_dec) ? -raw_dec : -increment;
		in_switches[SWITCH_DECREMENT].state_time = 0;
	}
	
	return increment;
}


int1 setup_coil_pulse()
{
	// Calculates reference for 5 volts with the mean of 8 samples 
	int8 reference_5v;
	
	reference_5v = coil_peak_ref();
	
	while (TRUE) {
		if ( in_switches[SWITCH_MODE].state )
			return in_wait_for_release_timeout( SWITCH_MODE, 2000 );
		
		if ( in_switches[SWITCH_AUTOSET].state ) {
			reference_5v = coil_peak_ref();
			dsp_setup_coil_pulse_ref( reference_5v );
			delay_ms(1000);
		}
		
		signed int16 inc = get_increment( coil.pulse_length,
										MIN_PULSE_TIME, MAX_PULSE_TIME );
		if ( inc != 0 ) {
			coil.pulse_length += inc;
			coil_reset_history();
		}
		
		int16 peak = coil_peak();
		
		dsp_setup_coil_pulse(peak, reference_5v);
		
		delay_ms(100);
	}
}


int1 setup_zero_point() 
{
			//Delay to calculate min zero: 3/2 of MAX_SAMPLE_DELAY
	int16 min_zero = coil_sample();
	
	while (TRUE) {
		if ( in_switches[SWITCH_MODE].state )
			return in_wait_for_release_timeout( SWITCH_MODE, 2000 );
		
		if ( in_switches[SWITCH_AUTOSET].state ) {
			//Set to 10% of max value
			coil.zero = min_zero - (min_zero/10);
		}

		coil.zero += get_increment( coil.zero, 0, min_zero );

		dsp_setup_zero_point( min_zero );
		
		delay_ms(100);
	}
}


int16 autoset_sample_delay() {
	int16 ret = MIN_SAMPLE_DELAY;
	while( ret < MAX_SAMPLE_DELAY ) {
		int16 sample = coil_sample( ret, 3 );
		dsp_setup_sample_delay( sample );
		if( sample <= 0 )
			return ret;
		++ret;
		delay_ms( 100 );
	}
	return ret;
}


int1 setup_sample_delay()
{
	while (TRUE) {
		if ( in_switches[SWITCH_MODE].state )
			return in_wait_for_release_timeout( SWITCH_MODE, 2000 );
		
		if ( in_switches[SWITCH_AUTOSET].state ) {
			coil.sample_delay = autoset_sample_delay();
			coil_reset_history();
		}
		
		signed int16 inc = get_increment( coil.sample_delay, 
										MIN_SAMPLE_DELAY, MAX_SAMPLE_DELAY );
		if ( inc != 0 ) {
			coil.sample_delay += inc;
			coil_reset_history();
		}

		signed int8 strength = pi_sample();
		dsp_setup_sample_delay( strength );
	}
}



void setup()
{
	if ( setup_coil_pulse() ) {
		return;
	}

	if ( setup_zero_point() ) {
		return;
	}
	
	if ( setup_start_sample_delay() ) {
		return;
	}
	/*
		if ( !setup_sample_time() ) {
			cnf_save_sample_time();
			return;
		}        

		if ( !setup_start_second_sample_delay() ) {
			cnf_start_second_sample_delay();
			return;
		}
	 */
}
