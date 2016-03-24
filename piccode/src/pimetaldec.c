#include "main.h"
#include "pimetaldec.h"
#include "picconfig.h"

//#use rs232( UART1, baud=19200, parity=N, bits=8, DISABLE_INTS )

struct PiDetector pi_data;

void pi_init(){
    IDLEN = 0;
    
    output_low( PI_COIL_PIN );
    output_drive( PI_COIL_PIN );
    
    output_low( PI_INTEGRATE_CTRL_PIN );
    output_drive( PI_INTEGRATE_CTRL_PIN );
    
    setup_adc_ports(sAN1, VSS_VDD);
    set_adc_channel(1);
    disable_interrupts(INT_AD);
}


void pi_doPulse() {
    output_high( PI_COIL_PIN );
    delay_us( PULSE_WIDTH_US );
    output_low( PI_COIL_PIN );
}


void pi_integrate() {
    output_high( PI_INTEGRATE_CTRL_PIN );
    delay_us( INTEGRATION_TIME_US );
    output_low( PI_INTEGRATE_CTRL_PIN );
}


int16 pi_sample() {
    int16 ret;
    
    setup_adc(ADC_CLOCK_INTERNAL|ADC_TAD_MUL_4);
    
    disable_interrupts(GLOBAL);
    clear_interrupt(INT_AD);
    enable_interrupts(INT_AD); //We want the ADC to wake from sleep
    
#if 0
    read_adc(ADC_START_ONLY);
#asm
    SLEEP   
#endasm
    ret=read_adc(ADC_READ_ONLY);
#else
    ret=read_adc(ADC_START_AND_READ);
#endif
      
    disable_interrupts(INT_AD);
    enable_interrupts(GLOBAL);
    setup_adc(ADC_OFF);
  
    return ret;
}