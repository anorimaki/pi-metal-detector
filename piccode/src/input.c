#include "main.h"
#include "input.h"


void in_init() {
    output_float( PI_BUTTON_MODE_PIN );
    output_float( PI_BUTTON_AUTO_PIN );
    output_float( PI_BUTTON_INC_PIN );
    output_float( PI_BUTTON_DEC_PIN );
    
    RBPU = 1;                              //Enable pull-up resistors on PORT B
    WPUB = PI_BUTTON_PULLUP_RESISTORS;     //Enable pull-ups for buttons
}

#INT_TIMER0
void isr_timer0 {
    int i;
    
    for( i=0; i<SWITCHES_SIZE; ++i ) {
        Switch* sw = in.switches[i];
        
        current_state = input_state( sw->pin );
        if ( current_state ) {
            sw->count_low++;
            sw->count_high = 0;
        }
        else {
            
        }
    }
}

Option in_read() {
    if ( in.switches[SWITCH_MODE].state ) {
        
    }
        
}