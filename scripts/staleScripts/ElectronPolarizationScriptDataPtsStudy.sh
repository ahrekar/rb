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

	source LoadWaveplatePositions.sh


	echo "Blocking probe beam..."
	sudo $RBC/setLaserFlag $PROBE $BLOCKED

	echo "Blocking pump beam..."
	sudo $RBC/setLaserFlag $PUMP $BLOCKED

	echo "No pump..."
	sudo $RBC/polarization "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=none, 60 datapts"
	sudo $RBC/polarization30 "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=none, 30 datapts"
	sudo $RBC/polarization20 "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=none, 20 datapts"
	sudo $RBC/polarization15 "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=none, 15 datapts"

	echo "Unblocking pump beam..."
	sudo $RBC/setLaserFlag $PUMP $UNBLOCKED

#	echo "Setting pump to Pi..."
#	sudo $RBC/setWavePlate $PIPOS
#	sudo $RBC/polarization "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=pi"

	echo "Setting pump to S+..."
	sudo $RBC/setWavePlate $SPLUSPOS
	sudo $RBC/polarization "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=s+, 60 datapts"
	sudo $RBC/polarization30 "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=s+, 30 datapts"
	sudo $RBC/polarization20 "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=s+, 20 datapts"
	sudo $RBC/polarization15 "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=s+, 15 datapts"

	echo "Setting pump to S-..."
	sudo $RBC/setWavePlate $SMINUSPOS
	sudo $RBC/polarization "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=s-, 60 pts"
	sudo $RBC/polarization30 "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=s-, 30 pts"
	sudo $RBC/polarization20 "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=s-, 20 pts"
	sudo $RBC/polarization15 "$AOUT" "$DWELL" "$AMMETERSCALE" "$LEAKCURRENT" "$COMMENTS, AOUT=$AOUT, pump=s-, 15 pts"

	echo "Unblocking probe beam..."
	sudo $RBC/setLaserFlag $PROBE $UNBLOCKED
fi
