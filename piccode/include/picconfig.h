#ifndef PICCONFIG_H
#define PICCONFIG_H

#define CLOCK_MHZ 20000000

#if defined(USE_PIC24FJ32GA002)

#include <24FJ32GA002.h>

#else

#error Define one microcotroller type

#endif   //Compiler type


#endif