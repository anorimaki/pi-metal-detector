#include "main.h"
#include "display.h"
#include "lcd.c"

void dsp_init() {
    lcd_init();
}

void dsp_clear() {
	lcd_putc( '\f' );
}

void dsp_hello() {
	lcd_putc( '\f' );
	printf( lcd_putc, "PI METAL DETEC.\n" );
	printf( lcd_putc, "A. & G. Corp." );
}