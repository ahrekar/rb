#!/bin/bash

for j in $(seq 1 4); do 
    for i in 7 25 35; do 
        for k in $(seq 1 5);do
            $RBC/faradayRotation 0 0 0 "$j" "$i" "Testing how accurate the angle measured with given dataset size. Rev: $j, StepSize: $i, Run $k"
        done
    done
done
