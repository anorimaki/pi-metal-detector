#include "main.h"
#include "pimetaldec.h"

struct PiDetector pi_data;

void pi_init(){
    output_low( PI_COIL_PIN );
    output_drive( PI_COIL_PIN );
}


void pi_doPulse() {
    
}