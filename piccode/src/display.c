#include "main.h"
#include "display.h"
#include "lcd.c"
#include "coil.h"

//Time wasted in uP instructions (measured with Proteus-Isis)
//This is the real minimum delay
#define SAMPLE_DELAY_CORRECTION		5		//In us

void dsp_init()
{
	lcd_init();
}


void dsp_clear()
{
	lcd_putc('\f');
}


void dsp_hello()
{
	lcd_putc('\f');
	printf(lcd_putc, "PI METAL DETEC.\n");
	printf(lcd_putc, "A. & G. Corp.");
}


void dsp_setup_coil_pulse_ref(int16 reference_5v)
{
	lcd_putc('\f');
	printf(lcd_putc, "SET: coil pulse\n");
	printf(lcd_putc, "5V ref.: %Lu", reference_5v);
}


void dsp_setup_coil_pulse(int16 measure, int16 reference_5v)
{
	lcd_putc('\f');
	printf(lcd_putc, "SET: coil pulse\n");
	printf(lcd_putc, "%Luus", coil.pulse_length);
	lcd_gotoxy(8, 2);
	int16 volts = (measure * 5) / reference_5v;
	printf(lcd_putc, "-->  %LuV", volts);
}


void dsp_setup_zero_point(int16 min_zero)
{
	int16 max_value = COIL_MAX_ADC_VALUE - min_zero;
	int8 drift = ((int32)(coil.zero-min_zero) * 100) / max_value;
	
	lcd_putc('\f');
	printf(lcd_putc, "SET: zero (%u%%)\n", drift);
	printf(lcd_putc, "%Lu (Min: %Lu)", coil.zero, min_zero);
}


signed int8 dsp_decay_strength( signed int32 sample )
{
	int16 max_value = COIL_MAX_ADC_VALUE - coil.zero;
	
	sample -= coil.zero;
	sample *= 100;
	sample /= max_value;
	
	return sample;
}


void dsp_setup_sample_delay( int16 sample )
{
	lcd_putc('\f');
	printf(lcd_putc, "SET: delay\n");
	printf(lcd_putc, "%uus",
		coil.sample_delay + SAMPLE_DELAY_CORRECTION);
	lcd_gotoxy(7, 2);
	signed int8 strength = dsp_decay_strength(sample);
	printf(lcd_putc, "-->  %d", strength );
}


void dsp_sample( int16 sample )
{
	lcd_putc('\f');
	printf(lcd_putc, "Delay: %uus\n",
		coil.sample_delay + SAMPLE_DELAY_CORRECTION);
	signed int8 strength = dsp_decay_strength(sample);
	printf(lcd_putc, "Strenght: %d", strength);
	
	
}
