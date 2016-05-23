#include "main.h"
#include "pimetaldec.h"
#include "config.h"
#include "picconfig.h"

#define MIN_INTEGRATION_RESULT 2
//#define ADC_IN_SLEEP


struct Config pi;

void pi_init()
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
}


//
// Precondition: interrupts sould be disabled for good timing confidence
//

void pi_coil_pulse()
{
	output_high(PI_COIL_CTRL_PIN);
	delay_us(pi.pulse_time);
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
	ret = read_adc(ADC_READ_ONLY); //Conversion time: 13 TADs -> 13*4 = 52us
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

int16 pi_read_peak_coil_ref()
{
	set_adc_channel(PI_COIL_VOLTAGE_CH);
	int16 ret = a2d_converter();
	setup_adc(ADC_OFF);

	return ret;
}

//
// Returns value in vols
//
int16 pi_read_peak_coil(int16 reference_5v)
{
	//Disable interrupts for good timing accuracy.
	disable_interrupts(GLOBAL);

	pi_coil_pulse();
	delay_us(2);
	int16 ret = pi_read_peak_coil_ref();

	//Enable interrupts to capture read input switches
	enable_interrupts(GLOBAL);

	ret *= 5;
	return ret / reference_5v;
}


int16 pi_raw_signle_sample()
{
	//Disable interrupts for good timing accuracy.
	disable_interrupts(GLOBAL);
	
	pi_coil_pulse();
	
	set_adc_channel(PI_DECAY_SIGNAL_CH);
	
	delay_us( pi.start_sample_delay );
	
	int16 sample = a2d_converter();
	
	setup_adc(ADC_OFF);
	
	enable_interrupts(GLOBAL);
	
	return sample;
}


//
// 
//
int16 pi_raw_sample()
{
	int16 ret = 0;
	for (int8 i = 0; i < 4; i++) {
		ret += pi_raw_signle_sample();
		delay_ms(100);
	}
	ret >>= 2;
	return ret;
}


signed int8 pi_sample() 
{
	signed int32 ret = pi.sample_zero_point;
	
	ret -= pi_raw_sample();
	
	ret *= 100;
	ret /= pi.sample_zero_point;

	return ret;
}