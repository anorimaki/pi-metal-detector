/* 
 * File:   setup.h
 * Author: David
 *
 * Created on 10 de mayo de 2016, 7:00
 */

#ifndef SETUP_STATES_H
#define	SETUP_STATES_H

#define MAX_SAMPLE_DELAY	50		//In us
#define MIN_SAMPLE_DELAY	0		//in us

void setup();

int16 setup_incremental_variable( int16 current, int16 min, int16 max );

#endif	/* CONFIG_H */

