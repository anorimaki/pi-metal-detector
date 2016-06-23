#ifndef INPUT_H
#define	INPUT_H


//At 16Mhz counter increments every 64us:
//	- For 250 counts, overflows every 16ms
//	- For 78 counts, overflows every 5ms
#define IN_TIMER_INIT_TIME (255-8)
#define IN_TIMER_PERIOD_MS 5

void in_init();

#endif

