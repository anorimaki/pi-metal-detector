#ifndef INPUT_H
#define	INPUT_H


//At 16Mhz counter increments every 64us:
//	- For 250 counts, overflows every 16ms
//	- For 8 counts, overflows every 512us
#define IN_TIMER_INIT_TIME (256-8)
#define IN_ENCODER_SCAN_PERIOD_US 512
#define IN_BUTTONS_SCAN_PERIOD_MS 4     //512us*8

void in_init();

#endif

