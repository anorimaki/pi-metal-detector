#include "main.h"
#include "usermodes.h"
#include "display.h"
#include "coil.h"
#include "input.h"
#include "samples.h"


#define MAX_PULSE_TIME	500		//In us
#define MIN_PULSE_TIME	10		//in us


void mode_setup_pulse()
{
	// Calculates reference for 5 volts with the mean of 8 samples 
	int16 reference_5v = coil_peak_ref();
	
	in_init_increment( MIN_PULSE_TIME, MAX_PULSE_TIME, INCREMENT_AUTO_RATE );
	
	while (TRUE) {
		if ( mode_changed() ) {
			return;
		}
		
		if ( in_button_pressed(SWITCH_AUTO) ) {
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


void mode_setup_autozero_threshold() 
{
	in_init_increment( 0, COIL_MAX_ADC_VALUE/2, INCREMENT_AUTO_RATE );
	
	int16 noise = 0;
	while (TRUE) {
		if ( mode_changed() ) {
			return;
		}
		
		if ( in_button_pressed(SWITCH_AUTO) ) {
			coil.auto_zero_threshold = noise;
		}

		coil.auto_zero_threshold += in_increment( coil.auto_zero_threshold  );
		
		coil_sample();
		noise = samples_upper_deviation();
		dsp_setup_autozero_threshold( noise );
		
		delay_ms( COIL_PULSE_PERIOD );
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


void mode_setup_delay()
{
	in_init_increment( COIL_MIN_SAMPLE_DELAY, COIL_MAX_SAMPLE_DELAY,
						INCREMENT_AUTO_RATE );
			
	while (TRUE) {
		if ( mode_changed() ) {
			return;
		}
		
		if ( in_button_pressed(SWITCH_AUTO) ) {
			coil.sample_delay = autoset_sample_delay();
		}
		
		coil.sample_delay += in_increment( coil.sample_delay );
		
		int16 sample = coil_sample();
		dsp_setup_sample_delay( sample );
		
		delay_ms( COIL_PULSE_PERIOD );
	}
}
