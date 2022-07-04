#!/bin/bash

mkdir results_100x100

# 100x100
# Soft - Springs
./runnable -m 100 -e 10 -s 7 -i 1000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 100 -e 50 -s 7 -i 1000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 100 -e 100 -s 7 -i 1000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 100 -e 200 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 100 -e 500 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 100 -e 1000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 100 -e 2000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 100 -e 5000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 100 -e 10000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 100 -e 20000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 100 -e 50000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 100 -e 100000 -g 5 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 0
# Soft - Random
./runnable -m 100 -e 10 -s 7 -i 1000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 100 -e 50 -s 7 -i 1000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 100 -e 100 -s 7 -i 1000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 100 -e 200 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 100 -e 500 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 100 -e 1000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 100 -e 2000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 100 -e 5000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 100 -e 10000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 100 -e 20000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 100 -e 50000 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 100 -e 100000 -g 5 -s 7 -i 10000 -t 3600 -x 0 -p 0 -a 1
# Medium - Springs
./runnable -m 100 -e 10 -s 7 -i 1000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 100 -e 50 -s 7 -i 1000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 100 -e 100 -s 7 -i 1000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 100 -e 200 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 100 -e 500 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 100 -e 1000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 100 -e 2000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 100 -e 5000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 100 -e 10000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 100 -e 20000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 100 -e 50000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 100 -e 100000 -g 10 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 0
# Medium - Random
./runnable -m 100 -e 10 -s 7 -i 1000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 100 -e 50 -s 7 -i 1000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 100 -e 100 -s 7 -i 1000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 100 -e 200 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 100 -e 500 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 100 -e 1000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 100 -e 2000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 100 -e 5000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 100 -e 10000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 100 -e 20000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 100 -e 50000 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 100 -e 100000 -g 10 -s 7 -i 10000 -t 3600 -x 0 -p 1 -a 1
# Hard - Springs
./runnable -m 100 -e 10 -s 7 -i 1000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 100 -e 50 -s 7 -i 1000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 100 -e 100 -s 7 -i 1000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 100 -e 200 -s 7 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 100 -e 500 -s 7 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 100 -e 1000 -s 7 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 100 -e 2000 -s 7 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 100 -e 5000 -s 7 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 100 -e 10000 -s 7 -g 10 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 100 -e 20000 -s 7 -g 10 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 100 -e 50000 -s 7 -g 100 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 100 -e 100000 -s 7 -g 180 -i 10000 -t 3600 -x 0 -p 2 -a 0
# Hard - Random
./runnable -m 100 -e 10 -s 7 -i 1000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 100 -e 50 -s 7 -i 1000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 100 -e 100 -s 7 -i 1000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 100 -e 200 -s 7 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 100 -e 500 -s 7 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 100 -e 1000 -s 7 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 100 -e 2000 -s 7 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 100 -e 5000 -s 7 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 100 -e 10000 -s 7 -g 10 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 100 -e 20000 -s 7 -g 10 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 100 -e 50000 -s 7 -g 100 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 100 -e 100000 -s 7 -g 180 -i 10000 -t 3600 -x 0 -p 2 -a 1

mv output* results_100x100
mv nodes* results_100x100
mv summary.csv summary_100x100.csv
