if [ "$#" -ne 3 ]; then 
	echo "usage: sudo ./StepTemperatureAndNotifyAtAngle <direction to step temperature (0 for down, 1 for up)> <angle to wait for> <additional comments>" 
else
    RBC="/home/pi/RbControl"

	sudo $RBC/stepTemperatureWaitForRotationAngle $1 ${2} "${3}"
	echo "IMPORTANT: The desired rotation angle has been reached." | mutt -s "RbPi Report: IMPORTANT" karl@huskers.unl.edu
fi
