#!/bin/bash

mkdir results_2000x2000

# 2000x2000
# Soft - Springs
./runnable -m 2000 -e 100 -i 1000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 2000 -e 200 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 2000 -e 500 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 2000 -e 1000 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 2000 -e 2000 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 2000 -e 5000 -g 5 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 2000 -e 10000 -g 5 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 2000 -e 20000 -g 10 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 2000 -e 50000 -g 20 -i 10000 -t 3600 -x 0 -p 0 -a 0
./runnable -m 2000 -e 100000 -g 80 -i 10000 -t 3600 -x 0 -p 0 -a 0
# Soft - Random
./runnable -m 2000 -e 100 -i 1000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 2000 -e 200 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 2000 -e 500 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 2000 -e 1000 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 2000 -e 2000 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 2000 -e 5000 -g 5 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 2000 -e 10000 -g 5 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 2000 -e 20000 -g 10 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 2000 -e 50000 -g 20 -i 10000 -t 3600 -x 0 -p 0 -a 1
./runnable -m 2000 -e 100000 -g 80 -i 10000 -t 3600 -x 0 -p 0 -a 1
# Medium - Springs
./runnable -m 2000 -e 100 -i 1000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 2000 -e 200 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 2000 -e 500 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 2000 -e 1000 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 2000 -e 2000 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 2000 -e 5000 -g 5 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 2000 -e 10000 -g 10 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 2000 -e 20000 -g 15 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 2000 -e 50000 -g 30 -i 10000 -t 3600 -x 0 -p 1 -a 0
./runnable -m 2000 -e 100000 -g 65 -i 10000 -t 3600 -x 0 -p 1 -a 0
# Medium - Random
./runnable -m 2000 -e 100 -i 1000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 2000 -e 200 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 2000 -e 500 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 2000 -e 1000 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 2000 -e 2000 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 2000 -e 5000 -g 5 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 2000 -e 10000 -g 10 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 2000 -e 20000 -g 15 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 2000 -e 50000 -g 30 -i 10000 -t 3600 -x 0 -p 1 -a 1
./runnable -m 2000 -e 100000 -g 65 -i 10000 -t 3600 -x 0 -p 1 -a 1
# Hard - Springs
./runnable -m 2000 -e 100 -g 50 -i 1000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 2000 -e 200 -g 80 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 2000 -e 500 -g 150 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 2000 -e 1000 -g 150 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 2000 -e 2000 -g 100 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 2000 -e 5000 -g 100 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 2000 -e 10000 -g 150 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 2000 -e 20000 -g 200 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 2000 -e 50000 -g 300 -i 10000 -t 3600 -x 0 -p 2 -a 0
./runnable -m 2000 -e 100000 -g 500 -i 10000 -t 3600 -x 0 -p 2 -a 0
# Hard - Random
./runnable -m 2000 -e 100 -g 50 -i 1000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 2000 -e 200 -g 80 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 2000 -e 500 -g 150 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 2000 -e 1000 -g 150 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 2000 -e 2000 -g 100 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 2000 -e 5000 -g 100 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 2000 -e 10000 -g 150 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 2000 -e 20000 -g 200 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 2000 -e 50000 -g 300 -i 10000 -t 3600 -x 0 -p 2 -a 1
./runnable -m 2000 -e 100000 -g 500 -i 10000 -t 3600 -x 0 -p 2 -a 1

mv output* results_2000x2000
mv nodes* results_2000x2000
mv summary.csv summary_2000x2000.csv
