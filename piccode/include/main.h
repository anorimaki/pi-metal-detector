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

#define PI_COIL_CTRL_PIN            PIN_A2
#define PI_ADC_INDICATOR_PIN        PIN_A6

#define PI_DECAY_SIGNAL_PIN         PIN_A0
#define PI_DECAY_SIGNAL_AN          sAN0
#define PI_DECAY_SIGNAL_CH          0
#define PI_COIL_VOLTAGE_PIN         PIN_A3 
#define PI_COIL_VOLTAGE_AN          sAN3
#define PI_COIL_VOLTAGE_CH          3
#define PI_BATTERY_VOLTAGE_PIN      PIN_A1
#define PI_BATTERY_VOLTAGE_AN       sAN1
#define PI_BATTERY_VOLTAGE_CH       1
#define PI_ANALOGIC_REF_PIN         PIN_A5
#define PI_ANALOGIC_REF_AN          sAN4
#define PI_ANALOGIC_REF_CH          4

#define PI_CHARLIEPLEX_SWITCH_1_PIN PIN_B0
#define PI_CHARLIEPLEX_SWITCH_2_PIN PIN_B1
#define PI_CHARLIEPLEX_SWITCH_3_PIN PIN_B2
#define PI_CHARLIEPLEX_SWITCH_4_PIN PIN_B3
#define PI_BUTTON_PULLUP_RESISTORS  0x0F

#define PI_TONE_PIN                 PIN_B4

#if 1
#define LCD_DATA_PORT           getenv("SFR:PORTC")
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



//#use delay(clock=CLOCK_HZ,crystal=4000000)
#use delay(clock=CLOCK_HZ,internal=4000000)
#use fast_io( A )
#use fast_io( B )
#use fast_io( C )

#endif

