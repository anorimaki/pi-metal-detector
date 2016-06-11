#include "main.h"
#include "tone.h"
#include "coil.h"

#define TONE_MIN_FREQUENCY	100		//In Hz
#define TONE_MAX_FREQUENCY	9000	//In Hz

// (1Mhz instruction frequency && T1_DIV_BY_8)
#define CPP_TIMER_PERIOD		8			//In us  



void tone_init()
{
#if 1
		//Just to configure CCP1 but not to activate it. See comments of
		// tone_being() function.
	setup_ccp1( CCP_COMPARE_INT_AND_TOGGLE | CCP_USE_TIMER1_AND_TIMER2 );
	setup_ccp1( CCP_OFF );
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
	// CCP1 generates output signal
	//setup_ccp1( CCP_COMPARE_INT_AND_TOGGLE | CCP_USE_TIMER1_AND_TIMER2 );
#asm
		//setup_ccp1() sould be here but 'MOVWF  PSTR1CON' of this function
		//resets pull-ups resistors configuration. setup_ccp1 has been
		//moved to tone_init() function and must be called before pull-ups 
		//resistors are configured
		//Here, CPP1 is only activated with previous configuration
	MOVLW  CCP_COMPARE_INT_AND_TOGGLE
	MOVWF  CCP1CON		//Compare mode, toggle output on match
#endasm
	
	
	// CCP2 reset timer to create a period (Special Event Trigger). 
	// It avoid the use of interrupt ISR
	// Note: Do not use CCP1 for Special Event Trigger or ADC will be started
	setup_ccp2( CCP_COMPARE_RESET_TIMER | CCP_USE_TIMER1_AND_TIMER2 );
	
	//At 1Mhz instruction frequency (ClockF/4):
	//	- It increments every: 8us
	//	- Min frequency selectable is: 1/8us * 16^2 * 2 ->
	//	- Max frequency selectable is: 1/8us * 2 -> 
	setup_timer_1( T1_INTERNAL| T1_DIV_BY_8 );
	
	TMR1ON = 0;		//Stops output signal
}

void tone_end()
{
	setup_timer_1(T1_DISABLED);
	setup_ccp1( CCP_OFF );
	setup_ccp2( CCP_OFF );
}


// value is the range of 0..4095
void tone_apply( int16 value )
{
	signed int16 freq = coil_normalize( value, 
						TONE_MAX_FREQUENCY-TONE_MIN_FREQUENCY );
	if ( freq <= 0 ) {
		TMR1ON = 0;		//Stops timer1 -> stops output signal
		return;
	}
	
	freq += TONE_MIN_FREQUENCY;
	
	int16 ccp = (int32)250000/freq;			//1000000*2/8 = 250000
	CCP_1 = ccp;		//Set toggle delay
	CCP_2 = ccp;		//Set reset delay (period)
	
	TMR1ON = 1;
}
