#include "main.h"
#include "pimetaldec.h"
#include "display.h"

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
        InOption in = in_read();
        if ( in.read == InOpMode )
            return;
        
        pi_coil_pulse();
        
                    //Main sample to read metal objects
        delay_us( config.start_sample_delay );
        sample1 = pi_sample();
        
                    //Second sample to cancel earth effect
        delay_us( config.start_second_sample_delay );
        sample2 = pi_sample();
        
        dsp_sample( sample1, sample2 );
    }
}


/*
 * Sampling state: Normal operation state
 */
void setup_coil_pulse() {
    int8 sample1;
    int8 sample2;
    
    while( TRUE ) {
        InOption in = in_read();
        if ( in.read == InOpMode )
            return;
        
        pi_coil_pulse();
        
                    //Main sample to read metal objects
        delay_us( config.start_sample_delay );
        sample1 = pi_sample();
        
                    //Second sample to cancel earth effect
        delay_us( config.start_second_sample_delay );
        sample2 = pi_sample();
        
        dsp_sample( sample1, sample2 );
    }
}


void setup() {
    if ( !setup_coil_pulse() ) {
        cnf_save_coil_pulse();
        return;
    }       

    if ( !setup_start_sample_delay() ) {
        cnf_save_start_sample_delay();
        return;
    }

    if ( !setup_sample_time() ) {
        cnf_save_sample_time();
        return;
    }        

    if ( !setup_start_second_sample_delay() ) {
        cnf_start_second_sample_delay();
        return;
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
