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
    int1 changed;
    int16 state_time;           // ms that switch has been in this state
	int8 change_confident_count;
};


#define SWITCHES_SIZE       5

#define SWITCH_MAIN 		0
#define SWITCH_SETUP_DELAY	1
#define SWITCH_SETUP_ZERO	2
#define SWITCH_SETUP_PULSE	3
#define SWITCH_AUTO     	4

#define INCREMENT_AUTO_RATE 0xFFFF

extern struct InSwitch in_switches[SWITCHES_SIZE];

void in_init();

int1 in_wait_for_release_timeout( int8 sw, int16 max_push_time );
void in_wait_for_release( int8 sw );

int1 in_button_pressed( int8 sw );

void in_init_increment( int16 min, int16 max, int16 rate );
signed int16 in_increment( int16 current );

#endif	/* INPUT_H */

