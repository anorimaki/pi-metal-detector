#include "main.h"
#include "config.h"
#include "coil.h"



#define PULSE_TIME_ADDR                    0
#define START_SAMPLE_DELAY_ADDR            2

#rom getenv("EEPROM_ADDRESS") = {    \
        0x00,0x50,      /*Initial pulse time = 80us*/ \
        15}             /*Second sample deplay*/


void cnf_load() {
    coil.pulse_length = read_eeprom( PULSE_TIME_ADDR );
	coil.pulse_length <<= 8;
    coil.pulse_length += read_eeprom( PULSE_TIME_ADDR+1 );
    
    coil.sample_delay = read_eeprom( START_SAMPLE_DELAY_ADDR );
    
	coil.zero = INITIAL_SAMPLE_ZERO_POINT;
}


void cnf_save_coil_pulse() {
    write_eeprom( PULSE_TIME_ADDR, coil.pulse_length >> 8 );
    write_eeprom( PULSE_TIME_ADDR+1, coil.pulse_length );
}


void cnf_save_start_sample_delay() {
    write_eeprom( START_SAMPLE_DELAY_ADDR, coil.sample_delay );
}

