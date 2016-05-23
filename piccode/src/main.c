#include "main.h"
#include "pimetaldec.h"
#include "display.h"
#include "input.h"
#include "config.h"
#include "setup.h"

//#use rs232( UART1, baud=9600, parity=N, bits=8 )



void init() {
    setup_adc_ports(NO_ANALOGS);
    setup_adc(ADC_OFF);

    set_tris_a( 0xFFFF );      //All inputs by default
    set_tris_b( 0xFFFF );      //All inputs by default
    set_tris_c( 0xFFFF );      //All inputs by default
    
    dsp_init();
    pi_init();
    in_init();
    
    enable_interrupts(GLOBAL);
}



void autoset_sample_zero_point() {
	
	
}


//
// Sampling state: Normal operation state
//
void sampling() {
	while (TRUE) {
		if ( in_switches[SWITCH_MODE].state )
			return;
		
		if ( in_switches[SWITCH_AUTOSET].state ) {
			pi.sample_zero_point = pi_raw_sample();
		}
		
		pi.start_sample_delay = setup_incremental_variable( pi.start_sample_delay, 
										MIN_SAMPLE_DELAY, MAX_SAMPLE_DELAY );

		signed int8 strength = pi_sample();
		
		dsp_sample( strength );
		
		delay_ms( 100 );
	}
}





void main() {
    init();
    
    dsp_hello();
    cnf_load();

    while( TRUE ) {
        sampling();
        setup();
    }
}

#if 1
#include "config.c"
#include "display.c"
#include "input.c"
#include "pimetaldec.c"
#include "setup.c"
#endif