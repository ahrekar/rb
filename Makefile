# Makefiles! Every lazy programmer's best friend. Because
# just typing "make" is so much easier than scrolling
# through bash commands to find the last compile that
# you did.
#

# CC will be used to specify the compiler we will be using
CC=gcc

# SOURCES are the names of the executable files that we are compiling
SOURCES=excitationfn.c getadc.c getcounts.c polarization.c stepmotor.c homemotor.c setProbeLaser.c RbAbsorbScan.c faradayscan.c homeWavePlate.c setWavePlate.c faradayAnalysis.c setOmega.c waitForOmega.c polarizationAnalysis.c 

# BINARIES are the names of the executable files that we are compiling
# This particular command substitutes a blank string for ".c" in the
# list of source files above. 
BINARIES=${subst .c,,${SOURCES}}

# CFLAGS are options, or flags, we will pass to the compiler
# -g produces debugging information 
# -Wall says to produce a larger number of warning messages
# -I. says to include the current directory (.) in the
#  search for header files. 
# -lm says to include the math library
# -O3 says to optimize the code, sacrificing debugging
#  abilities.
CFLAGS= -O3 -g -Wall -I. -lm

# PIFLAGS are flags that are needed for the raspberryPi
# code.
PIFLAGS= -l wiringPi -l mcchid -L. -L/usr/local/lib -lhid -lusb

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
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

polarizationAnalysisTools.c: polarizationAnalysisTools.h

INTERFACING=interfacing/grandvillePhillips.c interfacing/BK1696.c interfacing/omegaCN7500.c interfacing/kenBoard.c interfacing/USB1208.c

RbAbsorbScan: RbAbsorbScan.c mathTools.c $(INTERFACING)
excitationfn: excitationfn.c mathTools.c $(INTERFACING) 
setOmega: setOmega.c $(INTERFACING)
waitForOmega: waitForOmega.c $(INTERFACING)

stepmotor: stepmotor.c 				stepperMotorControl.c
homemotor: homemotor.c 				stepperMotorControl.c
faradayrotation: faradayrotation.c	stepperMotorControl.c
homeWavePlate: homeWavePlate.c 		stepperMotorControl.c
setWavePlate: setWavePlate.c 		stepperMotorControl.c

faradayscan: faradayscan.c mathTools.c $(INTERFACING)
polarization: polarization.c polarizationAnalysisTools.c mathTools.c tempControl.c rs485.c fileTools.c stepperMotorControl.c
polarizationAnalysis: polarizationAnalysis.c polarizationAnalysisTools.c mathTools.c
stepperMotorDiagnose: stepperMotorDiagnose.c tempControl.c rs485.c fileTools.c stepperMotorControl.c
