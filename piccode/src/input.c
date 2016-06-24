#include "main.h"
#include "input.h"
#include "inencoder.h"
#include "inbuttons.h"


#define IN_STATE_BUTTONS_MASK     0x1F		//For 5 buttons


void in_init()
{
	output_low(PI_CHARLIEPLEX_SWITCH_1_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_3_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_4_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_1_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_3_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_4_PIN);

	RBPU = 0; //Enable pull-up resistors on PORT B
	WPUB = PI_BUTTON_PULLUP_RESISTORS; //Enable pull-ups for buttons

	setup_timer_0(T0_INTERNAL | T0_DIV_256 | T0_8_BIT);
	set_timer0(IN_TIMER_INIT_TIME);
	enable_interrupts(INT_TIMER0);
}


int8 in_read_encoder_inputs()
{
	output_drive(PI_CHARLIEPLEX_SWITCH_2_PIN);
	int8 encoder = input_state(PI_CHARLIEPLEX_SWITCH_4_PIN);	//channel A
	encoder <<= 1;
	encoder |= input_state(PI_CHARLIEPLEX_SWITCH_3_PIN);		//channel B
	output_high(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_2_PIN);
	
	return encoder;
}
	

int8 in_read_all_inputs()
{
	int8 switches, encoder;

	output_drive(PI_CHARLIEPLEX_SWITCH_3_PIN);
	switches = input_state(PI_CHARLIEPLEX_SWITCH_1_PIN);
	output_high(PI_CHARLIEPLEX_SWITCH_3_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_3_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_3_PIN);

	output_drive(PI_CHARLIEPLEX_SWITCH_2_PIN);
	encoder = input_state(PI_CHARLIEPLEX_SWITCH_4_PIN);		//channel A
	encoder <<= 1;
	encoder |= input_state(PI_CHARLIEPLEX_SWITCH_3_PIN);	//channel B
	switches <<= 1;
	switches |= input_state(PI_CHARLIEPLEX_SWITCH_1_PIN);
	output_high(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_2_PIN);

	output_drive(PI_CHARLIEPLEX_SWITCH_1_PIN);
	switches <<= 1;
	switches |= input_state(PI_CHARLIEPLEX_SWITCH_4_PIN);
	switches <<= 1;
	switches |= input_state(PI_CHARLIEPLEX_SWITCH_3_PIN);
	switches <<= 1;
	switches |= input_state(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_high(PI_CHARLIEPLEX_SWITCH_1_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_1_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_1_PIN);

	return (encoder << BUTTONS_SIZE) | (~switches & IN_STATE_BUTTONS_MASK);
}



#int_timer0
void isr_timer0()
{
	static int8 counter = 0;
	
	set_timer0(IN_TIMER_INIT_TIME);
	
	int8 current_states;
	if ( (counter++ & 0x07) == 0 ) {
		current_states = in_read_all_inputs();
		buttons_update( current_states );
		current_states >>= BUTTONS_SIZE;
	}
	else {
		current_states = in_read_encoder_inputs();
	}

	int1 channelA = current_states & 0x01;
	current_states >>= 1;
	int1 channelB = current_states & 0x01;
	current_states >>= 1;
	encoder_update( channelA, channelB );
}
