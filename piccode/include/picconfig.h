#ifndef PICCONFIG_H
#define PICCONFIG_H

#define CLOCK_HZ 16000000

#if defined(USE_PIC24FJ32GA002)

#include <24FJ32GA002.h>

//FUSES HERE

#elif defined(USE_PIC24FV16KM202)

#include <24FV16KM202.h>

#elif defined(USE_PIC18F26K80)

#include <18F26K80.h>

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES VREGSLEEP             	//Ultra low-power regulator is disabled
#FUSES SOSC_DIG              	//Digital mode, I/O port functionality of RC0 and RC1
#FUSES NOXINST               	//Extended set extension and Indexed Addressing mode disabled (Legacy mode)
#FUSES FCMEN                 	//Fail-safe clock monitor enabled
#FUSES IESO                  	//Internal External Switch Over mode enabled
#FUSES NOPUT                 	//No Power Up Timer
#FUSES NOBROWNOUT            	//No brownout reset
#FUSES ZPBORM                	//Zero-Power BOR
#FUSES CANB                  	//CANTX and CANRX pins are located on RB2 and RB3
#FUSES MSSPMSK7              	//MSSP uses 7 bit Masking mode
#FUSES MCLR                  	//Master Clear pin enabled
#FUSES STVREN                	//Stack full/underflow will cause reset
#FUSES BBSIZ2K               	//2K words Boot Block size
#FUSES NOPROTECT             	//Code not protected from reading
#FUSES NOCPB                 	//No Boot Block code protection
#FUSES NOCPD                 	//No EE protection
#FUSES NOWRT                 	//Program memory not write protected
#FUSES NOWRTC                	//Configuration registers not write protected
#FUSES NOWRTB                	//Boot block not write protected
#FUSES NOWRTD                	//Data EEPROM not write protected
#FUSES NOEBTR                	//Memory not protected from table reads
#FUSES NOEBTRB               	//Boot block not protected from table reads

#byte OSCCON = 0xFD3
#bit IDLEN = OSCCON.7

#else

#error Define one microcotroller type

#endif   //Compiler type


#endif