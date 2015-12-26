#ifndef PICMETALDEC_H
#define PICMETALDEC_H

struct PiDetector{
    int32 sampleDeplay;
};

extern struct PiDetector pi_data;

void pi_init();
void pi_doPulse();

#endif