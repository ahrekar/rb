#!/bin/bash
#
#

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./PolarizationScript.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS=130.0
	N2OFFSET=2.0
	N2SWEEP=2.5
	HEOFFSET=80.0
	CURRENTSCALE=7
	SCANRANGE=30
	STEPSIZE=24
	DWELL=3
	COMMENTS=$1
	# TEMPS FOR TESTING
	#STARTTEMP=100
	#ENDTEMP=105
	#STEPTEMP=1

    PUMP=1
    PROBE=0

    BLOCKED=1
    UNBLOCKED=0

	#temp=100
	#j=1
	#i=1
	#AOUT=0


	date
	echo "Giving 1 hour to equillibrate"
	for i in $(seq 1 30); do
		sudo $RBC/scripts/RbQuickPolarizationScript.sh "$COMMENTS, auto run while equilibrates."
		#echo '$RBC/scripts/RbQuickPolarizationScript.sh "$COMMENTS, auto run while reservoir warms."'
	done

	sudo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, prelude" 
	#echo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, prelude" 

	echo "Blocking pump beam..."
	sudo $RBC/setLaserFlag $PUMP $BLOCKED

	sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, temp=$temp,  prelude"
	#echo '$RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, temp=$temp,  prelude"'

	NUMRUN=5
	for i in $(seq 1 $NUMRUN); do 
		echo "About to start next set of polarization runs. Pausing for 5 seconds to give the opportunity to cancel the run."
		sleep 5
		sudo $RBC/scripts/ElectronPolarizationScript.sh 144 3 $CURRENTSCALE "Run $i/$NUMRUN, AOUT=216, $COMMENTS"
		echo "About to start next AOUT value. Pausing for 5 seconds to give the opportunity to cancel the run."
		sleep 5
		sudo $RBC/scripts/ElectronPolarizationScript.sh 600 6 $CURRENTSCALE "Run $i/$NUMRUN, AOUT=504, $COMMENTS"
		echo "About to start next AOUT value. Pausing for 5 seconds to give the opportunity to cancel the run."
		sleep 5
		sudo $RBC/scripts/ElectronPolarizationScript.sh 792 3 $CURRENTSCALE "Run $i/$NUMRUN, AOUT=800, $COMMENTS"
	# EXACT REPEAT DONE
	done

	sudo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, temp=$temp, postscript" 

	echo "Blocking pump beam..."
	sudo $RBC/setLaserFlag $PUMP $BLOCKED

	sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, temp=$temp,  postscript"

fi
