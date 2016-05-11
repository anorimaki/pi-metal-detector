#include "main.h"
#include "setup.h"
#include "input.h"


#define MAX_PULSE_TIME	500		//In us
#define MIN_PULSE_TIME	10		//in us


bool setup_coil_pulse() {
    int16 coil_volts;
	
	dsp_setup_coil_pulse( 0 );
    
    while( TRUE ) {
        if ( in_switches[SWITCH_MODE].state ) 
			return in_is_long_pulse( SWITCH_MODE );
		
		if ( in_switches[SWITCH_INCREMENT].state && 
					(config.pulse_time < MAX_PULSE_TIME) )
			++config.pulse_time;
		
		if ( in_switches[SWITCH_DECREMENT].state && 
					(config.pulse_time > MIN_PULSE_TIME) )
			--config.pulse_time;
        
			//Disable interrupts for good timing confidence.
		disable_interrupts(GLOBAL);
				
		pi_coil_pulse();
		coil_volts = pi_peak_coil_sample();
        
			//Enable interrupts to capture read input switches
		disable_interrupts(GLOBAL);
		
		dsp_setup_coil_pulse( coil_volts );
		
		delay_ms( 50 );
    }
}


bool setup_start_sample_delay() {
	return true;
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
