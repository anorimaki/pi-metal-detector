#include "main.h"
#include "tone.h"

void tone_begin()
{
	setup_ccp1( CCP_PWM );
}

void tone_end()
{
	setup_ccp1( CCP_OFF );
}

void tone_apply( int16 value )
{
}
