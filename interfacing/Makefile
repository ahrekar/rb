CC = gcc
CFLAGS = -O3 -g -Wall -I. -lm
PIFLAGS = -lwiringPi -lmccusb -L. -L/usr/local/lib -lhidapi-libusb -lusb-1.0

SOURCES = mainTemplate.c testK6485.c testK485.c testSR830.c controlledWarmup.c controlledCooldown.c

OBJECTS = $(SOURCES:.c=.o)
BINARIES = ${subst .c,,${SOURCES}}

all: ${BINARIES}

mainTemplate : mainTemplate.o kenBoard.o K485meter.o K6485meter.o USB1208.o omegaCN7500.o grandvillePhillips.o RS485Devices.o
	$(CC) $(CFLAGS) $(PIFLAGS) $^ -o $@

controlledWarmup : controlledWarmup.o kenBoard.o omegaCN7500.o RS485Devices.o
	$(CC) $(CFLAGS) $(PIFLAGS) $^ -o $@

controlledCooldown : controlledCooldown.o kenBoard.o omegaCN7500.o RS485Devices.o
	$(CC) $(CFLAGS) $(PIFLAGS) $^ -o $@

testK6485 : testK6485.o kenBoard.o USB1208.o RS485Devices.o K6485meter.o
	$(CC) $(CFLAGS) $(PIFLAGS) $^ -o $@

testK485 : testK485.o kenBoard.o USB1208.o RS485Devices.o K485meter.o
	$(CC) $(CFLAGS) $(PIFLAGS) $^ -o $@

testSR830 : testSR830.o kenBoard.o USB1208.o RS485Devices.o SR830LockIn.o
	$(CC) $(CFLAGS) $(PIFLAGS) $^ -o $@

mainTemplate.o : mainTemplate.c
	$(CC) $(CFLAGS) $(PIFLAGS) $< -c -o $@

kenBoard.o : kenBoard.c
	$(CC) $(CFLAGS) $(PIFLAGS) $< -c -o $@

K485meter.o : K485meter.c
	$(CC) $(CFLAGS) $(PIFLAGS) $< -c -o $@

SR830LockIn.o : SR830LockIn.c SR830LockIn.h
	$(CC) $(CFLAGS) $(PIFLAGS) $< -c -o $@

K6485meter.o : K6485meter.c K6485meter.h
	$(CC) $(CFLAGS) $(PIFLAGS) $< -c -o $@

testK6485.o : testK6485.c K6485meter.o
	$(CC) $(CFLAGS) $(PIFLAGS) $< -c -o $@

testSR830.o : testSR830.c SR830LockIn.o
	$(CC) $(CFLAGS) $(PIFLAGS) $< -c -o $@

testK485.o : testK485.c
	$(CC) $(CFLAGS) $(PIFLAGS) $< -c -o $@
	

#%.o : %.c
#	$(CC) $(CFLAGS) $(PIFLAGS) kenBoard.o RS485Devices.o $< -c -o $@
