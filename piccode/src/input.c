#include "main.h"
#include "input.h"
#include "mathov.h"

void in_update_encoder( int1 channelA, int1 channelB ) ;
void in_update_switches( int8 switches_state );



//At 16Mhz counter increments every 64us:
//	- For 250 counts, overflows every 16ms
//	- For 78 counts, overflows every 5ms
#define TIMER_INIT_TIME (255-8)
#define TIMER_PERIOD_MS 5


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
	
	in_update_switches( current_states );
}


///////////////////////////////////////////////////////////////
// Switches
///////////////////////////////////////////////////////////////
#define CHANGE_CONFIDENT_THRESHOLD 2

struct InSwitch in_switches[SWITCHES_SIZE];

void in_update_switches( int8 switches_state )
{
	int8 i;
	for (i = 0; i < SWITCHES_SIZE; ++i ) {
		struct InSwitch* sw = &in_switches[i];

		int1 current_state = switches_state & 0x01;
		switches_state >>= 1;
	
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
// Encoder
///////////////////////////////////////////////////////////////
#define ENCODER_STATE_IDLE		0
#define ENCODER_STATE_INC_0		1		//A=0, B=1 after ENCODER_STATE_IDLE
#define ENCODER_STATE_INC_1		2		//A=0, B=0 after ENCODER_STATE_INC_0
#define ENCODER_STATE_INC_2		3		//A=1, B=0 after ENCODER_STATE_INC_X
#define ENCODER_STATE_DEC_0		4		//A=1, B=0 after ENCODER_STATE_IDLE
#define ENCODER_STATE_DEC_1		5		//A=0, B=0 after ENCODER_STATE_DEC_0
#define ENCODER_STATE_DEC_2		6		//A=1, B=0 after ENCODER_STATE_INC_X
#define ENCODER_STATE_A0B0		7
#define ENCODER_STATE_A0B1		8
#define ENCODER_STATE_A1B0		9

#define ENCODER_MAX_ACCUMULATED_SPEED	0x7FFF
#define ENCODER_MIN_ACCUMULATED_SPEED	0x8000

#define ENCODER_INITIAL_SPEED	8

struct InEncoder {
	int8 state;
	int16 time_periods;
	signed int8 pulses;
};

struct InEncoder in_encoder;

struct
{
	int16 min;
	int16 max;
	int16 rate;
} in_increment_state;


void in_update_encoder( int1 channelA, int1 channelB ) 
{
	in_encoder.time_periods++;
	if ( channelA && channelB ) {
		if ( (in_encoder.state == ENCODER_STATE_INC_1) ||
			(in_encoder.state == ENCODER_STATE_INC_2) ) {
			in_encoder.pulses++;
		}
		else if ( (in_encoder.state == ENCODER_STATE_DEC_1) ||
				 (in_encoder.state == ENCODER_STATE_DEC_2) ) {
			in_encoder.pulses--;
		}
		else if ( in_encoder.state == ENCODER_STATE_IDLE ) {
		}
		else {
			printf( "L" );
		}
		in_encoder.state = ENCODER_STATE_IDLE;
		return;
	}
	if ( !channelA && channelB ) {
		if ( in_encoder.state == ENCODER_STATE_IDLE ) {
			in_encoder.state = ENCODER_STATE_INC_0;
		}
		else if ( in_encoder.state == ENCODER_STATE_INC_2 ) {
			in_encoder.state = ENCODER_STATE_INC_0;
			in_encoder.pulses++;
		}
		else if ( (in_encoder.state==ENCODER_STATE_DEC_0) ||
				 (in_encoder.state==ENCODER_STATE_DEC_1) ||
				 (in_encoder.state==ENCODER_STATE_A0B0) ) {
			in_encoder.state = ENCODER_STATE_DEC_2;
		}
        else if ( (in_encoder.state==ENCODER_STATE_INC_0) ||
				 (in_encoder.state==ENCODER_STATE_DEC_2) ) {
        }
		else {
			in_encoder.state = ENCODER_STATE_A0B1;
		}
		return;
	}
	if ( channelA && !channelB ) {
		if ( in_encoder.state == ENCODER_STATE_IDLE ) {
			in_encoder.state = ENCODER_STATE_DEC_0;
		}
		else if ( in_encoder.state == ENCODER_STATE_DEC_2 ) {
			in_encoder.state = ENCODER_STATE_DEC_0;
			in_encoder.pulses--;
		}
		else if ( (in_encoder.state==ENCODER_STATE_INC_0) ||
				 (in_encoder.state==ENCODER_STATE_INC_1) ||
				 (in_encoder.state==ENCODER_STATE_A0B0) ) {
			in_encoder.state = ENCODER_STATE_INC_2;
		}
        else if ( (in_encoder.state==ENCODER_STATE_DEC_0) ||
				 (in_encoder.state==ENCODER_STATE_INC_2) ) {
        }
		else {
			in_encoder.state = ENCODER_STATE_A1B0;
		}
		return;
	}
	if ( !channelA && !channelB ) {
		if ( (in_encoder.state == ENCODER_STATE_INC_0) ||
			(in_encoder.state == ENCODER_STATE_A0B1) ) {
			in_encoder.state = ENCODER_STATE_INC_1;
		}
		else if ( in_encoder.state == ENCODER_STATE_DEC_0 ||
			(in_encoder.state == ENCODER_STATE_A1B0) ) {
			in_encoder.state = ENCODER_STATE_DEC_1;
		}
        else if ( (in_encoder.state==ENCODER_STATE_DEC_1) ||
				 (in_encoder.state==ENCODER_STATE_INC_1) ) {
        }
		else {
			in_encoder.state = ENCODER_STATE_A0B0;
		}
		return;
	}
}


void in_init_increment(int16 min, int16 max, int16 rate)
{
	disable_interrupts(INT_TIMER0);
	in_encoder.time_periods=0;
	in_encoder.pulses=0;
	in_encoder.state=ENCODER_STATE_IDLE;
	enable_interrupts(INT_TIMER0);
	
	in_increment_state.min = min;
	in_increment_state.max = max;

	if (rate == INCREMENT_AUTO_RATE) {
		rate = 15000 / (max - min); //All range in 15s
	}
	in_increment_state.rate = rate;
}


signed int16 in_increment(int16 current)
{
	disable_interrupts(INT_TIMER0);
	signed int16 pulses = in_encoder.pulses;
	int16 time_periods = in_encoder.time_periods;
	in_encoder.time_periods=0;
	in_encoder.pulses=0;
	enable_interrupts(INT_TIMER0);
	
	printf( "P: %Ld\r\n", pulses );
	
	signed int16 increment = (pulses<<7) / time_periods;
	
	if ( pulses>0 ) {
		if ( pulses>increment ) {
			increment = pulses;
		}
		int16 max_increment = in_increment_state.max - current;
		return max_increment > increment ? increment : max_increment;
	}
	
	if ( pulses<increment ) {
		increment = pulses;
	}
	signed int16 max_decrement = in_increment_state.min - current;
	return max_decrement < increment ? increment : max_decrement;
}
	