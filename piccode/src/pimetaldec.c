#include "main.h"
#include "pimetaldec.h"
#include "config.h"
#include "picconfig.h"

#define MIN_INTEGRATION_RESULT 2
//#define ADC_IN_SLEEP


struct Config config;

void pi_init(){
    IDLEN = 0;
    
    output_low( PI_COIL_CTRL_PIN );
    output_drive( PI_COIL_CTRL_PIN );
    
    output_low( PI_INTEGRATE_CTRL_PIN );
    output_drive( PI_INTEGRATE_CTRL_PIN );
    
    //VREF-: VSS, VREF+: VDD
    setup_adc_ports(PI_INTEGRATION_SIGNAL_PIN|PI_INTEGRATION_SIGNAL_CH, VSS_VDD);
    setup_adc(ADC_OFF);
    disable_interrupts(INT_AD);
    
    
  //  pi_set_start_integration();
}


void pi_set_start_integration() {
    int8 i;
    int8 value;
    
    value = 255;
    for( i=10; (i<50) && (value > MIN_INTEGRATION_RESULT); i+=2 ) {
        pi_coil_pulse();
        delay_us( i );
        value = pi_sample();
    }
    
    config.start_sample_delay = i;
}


void pi_coil_pulse() {
    output_high( PI_COIL_CTRL_PIN );
    delay_us( config.pulse_time );
    output_low( PI_COIL_CTRL_PIN );
}


int8 pi_sample() {
    int16 ret;
    
    set_adc_channel(PI_INTEGRATION_SIGNAL_CH);
                
    output_high( PI_INTEGRATE_CTRL_PIN );       //Start integration
    delay_us( config.sample_time );
    
    //Internal A/D clock must be selected to use A/D in sleep mode (better acuracy)
    //Acquisition Time (TACQT) = TAD*4 = 4us * 4 = 16us
    setup_adc(ADC_CLOCK_INTERNAL|ADC_TAD_MUL_4);
    
#if defined(ADC_IN_SLEEP)
    disable_interrupts(GLOBAL);
    clear_interrupt(INT_AD);
    enable_interrupts(INT_AD);   //We want the ADC to wake from sleep
    read_adc(ADC_START_ONLY);
#asm
    SLEEP   
#endasm
    ret=read_adc(ADC_READ_ONLY);    //Conversion time: 13 TADs -> 13*4 = 52us
    
    disable_interrupts(INT_AD);
    enable_interrupts(GLOBAL);
#else
    ret=read_adc(ADC_START_AND_READ);
#endif
    
    output_low( PI_INTEGRATE_CTRL_PIN );      //Stop integration
      
    setup_adc(ADC_OFF);
    
    ret = 4096 - ret;
    ret = ret * 10;
    ret >>= 6;
    ret *= 10;
    ret >>= 6;

    return ret;
}