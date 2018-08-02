CC=gcc
CFLAGS=-O3 -g -Wall -I. -lm
PIFLAGS= -l wiringPi -l mccusb -L. -L/usr/local/lib -lhidapi-libusb -lusb-1.0

SOURCES=mainTemplate.c getWavelength.c getAnalogIn.c setMirror.c setBKvolts.c setBKamps.c testLasers.c TestLaser.c controlledCooldown.c testKenBoard.c monitorTemperature.c

OBJECTS=$(SOURCES:.c=.o)
BINARIES=${subst .c,,${SOURCES}}

all: ${BINARIES}

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(PIFLAGS)

mainTemplate: mainTemplate.o grandvillePhillips.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o RS485Devices.o
	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o RS485Devices.o mainTemplate.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o -o mainTemplate
	
monitorTemperature: monitorTemperature.o omegaCN7500.o kenBoard.o USB1208.o RS485Devices.o
	$(CC) $(CFLAGS) $(PIFLAGS) RS485Devices.o monitorTemperature.o omegaCN7500.o kenBoard.o USB1208.o -o monitorTemperature
	
testKenBoard: testKenBoard.o grandvillePhillips.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o RS485Devices.o
	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o RS485Devices.o testKenBoard.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o -o testKenBoard

#bakeoutOmega: bakeoutOmega.o grandvillePhillips.o omegaCN7500.o kenBoard.o USB1208.o
#	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o bakeoutOmega.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o -o bakeoutOmega

controlledCooldown: controlledCooldown.o grandvillePhillips.o omegaCN7500.o kenBoard.o USB1208.o RS485Devices.o
	$(CC) $(CFLAGS) $(PIFLAGS) RS485Devices.o grandvillePhillips.o controlledCooldown.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o -o controlledCooldown
	
#controlledWarmup: controlledWarmup.o grandvillePhillips.o omegaCN7500.o kenBoard.o USB1208.o
#	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o controlledWarmup.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o -o controlledWarmup

systemStatus: systemStatus.o grandvillePhillips.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o
	$(CC) $(CFLAGS) $(PIFLAGS) RS485Devices.o grandvillePhillips.o systemStatus.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o -o systemStatus

getAnalogIn: getAnalogIn.o  kenBoard.o USB1208.o
	$(CC) $(CFLAGS) $(PIFLAGS) getAnalogIn.o kenBoard.o RS485Devices.o USB1208.o -o getAnalogIn 

getWavelength: getWavelength.o waveMeter.o RS485Devices.o
	$(CC) $(CFLAGS) $(PIFLAGS) RS485Devices.o getWavelength.o waveMeter.o flipMirror.o USB1208.c kenBoard.o -o getWavelength

setMirror: setMirror.o kenBoard.o USB1208.o flipMirror.h RS485Devices.o flipMirror.c
	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o $@.o BK1696.o omegaCN7500.o RS485Devices.o kenBoard.o USB1208.o flipMirror.o -o $@

setBKvolts: setBKvolts.o kenBoard.o USB1208.o BK1696.o
	$(CC) $(CFLAGS) $(PIFLAGS) $@.o BK1696.o kenBoard.o USB1208.o RS485Devices.o -o $@

setBKamps: setBKamps.o kenBoard.o USB1208.o BK1696.o
	$(CC) $(CFLAGS) $(PIFLAGS) $@.o BK1696.o kenBoard.o USB1208.o RS485Devices.o -o $@

testLasers: testLasers.o vortexLaser.o
	$(CC) $(CFLAGS) $(PIFLAGS) $@.o BK1696.o kenBoard.o USB1208.o vortexLaser.o RS485Devices.o -o $@

TestLaser: TestLaser.o sacherLaser.o
	$(CC) $(CFLAGS) $(PIFLAGS) $@.o BK1696.o kenBoard.o USB1208.o sacherLaser.o RS485Devices.o -o $@
