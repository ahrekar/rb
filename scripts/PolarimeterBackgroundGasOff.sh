#!/bin/bash
#
# Usage:
#
#		./ElectronPolarizationScript <aout background> <aout Helium Excited> <dwell> <additional comments>
#

if [ "$#" -ne 5 ]; then
	echo "usage: ./PolarimeterBackgroundGasOff.sh"
	echo "				<he potential (V)> ------assumed negative, input positive value, can input multiple values in quotes." 
	echo "				<dwell (s)> -------------length of time to measure counts at each position of the QWP in the polarimeter." 
	echo "				<ammeter scale #> -------the magnitude of the scale the ammeter is set to (if 154 nA, scale=7)" 
	echo "				<detune (GHz)> ----------The detuning to set the pump laser to. 1.5 gives max Rb polarization" 
	echo "				<additional comments>----Make sure to enclose in quotes"
else
    AOUT=$1
    DWELL=$2
	AMMETERSCALE=$3
    DETUNE=$4
    COMMENTS=$5
	LEAKCURRENT=0

	source LoadWaveplatePositions.sh

	NUMRUN=3

	sudo $RBC/setPumpDetuning $DETUNE

	for i in $( seq 1 $NUMRUN ); do 
		echo "About to start next sequence of runs..."
		sleep 5
		for a in $AOUT; do 
			echo "Blocking probe beam..."
			sudo $RBC/setLaserFlag $PROBE $BLOCKED

			echo "Blocking pump beam..."
			sudo $RBC/setLaserFlag $PUMP $BLOCKED

			echo "No pump..."
			sleep 30
			sudo $RBC/polarization "$a" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$a, pump->none, Run $i/$NUMRUN"

			echo "Unblocking pump beam..."
			sudo $RBC/setLaserFlag $PUMP $UNBLOCKED

			#echo "Setting pump to Pi..."
			#sudo $RBC/setWavePlate $PIPOS
			#sleep 10
			#sudo $RBC/polarization "$a" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$a, pump->pi"

			echo "Setting pump to S+..."
			sudo $RBC/setWavePlate $SPLUSPOS
			sleep 10
			sudo $RBC/polarization "$a" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$a, pump->s+, Run $i/$NUMRUN"

			echo "Setting pump to S-..."
			sudo $RBC/setWavePlate $SMINUSPOS
			sleep 10
			sudo $RBC/polarization "$a" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$a, pump->s-, Run $i/$NUMRUN"

			echo "Unblocking probe beam..."
			sudo $RBC/setLaserFlag $PROBE $UNBLOCKED
			
			echo "Checking in on detuning..."
			sudo $RBC/setPumpDetuning $DETUNE
		done 
	done

	sudo $RBC/setHeliumTarget 0
fi
