#ifndef PICMETALDEC_H
#define PICMETALDEC_H

struct PiDetector{
    int16 pulse_time;            //In us
    int8 integration_start;     //In us after coil pulse ends
    int8 integration_time;      //In us
};

extern struct PiDetector pi_data;

void pi_init();
void pi_coil_pulse();
void pi_set_start_integration();
int16 pi_sample();

#endif