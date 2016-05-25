#include "main.h"
#include "coil.h"
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
	int16 sample = 0;
	while (TRUE) {
		if ( in_switches[SWITCH_MODE].state )
			return;
		
		if ( in_switches[SWITCH_AUTOSET].state ) {
			int16 min_zero = coil_min_zero();
			coil.zero += sample;
			dsp_setup_zero_point( min_zero );
			delay_ms(1000);
		}
	
		sample = coil_sample();
		
		dsp_sample( sample );
		
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