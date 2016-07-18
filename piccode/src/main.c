#include "main.h"
#include "coil.h"
#include "display.h"
#include "input.h"
#include "inbuttons.h"
#include "config.h"
#include "usermodes.h"
#include "tone.h"

#use rs232( UART2, baud=9600, parity=N, bits=8 )

void init() {
    setup_adc_ports(NO_ANALOGS);
    setup_adc(ADC_OFF);

    set_tris_a( 0xFFFF );      //All inputs by default
    set_tris_b( 0xFFFF );      //All inputs by default
    set_tris_c( 0xFFFF );      //All inputs by default
    
    dsp_init();
	tone_init();
    coil_init();
    in_init();
	buttons_init();
	mode_init();
    
    enable_interrupts(GLOBAL);
}


void main()
{
    init();
    
    dsp_hello();
//	delay_ms(2000);
	
    cnf_load();
	
	while( TRUE ) {
		mode_execute_current();
	}
}


#if 1
#include "adconvert.c"
#include "config.c"
#include "display.c"
#include "input.c"
#include "inencoder.c"
#include "inbuttons.c"
#include "coil.c"
#include "modesetup.c"
#include "samples.c"
#include "tone.c"
#include "mathutil.c"
#include "modesetdelay.c"
#include "usermodes.c"
#include "battery.c"
#endif
