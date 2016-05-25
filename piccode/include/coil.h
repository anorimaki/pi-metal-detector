#ifndef COIL_H
#define COIL_H

struct Coil {
    int16 pulse_length;             //In us
    int8 sample_delay;              //In us after coil pulse ends
    int16 zero;                     //In ADC units
};

extern struct Coil coil;

void coil_init();

//Returns a [-100..100] value indicating signal strength
signed int8 coil_sample();

int16 pi_raw_sample( int8 delay_us );

int16 pi_raw_sample();

//Return value in volts
int16 pi_read_peak_coil( int16 ref );   

//Return 5V reference to calculate coil voltage
int16 pi_read_peak_coil_ref();  


#endif