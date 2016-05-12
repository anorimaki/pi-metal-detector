#ifndef PICMETALDEC_H
#define PICMETALDEC_H

struct Config {
    int16 pulse_time;                   //In us
    int8 start_sample_delay;            //In us after coil pulse ends
    int8 sample_time;                   //In us
    int8 start_second_sample_delay;     //In us
};

extern struct Config pi;

void pi_init();
void pi_coil_pulse();
int8 pi_sample();             //Returns a 0-100 vale indicating signal strength
int16 pi_read_peak_coil_volts();  //Return value in volts

#endif