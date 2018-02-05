#!/bin/bash
#
#

if [ "$#" -ne 3 ]; then
	echo "usage: sudo ./repeatPolarization <number of runs> <ammeter Magnitude> <comments_in_double_quotes>"
else
	NUMRUNS=$1
	AMMETERMAG=$2  #Ammeter Magnitude
	COMMENTS=$3
	AOUTLOW=0
	AOUTHIGH=3
	AOUTSTEP=30
	DWELL=5
	LEAKCURRENT=0
	
	for i in $(seq 1 $NUMRUNS); do 
		for AOUT in $(seq $AOUTLOW $AOUTHIGH); do
			sudo ./polarization "$AOUT" $DWELL $AMMETERMAG $LEAKCURRENT "Run $i, $COMMENTS"
		done
		sudo ./excitationfn 130 70 0.6 20.0 30 24 3 $AMMETERMAG "Run $i, $COMMENTS"
	done
fi
