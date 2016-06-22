#ifndef COIL_H
#define COIL_H

#define COIL_MAX_ADC_VALUE              4095

///http://www.whiteselectronics.com/media/wysiwyg/SurfPI_SurfMII_FieldEval_EngReport.pdf
// 'Pi detectors tipically operate in the range of 100 pulses per second'
//
// http://chemelec.com/Projects/Metal-AV/Metal-AV.htm
// 'The Present Frequency of Operation is 300 Hz.'


#define COIL_PULSE_PERIOD               7      //In ms
#define COIL_MAX_SAMPLE_DELAY           50		//In us
#define COIL_MIN_SAMPLE_DELAY           0		//in us
#define COIL_CALCULATE_MIN_ZERO_DELAY   COIL_MAX_SAMPLE_DELAY*4		//in us

struct Coil {
    int16 pulse_length;             //In us
    int8 sample_delay;              //In us after coil pulse ends
    int16 zero;                     //In ADC units
    int16 auto_zero_threshold;      //In ADC units (should be set based on 
                                    //  the environment noise)
};

extern struct Coil coil;

void coil_init();

int16 coil_peak_ref();
int16 coil_peak();

int16 coil_sample();
int16 coil_custom_sample( int8 delay, int8 n );

signed int32 coil_normalize( int16 sample, int16 zero, int16 new_max_value );

#endif