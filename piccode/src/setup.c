#include "main.h"
#include "setup.h"
#include "display.h"
#include "pimetaldec.h"
#include "config.h"
#include "input.h"


#define MAX_PULSE_TIME	500		//In us
#define MIN_PULSE_TIME	10		//in us


int1 setup_coil_pulse() {
    int16 coil_volts;
    int8 i;
    int16 reference_5v;
    
	       // Calculates reference for 5 volts with the mean of 8 samples 
    reference_5v=0;
    for( i=0; i<8; i++ )
        reference_5v += pi_read_peak_coil_ref();
    reference_5v >>= 3;
    
    dsp_setup_coil_pulse_ref( reference_5v );
    
    while( TRUE ) {
        if ( in_switches[SWITCH_MODE].state ) 
			return in_is_long_pulse( SWITCH_MODE );
		
		if ( in_switches[SWITCH_INCREMENT].state && 
					(pi.pulse_time < MAX_PULSE_TIME) )
			++pi.pulse_time;
		
		if ( in_switches[SWITCH_DECREMENT].state && 
					(pi.pulse_time > MIN_PULSE_TIME) )
			--pi.pulse_time;
        
		coil_volts = pi_read_peak_coil( reference_5v );
		
		dsp_setup_coil_pulse( coil_volts );
		
		delay_ms( 50 );
    }
}


int1 setup_start_sample_delay() {
	return TRUE;
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
/*
    if ( !setup_sample_time() ) {
        cnf_save_sample_time();
        return;
    }        

    if ( !setup_start_second_sample_delay() ) {
        cnf_start_second_sample_delay();
        return;
    }
*/
}
