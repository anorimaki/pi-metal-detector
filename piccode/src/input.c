#include "main.h"
#include "input.h"
#include "inencoder.h"
#include "inbuttons.h"



void in_init()
{
	output_low(PI_CHARLIEPLEX_SWITCH_1_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_3_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_1_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_3_PIN);
	output_float(PI_ENCODER_A_PIN);
	output_float(PI_ENCODER_B_PIN);

	RBPU = 0; //Enable pull-up resistors on PORT B
	WPUB = PI_BUTTON_PULLUP_RESISTORS; //Enable pull-ups for buttons

	setup_timer_0(T0_INTERNAL | T0_DIV_256 | T0_8_BIT);
	set_timer0(IN_TIMER_INIT_TIME);
	enable_interrupts(INT_TIMER0);
}


void in_encoder_read_and_update()
{
	int1 channelA = input_state(PI_ENCODER_A_PIN);
	int1 channelB = input_state(PI_ENCODER_B_PIN);
	encoder_update( channelA, channelB );
}
	

int8 in_read_charlieplex_inputs()
{
	int8 switches;

	output_drive(PI_CHARLIEPLEX_SWITCH_1_PIN);
	switches = input_state(PI_CHARLIEPLEX_SWITCH_3_PIN);
	switches <<= 1;
	switches |= input_state(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_high(PI_CHARLIEPLEX_SWITCH_1_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_1_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_1_PIN);

	output_drive(PI_CHARLIEPLEX_SWITCH_2_PIN);
	switches <<= 1;
	switches |= input_state(PI_CHARLIEPLEX_SWITCH_3_PIN);	
	switches <<= 1;
	switches |= input_state(PI_CHARLIEPLEX_SWITCH_1_PIN);
	output_high(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_2_PIN);

	//Note: In this block charliplex1 must be read before charliplex2
	// just to charliplex2 has enough time to recover because charliplex2 was 
	// low in previous block. This avoid to put a delay here.
	output_drive(PI_CHARLIEPLEX_SWITCH_3_PIN);
	switches <<= 1;
	switches |= input_state(PI_CHARLIEPLEX_SWITCH_1_PIN);
	switches <<= 1;
	switches |= input_state(PI_CHARLIEPLEX_SWITCH_2_PIN);
	output_high(PI_CHARLIEPLEX_SWITCH_3_PIN);
	output_float(PI_CHARLIEPLEX_SWITCH_3_PIN);
	output_low(PI_CHARLIEPLEX_SWITCH_3_PIN);

	return ~switches;
}



#int_timer0
void isr_timer0()
{
	static int8 counter = 0;
	
	set_timer0(IN_TIMER_INIT_TIME);
	
	if ( (counter++ & 0x07) == 0 ) {
		int8 current_states = in_read_charlieplex_inputs();
		buttons_update( current_states );
	}
	
	in_encoder_read_and_update();
}
