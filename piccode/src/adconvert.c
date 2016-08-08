#include "main.h"
#include "adconvert.h"
#include "mathutil.h"
#include "picconfig.h"


//#define ADC_IN_SLEEP
#define ADQUISITION_TIME	1	//In us
#define ADC_INVALID_VALUE	0xFFFF


//This high value avoids that new ADC starts without program it.
//If TIMER3_BASE was 0 and daly was 0 too:
//	when CPP1 is set to final_delay (0):
//		- ADC convertision starts: OK
//		- timer3 is reset (set to 0): OK
//	as the reset of timer3 match CPP1 value (0), new ADC starts: Bad!
// TIMER3_BASE should be great enough to ADC ends
// Note: playing with CCP1 interrupt configured as high interrupt can work
//	but only relaxes this max value.
#define TIMER3_BASE		0x8000


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
	TMR3RD16 = 0;	//Read and write it two 8 bits op to avoid TMR3H was a buffer
	set_timer3( TIMER3_BASE );
	
	disable_interrupts( INT_CCP1 );	
	setup_ccp1( CCP_COMPARE_RESET_TIMER | CCP_USE_TIMER3_AND_TIMER4 );
		
#if defined(ADC_IN_SLEEP)
	IDLEN = 0; //Enter in Sleep mode if SLEEP is executed
#endif
}


#int_ad
void isr_adc()
{
	TMR3ON = 0;		//Stops timer3
	
	adc_internal.last_value = read_adc(ADC_READ_ONLY);
	if ( adc_internal.last_value & 0x8000 ) {
		adc_internal.last_value = 0;		//Avoid negative values
	}
	disable_interrupts(INT_AD);
	setup_adc(ADC_OFF);
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



#define ADC_SETUP_TIME	10		//In us
void adc_read_async( int8 channel, int8 delay ) 
{
	//Automatic adqusition time disabled
	//TAD: 1us for a 16Mhz clock
	setup_adc(ADC_CLOCK_DIV_16|ADC_TAD_MUL_2);
	set_adc_channel( channel );
	
	clear_interrupt(INT_AD);
	clear_interrupt(INT_CCP1);
	
	int16 final_delay = delay;
	final_delay = CHECKED_SUB( final_delay+TIMER3_BASE,
							ADC_SETUP_TIME, TIMER3_BASE );
	CCP_1 = final_delay;		//Program adc 
	set_timer3( TIMER3_BASE );
	TMR3ON = 1;					//Start timer3 
	
	enable_interrupts(INT_AD);

	//read_adc(ADC_START_ONLY);		//Do not start read. Read will be started 
									//when CCP1 matches timer3
#if defined(ADC_IN_SLEEP)
#asm
	SLEEP
#endasm
#endif
}