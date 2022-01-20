#!/bin/bash
#
#

if [ "$#" -ne 11 ]; then 
	echo "You provided $# arguments"
	echo "usage: "
    echo "sudo ./PolarizationQuickScript.sh <1. filament bias> "
    echo "                                  <2. n2 offset>"
    echo "                                  <3. Volt 1D> "
    echo "                                  <4. Volt 2A> "
	echo "								    <5. he offset (for not-used excitation function)>"
    echo "                                  <6. currentScale>"
	echo "								    <7. dwell time (s)>"
    echo "                                  <8. aouts>"
    echo "                                  <9. # Polarization Runs>"
    echo "                                  <10. Pump Detuning>"
    echo "                                  <11. comments>"
    echo " " 
    echo "Remember to set the AOUTS in the file!" 
else
    RBC="/home/pi/RbControl"
	FILBIAS=$1
	N2OFFSET=$2
	ONED=$3
	TWOA=$4
	HEOFFSET=$5
	AMMETERSCALE=$6
	DWELL=$7
	AOUTS=$8
	NUMRUN=$9
	DETUNE=${10}
	COMMENTS=${11}

	NUM=3

	PROBEDETUNE=-10

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0
	source $RBC/scripts/LoadWaveplatePositions.sh

	for heVolt in $AOUTS; do 
		sudo $RBC/setHeliumTarget $heVolt
		for i in $(seq 1 $NUMRUN); do 
			echo "Setting Pump detuning..."
			sudo $RBC/setPumpDetuning $DETUNE

			echo "setting probe detuning"
			sudo $RBC/setProbeDetuning $PROBEDETUNE

			echo "Blocking probe beam..."
			sudo $RBC/setLaserFlag $PROBE $BLOCKED

			echo "Blocking pump beam..."
			sudo $RBC/setLaserFlag $PUMP $BLOCKED

			echo "Electron Polarization No pump..."
			echo "Pausing for 30 s to allow time to settle..."
			sleep 30
			sudo $RBC/quickPolarization "$heVolt" "$DWELL" "$NUM" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$heVolt, pump->none, Run->$i, TotalRuns->$NUMRUN"

			echo "Faraday Scan: no pump..."
			sudo $RBC/setLaserFlag $PROBE $UNBLOCKED
			$RBC/faradayRotation "$COMMENTS, pump->no"
			sudo $RBC/setLaserFlag $PROBE $BLOCKED

			echo "Unblocking pump beam..."
			sudo $RBC/setLaserFlag $PUMP $UNBLOCKED

			echo "Setting pump to S+..."
			sudo $RBC/setWavePlate $SPLUSPOS
			sleep 10
			echo "Electron Polarization S+ pump..."
			sudo $RBC/quickPolarization "$heVolt" "$DWELL" "$NUM" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$heVolt, pump->s+, Run->$i, TotalRuns->$NUMRUN"

			echo "Faraday Scan: no pump..."
			sudo $RBC/setLaserFlag $PROBE $UNBLOCKED
			$RBC/faradayRotation "$COMMENTS, pump->s+"
			sudo $RBC/setLaserFlag $PROBE $BLOCKED

			echo "Setting pump to S-..."
			sudo $RBC/setWavePlate $SMINUSPOS
			sleep 10
			echo "Electron Polarization S- pump..."
			sudo $RBC/quickPolarization "$heVolt" "$DWELL" "$NUM" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT->$heVolt, pump->s-, Run->$i, TotalRuns->$NUMRUN"
			echo "Faraday Scan: S- pump..."
			sudo $RBC/setLaserFlag $PROBE $UNBLOCKED
			$RBC/faradayRotation "$COMMENTS, pump->s-"
			sudo $RBC/setLaserFlag $PROBE $BLOCKED

			echo "Blocking pump beam..."
			sudo $RBC/setLaserFlag $PUMP $BLOCKED
		done
	done
fi
