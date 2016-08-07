#include "main.h"
#include "usermodes.h"
#include "tone.h"
#include "coil.h"
#include "display.h"
#include "samples.h"
#include "inbuttons.h"
#include "inencoder.h"
#include "modesetup.h"
#include "modesetdelay.h"
#include "battery.h"


ModeFuction mode_current;

void mode_init()
{
	mode_current = mode_main;
}

int8 mode_check_buttons()
{
	if (buttons_is_pressed(BUTTON_MAIN)) {
		mode_current = mode_main;
		return BUTTON_MAIN;
	}
	if (buttons_is_pressed(BUTTON_SETUP_DELAY)) {
		mode_current = mode_setup_delay;
		return BUTTON_SETUP_DELAY;
	}
	if (buttons_is_pressed(BUTTON_SETUP_AUTOZERO)) {
		mode_current = mode_setup_autozero_threshold;
		return BUTTON_SETUP_AUTOZERO;
	}
	if (buttons_is_pressed(BUTTON_SETUP_PULSE)) {
		mode_current = mode_setup_pulse;
		return BUTTON_SETUP_PULSE;
	}
	return NO_MODE_BUTTON;
}


int1 mode_changed() 
{
	ModeFuction previous_mode = mode_current;

	mode_check_buttons();
	
	return previous_mode != mode_current;
}


void mode_execute_current()
{
	(*mode_current)();
}


//
// Sampling state: Normal operation state
//
void mode_main()
{
	static int1 show_mode = DSP_SHOW_PERCENT;
	
	int8 update_display_period = 0;
	int16 sample = 0;
	int16 battery_volts = battery_read_volts();
	int16 read_battery_period = 0;
	encoder_set_increment( 0, COIL_MAX_ADC_VALUE, INCREMENT_AUTO_RATE );
	
	tone_begin();

	while (TRUE) {
		int8 mode_button = mode_check_buttons();
		if ( mode_button == BUTTON_MAIN ) {
			show_mode++;
		}
		else if ( mode_button != NO_MODE_BUTTON ) {
			tone_end();
			return;
		}
		
		if (buttons_is_pressed(BUTTON_AUTO)) {
			coil.zero = sample + coil.auto_zero_threshold;
		}
		
		coil.zero += encoder_increment( coil.zero );
		
		sample = coil_sample();
		
		if ( ++update_display_period > 8 ) {
				//Only update user interface every 8 loops
			dsp_main_mode( sample, samples_efficiency(), battery_volts,
							show_mode );
			tone_apply(sample);
			
			update_display_period = 0;
			read_battery_period++;
		}
		
		if ( read_battery_period > 1000 ) {
			battery_volts = battery_read_volts();
			read_battery_period = 0;
		}

		delay_ms( COIL_PULSE_PERIOD );
	}
}

