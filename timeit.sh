#!/bin/bash
## Runs n tests on all multisets, stroing the output in
## performance.out as:
## PROGRAM DATA-FILE-A DATA-FILE-B TIME(Nanosec.) ALG-OUTPUT(Yes/No)
## Usage:	bash -f timeit.sh n

RUNS=$1
PRG="sortAndDiff.sh"

for i in data1a,data1b data2a,data2b data3a,data3b data4a,data4b ; do IFS=","
	set $i
	DATA_A=$1
	DATA_B=$2
	N=$RUNS
	while [ $(( N -= 1 )) -ge 0 ]; do
		# Get current time in nano seconds
		T="$(date +%s%N)"
		# Run the current program on the current files
		RES=$(./"$PRG" "$DATA_A" "$DATA_B")
		# Stop! Hammer time!
		T="$(($(date +%s%N)-T))"
	
		echo "$PRG $DATA_A $DATA_B $T $RES" >> "performance.out"
	done
	echo $i
done
