#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "usage: ./repeatRbQuickPolarization.sh <number of runs> <additional comments>"
else
    RBC="/home/pi/RbControl"
	RUNS=$1
	COMMENTS=$2

	for i in $(seq 1 $RUNS); do
		sudo $RBC/scripts/RbPolarizationScript.sh "Run $i/$RUNS, $2"
	done

fi

