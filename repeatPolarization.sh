#!/bin/bash

AOUT=(100 400 100 400 100 400 100 400)
for i in ${AOUT[@]}; do

sudo ./polarization 2 20 1 ${i} 0 "$1"



done
