#include "main.h"
#include "modesetdelay.h"
#include "display.h"
#include "inbuttons.h"
#include "inencoder.h"
#include "coil.h"
#include "mathutil.h"

int16 autoset_delay_signals[DSP_AUTOSET_DELAY_MAX_LINES];

int8 mode_autoset_sample_delay() 
{
	dsp_autoset_sample_delay_init();
	
	encoder_set_increment( COIL_MIN_SAMPLE_DELAY, COIL_MAX_SAMPLE_DELAY, 1 );
	
	int8 selected = coil_get_read_delay();
	int8 first = CHECKED_SUB( selected, 1, COIL_MIN_SAMPLE_DELAY );
	int8 index_to_calculate = 0;
	
	while (TRUE) {
		int8 mode_button = mode_check_buttons();
		if ( mode_button == BUTTON_SETUP_DELAY ) {
			return coil_get_read_delay();		//No change
		}
		else if ( mode_button != NO_MODE_BUTTON ) {
			return 0xFF;					//No change and exit
		}
		
		if ( buttons_is_pressed(BUTTON_AUTO) ) {
			return selected;			//Return current selection
		}
		
		selected = encoder_increment( selected );
		if ( first > selected ) {
			first = selected;			//Scroll down LCD
			index_to_calculate = 0;
		}
		else if ( selected >= first+DSP_AUTOSET_DELAY_MAX_LINES ) {
										//Scroll up LCD
			first = selected - (DSP_AUTOSET_DELAY_MAX_LINES-1);
			index_to_calculate = 0;	
		}
		
		coil_set_working_read_delay( first+index_to_calculate );
		
		int1 confident_result = coil_fetch_result();
		
		autoset_delay_signals[index_to_calculate] = coil.result.value;
		
		if ( confident_result ) {
			if ( ++index_to_calculate == DSP_AUTOSET_DELAY_MAX_LINES ) {
				index_to_calculate = 0;
			}
		}
		
		dsp_autoset_sample_delay( first, selected-first, 
								 autoset_delay_signals );
	}
}


#define SETUP_DELAY_LOOP_PAUSE						5		//In ms

#define SETUP_DELAY_UPDATE_DISPLAY_PERIOD			100		//In ms
#define SETUP_DELAY_UPDATE_DISPLAY_COUNTER \
		(SETUP_DELAY_UPDATE_DISPLAY_PERIOD/SETUP_DELAY_LOOP_PAUSE)

void mode_setup_delay()
{
	static int1 show_mode = DSP_SHOW_PERCENT;
	
	dsp_setup_sample_delay_init();
	
	int8 update_display_counter = SETUP_DELAY_UPDATE_DISPLAY_COUNTER;
	
	encoder_set_increment( COIL_MIN_SAMPLE_DELAY, COIL_MAX_SAMPLE_DELAY,
						INCREMENT_AUTO_RATE );
	
	coil_read_decay_begin();
			
	while (TRUE) {
		int8 mode_button = mode_check_buttons();
		if ( mode_button == BUTTON_SETUP_DELAY ) {
			show_mode++;
		}
		else if ( mode_button != NO_MODE_BUTTON ) {
			coil_end();
			return;
		}
		
		if ( buttons_is_pressed(BUTTON_AUTO) ) {
			int8 autoset_value = mode_autoset_sample_delay();
			if ( autoset_value == 0xFF ) {
				coil_end();
				return;
			}
			coil_set_read_delay( autoset_value );
		}
		
		coil_set_read_delay( encoder_increment( coil_get_read_delay() ) );
		
		if ( --update_display_counter == 0 ) {
			coil_fetch_result();
			dsp_setup_sample_delay( coil.result.value, coil.result.noise, 
									show_mode );
			update_display_counter = SETUP_DELAY_UPDATE_DISPLAY_COUNTER;
		}
		
		delay_ms( SETUP_DELAY_LOOP_PAUSE );
	}
}
