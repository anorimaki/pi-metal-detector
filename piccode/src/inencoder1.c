#include "main.h"
#include "inencoder.h"
#include "mathutil.h"
#include <stdlib.h>


///////////////////////////////////////////////////////////////
// Encoder
///////////////////////////////////////////////////////////////
#define ENCODER_STATE_IDLE      0
#define ENCODER_STATE_INC		1
#define ENCODER_STATE_DEC		2
#define ENCODER_STATE_UP		3
#define ENCODER_STATE_DOWN      4

#define ENCODER_MAX_ACCUMULATED_SPEED   0x7FFF
#define ENCODER_MIN_ACCUMULATED_SPEED   0x8000

#define ENCODER_INITIAL_SPEED   8

struct InEncoder
{
	int8 state;
	int16 time_periods; //Time counter from last encoder read.
	signed int8 pulses; //Pulse counter from last encoder read.
	int8 losses; //Error counter from last encoder read.
};

struct InEncoder encoder;

struct
{
	int16 min;
	int16 max;
	int16 rate;
} encoder_settings;

void encoder_update(int1 channelA, int1 channelB)
{
	++encoder.time_periods;

	if (channelA == channelB) {
		if (encoder.state == ENCODER_STATE_INC) {
			encoder.pulses++;
		}
		else if (encoder.state == ENCODER_STATE_DEC) {
			encoder.pulses--;
		}
		encoder.state = channelA ? ENCODER_STATE_UP : ENCODER_STATE_DOWN;
		return;
	}

	if (channelA) { //A && !B
		if (encoder.state == ENCODER_STATE_UP) {
			encoder.state = ENCODER_STATE_DEC; //First change on B
		}
		else if (encoder.state == ENCODER_STATE_DOWN) {
			encoder.state = ENCODER_STATE_INC; //First change on A
		}
		return;
	}

	//!A && B
	if (encoder.state == ENCODER_STATE_UP) {
		encoder.state = ENCODER_STATE_INC; //First change on A
	}
	else if (encoder.state == ENCODER_STATE_DOWN) {
		encoder.state = ENCODER_STATE_DEC; //First change on B
	}
}

void encoder_set_increment(int16 min, int16 max, int8 rate)
{
	disable_interrupts(INT_TIMER4);
	encoder.time_periods = 0;
	encoder.pulses = 0;
	encoder.state = ENCODER_STATE_IDLE;
	encoder.losses = 0;
	enable_interrupts(INT_TIMER4);

	encoder_settings.min = min;
	encoder_settings.max = max;

	if (rate == INCREMENT_AUTO_RATE) {
		rate = ((max - min) / 200) + 1;
	}
	encoder_settings.rate = rate;
}


#define ENCODER_MIN_TIME_PERIODS 100      //2ms * 100 = 200ms

int16 encoder_increment(int16 current)
{
	disable_interrupts(INT_TIMER4);
	if (encoder.time_periods < ENCODER_MIN_TIME_PERIODS) {
		enable_interrupts(INT_TIMER4);
		return current;
	}
	signed int8 pulses = encoder.pulses;
	int8 time_periods = encoder.time_periods;
	encoder.time_periods = 0;
	encoder.pulses = 0;
	encoder.losses = 0;
	enable_interrupts(INT_TIMER4);

	if (pulses == 0) {
		return current;
	}

	int8 pulse_count = abs(pulses);

	int16 increment = pulse_count;

		//Calculate velocity. Apply factor of 32 before divide.
	int8 velocity = (increment << 5) / time_periods;

		//Adjust rate to velovity. Max velocity = 5
	int16 rate = encoder_settings.rate >> CHECKED_SUB(5, velocity, 0);

	++velocity;		//Min velociy = 1
	increment = _mul((int8) velocity, pulse_count);

	if (rate != 0) {
		increment *= rate;
	}

	if (pulses > 0) {
		return CHECKED_ADD(current, increment, encoder_settings.max);
	}
	return CHECKED_SUB(current, increment, encoder_settings.min);
}

