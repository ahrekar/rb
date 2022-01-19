#!/bin/bash
#
#
EXPECTEDARG=12
if [ "$#" -ne "$EXPECTEDARG" ]; then 
	echo "    Expected $EXPECTEDARG arguments"
	echo "You provided $# arguments"
	echo "usage:"
	echo "  sudo ./PolarizationScript.sh <1. filament bias> (Remember negative!)"
    echo "                               <2. n2 offset>"
    echo "                               <3. Volt 1D> "
    echo "                               <4. Volt 2A> "
	echo "                               <5. He V for Polarization> (Positive Value!)"
	echo "                               <6. He offset for EXFN> (Remember negative!)"
	echo "                               <7. He scan range for EXFN> (goes down this many volts from offset!)"
    echo "                               <8. currentScale>"
    echo "                               <9. dwell time>"
    echo "                               <10. # Polarization Runs>"
	echo "                               <11. Pump Detuning (1.5 is max)>"
    echo "                               <12. comments>"
else
    RBC="/home/pi/RbControl"
	FILBIAS=$1
	N2OFFSET=$2
	ONED=$3
	TWOA=$4
	AOUTS=$5
	HEOFFSET=$6
	SCANRANGE=$7
	CURRENTSCALE=$8
	STEPSIZE=24
	DWELL=$9
	NUMRUN=${10}
	DET=${11}
	COMMENTS=${12}

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0
	source $RBC/scripts/LoadWaveplatePositions.sh

	for i in $(seq 1 $NUMRUN); do 
		echo "About to start next energy polarization run ($i/$NUMRUN). Pausing for 5 seconds to give the opportunity to cancel the run."
		sleep 5
		echo $RBC/scripts/ElectronPolarizationScript.sh "$AOUTS" $DWELL $CURRENTSCALE $DET "Run $i/$NUMRUN, $COMMENTS"
		sudo $RBC/scripts/ElectronPolarizationScript.sh "$AOUTS" $DWELL $CURRENTSCALE $DET "Run $i/$NUMRUN, $COMMENTS"

		echo "Unblocking pump laser..."
		sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
		sudo $RBC/scripts/RbPolarizationScript.sh $DET "$COMMENTS, postscript" 
		echo "RAN RB POLARIZATION SCRIPT"

		echo "Blocking probe laser..."
		sudo $RBC/setLaserFlag $PROBE $BLOCKED
		echo "blocking pump laser..."
		sudo $RBC/setLaserFlag $PUMP $BLOCKED
		sleep 10
		sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$ONED" "$TWOA" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL "$COMMENTS, postscript, laser Off"

		sudo $RBC/setWavePlate "$SPLUSPOS"
		echo "Unblocking pump laser..."
		sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
		sleep 10
		sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$ONED" "$TWOA" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL "$COMMENTS, postscript, laser On"

		echo "Blocking lasers..."
		sudo $RBC/setLaserFlag $PUMP $BLOCKED
		sudo $RBC/setLaserFlag $PROBE $BLOCKED
		# EXACT REPEAT DONE
	done

fi
