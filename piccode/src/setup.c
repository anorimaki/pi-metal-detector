#include "main.h"
#include "setup.h"
#include "display.h"
#include "pimetaldec.h"
#include "config.h"
#include "input.h"


#define MAX_PULSE_TIME	500		//In us
#define MIN_PULSE_TIME	10		//in us


#inline
int8 pulse_time_increment( int16 switch_on_time ) {
	int8 ret = switch_on_time >> 6;
	return (ret==0) ? 1 : ret;
}


int16 setup_incremental_variable( int16 current, int16 min, int16 max ) {
	if ( in_switches[SWITCH_INCREMENT].state ) {
		int16 inc = pulse_time_increment( in_switches[SWITCH_INCREMENT].state_time );
		current = ((max-current)>inc) ? current+inc : max;
		in_switches[SWITCH_INCREMENT].state_time = 0;
	}
	
	if ( in_switches[SWITCH_DECREMENT].state ) {
		int16 inc = pulse_time_increment( in_switches[SWITCH_DECREMENT].state_time );
		current = ((current-min)>inc) ? current-inc : min;
		in_switches[SWITCH_DECREMENT].state_time = 0;
	}
	
	return current;
}


int16 get_peak_coil_ref() {
	int16 ret = 0;
	for (int8 i = 0; i < 8; i++)
		ret += pi_read_peak_coil_ref();
	ret >>= 3;
	
	dsp_setup_coil_pulse_ref(ret);
	
	delay_ms(1500);
	
	return ret;
}


int1 setup_coil_pulse()
{
	// Calculates reference for 5 volts with the mean of 8 samples 
	int8 reference_5v;
	
	reference_5v = get_peak_coil_ref();
	
	while (TRUE) {
		if ( in_switches[SWITCH_MODE].state )
			return in_wait_for_release_timeout( SWITCH_MODE, 2000 );
		
		if ( in_switches[SWITCH_AUTOSET].state ) {
			delay_ms(100);
			reference_5v = get_peak_coil_ref();
		}
		
		pi.pulse_time = setup_incremental_variable( pi.pulse_time,
											MIN_PULSE_TIME, MAX_PULSE_TIME );

		int16 coil_volts = 0;
		for (int8 i = 0; i < 4; i++) {
			coil_volts += pi_read_peak_coil(reference_5v);
			delay_ms(100);
		}
		coil_volts >>= 2;
		
		dsp_setup_coil_pulse(coil_volts);
	}
}


void autoset_sample_delay() {
	pi.sample_zero_point = INITIAL_SAMPLE_ZERO_POINT;
	pi.start_sample_delay = MIN_SAMPLE_DELAY;
	while( pi.start_sample_delay < MAX_SAMPLE_DELAY ) {
		signed int8 strength = pi_sample();
		dsp_setup_sample_delay( strength );
		if( strength <= 0 )
			return;
		++pi.start_sample_delay;
		delay_ms( 100 );
	}
}


int1 setup_start_sample_delay()
{
	while (TRUE) {
		if ( in_switches[SWITCH_MODE].state )
			return in_wait_for_release_timeout( SWITCH_MODE, 2000 );
		
		if ( in_switches[SWITCH_AUTOSET].state )
			autoset_sample_delay();
		
		pi.start_sample_delay = setup_incremental_variable( pi.start_sample_delay, 
										MIN_SAMPLE_DELAY, MAX_SAMPLE_DELAY );

		signed int8 strength = pi_sample();
		dsp_setup_sample_delay( strength );
	}
}


void setup()
{
	if ( setup_coil_pulse() ) {
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
