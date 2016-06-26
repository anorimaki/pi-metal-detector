#ifndef INENCODER_H
#define	INENCODER_H

/*
 * Updates encode state.
 */
void encoder_update( int1 channelA, int1 channelB ) ;

/*
 * Setup 'encoder_increment' function.
 * Setups minimum and maximum value and rate.
 * 'rate' is a multiplier factor that applies when there are more than one
 * encoder pulse pending to be read.
 */
#define INCREMENT_AUTO_RATE 0xFF
void encoder_set_increment( int16 min, int16 max, int8 rate );

/*
 * Returns increment (or decrement) based on the encode state and
 * settings: min and max value and rate.
 * 'current' is the value to be updated. This function nevers return an 
 * increment that exceeds maximun or mininum settings.
 * It clears encoder state.
 */
signed int16 encoder_increment( int16 current );

#endif

