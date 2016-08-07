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
	coil_read_decay_begin();
	
	int1 stable_result = 0;
	int16 sample = COIL_MAX_ADC_VALUE;
	coil.zero = 0;
	while( coil.zero < sample ) {
		stable_result = coil_fetch_result();
		sample = coil.result.value;
		tone_apply(sample);
		coil.zero += 2;
		delay_ms( 3 );
	}
	while( !stable_result ) {
		stable_result = coil_fetch_result();
	}
	
	coil.auto_zero_threshold = coil.result.noise;
	coil.zero = coil.result.value + coil.auto_zero_threshold;
	
	coil_end();
	tone_end();
}



void main()
{
    init();
    
    dsp_hello();
	delay_ms(500);
	cnf_load();
//	init_calibration();
	
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
