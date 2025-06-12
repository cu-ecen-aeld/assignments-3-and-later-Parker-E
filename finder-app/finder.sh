#!/bin/sh
filesdir=$1
searchstr=$2
if [ -z "$filesdir" ] || [ -z "$searchstr" ]; then
	echo "Error: File directory or search string not specified."
	echo "Usage: ./finder.sh {File directory} {Search string}"
	exit 1
fi

if [ ! -d $1 ]; then
	echo  "Error: Directory does not exist."
	exit 1
fi

linecount=0
filescount=0

for i in $(find $1 -type f -exec grep -hc $2 {} \; | grep -v 0); do
	linecount=$((linecount+$i))
	filescount=$((filescount+1))
done

echo "The number of files are ${filescount} and the number of matching lines are ${linecount}"
