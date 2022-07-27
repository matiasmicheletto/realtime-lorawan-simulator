#!/bin/bash

for i in {1..3}
do
    echo "Running clouds 1000x1000 $i"    
    ./clouds_1000x1000.sh & ./clouds_1000x1000.sh & ./clouds_1000x1000.sh & ./clouds_1000x1000.sh    
done
