#!/bin/bash
#
#

if [ "$#" -ne 10 ]; then
	echo "usage: sudo ./PolarizationScript.sh <aout start> <aout end> <step size> <RbScan start> <RbScan end> <aout background> <aout Helium Excited> <dwell> <leakage current> <additional comments>"
	echo "               Suggested values:        (0)         (400)        (50)       (500)         (1000)       (400)                  (0)              (2)   (0 if unused)                          "
else
	/home/pi/RbControl/RbPolarizationScript.sh "$1" "$2" "$3" "$4" "$5" "${10}" 

	for i in $(seq 1 5); do 
		/home/pi/RbControl/ElectronPolarizationScript.sh "$6" "$7" "$8" "$9" "Run $i, ${10}"
	done
fi
