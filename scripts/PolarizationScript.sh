#!/bin/bash
#
#

if [ "$#" -ne 8 ]; then 
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
	SCANRANGE=30
	STEPSIZE=24
	DWELL=$6
	NUMRUN=$7
	COMMENTS=$8
	AOUTS=(24)

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	#echo "Blocking pump laser..."
	#sudo $RBC/setLaserFlag $PUMP $BLOCKED

	#sudo $RBC/RbAbsorbScan 45 70 .2 "$COMMENTS, prelude"

	#echo "Unblocking pump laser..."
	#sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
	#sudo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, prelude" 

	#echo "Blocking probe laser..."
	#sudo $RBC/setLaserFlag $PROBE $BLOCKED

	#sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, prelude"

	for i in $(seq 1 $NUMRUN); do 
		echo "Unblocking pump beam..."
		sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
		sudo $RBC/scripts/takePictureAndSendToEmail.sh "RUN${i}of${NUMRUN}"
		echo "About to start next set of polarization runs. Pausing for 5 seconds to give the opportunity to cancel the run."
		sleep 5

		for AOUT in ${AOUTS[@]}; do 
			echo "About to start next energy polarization run. Pausing for 5 seconds to give the opportunity to cancel the run."
			sleep 5
			sudo $RBC/scripts/ElectronPolarizationScript.sh $AOUT $DWELL $CURRENTSCALE "Run $i/$NUMRUN, AOUT=$AOUT, $COMMENTS"
		done
	# EXACT REPEAT DONE
	done

	echo "Blocking pump laser..."
	sudo $RBC/setLaserFlag $PUMP $BLOCKED

	sudo $RBC/RbAbsorbScan 45 70 .2 "$COMMENTS, postscript"

	echo "Unblocking pump laser..."
	sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
	sudo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, postscript" 

	echo "Blocking probe laser..."
	sudo $RBC/setLaserFlag $PROBE $BLOCKED

	sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, postscript"

	echo "Unblocking lasers..."
	sudo $RBC/setLaserFlag $PUMP $UNBLOCKED
	sudo $RBC/setLaserFlag $PROBE $UNBLOCKED

	echo "Completed set of repeat polarization runs, $COMMENTS" | mutt -s "RbPi Report" karl@huskers.unl.edu

fi
