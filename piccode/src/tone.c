#include "main.h"
#include "tone.h"
#include "coil.h"

#define TONE_PERIOD_INCREMENT_US	100
#define TONE_MIN_PERIOD_US			200			//In us. (5KHz)
#define TONE_MAX_PERIOD_US			25000		//In us. (40Hz)

#define TONE_MIN_CCP		(TONE_MIN_PERIOD_US/TONE_PERIOD_INCREMENT_US)/2
#define TONE_MAX_CCP		(TONE_MAX_PERIOD_US/TONE_PERIOD_INCREMENT_US)/2

void tone_init()
{
#if 1
		//Just to configure CCP1 but not to activate it. See comments of
		// tone_being() function.
	setup_ccp5( CCP_COMPARE_INT_AND_TOGGLE | CCP_USE_TIMER3_AND_TIMER4 );
	setup_ccp5( CCP_OFF );
	disable_interrupts( INT_CCP5 );	
#else
#asm
			//This is equivale to setup_ccp1 but it's done in assemble
			//because 'MOVWF  PSTR1CON' resets pull-ups resistors configuration
	MOVLW  01
	MOVWF  PSTR1CON		//P1A pin has the PWM waveform with polarity
						// control from CCP1M<1:0>
						//P1B, P1B, P1C: are assigned to ports pin
						//Output steering update occurs at the beginning 
						// of the instruction cycle boundary
	CLRF   ECCP1DEL		//CCP1 Delay = 0
						//ECCP1AS must be set by software to restart
	CLRF   ECCP1AS		//ECCP outputs are operating
						//Auto-shutdown is disabled
						//Drive pins, P1A and P1C, to ?0?
						//Drive pins, P1B and P1D, to ?0?
	BCF    C1TSEL		//Select TMR1/TMR2 for CCP1
#endasm
#endif
	
	output_drive( PI_TONE_PIN );
}

void tone_begin()
{
	// CCP5 generates output signal
	//setup_ccp5( CCP_COMPARE_INT_AND_TOGGLE | CCP_USE_TIMER3_AND_TIMER4 );
#asm
		//setup_ccp5() sould be here but 'MOVWF  PSTR1CON' of this function
		//resets pull-ups resistors configuration. setup_ccp1 has been
		//moved to tone_init() function and must be called before pull-ups 
		//resistors are configured
		//Here, CPP1 is only activated with previous configuration
	MOVLW  CCP_COMPARE_INT_AND_TOGGLE
	MOVWF  CCP5CON		//Compare mode, toggle output on match
#endasm
	
	
	// CCP3 reset timer to create a period (Special Event Trigger). 
	// It avoid the use of interrupt ISR
	// Note: Do not use CCP1 for Special Event Trigger or ADC will be started
	setup_ccp3( CCP_COMPARE_RESET_TIMER | CCP_USE_TIMER3_AND_TIMER4 );
	disable_interrupts( INT_CCP3 );	
	
	//At 4Mhz instruction frequency (ClockF=16Mhz/4): it increments every 4us.
	setup_timer_4( T4_DIV_BY_16, TONE_PERIOD_INCREMENT_US/4, 1 );
	disable_interrupts( INT_TIMER4 );	
		
	//It increments every timer4 period (100us)		
	setup_timer_3( T3_INTERNAL | T3_GATE_TIMER4 | T3_DIV_BY_1 );
	disable_interrupts( INT_TIMER3 );
	
	T3GPOL = 1;
//	T3GTM = 0;
	
	TMR3ON = 0;		//Stops output signal
}

void tone_end()
{
	setup_timer_3(T3_DISABLED);
	setup_ccp5( CCP_OFF );
	setup_ccp3( CCP_OFF );
}


// value is the range of 0..4095
void tone_apply( int16 value )
{
	if ( coil.zero >= value ) {
		TMR3ON = 0;		//Stops timer3 -> stops output signal
		return;
	}
	signed int16 ccp = coil_normalize( value, coil.zero,
								TONE_MAX_CCP-TONE_MIN_CCP );

	ccp = TONE_MAX_CCP-ccp;
	
	int16 safe_timer_value = ccp-1;
	if ( get_timer3() > safe_timer_value ) {
		set_timer3( safe_timer_value );
	}
	
	CCP_5 = ccp;		//Set toggle delay
	CCP_3 = ccp;		//Set reset delay (period)
	
	TMR3ON = 1;
}

