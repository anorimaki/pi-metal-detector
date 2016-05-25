#include "main.h"
#include "input.h"

#define CHANGE_CONFIDENT_THRESHOLD 2

//At 16Mhz counter increments every 64us:
//	- For 250 counts, overflows every 16ms
#define TIMER_INIT_TIME (256-250)
#define TIMER_PERIOD_MS 16

struct InSwitch in_switches[SWITCHES_SIZE];

void in_init() {
    output_float( PI_BUTTON_MODE_PIN );
    output_float( PI_BUTTON_AUTO_PIN );
    output_float( PI_BUTTON_INC_PIN );
    output_float( PI_BUTTON_DEC_PIN );
    
    RBPU = 0;                              //Enable pull-up resistors on PORT B
    WPUB = PI_BUTTON_PULLUP_RESISTORS;     //Enable pull-ups for buttons
    
    in_switches[SWITCH_MODE].pin = PI_BUTTON_MODE_PIN;
    in_switches[SWITCH_AUTOSET].pin = PI_BUTTON_AUTO_PIN;
    in_switches[SWITCH_INCREMENT].pin = PI_BUTTON_INC_PIN;
    in_switches[SWITCH_DECREMENT].pin = PI_BUTTON_DEC_PIN;
    
    
    setup_timer_0( T0_INTERNAL | T0_DIV_256 | T0_8_BIT );
    set_timer0(TIMER_INIT_TIME);
    enable_interrupts(INT_TIMER0);
}


#int_timer0
void isr_timer0() {
	set_timer0(TIMER_INIT_TIME);
	
    int8 i;
    for( i=0; i<SWITCHES_SIZE; ++i ) {
        struct InSwitch* sw = &in_switches[i];
        
        int1 current_state = !input( sw->pin );
		
		if ( current_state == sw->state ) {
			sw->change_confident_count = 0;
			if ( sw->state_time < (0xFFFF - TIMER_PERIOD_MS) )
				sw->state_time += TIMER_PERIOD_MS;
		}
		else {
			if ( ++sw->change_confident_count == CHANGE_CONFIDENT_THRESHOLD ) {
				sw->state = current_state;
				sw->state_time = 0;
			}
		}
    }
}

// Wait until button releases or it has been pressed for more than 
// max_push_time ms. Returns true if max_push_time has been 
// reached.
int1 in_wait_for_release_timeout( int8 sw, int16 max_push_time ) {
    while( in_switches[sw].state ) {
		if ( in_switches[sw].state_time > max_push_time )
			return TRUE;
    }
    return FALSE;
}


void in_wait_for_release( int8 sw ) {
	while( in_switches[sw].state );
}
