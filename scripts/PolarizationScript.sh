#!/bin/bash
#
#

if [ "$#" -ne 13 ]; then
	echo "usage: sudo ./PolarizationScript.sh <FD aout start> <FD aout end> <FD step size> <RbScan start> <RbScan end> <aout background> <aout Helium Excited> <dwell> <leakage current> <probeOffset> <magnet1Voltage> <magnet2Voltage> <additional comments>"
	echo "               Suggested values:        (0)         (400)        (50)       			(500)         (1000)       (400)         			(0)          (2)    (0 if unused)      (37.0)          (~40)           (~60)"
else
	/home/pi/RbControl/RbPolarizationScript.sh "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "${13}" 

	for i in $(seq 1 5); do 
		/home/pi/RbControl/ElectronPolarizationScript.sh "$9" "${10}" "${11}" "${12}" "Run $i, ${13}"
	done
fi
