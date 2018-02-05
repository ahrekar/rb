#!/bin/bash

if [ "$#" -ne 9 ]; then
	echo "usage: ./ExcitationFunctionPumpDependenceScript.sh  <filament bias> <N2 Offset> <N2 Sweep> <He offset> <scan range> <step size> <dwell time> <orderOfMagnitudeOfCurrent> <comments>"
else
    FILBIAS=$1
	N2OFFSET=$2
    N2SWEEP=$3
    HEOFFSET=$4
    SCANRANGE=$5
	STEPSIZE=$6
    DWELL=$7
	CURRENTMAG=$8
	COMMENTS=$9

    PIPOS=74
    SPLUSPOS=30
    SMINUSPOS=118

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0
    RBC=/home/pi/RbControl

	echo "Blocking probe beam..."
	$RBC/setLaserFlag $PROBE $BLOCKED

	echo "Blocking pump beam..."
	$RBC/setLaserFlag $PUMP $BLOCKED
	echo "No pump..."
	$RBC/excitationfn $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTMAG "$COMMENTS, pump=none"

	echo "Unblocking pump beam..."
	$RBC/setLaserFlag $PUMP $UNBLOCKED

	echo "Setting pump to Pi..."
	$RBC/setWavePlate $PIPOS
	echo "Pi Polarized light..."
	$RBC/excitationfn $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTMAG "$COMMENTS, pump=pi"

	echo "Setting pump to S+..."
	$RBC/setWavePlate $SPLUSPOS
	echo "S+ light..."
	$RBC/excitationfn $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTMAG "$COMMENTS, pump=S+"

	echo "Setting pump to S-..."
	$RBC/setWavePlate $SMINUSPOS
	echo "S- light..."
	$RBC/excitationfn $FILBIAS $N2OFFSET $N2SWEEP $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTMAG "$COMMENTS, pump=S-"

	echo "Unblocking probe beam..."
	$RBC/setLaserFlag $PROBE $UNBLOCKED
fi
