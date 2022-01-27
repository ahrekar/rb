#!/bin/bash
#
#

if [ "$#" -ne 10 ]; then 
	echo "You provided $# arguments"
	echo "usage: 
    sudo ./PolarizationQuickScript.sh <1. filament bias> 
                                      <2. n2 offset>
                                      <3. Volt 1D> 
                                      <4. Volt 2A> 
                                      <5. currentScale>
                                      <6. dwell time>
                                      <7. aouts>
                                      <8. # Polarization Runs>
                                      <9. Pump Detuning>
                                      <10. comments>

    Remember to set the AOUTS in the file!" 
else
    RBC="/home/pi/RbControl"
	FILBIAS=$1
	N2OFFSET=$2
	ONED=$3
	TWOA=$4
	CURRENTSCALE=$5
	DWELL=$6
	AOUTS=$7
	NUMRUN=$8
	DETUNE=${9}
	COMMENTS=${10}
	NUMMEAS=10

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0
	source $RBC/scripts/LoadWaveplatePositions.sh

	for i in $(seq 1 $NUMRUN); do 
		for heVolt in $AOUTS; do 
			sudo $RBC/setPumpDetuning 1.5
			sudo $RBC/setHeliumTarget $heVolt
			sudo $RBC/setLaserFlag 1 0
			#sleep 10
			sudo $RBC/quickPolarization $heVolt $DWELL $NUMMEAS $CURRENTSCALE 0 "E_i->$N2OFFSET, $COMMENTS"
			#sudo echo $heVolt $DWELL $NUMMEAS $CURRENTSCALE 0 "E_i->$N2OFFSET, $COMMENTS"
		done
		sudo $RBC/excitationfn $FILBIAS $N2OFFSET $ONED $TWOA 0 40 8 1 "range->turnoff, $COMMENTS"
		#sudo ./excitationfn $FILBIAS $N2OFFSET $ONED $TWOA 0 $( -$FILBIAS ) 24 1 "range->full, $COMMENTS"
	done
fi
