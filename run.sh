#!/bin/bash

if [ "$1" == "server" ];
then
    ./icws --port 12345 --root $2
elif [ "$1" == "telnet" ];
then
    telnet localhost 12345
elif [ "$1" == "netcat" ];
then 
    netcat localhost 12345 < $2
fi

