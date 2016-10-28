CFLAGS=-O3 -g -Wall -I. -lm
all:mainTemplate bakeoutOmega
mainTemplate: mainTemplate.c grandvillePhillips.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c
	gcc $(CFLAGS) grandvillePhillips.c mainTemplate.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c -o mainTemplate -l wiringPi -l mcchid -lhid -lusb
bakeoutOmega: bakeoutOmega.c grandvillePhillips.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c
	gcc $(CFLAGS) grandvillePhillips.c bakeoutOmega.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c -o bakeoutOmega -l wiringPi -l mcchid -lhid -lusb
