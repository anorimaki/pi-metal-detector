#ifndef PICMETALDEC_H
#define PICMETALDEC_H

void pi_init();
void pi_coil_pulse();
int8 pi_sample();             //Returns a 0-100 vale indicating signal strength
int16 pi_peak_coil_sample();  //Return value in volts

#endif