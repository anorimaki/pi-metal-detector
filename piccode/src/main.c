#include "main.h"

void init() {
    setup_adc_ports(NO_ANALOGS);
    setup_adc(ADC_OFF);

#ifdef USE_PIC24FJ32GA002
#endif

    set_tris_a(0xFF);
    set_tris_b(0xFF);

    enable_interrupts(GLOBAL);
}

void main_loop() {
}

void main() {
    init();

    main_loop();
}

