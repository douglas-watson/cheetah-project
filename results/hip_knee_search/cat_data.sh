#!/bin/bash
# Concatenate all the different data files into one large one

# usage: cat_data.sh input_folder output_file
# concatenates all the .txt files.

# get the header line from one of the files
file=`ls $1 | head -n 1`
head -n 1 $1/$file >> $2
for file in `ls $1`; do
	tail --lines +2 $1/$file >> $2
done
