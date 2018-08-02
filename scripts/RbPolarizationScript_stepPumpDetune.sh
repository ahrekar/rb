#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	COMMENTS=$1
	STARTFREQ=27.659
	ENDFREQ=27.752
	STEPFREQ=.0125

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for detune in $(seq $STARTFREQ $STEPFREQ $ENDFREQ); do 
		echo "About to change freq to $detune, giving 1 minutes opportunity to cancel" 
		echo "About to change temperature to $detune" | mutt -s "RbPi Report" karl@huskers.unl.edu
		sleep 60

		sudo $RBC/interfacing/TestLaser $detune
		sleep 4
		sudo $RBC/interfacing/TestLaser $detune
		sleep 4
		sudo $RBC/interfacing/TestLaser $detune
		sleep 4
		echo "Giving 30 s for the laser to settle"
		sleep 30

		sudo $RBC/scripts/RbQuickPolarizationScript.sh "detune=$detune,  $COMMENTS"
	# detune LOOP DONE
	done 
	echo "Finished with pump scan run." | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
