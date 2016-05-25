#include "main.h"
#include "display.h"
#include "lcd.c"
#include "coil.h"

//Time wasted in uP instructions (measured with Proteus-Isis)
//This is the real minimum delay
#define SAMPLE_DELAY_CORRECTION		5		//In us


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


void dsp_setup_coil_pulse_ref( int16 reference_5v ) {
	lcd_putc( '\f' );
	printf( lcd_putc, "SET: coil pulse\n" );
	printf( lcd_putc, "5V ref.: %Lu", reference_5v );
}


void dsp_setup_coil_pulse( int16 coil_volts ) {
	lcd_putc( '\f' );
	printf( lcd_putc, "SET: coil pulse\n" );
	printf( lcd_putc, "%Luus", pi.pulse_time );
	lcd_gotoxy( 8, 2 );
	printf( lcd_putc, "-->  %LuV", coil_volts );
}


void dsp_setup_sample_delay( signed int8 strength ) {
	lcd_putc( '\f' );
	printf( lcd_putc, "SET: delay\n" );
	printf( lcd_putc, "%uus", 
		 pi.start_sample_delay + SAMPLE_DELAY_CORRECTION );
	lcd_gotoxy( 7, 2 );
	printf( lcd_putc, "-->  %d", strength );
}


void dsp_setup_zero_point( int16 min_zero ) {
	lcd_putc( '\f' );
	
	int8 drift = ((int32)pi.sample_zero_point * 100) / min_zero;
	printf( lcd_putc, "SET: zero (%u%%)\n", drift );
	
	printf( lcd_putc, "%Lu (Min: %Lu)", pi.sample_zero_point, min_zero );
}


void dsp_sample( signed int8 strength ) {
	lcd_putc( '\f' );
	printf( lcd_putc, "Delay: %uus\n", 
		 pi.start_sample_delay + SAMPLE_DELAY_CORRECTION );
	printf( lcd_putc, "Strenght: %d", strength );
}
