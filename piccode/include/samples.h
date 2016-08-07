#ifndef SAMPLES_H
#define	SAMPLES_H

#define SAMPLES_HISTORY_SIZE_LOG	3
#define SAMPLES_HISTORY_SIZE		(1<<SAMPLES_HISTORY_SIZE_LOG)

void samples_init();
int16 samples_add( int16 value );
int16 samples_std_deviation() ;
int16 samples_efficiency();

#endif

