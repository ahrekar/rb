#!/bin/bash

# script to take faraday scan data and absoption data
# ALL OF THE INFORMATION GATHERED BY THIS SCRIPT IS USED TO CALCUATE NUMBER DENSITY AND Rb POLARIZATION
# Usage:
#
#		./RbQuickPolarizationScript.sh < 

if [ "$#" -lt 1 ]; then
	echo "usage: 1. ./RbQuickPolarizationScript.sh <comments>"
	echo "  "
	echo "       2. ./RbQuickPolarizationScript.sh <probe detuning>"
	echo "                                         <comments>"
#if [ "$#" -ne 2 ]; then
	#echo "usage: ./RbQuickPolarizationScript.sh <number of files>    <comments>"
	#echo "                                    ( 2 = no pump(NP), S+            "
	#echo "                                      3 = NP, S+, S-          	   "
	#echo "                                      4 = NP, pi, S+, S- )            "
else
	if [ "$#" -eq 2 ];then
		PROBEDETUNE=$1
		COMMENTS=$2
	else
		COMMENTS=$1
		PROBEDETUNE=-15
	fi

	source LoadWaveplatePositions.sh

	echo "setting the probe laser to a proper detuning"
	sudo $RBC/setProbeDetuning $PROBEDETUNE

	#set laser flag to unblock the probe beam.
	echo "Unblocking probe Beam."
	$RBC/setLaserFlag $PROBE $UNBLOCKED

	#set laser flag to block the pump beam.
	echo "Blocking Pump Beam."
	$RBC/setLaserFlag $PUMP $BLOCKED

	echo "Faraday Scan: no pump..."
	$RBC/faradayRotation "$COMMENTS, no pump"

	# Unblock pump beam for sure
	echo "Unblocking pump beam."
	$RBC/setLaserFlag $PUMP $UNBLOCKED

	# set QWP for s+ light
	echo "Setting QWP for S+ light..."
	$RBC/setWavePlate $SPLUSPOS
	echo "Faraday Scan: S+ pump..."
    $RBC/faradayRotation "$COMMENTS, s+ pump"

# set QWP for s- light
#	echo "Setting QWP for S- light..."
#	$RBC/setWavePlate $SMINUSPOS
#	echo "Faraday Scan: S- pump..."
#    $RBC/faradayRotation "$COMMENTS, s- pump"
fi
