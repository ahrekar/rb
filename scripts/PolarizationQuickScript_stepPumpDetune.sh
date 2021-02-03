#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./QuickPolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="-135.1" # Should be negative
	N2OFFSET="34"
	ONED="1"
	TWOA="3.0"
	HEOFFSET=0	# Should be negative
	CURRENTSCALE=6
	SCANRANGE=40
	STEPSIZE=24
	DWELL=1
	NUMRUN=1
	COMMENTS=$1

	STARTDETUNE=3
	ENDDETUNE=11
	STEPDETUNE=2

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for detune in $(seq $STARTDETUNE $STEPDETUNE $ENDDETUNE); do 
		echo "About to change freq to $detune, giving 1 minute opportunity to cancel" 
		sleep 6

		sudo $RBC/setPumpDetuning $detune
		echo "Giving 30 s for the laser to settle"
		#sleep 3
		sleep 30

		sudo $RBC/setPumpDetuning $detune
		echo "Giving 3 s for the laser to settle"
		sleep 3

		sudo $RBC/scripts/PolarizationQuickScript.sh $FILBIAS $N2OFFSET $ONED $TWOA $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN "detune->$detune, $COMMENTS"
	# detune LOOP DONE
	done 
	echo "Finished with pump scan run." | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
