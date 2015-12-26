#ifndef MAIN_H
#define	MAIN_H


/*******************************************************/
// TARGET HW
/*******************************************************/
#define USE_PIC24FJ32GA002
//#define USE_PIC24FV16KM202

#include "picconfig.h"

/*******************************************************/
//   INPUT/OUPUT PINS
/*******************************************************/
#if defined(USE_PIC24FJ32GA002)

#define PI_COIL_PIN PIN_B1

#elif defined(USE_PIC24FV16KM202)

#define PI_COIL_PIN PIN_B1

#endif


#USE DELAY( clock=CLOCK_MHZ )
#USE FAST_IO( A )
#USE FAST_IO( B )

#endif

