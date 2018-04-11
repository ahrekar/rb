#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="120.0"
	N2OFFSET="37.3"
	N2SWEEP="5.8"
	HEOFFSET=40
	CURRENTSCALE=6
	SCANRANGE=30
	STEPSIZE=24
	DWELL=1
	NUMRUN=4
	COMMENTS=$1
	STARTFREQ=28.065
	ENDFREQ=28.2
	STEPFREQ=.017

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for detune in $(seq $STARTFREQ $STEPFREQ $ENDFREQ; seq 28.2 .035 28.305); do 
##		echo "About to change freq to $detune, giving 5 minutes opportunity to cancel" 
#		echo "About to change temperature to $detune" | mutt -s "RbPi Report" karl@huskers.unl.edu
#		sleep 300
#
		sudo $RBC/interfacing/TestLaser $detune
		echo "Giving 30 s for the laser to settle"
		sleep 3
#		sleep 30
#
#		sudo $RBC/scripts/PolarizationScript.sh $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN "detune=$detune,  $COMMENTS"
	# detune LOOP DONE
	done 
	echo "Finished with pump scan run." | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
