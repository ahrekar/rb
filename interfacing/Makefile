CFLAGS=-O3 -g -Wall -I. -lm
PIFLAGS= -l wiringPi -l mccusb -L. -L/usr/local/lib -lhidapi-libusb -lusb-1.0

all:mainTemplate bakeoutOmega controlledCooldown systemStatus getWavelength getAnalogIn mirrorTemplate controlledWarmup setMirror setBKvolts setBKamps

mainTemplate: mainTemplate.c grandvillePhillips.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c
	gcc $(CFLAGS) $(PIFLAGS) grandvillePhillips.c mainTemplate.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c -o mainTemplate
	
bakeoutOmega: bakeoutOmega.c grandvillePhillips.c omegaCN7500.c kenBoard.c USB1208.c
	gcc $(CFLAGS) $(PIFLAGS) grandvillePhillips.c bakeoutOmega.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c -o bakeoutOmega

controlledCooldown: controlledCooldown.c grandvillePhillips.c omegaCN7500.c kenBoard.c USB1208.c
	gcc $(CFLAGS) $(PIFLAGS) grandvillePhillips.c controlledCooldown.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c -o controlledCooldown
	
controlledWarmup: controlledWarmup.c grandvillePhillips.c omegaCN7500.c kenBoard.c USB1208.c
	gcc $(CFLAGS) $(PIFLAGS) grandvillePhillips.c controlledWarmup.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c -o controlledWarmup

systemStatus: systemStatus.c grandvillePhillips.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c
	gcc $(CFLAGS) $(PIFLAGS) grandvillePhillips.c systemStatus.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c -o systemStatus

getAnalogIn: getAnalogIn.c  kenBoard.c USB1208.c
	gcc $(CFLAGS) $(PIFLAGS) getAnalogIn.c kenBoard.c USB1208.c -o getAnalogIn 

mirrorTemplate: mirrorTemplate.c kenBoard.c USB1208.c flipMirror.h flipMirror.c
	gcc $(CFLAGS) $(PIFLAGS) grandvillePhillips.c mirrorTemplate.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c flipMirror.c -o mirrorTemplate

getWavelength: getWavelength.c waveMeter.c
	gcc $(CFLAGS) $(PIFLAGS) getWavelength.c waveMeter.c -o getWavelength

setMirror: setMirror.c kenBoard.c USB1208.c flipMirror.h flipMirror.c
	gcc $(CFLAGS) $(PIFLAGS) grandvillePhillips.c $@.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c flipMirror.c -o $@

setBKvolts: setBKvolts.c kenBoard.c USB1208.c BK1696.c
	gcc $(CFLAGS) $(PIFLAGS) $@.c BK1696.c kenBoard.c USB1208.c -o $@

setBKamps: setBKamps.c kenBoard.c USB1208.c BK1696.c
	gcc $(CFLAGS) $(PIFLAGS) $@.c BK1696.c kenBoard.c USB1208.c -o $@

