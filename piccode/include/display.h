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
void dsp_setup_coil_pulse( int16 coil_volts );
void dsp_sample( int8 sample1_strength, int8 sample2_strength );

#endif	/* DISPLAY_H */

