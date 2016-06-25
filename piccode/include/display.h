#ifndef DISPLAY_H
#define	DISPLAY_H

void dsp_init();
void dsp_clear();
void dsp_hello();

void dsp_setup_coil_pulse_ref( int16 reference_5v );
void dsp_setup_coil_pulse(int16 measure, int16 reference_5v);

void dsp_setup_sample_delay( int16 signal, int16 noise_estimation, int1 mode );

void dsp_show_zero( int16 min_zero );

void dsp_setup_autozero_threshold(int16 noise);

#define DSP_SHOW_PERCENT    0
#define DSP_SHOW_VALUES     1
void dsp_main_mode( int16 signal, int16 noise_estimation, int1 mode );

#endif

