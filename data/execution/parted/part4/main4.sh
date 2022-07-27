#!/bin/bash

for i in {1..3}
do
    echo "Running clouds 100x100 $i"
    ./clouds_100x100.sh & ./clouds_100x100.sh & ./clouds_100x100.sh & ./clouds_100x100.sh
done
