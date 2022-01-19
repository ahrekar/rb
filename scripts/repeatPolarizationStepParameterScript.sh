#!/bin/bash

RBC=/home/pi/RbControl

if [ "$#" -ne 5 ]; then
	echo "usage: ./repeaPolarizationStepParameterScript.sh <HE target Voltage> "
	echo "                                                 <dwell time> "
	echo "                                                 <order of magnitude of current> "
	echo "                                                 <number of runs> "
	echo "                                                 <comments>"
	echo "                                                  "
else
	HEVOLT=$1
	DWELL=$2
	EXP=$3
	NUMRUN=$4
	COMMENTS=$5
	for a in $HEVOLT; do
		for i in $(seq $NUMRUN); do
			echo "Run number ${i}/$NUMRUN,"
			echo "Aout = ${a} / (${HEVOLT})"
			echo "pausing for 3 seconds to allow the opportunity to cancel"
			sleep 3
			sudo $RBC/polarization $a $DWELL $EXP 0 "$COMMENTS, run->${i}, totalRuns->$NUMRUN"
		done
	done
fi
