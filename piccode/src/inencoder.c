#include "main.h"
#include "inencoder.h"
#include "mathov.h"


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

struct InEncoder encoder;

struct
{
	int16 min;
	int16 max;
	int16 rate;
} encoder_increment_state;


void encoder_update( int1 channelA, int1 channelB ) 
{
	encoder.time_periods++;
	if ( channelA && channelB ) {
		if ( (encoder.state == ENCODER_STATE_INC_1) ||
			(encoder.state == ENCODER_STATE_INC_2) ) {
			encoder.pulses++;
		}
		else if ( (encoder.state == ENCODER_STATE_DEC_1) ||
				 (encoder.state == ENCODER_STATE_DEC_2) ) {
			encoder.pulses--;
		}
		else if ( encoder.state == ENCODER_STATE_IDLE ) {
		}
		else {
			printf( "L" );
		}
		encoder.state = ENCODER_STATE_IDLE;
		return;
	}
	if ( !channelA && channelB ) {
		if ( encoder.state == ENCODER_STATE_IDLE ) {
			encoder.state = ENCODER_STATE_INC_0;
		}
		else if ( encoder.state == ENCODER_STATE_INC_2 ) {
			encoder.state = ENCODER_STATE_INC_0;
			encoder.pulses++;
		}
		else if ( (encoder.state==ENCODER_STATE_DEC_0) ||
				 (encoder.state==ENCODER_STATE_DEC_1) ||
				 (encoder.state==ENCODER_STATE_A0B0) ) {
			encoder.state = ENCODER_STATE_DEC_2;
		}
        else if ( (encoder.state==ENCODER_STATE_INC_0) ||
				 (encoder.state==ENCODER_STATE_DEC_2) ) {
        }
		else {
			encoder.state = ENCODER_STATE_A0B1;
		}
		return;
	}
	if ( channelA && !channelB ) {
		if ( encoder.state == ENCODER_STATE_IDLE ) {
			encoder.state = ENCODER_STATE_DEC_0;
		}
		else if ( encoder.state == ENCODER_STATE_DEC_2 ) {
			encoder.state = ENCODER_STATE_DEC_0;
			encoder.pulses--;
		}
		else if ( (encoder.state==ENCODER_STATE_INC_0) ||
				 (encoder.state==ENCODER_STATE_INC_1) ||
				 (encoder.state==ENCODER_STATE_A0B0) ) {
			encoder.state = ENCODER_STATE_INC_2;
		}
        else if ( (encoder.state==ENCODER_STATE_DEC_0) ||
				 (encoder.state==ENCODER_STATE_INC_2) ) {
        }
		else {
			encoder.state = ENCODER_STATE_A1B0;
		}
		return;
	}
	if ( !channelA && !channelB ) {
		if ( (encoder.state == ENCODER_STATE_INC_0) ||
			(encoder.state == ENCODER_STATE_A0B1) ) {
			encoder.state = ENCODER_STATE_INC_1;
		}
		else if ( encoder.state == ENCODER_STATE_DEC_0 ||
			(encoder.state == ENCODER_STATE_A1B0) ) {
			encoder.state = ENCODER_STATE_DEC_1;
		}
        else if ( (encoder.state==ENCODER_STATE_DEC_1) ||
				 (encoder.state==ENCODER_STATE_INC_1) ) {
        }
		else {
			encoder.state = ENCODER_STATE_A0B0;
		}
		return;
	}
}


void encoder_set_increment(int16 min, int16 max, int16 rate)
{
	disable_interrupts(INT_TIMER0);
	encoder.time_periods=0;
	encoder.pulses=0;
	encoder.state=ENCODER_STATE_IDLE;
	enable_interrupts(INT_TIMER0);
	
	encoder_increment_state.min = min;
	encoder_increment_state.max = max;

	if (rate == INCREMENT_AUTO_RATE) {
		rate = 15000 / (max - min); //All range in 15s
	}
	encoder_increment_state.rate = rate;
}


signed int16 encoder_increment(int16 current)
{
	disable_interrupts(INT_TIMER0);
	signed int16 pulses = encoder.pulses;
	int16 time_periods = encoder.time_periods;
	encoder.time_periods=0;
	encoder.pulses=0;
	enable_interrupts(INT_TIMER0);
	
	printf( "P: %Ld\r\n", pulses );
	
	signed int16 increment = (pulses<<7) / time_periods;
	
	if ( pulses>0 ) {
		if ( pulses>increment ) {
			increment = pulses;
		}
		int16 max_increment = encoder_increment_state.max - current;
		return max_increment > increment ? increment : max_increment;
	}
	
	if ( pulses<increment ) {
		increment = pulses;
	}
	signed int16 max_decrement = encoder_increment_state.min - current;
	return max_decrement < increment ? increment : max_decrement;
}
	