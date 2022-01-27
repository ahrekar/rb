#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript_quickStepPumpDetune.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="-37" # Should be negative
	N2OFFSET="2"
	N2SWEEP="3.0"
	HEOFFSET=0	# Should be negative
	CURRENTSCALE=6
	SCANRANGE=63
	STEPSIZE=24
	DWELL=1
	NUMRUN=1
	COMMENTS=$1

	STARTDETUNE=3
	ENDDETUNE=0
	STEPDETUNE=-1

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

		sudo $RBC/scripts/PolarizationQuickScript.sh $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN "detune=$detune, $COMMENTS"
	# detune LOOP DONE
	done 

	echo "Finished with pump scan run." | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
