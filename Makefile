# Makefiles! Every lazy programmer's best friend. Becuase
# just typing "make" is so much easier than scrolling
# through bash commands to find the last compile that
# you did.
#

# CC will be used to specify the compiler we will be using
CC=gcc

# SOURCES are the names of the executable files that we are compiling
SOURCES=excitationfn.c analogout.c getadc.c getcounts.c polarization.c setHP3617.c stepmotor.c

# BINARIES are the names of the executable files that we are compiling
BINARIES=excitationfn analogout getadc getcounts polarization setHP3617 stepmotor

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
# putting using $(VARIABLE)
#
# Additionally, the "$@" characters refer to the target.
all: $(BINARIES)

excitationfn: excitationfn.c 
	$(CC) -o $@ $@.c $(CFLAGS) $(PIFLAGS)

analogout: analogout.c 
	$(CC) -o $@ $@.c $(CFLAGS) $(PIFLAGS)

getadc: getadc.c 
	$(CC) -o $@ $@.c $(CFLAGS) $(PIFLAGS)

getcounts: getcounts.c 
	$(CC) -o $@ $@.c $(CFLAGS) $(PIFLAGS)

polarization: polarization.c 
	$(CC) -o $@ $@.c $(CFLAGS) $(PIFLAGS)

setHP3617: setHP3617.c 
	$(CC) -o $@ $@.c $(CFLAGS) $(PIFLAGS)
	
stepmotor: stepmotor.c 
	$(CC) -o $@ $@.c $(CFLAGS) $(PIFLAGS)
