#include "main.h"
#include "input.h"

#define CHANGE_CONFIDENT_THRESHOLD 100

struct InSwitch in_switches[SWITCHES_SIZE];

void in_init() {
    output_float( PI_BUTTON_MODE_PIN );
    output_float( PI_BUTTON_AUTO_PIN );
    output_float( PI_BUTTON_INC_PIN );
    output_float( PI_BUTTON_DEC_PIN );
    
    RBPU = 1;                              //Enable pull-up resistors on PORT B
    WPUB = PI_BUTTON_PULLUP_RESISTORS;     //Enable pull-ups for buttons
    
    in_switches[SWITCH_MODE].pin = PI_BUTTON_MODE_PIN;
    in_switches[SWITCH_AUTOSET].pin = PI_BUTTON_AUTO_PIN;
    in_switches[SWITCH_INCREMENT].pin = PI_BUTTON_INC_PIN;
    in_switches[SWITCH_DECREMENT].pin = PI_BUTTON_DEC_PIN;
    
    //At 16Mhz increments every 64us and overflows every 16.384ms
    setup_timer_0( T0_INTERNAL | T0_DIV_256 | T0_8_BIT );
    set_timer0(0);
    enable_interrupts(INT_TIMER0);
}


#int_timer0
void isr_timer0() {
    int8 i;
	int1 current_state;
    
    for( i=0; i<SWITCHES_SIZE; ++i ) {
        struct InSwitch* sw = in_switches[i];
        
        current_state = input_state( sw->pin );
		
		if ( current_state == sw->state ) {
			sw->change_confident_count = 0;
		}
		else {
			if ( sw->change_confident_count++ == CHANGE_CONFIDENT_THRESHOLD ) {
				sw->state = current_state;
			}
		}
    }
}

/*
 * Wait to switch release and return true
 * if switch has been pressed for more than
 * 2 seconds.
 */
int1 in_is_long_pulse( int8 sw ) {
	int8 counter;
    
    counter = 20;
    while( in_switches[sw].state ) {
        if ( counter > 0 ) 
            --counter;
        delay_ms( 100 );
    }

    return counter == 0;
}


void in_wait_for_release( int8 sw ) {
	while( in_switches[sw].state );
}
