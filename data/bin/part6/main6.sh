#!/bin/bash

for i in {1..3}
do
    echo "Running clouds 2000x2000 $i"
    ./clouds_2000x2000.sh & ./clouds_2000x2000.sh & ./clouds_2000x2000.sh & ./clouds_2000x2000.sh
done
