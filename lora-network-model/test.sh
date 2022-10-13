rm summary.csv

./runnable -m 1000 -e 10000 -x 0 -p 0 -s 12 -g 1 -i 5000 -t 1000
./runnable -m 1000 -e 10000 -x 0 -p 1 -s 12 -g 1 -i 5000 -t 1000
./runnable -m 1000 -e 10000 -x 0 -p 2 -s 9 -g 85 -i 5000 -t 1000

./runnable -m 1000 -e 50000 -x 0 -p 0 -s 12 -g 5 -i 5000 -t 1000
./runnable -m 1000 -e 50000 -x 0 -p 2 -s 7 -g 100 -i 5000 -t 1000

./runnable -m 1000 -e 10000 -x 2 -p 0 -s 12 -g 1 -i 5000 -t 1000
./runnable -m 1000 -e 10000 -x 2 -p 2 -s 9 -g 30 -i 5000 -t 1000

./runnable -m 1000 -e 50000 -x 2 -p 0 -s 12 -g 5 -i 5000 -t 1000
./runnable -m 1000 -e 50000 -x 2 -p 2 -s 7 -g 100 -i 5000 -t 1000