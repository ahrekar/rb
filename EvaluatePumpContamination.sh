#!/bin/bash

# This script will be used to systematically evaluate the 
# amount of pump-probe contamination.


if [ "$#" -ne 4 ]; then
    echo "usage:"
    echo "  sudo ./EvaluatePumpContamination.sh <probeOffset> <mag1Volt> <mag2Volt> <comments>"
else
    PROBEOFFSET=$1
    MAG1VOLT=$2
    MAG2VOLT=$3
    COMMENTS=$4

    PUMP=1
    PROBE=0

    PIPOS=93
    SPLUSPOS=49
    SMINUSPOS=137

    BLOCKED=1
    UNBLOCKED=0

    RBC=/home/pi/RbControl

    echo "Unblocking both beams."
    $RBC/setLaserFlag $PUMP $UNBLOCKED
    $RBC/setLaserFlag $PROBE $UNBLOCKED
    
	echo "Setting QWP for Pi light..."
	sudo /home/pi/RbControl/setWavePlate $PIPOS

    $RBC/faradayRotation $PROBEOFFSET $MAG1VOLT $MAG2VOLT "$COMMENTS, no beams blocked, Pi Light"

	echo "Setting QWP for S+ light..."
	sudo /home/pi/RbControl/setWavePlate $SPLUSPOS

    $RBC/faradayRotation $PROBEOFFSET $MAG1VOLT $MAG2VOLT "$COMMENTS, no beams blocked, S+ Light"

	echo "Setting QWP for S- light..."
	sudo /home/pi/RbControl/setWavePlate $SMINUSPOS

    $RBC/faradayRotation $PROBEOFFSET $MAG1VOLT $MAG2VOLT "$COMMENTS, no beams blocked, S- Light"

    echo "Blocking only probe beams"
    $RBC/setLaserFlag $PROBE $BLOCKED

	echo "Setting QWP for Pi light..."
	sudo /home/pi/RbControl/setWavePlate $PIPOS

    $RBC/faradayRotation $PROBEOFFSET $MAG1VOLT $MAG2VOLT "$COMMENTS, probe beam blocked, Pi Light"

	echo "Setting QWP for S+ light..."
	sudo /home/pi/RbControl/setWavePlate $SPLUSPOS

    $RBC/faradayRotation $PROBEOFFSET $MAG1VOLT $MAG2VOLT "$COMMENTS, probe beam blocked, S+ Light"

	echo "Setting QWP for S- light..."
	sudo /home/pi/RbControl/setWavePlate $SMINUSPOS

    $RBC/faradayRotation $PROBEOFFSET $MAG1VOLT $MAG2VOLT "$COMMENTS, probe beam blocked, S- Light"

    echo "Blocking only pump beam"
    $RBC/setLaserFlag $PUMP $BLOCKED
    $RBC/setLaserFlag $PROBE $UNBLOCKED
    
    $RBC/faradayRotation $PROBEOFFSET $MAG1VOLT $MAG2VOLT "$COMMENTS, pump beam blocked"

    echo "Blocking both beams."
    $RBC/setLaserFlag $PUMP $BLOCKED
    $RBC/setLaserFlag $PROBE $BLOCKED
    
    $RBC/faradayRotation $PROBEOFFSET $MAG1VOLT $MAG2VOLT "$COMMENTS, both beams blocked"

    echo "Unblocking both beams."
    $RBC/setLaserFlag $PUMP $UNBLOCKED
    $RBC/setLaserFlag $PROBE $UNBLOCKED
fi
