#include "main.h"
#include "adconvert.h"
#include "mathutil.h"
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
	enable_interrupts(INT_AD);
//	disable_interrupts(INT_AD);
	disable_interrupts(INT_TIMER1);
	
//	output_low( INDICATOR_PIN );
//	output_drive( INDICATOR_PIN );
	
	//At 4Mhz instruction frequency (ClockF=16Mhz/4), it increments evey us.
	setup_timer_1( T1_INTERNAL| T1_DIV_BY_4 );
	
	//Automatic adqusition time disabled
	//TAD: 1us for a 16Mhz clock
	setup_adc(ADC_CLOCK_DIV_16);	//Set ADCON2 register & ADON=1
	ADON=0;							//Turn off ADC module
	ADCON1 = 0x80;	//bit 7-6 TRIGSEL<1:0>: Special Trigger Select bits:
					//		10 = Selects the special trigger from the Timer1
					//bit 5-4 VCFG<1:0>: A/D VREF+ Configuration bits: 
					//		00 = AVDD
					//bit 3 VNCFG: A/D VREF- Configuration bit
					//		0 = AVSS
					//bit 2-0 CHSN<2:0>: Analog Negative Channel Select bits
					//		000 = Channel 00 (AVSS)
		
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
	//disable_interrupts(INT_AD);
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
	adc_read_async( channel, 1 ) ;
	while( adc_internal.last_value == ADC_INVALID_VALUE );
	
	adc_read_callback = callback;
	
	return adc_internal.last_value;
}


// delay must be greater than 0! CCP_1 ca not be 
// This is not the true delay. It must be added the overhead of ADC setup.
void adc_read_async( int8 channel, int8 delay ) 
{
	ADCON0 = (channel << 2);			//Select channel

//	enable_interrupts(INT_AD);

	set_timer1( 0xFF00 | -delay );
	ADON = 1;						//Turn on ADC module

//read_adc(ADC_START_ONLY);			//Do not start read. Read will be started 
									//when timer1 reach 0
#if defined(ADC_IN_SLEEP)
#asm
	SLEEP
#endasm
#endif
}