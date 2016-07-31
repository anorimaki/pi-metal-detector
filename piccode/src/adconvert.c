#include "main.h"
#include "adconvert.h"
#include "picconfig.h"


//#define ADC_IN_SLEEP


void adc_init()
{			//VREF-: VSS, VREF+: VDD
	setup_adc_ports(PI_DECAY_SIGNAL_AN | 
					PI_COIL_VOLTAGE_AN |
					PI_BATTERY_VOLTAGE_AN,
					VSS_VDD);
	setup_adc(ADC_OFF);
	disable_interrupts(INT_AD);
}


#inline
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
//	setup_adc(ADC_CLOCK_DIV_16 | ADC_TAD_MUL_0);
	setup_adc(ADC_CLOCK_DIV_16);
	delay_us(1);
	//output_high(PI_ADC_INDICATOR_PIN);
	int16 ret = read_adc(ADC_START_AND_READ); //Conv. time: 13 TADs -> 13*1 = 13us
	//output_low(PI_ADC_INDICATOR_PIN);
	
#endif 
	return (ret & 0x8000) ? 0 : ret;
}



int16 adc_read( int8 channel ) 
{
	set_adc_channel( channel );
	int16 ret = a2d_converter(); //Conv. time: 13 TADs -> 13*1 = 13us
	setup_adc(ADC_OFF);
	
	return ret;
}