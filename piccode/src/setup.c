#include "main.h"
#include "setup.h"
#include "display.h"
#include "coil.h"
#include "input.h"
#include "samples.h"


#define MAX_PULSE_TIME	500		//In us
#define MIN_PULSE_TIME	10		//in us



int1 setup_coil_pulse()
{
	// Calculates reference for 5 volts with the mean of 8 samples 
	int16 reference_5v = coil_peak_ref();
	
	in_init_increment( MIN_PULSE_TIME, MAX_PULSE_TIME, INCREMENT_AUTO_RATE );
	
	while (TRUE) {
		if ( in_button_pressed(SWITCH_MODE) )
			return in_wait_for_release_timeout( SWITCH_MODE, 2000 );
		
		if ( in_button_pressed(SWITCH_AUTOSET) ) {
			reference_5v = coil_peak_ref();
			dsp_setup_coil_pulse_ref( reference_5v );
			delay_ms(1000);
		}
		
		coil.pulse_length += in_increment( coil.pulse_length );

		int16 peak = coil_peak();
		dsp_setup_coil_pulse(peak, reference_5v);
		
		delay_ms( COIL_PULSE_PERIOD );
	}
}


int1 setup_zero_point() 
{
	int16 min_zero = coil_custom_sample( COIL_CALCULATE_MIN_ZERO_DELAY, 3 );
	
	in_init_increment( min_zero, (COIL_MAX_ADC_VALUE*80)/100, 
					 INCREMENT_AUTO_RATE );
	
	while (TRUE) {
		if ( in_button_pressed(SWITCH_MODE) )
			return in_wait_for_release_timeout( SWITCH_MODE, 2000 );
		
		if ( in_button_pressed(SWITCH_AUTOSET) ) {
			//Set to 10% of max value
			int16 max_value = COIL_MAX_ADC_VALUE - min_zero;
			coil.zero = min_zero + (max_value/10);
		}

		coil.zero += in_increment( coil.zero  );

		dsp_setup_zero_point( min_zero );
		
		delay_ms(100);
	}
}


int16 autoset_sample_delay() {
	int16 ret = COIL_MIN_SAMPLE_DELAY;
	while( ret < COIL_MAX_SAMPLE_DELAY ) {
		int16 sample = coil_custom_sample( ret, 4 );
		dsp_setup_sample_delay( sample );
		if( sample <= coil.zero )
			return ret;
		++ret;
	}
	return ret;
}


int1 setup_sample_delay()
{
	in_init_increment( COIL_MIN_SAMPLE_DELAY, COIL_MAX_SAMPLE_DELAY,
						INCREMENT_AUTO_RATE );
			
	while (TRUE) {
		if ( in_button_pressed(SWITCH_MODE) )
			return in_wait_for_release_timeout( SWITCH_MODE, 2000 );
		
		if ( in_button_pressed(SWITCH_AUTOSET) ) {
			coil.sample_delay = autoset_sample_delay();
		}
		
		coil.sample_delay += in_increment( coil.sample_delay );
		
		int16 sample = coil_sample();
		dsp_setup_sample_delay( sample );
		
		delay_ms( COIL_PULSE_PERIOD );
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
	
	if ( setup_sample_delay() ) {
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
