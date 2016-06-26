#include "main.h"
#include "inencoder.h"
#include "mathutil.h"
#include <stdlib.h>


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
	int16 time_periods;		//Time counter from last encoder read.
	signed int8 pulses;		//Pulse counter from last encoder read.
	int8 losses;			//Error counter from last encoder read.
};

struct InEncoder encoder;

struct
{
	int16 min;
	int16 max;
	int16 rate;
} encoder_settings;

/*
 * Updtes encoder state with current channels value
 * 
 * It infers missing states if they can be inferred. For example, 
 * complete increment states are: 
 *		{A=1, B=1}, {A=0, B=1}, {A=0, B=0}, {A=1, B=0}, {A=1, B=1}
 * If states {A=0, B=1} or {A=0, B=0} are missing, it infferes that is an
 * increment.
 * But if {A=0, B=1} and {A=0, B=0} are missing, it counts as an error.
 * This is usefull if encoder scan is too slow to sample all channel variations.
 */
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
			encoder.losses++;
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
			
			//!channelA && !channelB
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
}


void encoder_set_increment(int16 min, int16 max, int8 rate)
{
	disable_interrupts(INT_TIMER0);
	encoder.time_periods=0;
	encoder.pulses=0;
	encoder.state=ENCODER_STATE_IDLE;
	encoder.losses=0;
	enable_interrupts(INT_TIMER0);
	
	encoder_settings.min = min;
	encoder_settings.max = max;

	if (rate == INCREMENT_AUTO_RATE) {
		rate = ((max-min) / 100) + 1;
	}
	encoder_settings.rate = rate;
}


#define ENCODER_MIN_TIME_PERIODS 32		//~512us*32 = 16ms

signed int16 encoder_increment(int16 current)
{
	disable_interrupts(INT_TIMER0);
	if( encoder.time_periods < ENCODER_MIN_TIME_PERIODS ) {
		enable_interrupts(INT_TIMER0);
		return 0;
	}
	signed int16 pulses = encoder.pulses;
	int16 time_periods = encoder.time_periods;
	encoder.time_periods=0;
	encoder.pulses=0;
	encoder.losses=0;
	enable_interrupts(INT_TIMER0);
	
	int16 pulse_count = abs(pulses);

	int16 increment = pulse_count;
	if ( pulse_count!=1 ) {
		increment <<= 6;
		increment /= time_periods;
		if ( pulse_count>increment ) {
			increment = pulse_count;
		}
		if ( pulse_count != increment ) {
			increment *= encoder_settings.rate;
		}
	}
	
	if ( pulses>0 ) {
		int16 max_increment = encoder_settings.max - current;
		return (max_increment > increment) ? increment : max_increment;
	}
	
	int16 max_decrement = current - encoder_settings.min;
	return -((max_decrement > increment) ? increment : max_decrement);
}
	