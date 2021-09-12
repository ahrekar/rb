#!/bin/bash
#
# Usage:
#
#		./ElectronQuickPolarizationScript <aout background> <aout Helium Excited> <dwell> <additional comments>
#

if [ "$#" -ne 6 ]; then
	echo "usage: ./ElectronQuickPolarizationScript.sh"
	echo "				<Helium Potential> (enter positive value)" 
	echo "				<dwell>" 
	echo "				<num. measurements>" 
	echo "				<ammeter scale>" 
	echo "				<pump detuning>" 
	echo "				<additional comments>"
else
    AOUT=$1
    DWELL=$2
    NUM=$3
	AMMETERSCALE=$4
    DETUNE=$5
    COMMENTS=$6
	LEAKCURRENT=0
    RBC="/home/pi/RbControl"

	source $RBC/scripts/LoadWaveplatePositions.sh

	NUMRUN=1

	for i in $( seq 1 $NUMRUN ); do 
		echo "About to start next sequence of runs..."
		sleep 5
		for a in $AOUT; do 
			echo "Checking in on detuning..."
			sudo $RBC/setPumpDetuning $DETUNE
			
			echo "Blocking probe beam..."
			sudo $RBC/setLaserFlag $PROBE $BLOCKED

			echo "Blocking pump beam..."
			sudo $RBC/setLaserFlag $PUMP $BLOCKED

			echo "No pump..."
			sleep 30
			sudo $RBC/quickPolarization "$a" "$DWELL" "$NUM" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$a, pump->none, Run->$i, TotalRuns->$NUMRUN"

			echo "Unblocking pump beam..."
			sudo $RBC/setLaserFlag $PUMP $UNBLOCKED

			#echo "Setting pump to Pi..."
			#sudo $RBC/setWavePlate $PIPOS
			#sleep 10
			#sudo $RBC/quickPolarization "$a" "$DWELL" "$NUM" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$a, pump->pi, Run->$i, TotalRuns->$NUMRUN"

			echo "Setting pump to S+..."
			sudo $RBC/setWavePlate $SPLUSPOS
			sleep 10
			sudo $RBC/quickPolarization "$a" "$DWELL" "$NUM" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$a, pump->s+, Run->$i, TotalRuns->$NUMRUN"

			echo "Setting pump to S-..."
			sudo $RBC/setWavePlate $SMINUSPOS
			sleep 10
			sudo $RBC/quickPolarization "$a" "$DWELL" "$NUM" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$a, pump->s-, Run->$i, TotalRuns->$NUMRUN"

			echo "Unblocking probe beam..."
			sudo $RBC/setLaserFlag $PROBE $UNBLOCKED
		done 
	done

	sudo $RBC/setHeliumTarget 0
fi
