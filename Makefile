# Makefiles! Every lazy programmer's best friend. Becuase
# just typing "make" is so much easier than scrolling
# through bash commands to find the last compile that
# you did.
#
# NOTE: I'm missing the libraries for the wiringPi, I'm not 
#       sure where to find them.

# CC will be used to specify the compiler we will be using
CC=gcc

# CFLAGS are options, or flags, we will pass to the compiler
# -g produces debugging information 
# -Wall says to produce a larger number of warning messages
# -I. says to include the current directory (.) in the
#  search for header files. 
CFLAGS= -g -Wall -I.

# PIFLAGS are flags that are needed for the raspberryPi
# code.
PIFLAGS= -lmcchid -L. -lm -L/usr/local/lib -lhid -lusb

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
excitationfn: excitationfn.c libmcchid.a
	$(CC) -o $@ $@.c $(CFLAGS) $(PIFLAGS)
