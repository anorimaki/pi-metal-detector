#ifndef SAMPLES_H
#define	SAMPLES_H

#define SAMPLES_HISTORY_MAX_SIZE_LOG	7		
#define SAMPLES_HISTORY_MIN_SIZE_LOG	1
#define SAMPLES_HISTORY_MIN_SIZE		(1<<SAMPLES_HISTORY_MIN_SIZE_LOG)
#define SAMPLES_HISTORY_MAX_SIZE		(1<<SAMPLES_HISTORY_MAX_SIZE_LOG)
#define SAMPLES_UNDEFINED_VALUE         -1

void samples_init( int8 size_log );
int16 samples_add( int16 value );
int16 samples_std_deviation() ;
int16 samples_efficiency();

#inline
int8 samples_size_log();

#endif

