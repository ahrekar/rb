#!/bin/bash

# script to take faraday scan data and absoption data
# ALL OF THE INFORMATION GATHERED BY THIS SCRIPT IS USED TO CALCUATE NUMBER DENSITY AND Rb POLARIZATION
# Usage:
#
#		./RbPolarizationScript <aout start> <aout end> <step size> <RbScan start> <RbScan end> <additional comments>

if [ "$#" -ne 1 ]; then
	echo "usage: ./RbPolarizationScript.sh <comments>"
else
    COMMENTS=$1

    PIPOS=93
    SPLUSPOS=49
    SMINUSPOS=137

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0
    RBC=/home/pi/RbControl

	#set laser flag to unblock the beam.
	echo "Blocking Pump Beam."
	$RBC/setLaserFlag $PUMP $BLOCKED

	echo "Faraday Scan: no pump..."
	$RBC/faradayRotation "$COMMENTS, no pump"

	# Unblock pump beam for sure
	echo "Unblocking pump beam."
	$RBC/setLaserFlag $PUMP $UNBLOCKED

	# set QWP for Pi light
	echo "Setting QWP for Pi light..."
	$RBC/setWavePlate $PIPOS
	echo "Faraday Scan: Pi pump..."
    $RBC/faradayRotation "$COMMENTS, Pi pump"

	# set QWP for s+ light
	echo "Setting QWP for S+ light..."
	$RBC/setWavePlate $SPLUSPOS
	echo "Faraday Scan: S+ pump..."
    $RBC/faradayRotation "$COMMENTS, s+ pump"

	# set QWP for s- light
	echo "Setting QWP for S- light..."
	$RBC/setWavePlate $SMINUSPOS
	echo "Faraday Scan: S- pump..."
    $RBC/faradayRotation "$COMMENTS, s- pump"
fi
