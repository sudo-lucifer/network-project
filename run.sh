#!/bin/bash
# echo $2
# echo $3
# echo $4

if [ "$1" == "server" ];
then
    if [ "$4" != "" ]; then
        ./icws --port 28581 --root $2 --numThreads $3 --timeout $4
    elif [ "$3" != "" ]; then
        ./icws --port 28581 --root $2 --numThreads $3 --timeout 1000
    elif [ "$2" != "" ]; then
        ./icws --port 28581 --root $2 --numThreads 10 --timeout 1000 
    fi

elif [ "$1" == "telnet" ];
then
    telnet localhost 28581
elif [ "$1" == "netcat" ];
then 
    netcat localhost 28581 < $2
fi

