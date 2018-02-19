#!/bin/bash
#
# Usage:
#
#		./ElectronPolarizationScript <aout background> <aout Helium Excited> <dwell> <additional comments>
#

if [ "$#" -ne 4 ]; then
	echo "usage: ./ElectronPolarizationScript.sh <aout energy> <dwell> <ammeter scale> <additional comments>"
else
    AOUT=$1
    DWELL=$2
	AMMETERSCALE=$3
    COMMENTS=$4
	LEAKCURRENT=0

    PIPOS=74
    SPLUSPOS=30
    SMINUSPOS=118

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0
    RBC=/home/pi/RbControl

	echo "Blocking probe beam..."
	sudo $RBC/setLaserFlag $PROBE $BLOCKED

	echo "Blocking pump beam..."
	sudo $RBC/setLaserFlag $PUMP $BLOCKED

	echo "No pump..."
	sudo $RBC/polarization "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=none"

	echo "Unblocking pump beam..."
	sudo $RBC/setLaserFlag $PUMP $UNBLOCKED

	echo "Setting pump to Pi..."
	sudo $RBC/setWavePlate $PIPOS
	sudo $RBC/polarization "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=pi"

	echo "Setting pump to S+..."
	sudo $RBC/setWavePlate $SPLUSPOS
	sudo $RBC/polarization "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=s+"

	echo "Setting pump to S-..."
	sudo $RBC/setWavePlate $SMINUSPOS
	sudo $RBC/polarization "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=s-"

	echo "Unblocking probe beam..."
	sudo $RBC/setLaserFlag $PROBE $UNBLOCKED
fi
