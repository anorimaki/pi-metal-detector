#include "main.h"
#include "input.h"
#include "inencoder.h"
#include "inbuttons.h"


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

int8 in_read_inputs()
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

	return (~switches << 2) | encoder;
}



#int_timer0
void isr_timer0()
{
	set_timer0(IN_TIMER_INIT_TIME);

	int8 current_states = in_read_inputs();

	int1 channelA = current_states & 0x01;
	current_states >>= 1;
	int1 channelB = current_states & 0x01;
	current_states >>= 1;
	encoder_update( channelA, channelB );
	
	buttons_update( current_states );
}
