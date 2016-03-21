#include "main.h"
#include "pimetaldec.h"

struct PiDetector pi_data;

void pi_init(){
    output_low( PI_COIL_PIN );
    output_drive( PI_COIL_PIN );
}


void pi_doPulse() {
    output_high( PI_COIL_PIN );
    delay_us( 200 );
    output_low( PI_COIL_PIN );
}