#!/bin/bash

#  USAGE:
#
#  	sudo ./RbAbsorbScan <startPointOfScan> <endPointOfScan> <stepSize> <comments>
#
#		startPointOfScan: min=0 , max=1023
#		endPointOfScan: min=0 , max=1023
#		stepSize: should be smaller than (max-min)
# 		comments: to be added to the comment line for 
#				  RBAbsorb and faradayScan. Should not
#				  include double quotes.
#
# Since we only use this to establish a frequency reference
# there is no need to do a full scan. I do some test scans and 
# adjust the V of the probe laser so that the strongest peak is 
# at an Aout of about 800±50. This saves time vs. doing the full
# scan.

if [ "$#" -ne 1 ]; then
	echo "usage: sudo ./fscanAndAbsorb.sh <comments>"
else
	sudo ./RbAbsorbScan 600 1000 5 "$1"

	# faraday rotation is supposed to be taken at large detunings.
	# this is at the other extreme of Aout.

	sudo ./faradayscan2 100 601 100 "$1"
fi
