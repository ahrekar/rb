#!/bin/bash
#

if [ "$#" -ne 5 ]; then
	echo "usage: ./PolarimeterBackgroundBeamOff.sh"
	echo "				<he potential (V)> ------assumed negative, input positive value, can input multiple values in quotes." 
	echo "				<dwell (s)> -------------length of time to measure counts at each position of the QWP in the polarimeter." 
	echo "				<ammeter scale #> -------the magnitude of the scale the ammeter is set to (if 154 nA, scale=7)" 
	echo "				<detune (GHz)> ----------The detuning to set the pump laser to. 1.5 gives max Rb polarization" 
	echo "				<additional comments>----Make sure to enclose in quotes"
else
	RBC=/home/pi/RbControl
    AOUT=$1
    DWELL=$2
	AMMETERSCALE=$3
    DETUNE=$4
    COMMENTS=$5
	LEAKCURRENT=0

	source LoadWaveplatePositions.sh

	NUMRUN=3

	echo "Checking in on detuning..."
	sudo $RBC/setPumpDetuning $DETUNE

	for i in $( seq 1 $NUMRUN ); do 
		DELAY=5 # seconds
		echo "Pausing for $DELAY seconds before starting set $i of $NUMRUN..."
		sleep $DELAY
		for a in $AOUT; do 
			echo "Blocking probe beam..."
			sudo $RBC/setLaserFlag $PROBE $BLOCKED

			echo "Blocking pump beam..."
			sudo $RBC/setLaserFlag $PUMP $BLOCKED

			echo "No pump..."
			PAUSE=30
			echo "$PAUSE second pause to allow settling."
			sleep $PAUSE
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
