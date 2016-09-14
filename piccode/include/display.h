#ifndef DISPLAY_H
#define	DISPLAY_H

void dsp_init();
void dsp_clear();
void dsp_hello();

#define DSP_SHOW_VOLTS      0
void dsp_setup_coil_pulse_init();
void dsp_setup_coil_pulse( int16 measure, int16 reference_5v, int1 mode );

#define DSP_AUTOSET_DELAY_MAX_LINES 4
void dsp_autoset_sample_delay_init();
void dsp_autoset_sample_delay( int8 first, int8 selected, 
                            int16* signals );

void dsp_setup_sample_delay_init();
void dsp_setup_sample_delay( int16 signal, int16 noise_estimation, int1 mode );

void dsp_setup_autozero_threshold_init();
void dsp_setup_autozero_threshold(int16 noise);

#define DSP_SELECTION_PULSE_PERIOD      0
#define DSP_SELECTION_SAMPLES_HISTORY   1
void dsp_setup_response_time_init();
void dsp_setup_response_time( int1 selected );

#define DSP_SHOW_PERCENT    0
#define DSP_SHOW_VALUES     1
void dsp_main_mode_init();
void dsp_main_mode( int16 signal, int16 noise_estimation, int16 battery_volts,
                    int1 mode );

#endif

