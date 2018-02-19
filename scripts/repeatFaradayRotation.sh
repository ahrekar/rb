#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "usage: ./repeatFaradayRotation.sh <number of runs> <comments>"
	echo "                                                  "
else
	for i in $(seq $1); do
		echo "Run number ${i}/$1, pausing for 5 seconds to allow the opportunity to cancel"
		sleep 5
		./faradayRotation "Repeat Runs, run number $i, $2"
	done
fi
