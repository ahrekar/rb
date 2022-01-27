#/bin/bash

# I found that I was clipping some of the pump beam with these values
# It might have been because I wanted the pump and probe to 
# have the same axis so that the optical isolator would work best,
# but I checked and the new position of the LP for the pump beam
# doesn't have any visible transmission on the sensitive IR card.
#PIPOS=125
#SPLUSPOS=170
#SMINUSPOS=82

# 2021-07-01: the new values for LP @ 144
#PIPOS=115
#SPLUSPOS=164
#SMINUSPOS=72

# 2022-01-25: Something happened. I don't understand. These appear to be the new values.
PIPOS=58
SPLUSPOS=15
SMINUSPOS=103

PUMP=1
PROBE=0

BLOCKED=1
UNBLOCKED=0

RBC=/home/pi/RbControl
