# Makefiles! Every lazy programmer's best friend. Because
# just typing "make" is so much easier than scrolling
# through bash commands to find the last compile that
# you did.
#
#

#NEWDEP
#DEPDIR := .d
#$(bash mkdir -p $(DEPDIR) >/dev/null)
#DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

# CC will be used to specify the compiler we will be using
CC=gcc

# SOURCES are the names of the executable files that we are combiling
SOURCES=excitationfn.c getcounts.c polarization.c stepmotor.c homemotor.c setProbeLaser.c RbAbsorbScan.c quickFaradayScan.c faradayScan.c faradayScanAnalysis.c homeWavePlate.c setWavePlate.c setOmega.c getOmega.c waitForOmega.c polarizationAnalysis.c setHeliumTarget.c polarizationScriptAnalysis.c toggleLaserFlag.c setLaserFlag.c faradayRotation.c pumpLaserProfileScan.c monitorCountsAndCurrent.c razorBladeLaserProfiling.c recordEverythingAndTwistMotor.c

# INTERFACING are all of the programs that we use to communicate with the experimental apparatus.
INTDIR=interfacing
_INTERFACING=grandvillePhillips.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c waveMeter.c vortexLaser.c flipMirror.c
INTERFACING=$(patsubst %,$(INTDIR)/%,$(_INTERFACING))

# The directory to put object files into.
ODIR=obj

# Take all of the interfacing source files, and create a variable having the same names but with .o suffixes instead.
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
PIFLAGS= -l wiringPi -l mccusb -L. -L/usr/local/lib -lhidapi-libusb -lusb-1.0

#NEWDEP
# This is the comile line. It includes the flags, the compiler and all that good stuff.
#COMPILE.c = $(CC) $(DEPFLAGS) 

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
# binaries are edited, we should do stuff. If this
# isn't included, the makefile only looks at the first
# rule?
all: ${BINARIES}

# Each binary needs to be compiled. In this command, each
# file in the BINARIES list is selected by the % symbol.
# The percent symbol from that point forward then 
# represents the binary file's name.  Currently, this line
# is commented out because I haven't figured out how to
# get Make to look at the header files to know what other 
# object files it will need. 
#$(BINARIES):$@.o
#	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

# Create the object directory to store the object
# files in if it doesn't already exist.
$(ODIR): 
	mkdir -p $(ODIR)
	mkdir -p $(ODIR)/$(INTDIR)

$(ODIR)/%.o: %.c | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(PIFLAGS)

#$(ODIR)/fileTools.o: fileTools.c
#	$(CC) -c -o $@ $< $(CFLAGS) $(PIFLAGS)
#
#$(ODIR)/faradayScanAnalysisTools.o: faradayScanAnalysisTools.c
#	$(CC) -c -o $@ $^ $(CFLAGS) $(PIFLAGS)
$(ODIR)/polarizationAnalysisTools.o: polarizationAnalysisTools.c
	$(CC) -c -o $@ $^ $(CFLAGS) $(PIFLAGS)


getcounts: obj/getcounts.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
RbAbsorbScan: obj/RbAbsorbScan.o obj/mathTools.o obj/fileTools.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
pumpLaserProfileScan: obj/pumpLaserProfileScan.o obj/mathTools.o obj/fileTools.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
excitationfn: obj/excitationfn.o obj/mathTools.o $(INTOBJECTS) 
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
monitorCountsAndCurrent: obj/monitorCountsAndCurrent.o obj/mathTools.o $(INTOBJECTS) 
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
recordEverythingAndTwistMotor: obj/recordEverythingAndTwistMotor.o obj/mathTools.o $(INTOBJECTS) 
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
faradayScan: obj/faradayScan.o obj/mathTools.o obj/faradayScanAnalysisTools.o obj/fileTools.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
quickFaradayScan: obj/quickFaradayScan.o obj/mathTools.o obj/faradayScanAnalysisTools.o obj/fileTools.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
razorBladeLaserProfiling: obj/razorBladeLaserProfiling.o obj/mathTools.o obj/fileTools.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
faradayRotation: obj/faradayRotation.o obj/mathTools.o obj/faradayScanAnalysisTools.o obj/fileTools.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
polarization: obj/polarization.o obj/mathTools.o obj/fileTools.o $(INTOBJECTS) obj/polarizationAnalysisTools.o
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
setOmega: obj/setOmega.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
getOmega: obj/getOmega.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
setProbeLaser: obj/setProbeLaser.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
toggleLaserFlag: obj/toggleLaserFlag.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
setLaserFlag: obj/setLaserFlag.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
setHeliumTarget: obj/setHeliumTarget.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
waitForOmega: obj/waitForOmega.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

stepmotor: obj/stepmotor.o 				$(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
homemotor: obj/homemotor.o 				$(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
homeWavePlate: homeWavePlate.c 		$(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
setWavePlate: setWavePlate.c obj/fileTools.o $(INTOBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

polarizationAnalysis: obj/polarizationAnalysis.o obj/polarizationAnalysisTools.o obj/mathTools.o obj/fileTools.o
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
polarizationScriptAnalysis: obj/polarizationScriptAnalysis.o obj/polarizationAnalysisTools.o obj/mathTools.o obj/fileTools.o
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)
faradayScanAnalysis: obj/faradayScanAnalysis.o obj/faradayScanAnalysisTools.o obj/mathTools.o obj/fileTools.o
	$(CC) -o $@ $^ $(CFLAGS) $(PIFLAGS)

