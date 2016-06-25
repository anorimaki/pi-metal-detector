#include "main.h"
#include "display.h"
#include "lcd.c"
#include "coil.h"

//Time wasted in uP instructions (measured with Proteus-Isis)
//This is the real minimum delay
#define SAMPLE_DELAY_CORRECTION		4		//In us


int8 CHAR_START_RANGE_GLYPH[8] = { 
	0b10000, 
	0b10000, 
	0b10000, 
	0b11111, 
	0b10000, 
	0b10000, 
	0b10000, 
	0b10000 };

int8 CHAR_END_RANGE_GLYPH[8] = { 
	0b00001, 
	0b00001, 
	0b00001, 
	0b11111, 
	0b00001, 
	0b00001, 
	0b00001, 
	0b00001 };

int8 CHAR_START_RANGE_MID_GLYPH[3][3] = {
	{
	0b11110, 
	0b11111, 
	0b11110 }, 
	{
	0b10111, 
	0b11111, 
	0b10111 },
	{
	0b10000, 
	0b11111, 
	0b10000 } };

int8 CHAR_END_RANGE_MID_GLYPH[3][3] = {
	{
	0b01111, 
	0b11111, 
	0b01111 },
	{
	0b11101, 
	0b11111, 
	0b11101 },
	{	
	0b00001, 
	0b11111, 
	0b00001 } };

int8 CHAR_RANGE_POSITION_GLYPH[3][3] = {{	
	0b11100, 
	0b11111, 
	0b11100 },
	{
	0b01110, 
	0b11111, 
	0b01110 }, 
	{
	0b00111, 
	0b11111, 
	0b00111 }};


		
#define CHAR_START_RANGE	0
#define CHAR_END_RANGE		1
#define CHAR_RANGE_POSITION	2
#define CHAR_FULL_SIGNAL	3
#define CHAR_END_SIGNAL		4

void dsp_set_cgram_address( int8 which, int8 line ) {
	which <<= 3;
	which &= 0x38;		//Begin of glyph
	which |= line;		//Position in glyph

	lcd_send_byte(0, 0x40 | which);  //set cgram address
}


void dsp_sample_write_mid_glyph( int8 which, int8 *ptr ) {
	dsp_set_cgram_address( which, 2 );

	for( int8 i=0; i<3; i++ )
	   lcd_send_byte(1, *ptr++);
}

int8 CHAR_SIGNAL_STRENGTH_GLYPH[6] = { 0b00000, 0b10000, 0b11000, 
										0b11100, 0b11110, 0b11111 };
void dsp_create_signal_character( int8 which, int8 strength ) {
	dsp_set_cgram_address( which, 2 );
	
	for( int8 i=0; i<3; i++ )
	   lcd_send_byte(1, CHAR_SIGNAL_STRENGTH_GLYPH[strength]);
}

void dsp_clear_character( int8 which ) {
	dsp_set_cgram_address( which, 0 );
	for( int8 i=0; i<8; i++ )
	   lcd_send_byte(1, 0);
}


//////////////////////////////////////////////////////////////


void dsp_init()
{
	lcd_init();
	lcd_set_cgram_char( CHAR_START_RANGE, CHAR_START_RANGE_GLYPH );
	lcd_set_cgram_char( CHAR_END_RANGE, CHAR_END_RANGE_GLYPH );
	dsp_clear_character( CHAR_RANGE_POSITION );
	dsp_clear_character( CHAR_FULL_SIGNAL );
	dsp_clear_character( CHAR_END_SIGNAL );
	dsp_create_signal_character( CHAR_FULL_SIGNAL, 5 );
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


void dsp_setup_autozero_threshold( int16 noise )
{
	lcd_putc('\f');
	printf(lcd_putc, "SET: autozero thrs\n");
	printf(lcd_putc, "%Lu (Noise: %Lu)", coil.auto_zero_threshold, noise);
}


void dsp_setup_sample_delay( int16 sample )
{
	lcd_putc('\f');
	printf(lcd_putc, "SET: delay\n");
	printf(lcd_putc, "%uus",
		coil.sample_delay + SAMPLE_DELAY_CORRECTION);
	lcd_gotoxy(7, 2);
	signed int8 strength = coil_normalize(sample, coil.zero, 100);
	printf(lcd_putc, "-->  %d", strength );
}


void dsp_show_zero( int16 min_zero ) 
{
	signed int8 zero_per_cent = coil_normalize(coil.zero, 0, 100);
	signed int8 min_zero_per_cent = coil_normalize(min_zero, 0, 100);
	lcd_putc('\f');
	printf(lcd_putc, "Zero: %Lu (%d%%)\n", coil.zero, zero_per_cent);
	printf(lcd_putc, "Min: %Lu (%d%%)", min_zero, min_zero_per_cent);
}


void dsp_percent( signed int8 percent ) 
{
	if ( percent == -100 ) {
		percent = -99;
	}
	printf(lcd_putc, "%3d%%", percent);
}


// DSP_RANGE_CHARACTERS-2 mid positions with 3 possibilities in each
// 2 possibilities for begin and end range marks
#define DSP_RANGE_CHARACTERS	16
#define DSP_RANGE_POSITIONS		(3*(DSP_RANGE_CHARACTERS-2)) + 2 + 2
void dsp_show_zero_range( int8 line, int1 mode ) {
	int8 mark = coil_normalize(coil.zero, 0, DSP_RANGE_POSITIONS);
	
	int8 start_glyph;
	int8 end_glyph;
	int8 mark_glyph;
	int8 mark_pos;
	if ( mark < 2 ) {								//Mark at start range
		mark_pos = 0;
		mark_glyph = 0xFF;
		start_glyph = mark;
		end_glyph = 2;
	}
	else if ( mark > (DSP_RANGE_POSITIONS-2) ) {	//Mark at end range
		mark_pos = 0;
		mark_glyph = 0xFF;
		start_glyph = 2;
		end_glyph = (DSP_RANGE_POSITIONS-mark);
	}
	else if ( mark == (DSP_RANGE_POSITIONS-2) ) {	//Mark just before end range
		mark_pos = 0;
		mark_glyph = 0xFF;
		start_glyph = 2;
		end_glyph = 1;
	}
	else {											//Mark at mid range
		mark -= 2;
		mark_pos = mark/3;
		mark_glyph = mark-(mark_pos*3);
		start_glyph = 2;
		end_glyph = 2;
	}
	
	dsp_sample_write_mid_glyph( CHAR_START_RANGE, 
								CHAR_START_RANGE_MID_GLYPH[start_glyph] );
	dsp_sample_write_mid_glyph( CHAR_END_RANGE, 
								CHAR_END_RANGE_MID_GLYPH[end_glyph] );
	if ( mark_glyph != 0xFF ) {
		dsp_sample_write_mid_glyph( CHAR_RANGE_POSITION, 
									CHAR_RANGE_POSITION_GLYPH[mark_glyph] );
	}
	
	lcd_gotoxy( 1, line );
	
	int8 i;
	lcd_putc(CHAR_START_RANGE);
	for( i=0; i<mark_pos; ++i ) {
		lcd_putc('-');
	}
	lcd_putc( (mark_glyph==0xFF) ? '-' : CHAR_RANGE_POSITION);
	for( i=mark_pos+1; i<(DSP_RANGE_CHARACTERS-2); ++i ) {
		lcd_putc('-');
	}
	lcd_putc(CHAR_END_RANGE);
	
	if ( mode == DSP_SHOW_PERCENT ) {
		signed int8 percent = coil_normalize(coil.zero, 0, 100);
		dsp_percent( percent );
	}
	else {
		printf(lcd_putc, "%4Ld", coil.zero);
	} 
}


//void dsp_strength_line( int8 y, int8 x, int8 width, )




// STRENGTH_CHARACTERS LCD position
//	6 possibles values per position
#define STRENGTH_CHARACTERS	16
#define STRENGTH_POSITIONS	STRENGTH_CHARACTERS*6
void dsp_show_signal_strength( int8 line, int16 sample, int1 mode )
{
	signed int8 mark = coil_normalize(sample, coil.zero, STRENGTH_POSITIONS);
	if ( mark < 0 ) {
		mark=0;
	}
	int8 mark_pos = mark/6;
	int8 mark_glyph = mark-(mark_pos*6);
	
	dsp_create_signal_character( CHAR_END_SIGNAL, mark_glyph );
	
	lcd_gotoxy( 1, line );
	
	int8 i;
	for( i=0; i<mark_pos; ++i ) {
		lcd_putc(CHAR_FULL_SIGNAL);
	}
	if ( mark_pos != STRENGTH_CHARACTERS ) {
		lcd_putc(CHAR_END_SIGNAL);
	}
	for( i=mark_pos+1; i<STRENGTH_CHARACTERS; ++i ) {
		lcd_putc(' ');
	}
	
	if ( mode == DSP_SHOW_PERCENT ) {
		signed int8 percent = coil_normalize(sample, coil.zero, 100);
		dsp_percent( percent );
	}
	else {
		signed int16 val = (signed int16)sample - coil.zero;
		printf(lcd_putc, "%4Ld", val);
	}
}


void dsp_sample( int16 sample, int1 mode )
{
	dsp_show_zero_range( 3, mode );
	dsp_show_signal_strength( 4, sample, mode );
}
