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


//
// Sampling state: Normal operation state
//
void sampling() {
    while( TRUE ) {
		if ( in_switches[SWITCH_MODE].state )
			return;
		  
        int8 sample = pi_sample();
        
        dsp_sample( sample );
        
        delay_ms( 300 );
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