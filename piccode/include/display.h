/* 
 * File:   display.h
 * Author: David
 *
 * Created on 30 de abril de 2016, 15:31
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

void dsp_init();
void dsp_clear();
void dsp_hello();

void dsp_setup_coil_pulse_ref( int16 reference_5v );
void dsp_setup_coil_pulse(int16 measure, int16 reference_5v);

void dsp_setup_sample_delay( int16 sample );

void dsp_setup_zero_point(int16 min_zero);

void dsp_sample( int16 sample );

#endif	/* DISPLAY_H */

