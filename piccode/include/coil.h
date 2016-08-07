#ifndef COIL_H
#define COIL_H

#define COIL_MAX_ADC_VALUE              4095



#define COIL_MAX_SAMPLE_DELAY           50		//In us
#define COIL_MIN_SAMPLE_DELAY           0		//in us
#define COIL_CALCULATE_MIN_ZERO_DELAY   COIL_MAX_SAMPLE_DELAY*4		//in us

struct CoilResult 
{
    int16 value;
    int16 noise;
};

struct Coil {
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