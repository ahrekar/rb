CC=gcc
CFLAGS=-O3 -g -Wall -I. -lm
PIFLAGS= -l wiringPi -l mccusb -L. -L/usr/local/lib -lhidapi-libusb -lusb-1.0

SOURCES=mainTemplate.c bakeoutOmega.c controlledCooldown.c systemStatus.c getWavelength.c getAnalogIn.c mirrorTemplate.c controlledWarmup.c setMirror.c setBKvolts.c setBKamps.c testLasers.c TestLaser.c TestBridge.c

OBJECTS=$(SOURCES:.c=.o)
BINARIES=${subst .c,,${SOURCES}}

all: ${BINARIES}

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(PIFLAGS)

mainTemplate: mainTemplate.o grandvillePhillips.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o
	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o mainTemplate.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o -o mainTemplate
	
bakeoutOmega: bakeoutOmega.o grandvillePhillips.o omegaCN7500.o kenBoard.o USB1208.o
	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o bakeoutOmega.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o -o bakeoutOmega

controlledCooldown: controlledCooldown.o grandvillePhillips.o omegaCN7500.o kenBoard.o USB1208.o
	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o controlledCooldown.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o -o controlledCooldown
	
controlledWarmup: controlledWarmup.o grandvillePhillips.o omegaCN7500.o kenBoard.o USB1208.o
	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o controlledWarmup.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o -o controlledWarmup

#systemStatus: systemStatus.o grandvillePhillips.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o
#	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o systemStatus.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o -o systemStatus
systemStatus: systemStatus.o grandvillePhillips.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o
	$(CC) $(CFLAGS) $(PIFLAGS) $^ -o systemStatus

getAnalogIn: getAnalogIn.o  kenBoard.o USB1208.o
	$(CC) $(CFLAGS) $(PIFLAGS) getAnalogIn.o kenBoard.o USB1208.o -o getAnalogIn 

mirrorTemplate: mirrorTemplate.o kenBoard.o USB1208.o flipMirror.h flipMirror.o
	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o mirrorTemplate.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o flipMirror.o -o mirrorTemplate

getWavelength: getWavelength.o waveMeter.o 
	$(CC) $(CFLAGS) $(PIFLAGS) getWavelength.o waveMeter.o flipMirror.o kenBoard.o -o getWavelength

setMirror: setMirror.o kenBoard.o USB1208.o flipMirror.h flipMirror.o
	$(CC) $(CFLAGS) $(PIFLAGS) grandvillePhillips.o $@.o BK1696.o omegaCN7500.o kenBoard.o USB1208.o flipMirror.o -o $@

setBKvolts: setBKvolts.o kenBoard.o USB1208.o BK1696.o
	$(CC) $(CFLAGS) $(PIFLAGS) $@.o BK1696.o kenBoard.o USB1208.o -o $@

setBKamps: setBKamps.o kenBoard.o USB1208.o BK1696.o
	$(CC) $(CFLAGS) $(PIFLAGS) $@.o BK1696.o kenBoard.o USB1208.o -o $@

testLasers: testLasers.o vortexLaser.o
	$(CC) $(CFLAGS) $(PIFLAGS) $@.o BK1696.o kenBoard.o USB1208.o vortexLaser.o -o $@

TestLaser: TestLaser.o sacherLaser.o
	$(CC) $(CFLAGS) $(PIFLAGS) $@.o BK1696.o kenBoard.o USB1208.o sacherLaser.o -o $@

TestBridge: TestBridge.o kenBoard.o
	$(CC) $(CFLAGS) $(PIFLAGS) $@.o BK1696.o kenBoard.o USB1208.o -o $@
