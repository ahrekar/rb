#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./RbPolarization_stepTemperature.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	COMMENTS=$1
	CCELLTEMP=196
	#RESTEMP=
	STARTTEMP=159.8
	ENDTEMP=165.0
	STEPTEMP=.2

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for temp in $(seq $STARTTEMP $STEPTEMP $ENDTEMP); do 
		echo "About to change temperature to $temp, giving 5 minutes opportunity to cancel" 
		echo "About to change temperature to $temp, giving 5 minutes opportunity to cancel" | mutt -s "RbPi Report" karl@huskers.unl.edu
		sleep 300
		#sleep 3
		sudo $RBC/setOmega $CCELLTEMP $temp
		echo "Changed temperature to $temp, giving 25 minutes to equilibrate before recording profile." 
		sleep 1500
		#sleep 3
		sudo ./RunProgramTakePictureSendToEmail.sh

	# TEMP LOOP DONE
	done 
	echo "Done with temp step run from $STARTTEMP to $ENDTEMP" | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
