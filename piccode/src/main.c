#include "main.h"
#include "pimetaldec.h"

#use rs232( UART1, baud=9600, parity=N, bits=8 )

void init() {
    setup_adc_ports(NO_ANALOGS);
    setup_adc(ADC_OFF);

#if getenv("DEVICE")=="PIC24FJ32GA002"
    
#elif getenv("DEVICE")=="PIC24FV16KM202"
    
#endif

    set_tris_a( 0xFFFF );      //All inputs by default
    set_tris_b( 0xFFFF );      //All inputs by default
    
    output_drive( PIN_C6 );    //UART TX

    pi_init();
    
    enable_interrupts(GLOBAL);
}


void main_loop() {
    int16 sample;
            
    while( TRUE ) {
        pi_doPulse();
        delay_us( INTEGRATION_START_US );
        pi_integrate();
        sample = pi_sample();
        
        delay_ms( 5 );
        printf( "Sample: %Ld\r\n", sample );
    }
}


void main() {
    init();
    main_loop();
}
