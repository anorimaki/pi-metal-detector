#include "main.h"
#include "display.h"
#include "mathutil.h"
#include "modesetup.h"
#include "lcd.c"
#include "coil.h"


#define CHAR_FULL_STRENGTH			6
//#define CHAR_START_RANGE			5
//#define CHAR_END_RANGE				4
//#define CHAR_MID_RANGE				3

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


void dsp_set_cgram_address( int8 which, int8 line ) {
	which <<= 3;
	which &= 0x38;		//Begin of glyph
	which |= line;		//Position in glyph

	lcd_send_byte(0, 0x40 | which);  //set cgram address
}


void dsp_write_mid_glyph( int8 which, int8 *ptr ) {
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
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


void dsp_init()
{
	lcd_init();
	
	for( int8 i=0; i<8; ++i ) {
		dsp_clear_character( i );
	}
	
//	lcd_set_cgram_char( CHAR_START_RANGE, CHAR_START_RANGE_GLYPH );
 //   lcd_set_cgram_char( CHAR_END_RANGE, CHAR_END_RANGE_GLYPH );
	dsp_create_signal_character( CHAR_FULL_STRENGTH, 5 );
}


void dsp_clear()
{
	lcd_putc('\f');
}


void dsp_hello()
{
	lcd_gotoxy( 1, 1 );
	printf( lcd_putc, "********************");
	
	lcd_gotoxy( 1, 2 );
	printf( lcd_putc, " PI METAL DETECTOR  ");
	
	lcd_gotoxy( 1, 3 );
	printf( lcd_putc, "A. & G. Corp. (2016)");
	
	lcd_gotoxy( 1, 4 );
	printf( lcd_putc, "********************");
}



void dsp_percent( signed int8 percent ) 
{
	if ( percent == -100 ) {
		percent = -99;
	}
	printf(lcd_putc, "%3d%%", percent);
}

/*
 * Display a range bar with:
 *	- 3 possibles values per position in mid characters
 *	- 2 possibles values per position in start and end characters
 * Note: Only one range line is allowed.
 */
void dsp_range_line( int8 start_char_index, 
					int8 mid_char_index, 
					int8 end_char_index, 
					int8 width, int16 value, 
					int16 max_value ) 
{
	int8 range_positions = (3*(width-2)) + 2 + 2;
	int8 mark = math_change_range(value, max_value, range_positions);
	
	int8 start_glyph;
	int8 end_glyph;
	int8 mark_glyph;
	int8 mark_pos;
	if ( mark < 2 ) {							//Mark at start range
		mark_pos = 0;
		mark_glyph = 0xFF;
		start_glyph = mark;
		end_glyph = 2;
	}
	else if ( mark > (range_positions-2) ) {	//Mark at end range
		mark_pos = 0;
		mark_glyph = 0xFF;
		start_glyph = 2;
		end_glyph = (range_positions-mark);
	}
	else if ( mark == (range_positions-2) ) {	//Mark just before end range
		mark_pos = 0;
		mark_glyph = 0xFF;
		start_glyph = 2;
		end_glyph = 1;
	}
	else {										//Mark at mid range
		mark -= 2;
		mark_pos = mark/3;
		mark_glyph = mark-(mark_pos*3);
		start_glyph = 2;
		end_glyph = 2;
	}
	
	lcd_output_rs(0);
	int8 current_address = lcd_read_byte() & 0x7F;
	
				// Set glyphs
	dsp_write_mid_glyph( start_char_index, 
								CHAR_START_RANGE_MID_GLYPH[start_glyph] );
	dsp_write_mid_glyph( end_char_index, 
								CHAR_END_RANGE_MID_GLYPH[end_glyph] );
	if ( mark_glyph != 0xFF ) {
		dsp_write_mid_glyph( mid_char_index, 
									CHAR_RANGE_POSITION_GLYPH[mark_glyph] );
	}
	
	lcd_send_byte( 0, 0x80|current_address );	//Restore DDRAM address
	
				// Write range line
	int8 i;
	lcd_putc(start_char_index);
	for( i=0; i<mark_pos; ++i ) {
		lcd_putc('-');
	}
	lcd_putc( (mark_glyph==0xFF) ? '-' : mid_char_index);
	for( i=mark_pos+1; i<(width-2); ++i ) {
		lcd_putc('-');
	}
	lcd_putc(end_char_index);
}



/*
 * Display a stength bar with 6 possibles values per position
 * Multiples strength bars can be displayed in LCD at same time but
 * end_signal_char_index must be different in each one (is the glyph of last
 * character)
 */
#define STRENGTH_VALUES_PER_CHAR	6
void dsp_strength_bar( int8 end_signal_char_index,
						int8 width, int8 value ) 
{
	int8 mark_pos = value/STRENGTH_VALUES_PER_CHAR;
	int8 mark_glyph = value-(mark_pos*STRENGTH_VALUES_PER_CHAR);
	
	lcd_output_rs(0);
	int8 current_address = lcd_read_byte() & 0x7F;
	
	dsp_create_signal_character( end_signal_char_index, mark_glyph );
	
	lcd_send_byte( 0, 0x80|current_address );	//Restore DDRAM address
	
	int8 i;
	for( i=0; i<mark_pos; ++i ) {
		lcd_putc(CHAR_FULL_STRENGTH);
	}
	if ( mark_pos != width ) {
		lcd_putc(end_signal_char_index);
	}
	for( i=mark_pos+1; i<width; ++i ) {
		lcd_putc(' ');
	}
}


void dsp_prop_strength_bar( int8 end_signal_char_index,
						int8 width, int16 value, 
						int16 max_value ) 
{
	value = math_change_range(value, max_value, 
							width*STRENGTH_VALUES_PER_CHAR);
	dsp_strength_bar( end_signal_char_index, width, value );
}


void dsp_log_strength_bar( int8 end_signal_char_index,
						int8 width, int16 value, 
						int16 max_value ) 
{
	value = math_change_log_range(value, max_value, 
							width*STRENGTH_VALUES_PER_CHAR);
	dsp_strength_bar( end_signal_char_index, width, value );
}


void dis_signal( int16 signal, int1 mode ) 
{
	signed int16 adjusted_signal = signal - coil.zero;
	int16 adjusted_max_signal = COIL_MAX_ADC_VALUE - coil.zero;
	
	int16 signal_strength_bar = (adjusted_signal<0) ? 0 : adjusted_signal;
	dsp_log_strength_bar( 0, 12, signal_strength_bar, adjusted_max_signal );
	if ( mode == DSP_SHOW_PERCENT ) {
		signed int8 percent = math_change_log_range(adjusted_signal, 
												adjusted_max_signal, 100);
		dsp_percent( percent );
	}
	else {
		printf(lcd_putc, "%4Ld", adjusted_signal);
	}
}


void dis_noise( int16 noise_estimation, int1 mode ) 
{
	noise_estimation &= 0x7FFF;		//Limit noise to max positive sign int16
	
	printf( lcd_putc, "Noi " );
	dsp_prop_strength_bar( 1, 12, noise_estimation, COIL_MAX_ADC_VALUE );
	if ( mode == DSP_SHOW_PERCENT ) {
		signed int8 percent = math_change_range(noise_estimation, 
												COIL_MAX_ADC_VALUE, 100);
		dsp_percent( percent );
	}
	else {
		printf(lcd_putc, "%4Ld", noise_estimation);
	}
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

void dsp_main_mode_init()
{
	dsp_clear_character( 0 );		//Signal char
	dsp_clear_character( 1 );		//Noise char
	lcd_set_cgram_char( 2, CHAR_START_RANGE_GLYPH );
	dsp_clear_character( 3 );
    lcd_set_cgram_char( 4, CHAR_END_RANGE_GLYPH );
}

void dsp_main_mode( int16 signal, int16 noise_estimation, int16 battery_volts,
					int1 mode )
{
	lcd_gotoxy( 1, 1 );
	printf( lcd_putc, "* Main mode * %5.2wV", battery_volts);
	
	lcd_gotoxy( 1, 2 );
	printf( lcd_putc, "Zer " );
	dsp_range_line( 2, 3, 4, 12, coil.zero, COIL_MAX_ADC_VALUE );
	if ( mode == DSP_SHOW_PERCENT ) {
		signed int8 percent = math_change_range(coil.zero, 
												COIL_MAX_ADC_VALUE, 100);
		dsp_percent( percent );
	}
	else {
		printf(lcd_putc, "%4Ld", coil.zero);
	}
	
	lcd_gotoxy( 1, 3 );
	printf( lcd_putc, "Sig " );
	dis_signal( signal, mode );
	
	lcd_gotoxy( 1, 4 );
	dis_noise( noise_estimation, mode );
}


void dsp_setup_sample_delay_init()
{
	dsp_clear_character( 0 );		//Signal char
	dsp_clear_character( 1 );		//Noise char
	lcd_set_cgram_char( 2, CHAR_START_RANGE_GLYPH );
	dsp_clear_character( 3 );
    lcd_set_cgram_char( 4, CHAR_END_RANGE_GLYPH );
}


void dsp_setup_sample_delay( int16 signal, int16 noise_estimation, int1 mode )
{
	lcd_gotoxy( 1, 1 );
	printf( lcd_putc, " *** Setup delay ***");
	
	lcd_gotoxy( 1, 2 );
	printf( lcd_putc, "Del " );
	dsp_range_line( 2, 3, 4, 12, 
				coil.sample_delay-COIL_MIN_SAMPLE_DELAY, 
				COIL_MAX_SAMPLE_DELAY-COIL_MIN_SAMPLE_DELAY );
	printf(lcd_putc, "%2uus",
				coil.sample_delay + COIL_SAMPLE_DELAY_CORRECTION);
	
	lcd_gotoxy( 1, 3 );
	printf( lcd_putc, "Sig " );
	dis_signal( signal, mode );
	
	lcd_gotoxy( 1, 4 );
	dis_noise( noise_estimation, mode );
}


void dsp_autoset_sample_delay_init()
{
	for( int8 i=0; i<DSP_AUTOSET_DELAY_MAX_LINES; ++i ) {
		dsp_clear_character( i );
	}
}


void dsp_autoset_sample_delay( int8 first, int8 selected, 
                            int16* signals )
{
	for( int8 line=0; line<DSP_AUTOSET_DELAY_MAX_LINES; ++line ) {
		lcd_gotoxy( 1, line+1 );
		lcd_putc( (selected==line) ? '>' : ' ' );
		
		printf(lcd_putc, "%2u ", first+line+COIL_SAMPLE_DELAY_CORRECTION );
		
		int16 signal = signals[line];
		dsp_prop_strength_bar( line, 12, signal, COIL_MAX_ADC_VALUE );
		
		printf(lcd_putc, "%4Ld", signal);
	}
}


void dsp_setup_coil_pulse_init()
{
	dsp_clear_character( 0 );		//Volts
	lcd_set_cgram_char( 2, CHAR_START_RANGE_GLYPH );
	dsp_clear_character( 3 );
    lcd_set_cgram_char( 4, CHAR_END_RANGE_GLYPH );
}


void dsp_setup_coil_pulse(int16 measure, int16 reference_5v, int1 mode)
{
	lcd_gotoxy( 1, 1 );
	printf( lcd_putc, "* Setup pulse time *");
	
	lcd_gotoxy( 1, 2 );
	printf( lcd_putc, "Pul " );
	dsp_range_line( 2, 3, 4, 11, 
				 coil.pulse_length-COIL_MIN_PULSE_LENGTH,
				 COIL_MAX_PULSE_LENGTH-COIL_MIN_PULSE_LENGTH );
	printf(lcd_putc, "%3Luus", coil.pulse_length +
								COIL_PULSE_LENGTH_CORRECTION);
	
	lcd_gotoxy( 1, 3 );
	printf( lcd_putc, "Vol " );
	dsp_prop_strength_bar( 0, 12, measure, COIL_MAX_ADC_VALUE );
	if ( mode == DSP_SHOW_VOLTS ) {
		int16 volts = (measure * 5) / reference_5v;
		if ( volts > 999 ) {
			volts = 999;
		}
		printf(lcd_putc, "%3LuV", volts);
	}
	else {
		printf(lcd_putc, "%4Lu", measure);
	}
	
	lcd_gotoxy( 1, 4 );
	printf(lcd_putc, "5V reference:   %4Lu", reference_5v);
}


void dsp_setup_autozero_threshold_init()
{
	dsp_clear_character( 0 );		//Noise
	lcd_set_cgram_char( 2, CHAR_START_RANGE_GLYPH );
	dsp_clear_character( 3 );
    lcd_set_cgram_char( 4, CHAR_END_RANGE_GLYPH );
}


void dsp_setup_autozero_threshold( int16 noise )
{
	lcd_gotoxy( 1, 1 );
	printf( lcd_putc, "** Setup autozero **");
	
	lcd_gotoxy( 1, 2 );
	printf( lcd_putc, "Thr " );
	dsp_range_line( 2, 3, 4, 12, coil.auto_zero_threshold, 
					MODE_SETUP_AUTOZERO_THRESHOLD_MAX );
	printf( lcd_putc, "%4Lu", coil.auto_zero_threshold );
	
	lcd_gotoxy( 1, 3 );
	printf( lcd_putc, "Noi " );
	dsp_prop_strength_bar( 0, 12, noise, COIL_MAX_ADC_VALUE );
	printf( lcd_putc, "%4Lu", noise );
	
	lcd_gotoxy( 1, 4 );
	printf( lcd_putc, "Zero relation:  %4Lu", coil.zero /
								coil.auto_zero_threshold );
}


void dsp_setup_response_time_init()
{				
					//Pulse frequency range line
	lcd_set_cgram_char( 0, CHAR_START_RANGE_GLYPH );
	dsp_clear_character( 1 );
    lcd_set_cgram_char( 2, CHAR_END_RANGE_GLYPH );
	
					//Samples history range line
	lcd_set_cgram_char( 3, CHAR_START_RANGE_GLYPH );
	dsp_clear_character( 4 );
    lcd_set_cgram_char( 5, CHAR_END_RANGE_GLYPH );
}



#define DSP_PULSE_PERIOD_US_LIMIT	(10000/COIL_PULSE_PERIOD_STEP)
#define DSP_PULSE_PERIOD_MS_FACTOR	(10-COIL_PULSE_PERIOD_STEP_LOG)

#define DSP_TIME_UNIT_US	0
#define DSP_TIME_UNIT_MS	1

struct 
{
	int16 value;
	int1 unit;
} dsp_coil_period_step_to_time_data;

void dsp_coil_period_step_to_time( int32 steps ) 
{
	if ( steps > DSP_PULSE_PERIOD_US_LIMIT )
	{
		dsp_coil_period_step_to_time_data.unit = DSP_TIME_UNIT_MS;
		
		//Should be steps*64/1000 but steps*64/1024 it's a good approximation.
		//steps*64/1024 = coil.steps*2^6/2^10 = steps*2^(10-6)
		dsp_coil_period_step_to_time_data.value =
							steps>>DSP_PULSE_PERIOD_MS_FACTOR;
		dsp_coil_period_step_to_time_data.unit = DSP_TIME_UNIT_MS;
		return;
	}
	
	dsp_coil_period_step_to_time_data.value = 
							steps<<COIL_PULSE_PERIOD_STEP_LOG;
	dsp_coil_period_step_to_time_data.unit = DSP_TIME_UNIT_US;
}

#define DSP_PRINT_TIME_UNIT_STR(ch, v) \
		if (v==DSP_TIME_UNIT_MS) printf( lcd_putc, "ms" ); \
		else printf( lcd_putc, "us" );

void dsp_setup_response_time( int1 selected )
{
	lcd_gotoxy( 1, 1 );
	printf( lcd_putc, "** Setup response **");
	
	lcd_gotoxy( 1, 2 );
	lcd_putc( (selected == DSP_SELECTION_PULSE_PERIOD) ? '>' : ' ' );
	printf( lcd_putc, "Per" );
	dsp_range_line( 0, 1, 2, 10, 
				coil.pulse_period-COIL_MIN_PULSE_PERIOD_COUNT, 
				COIL_MAX_PULSE_PERIOD_COUNT-COIL_MIN_PULSE_PERIOD_COUNT );
	dsp_coil_period_step_to_time( coil.pulse_period );
	printf( lcd_putc, "%4Lu", dsp_coil_period_step_to_time_data.value );
	DSP_PRINT_TIME_UNIT_STR( lcd_putc, dsp_coil_period_step_to_time_data.unit );
		
	lcd_gotoxy( 1, 3 );
	lcd_putc( (selected == DSP_SELECTION_SAMPLES_HISTORY) ? '>' : ' ' );
	printf( lcd_putc, "His" );
	dsp_range_line( 3, 4, 5, 10, 
				coil.samples_history_size_log-SAMPLES_HISTORY_MIN_SIZE_LOG, 
				SAMPLES_HISTORY_MAX_SIZE_LOG-SAMPLES_HISTORY_MIN_SIZE_LOG );
	printf( lcd_putc, "  %4u", 1 << coil.samples_history_size_log );
	
	lcd_gotoxy( 1, 4 );
	int32 response_steps = coil.pulse_period << coil.samples_history_size_log;
	dsp_coil_period_step_to_time( response_steps );
	printf( lcd_putc, "Resp. time: %4Lu", 
			dsp_coil_period_step_to_time_data.value );
	DSP_PRINT_TIME_UNIT_STR( lcd_putc, dsp_coil_period_step_to_time_data.unit );
	printf( lcd_putc, "  " );
}
