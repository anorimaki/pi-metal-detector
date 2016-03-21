#ifndef MAIN_H
#define	MAIN_H


/*******************************************************/
// TARGET HW
/*******************************************************/
#define USE_PIC18F26K80
//#define USE_PIC24FV16KM202

#include "picconfig.h"

/*******************************************************/
//   INPUT/OUPUT PINS
/*******************************************************/
#if defined(USE_PIC24FJ32GA002)

#define PI_COIL_PIN PIN_B1

#elif defined(USE_PIC24FV16KM202)

#define PI_COIL_PIN PIN_B1

#elif defined(USE_PIC18F26K80)

#define PI_COIL_PIN PIN_B1

#endif



#use delay(clock=CLOCK_HZ,crystal=4000000)
#use fast_io( A )
#use fast_io( B )
#use fast_io( C )

#endif

