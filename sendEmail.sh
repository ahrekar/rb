#!/bin/bash

# This script finds the most recently edited file and sends it in an e-mail to the specified recipients. 
RECIPIENTS="karl@huskers.unl.edu"
#echo "data" | sudo mutt -s "data" -a  'ls -tl | head -n 2 | grep -oE '[^ ]+$' | tail -n 1' -- mangelohumm@gmail.com karl@huskers.unl.edu
echo "data" | sudo mutt -s "data" -a  `ls -tl | head -n 2 | grep -oE '[^ ]+$' | tail -n 1` -- $RECIPIENTS
