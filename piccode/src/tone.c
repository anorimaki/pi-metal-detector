#include "main.h"
#include "tone.h"
#include "coil.h"

#define TONE_MAX_DELAY_MS	1000

//Timer increments every 2us and uses only the 8 MSB.
//	-> 1 unit = 2 * 256 = 512us
// 8 LSM must me 0, so we do a final AND.
#define TONE_MAX_DELAY_UNITS	(((TONE_MAX_DELAY_MS*1000/512)+1) & 0xFF00)

/*
 * It uses:
 *	- Timer4 and CPP3 to generate a fixed ~1KHz tone
 *	- Timer3 and its interrupt to generate tone length and tone delay
 */


void tone_init()
{
	//At 16Mhz clock It increments every 4us.
	//Set it at max period (255), it resets every: ~1ms 
	//		-> It generate a tone about 1KHz
	setup_timer_4( T4_DIV_BY_16, 0xFF, 1 );
	disable_interrupts( INT_TIMER4 );
	TMR4ON = 0;
	
	// CCP5 generates 1KHz square signal
	setup_ccp5( CCP_PWM | CCP_USE_TIMER3_AND_TIMER4 );
	set_pwm5_duty( 0x1FC );			//50%
	disable_interrupts( INT_CCP5 );	
	
	//At 16Mhz clock, it increments every 2us
	//It completes a cycle every ~131ms
	setup_timer_3( T3_INTERNAL | T3_DIV_BY_8 );
	enable_interrupts( INT_TIMER3 );
	TMR3ON = 0;
	
	output_drive( PI_TONE_PIN );
}


void tone_begin()
{
}


void tone_end()
{
	TMR3ON = 0;
	TMR4ON = 0;
}


struct {
	struct {
		int8 timer_isr_counts;
		int16 timer_value;
	} delay;
	
	int8 timer_isr_count;
} tone;


#int_timer3
void tone_update() 
{
	if ( TMR4ON == 0 ) {
		if ( tone.timer_isr_count == 0 ) {
				//End delay and start tone
			TMR4ON = 1;
			return;
		}
		--tone.timer_isr_count;
		if ( tone.timer_isr_count == 0 ) {
			set_timer3( tone.delay.timer_value );
		}
		return;
	}
	
	tone.timer_isr_count = tone.delay.timer_isr_counts;
	if ( tone.timer_isr_count == 0 ) {
		if ( tone.delay.timer_value == 0 ) {
			return;		//Continue tone
		}
		set_timer3( tone.delay.timer_value );
	}
	TMR4ON = 0;			//End tone and start delay
}


// value is the range of 0..4095
void tone_apply( int16 value )
{
	if ( coil.zero > value ) {
		TMR3ON = 0;		//Stops timer3 -> stops output signal
		TMR4ON = 0;
		return;
	}
	
	value = math_change_log_range( value-coil.zero, 
					COIL_MAX_ADC_VALUE-coil.zero, TONE_MAX_DELAY_UNITS );
	
	int16 inv_value = TONE_MAX_DELAY_UNITS-value;
	
	disable_interrupts( INT_TIMER3 );
	tone.delay.timer_isr_counts = inv_value >> 8;
	tone.delay.timer_value = value << 8;
	enable_interrupts( INT_TIMER3 );
	
	if ( TMR3ON == 0 ) {
		TMR4ON = 1;
		TMR3ON = 1;
	}
}
