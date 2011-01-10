#!/bin/bash

# Run webots ten times with different values of kneePhaseLag

PREF_FILE='pref_file'
WORLD='/home/dwatson/myfiles/sensory-motor-modelling/CheetahGait/worlds/Amarsi-Quadrupedal.wbt'

for offset in 0.25 0.30 0.35 0.40 0.45 0.50 0.55 0.60
do
	echo $offset > $PREF_FILE
	webots --minimize --mode=run $WORLD
done
