/* 
 * File:   newfile.h
 * Author: David
 *
 * Created on 9 de mayo de 2016, 6:51
 */

#ifndef INPUT_H
#define	INPUT_H


struct Switch {
    bool state;
	int8 change_confident_count;
};


#define SWITCHES_SIZE 		4

#define SWITCH_MODE 		0
#define SWITCH_INCREMENT 	1
#define SWITCH_DECREMENT 	2
#define SWITCH_AUTOSET 		3

extern struct Switch in_switches[SWITCHES_SIZE];


void in_init();
bool in_is_long_pulse( int8 sw );
void in_wait_for_release( int8 sw );


#endif	/* INPUT_H */

