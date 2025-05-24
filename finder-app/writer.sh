#!/bin/bash
path=$1
if  [ -z $1 ] || [ -z $2 ]; then
	echo "Error: File or string not specified."
	echo "Usage: ./writer.sh {Filepath and filename} {String to write}"
	exit 1
fi

if [ ! -d ${path%/*} ]; then
	mkdir -p ${path%/*}
fi

if touch $1; then
	echo $2 > $1
else
	echo "Error: Issue creating file"
	exit 1
fi


