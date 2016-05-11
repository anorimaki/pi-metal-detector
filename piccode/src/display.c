#include "main.h"
#include "display.h"
#include "lcd.c"
#include "pimetaldec.h"

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

void dsp_data( int16 value ) {
	lcd_putc( '\f' );
	printf( lcd_putc, "P: %Lu", config.pulse_time );
    lcd_gotoxy( 10, 1 );
    printf( lcd_putc, "S: %u\n", config.start_sample_delay );
	printf( lcd_putc, "I: %u", config.sample_time );
    lcd_gotoxy( 10, 2 );
    printf( lcd_putc, "V: %Lu\n", value );
}