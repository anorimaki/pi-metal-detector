#ifndef SETUP_STATES_H
#define	SETUP_STATES_H

#include "usermodes.h"
#include "coil.h"

#define MODE_SETUP_AUTOZERO_THRESHOLD_MAX   COIL_MAX_ADC_VALUE/5
void mode_setup_autozero_threshold();

#define MODE_SETUP_PULSE_TIME_MAX	500		//In us
#define MODE_SETUP_PULSE_TIME_MIN	10		//in us
void mode_setup_pulse();

#endif

