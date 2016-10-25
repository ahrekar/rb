#!/bin/bash
#
# Usage:
#
#		sudo ./ElectronPolarizationScript <aout background> <aout Helium Excited> <dwell> <additional comments>
#

if [ "$#" -ne 5 ]; then
	echo "usage: sudo ./ElectronPolarizationScript.sh <aout background> <aout Helium Excited> <dwell> <leakageCurrent> <additional comments>"
	echo "                                                 (400)               (0)           	(2)   (0 if not used)                       "
else
	# Unblock the beam
	echo "Unblocking pump beam..."
	sudo /home/pi/RbControl/setLaserFlag 1 0

	echo "Blocking probe beam..."
	sudo /home/pi/RbControl/setLaserFlag 0 1

	echo "Setting pump to Pi..."
	sudo /home/pi/RbControl/setWavePlate 61
	echo "Pi Polarized light (Background)..."
	sudo /home/pi/RbControl/polarization "$1" "$3" "$4" "$5, pump=pi"
	echo "Pi Polarized light (Excited He)..."
	sudo /home/pi/RbControl/polarization "$2" "$3" "$4" "$5, pump=pi"

	echo "Setting pump to S+..."
	sudo /home/pi/RbControl/setWavePlate 17
	echo "Polarization Run with S+ light..."
	sudo /home/pi/RbControl/polarization "$2" "$3" "$4" "$5, pump=s+"

	echo "Setting pump to S-..."
	sudo /home/pi/RbControl/setWavePlate 105
	echo "Polarization Run with S- light..."
	sudo /home/pi/RbControl/polarization "$2" "$3" "$4" "$5, pump=s-"

	echo "Unblocking probe beam..."
	sudo /home/pi/RbControl/setLaserFlag 0 0
fi
