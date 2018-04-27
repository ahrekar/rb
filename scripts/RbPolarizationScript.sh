#!/bin/bash

# script to take faraday scan data and absoption data
# ALL OF THE INFORMATION GATHERED BY THIS SCRIPT IS USED TO CALCUATE NUMBER DENSITY AND Rb POLARIZATION
# Usage:
#
#		./RbPolarizationScript <aout start> <aout end> <step size> <RbScan start> <RbScan end> <additional comments>

if [ "$#" -ne 1 ]; then
	echo "usage: ./RbPolarizationScript.sh <additional comments>"
else
    COMMENTS=$1

    PIPOS=76
    SPLUSPOS=30
    SMINUSPOS=118

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0
    RBC=/home/pi/RbControl

	echo "Blocking Pump Beam and unblocking probe..."
	$RBC/setLaserFlag $PUMP $BLOCKED
	$RBC/setLaserFlag $PROBE $UNBLOCKED

	echo "Faraday Scan: no pump..."
	$RBC/quickFaradayScan "$COMMENTS, no pump"

	# Unblock pump beam for sure
	echo "Unblocking pump beam..."
	$RBC/setLaserFlag $PUMP $UNBLOCKED

#	# set QWP for Pi light
#	echo "Setting QWP for Pi light..."
#	$RBC/setWavePlate $PIPOS
#	echo "Faraday Scan: Pi pump..."
#    $RBC/quickFaradayScan "$COMMENTS, Pi pump"

	# set QWP for S+ light
	echo "Setting QWP for S+ light..."
	$RBC/setWavePlate $SPLUSPOS
	echo "Faraday Scan: S+ pump..."
    $RBC/quickFaradayScan "$COMMENTS, S+ pump"

	# set QWP for S- light
	echo "Setting QWP for S- light..."
	$RBC/setWavePlate $SMINUSPOS
	echo "Faraday Scan: S- pump..."
    $RBC/quickFaradayScan "$COMMENTS, S- pump"

	echo "Finished RbScan: $COMMENTS" | mutt -s "RbControl Status Update" -- karl.ahrendsen@gmail.com
fi
