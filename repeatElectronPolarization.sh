#!/bin/bash

if [ "$#" -ne 7 ]; then
	echo "usage: sudo ./repeatElectronPolarization <number of runs> <aout_background> <aout_thresh> <aout_Excited> <dwell> <leakageCurrent> <comments_in_double_quotes>"
else
	for i in $(seq 1 "$1"); do 
		sudo scripts/RbQuickPolarizationScript.sh "Run $i, $7"
        sudo scripts/ElectronPolarizationScript.sh "$2" "$3" "$4" "$5" "$6" "Run $i, $7"
		sudo ./excitationfn 100.0 6.0 1.1 50 20 8 1 7 "Run $i, $7"
	done
fi
