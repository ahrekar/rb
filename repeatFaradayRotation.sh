#!/bin/bash
#
#

if [ "$#" -ne 2 ]; then
	echo "usage: sudo ./repeatPolarization <number of runs> <comments_in_double_quotes>"
	echo " "
	echo " Don't forget to edit the config file!"
else
	for i in $(seq 1 $1); do 
        sudo ./faradayRotation "Run $i, $2"
	done
fi
