#!/bin/bash
#
#

if [ "$#" -ne 9 ]; then 
	echo "You provided $# arguments"
	echo "usage:
    sudo ./PolarizationScript.sh <1. filament bias> 
                                 <2. n2 offset>
                                 <3. Volt 1D> 
                                 <4. Volt 2A> 
                                 <5. he offset> 
                                 <6. currentScale>
                                 <7. dwell time>
                                 <8. # Polarization Runs>
								 <9. Pump Detuning (1.5 is max)>
                                 <10. comments>

	Remember to set the AOUTS in the file!" 
else
    RBC="/home/pi/RbControl"
	FILBIAS=$1
	N2OFFSET=$2
	ONED=$3
	TWOA=$4
	HEOFFSET=$5
	CURRENTSCALE=$6
	SCANRANGE=59
	STEPSIZE=16
	DWELL=$7
	NUMRUN=$8
	DET=$8
	COMMENTS=$10
	AOUTS="12.4"

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
		sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$ONED" "$TWOA" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, postscript, laser Off"

		sudo $RBC/setWavePlate "$SPLUSPOS"
		echo "Unblocking pump laser..."
		sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
		sleep 10
		sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$ONED" "$TWOA" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, postscript, laser On"

		echo "Blocking lasers..."
		sudo $RBC/setLaserFlag $PUMP $BLOCKED
		sudo $RBC/setLaserFlag $PROBE $BLOCKED
		# EXACT REPEAT DONE
	done

fi
