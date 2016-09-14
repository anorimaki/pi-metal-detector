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

#FUSES DEBUG
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

// A/D CONTROL REGISTER 0
#byte ADCON0 = 0xFC2

//ADON: A/D On bit
//1 = A/D Converter is operating
//0 = A/D conversion module is shut off and consuming no operating current
#bit ADON = ADCON0.0

// A/D CONTROL REGISTER 1
#byte ADCON1 = 0xFC1

// A/D CONTROL REGISTER 2
#byte ADCON2 = 0xFC0

//OSCCON register (Register 3-1) controls the main
//  aspects of the device clock?s operation
#byte OSCCON = 0xFD3

//Idle Enable bit.
// 1 = Device enters an Idle mode when a SLEEP instruction is executed
// 0 = Device enters Sleep mode when a SLEEP instruction is executed
#bit IDLEN = OSCCON.7           

//WPUB register: Weak pull-up PORT_B enable
// WPUB<7:0>: Weak Pull-Up Enable Register bits
// 1 = Pull-up enabled on corresponding PORTB pin when RBPU = 0 and the pin is an input
// 0 = Pull-up disabled on corresponding PORTB pin
#byte WPUB = 0xF5B

//INTCON2 register: Interrupt control register
#byte INTCON2 = 0xFF1

//RBPU: PORTB Pull-up Enable bit
// 1 = All PORTB pull-ups are disabled
// 0 = PORTB pull-ups are enabled by individual port latch values
#bit RBPU = INTCON2.7

//T1CON: TIMER1 CONTROL REGISTER
#byte T1CON = 0xFCD

//TMR1ON: Timer1 On bit
//  1 = Enables Timer1
//  0 = Stops Timer1
#bit TMR1ON = T1CON.0

//RD16: 16-Bit Read/Write Mode Enable bit
//1 = Enables register read/write of Timer1 in one 16-bit operation
//0 = Enables register read/write of Timer1 in two eight-bit operations
#bit TMR1RD16 = T1CON.1

//T3CON: TIMER3 CONTROL REGISTER
#byte T3CON = 0xFB1

//TMR3ON: Timer3 On bit
//  1 = Enables Timer1
//  0 = Stops Timer1
#bit TMR3ON = T3CON.0

//RD16: 16-Bit Read/Write Mode Enable bit
//1 = Enables register read/write of Timer3 in one 16-bit operation
//0 = Enables register read/write of Timer3 in two eight-bit operations
#bit TMR3RD16 = T3CON.1

//ENHANCED CAPTURE/COMPARE/PWM1 CONTROL
#byte CCP1CON = 0xFBB

//ENHANCED PWM CONTROL REGISTER
#byte ECCP1DEL = 0xFBE

//ECCP1 AUTO-SHUTDOWN CONTROL REGISTER
#byte ECCP1AS = 0xFBF

//PULSE STEERING CONTROL
#byte PSTR1CON = 0xF9C

//CCP TIMER SELECT REGISTER
#byte CCPTMRS = 0xF99

//C1TSEL: CCP1 Timer Selection bit
//  0 = ECCP1 is based off of TMR1/TMR2
//  1 = ECCP1 is based off of TMR3/TMR4
#bit C1TSEL = CCPTMRS.0

// CCP5CON: CCP5 CONTROL REGISTER
#byte CCP5CON = 0xF47

#else

#error Define one microcotroller type

#endif   //Compiler type


#endif