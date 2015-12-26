#ifndef PICCONFIG_H
#define PICCONFIG_H

#define CLOCK_MHZ 20000000

#if defined(USE_PIC24FJ32GA002)

#include <24FJ32GA002.h>

//FUSES HERE

#elif defined(USE_PIC24FV16KM202)

#include <24FV16KM202.h>

#else

#error Define one microcotroller type

#endif   //Compiler type


#endif