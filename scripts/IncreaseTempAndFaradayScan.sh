#!/bin/bash
#
#

if [ "$#" -ne 1 ]; then 
	echo "usage: sudo ./IncreaseTempAndFaradayScan.sh <additional comments>" 
else
    RBC="/home/pi/RbControl"
	FILBIAS="130.0"
	N2OFFSET="100.1"
	N2SWEEP="2.5"
	HEOFFSET=-10
	CURRENTSCALE=6
	SCANRANGE=30
	STEPSIZE=24
	DWELL=3
	COMMENTS=$1
	STARTTEMP=120
	ENDTEMP=125
	STEPTEMP=5
	FINALTEMPCCELL=150
	FINALTEMPRES=0
	AOUT1=240
	AOUT2=480
	AOUT3=792
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

	for temp in $(seq $STARTTEMP $STEPTEMP $ENDTEMP); do 
		sudo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, temp=$temp, prelude" 
		#echo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, temp=$temp, prelude" 

		echo "Blocking pump beam..."
		sudo $RBC/setLaserFlag $PUMP $BLOCKED

		sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, temp=$temp,  prelude"
		#echo '$RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, temp=$temp,  prelude"'

		NUMRUNS=3
		for i in $(seq 1 $NUMRUNS); do 
			echo "About to start polarization run $i of $NUMRUNS. Pausing for 5 seconds to give the opportunity to cancel the run."
			sleep 5
			sudo $RBC/scripts/ElectronPolarizationScript.sh $AOUT1 2 $CURRENTSCALE 1.5 "temp=$temp, Run $i, AOUT=$AOUT1, $COMMENTS"
			sudo $RBC/scripts/ElectronPolarizationScript.sh $AOUT2 3 $CURRENTSCALE 1.5 "temp=$temp, Run $i, AOUT=$AOUT2, $COMMENTS"
			sudo $RBC/scripts/ElectronPolarizationScript.sh $AOUT3 2 $CURRENTSCALE 1.5 "temp=$temp, Run $i, AOUT=$AOUT3, $COMMENTS"
			#echo '$RBC/scripts/ElectronPolarizationScript.sh 216 3 $CURRENTSCALE "Run $i, AOUT=216, $COMMENTS"'
			#echo '$RBC/scripts/ElectronPolarizationScript.sh 504 6 $CURRENTSCALE "Run $i, AOUT=504, $COMMENTS"'
			#echo '$RBC/scripts/ElectronPolarizationScript.sh 800 3 $CURRENTSCALE "Run $i, AOUT=800, $COMMENTS"'

			sudo $RBC/scripts/RbQuickPolarizationScript.sh "$COMMENTS, temp=$temp, Run=$i, quick check on polarization"
			sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, temp=$temp, Run=$i"
		# EXACT REPEAT DONE
		done

		sudo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, temp=$temp, postscript" 

		echo "Blocking pump beam..."
		sudo $RBC/setLaserFlag $PUMP $BLOCKED

		sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, temp=$temp,  postscript"

		echo "About to change temperature. Giving 5 seconds to give the oppotunity to cancel the run."
		sleep 5
		echo "Increasing temp by 5 degrees"
		sudo $RBC/setOmega $((temp + 30)) $((temp))
		#echo "$RBC/setOmega $((temp + 30)) $((temp))"

		date
		echo "Giving 1 hour to equillibrate"
		for i in $(seq 1 30); do
			sudo $RBC/scripts/RbQuickPolarizationScript.sh "$COMMENTS, auto run while reservoir warms."
			#echo '$RBC/scripts/RbQuickPolarizationScript.sh "$COMMENTS, auto run while reservoir warms."'
		done
	# TEMP LOOP DONE
	done 

	#LAST RUN
	temp=$ENDTEMP
	sudo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, temp=$temp, prelude" 
	#echo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, temp=$temp, prelude" 

	echo "Blocking pump beam..."
	sudo $RBC/setLaserFlag $PUMP $BLOCKED

	sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, temp=$temp,  prelude"
	#echo '$RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, temp=$temp,  prelude"'

	NUMRUNS=5
	for i in $(seq 1 $NUMRUNS); do 
		echo "About to start polarization run $i of $NUMRUNS. Pausing for 5 seconds to give the opportunity to cancel the run."
		sleep 5
		sudo $RBC/scripts/ElectronPolarizationScript.sh $AOUT1 2 $CURRENTSCALE 1.5 "temp=$temp, Run $i, AOUT=$AOUT1, $COMMENTS"
		sudo $RBC/scripts/ElectronPolarizationScript.sh $AOUT2 3 $CURRENTSCALE 1.5 "temp=$temp, Run $i, AOUT=$AOUT2, $COMMENTS"
		sudo $RBC/scripts/ElectronPolarizationScript.sh $AOUT3 2 $CURRENTSCALE 1.5 "temp=$temp, Run $i, AOUT=$AOUT3, $COMMENTS"
		#echo '$RBC/scripts/ElectronPolarizationScript.sh 216 3 $CURRENTSCALE "Run $i, AOUT=216, $COMMENTS"'
		#echo '$RBC/scripts/ElectronPolarizationScript.sh 504 6 $CURRENTSCALE "Run $i, AOUT=504, $COMMENTS"'
		#echo '$RBC/scripts/ElectronPolarizationScript.sh 800 3 $CURRENTSCALE "Run $i, AOUT=800, $COMMENTS"'

		sudo $RBC/scripts/RbQuickPolarizationScript.sh "$COMMENTS, temp=$temp, Run=$i, quick check on polarization"
		sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, temp=$temp, Run=$i"
	# EXACT REPEAT DONE
	done

	sudo $RBC/scripts/RbPolarizationScript.sh "$COMMENTS, temp=$temp, postscript" 

	echo "Blocking pump beam..."
	sudo $RBC/setLaserFlag $PUMP $BLOCKED

	sudo $RBC/excitationfn $FILBIAS "$N2OFFSET" "$N2SWEEP" $HEOFFSET $SCANRANGE $STEPSIZE $DWELL $CURRENTSCALE "$COMMENTS, temp=$temp,  postscript"

	sudo $RBC/setOmega $FINALTEMPCCELL $FINALTEMPRES
	#echo '$RBC/setOmega $FINALTEMPCCELL $FINALTEMPRES'

fi
