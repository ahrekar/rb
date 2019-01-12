#!/bin/bash

# This script will be used to systematically evaluate the 
# amount of pump-probe contamination.


if [ "$#" -ne 1 ]; then
    echo "usage:"
    echo "  sudo ./EvaluatePumpContamination.sh <comments>"
else
    COMMENTS=$1

	source LoadWaveplatePositions.sh

    echo "Unblocking both beams."
    $RBC/setLaserFlag $PUMP $UNBLOCKED
    $RBC/setLaserFlag $PROBE $UNBLOCKED
    
	echo "Setting QWP for Pi light..."
	sudo /home/pi/RbControl/setWavePlate $PIPOS

    $RBC/faradayRotation "$COMMENTS, no beams blocked, Pi Light"

	echo "Setting QWP for S+ light..."
	sudo /home/pi/RbControl/setWavePlate $SPLUSPOS

    $RBC/faradayRotation "$COMMENTS, no beams blocked, S+ Light"

	echo "Setting QWP for S- light..."
	sudo /home/pi/RbControl/setWavePlate $SMINUSPOS

    $RBC/faradayRotation "$COMMENTS, no beams blocked, S- Light"

    echo "Blocking only probe beams"
    $RBC/setLaserFlag $PROBE $BLOCKED

	echo "Setting QWP for Pi light..."
	sudo /home/pi/RbControl/setWavePlate $PIPOS

    $RBC/faradayRotation "$COMMENTS, probe beam blocked, Pi Light"

	echo "Setting QWP for S+ light..."
	sudo /home/pi/RbControl/setWavePlate $SPLUSPOS

    $RBC/faradayRotation "$COMMENTS, probe beam blocked, S+ Light"

	echo "Setting QWP for S- light..."
	sudo /home/pi/RbControl/setWavePlate $SMINUSPOS

    $RBC/faradayRotation "$COMMENTS, probe beam blocked, S- Light"

    echo "Blocking only pump beam."
    $RBC/setLaserFlag $PUMP $BLOCKED
    $RBC/setLaserFlag $PROBE $UNBLOCKED
    
    $RBC/faradayRotation "$COMMENTS, pump beam blocked"

    echo "Blocking both beams."
    $RBC/setLaserFlag $PUMP $BLOCKED
    $RBC/setLaserFlag $PROBE $BLOCKED
    
    $RBC/faradayRotation "$COMMENTS, both beams blocked"

    echo "Unblocking both beams."
    $RBC/setLaserFlag $PUMP $UNBLOCKED
    $RBC/setLaserFlag $PROBE $UNBLOCKED
fi
