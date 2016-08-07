#ifndef ADCONVERT_H
#define	ADCONVERT_H

typedef void (*AdcReadCallback) ( int16 value );

extern AdcReadCallback adc_read_callback;

void adc_init();
int16 adc_read( int8 channel ) ;
void adc_read_async( int8 channel, int8 delay ) ;


#endif
