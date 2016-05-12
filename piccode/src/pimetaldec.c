#include "main.h"
#include "pimetaldec.h"
#include "config.h"
#include "picconfig.h"

#define MIN_INTEGRATION_RESULT 2
//#define ADC_IN_SLEEP


struct Config pi;

void pi_init(){
    IDLEN = 0;      //Enter in Sleep mode if SLEEP is executed
    
    output_low( PI_COIL_CTRL_PIN );
    output_drive( PI_COIL_CTRL_PIN );
    
    output_low( PI_INTEGRATE_CTRL_PIN );
    output_drive( PI_INTEGRATE_CTRL_PIN );
    
    //VREF-: VSS, VREF+: VDD
    setup_adc_ports(PI_INTEGRATION_SIGNAL_PIN|PI_INTEGRATION_SIGNAL_CH, VSS_VDD);
    setup_adc(ADC_OFF);
    disable_interrupts(INT_AD);

}


/*
* Precondition: interrupts sould be disabled for good timing confidence
*/
void pi_coil_pulse() {
    output_high( PI_COIL_CTRL_PIN );
    delay_us( pi.pulse_time );
    output_low( PI_COIL_CTRL_PIN );
}


int16 a2d_converter() {
#if defined(ADC_IN_SLEEP)
    //Internal A/D clock must be selected to use A/D in sleep mode (better acuracy)
    //Acquisition Time (TACQT) = TAD*2 = 4us * 2 = 8us (Could be 4us?)
    setup_adc(ADC_CLOCK_INTERNAL|ADC_TAD_MUL_2);
    
    clear_interrupt(INT_AD);
    enable_interrupts(INT_AD);   	//We want the ADC to wake from sleep
    read_adc(ADC_START_ONLY);
#asm
    SLEEP   
#endasm
    ret=read_adc(ADC_READ_ONLY);    //Conversion time: 13 TADs -> 13*4 = 52us
    disable_interrupts(INT_AD);
#else
    //TAD: 1us for a 16Mhz clock
    //Acquisition Time (TACQT) = TAD*4 = 1us * 4 = 4us
    setup_adc(ADC_CLOCK_DIV_16|ADC_TAD_MUL_4);
    return read_adc(ADC_START_AND_READ); //Conv. time: 13 TADs -> 13*1 = 13us
#endif   
}

/*
* Precondition: interrupts sould be disabled for good timing confidence
*/
int8 pi_sample() {
    int16 ret;
    
    set_adc_channel(PI_INTEGRATION_SIGNAL_CH);
                
    output_high( PI_INTEGRATE_CTRL_PIN );       //Start integration
    delay_us( pi.sample_time );
    
    ret = a2d_converter();
    
    output_low( PI_INTEGRATE_CTRL_PIN );      //Stop integration
      
    setup_adc(ADC_OFF);
    
    ret = 4096 - ret;
    ret = ret * 10;
    ret >>= 6;
    ret *= 10;
    ret >>= 6;

    return ret;
}


int16 pi_read_peak_coil_volts() {
    int16 ret;
   
    set_adc_channel(PI_COIL_VOLTAGE_CH);
    ret = a2d_converter();
    setup_adc(ADC_OFF);
    
    return ret;
}

/*
 * Returns value in vols
 */
int16 pi_read_peak_coil_volts( int16 reference_5v ) {
    int16 ret;
   
         //Disable interrupts for good timing accuracy.
    disable_interrupts(GLOBAL);
    
    pi_coil_pulse();
    ret = pi_read_peak_coil_volts();
				
		//Enable interrupts to capture read input switches
    disable_interrupts(GLOBAL);
    
    return (ret*5) / reference_5v;
}