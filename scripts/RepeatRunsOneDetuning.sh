#!/bin/bash
#
#

if [ "$#" -ne 4 ]; then
	echo "usage: sudo ./RepeatRunsOneDetuning <probeOffset> <Mag. 1 voltage> <Mag. 2 voltage> <Comments>"
else
	for i in $(seq 1 20); do 
        sudo ./faradayRotation "$1" "$2" "$3" "Run $i, $4"
	done
fi
