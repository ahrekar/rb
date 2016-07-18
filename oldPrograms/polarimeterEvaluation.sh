#!/bin/bash

if [ "$#" -ne 0 ]; then
	echo "usage: sudo ./polarimeterEvaluation.sh"
else
	sudo ./polarization 0 "Max transmission (103 steps)"
	sudo ./stepmotor 1 22 1
	sudo ./polarization 0 "Intermediate transmission (125)"
	sudo ./stepmotor 1 22 1
	sudo ./polarization 0 "Intermediate transmission (147)"
	sudo ./stepmotor 1 23 1
	sudo ./polarization 0 "Intermediate transmission (170)"
	sudo ./stepmotor 1 23 1
	sudo ./polarization 0 "Min transmission (193)"
	sudo ./stepmotor 1 90 0
fi
