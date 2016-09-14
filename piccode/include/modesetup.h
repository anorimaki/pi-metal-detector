#ifndef SETUP_STATES_H
#define	SETUP_STATES_H

#include "usermodes.h"
#include "coil.h"

#define MODE_SETUP_AUTOZERO_THRESHOLD_MAX   COIL_MAX_ADC_VALUE/5
void mode_setup_autozero_threshold();

void mode_setup_pulse();

void mode_setup_response_time();

#endif

