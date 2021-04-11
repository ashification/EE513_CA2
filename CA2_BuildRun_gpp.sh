#!/bin/bash

if [ "$1" == "" ]; then
    printf  "ERROR: First Arg is empty, Please declare script name"
else  
	printf  "Building Script Please wait...\n"
	g++ $1 -o ca1_script_$2 -lwiringPi 
	printf  "Running Script now....\n\n\n"
	./ca1_script_$2
	printf "\n\n"
	printf "Script End\n"
fi

