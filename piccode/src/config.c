#include "main.h"
#include "config.h"
#include "coil.h"



#define PULSE_TIME_ADDR                    0
#define START_SAMPLE_DELAY_ADDR            2

#rom getenv("EEPROM_ADDRESS") = {    \
        0x00,0x50,      /*Initial pulse time = 80us*/ \
        15}             /*Second sample deplay*/


void cnf_load() {
    pi.pulse_time = read_eeprom( PULSE_TIME_ADDR );
    pi.pulse_time <<= 8;
    pi.pulse_time += read_eeprom( PULSE_TIME_ADDR+1 );
    
    pi.start_sample_delay = read_eeprom( START_SAMPLE_DELAY_ADDR );
    
	pi.sample_zero_point = INITIAL_SAMPLE_ZERO_POINT;
}


void cnf_save_coil_pulse() {
    write_eeprom( PULSE_TIME_ADDR, pi.pulse_time >> 8 );
    write_eeprom( PULSE_TIME_ADDR+1, pi.pulse_time );
}


void cnf_save_start_sample_delay() {
    write_eeprom( START_SAMPLE_DELAY_ADDR, pi.start_sample_delay );
}


void cnf_save_sample_time() {
    
}


void cnf_save_start_second_sample_delay() {
    
}
