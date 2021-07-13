#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./RbPolarizationScript_stepPumpDetune.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	COMMENTS=$1
	PROBEFREQ=15
	STARTFREQ=0
	ENDFREQ=2.5
	STEPFREQ=.5

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for detune in $(seq $STARTFREQ $STEPFREQ $ENDFREQ); do 
		sudo $RBC/setPumpDetuning $detune
		sleep 1

		sudo $RBC/scripts/RbQuickPolarizationScript.sh $PROBEFREQ "probedetune->$PROBEFREQ,pumpdetune->$detune,  $COMMENTS"
	# detune LOOP DONE
	done 
fi
