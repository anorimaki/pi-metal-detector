#include "main.h"
#include "modesetdelay.h"
#include "display.h"
#include "inbuttons.h"
#include "inencoder.h"
#include "coil.h"
#include "samples.h"
#include "mathutil.h"

int16 autoset_delay_signals[DSP_AUTOSET_DELAY_MAX_LINES];

int8 mode_autoset_sample_delay() 
{
	encoder_set_increment( COIL_MIN_SAMPLE_DELAY, COIL_MAX_SAMPLE_DELAY, 1 );
	
	int8 selected = coil.sample_delay;
	int8 first = CHECKED_SUB( selected, 1, COIL_MIN_SAMPLE_DELAY );
	int8 index_to_calculate = 0;
	int8 samples_to_next = 0;
	
	while (TRUE) {
		int8 mode_button = mode_check_buttons();
		if ( mode_button == BUTTON_SETUP_DELAY ) {
			return coil.sample_delay;		//No change
		}
		else if ( mode_button != NO_MODE_BUTTON ) {
			return 0xFF;					//No change and exit
		}
		
		if ( buttons_is_pressed(BUTTON_AUTO) ) {
			return selected;			//Return current selection
		}
		
		selected += encoder_increment( selected );
		if ( first > selected ) {
			first = selected;			//Scroll down LCD
			index_to_calculate = 0;
			samples_to_next = 0;
		}
		else if ( selected >= first+DSP_AUTOSET_DELAY_MAX_LINES ) {
										//Scroll up LCD
			first = selected - (DSP_AUTOSET_DELAY_MAX_LINES-1);
			index_to_calculate = 0;		
			samples_to_next = 0;
		}
		else if ( ++samples_to_next == SAMPLES_HISTORY_SIZE+1 ) {
			samples_to_next = 0;
			if ( ++index_to_calculate == DSP_AUTOSET_DELAY_MAX_LINES ) {
				index_to_calculate = 0;
			}
		}
		
		autoset_delay_signals[index_to_calculate] = 
							coil_custom_sample( first+index_to_calculate );
		
		dsp_autoset_sample_delay( first, selected-first, 
								 autoset_delay_signals );
	}
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
			int8 autoset_value = mode_autoset_sample_delay();
			if ( autoset_value == 0xFF ) {
				return;
			}
			coil.sample_delay = autoset_value;
		}
		
		coil.sample_delay += encoder_increment( coil.sample_delay );
		
		int16 sample = coil_sample();
		if ( ++update_display | 0x07 ) {
				//Only update user interface every 8 loops
			dsp_setup_sample_delay( sample, samples_efficiency(), show_mode );
		}
		
		delay_ms( COIL_PULSE_PERIOD );
	}
}
