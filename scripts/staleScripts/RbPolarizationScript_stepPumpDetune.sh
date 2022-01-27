#!/bin/bash

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./RbPolarizationScript_stepPumpDetune <additional comments>" 
else
    rbc="/home/pi/RbControl"
	comments=$1
	probefreq=-30.
	startfreq=-10.
	endfreq=0.
	stepfreq=.5

	BIGSTEP=2.5
	LILSTEP=.5

    pump=1
    probe=0

    blocked=1
    unblocked=0

	for seedDetune in $(seq $startfreq $LILSTEP 7.5); do 
		for detune in $(seq $seedDetune $BIGSTEP $endfreq); do 
			sudo $rbc/setPumpDetuning $detune
			#echo "sudo $rbc/setPumpDetuning $detune"
			sleep 1

			sudo $rbc/scripts/RbQuickPolarizationScript.sh $probefreq "probedetune->$probefreq, pumpdetune->$detune,  $comments"
			#echo "sudo $rbc/scripts/RbQuickPolarizationScript.sh $probefreq probedetune->$probefreq, pumpdetune->$detune,  $comments"
		# detune loop done
		done 
	done
fi
