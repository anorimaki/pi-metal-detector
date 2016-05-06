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

#define PI_COIL_CTRL_PIN            PIN_A3
#define PI_INTEGRATE_CTRL_PIN       PIN_A5

#define PI_INTEGRATION_SIGNAL_PIN   sAN0        //RA0
#define PI_INTEGRATION_SIGNAL_CH    0
#define PI_COIL_VOLTAGE_PIN         sAN1        //RA1
#define PI_COIL_VOLTAGE_CH          1

#if 1
#define LCD_DATA_PORT           getenv("SFR:PORTB")
#else
#define LCD_RW_PIN  				PIN_B2
#define LCD_ENABLE_PIN  			PIN_B0
#define LCD_RS_PIN     				PIN_B1
#define LCD_DATA0      				PIN_B4
#define LCD_DATA1       			PIN_B5
#define LCD_DATA2       			PIN_B6
#define LCD_DATA3       			PIN_B7 
#endif

#DEVICE ADC=12

#endif



#use delay(clock=CLOCK_HZ,crystal=4000000)
#use fast_io( A )
#use fast_io( B )
#use fast_io( C )

#endif

