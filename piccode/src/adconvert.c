#include "main.h"
#include "adconvert.h"
#include "picconfig.h"


//#define ADC_IN_SLEEP
#define ADQUISITION_TIME	1	//In us


void adc_init()
{			//VREF-: VSS, VREF+: VDD
	setup_adc_ports(PI_DECAY_SIGNAL_AN | 
					PI_COIL_VOLTAGE_AN |
					PI_BATTERY_VOLTAGE_AN,
					VSS_VDD);
	setup_adc(ADC_OFF);
	disable_interrupts(INT_AD);
	
	output_low(PI_ADC_INDICATOR_PIN);
	output_drive(PI_ADC_INDICATOR_PIN);
}


#inline
int16 a2d_converter()
{
#if defined(ADC_IN_SLEEP)
	clear_interrupt(INT_AD);
	enable_interrupts(INT_AD); //We want the ADC to wake from sleep
	read_adc(ADC_START_ONLY);
#asm
	SLEEP
#endasm
	int16 ret = read_adc(ADC_READ_ONLY); //Conversion time: 13 TADs -> 13*4 = 52us
	disable_interrupts(INT_AD);
#else
//	output_high(PI_ADC_INDICATOR_PIN);
	int16 ret = read_adc(ADC_START_AND_READ); //Conv. time: 13 TADs -> 13*1 = 13us
//	output_low(PI_ADC_INDICATOR_PIN);
	
#endif 
	return (ret & 0x8000) ? 0 : ret;
}



int16 adc_read( int8 channel ) 
{
	//TAD: 1us for a 16Mhz clock
	//Automatic adqusition time disabled
	setup_adc(ADC_CLOCK_DIV_16);
	delay_us( ADQUISITION_TIME );
	set_adc_channel( channel );
	int16 ret = a2d_converter(); //Conv. time: 13 TADs -> 13*1 = 13us
	setup_adc(ADC_OFF);
	
	return ret;
}