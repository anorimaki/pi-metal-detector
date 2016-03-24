#ifndef PICMETALDEC_H
#define PICMETALDEC_H

#define PULSE_WIDTH_US          100
#define INTEGRATION_START_US    80
#define INTEGRATION_TIME_US     80

struct PiDetector{
    int32 sampleDeplay;
};

extern struct PiDetector pi_data;

void pi_init();
void pi_doPulse();
void pi_integrate();
int16 pi_sample();

#endif