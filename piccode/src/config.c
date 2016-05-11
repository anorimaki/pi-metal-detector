#include "main.h"
#include "config.h"
#include "pimetaldec.h"

struct Config config;

#define PULSE_TIME_ADDR                    0
#define START_SAMPLE_DELAY_ADDR            2
#define SAMPLE_TIME_ADDR                   START_SAMPLE_DELAY_ADDR + 1
#define START_SECOND_SAMPLE_DELAY_ADDR     SAMPLE_TIME_ADDR + 1

#rom getenv("EEPROM_ADDRESS") = {    \
        0x01,0x5E,      /*Initial pulse time = 350us*/ \
        15,             /*Sample delay*/ \
        80,             /*Sample time*/ \
        80}             /*Second sample deplay*/


void cnf_load() {
    config.pulse_time = read_eeprom( PULSE_TIME_ADDR );
    config.pulse_time <<= 8;
    config.pulse_time = read_eeprom( PULSE_TIME_ADDR );
    
    config.start_sample_delay = read_eeprom( START_SAMPLE_DELAY_ADDR );
    
    config.sample_time = read_eeprom( SAMPLE_TIME_ADDR );
    
    config.start_second_sample_delay = 
                    read_eeprom( START_SECOND_SAMPLE_DELAY_ADDR );
}


void cnf_save_coil_pulse() {
    write_eeprom( PULSE_TIME_ADDR, config.pulse_time >> 8 );
    write_eeprom( PULSE_TIME_ADDR, config.pulse_time );
}


void cnf_save_start_sample_delay() {
    write_eeprom( START_SAMPLE_DELAY_ADDR, config.start_sample_delay );
}


void cnf_save_sample_time() {
    
}


void cnf_save_start_second_sample_delay() {
    
}
