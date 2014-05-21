#!/bin/bash
## Runs n tests on all multisets, stroing the output in
## performance.out as:
## PROGRAM DATA-FILE-A DATA-FILE-B SEED TIME(Nanosec.) ALG-OUTPUT(Yes/No)
## Usage:	bash -f timeit.sh program nRuns


PRG=$1
RUNS=$2

for i in data1a,data1b data2a,data2b data3a,data3b data4a,data4b data5a,data5b data6a,data6b data7a,data7b; do IFS=","
	set $i
	DATA_A=$1
	DATA_B=$2
	
	while read SEED; do
		N=$RUNS
		while [ $(( N -= 1 )) -ge 0 ]; do
			# Get current time in nano seconds
			T="$(date +%s%N)"
			# Run the current program on the current files
			RES=$(./"$PRG" "$DATA_A" "$DATA_B" "$SEED")
			# Stop! Hammer time!
			T="$(($(date +%s%N)-T))"
			echo "$PRG $DATA_A $DATA_B $SEED $T $RES" >> "performance.out"
		done
	done < "www.random.org"
	
done
