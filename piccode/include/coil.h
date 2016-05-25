#ifndef COIL_H
#define COIL_H

#define COIL_MAX_ADC_VALUE              4095
#define COIL_PULSE_PERIOD               50      //In ms
#define COIL_MAX_SAMPLE_DELAY           50		//In us
#define COIL_MIN_SAMPLE_DELAY           0		//in us
#define COIL_CALCULATE_MIN_ZERO_DELAY   COIL_MAX_SAMPLE_DELAY*4		//in us

struct Coil {
    int16 pulse_length;             //In us
    int8 sample_delay;              //In us after coil pulse ends
    int16 zero;                     //In ADC units
};

extern struct Coil coil;

void coil_init();

int8 coil_peak_ref();
int16 coil_peak();

signed int16 coil_sample();
int16 coil_raw_sample( int8 delay, int8 n );

#endif