#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "usage: ./repeatRbQuickPolarization.sh <number of runs> <additional comments>"
else
    RBC="/home/pi/RbControl"
	RUNS=$1
	COMMENTS=$2

	for i in $(seq 1 $RUNS); do
		SLEEPTIME=1 # seconds
		echo "Pausing for $SLEEPTIME seconds to give the opportunity to cancel."
		sleep $SLEEPTIME
		sudo $RBC/scripts/RbQuickPolarizationScript.sh -10 "Run $i/$RUNS, $2"
	done

fi

