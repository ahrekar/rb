#!/bin/bash
#
# Usage:
#
#		./ElectronPolarizationScript <aout background> <aout Helium Excited> <dwell> <additional comments>
#

if [ "$#" -ne 5 ]; then
	echo "usage: ./ElectronPolarizationScript.sh <aout background> <aout Helium Excited> <dwell> <leakageCurrent> <additional comments>"
	echo "                                                 (400)               (0)           	(2)   (0 if not used)                       "
else
    AOUTBACK=$1
    AOUTEXCITE=$2
    DWELL=$3
    LEAKCURRENT=$4
    COMMENTS=$5

    PIPOS=93
    SPLUSPOS=49
    SMINUSPOS=137

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0
    RBC=/home/pi/RbControl

	# Unblock the beam
	echo "Unblocking pump beam..."
	$RBC/setLaserFlag $PUMP $UNBLOCKED

	echo "Blocking probe beam..."
	$RBC/setLaserFlag $PROBE $BLOCKED

	echo "Setting pump to Pi..."
	$RBC/setWavePlate $PIPOS
	echo "Pi Polarized light (Background)..."
	$RBC/polarization "$AOUTBACK" "$DWELL" "$LEAKCURRENT" "$COMMENTS, pump=pi"
	echo "Pi Polarized light (Excited He)..."
	$RBC/polarization "$AOUTEXCITE" "$DWELL" "$LEAKCURRENT" "$COMMENTS, pump=pi"

	echo "Setting pump to S+..."
	$RBC/setWavePlate $SPLUSPOS
	echo "Polarization Run with S+ light..."
	$RBC/polarization "$AOUTEXCITE" "$DWELL" "$LEAKCURRENT" "$COMMENTS, pump=s+"

	echo "Setting pump to S-..."
	$RBC/setWavePlate $SMINUSPOS
	echo "Polarization Run with S- light..."
	$RBC/polarization "$AOUTEXCITE" "$DWELL" "$LEAKCURRENT" "$COMMENTS, pump=s-"

	echo "Unblocking probe beam..."
	$RBC/setLaserFlag $PROBE $UNBLOCKED
fi
