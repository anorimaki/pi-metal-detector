#include "main.h"
#include "usermodes.h"
#include "tone.h"
#include "coil.h"
#include "display.h"
#include "samples.h"
#include "inbuttons.h"
#include "inencoder.h"
#include "modesetup.h"


ModeFuction mode_current;

void mode_init()
{
	mode_current = mode_main;
}

int1 mode_changed()
{
	ModeFuction mode_next = mode_current;

	if (buttons_is_pressed(BUTTON_MAIN))
		mode_next = mode_main;
	else if (buttons_is_pressed(BUTTON_SETUP_DELAY))
		mode_next = mode_setup_delay;
	else if (buttons_is_pressed(BUTTON_SETUP_ZERO))
		mode_next = mode_setup_autozero_threshold;
	else if (buttons_is_pressed(BUTTON_SETUP_PULSE))
		mode_next = mode_setup_pulse;

	int1 ret = (mode_next != mode_current);

	mode_current = mode_next;

	return ret;
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
	int16 sample = 0;

	encoder_set_increment( 0, COIL_MAX_ADC_VALUE, INCREMENT_AUTO_RATE );
	
	tone_begin();

	while (TRUE) {
		if (mode_changed()) {
			tone_end();
			return;
		}

		if (buttons_is_pressed(BUTTON_AUTO)) {
			int16 min_zero =
					coil_custom_sample(COIL_CALCULATE_MIN_ZERO_DELAY, 3);
			//Set it a little greater than sample
			coil.zero = sample + coil.auto_zero_threshold;
			dsp_show_zero(min_zero);
			delay_ms(1000);
		}
		
//		coil.zero += in_increment( coil.zero );
		
		signed int16 increment = encoder_increment( coil.zero );
		lcd_putc('\f');
		printf(lcd_putc, "INC: %Ld\n", increment );

		sample = coil_sample();
//		dsp_sample(sample);
		tone_apply(sample);

		delay_ms( COIL_PULSE_PERIOD );
	}
}

