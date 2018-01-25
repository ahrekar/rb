#!/bin/bash
#
# Usage:
#
#		./ElectronPolarizationScript <aout background> <aout Helium Excited> <dwell> <additional comments>
#

if [ "$#" -ne 6 ]; then
	echo "usage: ./ElectronPolarizationScript.sh <aout background> <AOUT THRESHOLD> <aout Helium Excited> <dwell> <leakageCurrent> <additional comments>"
	echo "                                                 (400)               (0)           	(2)   (0 if not used)                       "
else
    AOUTBACK=$1
	AOUTTHRESH=$2
    AOUTEXCITE=$3
    DWELL=$4
    LEAKCURRENT=$5
    COMMENTS=$6

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

	echo "No pump (Background)..."
	$RBC/polarization "$AOUTBACK" "$DWELL" "$LEAKCURRENT" "$COMMENTS, background, pump=none"
	echo "No pump (Threshold He)..."
	$RBC/polarization "$AOUTTHRESH" "$DWELL" "$LEAKCURRENT" "$COMMENTS, excited1, pump=none"
	echo "No pump (Excited He)..."
	$RBC/polarization "$AOUTEXCITE" "$DWELL" "$LEAKCURRENT" "$COMMENTS, excited2, pump=none"

	echo "Unblocking pump beam..."
	$RBC/setLaserFlag $PUMP $UNBLOCKED

	echo "Setting pump to Pi..."
	$RBC/setWavePlate $PIPOS
	echo "Pi Polarized light (Background)..."
	$RBC/polarization "$AOUTBACK" "$DWELL" "$LEAKCURRENT" "$COMMENTS, background, pump=pi"
	echo "Pi Polarized light (Threshold He)..."
	$RBC/polarization "$AOUTTHRESH" "$DWELL" "$LEAKCURRENT" "$COMMENTS, excited1, pump=pi"
	echo "Pi Polarized light (Excited He)..."
	$RBC/polarization "$AOUTEXCITE" "$DWELL" "$LEAKCURRENT" "$COMMENTS, excited2, pump=pi"

	echo "Setting pump to S+..."
	$RBC/setWavePlate $SPLUSPOS
	echo "Polarization Run with S+ light (background)..."
	$RBC/polarization "$AOUTBACK" "$DWELL" "$LEAKCURRENT" "$COMMENTS, background, pump=s+"
	echo "Polarization Run with S+ light (excited1)..."
	$RBC/polarization "$AOUTTHRESH" "$DWELL" "$LEAKCURRENT" "$COMMENTS, excited1, pump=s+"
	echo "Polarization Run with S+ light (excited2)..."
	$RBC/polarization "$AOUTEXCITE" "$DWELL" "$LEAKCURRENT" "$COMMENTS, excited2, pump=s+"

	echo "Setting pump to S-..."
	$RBC/setWavePlate $SMINUSPOS
	echo "Polarization Run with S- light (background)..."
	$RBC/polarization "$AOUTBACK" "$DWELL" "$LEAKCURRENT" "$COMMENTS, background, pump=s-"
	echo "Polarization Run with S- light (excited1)..."
	$RBC/polarization "$AOUTTHRESH" "$DWELL" "$LEAKCURRENT" "$COMMENTS, excited1, pump=s-"
	echo "Polarization Run with S- light (excited)..."
	$RBC/polarization "$AOUTEXCITE" "$DWELL" "$LEAKCURRENT" "$COMMENTS, excited2, pump=s-"

	echo "Unblocking probe beam..."
	$RBC/setLaserFlag $PROBE $UNBLOCKED
fi
