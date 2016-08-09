#include "main.h"
#include "modesetup.h"
#include "usermodes.h"
#include "display.h"
#include "coil.h"
#include "inbuttons.h"
#include "inencoder.h"


#define SETUP_PULSE_LOOP_PAUSE				5		//In ms

#define SETUP_PULSE_READ_REFERENCE_PERIOD	2000	//In ms
#define SETUP_PULSE_READ_REFERENCE_COUNTER \
		(SETUP_PULSE_READ_REFERENCE_PERIOD/SETUP_PULSE_LOOP_PAUSE)

//Time needed to discharge peak detector capacitor
#define SETUP_PULSE_READ_REFERENCE_WAIT		100		//In ms
#define SETUP_PULSE_READ_REFERENCE_WAIT_COUNTER \
		(SETUP_PULSE_READ_REFERENCE_WAIT/SETUP_PULSE_LOOP_PAUSE)

#define SETUP_PULSE_UPDATE_DISPLAY_PERIOD	100		//In ms
#define SETUP_PULSE_UPDATE_DISPLAY_COUNTER \
		(SETUP_PULSE_UPDATE_DISPLAY_PERIOD/SETUP_PULSE_LOOP_PAUSE)

void mode_setup_pulse()
{
	static int1 show_mode = DSP_SHOW_VOLTS;
	
	encoder_set_increment( MODE_SETUP_PULSE_TIME_MIN, MODE_SETUP_PULSE_TIME_MAX,
						 INCREMENT_AUTO_RATE );
	
	int16 read_reference_counter = SETUP_PULSE_READ_REFERENCE_COUNTER;
	int8 update_display_counter = SETUP_PULSE_UPDATE_DISPLAY_COUNTER;
	
	int16 reference_5v = coil_peak_ref();
	int1 wait_for_read_reference = 0;
	
	coil_read_peak_begin();
	
	while (TRUE) {
		int8 mode_button = mode_check_buttons();
		if ( mode_button == BUTTON_SETUP_PULSE ) {
			++show_mode;		
		}
		else if ( mode_button != NO_MODE_BUTTON ) {
			coil_end();
			return;
		}
		
		coil_set_pulse_length( encoder_increment( coil_get_pulse_length() ) );
		
		if ( wait_for_read_reference ) {
			if ( --read_reference_counter == 0 ) {
				reference_5v = coil_peak_ref();
				coil_wakeup();
				wait_for_read_reference = 0;
				read_reference_counter = SETUP_PULSE_READ_REFERENCE_COUNTER;
			}
		}
		else {
			if ( coil_fetch_result() ) {
				if ( --read_reference_counter == 0) {
					coil_sleep();
					wait_for_read_reference = 1;
					read_reference_counter = SETUP_PULSE_READ_REFERENCE_WAIT_COUNTER;
				}
			}
		}
		
		if ( --update_display_counter == 0 ) {
			dsp_setup_coil_pulse( coil.result.value, reference_5v, show_mode);
			update_display_counter = SETUP_PULSE_UPDATE_DISPLAY_COUNTER;
		}
		
		delay_ms( SETUP_PULSE_LOOP_PAUSE );
	}
}


#define SETUP_AUTOZERO_THRES_LOOP_PAUSE		5		//In ms

#define SETUP_AUTOZERO_THRES_DISPLAY_PERIOD	100		//In ms
#define SETUP_AUTOZERO_THRES_DISPLAY_COUNTER \
		(SETUP_AUTOZERO_THRES_DISPLAY_PERIOD/SETUP_AUTOZERO_THRES_LOOP_PAUSE)


void mode_setup_autozero_threshold() 
{
	encoder_set_increment( 0, MODE_SETUP_AUTOZERO_THRESHOLD_MAX,
						 INCREMENT_AUTO_RATE );
	
	int16 noise = 0;
	int8 update_display_counter = SETUP_AUTOZERO_THRES_DISPLAY_COUNTER;
	
	coil_read_decay_begin();

	while (TRUE) {
		if ( mode_changed() ) {
			coil_end();
			return;
		}
		
		if ( buttons_is_pressed(BUTTON_AUTO) ) {
			coil.auto_zero_threshold = noise;
		}

		coil.auto_zero_threshold = encoder_increment( coil.auto_zero_threshold );
		
		coil_fetch_result();
		
		if ( coil.result.noise>noise )	 //Take max noise signal to show
			noise = coil.result.noise;
		
		if ( --update_display_counter == 0 ) {
				//Only update user interface every 8 loops
			dsp_setup_autozero_threshold( noise );
			noise = 0;
			update_display_counter = SETUP_AUTOZERO_THRES_DISPLAY_COUNTER;
		}
		
		delay_ms( SETUP_AUTOZERO_THRES_LOOP_PAUSE );
	}
}