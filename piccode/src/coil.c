#include "main.h"
#include "coil.h"
#include "config.h"
#include "samples.h"
#include "mathutil.h"
#include "adconvert.h"
#include "picconfig.h"


struct Coil coil;

void coil_add_value( int16 value ) ;
void coil_add_invert_value( int16 value ) ;
void coil_begin( int8 channel, int16 pulse_period, 
				int8 read_delay, int8 samples_history_size_log,
				int1 invert_signal );


struct
{
	struct {
		int16 pulse_period;             //In timer0 steps (64us)
		int8 sample_delay;              //In us after coil pulse ends
	} current;
	int8 read_channel;
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
	coil_begin( PI_DECAY_SIGNAL_CH, coil.pulse_period, coil.sample_delay, 
			 coil.samples_history_size_log, 1 );
}


void coil_read_peak_begin()
{
	coil_begin( PI_COIL_VOLTAGE_CH, COIL_READ_PEAK_PULSE_PERIOD_COUNT,
			 COIL_READ_PEAK_SAMPLE_DEPLAY, coil.samples_history_size_log, 0 );
}


void coil_begin( int8 channel, int16 pulse_period,
				int8 read_delay, int8 samples_history_size_log,
				int1 invert_signal )
{
	coil_internal.read_channel = channel;
	coil_internal.current.sample_delay = read_delay;
	coil_internal.current.pulse_period = pulse_period;
	coil_internal.is_active = 0;
	
	samples_init( samples_history_size_log );
	
	if ( invert_signal )
		adc_read_callback = coil_add_invert_value;
	else
		adc_read_callback = coil_add_value;
	
	//At 4Mhz instruction frequency (ClockF=16Mhz/4): it increments every 64us
	setup_timer_0( T0_INTERNAL | T0_DIV_256 );
	coil_wakeup();
}


void coil_end()
{
	coil_sleep();
	setup_timer_0( T0_OFF );
}


void coil_sleep()
{
	disable_interrupts(INT_TIMER0);
	while( coil_internal.is_active );
}


void coil_wakeup()
{
	set_timer0( -coil_internal.current.pulse_period );
	clear_interrupt( INT_TIMER0 );
	enable_interrupts( INT_TIMER0 );
}


#int_timer0
void coil_start_pulse()
{
	set_timer0( -coil_internal.current.pulse_period );
	
	coil_internal.is_active = 1;

	set_timer1( -coil.pulse_length );
	clear_interrupt( INT_TIMER1 );
	enable_interrupts( INT_TIMER1 );
	output_high(PI_COIL_CTRL_PIN);
}


#int_timer1 HIGH
void coil_end_pulse()
{
	output_low(PI_COIL_CTRL_PIN);
	disable_interrupts( INT_TIMER1 );
	adc_read_async( coil_internal.read_channel,
				coil_internal.current.sample_delay ); 
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
	samples_init( samples_size_log() );
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
	if ( delay == coil_internal.current.sample_delay )
		return;
	coil_sleep();
	samples_init( samples_size_log() );
	coil_internal.current.sample_delay = delay;
	coil_wakeup();
}


void coil_set_working_samples_history_size_log( int8 size_log )
{
	coil_sleep();
	samples_init( size_log );
	coil_wakeup();
}


int16 coil_peak_ref()
{
	return adc_read( PI_COIL_VOLTAGE_CH ); 
}


signed int32 coil_normalize(int16 value, int16 zero, int16 new_max_value)
{
	return math_change_range( value - zero, 
				COIL_MAX_ADC_VALUE - zero, new_max_value );
} 
