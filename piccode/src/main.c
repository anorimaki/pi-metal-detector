#include "main.h"
#include "pimetaldec.h"
#include "display.h"

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
    dsp_init();
    
    enable_interrupts(GLOBAL);
}


void main_loop() {
    int16 sample;
    
    while( TRUE ) {
        pi_coil_pulse();
        delay_us( pi_data.integration_start );
        sample = pi_sample();
        
        delay_ms( 50 );
        
    }
}


void main() {
    init();
    dsp_hello();
    main_loop();
}
