#include "main.h"
#include "usermodes.h"
#include "tone.h"
#include "coil.h"
#include "display.h"
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
	if (buttons_is_pressed(BUTTON_SETUP_RESPONSE)) {
		mode_current = mode_setup_response_time;
		return BUTTON_SETUP_RESPONSE;
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

#define MAIN_LOOP_PAUSE						1		//In ms

#define MAIN_READ_BATTERY_PERIOD			30000	//In ms
#define MAIN_READ_BATTERY_COUNTER \
		(MAIN_READ_BATTERY_PERIOD/MAIN_LOOP_PAUSE)

#define MAIN_UPDATE_DISPLAY_PERIOD			100		//In ms
#define MAIN_UPDATE_DISPLAY_COUNTER \
		(MAIN_UPDATE_DISPLAY_PERIOD/MAIN_LOOP_PAUSE)

#define MAIN_NOISE_RESET_COUNTER			10

void mode_main()
{
	static int1 show_mode = DSP_SHOW_PERCENT;
	
	dsp_main_mode_init();
	
	encoder_set_increment( 0, COIL_MAX_ADC_VALUE, INCREMENT_AUTO_RATE );

	int8 update_display_counter = MAIN_UPDATE_DISPLAY_COUNTER;
	int16 read_battery_counter = MAIN_READ_BATTERY_COUNTER;
	
	tone_begin();
	coil_read_decay_begin();
	
	int16 battery_volts = battery_read_volts();
	int16 noise = 0;
	int8 noise_reset_counter = MAIN_NOISE_RESET_COUNTER;

	while (TRUE) {
		int8 mode_button = mode_check_buttons();
		if ( mode_button == BUTTON_MAIN ) {
			show_mode++;
		}
		else if ( mode_button != NO_MODE_BUTTON ) {
			coil_end();
			tone_end();
			return;
		}
		
		if (buttons_is_pressed(BUTTON_AUTO)) {
			coil.zero = coil.result.value + coil.auto_zero_threshold;
		}
		
		coil.zero = encoder_increment( coil.zero );
		
		if ( --update_display_counter == 0 ) {
			coil_fetch_result();
			
			if ( coil.result.noise>noise )	 //Take max noise signal to show
				noise = coil.result.noise;
			
			dsp_main_mode( coil.result.value, noise, 
							battery_volts, show_mode );
			tone_apply( coil.result.value );
			
			if ( --noise_reset_counter == 0 ) {
				noise_reset_counter = MAIN_NOISE_RESET_COUNTER;
				noise = 0;
			}
			
			update_display_counter = MAIN_UPDATE_DISPLAY_COUNTER;
		}
		
		if ( --read_battery_counter == 0 ) {
			coil_sleep();
			battery_volts = battery_read_volts();
			coil_wakeup();
			read_battery_counter = MAIN_READ_BATTERY_COUNTER;
		} 

		delay_ms( MAIN_LOOP_PAUSE );
	}
}

