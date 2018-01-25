#!/bin/bash
#
#

if [ "$#" -ne 5 ]; then
	echo "usage: sudo ./repeatPolarization <number of runs> <aout_for_HeTarget> <dwell> <leakageCurrent> <comments_in_double_quotes>"
else
	for i in $(seq 1 $1); do 
        sudo ./polarization "$2" "$3" "$4" "Run $i, $5"
	done
fi
