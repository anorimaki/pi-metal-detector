#ifndef PICMETALDEC_H
#define PICMETALDEC_H

struct Config {
    int16 pulse_time;                   //In us
    int8 start_sample_delay;            //In us after coil pulse ends
    int8 sample_time;                   //In us
    int8 start_second_sample_delay;     //In us
    int16 sample_zero_point;            //In ADC units
};

extern struct Config pi;

void pi_init();
void pi_coil_pulse();

//Returns a [-100..100] value indicating signal strength
signed int8 pi_sample();

int16 pi_raw_sample();

//Return value in volts
int16 pi_read_peak_coil( int16 ref );   

//Return 5V reference to calculate coil voltage
int16 pi_read_peak_coil_ref();  


#endif