#!/bin/bash
#
# Usage:
#
#		./ElectronPolarizationScript <aout background> <aout Helium Excited> <dwell> <additional comments>
#

if [ "$#" -ne 4 ]; then
	echo "usage: ./ElectronPolarizationScript.sh"
	echo "				<aout energy>" 
	echo "				<dwell>" 
	echo "				<ammeter scale>" 
	echo "				<additional comments>"
else
    AOUT=$1
    DWELL=$2
	AMMETERSCALE=$3
    COMMENTS=$4
	DETUNING=2.75
	LEAKCURRENT=0

	source LoadWaveplatePositions.sh

	NUMRUN=10

	for i in $( seq 1 $NUMRUN ); do 
		sudo $RBC/setPumpDetuning 2.75
		echo "About to start next sequence of runs..."
		sleep 5
		for a in $AOUT; do 
			echo "Blocking probe beam..."
			sudo $RBC/setLaserFlag $PROBE $BLOCKED

			echo "Blocking pump beam..."
			sudo $RBC/setLaserFlag $PUMP $BLOCKED

			echo "No pump..."
			sudo $RBC/polarization "$a" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$a, pump->none, Run $i/$NUMRUN"

			echo "Unblocking pump beam..."
			sudo $RBC/setLaserFlag $PUMP $UNBLOCKED

		##	echo "Setting pump to Pi..."
		##	sudo $RBC/setWavePlate $PIPOS
		##	sudo $RBC/polarization "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=pi"

			echo "Setting pump to S+..."
			sudo $RBC/setWavePlate $SPLUSPOS
			sudo $RBC/polarization "$a" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$a, pump->s+, Run $i/$NUMRUN"

			echo "Setting pump to S-..."
			sudo $RBC/setWavePlate $SMINUSPOS
			sudo $RBC/polarization "$a" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$a, pump->s-, Run $i/$NUMRUN"

			echo "Unblocking probe beam..."
			sudo $RBC/setLaserFlag $PROBE $UNBLOCKED
		done 
	done
fi
