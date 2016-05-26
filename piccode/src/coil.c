#include "main.h"
#include "coil.h"
#include "config.h"
#include "samples.h"
#include "picconfig.h"



//#define ADC_IN_SLEEP


struct Coil coil;

#define COIL_HISTORY_PEAK   0
#define COIL_HISTORY_DECAY  1

struct
{
	int1 type;
	int16 value;
} coil_history;

void coil_init()
{
	IDLEN = 0; //Enter in Sleep mode if SLEEP is executed

	output_low(PI_COIL_CTRL_PIN);
	output_drive(PI_COIL_CTRL_PIN);

	output_low(PI_ADC_INDICATOR_PIN);
	output_drive(PI_ADC_INDICATOR_PIN);

	//VREF-: VSS, VREF+: VDD
	setup_adc_ports(PI_DECAY_SIGNAL_PIN | PI_COIL_VOLTAGE_PIN, VSS_VDD);
	setup_adc(ADC_OFF);
	disable_interrupts(INT_AD);
	
	samples_init();
}


//
// Precondition: interrupts sould be disabled for good timing confidence
//

void coil_pulse()
{
	output_high(PI_COIL_CTRL_PIN);
	delay_us(coil.pulse_length);
	output_low(PI_COIL_CTRL_PIN);
}


int16 a2d_converter()
{
#if defined(ADC_IN_SLEEP)
	//Internal A/D clock must be selected to use A/D in sleep mode (better acuracy)
	//Acquisition Time (TACQT) = TAD*2 = 4us * 2 = 8us (Could be 4us?)
	setup_adc(ADC_CLOCK_INTERNAL | ADC_TAD_MUL_2);

	clear_interrupt(INT_AD);
	enable_interrupts(INT_AD); //We want the ADC to wake from sleep
	read_adc(ADC_START_ONLY);
#asm
	SLEEP
#endasm
	int16 ret = read_adc(ADC_READ_ONLY); //Conversion time: 13 TADs -> 13*4 = 52us
	disable_interrupts(INT_AD);
#else
	//TAD: 1us for a 16Mhz clock
	//Acquisition Time (TACQT) = TAD*4 = 1us * 4 = 4us
	setup_adc(ADC_CLOCK_DIV_16 | ADC_TAD_MUL_4);
	//	output_high(PI_ADC_INDICATOR_PIN);
	int16 ret = read_adc(ADC_START_AND_READ); //Conv. time: 13 TADs -> 13*1 = 13us
	//	output_low(PI_ADC_INDICATOR_PIN);
	return (ret & 0x8000) ? 0 : ret;
#endif   
}


void coil_check_samples_history(int1 type, int16 value)
{
	if ((type == coil_history.type) && (value == coil_history.value))
		return;

	coil_history.type = type;
	coil_history.value = value;
	
	samples_init();
}


#inline
int16 coil_read_peak()
{
	set_adc_channel(PI_COIL_VOLTAGE_CH);
	int16 ret = a2d_converter();
	setup_adc(ADC_OFF);
	
	return ret;
}


int16 coil_peak_ref()
{
		//Wait to guaranty that previous pulse does not affect measure
	delay_ms( COIL_PULSE_PERIOD );
						
	disable_interrupts(GLOBAL);
	int16 ret = coil_read_peak();
	enable_interrupts(GLOBAL);

	return ret;
}


int16 coil_peak()
{
	coil_check_samples_history(COIL_HISTORY_PEAK, coil.pulse_length);

	//Disable interrupts for good timing accuracy.
	disable_interrupts(GLOBAL);

	coil_pulse();
	delay_us(2);
	int16 measure = coil_read_peak();

	//Enable interrupts to capture read input switches
	enable_interrupts(GLOBAL);

	return samples_add( measure );
}


int16 coil_read_decay( int8 delay )
{
	//Disable interrupts for good timing accuracy.
	disable_interrupts(GLOBAL);

	coil_pulse();

	set_adc_channel(PI_DECAY_SIGNAL_CH);

	delay_us(delay);

	int16 measure = a2d_converter();

	setup_adc(ADC_OFF);

	enable_interrupts(GLOBAL);
	
	return COIL_MAX_ADC_VALUE-measure;
}


int16 coil_custom_sample( int8 delay, int8 n_log2 )
{
	int8 n = 1 << n_log2;
	int16 sum = 0;
	for( int8 i; i<n; ++i ) {
		sum += coil_read_decay( delay );
		delay_ms( COIL_PULSE_PERIOD );
	}
	return sum >> n_log2;
}


int16 coil_sample()
{
	coil_check_samples_history(COIL_HISTORY_DECAY, coil.sample_delay);
	
	int16 measure = coil_read_decay( coil.sample_delay );
	
	return samples_add( measure );
}
