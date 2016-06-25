#include "main.h"
#include "usermodes.h"
#include "display.h"
#include "coil.h"
#include "inbuttons.h"
#include "inencoder.h"
#include "samples.h"


#define MAX_PULSE_TIME	500		//In us
#define MIN_PULSE_TIME	10		//in us


void mode_setup_pulse()
{
	// Calculates reference for 5 volts with the mean of 8 samples 
	int16 reference_5v = coil_peak_ref();
	
	encoder_set_increment( MIN_PULSE_TIME, MAX_PULSE_TIME, INCREMENT_AUTO_RATE );
	
	while (TRUE) {
		if ( mode_changed() ) {
			return;
		}
		
		if ( buttons_is_pressed(BUTTON_AUTO) ) {
			reference_5v = coil_peak_ref();
			dsp_setup_coil_pulse_ref( reference_5v );
			delay_ms(1000);
		}
		
		coil.pulse_length += encoder_increment( coil.pulse_length );

		int16 peak = coil_peak();
		dsp_setup_coil_pulse(peak, reference_5v);
		
		delay_ms( COIL_PULSE_PERIOD );
	}
}


void mode_setup_autozero_threshold() 
{
	encoder_set_increment( 0, COIL_MAX_ADC_VALUE/10, INCREMENT_AUTO_RATE );
	
	int16 noise = 0;
	while (TRUE) {
		if ( mode_changed() ) {
			return;
		}
		
		if ( buttons_is_pressed(BUTTON_AUTO) ) {
			coil.auto_zero_threshold = noise;
		}

		coil.auto_zero_threshold += encoder_increment( coil.auto_zero_threshold  );
		
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
	//	dsp_setup_sample_delay( sample );
		if( sample <= coil.zero )
			return ret;
		++ret;
	}
	return ret;
}


void mode_setup_delay()
{
	static int1 show_mode = DSP_SHOW_PERCENT;
	int8 update_display = 0;
	
	encoder_set_increment( COIL_MIN_SAMPLE_DELAY, COIL_MAX_SAMPLE_DELAY,
						INCREMENT_AUTO_RATE );
			
	while (TRUE) {
		int8 mode_button = mode_check_buttons();
		if ( mode_button == BUTTON_SETUP_DELAY ) {
			show_mode++;
		}
		else if ( mode_button != NO_MODE_BUTTON ) {
			return;
		}
		
		if ( buttons_is_pressed(BUTTON_AUTO) ) {
			coil.sample_delay = autoset_sample_delay();
		}
		
		coil.sample_delay += encoder_increment( coil.sample_delay );
		
		int16 sample = coil_sample();
		if ( ++update_display & 0x07 ) {
				//Only update user interface every 8 loops
			dsp_setup_sample_delay( sample, samples_efficiency(), show_mode );
		}
		
		delay_ms( COIL_PULSE_PERIOD );
	}
}
