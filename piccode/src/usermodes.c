#include "main.h"
#include "usermodes.h"
#include "tone.h"
#include "coil.h"
#include "display.h"
#include "samples.h"
#include "input.h"
#include "modesetup.h"


ModeFuction mode_current;

void mode_init()
{
	mode_current = mode_main;
}

int1 mode_changed()
{
	ModeFuction mode_next = mode_current;

	if (in_button_pressed(SWITCH_MAIN))
		mode_next = mode_main;
	else if (in_button_pressed(SWITCH_SETUP_DELAY))
		mode_next = mode_setup_delay;
	else if (in_button_pressed(SWITCH_SETUP_ZERO))
		mode_next = mode_setup_zero;
	else if (in_button_pressed(SWITCH_SETUP_PULSE))
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

	tone_begin();

	while (TRUE) {
		if (mode_changed()) {
			tone_end();
			return;
		}

		if (in_button_pressed(SWITCH_AUTO)) {
			int16 min_zero =
					coil_custom_sample(COIL_CALCULATE_MIN_ZERO_DELAY, 3);
			//Set it a little greater than sample
			coil.zero = sample + samples_upper_deviation();
			dsp_setup_zero_point(min_zero);
			delay_ms(1000);
		}

		sample = coil_sample();
		dsp_sample(sample);
		tone_apply(sample);

		delay_ms(COIL_PULSE_PERIOD);
	}
}

