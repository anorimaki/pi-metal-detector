#include "main.h"
#include "adconvert.h"
#include "picconfig.h"


//#define ADC_IN_SLEEP
#define ADQUISITION_TIME	1	//In us
#define ADC_INVALID_VALUE	0xFFFF

AdcReadCallback adc_read_callback;

struct 
{
	int16 last_value;
} adc_internal;


void adc_init()
{			//VREF-: VSS, VREF+: VDD
	setup_adc_ports(PI_DECAY_SIGNAL_AN | 
					PI_COIL_VOLTAGE_AN |
					PI_BATTERY_VOLTAGE_AN,
					VSS_VDD);
	setup_adc(ADC_OFF);
	disable_interrupts(INT_AD);
	disable_interrupts(INT_TIMER3);
	
	//At 4Mhz instruction frequency (ClockF=16Mhz/4), it increments evey us.
	setup_timer_3( T3_INTERNAL| T3_DIV_BY_4 );
	TMR3ON = 0;		//Stops timer3 but it's already configured
		
	setup_ccp1( CCP_COMPARE_RESET_TIMER | CCP_USE_TIMER3_AND_TIMER4 );
	
#if defined(ADC_IN_SLEEP)
	IDLEN = 0; //Enter in Sleep mode if SLEEP is executed
#endif
}


#int_ad
void isr_adc()
{
	adc_internal.last_value = read_adc(ADC_READ_ONLY);
	if ( adc_internal.last_value & 0x8000 ) {
		adc_internal.last_value = 0;		//Avoid negative values
	}
	disable_interrupts(INT_AD);
	setup_adc(ADC_OFF);
	TMR3ON = 0;				//Stops timer3
	if ( adc_read_callback != 0 ) {
		(*adc_read_callback)( adc_internal.last_value );
	}
}

int16 adc_read( int8 channel ) 
{
	AdcReadCallback callback = adc_read_callback;
	adc_read_callback = 0;
	
	adc_internal.last_value = ADC_INVALID_VALUE;
	adc_read_async( channel, 0 ) ;
	while( adc_internal.last_value == ADC_INVALID_VALUE );
	
	adc_read_callback = callback;
	
	return adc_internal.last_value;
}


void adc_read_async( int8 channel, int8 delay ) 
{
	set_timer3( 0 );
	TMR3ON = 1;			//Start timer3 
	CCP_1 = delay;		//Program adc (TODO: Adjust adquisition time and instructions wasted time)
	
	//Automatic adqusition time disabled
	//TAD: 1us for a 16Mhz clock
	setup_adc(ADC_CLOCK_DIV_16);
	set_adc_channel( channel );
	
	clear_interrupt(INT_AD);
	enable_interrupts(INT_AD);
	//read_adc(ADC_START_ONLY);		//Do not start read. Read will be started 
									//when CCP1 matches timer3
#if defined(ADC_IN_SLEEP)
#asm
	SLEEP
#endasm
#endif
}