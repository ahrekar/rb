CFLAGS=-O3 -g -Wall -I. -lm
PIFLAGS= -l wiringPi -l mccusb -L. -L/usr/local/lib -lhidapi-libusb -lusb-1.0

all:mainTemplate bakeoutOmega controlledCooldown systemStatus
mainTemplate: mainTemplate.c grandvillePhillips.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c
	gcc $(CFLAGS) $(PIFLAGS) grandvillePhillips.c mainTemplate.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c -o mainTemplate
bakeoutOmega: bakeoutOmega.c grandvillePhillips.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c
	gcc $(CFLAGS) $(PIFLAGS) grandvillePhillips.c bakeoutOmega.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c -o bakeoutOmega
controlledCooldown: controlledCooldown.c grandvillePhillips.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c
	gcc $(CFLAGS) $(PIFLAGS) grandvillePhillips.c controlledCooldown.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c -o controlledCooldown
systemStatus: systemStatus.c grandvillePhillips.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c
	gcc $(CFLAGS) $(PIFLAGS) grandvillePhillips.c systemStatus.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c -o systemStatus
