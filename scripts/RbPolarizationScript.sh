#!/bin/bash

# script to take faraday scan data and absoption data
# ALL OF THE INFORMATION GATHERED BY THIS SCRIPT IS USED TO CALCUATE NUMBER DENSITY AND Rb POLARIZATION
# Usage:
#
#		./RbPolarizationScript <aout start> <aout end> <step size> <RbScan start> <RbScan end> <additional comments>

if [ "$#" -ne 1 ]; then
	echo "usage: ./RbPolarizationScript.sh <additional comments>"
else
	RBC=/home/pi/RbControl
    COMMENTS=$1

	source $RBC/scripts/LoadWaveplatePositions.sh

	echo "Blocking Pump Beam and unblocking probe..."
	$RBC/setLaserFlag $PUMP $BLOCKED
	$RBC/setLaserFlag $PROBE $UNBLOCKED

	echo "Faraday Scan: no pump..."
	$RBC/faradayScan "$COMMENTS, no pump"

	# Unblock pump beam for sure
	echo "Unblocking pump beam..."
	$RBC/setLaserFlag $PUMP $UNBLOCKED

	# set QWP for S+ light
	echo "Setting QWP for S+ light..."
	$RBC/setWavePlate $SPLUSPOS
	echo "Faraday Scan: S+ pump..."
    $RBC/faradayScan "$COMMENTS, S+ pump"

	# set QWP for S- light
	echo "Setting QWP for S- light..."
	$RBC/setWavePlate $SMINUSPOS
	echo "Faraday Scan: S- pump..."
    $RBC/faradayScan "$COMMENTS, S- pump"

	echo "Blocking both beams..."
	$RBC/setLaserFlag $PUMP $BLOCKED
	$RBC/setLaserFlag $PROBE $BLOCKED

	$RBC/scripts/takePictureAndSendToEmail.sh "AutoPic"
	echo "Finished RbScan: $COMMENTS" | mutt -s "RbControl Status Update" -- karl.ahrendsen@gmail.com
fi
