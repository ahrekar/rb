#!/bin/bash
#
#

if [ "$#" -ne 8 ]; then 
	echo "You provided $# arguments"
	echo "usage: 
	sudo ./PolarizationScript.sh <1. filament bias> <2. n2 offset> <3. n2 Sweep> <4. he offset> <5. currentScale> <6. dwell time> <7. # Polarization Runs> <8. comments>

	Remember to set the AOUTS in the file!" 
else
    RBC="/home/pi/RbControl"
	FILBIAS=$1
	N2OFFSET=$2
	N2SWEEP=$3
	HEOFFSET=$4
	CURRENTSCALE=$5
	SCANRANGE=63
	STEPSIZE=16
	DWELL=$6
	NUMRUN=$7
	COMMENTS=$8
	AOUTS="408 0"

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0
	source $RBC/scripts/LoadWaveplatePositions.sh

	for i in $(seq 1 $NUMRUN); do 
		echo "About to start next energy polarization run ($i/$NUMRUN). Pausing for 5 seconds to give the opportunity to cancel the run."
		sleep 5
		sudo $RBC/scripts/ElectronPolarizationScript.sh "$AOUTS" $DWELL $CURRENTSCALE "Run $i/$NUMRUN, $COMMENTS"

		echo "Unblocking pump laser..."
		sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
		sudo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, postscript" 
		echo "RAN RB POLARIZATION SCRIPT"

		echo "Blocking probe laser..."
		sudo $RBC/setLaserFlag $PROBE $BLOCKED
		echo "blocking pump laser..."
		sudo $RBC/setLaserFlag $PUMP $BLOCKED
		sleep 10
		sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, postscript, laser Off"

		sudo $RBC/setWavePlate "$SPLUSPOS"
		echo "Unblocking pump laser..."
		sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
		sleep 10
		sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, postscript, laser On"

		echo "Blocking lasers..."
		sudo $RBC/setLaserFlag $PUMP $BLOCKED
		sudo $RBC/setLaserFlag $PROBE $BLOCKED
		# EXACT REPEAT DONE
	done


	echo "Completed set of repeat polarization runs, $COMMENTS" | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
