#include "main.h"
#include "config.h"
#include "coil.h"

#define PULSE_TIME_ADDR						0
#define SAMPLE_DELAY_ADDR					2
#define SAMPLE_ZERO_THRESHOLD_ADDR			3
#define SAMPLES_HISTORY_SIZE_ADDR			5
#define PULSE_PERIOD_ADDR					6

#rom getenv("EEPROM_ADDRESS") = { 
        0x00, 140,      /*Initial pulse time = 140us*/ 
        25,				/*Sample delay*/
		0x00, 10, 		/*Zero thresholdd*/
		6,				/*History size*/
		0, 5000/64 }	/*pulse period in 64us steps*/


void cnf_load() {
	coil.pulse_period = read_eeprom( PULSE_PERIOD_ADDR );
	coil.pulse_period <<= 8;
	coil.pulse_period = read_eeprom( PULSE_PERIOD_ADDR+1 );
	
    coil.pulse_length = read_eeprom( PULSE_TIME_ADDR );
	coil.pulse_length <<= 8;
    coil.pulse_length += read_eeprom( PULSE_TIME_ADDR+1 );
    
    coil.sample_delay = read_eeprom( SAMPLE_DELAY_ADDR );
    
	coil.zero = 0;
	
	coil.auto_zero_threshold = read_eeprom( SAMPLE_ZERO_THRESHOLD_ADDR );
	coil.auto_zero_threshold <<= 8;
	coil.auto_zero_threshold = read_eeprom( SAMPLE_ZERO_THRESHOLD_ADDR+1 );
	
	coil.samples_history_size_log = read_eeprom( SAMPLES_HISTORY_SIZE_ADDR );
}


void cnf_save() {
	cnf_save_pulse_period();
	cnf_save_pulse_length();
	cnf_save_start_sample_delay();
	cnf_save_zero_threshold();
	cnf_save_history_size();
}


void cnf_save_pulse_period() {
    write_eeprom( PULSE_PERIOD_ADDR, coil.pulse_period >> 8 );
	write_eeprom( PULSE_PERIOD_ADDR+1, coil.pulse_period );
}


void cnf_save_pulse_length() {
    write_eeprom( PULSE_TIME_ADDR, coil.pulse_length >> 8 );
    write_eeprom( PULSE_TIME_ADDR+1, coil.pulse_length );
}


void cnf_save_start_sample_delay() {
    write_eeprom( SAMPLE_DELAY_ADDR, coil.sample_delay );
}


void cnf_save_zero_threshold() {
    write_eeprom( SAMPLE_ZERO_THRESHOLD_ADDR, coil.auto_zero_threshold >> 8 );
    write_eeprom( SAMPLE_ZERO_THRESHOLD_ADDR+1, coil.auto_zero_threshold );
}


void cnf_save_history_size() {
    write_eeprom( SAMPLES_HISTORY_SIZE_ADDR, coil.samples_history_size_log );
}

