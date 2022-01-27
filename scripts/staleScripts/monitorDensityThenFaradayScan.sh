#!/bin/bash
if [ "$#" -ne 1 ]; then
	echo "usage: ./monitorDensityThenFaradayScan.sh <comments>"
else
    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

    COMMENTS=$1
	FARADAYROTATIONAOUT=45
    RBC=/home/pi/RbControl

	echo "Blocking pump beam..."
	sudo $RBC/setLaserFlag $PUMP $BLOCKED
	sudo $RBC/setLaserFlag $PROBE $UNBLOCKED

	sudo $RBC/setProbeLaser $FARADAYROTATIONAOUT

	echo "Runs while density settles"
	for i in $(seq 1 5); do
		sudo $RBC/faradayRotation "$COMMENTS, auto run while density stabilizes."
	done
	sudo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, run"
fi
