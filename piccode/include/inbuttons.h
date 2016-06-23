#ifndef INSWITCH_H
#define	INSWITCH_H

struct InButton {
    int1 state;                 // ON/OFF state
    int1 changed;
    int16 state_time;           // ms that switch has been in this state
	int8 change_confident_count;
};


#define BUTTONS_SIZE       5

extern struct InButton in_buttons[BUTTONS_SIZE];

#define SWITCH_MAIN 		0
#define SWITCH_SETUP_DELAY	1
#define SWITCH_SETUP_ZERO	2
#define SWITCH_SETUP_PULSE	3
#define SWITCH_AUTO     	4

void buttons_init();

void buttons_update( int8 switches_state );

int1 buttons_wait_for_release_timeout( int8 sw, int16 max_push_time );
void buttons_wait_for_release( int8 sw );

int1 buttons_is_pressed( int8 sw );

#endif

