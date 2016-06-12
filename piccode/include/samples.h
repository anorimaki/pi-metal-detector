#ifndef SAMPLES_H
#define	SAMPLES_H

#define SAMPLES_HISTORY_SIZE_LOG	5
#define SAMPLES_HISTORY_SIZE		(1<<SAMPLES_HISTORY_SIZE_LOG)

void samples_init();
int16 samples_add( int16 value );
int16 samples_upper_deviation();

#endif
