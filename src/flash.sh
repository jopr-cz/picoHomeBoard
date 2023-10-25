#!/bin/bash

FILE="${1}"
PORT="/dev/ttyACM0"
export PICO_SDK_PATH=/home/pou/pico/pico-sdk

echo "Starting flashing script"



if [ -z "${FILE}" ]; then
	echo "Specifi file to upload"
	exit
fi

if [ ! -e "${FILE}" ]; then
	echo "File ${FILE} not found"
	exit
fi

if [[ ! ${FILE} == *.uf2 ]]; then
	echo "File must by uf2!"
	exit
fi



echo "Uploading file ${FILE}"


if [ -e "${PORT}" ]; then

	stty -F ${PORT}  3015:10:ebd:800:3:1c:7f:15:1:5:1:0:11:13:1a:0:12:f:17:16:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0
	stty 115200 -F ${PORT}
	
	echo "Sending rebot cmd..."
	echo "r" >  ${PORT}
	sleep 2
else 
	echo "Serial port ${PORT} not found - maybe pico is in bootsel mode"
fi


sleep 1
echo "Flesh"
./picotool info

./picotool  load -x "${FILE}"

echo "DONE"
