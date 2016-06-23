#ifndef INENCODER_H
#define	INENCODER_H

#define INCREMENT_AUTO_RATE 0xFFFF

void encoder_update( int1 channelA, int1 channelB ) ;

void encoder_set_increment( int16 min, int16 max, int16 rate );
signed int16 encoder_increment( int16 current );

#endif	/* INENCODER_H */

