#!/bin/bash

# script to take faraday scan data and absoption data
# ALL OF THE INFORMATION GATHERED BY THIS SCRIPT IS USED TO CALCUATE NUMBER DENSITY AND Rb POLARIZATION
# Usage:
#
#		./RbPolarizationScript <aout start> <aout end> <step size> <RbScan start> <RbScan end> <additional comments>

if [ "$#" -ne 4 ]; then
	echo "usage: ./RbPolarizationScript.sh <probeOffset> <magnet1> <magnet2> <additional comments>"
else
    PROBEOFFSET=$1
    MAG1VOLT=$2
    MAG2VOLT=$3
    COMMENTS=$4

    PIPOS=93
    SPLUSPOS=49
    SMINUSPOS=137

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0
    RBC=/home/pi/RbControl
	#set laser flag to unblock the beam.
	echo "Blocking Pump Beam..."
	$RBC/setLaserFlag $PUMP $BLOCKED

	echo "Faraday Scan: no pump..."
	$RBC/faradayRotation "$PROBEOFFSET" "$MAG1VOLT" "$MAG2VOLT" "$COMMENTS, no pump"

	# Unblock pump beam for sure
	echo "Unblocking pump beam..."
	$RBC/setLaserFlag $PUMP $UNBLOCKED

	# set QWP for s+ light
	echo "Setting QWP for S+ light..."
	$RBC/setWavePlate $SPLUSPOS
	echo "Faraday Scan: S+ pump..."
    $RBC/faradayRotation "$PROBEOFFSET" "$MAG1VOLT" "$MAG2VOLT" "$COMMENTS, s+ pump"

	# set QWP for s- light
	echo "Setting QWP for S- light..."
	$RBC/setWavePlate $SMINUSPOS
	echo "Faraday Scan: S- pump..."
    $RBC/faradayRotation "$PROBEOFFSET" "$MAG1VOLT" "$MAG2VOLT" "$COMMENTS, s- pump"
fi
