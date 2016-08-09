#include "main.h"
#include "coil.h"
#include "config.h"
#include "samples.h"
#include "mathutil.h"
#include "adconvert.h"
#include "picconfig.h"


///http://www.whiteselectronics.com/media/wysiwyg/SurfPI_SurfMII_FieldEval_EngReport.pdf
// 'Pi detectors tipically operate in the range of 100 pulses per second'
//
// http://chemelec.com/Projects/Metal-AV/Metal-AV.htm
// 'The Present Frequency of Operation is 300 Hz.'
#define COIL_PULSE_PERIOD               16000    //In us


struct Coil coil;

void coil_add_value( int16 value ) ;
void coil_add_invert_value( int16 value ) ;
void coil_begin( int8 channel, int8 read_delay, int1 invert_signal );


struct
{
	int8 read_channel;
	int8 working_read_delay;			//In us
	int16 last_value;
	int1 is_active;
	int1 invert_signal;
} coil_internal;


void coil_init()
{
	output_low(PI_COIL_CTRL_PIN);
	output_drive(PI_COIL_CTRL_PIN);
}


void coil_read_decay_begin()
{
	coil_begin( PI_DECAY_SIGNAL_CH, coil.sample_delay, 1 );
}


void coil_read_peak_begin()
{
	coil_begin( PI_COIL_VOLTAGE_CH, 2, 0 );
}


void coil_begin( int8 channel, int8 read_delay, int1 invert_signal )
{
	coil_internal.read_channel = channel;
	coil_internal.working_read_delay = read_delay;
	coil_internal.is_active = 0;
	
	samples_init();
	
	if ( invert_signal )
		adc_read_callback = coil_add_invert_value;
	else
		adc_read_callback = coil_add_value;
	
	//At 4Mhz instruction frequency (ClockF=16Mhz/4): it increments every 4us
	//	With post scaler: generate an interrupt every 16 periods:
	//		Resolution: 64us
	//		Min period: 64us (15.625 KHz) 
	//		Max period: 255*64us = 16.320 ms (61Hz)
	setup_timer_4( T4_DIV_BY_16, COIL_PULSE_PERIOD>>6, 16 );
	coil_wakeup();
}


void coil_end()
{
	coil_sleep();
	setup_timer_4( T4_DISABLED, 0, 1 );
}


void coil_sleep()
{
	disable_interrupts(INT_TIMER4);
	while( coil_internal.is_active );
}


void coil_wakeup()
{
	enable_interrupts(INT_TIMER4);
}


#inline
void coil_pulse()
{		//This code is executed in a ISR.
		//It's strange a delay in a ISR but it's the only way to ensure an 
		//accurate pulse length without usign interrupts priorities.
	output_high(PI_COIL_CTRL_PIN);
	delay_us(coil.pulse_length);		
	output_low(PI_COIL_CTRL_PIN);
}


#int_timer4
void coil_pulse_and_read()
{
	coil_internal.is_active = 1;
	coil_pulse();
	adc_read_async( coil_internal.read_channel,
				coil_internal.working_read_delay ); 
}

#define COIL_ADD_VALUE_IMPL(value) \
	coil_internal.last_value = samples_add( value ); \
	coil_internal.is_active = 0;

void coil_add_value( int16 value ) 
{
	COIL_ADD_VALUE_IMPL(value)
}


void coil_add_invert_value( int16 value ) 
{
	COIL_ADD_VALUE_IMPL( COIL_MAX_ADC_VALUE-value );
}


int1 coil_fetch_result() 
{
	disable_interrupts( INT_AD );
	coil.result.noise = samples_efficiency();
	coil.result.value = coil_internal.last_value;
	enable_interrupts( INT_AD );
	return coil.result.noise != SAMPLES_UNDEFINED_VALUE;
}


void coil_set_pulse_length( int16 pulse_length ) 
{
	if ( pulse_length == coil.pulse_length )
		return;
	coil_sleep();
	samples_init();
	coil.pulse_length = pulse_length;
	coil_wakeup();
}


void coil_set_read_delay( int8 delay ) 
{
	coil_set_working_read_delay( delay );
	coil.sample_delay = delay;
}


void coil_set_working_read_delay( int8 delay )
{
	if ( delay == coil_internal.working_read_delay )
		return;
	coil_sleep();
	samples_init();
	coil_internal.working_read_delay = delay;
	coil_wakeup();
}


int16 coil_peak_ref()
{
	coil_sleep();
	
	int16 ret = adc_read( PI_COIL_VOLTAGE_CH ); 
		
	coil_wakeup();
	
	return ret;
}


signed int32 coil_normalize(int16 value, int16 zero, int16 new_max_value)
{
	return math_change_range( value - zero, 
				COIL_MAX_ADC_VALUE - zero, new_max_value );
} 
