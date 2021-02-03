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
	DWELL=2
	NUMRUN=1
	COMMENTS=$1

	STARTDETUNE=2
	ENDDETUNE=4
	STEPDETUNE=1

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for power in 1800 2300 2800 3300 3800; do 
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

			sudo $RBC/scripts/PolarizationQuickScript.sh $FILBIAS $N2OFFSET $ONED $TWOA $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN "detune->$detune, power=$power $COMMENTS"
		# detune LOOP DONE
		done 
	#power LOOP DONE
	done
	echo "Finished with pump scan run." | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
