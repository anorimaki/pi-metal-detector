#ifndef INPUT_H
#define	INPUT_H

#define IN_ENCODER_SCAN_PERIOD_US 2000
#define IN_BUTTONS_SCAN_PERIOD_MS ((IN_ENCODER_SCAN_PERIOD_US*8)/1000) 

void in_init();

#endif

