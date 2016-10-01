#include "main.h"
#include "config.h"
#include "coil.h"

#define INITIAL_SAMPLE_ZERO_POINT			400
#define PULSE_TIME_ADDR						0
#define SAMPLE_DELAY_ADDR					2

#rom getenv("EEPROM_ADDRESS") = {    \
        0x00,0x81,      /*Initial pulse time = 8CH -> 140us*/ \
        23}             /*Sample deplay*/


void cnf_load() {
    coil.pulse_length = read_eeprom( PULSE_TIME_ADDR );
	coil.pulse_length <<= 8;
    coil.pulse_length += read_eeprom( PULSE_TIME_ADDR+1 );
    
    coil.sample_delay = read_eeprom( SAMPLE_DELAY_ADDR );
    
	coil.zero = INITIAL_SAMPLE_ZERO_POINT;
	
	coil.auto_zero_threshold = 10;
	
	coil.samples_history_size_log = 6;
	
	coil.pulse_period = 5000/64;			    //In 64us steps
}


void cnf_save_coil_pulse() {
    write_eeprom( PULSE_TIME_ADDR, coil.pulse_length >> 8 );
    write_eeprom( PULSE_TIME_ADDR+1, coil.pulse_length );
}


void cnf_save_start_sample_delay() {
    write_eeprom( SAMPLE_DELAY_ADDR, coil.sample_delay );
}

