# Makefiles! Every lazy programmer's best friend. Because
# just typing "make" is so much easier than scrolling
# through bash commands to find the last compile that
# you did.
#

# CC will be used to specify the compiler we will be using
CC=gcc

# SOURCES are the names of the executable files that we are compiling
SOURCES=excitationfn.c getcounts.c polarization.c stepmotor.c homemotor.c setProbeLaser.c RbAbsorbScan.c faradayScan.c faradayScanAnalysis.c homeWavePlate.c setWavePlate.c setOmega.c waitForOmega.c polarizationAnalysis.c setHeliumTarget.c polarizationScriptAnalysis.c

INTDIR=interfacing
_INTERFACING=grandvillePhillips.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c
INTERFACING=$(patsubst %,$(INTDIR)/%,$(_INTERFACING))

# The directory to put object files into.
ODIR=obj

# Take all of the interfacing source files, and give them .o suffixes instead.
_INTOBJECTS=$(INTERFACING:.c=.o)
# Create all of the interfacing object files in their own directory.
INTOBJECTS=$(patsubst %,$(ODIR)/%,$(_INTOBJECTS))
# Take all of the regular source files, and give them .o suffixes instead.
_OBJECTS=$(SOURCES:.c=.o)
# Create all of the object files in their own directory.
OBJECTS=$(patsubst %,$(ODIR)/%,$(_OBJECTS))

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
#$(BINARIES):$@.o
#	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
$(ODIR)/%.o: %.c
		$(CC) -c -o $@ $< $(CFLAGS) $(PIFLAGS)

getcounts: obj/getcounts.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
RbAbsorbScan: obj/RbAbsorbScan.o obj/mathTools.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
excitationfn: obj/excitationfn.o obj/mathTools.o $(INTOBJECTS) 
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
faradayScan: obj/faradayScan.o obj/mathTools.o obj/faradayScanAnalysisTools.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
polarization: obj/polarization.o obj/mathTools.o obj/fileTools.o $(INTOBJECTS) obj/polarizationAnalysisTools.o
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
setOmega: obj/setOmega.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
setProbeLaser: obj/setProbeLaser.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
setHeliumTarget: obj/setHeliumTarget.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
waitForOmega: obj/waitForOmega.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

stepmotor: obj/stepmotor.o 				$(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
homemotor: obj/homemotor.o 				$(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
faradayrotation: obj/faradayrotation.o	$(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
homeWavePlate: homeWavePlate.c 		$(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
setWavePlate: setWavePlate.c 		$(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

polarizationAnalysis: obj/polarizationAnalysis.o obj/polarizationAnalysisTools.o obj/mathTools.o
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
polarizationScriptAnalysis: obj/polarizationScriptAnalysis.o obj/polarizationAnalysisTools.o obj/mathTools.o
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
faradayScanAnalysis: obj/faradayScanAnalysis.o obj/faradayScanAnalysisTools.o obj/mathTools.o
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
