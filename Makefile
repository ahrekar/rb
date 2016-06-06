# Makefiles! Every lazy programmer's best friend. Because
# just typing "make" is so much easier than scrolling
# through bash commands to find the last compile that
# you did.
#

# CC will be used to specify the compiler we will be using
CC=gcc

# SOURCES are the names of the executable files that we are compiling
SOURCES=excitationfn.c analogout.c getadc.c getcounts.c polarization.c setHP3617.c stepmotor.c diagnoseCounts.c faradayrotation.c homemotor.c setProbeLaser.c faradayscan.c RbAbsorbScan.c gnutest.c faradayscan2.c homeWavePlate.c setWavePlate.c

# BINARIES are the names of the executable files that we are compiling
# This particular command substitutes a blank string for ".c" in the
# list of source files above. 
BINARIES=${subst .c,,${SOURCES}}

# CFLAGS are options, or flags, we will pass to the compiler
# -g produces debugging information 
# -Wall says to produce a larger number of warning messages
# -I. says to include the current directory (.) in the
#  search for header files. 
CFLAGS= -g -Wall -I.

# PIFLAGS are flags that are needed for the raspberryPi
# code.
PIFLAGS= -l wiringPi -l mcchid -L. -lm -L/usr/local/lib -lhid -lusb

# What follows is the code to actually compile the code.
# it is always of the form
# 
# target: <dependencies>
# <TAB>Code to execute to compile.
#
# The previously created variables can be referenced by
# using $(VARIABLE)
#
# Additionally, the "$@" character refers to the target.
# 				the "$^" refers to the dependencies

# Everything depends on the binaries. If any of the 
# binaries are edited, we should do stuff. I don't really
# get why this is necessary.
all: ${BINARIES}

# Each binary needs to be compiled. In this command, each
# file in the BINARIES list is selected by the % symbol.
# The percent symbol from that point forward then 
# represents the binary file's name. 
${BINARIES}: % : %.c
	$(CC) -o $@ $@.c $(CFLAGS) $(PIFLAGS)

RbAbsorbScan: RbAbsorbScan.c tempControl.c rs485.c
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

stepmotor: stepmotor.c stepperMotorControl.c
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

homemotor: homemotor.c stepperMotorControl.c
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

faradayscan: faradayscan.c stepperMotorControl.c
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

faradayscan2: faradayscan2.c tempControl.c rs485.c stepperMotorControl.c
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

polarization: polarization.c tempControl.c rs485.c fileTools.c
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

homeWavePlate: homeWavePlate.c stepperMotorControl.c
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

setWavePlate: setWavePlate.c stepperMotorControl.c
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
