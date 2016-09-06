#!/bin/bash
#
# Usage:
#
#		sudo ./ElectronPolarizationScript <aout background> <aout Helium Excited> <additional comments>
#

if [ "$#" -ne 9 ]; then
	echo "usage: sudo ./ElectronPolarizationScript.sh <aout start> <aout end> <step size> <RbScan start> <RbScan end> <aout background> <aout Helium Excited> <dwell> <additional comments>"
	echo "                                               (0)         (400)        (50)       (500)         (1000)       (400)                  (0)              (2)                        "
else
	/home/pi/RbControl/RbPolarizationScript.sh "$1" "$2" "$3" "$4" "$5" "$9" 

	for i in $(seq 1 5); do 
		# Unblock the beam
		echo "Unblocking pump beam..."
		sudo /home/pi/RbControl/setLaserFlag 1 0

		echo "Blocking probe beam..."
		sudo /home/pi/RbControl/setLaserFlag 0 1

		echo "Setting pump to Pi..."
		sudo /home/pi/RbControl/setWavePlate 61
		echo "Pi Polarized light (Background)..."
		sudo /home/pi/RbControl/polarization "$6" "$8" "$9, pump=pi, Run $i"
		echo "Pi Polarized light (Excited He)..."
		sudo /home/pi/RbControl/polarization "$7" "$8" "$9, pump=pi, Run $i"

		echo "Setting pump to S+..."
		sudo /home/pi/RbControl/setWavePlate 17
		echo "Polarization Run with S+ light..."
		sudo /home/pi/RbControl/polarization "$7" "$8" "$9, pump=s+, Run $i"

		echo "Setting pump to S-..."
		sudo /home/pi/RbControl/setWavePlate 105
		echo "Polarization Run with S- light..."
		sudo /home/pi/RbControl/polarization "$7" "$8" "$9, pump=s-, Run $i"

		echo "Unblocking probe beam..."
		sudo /home/pi/RbControl/setLaserFlag 0 0
	done
fi
