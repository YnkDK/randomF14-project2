#!/bin/bash
## Sorts both files given as argument and then compares them
## Usage:	bash -f sortAndDiff.sh data-file-a data-file-b

DIFF=$(diff <(sort "$1") <(sort "$2"))

if [ "$DIFF" == "" ]; then
	echo "Yes"
	exit
fi

echo "No"
