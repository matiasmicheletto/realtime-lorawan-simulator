#!/bin/bash

re='^[0-3]$'
if [[ $# -eq 1 && $1 =~ $re ]]; then # Check arguments
    echo 'Running optimization...'
    ./new_runnable -m 100 -e 100 -i 10000 -t 5400 -p 1 -a $1
    ./new_runnable -m 250 -e 625 -i 10000 -t 5400 -p 1 -a $1
    ./new_runnable -m 500 -e 2500 -i 10000 -t 5400 -p 1 -a $1
    ./new_runnable -m 750 -e 5625 -i 10000 -t 5400 -p 1 -a $1
    ./new_runnable -m 1000 -e 10000 -i 10000 -t 5400 -p 1 -a $1
    ./new_runnable -m 1500 -e 22500 -i 10000 -t 5400 -p 1 -a $1
    ./new_runnable -m 2000 -e 40000 -i 10000 -t 5400 -p 1 -a $1
else
    echo 'Argument must be 0, 1, 2 or 3'
fi
