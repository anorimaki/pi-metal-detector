#include "main.h"
#include "coil.h"
#include "display.h"
#include "input.h"
#include "inbuttons.h"
#include "config.h"
#include "usermodes.h"
#include "tone.h"
#include "samples.h"
#include "adconvert.h"

#use rs232( UART2, baud=9600, parity=N, bits=8 )

void init() {
	set_tris_a( 0xFFFF );      //All inputs by default
    set_tris_b( 0xFFFF );      //All inputs by default
    set_tris_c( 0xFFFF );      //All inputs by default
    
	adc_init();
	coil_init();
	tone_init();
    in_init();
	buttons_init();
	dsp_init();
	mode_init();
    
    enable_interrupts(GLOBAL);
}


void init_calibration() 
{
	tone_begin();
	int16 sample = COIL_MAX_ADC_VALUE;
	coil.zero = 0;
	while( coil.zero < sample ) {
		sample = coil_sample();
		tone_apply(sample);
		coil.zero += 2;
		delay_ms( COIL_PULSE_PERIOD );
	}
	coil.auto_zero_threshold = samples_efficiency();
	coil.zero = sample + coil.auto_zero_threshold;
	tone_end();
}



void main()
{
    init();
    
    dsp_hello();
	delay_ms(500);
	cnf_load();
	init_calibration();
	
	while( TRUE ) {
		mode_execute_current();
	}
}


#if 1
#include "adconvert.c"
#include "config.c"
#include "display.c"
#include "input.c"
#include "inencoder.c"
#include "inbuttons.c"
#include "coil.c"
#include "modesetup.c"
#include "samples.c"
#include "tone.c"
#include "mathutil.c"
#include "modesetdelay.c"
#include "usermodes.c"
#include "battery.c"
#endif
