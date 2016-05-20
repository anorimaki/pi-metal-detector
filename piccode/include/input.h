/* 
 * File:   newfile.h
 * Author: David
 *
 * Created on 9 de mayo de 2016, 6:51
 */

#ifndef INPUT_H
#define	INPUT_H


struct InSwitch {
    int1 state;                 // ON/OFF state
    int16 state_time;           // ms that switch has been in this state
	int8 change_confident_count;
    int16 pin;
};


#define SWITCHES_SIZE 		4

#define SWITCH_MODE 		0
#define SWITCH_AUTOSET 		1
#define SWITCH_INCREMENT 	2
#define SWITCH_DECREMENT 	3


extern struct InSwitch in_switches[SWITCHES_SIZE];


void in_init();
int1 in_wait_for_release_timeout( int8 sw, int16 max_push_time );
void in_wait_for_release( int8 sw );


#endif	/* INPUT_H */

