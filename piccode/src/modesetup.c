#include "main.h"
#include "usermodes.h"
#include "display.h"
#include "coil.h"
#include "inbuttons.h"
#include "inencoder.h"
#include "samples.h"


void mode_setup_pulse()
{
	static int1 show_mode = DSP_SHOW_VOLTS;
	
	encoder_set_increment( MODE_SETUP_PULSE_TIME_MIN, MODE_SETUP_PULSE_TIME_MAX,
						 INCREMENT_AUTO_RATE );
	
	int8 measuresLeft = SAMPLES_HISTORY_SIZE+2;
	int8 measureRef = 0;
	
	int16 reference_5v;
	int16 peak = 0;
	
	while (TRUE) {
		int8 mode_button = mode_check_buttons();
		if ( mode_button == BUTTON_SETUP_PULSE ) {
			++show_mode;		
		}
		else if ( mode_button != NO_MODE_BUTTON ) {
			return;
		}
		
		coil.pulse_length += encoder_increment( coil.pulse_length );
		
		if ( --measuresLeft == 0 ) {
			measuresLeft = SAMPLES_HISTORY_SIZE+2;
			++measureRef;
		}
		
		if ( measureRef & 0x07 ) {	//Measure peak 8 times more than reference
			peak = coil_peak();
		}
		else {
			reference_5v = coil_peak_ref();
		}
		
		dsp_setup_coil_pulse(peak, reference_5v, show_mode);
		
		delay_ms( 3 );
	}
}


void mode_setup_autozero_threshold() 
{
	encoder_set_increment( 0, MODE_SETUP_AUTOZERO_THRESHOLD_MAX,
						 INCREMENT_AUTO_RATE );
	
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
		noise = samples_efficiency();
		dsp_setup_autozero_threshold( noise );
		
		delay_ms( COIL_PULSE_PERIOD );
	}
}

