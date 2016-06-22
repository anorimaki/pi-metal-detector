#include "main.h"
#include "input.h"

#define CHANGE_CONFIDENT_THRESHOLD 2

//At 16Mhz counter increments every 64us:
//	- For 250 counts, overflows every 16ms
//	- For 78 counts, overflows every 5ms
#define TIMER_INIT_TIME (255-8)
#define TIMER_PERIOD_MS 5

#define INPUTS_SIZE					7

#define ROTATORY_INPUT_CHANNEL_A	4
#define ROTATORY_INPUT_CHANNEL_B	5

struct InSwitch in_switches[SWITCHES_SIZE];


#define ENCODER_STATE_IDLE		0
#define ENCODER_STATE_INC_0		0x01		//A=0, B=1 after ENCODER_STATE_IDLE
#define ENCODER_STATE_INC_1		0x02		//A=0, B=0 after ENCODER_STATE_INC_0
#define ENCODER_STATE_INC_2		0x04		//A=1, B=0 after ENCODER_STATE_INC_X
#define ENCODER_STATE_DEC_0		0x10		//A=1, B=0 after ENCODER_STATE_IDLE
#define ENCODER_STATE_DEC_1		0x20		//A=0, B=0 after ENCODER_STATE_DEC_0
#define ENCODER_STATE_DEC_2		0x40		//A=1, B=0 after ENCODER_STATE_INC_X

#define 

struct InEncoder {
	int8 state;
};

struct InSwitch in_encoder;

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

	for (int8 i = 0; i < SWITCHES_SIZE; ++i) {
		in_switches[i].state = FALSE;
		in_switches[i].changed = FALSE;
		in_switches[i].change_confident_count = 0;
	}

	in_encoder.state = ENCODER_STATE_IDLE;
	
	setup_timer_0(T0_INTERNAL | T0_DIV_256 | T0_8_BIT);
	set_timer0(TIMER_INIT_TIME);
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


void in_update_encoder( int1 channelA, int1 channelB ) 
{
	if ( channelA && channelB ) {
		if ( in_encoder.state & (ENCODER_STATE_INC_1|ENCODER_STATE_INC_2) ) {
			in_encoder.speed = CHECKED_SUM( in_encoder.speed, 
										 in_encoder.current_speed,
										 ENCODER_MAX_SPEED );
			in_encoder.pulses++;
			in_encoder.current_speed = ENCODER_MAX_SPEED;
		}
		if ( in_encoder.state & (ENCODER_STATE_DEC_1|ENCODER_STATE_DEC_2) ) {
			in_encoder.speed = CHECKED_SUB( in_encoder.speed, 
										 in_encoder.current_speed,
										 ENCODER_MIN_SPEED );
			in_encoder.pulses++;
			in_encoder.current_speed = ENCODER_MAX_SPEED;
		}
		in_encoder.state = ENCODER_STATE_IDLE;
		return;
	}
	if ( !channelA && channelB ) {
		if ( in_encoder.state & (ENCODER_STATE_INC_1|ENCODER_STATE_INC_2) ) {
			in_encoder.speed = CHECKED_SUM( in_encoder.speed, 
										 in_encoder.current_speed,
										 ENCODER_MAX_SPEED );
			in_encoder.pulses++;
			in_encoder.current_speed = ENCODER_MAX_SPEED;
		}
		if ( in_encoder.state & (ENCODER_STATE_DEC_1|ENCODER_STATE_DEC_2) ) {
			in_encoder.speed = CHECKED_SUB( in_encoder.speed, 
										 in_encoder.current_speed,
										 ENCODER_MIN_SPEED );
			in_encoder.pulses++;
			in_encoder.current_speed = ENCODER_MAX_SPEED;
		}
	}
}


#int_timer0
void isr_timer0()
{
	set_timer0(TIMER_INIT_TIME);

	int8 current_states = in_read_inputs();

	int1 channelA = current_states & 0x01;
	current_states >>= 1;
	int1 channelB = current_states & 0x01;
	current_states >>= 1;
	
	in_update_encoder( channelA, channelB );

	int8 i;
	for (i = 0; i < INPUTS_SIZE; ++i ) {
		struct InSwitch* sw = &in_switches[i];

		int1 current_state = current_states & 0x01;
		current_states >>= 1;
	
		if (current_state == sw->state) {
			sw->change_confident_count = 0;
			if (sw->state_time < (0xFFFF - TIMER_PERIOD_MS))
				sw->state_time += TIMER_PERIOD_MS;
		}
		else {
			if (++sw->change_confident_count == CHANGE_CONFIDENT_THRESHOLD) {
				sw->changed = TRUE;
				sw->state = current_state;
				sw->state_time = 0;
			}
		}
	}
}



// Wait until button releases or it has been pressed for more than 
// max_push_time ms. Returns true if max_push_time has been 
// reached.

int1 in_wait_for_release_timeout(int8 sw, int16 max_push_time)
{
	while (in_switches[sw].state) {
		if (in_switches[sw].state_time > max_push_time)
			return TRUE;
	}
	return FALSE;
}

void in_wait_for_release(int8 sw)
{
	while (in_switches[sw].state);
}

int1 in_button_pressed(int8 sw)
{
	int1 ret = in_switches[sw].state && in_switches[sw].changed;
	in_switches[sw].changed = FALSE;
	return ret;
}


///////////////////////////////////////////////////////////////
// Increment switches
///////////////////////////////////////////////////////////////

struct
{
	int16 min;
	int16 max;
	int16 rate;
} in_increment_state;

void in_init_increment(int16 min, int16 max, int16 rate)
{
	in_switches[SWITCH_INCREMENT].state_time = 0;
	in_switches[SWITCH_DECREMENT].state_time = 0;

	in_increment_state.min = min;
	in_increment_state.max = max;

	if (rate == INCREMENT_AUTO_RATE) {
		rate = 15000 / (max - min); //All range in 15s
	}
	in_increment_state.rate = rate;
}

int8 pulse_time_increment(int8 sw)
{
	int8 ret = in_switches[sw].state_time / in_increment_state.rate;
	in_switches[sw].state_time -= (ret * in_increment_state.rate);
	return ret;
}

signed int16 in_increment(int16 current)
{
	signed int16 increment = 0;

	if (in_switches[SWITCH_INCREMENT].state) {
		int16 raw_inc = pulse_time_increment(SWITCH_INCREMENT);
		increment = in_increment_state.max - current;
		increment = (increment > raw_inc) ? raw_inc : increment;
	}

	if (in_switches[SWITCH_DECREMENT].state) {
		int16 raw_dec = pulse_time_increment(SWITCH_DECREMENT);
		increment = current - in_increment_state.min;
		increment = (increment > raw_dec) ? -raw_dec : -increment;
	}

	return increment;
}