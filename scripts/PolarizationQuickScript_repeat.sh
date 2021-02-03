#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="-37" # Should be negative
	N2OFFSET="2"
	ONED="1"
	TWOA="3.0"
	HEOFFSET=0	# Should be negative
	CURRENTSCALE=7
	SCANRANGE=40
	STEPSIZE=24
	DWELL=1
	NUMRUN=1
	COMMENTS=$1

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	sudo $RBC/setPumpDetuning 3
	for run in $(seq 1 100); do 
		sudo $RBC/scripts/PolarizationQuickScript.sh $FILBIAS $N2OFFSET $ONED $TWOA $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN "runNumber->$run $COMMENTS"
	# detune LOOP DONE
	done 
	echo "Finished with pump scan run." | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
