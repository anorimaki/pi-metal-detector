#include "main.h"
#include "config.h"
#include "coil.h"

#define INITIAL_SAMPLE_ZERO_POINT			400
#define PULSE_TIME_ADDR						0
#define SAMPLE_DELAY_ADDR					2

#rom getenv("EEPROM_ADDRESS") = {    \
        0x00,0x82,      /*Initial pulse time = 130us*/ \
        18}             /*Sample deplay*/


void cnf_load() {
    coil.pulse_length = read_eeprom( PULSE_TIME_ADDR );
	coil.pulse_length <<= 8;
    coil.pulse_length += read_eeprom( PULSE_TIME_ADDR+1 );
    
    coil.sample_delay = read_eeprom( SAMPLE_DELAY_ADDR );
    
	coil.zero = INITIAL_SAMPLE_ZERO_POINT;
	
	coil.auto_zero_threshold = 0;
}


void cnf_save_coil_pulse() {
    write_eeprom( PULSE_TIME_ADDR, coil.pulse_length >> 8 );
    write_eeprom( PULSE_TIME_ADDR+1, coil.pulse_length );
}


void cnf_save_start_sample_delay() {
    write_eeprom( SAMPLE_DELAY_ADDR, coil.sample_delay );
}

