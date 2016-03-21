#include "main.h"
#include "pimetaldec.h"

void init() {
    setup_adc_ports(NO_ANALOGS);
    setup_adc(ADC_OFF);

#if getenv("DEVICE")=="PIC24FJ32GA002"
    
#elif getenv("DEVICE")=="PIC24FV16KM202"
    
#endif

    set_tris_a( 0xFFFF );       //All inputs by default
    set_tris_b( 0xFFFF );       //All inputs by default

    pi_init();
    
    enable_interrupts(GLOBAL);
}

void main_loop() {
    while( TRUE ) {
        pi_doPulse();
        
        delay_ms( 10 );
    }
}

void main() {
    init();
    
    
    main_loop();
}
