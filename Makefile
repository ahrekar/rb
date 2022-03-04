# Makefiles! Every lazy programmer's best friend. Because
# just typing "make" is so much easier than scrolling
# through bash commands to find the last compile that
# you did.
#
# Note that $(OUTPUT_OPTION) is defined to be "-o $@" in my 
# case. This is defined by the compiler.
#
# So the embarrassing truth is I don't really know how 
# this all works. It's just a house of cards that I 
# occassionally add another component on to. If it were
# to collapse at some point, I'd be hopelessly lost.

#NEWDEP
DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
$(shell mkdir -p $(DEPDIR)/interfacing >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

VPATH = obj

# CC will be used to specify the compiler we will be using
CC=gcc

# SOURCES are the names of the executable files that we are compiling
SOURCES=excitationfn.c getCounts.c polarization.c quickPolarization.c stepmotor.c homemotor.c setProbeLaser.c setPumpLaser.c RbPumpAbsorbScan.c RbAbsorbScan.c findBPDBalance.c getAngleBPD.c monitorAngleBPD.c faradayScanBPD.c faradayScan.c faradayScanAnalysis.c homeWavePlate.c setWavePlate.c setOmega.c getOmega.c polarizationAnalysis.c setHeliumTarget.c toggleFlipMirror.c toggleLaserFlag.c setLaserFlag.c faradayRotation.c monitorCountsAndCurrent.c setTACurrent.c setProbeDetuning.c setPumpDetuning.c getWavemeter.c monitorPhotodiodes.c monitorPhotodiodesOnKeyPress.c getPhotoDiodes.c turnOffPumpLaser.c deflectorTransmission.c asymmetry.c monitorCountsAndCurrentDigital.c recordRetroReflection.c 
# unused: RbAbsorbScanAutoFindDetuning.c stepTemperatureWaitForRotationAngle.c findDetuningForMaxPolarization.c

# INTERFACING are all of the programs that we use to communicate with the experimental apparatus.
INTDIR=interfacing
_INTERFACING=grandvillePhillips.c BK1696.c omegaCN7500.c kenBoard.c USB1208.c waveMeter.c vortexLaser.c flipMirror.c RS485Devices.c topticaLaser.c keithley.c Sorensen120.c K617meter.c laserFlag.c sacherLaser.c K485meter.c K6485meter.c
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
# This is the compile line. It includes the flags, the compiler and all that good stuff.
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(PIFLAGS) -c
# This is the link line. It also includes the flags, the compiler and all that good stuff. It binds the compiled files together.
LINK.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(PIFLAGS)
# These commands are run after the compiling process.
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

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
# 				the "$<" refers to the FIRST dependency

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
#	$(COMPILE.c) -o $@ $^ 

# Create the object directory to store the object
# files in if it doesn't already exist.
$(ODIR): 
	mkdir -p $(ODIR)
	mkdir -p $(ODIR)/$(INTDIR)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d
.PRECIOUS: $(DEPDIR)/interfacing/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCES))))

# The vertical line in this command separates the normal-prerequisites from the order-only prerequisites. 
# The order-only prerequisites tells make that it needs to run the rules for $(ODIR) first, before 
# doing the commands below, but it does not require the target to be updated if the $(ODIR) rules are run.
$(ODIR)/%.o: %.c | $(ODIR)
$(ODIR)/%.o: %.c $(DEPDIR)/%.d | $(ODIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)


#ORIGINAL
COMMONOBJECTS=$(INTDIR)/kenBoard.o $(INTDIR)/USB1208.o $(INTDIR)/grandvillePhillips.o $(INTDIR)/omegaCN7500.o $(INTDIR)/RS485Devices.o
PROBELASEROBJECTS=$(INTDIR)/sacherLaser.o $(INTDIR)/waveMeter.o $(INTDIR)/flipMirror.o $(INTDIR)/laserFlag.o probeLaserControl.o
PUMPLASEROBJECTS=$(INTDIR)/topticaLaser.o $(INTDIR)/waveMeter.o $(INTDIR)/flipMirror.o $(INTDIR)/laserFlag.o

getCounts: getCounts.o $(COMMONOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
RbAbsorbScan: RbAbsorbScan.o mathTools.o fileTools.o $(COMMONOBJECTS) $(PROBELASEROBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
getWavemeter: getWavemeter.o $(INTDIR)/waveMeter.o $(COMMONOBJECTS) $(PROBELASEROBJECTS) $(PUMPLASEROBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
RbPumpAbsorbScan: RbPumpAbsorbScan.o mathTools.o fileTools.o $(COMMONOBJECTS) $(PUMPLASEROBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
pumpLaserProfileScan: pumpLaserProfileScan.o mathTools.o fileTools.o $(COMMONOBJECTS) $(PUMPLASEROBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
excitationfn: excitationfn.o mathTools.o $(INTDIR)/Sorensen120.o $(INTDIR)/K617meter.o $(COMMONOBJECTS) 
	$(LINK.c) $(OUTPUT_OPTION) $^ 
monitorCountsAndCurrent: monitorCountsAndCurrent.o mathTools.o $(INTOBJECTS) 
	$(LINK.c) $(OUTPUT_OPTION) $^ 
monitorPhotodiodes: monitorPhotodiodes.o mathTools.o $(INTOBJECTS) 
	$(LINK.c) $(OUTPUT_OPTION) $^ 
monitorPhotodiodesOnKeyPress: monitorPhotodiodesOnKeyPress.o mathTools.o $(INTOBJECTS) 
	$(LINK.c) $(OUTPUT_OPTION) $^ 
deflectorTransmission: deflectorTransmission.o mathTools.o $(INTOBJECTS) 
	$(LINK.c) $(OUTPUT_OPTION) $^ 
asymmetry: asymmetry.o mathTools.o $(INTDIR)/kenBoard.o $(INTDIR)/K6485meter.o $(INTDIR)/K485meter.o $(INTDIR)/K617meter.o $(COMMONOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
monitorCountsAndCurrentDigital: monitorCountsAndCurrentDigital.o mathTools.o $(INTDIR)/kenBoard.o $(INTDIR)/K6485meter.o $(INTDIR)/K485meter.o $(INTDIR)/K617meter.o $(COMMONOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
recordRetroReflection: recordRetroReflection.o mathTools.o $(INTOBJECTS) 
	$(LINK.c) $(OUTPUT_OPTION) $^ 
faradayScan: faradayScan.o mathTools.o faradayScanAnalysisTools.o fileTools.o probeLaserControl.o $(INTOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
faradayScanBPD: faradayScanBPD.o mathTools.o faradayScanAnalysisTools.o fileTools.o probeLaserControl.o $(INTOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
getAngleBPD: getAngleBPD.o mathTools.o faradayScanAnalysisTools.o fileTools.o probeLaserControl.o $(INTOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
monitorAngleBPD: monitorAngleBPD.o mathTools.o faradayScanAnalysisTools.o fileTools.o probeLaserControl.o $(INTOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
findBPDBalance: findBPDBalance.o mathTools.o faradayScanAnalysisTools.o fileTools.o probeLaserControl.o $(INTOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
#findDetuningForMaxPolarization: findDetuningForMaxPolarization.o mathTools.o faradayScanAnalysisTools.o fileTools.o probeLaserControl.o $(INTOBJECTS)
#	$(LINK.c) $(OUTPUT_OPTION) $^ 
faradayRotation: faradayRotation.o mathTools.o faradayScanAnalysisTools.o fileTools.o $(INTOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
#stepTemperatureWaitForRotationAngle: stepTemperatureWaitForRotationAngle.o mathTools.o faradayScanAnalysisTools.o fileTools.o $(INTOBJECTS)
#	$(LINK.c) $(OUTPUT_OPTION) $^ 
polarization: polarization.o mathTools.o fileTools.o $(COMMONOBJECTS) polarizationAnalysisTools.o $(INTDIR)/Sorensen120.o $(INTDIR)/K6485meter.o
	$(LINK.c) $(OUTPUT_OPTION) $^ 
#polarization: polarization.o mathTools.o fileTools.o polarizationAnalysisTools.o $(INTOBJECTS)
	#$(LINK.c) $(OUTPUT_OPTION) $^ 
quickPolarization: quickPolarization.o mathTools.o fileTools.o $(COMMONOBJECTS) polarizationAnalysisTools.o $(INTDIR)/Sorensen120.o
	$(LINK.c) $(OUTPUT_OPTION) $^ 
setOmega: setOmega.o $(COMMONOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
getOmega: getOmega.o $(COMMONOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
setProbeLaser: setProbeLaser.o $(COMMONOBJECTS) $(PROBELASEROBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
setPumpLaser: setPumpLaser.o $(INTDIR)/kenBoard.o $(INTDIR)/USB1208.o $(INTDIR)/topticaLaser.o
	$(LINK.c) $(OUTPUT_OPTION) $^ 
setProbeDetuning: setProbeDetuning.o probeLaserControl.o $(INTOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
setPumpDetuning: setPumpDetuning.o pumpLaserControl.o $(INTOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
setTACurrent: setTACurrent.o fileTools.o $(INTOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
turnOffPumpLaser: turnOffPumpLaser.o $(INTDIR)/topticaLaser.o $(INTDIR)/kenBoard.o
	$(LINK.c) $(OUTPUT_OPTION) $^ 
toggleLaserFlag: toggleLaserFlag.o $(INTOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
toggleFlipMirror: toggleFlipMirror.o $(INTOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
setLaserFlag: setLaserFlag.o $(COMMONOBJECTS) $(INTDIR)/laserFlag.o
	$(LINK.c) $(OUTPUT_OPTION) $^ 
setHeliumTarget: setHeliumTarget.o $(COMMONOBJECTS) $(INTDIR)/Sorensen120.o
	$(LINK.c) $(OUTPUT_OPTION) $^ 

stepmotor: stepmotor.o 				$(COMMONOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
homemotor: homemotor.o 				$(COMMONOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
homeWavePlate: homeWavePlate.c 		$(COMMONOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 
setWavePlate: setWavePlate.c fileTools.o $(COMMONOBJECTS)
	$(LINK.c) $(OUTPUT_OPTION) $^ 

polarizationAnalysis: polarizationAnalysis.o polarizationAnalysisTools.o mathTools.o fileTools.o
	$(LINK.c) $(OUTPUT_OPTION) $^ 
polarizationScriptAnalysis: polarizationScriptAnalysis.o polarizationAnalysisTools.o mathTools.o fileTools.o
	$(LINK.c) $(OUTPUT_OPTION) $^ 
faradayScanAnalysis: faradayScanAnalysis.o faradayScanAnalysisTools.o mathTools.o fileTools.o
	$(LINK.c) $(OUTPUT_OPTION) $^ 
getPhotoDiodes: getPhotoDiodes.o mathTools.o $(INTDIR)/kenBoard.o $(INTDIR)/USB1208.o
	$(LINK.c) $(OUTPUT_OPTION) $^ 

