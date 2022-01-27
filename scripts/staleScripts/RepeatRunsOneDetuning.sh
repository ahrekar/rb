#!/bin/bash
#
#

if [ "$#" -ne 4 ]; then
	echo "usage: sudo ./RepeatRunsOneDetuning <probeOffset> <Mag. 1 voltage> <Mag. 2 voltage> <Comments>"
else
	for i in $(seq 1 50); do 
		sleep 1
		sudo ../setProbeDetuning $1
        sudo ../faradayRotation "probeDetuning->$1, magnet1->$2, magnet2->$3, Run->$i, $4"
	done
fi
