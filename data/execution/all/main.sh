#!/bin/bash

for i in {1..5}
do
    echo "Running uniform $i"
    ./uniform_100x100.sh & ./uniform_100x100.sh & ./uniform_100x100.sh & ./uniform_100x100.sh
    ./uniform_1000x1000.sh & ./uniform_1000x1000.sh & ./uniform_1000x1000.sh & ./uniform_1000x1000.sh
    ./uniform_2000x2000.sh & ./uniform_2000x2000.sh & ./uniform_2000x2000.sh & ./uniform_2000x2000.sh
done

for i in {1..5}
do
    echo "Running clouds $i"
    ./clouds_100x100.sh & ./clouds_100x100.sh & ./clouds_100x100.sh & ./clouds_100x100.sh
    ./clouds_1000x1000.sh & ./clouds_1000x1000.sh & ./clouds_1000x1000.sh & ./clouds_1000x1000.sh
    ./clouds_2000x2000.sh & ./clouds_2000x2000.sh & ./clouds_2000x2000.sh & ./clouds_2000x2000.sh
done
