#ifndef COIL_H
#define COIL_H

#define COIL_MAX_ADC_VALUE              4095

//Time spent in microcontroller instructions (measured with Proteus-Isis). This
// is the time between PI_COIL_CTRL_PIN goes down and ADC starts minus the 
// programed time in Timer1 (ADC starts with Timer1 Special Event Trigger).
//Most of the instructions are in ADC channel selection, ADC turn on and Timer1
// value calculation.
#define COIL_SAMPLE_DELAY_CORRECTION	6		//In us 

//Times in us with COIL_SAMPLE_DELAY_CORRECTION
#define COIL_MAX_SAMPLE_DELAY           80-COIL_SAMPLE_DELAY_CORRECTION		//In us
#define COIL_MIN_SAMPLE_DELAY           1		//in us

#define COIL_CALCULATE_MIN_ZERO_DELAY   COIL_MAX_SAMPLE_DELAY*4		//in us

//Time spent in microcontroller instructions (measured with Proteus-Isis). This
// is the time between PI_COIL_CTRL_PIN goes up and PI_COIL_CTRL_PIN goes down 
// minus the programed time in Timer1.
//Most of the instructions are in generated PIC-C code for interrupt
//dispatcher (save registers and discover specific interrupt).
#define COIL_PULSE_LENGTH_CORRECTION    11		//in us

//Times in us with COIL_PULSE_LENGTH_CORRECTION
#define COIL_MAX_PULSE_LENGTH   (500-COIL_PULSE_LENGTH_CORRECTION) //In us
#define COIL_MIN_PULSE_LENGTH   (20-COIL_PULSE_LENGTH_CORRECTION)  //In us


///http://www.whiteselectronics.com/media/wysiwyg/SurfPI_SurfMII_FieldEval_EngReport.pdf
// 'Pi detectors tipically operate in the range of 100 pulses per second'
//
// http://chemelec.com/Projects/Metal-AV/Metal-AV.htm
// 'The Present Frequency of Operation is 300 Hz.'
#define COIL_MAX_PULSE_PERIOD           1000000	//In us
#define COIL_MIN_PULSE_PERIOD           600		//In us
#define COIL_PULSE_PERIOD_STEP_LOG		6
#define COIL_PULSE_PERIOD_STEP  \
			(1<<COIL_PULSE_PERIOD_STEP_LOG)		//In us (timer0 period, 64us)
#define COIL_MAX_PULSE_PERIOD_COUNT	\
			(COIL_MAX_PULSE_PERIOD/COIL_PULSE_PERIOD_STEP)
#define COIL_MIN_PULSE_PERIOD_COUNT	\
			(COIL_MIN_PULSE_PERIOD/COIL_PULSE_PERIOD_STEP)

//Fixed values for read peak coil voltages
#define COIL_READ_PEAK_PULSE_PERIOD		20000       //In us
#define COIL_READ_PEAK_PULSE_PERIOD_COUNT	\
           (COIL_READ_PEAK_PULSE_PERIOD/COIL_PULSE_PERIOD_STEP)
#define COIL_READ_PEAK_SAMPLE_DEPLAY	1			//In us

struct CoilResult 
{
    int16 value;
    int16 noise;
};

struct Coil {
    int8 samples_history_size_log; 
    int16 pulse_period;             //In timer0 steps (64us)
    int16 pulse_length;             //In us
    int8 sample_delay;              //In us after coil pulse ends
    int16 zero;                     //In ADC units
    int16 auto_zero_threshold;      //In ADC units (should be set based on 
                                    //  the environment noise)
    
    struct CoilResult result;       // Fetched result
};

extern struct Coil coil;

void coil_init();

//
// Functions to control coil activity
//
void coil_read_decay_begin();
void coil_read_peak_begin();
void coil_sleep();
void coil_wakeup();
void coil_end();

//
//  Function access coil parameters: Use them!
//
void coil_set_pulse_length( int16 pulse_length /*in us*/ );
#define coil_get_pulse_length() (coil.pulse_length)

//Changes delay in coil configuration and current coil actions
void coil_set_read_delay( int8 delay /*in us*/ );
#define coil_get_read_delay() (coil.sample_delay)

//Only change the delay applied to current coil actions but not the coil 
//configuration. An other call to coil_read_decay_begin() will use
//value from configuration and not the value set with this function
void coil_set_working_read_delay( int8 delay );

//
// Function to make coil results accesible. Returns 1 if result has enought
//  confident
int1 coil_fetch_result();

//
// Synchronous functions
//
int16 coil_peak_ref();


signed int32 coil_normalize( int16 sample, int16 zero, int16 new_max_value );

#endif