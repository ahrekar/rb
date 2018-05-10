#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./RbPolarization_stepTemperature.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	COMMENTS=$1
	CCELLTEMP=150
	STARTTEMP=125
	ENDTEMP=130
	STEPTEMP=1

	NUMPOLRUNS=5

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	for temp in $(seq $STARTTEMP $STEPTEMP $ENDTEMP); do 
		echo "About to change temperature to $temp, giving 5 minutes opportunity to cancel" 
		echo "About to change temperature to $temp" | mutt -s "RbPi Report" karl@huskers.unl.edu
		sleep 300
		sudo $RBC/setOmega $CCELLTEMP $temp

		for run in $(seq $NUMPOLRUNS); do 
			sudo ./RbPolarizationScript.sh "Temp: $temp, Run: $run/$NUMPOLRUN, $COMMENTS"
		#RUN LOOP DONE
		done 

	# TEMP LOOP DONE
	done 
	echo "Done with temp step run from $STARTTEMP to $ENDTEMP" | mutt -s "RbPi Report" karl@huskers.unl.edu
fi
