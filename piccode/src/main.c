#include "main.h"
#include "pimetaldec.h"
#include "display.h"
#include "input.h"

//#use rs232( UART1, baud=9600, parity=N, bits=8 )



void init() {
    setup_adc_ports(NO_ANALOGS);
    setup_adc(ADC_OFF);

    set_tris_a( 0xFFFF );      //All inputs by default
    set_tris_b( 0xFFFF );      //All inputs by default
    set_tris_c( 0xFFFF );      //All inputs by default
    
    dsp_init();
    pi_init();
    
    enable_interrupts(GLOBAL);
}


/*
 * Sampling state: Normal operation state
 */
void sampling() {
    int8 sample1;
    int8 sample2;
    
    while( TRUE ) {
        if ( in_switches[SWITCH_MODE].state ) {
			in_wait_for_release(SWITCH_MODE);
			return;
		}
        
				//disable interrupts for good timing confidence.
		disable_interrupts(GLOBAL);
		
        pi_coil_pulse();
        
                    //Main sample to read metal objects
        delay_us( config.start_sample_delay );
        sample1 = pi_sample();
        
                    //Second sample to cancel earth effect
        delay_us( config.start_second_sample_delay );
        sample2 = pi_sample();
		
				//Enable interrupts to capture read input switches
		enable_interrupts(GLOBAL);
        
        dsp_sample( sample1, sample2 );
        
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
