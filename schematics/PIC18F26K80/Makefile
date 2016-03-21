###############################################################################
# Makefile for project 
###############################################################################

## General options
PROJECT = 
MCU = PIC18F26K80
COMPILER = "CCS for PIC"
TARGET = Debug
SHELL = C:\WINDOWS\system32\cmd.exe

## Tools general options
CCFLAGS=-J +DF +ES -P -Z +O8 +CC +EXPORT +FH I+=..\..\piccode\include
LDFLAGS=-J +DF +ES -P -Z +O8 +CC +FH

## Processing Tools
CC=ccsc.exe
LD=ccsc.exe
#  E:/cdu/pi-metal-detector/schematics/PIC18F26K80/Debug.cof: TOOL=LD
#     E:/cdu/pi-metal-detector/schematics/PIC18F26K80/../../piccode/src/main.o: TOOL=CC
#        E:/cdu/pi-metal-detector/piccode/src/main.c
#     E:/cdu/pi-metal-detector/schematics/PIC18F26K80/../../piccode/src/pimetaldec.o: TOOL=CC
#        E:/cdu/pi-metal-detector/piccode/src/pimetaldec.c

# Build tree:
all:	Debug

Debug:	Debug.cof

Debug.cof:	../../piccode/src/main.o ../../piccode/src/pimetaldec.o 
	$(LD) $(LDFLAGS)  LINK="Debug.cof=..\..\piccode\src\main.o,..\..\piccode\src\pimetaldec.o"

../../piccode/src/main.o:	../../piccode/src/main.c 
	$(CC) $(CCFLAGS)  "..\..\piccode\src\main.c"

../../piccode/src/pimetaldec.o:	../../piccode/src/pimetaldec.c 
	$(CC) $(CCFLAGS)  "..\..\piccode\src\pimetaldec.c"

# tidy - delete all temporary files which are not involved in the target generation
tidy:
	rm -rf ./Debug.esym
	rm -rf ./Debug.sym
	rm -rf ./Debug.lst
	rm -rf Debug.err
	rm -rf ../../piccode/src/main.esym
	rm -rf ../../piccode/src/main.osym
	rm -rf ../../piccode/src/main.err
	rm -rf ../../piccode/src/pimetaldec.esym
	rm -rf ../../piccode/src/pimetaldec.osym
	rm -rf ../../piccode/src/pimetaldec.err

# cleanup - delete all generated files
clean:	tidy
	rm -rf Debug.cof
	rm -rf ../../piccode/src/main.o
	rm -rf ../../piccode/src/pimetaldec.o
