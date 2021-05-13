#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="-150" # Should be negative
	N2OFFSET="100"
	ONED="1.0"
	TWOA="0.8"
	HEOFFSET=0	# Should be negative
	CURRENTSCALE=6
	SCANRANGE=59
	STEPSIZE=24
	DWELL=1
	NUMRUN=1
	COMMENTS=$1
	####
	## DON'T FORGET TO SET THE AOUT IN THE PolarizationScript.sh
	####

	#STARTDETUNE=1.5
	#ENDDETUNE=11.5
	#STEPDETUNE=2.5
	# OR, you can specify the detunings space separated. This is 
	# useful if you want to make sure that you are not suceptible
	# to drifting effects. E.G: 1.5 11.5 4.0 9.0 6.5

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	echo "Scheduled detunings are:"
	for detune in 1.5 11.5 4.0 9.0 6.5; do 
		echo $detune
	done

	for detune in 1.5 11.5 4.0 9.0 6.5; do 
		echo "About to change freq to $detune, giving 1 minute opportunity to cancel" 
		sleep 6

		sudo $RBC/setPumpDetuning $detune
		echo "Giving 30 s for the laser to settle"
		#sleep 3
		sleep 30

		sudo $RBC/setPumpDetuning $detune
		echo "Giving 3 s for the laser to settle"
		sleep 3

		sudo $RBC/scripts/PolarizationScript.sh $FILBIAS $N2OFFSET $ONED $TWOA $HEOFFSET $CURRENTSCALE $DWELL $NUMRUN $detune "detune->$detune, $COMMENTS"
	# detune LOOP DONE
	done 
fi
