#!/bin/bash
#
#

if [ "$#" -ne 4 ]; then
	echo "usage: sudo ./PolarizationScript.sh <aout> <dwell> <leakage current> <additional comments>"
	echo "               Suggested values:      (0)    (2)   (0 if unused)                          "
else
	for i in $(seq 1 5); do 
		/home/pi/RbControl/polarization "$1" "$2" "$3" "Run $i, ${4}"
	done
fi
